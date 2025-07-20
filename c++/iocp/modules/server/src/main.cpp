#include "IOCPServer.h"
#include "Common.h"
#include <iostream>

// Custom server class for demonstration
class MyServer : public IOCPServer
{
public:
    explicit MyServer(int port = DEFAULT_PORT) : IOCPServer(port) {}

protected:
    void OnPacketReceived(SOCKET clientSocket, const std::vector<uint8_t>& packet) override
    {
        // Call base implementation for logging
        IOCPServer::OnPacketReceived(clientSocket, packet);
        
        // Add custom packet processing here
        // For example, echo the packet back to the client
        // EchoPacket(clientSocket, packet);
    }
};

int main()
{
    try
    {
        MyServer server(DEFAULT_PORT);
        
        if (!server.Initialize())
        {
            std::cerr << "Failed to initialize server" << std::endl;
            return 1;
        }
        
        server.Run();
    }
    catch (const std::exception& e)
    {
        std::cerr << "Exception: " << e.what() << std::endl;
        return 1;
    }
    
    return 0;
}