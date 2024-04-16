#include <iostream>
#include <string>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <vector>
#include <thread>

#define PORT 8080

struct sockaddr_in srv;
fd_set fr, fw, fe; // fr -> ready to read sockets, fw -> ready to write to the network sockets, fe -> The sockets that are throwing an exception.
int nMaxFD;
int nSocket;
int sd;
int new_socket;
int addrlen = sizeof(srv);

void handleClient(int client_fd)
{
    char buffer[1024] = {0};
    int valread = read(client_fd, buffer, 1024);
    if (valread < 0)
    {
        std::cerr << "Error reading from client" << std::endl;
        return;
    }

    std::cout << "Received from client "
              << ": " << buffer << std::endl;

    // Close the client connection
    close(client_fd);
}

int main()
{
    int nRet = 0;
    // Initialize the socket
    nSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

    if (nSocket < 0)
    {
        std::cout << "Connection failed\n"
                  << nSocket << "/n";
        exit(EXIT_FAILURE);
    }
    else
    {
        std::cout << "Connection sucessful "
                  << nSocket << "\n";
    }

    // Set setsockopt
    int nOptVal = 0;
    int nOptlen = sizeof(nOptVal);
    nRet = setsockopt(nSocket, SOL_SOCKET, SO_REUSEADDR, (const char *)&nOptVal, nOptlen);
    if (!nRet)
    {
        std::cout << "Set Sock Opt Val Sucessful\n";
    }
    else
    {
        std::cout << "Set Sock Opt Val unsucessful\n";
    }

    // Initialize the environment for sockaddr struct.
    srv.sin_family = AF_INET;
    srv.sin_port = htons(PORT);
    srv.sin_addr.s_addr = INADDR_ANY;

    // Bind the socket to the local port
    nRet = bind(nSocket, (sockaddr *)&srv, sizeof(srv));
    if (nRet < 0)
    {
        std::cout << "Fail to bind to local port\n";
        exit(EXIT_FAILURE);
    }
    else
    {
        std::cout << "Sucessful in binding to local port\n";
    }

    // Listen the request from the client (queues the requests).
    nRet = listen(nSocket, 5);
    if (nRet < 0)
    {
        std::cout << "Fail to start on local port\n";
        exit(EXIT_FAILURE);
    }
    else
    {
        std::cout << "Listening\n";
    }

    while (true)
    {
        if ((new_socket = accept(nSocket, (struct sockaddr *)&srv, (socklen_t *)&addrlen)) < 0)
        {
            std::cerr << "Accept Failed\n";
            continue;
        }

        std::thread client_thread(handleClient, new_socket);
        client_thread.detach();
        std::this_thread::sleep_for(std::chrono::milliseconds(2000));
    }
}