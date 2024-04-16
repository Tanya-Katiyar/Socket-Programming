#include <iostream>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <cstring>
#define PORT 8080

struct sockaddr_in srv;

int main()
{
    int nSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (nSocket < 0)
    {
        std::cout << "Connection not established\n";
        exit(EXIT_FAILURE);
    }

    srv.sin_family = AF_INET;
    srv.sin_port = htons(PORT);
    srv.sin_addr.s_addr = inet_addr("127.0.0.1");
    memset(&(srv.sin_zero), 0, 8);

    size_t cnct = connect(nSocket, (struct sockaddr *)&srv, sizeof(srv));
    if (cnct < 0)
    {
        std::cout << "Not connected\n";
        exit(EXIT_FAILURE);
    }
    else
    {
        while (true)
        {
            std::string str;
            std::cin >> str;
            int snd = send(nSocket, str.c_str(), str.length(), 0);
            if (snd < 0)
            {
                std::cout << "Message not send\n"
                          << snd;
            }
            else
            {
                std::cout << "Message sent\n";
            }
        }
    }
    return 0;
}