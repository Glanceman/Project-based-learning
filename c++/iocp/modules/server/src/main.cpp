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

volatile bool running       = true;
SOCKET        serverSocket  = INVALID_SOCKET; // Now a global variable
constexpr int nPort         = 8080;           // Example port number
constexpr int MAX_BUFF_SIZE = 1024;           // Example buffer size

enum class IOType
{
    Read,
    Write
};

typedef struct PER_IO_OPERATION_DATA
{
    WSAOVERLAPPED Overlapped;
    WSABUF        wsaBuf{MAX_BUFF_SIZE, buffer};
    CHAR          buffer[MAX_BUFF_SIZE];
    IOType        type{};
    SOCKET        socket = INVALID_SOCKET;
    DWORD         nBytes = 0;
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
    // This function would handle IOCP events, such as processing completed IO operations.
    HANDLE                  hCompletionPort = (HANDLE)lpParam;
    DWORD                   bytesTransferred;
    void*                   lpCompletionKey = nullptr;
    LPPER_IO_OPERATION_DATA perIoData;

    while (running)
    {
        // Wait for an IO completion event
        BOOL result = GetQueuedCompletionStatus(hCompletionPort, &bytesTransferred, (PULONG_PTR)&lpCompletionKey, (LPOVERLAPPED*)&perIoData, INFINITE);
        if (result == 0)
        {
            DWORD error = GetLastError();
            if (error == WAIT_TIMEOUT || error == ERROR_NETNAME_DELETED)
            {
                closesocket(perIoData->socket);
                delete perIoData;
                continue;
            }
            else
            {
                logger.log(TinyLogger::LogLevel::ERR, "GetQueuedCompletionStatus failed: {}", error);
                continue;
            }
        }

        // client disconnected
        if (bytesTransferred == 0)
        {
            logger.log(TinyLogger::LogLevel::INFO, "Client disconnected, closing socket.");
            closesocket(perIoData->socket);
            delete perIoData;
            continue;
        }

        // 取得数据并处理
        logger.log(TinyLogger::LogLevel::INFO, "{} send message : {}", perIoData->socket, perIoData->buffer);

        // 继续向 socket 投递WSARecv操作
        DWORD Flags  = 0;
        DWORD dwRecv = 0;
        ZeroMemory(perIoData, sizeof(PER_IO_OPERATION_DATA));
        perIoData->wsaBuf.buf = perIoData->buffer;
        perIoData->wsaBuf.len = bytesTransferred;
        WSARecv(perIoData->socket, &perIoData->wsaBuf, 1, &dwRecv, &Flags, &perIoData->Overlapped, NULL);
    }
}

bool Setup(HANDLE& iocpHandle, TinyLogger& logger)
{
    WSAData data{};
    if (WSAStartup(MAKEWORD(2, 2), &data) != 0)
    {
        logger.log(TinyLogger::LogLevel::ERR, "WSAStartup failed");
        WSACleanup();
        return false;
    }

    // create socket
    serverSocket = WSASocket(AF_INET, SOCK_STREAM, 0, NULL, 0, WSA_FLAG_OVERLAPPED);

    // bind port
    struct sockaddr_in servAddr;
    servAddr.sin_family      = AF_INET;
    servAddr.sin_port        = htons(nPort);
    servAddr.sin_addr.s_addr = htonl(INADDR_ANY);

    if (bind(serverSocket, (struct sockaddr*)&servAddr, sizeof(servAddr)) < 0)
    {
        logger.log(TinyLogger::LogLevel::ERR, "bind Failed!");
        closesocket(serverSocket);
        WSACleanup();
        return false;
    }

    // set listen queue to 200
    // backlog limit is about pending connections(handshake)
    if (listen(serverSocket, 200) != 0)
    {
        logger.log(TinyLogger::LogLevel::ERR, "listen Failed!");
        closesocket(serverSocket);
        WSACleanup();
        return false;
    }

    iocpHandle = CreateIoCompletionPort(INVALID_HANDLE_VALUE, NULL, 0, 0);
    if (iocpHandle == NULL)
    {
        logger.log(TinyLogger::LogLevel::ERR, "CreateIoCompletionPort failed {}", GetLastError());
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
    // Test Hazard Pointer Guard
    TinyLogger logger("server-log");
    logger.log(TinyLogger::LogLevel::INFO, "Start to setup");
    HANDLE iocpHandle;
    if (!Setup(iocpHandle, logger))
    {
        logger.log(TinyLogger::LogLevel::ERR, "Setup failed");
        return 1;
    }
    // create threads to process IOCP events
    std::vector<std::jthread> processThreadGroup;
    for (int i = 0; i < 4; ++i) // Example: create 4 threads for processing IOCP events
    {
        auto t = std::jthread(ProcessIO, std::ref(logger), iocpHandle);
        processThreadGroup.emplace_back(std::move(t));
    }

    logger.log(TinyLogger::LogLevel::INFO, "Setup complete. Server is running...");

    while (running)
    {
        // accept connections or handle IOCP events here
        sockaddr_in clientAddr;
        int         addrLen      = sizeof(clientAddr);
        SOCKET      clientSocket = accept(serverSocket, (sockaddr*)&clientAddr, &addrLen);

        if (clientSocket == INVALID_SOCKET)
        {
            logger.log(TinyLogger::LogLevel::ERR, "accept failed: {}", WSAGetLastError());
            continue;
        }

        // associate the client socket with the IOCP
        HANDLE clientHandle = CreateIoCompletionPort((HANDLE)clientSocket, iocpHandle, 0, 0);
        if (clientHandle == NULL)
        {
            logger.log(TinyLogger::LogLevel::ERR, "CreateIoCompletionPort failed: {}", GetLastError());
            closesocket(clientSocket);
            continue;
        }

        // Initialize the PER_IO_OPERATION_DATA structure
        LPPER_IO_OPERATION_DATA perIoData = new PER_IO_OPERATION_DATA();
        memset(&(perIoData->Overlapped), 0, sizeof(WSAOVERLAPPED));
        perIoData->wsaBuf.len = MAX_BUFF_SIZE;
        perIoData->wsaBuf.buf = perIoData->buffer;

        DWORD bytesRecv = 0;
        DWORD flags     = 0;
        // Post a receive operation
        if (WSARecv(clientSocket, &(perIoData->wsaBuf), 1, &bytesRecv, &flags, &(perIoData->Overlapped), NULL) == SOCKET_ERROR)
        {
            if (WSAGetLastError() != WSA_IO_PENDING)
            {
                logger.log(TinyLogger::LogLevel::ERR, "WSARecv failed: {}", WSAGetLastError());
                delete perIoData;
                closesocket(clientSocket);
                continue;
            }
        }
        logger.log(TinyLogger::LogLevel::INFO, "Accepted a new connection from {}:{}", inet_ntoa(clientAddr.sin_addr), ntohs(clientAddr.sin_port));
    }

    logger.log(TinyLogger::LogLevel::INFO, "Server is shutting down...");
    CloseHandle(iocpHandle);
    closesocket(serverSocket);
    WSACleanup();
    return 0;
}