#pragma once

#include <cstdint>
#include <cstddef>

// Network constants
constexpr int DEFAULT_PORT           = 60000;
constexpr int MAX_BUFFER_SIZE        = 1024;
constexpr int LISTEN_BACKLOG         = 200;
constexpr int DEFAULT_WORKER_THREADS = 4;

// Protocol constants
constexpr size_t PACKET_HEADER_SIZE = sizeof(uint32_t); // 4 bytes for content size

enum class IOType : uint8_t
{
    Read,
    Write
};

// Packet structure
struct PacketHeader
{
    uint32_t m_contentSize; // Size of the content following the header

    PacketHeader() :
        m_contentSize(0) {}
    explicit PacketHeader(uint32_t contentSize) :
        m_contentSize(contentSize) {}
};

// Global state
extern volatile bool g_running;