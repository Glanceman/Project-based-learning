# IOCP Echo Server – README

A minimal, high-performance Windows-overlapped I/O echo server built with IOCP (Input/Output Completion Port), C++17 and Winsock.
It accepts many concurrent TCP connections, parses length-prefixed binary packets and (by default) echoes every received packet back to the sender.
The project is deliberately small and self-contained; extend OnPacketReceived() to turn it into a game server, chat back-end, etc.

---

## Features
- IOCP – scales to thousands of connections with a fixed-size thread pool.
- Length-prefixed protocol – each message is preceded by a 4-byte little-endian length field.
- Thread-safe client management – lock-free access from worker threads via ClientManager.
- Graceful shutdown – handles SIGINT/SIGTERM, closes sockets and joins worker threads.
- Extensible – override OnPacketReceived() for custom logic.

---
## Build
```sh
mkdir build && cd build
cmake .. -G "Visual Studio 17 2022"
cmake --build . --config Release
```

Run:
```sh
.\dist\iocp_server.exe
```

---
## Architecture Overview

Class Diagram

```mermaid
classDiagram
    class IOCPServer {
        -SOCKET m_serverSocket
        -HANDLE m_iocpHandle
        -ClientManager m_clientManager
        -vector~jthread~ m_workerThreads
        -unique_ptr~TinyLogger~ m_logger
        +Initialize() bool
        +Run()
        +Shutdown()
        +OnPacketReceived(SOCKET, const vector~uint8_t~&)
    }

    class ClientManager {
        -unordered_map~SOCKET,unique_ptr~ClientState~~ m_clientStates
        -mutex m_clientStatesMutex
        +AddClient(SOCKET)
        +RemoveClient(SOCKET)
        +GetClientState(SOCKET) ClientState*
        +Clear()
    }

    class ClientState {
        -PacketBuffer m_packetBuffer
        -mutex m_bufferMutex
        +ProcessData(const uint8_t*, size_t) vector~vector~uint8_t~~
    }

    class PacketBuffer {
        -vector~uint8_t~ m_buffer
        -PacketState m_state
        -PacketHeader m_currentHeader
        -size_t m_bytesNeeded
        +ProcessReceivedData(const uint8_t*, size_t) vector~vector~uint8_t~~
        +Reset()
    }

    IOCPServer --> ClientManager : uses
    ClientManager --> ClientState : owns
    ClientState --> PacketBuffer : contains

```
### Packet Format

| 4 bytes (little-endian) | N bytes |
| ----------------------- | ------- |
| Content-Length          | Payload |


---
## Runtime Flow
Sequence Diagram

```mermaid
sequenceDiagram
    participant Main
    participant IOCPServer
    participant WorkerThread
    participant ClientManager
    participant ClientState
    participant Client

    Main->>IOCPServer: Run()
    loop accept loop
        IOCPServer->>Client: accept()
        IOCPServer->>ClientManager: AddClient(socket)
        IOCPServer->>ClientManager: GetClientState(socket)
        IOCPServer->>WorkerThread: Post initial WSARecv()
    end

    Client-->>WorkerThread: sends length-prefixed data
    WorkerThread->>ClientState: ProcessData(buffer, bytes)
    ClientState->>ClientState: parse header & body
    ClientState-->>WorkerThread: complete packets
    WorkerThread->>IOCPServer: OnPacketReceived(socket, packet)
    IOCPServer->>Client: send(packet)  // echo
```
---

- Main thread
  - accept() loop, adds new sockets to IOCP.
- Worker threads (N = CPU cores)
  - Block on GetQueuedCompletionStatus().
  - Parse packets (ClientState::ProcessData).
  - Invoke OnPacketReceived() (user code).
  - Re-post WSARecv() for next chunk.
  
All shared data (ClientManager, ClientState) is protected by std::mutex with minimal lock contention.

---
## Extending the Server

Derive from IOCPServer and override OnPacketReceived():

```cpp
class ChatServer : public IOCPServer {
protected:
    void OnPacketReceived(SOCKET client, const std::vector<uint8_t>& pkt) override {
        // broadcast to all other clients
        for (auto [sock, _] : m_clientManager) {
            if (sock != client) send(sock, pkt);
        }
    }
};
```

---

## logging

Logs are written to server-log.txt via the bundled `tinyLogger`.

Levels: INFO, WARN, ERR.

---
## Shutdown Handling
- `SIGINT/SIGTERM` sets `g_running = false`.
- Main loop exits → `accept()` returns `INVALID_SOCKET`.
- Worker threads time-out, notice `!g_running`, exit gracefully.
- All sockets closed, resources released.
