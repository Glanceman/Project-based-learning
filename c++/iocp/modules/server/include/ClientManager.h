#pragma once

#include "NetworkStructures.h"
#include <unordered_map>
#include <mutex>
#include <memory>

class TinyLogger;

class ClientState
{
private:
    PacketBuffer       m_packetBuffer;
    mutable std::mutex m_bufferMutex;

public:
    ClientState() = default;

    // Non-copyable but movable
    ClientState(const ClientState&)            = delete;
    ClientState& operator=(const ClientState&) = delete;
    ClientState(ClientState&&)                 = delete;
    ClientState& operator=(ClientState&&)      = delete;

    // Process received data and return complete packets
    std::vector<std::vector<uint8_t>> ProcessData(const uint8_t* data, size_t dataSize);

    // Thread-safe access to packet buffer
    std::lock_guard<std::mutex> GetBufferLock() const { return std::lock_guard<std::mutex>(m_bufferMutex); }
};

class ClientManager
{
private:
    std::unordered_map<SOCKET, std::unique_ptr<ClientState>> m_clientStates;
    mutable std::mutex                                       m_clientStatesMutex;

public:
    void         AddClient(SOCKET clientSocket);
    void         RemoveClient(SOCKET clientSocket);
    ClientState* GetClientState(SOCKET clientSocket);
    void         Clear();

    size_t GetClientCount() const;
};