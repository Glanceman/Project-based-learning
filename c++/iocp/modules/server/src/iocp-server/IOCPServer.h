#pragma once

#include "Common.h"
#include "ClientManager.h"
#include <WinSock2.h>
#include <thread>
#include <vector>
#include <memory>

class TinyLogger;

class IOCPServer
{
private:
    SOCKET                      m_serverSocket;
    HANDLE                      m_iocpHandle;
    int                         m_port;
    int                         m_workerThreadCount;
    std::vector<std::jthread>   m_workerThreads;
    ClientManager               m_clientManager;
    std::unique_ptr<TinyLogger> m_logger;

    static void        s_signalHandler(int signum);
    static IOCPServer* s_instance;

    bool SetupWinsock();
    bool CreateServerSocket();
    bool SetupIOCP();
    void ProcessIO(HANDLE iocpHandle);
    bool PostReceiveOperation(SOCKET clientSocket);
    void HandleReceivedData(PerIOOperationData* perIoData, DWORD bytesTransferred);
    void HandleClientDisconnection(SOCKET clientSocket, const std::string& reason);
    void ProcessCompletePackets(SOCKET clientSocket, const std::vector<std::vector<uint8_t>>& packets);

public:
    explicit IOCPServer(int port = DEFAULT_PORT, int workerThreads = DEFAULT_WORKER_THREADS);
    ~IOCPServer();

    // Non-copyable and non-movable
    IOCPServer(const IOCPServer&)            = delete;
    IOCPServer& operator=(const IOCPServer&) = delete;
    IOCPServer(IOCPServer&&)                 = delete;
    IOCPServer& operator=(IOCPServer&&)      = delete;

    bool Initialize();
    void Run();
    void Shutdown();

    // Packet processing (override this for custom behavior)
    virtual void OnPacketReceived(SOCKET clientSocket, const std::vector<uint8_t>& packet);
};