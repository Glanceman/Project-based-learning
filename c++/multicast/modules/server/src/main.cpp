#include <iostream>
#include <string>
#include <Winsock2.h>
#include <Ws2tcpip.h>



int         Port   = 8910;
std::string IP     = "234.5.6.7";
int         optval = 0;

int main()
{
    // WSAStartup
    WSADATA wsaData;
    int     iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
    if (iResult != NO_ERROR)
    {
        std::cout << "WSAStartup function failed with error: <" << iResult << ">\n";
        WSACleanup();
        return -1;
    }
    // Create socket
    SOCKET Socket = WSASocket(AF_INET, SOCK_DGRAM, IPPROTO_UDP, NULL, 0, 0);
    // An IPV4 udp blocking socket, I will write a tutorial on overlapped socket in the future.
    if (Socket == INVALID_SOCKET)
    {
        std::cout << "Can not create socket: <" << WSAGetLastError() << ">\n";
        closesocket(Socket);
        WSACleanup();
        return -1;
    }

    // Allow reuse of port
    optval = 1;
    if ((setsockopt(Socket, SOL_SOCKET, SO_REUSEADDR, (char*)&optval, sizeof(optval))) < 0)
    {
        std::cout << "Socket set SO_REUSEADDR fail\n";
        closesocket(Socket);
        WSACleanup();
        return -1;
    }

    // Set target address
    sockaddr_in TargetAddr;
    memset(&TargetAddr, 0, sizeof(TargetAddr));
    TargetAddr.sin_family = AF_INET;
    if (inet_pton(AF_INET, (PCSTR)(IP.c_str()), &TargetAddr.sin_addr.s_addr) < 0)
    {
        std::cout << "Multicast failed set join group\n";
        closesocket(Socket);
        WSACleanup();
        return -1;
    }
    TargetAddr.sin_port = htons(Port);

    std::string SendMessage;
    // Send
    while (1)
    {
        std::cin >> SendMessage;
        iResult = sendto(
            Socket,
            SendMessage.c_str(),
            SendMessage.length(),
            0,
            (struct sockaddr*)&TargetAddr,
            sizeof(TargetAddr));
        if (iResult == SOCKET_ERROR)
        {
            std::cout << "Sendto failed with error: <" << WSAGetLastError() << ">\n";
            closesocket(Socket);
            WSACleanup();
        }
        if (SendMessage.compare("bye") == 0)
        {
            break;
        }
    }

    closesocket(Socket);
    WSACleanup();

    return 1;
}