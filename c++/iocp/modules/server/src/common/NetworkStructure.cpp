#include "NetworkStructures.h"
#include <algorithm>
#include <cstring>

PerIOOperationData::PerIOOperationData() :
    m_type(IOType::Read), m_socket(INVALID_SOCKET), m_bytesTransferred(0)
{
    ZeroMemory(&m_overlapped, sizeof(WSAOVERLAPPED));
    ZeroMemory(m_buffer, MAX_BUFFER_SIZE);
    m_wsaBuf.buf = m_buffer;
    m_wsaBuf.len = MAX_BUFFER_SIZE;
}

PacketBuffer::PacketBuffer() :
    m_state(PacketState::WaitingForHeader), m_bytesNeeded(PACKET_HEADER_SIZE)
{
    m_buffer.reserve(MAX_BUFFER_SIZE);
}

std::vector<std::vector<uint8_t>> PacketBuffer::ProcessReceivedData(const uint8_t* data, size_t dataSize)
{
    std::vector<std::vector<uint8_t>> completePackets;

    size_t dataOffset = 0;
    while (dataOffset < dataSize)
    {
        size_t bytesToCopy = std::min(dataSize - dataOffset, m_bytesNeeded);

        // Append data to our buffer
        m_buffer.insert(m_buffer.end(), data + dataOffset, data + dataOffset + bytesToCopy);
        dataOffset += bytesToCopy;
        m_bytesNeeded -= bytesToCopy;

        if (m_bytesNeeded == 0)
        {
            if (m_state == PacketState::WaitingForHeader)
            {
                // We have a complete header
                std::memcpy(&m_currentHeader, m_buffer.data(), PACKET_HEADER_SIZE);

                if (m_currentHeader.m_contentSize == 0)
                {
                    // Header-only packet
                    completePackets.emplace_back(m_buffer);
                    Reset();
                }
                else
                {
                    // Wait for content
                    m_state       = PacketState::WaitingForContent;
                    m_bytesNeeded = m_currentHeader.m_contentSize;
                }
            }
            else // WaitingForContent
            {
                // We have a complete packet (header + content)
                completePackets.emplace_back(m_buffer);
                Reset();
            }
        }
    }

    return completePackets;
}

void PacketBuffer::Reset()
{
    m_buffer.clear();
    m_state         = PacketState::WaitingForHeader;
    m_bytesNeeded   = PACKET_HEADER_SIZE;
    m_currentHeader = PacketHeader{};
}