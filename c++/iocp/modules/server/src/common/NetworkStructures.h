#pragma once

#include "Common.h"
#include <WinSock2.h>
#include <string>
#include <vector>
#include <memory>

struct PerIOOperationData
{
    WSAOVERLAPPED m_overlapped;
    WSABUF        m_wsaBuf;
    char          m_buffer[MAX_BUFFER_SIZE];
    IOType        m_type;
    SOCKET        m_socket;
    DWORD         m_bytesTransferred;

    PerIOOperationData();
    ~PerIOOperationData() = default;

    // Non-copyable but movable
    PerIOOperationData(const PerIOOperationData&)            = delete;
    PerIOOperationData& operator=(const PerIOOperationData&) = delete;
    PerIOOperationData(PerIOOperationData&&)                 = default;
    PerIOOperationData& operator=(PerIOOperationData&&)      = default;
};

using PerIOOperationDataPtr = std::unique_ptr<PerIOOperationData>;

enum class PacketState
{
    WaitingForHeader,
    WaitingForContent
};

class PacketBuffer
{
private:
    std::vector<uint8_t> m_buffer;
    PacketState          m_state;
    PacketHeader         m_currentHeader;
    size_t               m_bytesNeeded;

public:
    PacketBuffer();

    // Add received data and return complete packets
    std::vector<std::vector<uint8_t>> ProcessReceivedData(const uint8_t* data, size_t dataSize);

    void        Reset();
    size_t      GetBytesNeeded() const { return m_bytesNeeded; }
    PacketState GetState() const { return m_state; }
};