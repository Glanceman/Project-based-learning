#include <cstddef>
#include <iostream>
#include "tinyLogger.h"
#include <psdk_inc/_ip_types.h>
#include <signal.h>
#include <WinSock2.h>
#include <winerror.h>
#ifdef _MSC_VER
#pragma comment(lib, "ws2_32.lib")
#endif
#include <WS2tcpip.h>
#include <vector>
#include <thread>

volatile bool running       = true;
SOCKET        serverSocket  = INVALID_SOCKET;
constexpr int nPort         = 60000;
constexpr int MAX_BUFF_SIZE = 1024;

enum class IOType
{
    Read,
    Write
};

typedef struct PER_IO_OPERATION_DATA
{
    WSAOVERLAPPED Overlapped;
    WSABUF        wsaBuf;
    CHAR          buffer[MAX_BUFF_SIZE];
    IOType        type;
    SOCKET        socket;
    DWORD         nBytes;

    PER_IO_OPERATION_DATA() :
        type(IOType::Read), socket(INVALID_SOCKET), nBytes(0)
    {
        ZeroMemory(&Overlapped, sizeof(WSAOVERLAPPED));
        ZeroMemory(buffer, MAX_BUFF_SIZE);
        wsaBuf.buf = buffer;
        wsaBuf.len = MAX_BUFF_SIZE;
    }
} PER_IO_OPERATION_DATA, *LPPER_IO_OPERATION_DATA;

void SignalHandler(int signum)
{
    std::cout << "Caught signal " << signum << ", exiting gracefully." << std::endl;
    running = false;

    if (serverSocket != INVALID_SOCKET)
    {
        closesocket(serverSocket);
        serverSocket = INVALID_SOCKET;
    }
}

void ProcessIO(TinyLogger& logger, LPVOID lpParam)
{
    HANDLE                  hCompletionPort = (HANDLE)lpParam;
    DWORD                   bytesTransferred;
    ULONG_PTR               lpCompletionKey = 0;
    LPPER_IO_OPERATION_DATA perIoData       = nullptr;

    while (running)
    {
        // Wait for an IO completion event
        BOOL result = GetQueuedCompletionStatus(
            hCompletionPort,
            &bytesTransferred,
            &lpCompletionKey,
            (LPOVERLAPPED*)&perIoData,
            1000 // 1 second timeout instead of INFINITE
        );

        if (!result)
        {
            DWORD error = GetLastError();
            if (error == WAIT_TIMEOUT)
            {
                continue; // Check running flag and continue
            }
            else if (error == ERROR_NETNAME_DELETED || error == ERROR_CONNECTION_ABORTED)
            {
                if (perIoData != nullptr)
                {
                    logger.log(TinyLogger::LogLevel::INFO, "Client {} disconnected (network error)", perIoData->socket);
                    closesocket(perIoData->socket);
                    delete perIoData;
                }
                continue;
            }
            else
            {
                logger.log(TinyLogger::LogLevel::ERR, "GetQueuedCompletionStatus failed: {}", error);
                if (perIoData != nullptr)
                {
                    closesocket(perIoData->socket);
                    delete perIoData;
                }
                continue;
            }
        }

        // Check if we got a valid operation
        if (perIoData == nullptr)
        {
            continue;
        }

        // Client disconnected gracefully (bytesTransferred == 0)
        if (bytesTransferred == 0)
        {
            logger.log(TinyLogger::LogLevel::INFO, "Client {} disconnected gracefully", perIoData->socket);
            closesocket(perIoData->socket);
            delete perIoData;
            continue;
        }

        // Null-terminate the received data to ensure it's a valid string
        if (bytesTransferred < MAX_BUFF_SIZE)
        {
            perIoData->buffer[bytesTransferred] = '\0';
        }
        else
        {
            perIoData->buffer[MAX_BUFF_SIZE - 1] = '\0';
        }

        // Log the received message
        logger.log(TinyLogger::LogLevel::INFO, "[{}] Client {} sent message: '{}'", GetCurrentThreadId(), perIoData->socket, perIoData->buffer);

        // Echo the message back to the client
        LPPER_IO_OPERATION_DATA sendData = new PER_IO_OPERATION_DATA();
        sendData->socket                 = perIoData->socket;
        sendData->type                   = IOType::Write;
        strcpy_s(sendData->buffer, MAX_BUFF_SIZE, perIoData->buffer);
        sendData->wsaBuf.len = bytesTransferred;

        // DWORD bytesSent  = 0;
        // int   sendResult = WSASend(
        //     sendData->socket,
        //     &sendData->wsaBuf,
        //     1,
        //     &bytesSent,
        //     0,
        //     &sendData->Overlapped,
        //     NULL);

        // if (sendResult == SOCKET_ERROR)
        // {
        //     if (WSAGetLastError() != WSA_IO_PENDING)
        //     {
        //         logger.log(TinyLogger::LogLevel::ERR, "WSASend failed: {}", WSAGetLastError());
        //         delete sendData;
        //     }
        // }

        // Continue to receive more data from this client
        LPPER_IO_OPERATION_DATA newRecvData = new PER_IO_OPERATION_DATA();
        newRecvData->socket                 = perIoData->socket;
        newRecvData->type                   = IOType::Read;

        DWORD dwRecv     = 0;
        DWORD flags      = 0;
        int   recvResult = WSARecv(
            newRecvData->socket,
            &newRecvData->wsaBuf,
            1,
            &dwRecv,
            &flags,
            &newRecvData->Overlapped,
            NULL);

        if (recvResult == SOCKET_ERROR)
        {
            if (WSAGetLastError() != WSA_IO_PENDING)
            {
                logger.log(TinyLogger::LogLevel::ERR, "WSARecv failed: {}", WSAGetLastError());
                closesocket(newRecvData->socket);
                delete newRecvData;
            }
        }

        // Clean up the current operation data
        delete perIoData;
    }
}

bool Setup(HANDLE& iocpHandle, TinyLogger& logger)
{
    WSAData data{};
    if (WSAStartup(MAKEWORD(2, 2), &data) != 0)
    {
        logger.log(TinyLogger::LogLevel::ERR, "WSAStartup failed");
        return false;
    }

    // Create socket with overlapped flag
    serverSocket = WSASocket(AF_INET, SOCK_STREAM, IPPROTO_TCP, NULL, 0, WSA_FLAG_OVERLAPPED);
    if (serverSocket == INVALID_SOCKET)
    {
        logger.log(TinyLogger::LogLevel::ERR, "WSASocket failed: {}", WSAGetLastError());
        WSACleanup();
        return false;
    }

    // Enable SO_REUSEADDR
    BOOL reuseAddr = TRUE;
    setsockopt(serverSocket, SOL_SOCKET, SO_REUSEADDR, (char*)&reuseAddr, sizeof(reuseAddr));

    // Bind port
    struct sockaddr_in servAddr;
    servAddr.sin_family      = AF_INET;
    servAddr.sin_port        = htons(nPort);
    servAddr.sin_addr.s_addr = htonl(INADDR_ANY);

    if (bind(serverSocket, (struct sockaddr*)&servAddr, sizeof(servAddr)) == SOCKET_ERROR)
    {
        logger.log(TinyLogger::LogLevel::ERR, "bind failed: {}", WSAGetLastError());
        closesocket(serverSocket);
        WSACleanup();
        return false;
    }

    // Set listen queue to 200
    if (listen(serverSocket, 200) == SOCKET_ERROR)
    {
        logger.log(TinyLogger::LogLevel::ERR, "listen failed: {}", WSAGetLastError());
        closesocket(serverSocket);
        WSACleanup();
        return false;
    }

    // Create IOCP with number of concurrent threads = number of processors
    SYSTEM_INFO sysInfo;
    GetSystemInfo(&sysInfo);
    iocpHandle = CreateIoCompletionPort(INVALID_HANDLE_VALUE, NULL, 0, sysInfo.dwNumberOfProcessors);
    if (iocpHandle == NULL)
    {
        logger.log(TinyLogger::LogLevel::ERR, "CreateIoCompletionPort failed: {}", GetLastError());
        closesocket(serverSocket);
        WSACleanup();
        return false;
    }

    return true;
}

int main()
{
    signal(SIGINT, SignalHandler);
    signal(SIGTERM, SignalHandler);

    TinyLogger logger("server-log");
    logger.log(TinyLogger::LogLevel::INFO, "Starting server setup...");

    HANDLE iocpHandle;
    if (!Setup(iocpHandle, logger))
    {
        logger.log(TinyLogger::LogLevel::ERR, "Setup failed");
        return 1;
    }

    // Create worker threads for IOCP
    SYSTEM_INFO sysInfo;
    GetSystemInfo(&sysInfo);
    //int numThreads = sysInfo.dwNumberOfProcessors * 2; // Common practice: 2x CPU cores
    int numThreads = 4;
    std::vector<std::jthread> processThreadGroup;
    for (int i = 0; i < numThreads; ++i)
    {
        processThreadGroup.emplace_back(ProcessIO, std::ref(logger), iocpHandle);
    }

    logger.log(TinyLogger::LogLevel::INFO, "Server is running on port {} with {} worker threads...", nPort, numThreads);

    while (running)
    {
        sockaddr_in clientAddr;
        int         addrLen      = sizeof(clientAddr);
        SOCKET      clientSocket = accept(serverSocket, (sockaddr*)&clientAddr, &addrLen);

        if (clientSocket == INVALID_SOCKET)
        {
            if (running) // Only log if we're still running (avoid spam during shutdown)
            {
                DWORD error = WSAGetLastError();
                if (error != WSAEINTR && error != WSAENOTSOCK)
                {
                    logger.log(TinyLogger::LogLevel::ERR, "accept failed: {}", error);
                }
            }
            continue;
        }

        // Associate the client socket with the IOCP
        HANDLE clientHandle = CreateIoCompletionPort((HANDLE)clientSocket, iocpHandle, clientSocket, 0);
        if (clientHandle == NULL)
        {
            logger.log(TinyLogger::LogLevel::ERR, "CreateIoCompletionPort for client failed: {}", GetLastError());
            closesocket(clientSocket);
            continue;
        }

        // Initialize the PER_IO_OPERATION_DATA structure for receiving
        LPPER_IO_OPERATION_DATA perIoData = new PER_IO_OPERATION_DATA();
        perIoData->socket                 = clientSocket;
        perIoData->type                   = IOType::Read;

        DWORD bytesRecv = 0;
        DWORD flags     = 0;

        // Post initial receive operation
        int result = WSARecv(
            clientSocket,
            &perIoData->wsaBuf,
            1,
            &bytesRecv,
            &flags,
            &perIoData->Overlapped,
            NULL);

        if (result == SOCKET_ERROR)
        {
            if (WSAGetLastError() != WSA_IO_PENDING)
            {
                logger.log(TinyLogger::LogLevel::ERR, "Initial WSARecv failed: {}", WSAGetLastError());
                delete perIoData;
                closesocket(clientSocket);
                continue;
            }
        }

        char clientIP[INET_ADDRSTRLEN];
        inet_ntop(AF_INET, &clientAddr.sin_addr, clientIP, INET_ADDRSTRLEN);
        logger.log(TinyLogger::LogLevel::INFO, "Accepted new connection from {}:{}", clientIP, ntohs(clientAddr.sin_port));
    }

    logger.log(TinyLogger::LogLevel::INFO, "Server is shutting down...");

    // Wait for worker threads to finish
    for (auto& thread : processThreadGroup)
    {
        if (thread.joinable())
        {
            thread.join();
        }
    }

    CloseHandle(iocpHandle);
    if (serverSocket != INVALID_SOCKET)
    {
        closesocket(serverSocket);
    }
    WSACleanup();

    logger.log(TinyLogger::LogLevel::INFO, "Server shutdown complete.");
    return 0;
}