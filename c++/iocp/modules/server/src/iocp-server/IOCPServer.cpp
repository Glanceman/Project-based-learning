#include "IOCPServer.h"
#include "tinyLogger.h"
#include <WS2tcpip.h>
#include <signal.h>
#include <iostream>

#ifdef _MSC_VER
#pragma comment(lib, "ws2_32.lib")
#endif

IOCPServer* IOCPServer::s_instance = nullptr;

IOCPServer::IOCPServer(int port, int workerThreads) :
    m_serverSocket(INVALID_SOCKET), m_iocpHandle(nullptr), m_port(port), m_workerThreadCount(workerThreads), m_logger(std::make_unique<TinyLogger>("server-log"))
{
    s_instance = this;
    signal(SIGINT, s_signalHandler);
    signal(SIGTERM, s_signalHandler);
}

IOCPServer::~IOCPServer()
{
    Shutdown();
    s_instance = nullptr;
}

void IOCPServer::s_signalHandler(int signum)
{
    std::cout << "Caught signal " << signum << ", exiting gracefully." << std::endl;
    g_running = false;

    if (s_instance && s_instance->m_serverSocket != INVALID_SOCKET)
    {
        closesocket(s_instance->m_serverSocket);
        s_instance->m_serverSocket = INVALID_SOCKET;
    }
}

bool IOCPServer::Initialize()
{
    m_logger->log(TinyLogger::LogLevel::INFO, "Starting server initialization...");

    if (!SetupWinsock())
    {
        return false;
    }

    if (!CreateServerSocket())
    {
        WSACleanup();
        return false;
    }

    if (!SetupIOCP())
    {
        closesocket(m_serverSocket);
        WSACleanup();
        return false;
    }

    m_logger->log(TinyLogger::LogLevel::INFO, "Server initialization complete");
    return true;
}

bool IOCPServer::SetupWinsock()
{
    WSAData wsaData{};
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
    {
        m_logger->log(TinyLogger::LogLevel::ERR, "WSAStartup failed");
        return false;
    }
    return true;
}

bool IOCPServer::CreateServerSocket()
{
    m_serverSocket = WSASocket(AF_INET, SOCK_STREAM, IPPROTO_TCP, nullptr, 0, WSA_FLAG_OVERLAPPED);
    if (m_serverSocket == INVALID_SOCKET)
    {
        m_logger->log(TinyLogger::LogLevel::ERR, "WSASocket failed: {}", WSAGetLastError());
        return false;
    }

    // Enable SO_REUSEADDR
    BOOL reuseAddr = TRUE;
    setsockopt(m_serverSocket, SOL_SOCKET, SO_REUSEADDR, reinterpret_cast<char*>(&reuseAddr), sizeof(reuseAddr));

    // Bind socket
    sockaddr_in serverAddr{};
    serverAddr.sin_family      = AF_INET;
    serverAddr.sin_port        = htons(static_cast<u_short>(m_port));
    serverAddr.sin_addr.s_addr = htonl(INADDR_ANY);

    if (bind(m_serverSocket, reinterpret_cast<sockaddr*>(&serverAddr), sizeof(serverAddr)) == SOCKET_ERROR)
    {
        m_logger->log(TinyLogger::LogLevel::ERR, "bind failed: {}", WSAGetLastError());
        return false;
    }

    if (listen(m_serverSocket, LISTEN_BACKLOG) == SOCKET_ERROR)
    {
        m_logger->log(TinyLogger::LogLevel::ERR, "listen failed: {}", WSAGetLastError());
        return false;
    }

    return true;
}

bool IOCPServer::SetupIOCP()
{
    SYSTEM_INFO sysInfo;
    GetSystemInfo(&sysInfo);

    m_iocpHandle = CreateIoCompletionPort(INVALID_HANDLE_VALUE, nullptr, 0, sysInfo.dwNumberOfProcessors);
    if (m_iocpHandle == nullptr)
    {
        m_logger->log(TinyLogger::LogLevel::ERR, "CreateIoCompletionPort failed: {}", GetLastError());
        return false;
    }

    return true;
}

void IOCPServer::Run()
{
    // Create worker threads
    for (int i = 0; i < m_workerThreadCount; ++i)
    {
        m_workerThreads.emplace_back([this](HANDLE handle) { ProcessIO(handle); }, m_iocpHandle);
    }

    m_logger->log(TinyLogger::LogLevel::INFO, "Server running on port {} with {} worker threads", m_port, m_workerThreadCount);

    // Accept connection loop
    while (g_running)
    {
        sockaddr_in clientAddr{};
        int         addrLen      = sizeof(clientAddr);
        SOCKET      clientSocket = accept(m_serverSocket, reinterpret_cast<sockaddr*>(&clientAddr), &addrLen);

        if (clientSocket == INVALID_SOCKET)
        {
            if (g_running)
            {
                DWORD error = WSAGetLastError();
                if (error != WSAEINTR && error != WSAENOTSOCK)
                {
                    m_logger->log(TinyLogger::LogLevel::ERR, "accept failed: {}", error);
                }
            }
            continue;
        }

        // Add client to manager
        m_clientManager.AddClient(clientSocket);

        // Associate client socket with IOCP
        HANDLE clientHandle = CreateIoCompletionPort(reinterpret_cast<HANDLE>(clientSocket), m_iocpHandle, clientSocket, 0);
        if (clientHandle == nullptr)
        {
            m_logger->log(TinyLogger::LogLevel::ERR, "CreateIoCompletionPort for client failed: {}", GetLastError());
            m_clientManager.RemoveClient(clientSocket);
            closesocket(clientSocket);
            continue;
        }

        // Post initial receive operation
        if (!PostReceiveOperation(clientSocket))
        {
            m_clientManager.RemoveClient(clientSocket);
            closesocket(clientSocket);
            continue;
        }

        // Log new connection
        char clientIP[INET_ADDRSTRLEN];
        inet_ntop(AF_INET, &clientAddr.sin_addr, clientIP, INET_ADDRSTRLEN);
        m_logger->log(TinyLogger::LogLevel::INFO, "New connection from {}:{}", clientIP, ntohs(clientAddr.sin_port));
    }
}

bool IOCPServer::PostReceiveOperation(SOCKET clientSocket)
{
    auto perIoData      = std::make_unique<PerIOOperationData>();
    perIoData->m_socket = clientSocket;
    perIoData->m_type   = IOType::Read;

    DWORD bytesRecv = 0;
    DWORD flags     = 0;

    int result = WSARecv(
        clientSocket,
        &perIoData->m_wsaBuf,
        1,
        &bytesRecv,
        &flags,
        &perIoData->m_overlapped,
        nullptr);

    if (result == SOCKET_ERROR && WSAGetLastError() != WSA_IO_PENDING)
    {
        m_logger->log(TinyLogger::LogLevel::ERR, "WSARecv failed: {}", WSAGetLastError());
        return false;
    }

    // Release ownership - IOCP will handle cleanup
    perIoData.release();
    return true;
}

void IOCPServer::ProcessIO(HANDLE iocpHandle)
{
    DWORD               bytesTransferred;
    ULONG_PTR           completionKey = 0;
    PerIOOperationData* perIoData     = nullptr;

    while (g_running)
    {
        BOOL result = GetQueuedCompletionStatus(
            iocpHandle,
            &bytesTransferred,
            &completionKey,
            reinterpret_cast<LPOVERLAPPED*>(&perIoData),
            1000); // 1 second timeout

        if (!result)
        {
            DWORD error = GetLastError();
            if (error == WAIT_TIMEOUT)
            {
                continue;
            }
            else if (error == ERROR_NETNAME_DELETED || error == ERROR_CONNECTION_ABORTED)
            {
                if (perIoData != nullptr)
                {
                    HandleClientDisconnection(perIoData->m_socket, "Network error");
                    delete perIoData;
                }
                continue;
            }
            else
            {
                m_logger->log(TinyLogger::LogLevel::ERR, "GetQueuedCompletionStatus failed: {}", error);
                if (perIoData != nullptr)
                {
                    HandleClientDisconnection(perIoData->m_socket, "IOCP error");
                    delete perIoData;
                }
                continue;
            }
        }

        if (perIoData == nullptr)
        {
            continue;
        }

        if (bytesTransferred == 0)
        {
            HandleClientDisconnection(perIoData->m_socket, "Graceful disconnect");
            delete perIoData;
            continue;
        }

        // Handle received data
        HandleReceivedData(perIoData, bytesTransferred);

        // Post another receive operation
        if (!PostReceiveOperation(perIoData->m_socket))
        {
            HandleClientDisconnection(perIoData->m_socket, "Failed to post receive");
        }

        delete perIoData;
    }
}

void IOCPServer::HandleReceivedData(PerIOOperationData* perIoData, DWORD bytesTransferred)
{
    ClientState* clientState = m_clientManager.GetClientState(perIoData->m_socket);
    if (clientState == nullptr)
    {
        m_logger->log(TinyLogger::LogLevel::ERR, "Client state not found for socket {}", perIoData->m_socket);
        HandleClientDisconnection(perIoData->m_socket, "No client state");
        return;
    }

    // Process received data
    auto completePackets = clientState->ProcessData(
        reinterpret_cast<const uint8_t*>(perIoData->m_buffer),
        bytesTransferred);

    // Handle complete packets
    if (!completePackets.empty())
    {
        ProcessCompletePackets(perIoData->m_socket, completePackets);
    }
}

void IOCPServer::HandleClientDisconnection(SOCKET clientSocket, const std::string& reason)
{
    m_logger->log(TinyLogger::LogLevel::INFO, "Client {} disconnected: {}", clientSocket, reason);
    m_clientManager.RemoveClient(clientSocket);
    closesocket(clientSocket);
}

void IOCPServer::ProcessCompletePackets(SOCKET clientSocket, const std::vector<std::vector<uint8_t>>& packets)
{
    for (const auto& packet : packets)
    {
        OnPacketReceived(clientSocket, packet);
    }
}

void IOCPServer::OnPacketReceived(SOCKET clientSocket, const std::vector<uint8_t>& packet)
{
    // Default implementation - log the packet
    if (packet.size() >= PACKET_HEADER_SIZE)
    {
        PacketHeader header;
        memcpy(&header, packet.data(), PACKET_HEADER_SIZE);

        m_logger->log(TinyLogger::LogLevel::INFO,
                      "[{}] Client {} sent packet: header_size={}, content_size={}, total_size={}",
                      GetCurrentThreadId(),
                      clientSocket,
                      PACKET_HEADER_SIZE,
                      header.m_contentSize,
                      packet.size());

        // You can process the content here if needed
        if (header.m_contentSize > 0 && packet.size() > PACKET_HEADER_SIZE)
        {
            // const uint8_t* content = packet.data() + PACKET_HEADER_SIZE;
            // Process content...
        }
    }
}

void IOCPServer::Shutdown()
{
    m_logger->log(TinyLogger::LogLevel::INFO, "Server shutting down...");

    // Wait for worker threads to finish
    for (auto& thread : m_workerThreads)
    {
        if (thread.joinable())
        {
            thread.join();
        }
    }

    // Clean up resources
    m_clientManager.Clear();

    if (m_iocpHandle != nullptr)
    {
        CloseHandle(m_iocpHandle);
        m_iocpHandle = nullptr;
    }

    if (m_serverSocket != INVALID_SOCKET)
    {
        closesocket(m_serverSocket);
        m_serverSocket = INVALID_SOCKET;
    }

    WSACleanup();
    m_logger->log(TinyLogger::LogLevel::INFO, "Server shutdown complete");
}