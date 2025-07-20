#include "ClientManager.h"

std::vector<std::vector<uint8_t>> ClientState::ProcessData(const uint8_t* data, size_t dataSize)
{
    std::lock_guard<std::mutex> lock(m_bufferMutex);
    return m_packetBuffer.ProcessReceivedData(data, dataSize);
}

void ClientManager::AddClient(SOCKET clientSocket)
{
    std::lock_guard<std::mutex> lock(m_clientStatesMutex);
    m_clientStates[clientSocket] = std::make_unique<ClientState>();
}

void ClientManager::RemoveClient(SOCKET clientSocket)
{
    std::lock_guard<std::mutex> lock(m_clientStatesMutex);
    m_clientStates.erase(clientSocket);
}

ClientState* ClientManager::GetClientState(SOCKET clientSocket)
{
    std::lock_guard<std::mutex> lock(m_clientStatesMutex);
    auto                        it = m_clientStates.find(clientSocket);
    return (it != m_clientStates.end()) ? it->second.get() : nullptr;
}

void ClientManager::Clear()
{
    std::lock_guard<std::mutex> lock(m_clientStatesMutex);
    m_clientStates.clear();
}

size_t ClientManager::GetClientCount() const
{
    std::lock_guard<std::mutex> lock(m_clientStatesMutex);
    return m_clientStates.size();
}