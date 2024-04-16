#include <iostream>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/fcntl.h>
#include <sys/ioctl.h>
#include <thread>
#include <cstring>

#define PORT 8080

struct sockaddr_in srv;
fd_set fr, fw, fe; // fr -> ready to read sockets, fw -> ready to write to the network sockets, fe -> The sockets that are throwing an exception.
int nMaxFD;
int nSocket;

void ProcessRequest()
{
    if (FD_ISSET(nSocket, &fr))
    {
        socklen_t sockLen = sizeof(srv);
        int Accept = accept(nSocket, NULL, &sockLen);
        if (Accept < 0)
        {
            std::cout << "Error no message recived\n";
            exit(EXIT_FAILURE);
        }
        else
        {
            while (true)
            {
                char ch[1024];
                int rec = recv(Accept, &ch, 1024, 0);
                if (rec > 0)
                {
                    std::cout << "Message : " << ch << "\n";
                }
            }
        }
    }
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

    // Initialize the environment for sockaddr struct.
    srv.sin_family = AF_INET;
    srv.sin_port = htons(PORT);
    // srv.sin_addr.s_addr = inet_addr("127.0.0.1");
    srv.sin_addr.s_addr = INADDR_ANY;
    // memset(srv.sin_zero, 0, 8);
    memset(&(srv.sin_zero), 0, 8);

    // About blocking v/s non blocking sockets
    // optval = 0 means  blocking and optval != 0 means nonblocking
    // u_long optval = 1;
    // nRet = ioctl(nSocket, FIONBIO, &optval);
    // if (nRet != 0)
    // {
    //     std::cout << "fcntl socket failed\n";
    // }
    // else
    // {
    //     std::cout << "fcntl socket passed\n";
    // }

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
    nMaxFD = nSocket;
    struct timeval tv;
    tv.tv_sec = 1;
    tv.tv_usec = 0;

    while (true)
    {
        FD_ZERO(&fr);

        FD_SET(nSocket, &fr);
        // Keep waiting for new requests and proceed as per the request.
        nRet = select(nMaxFD + 1, &fr, nullptr, nullptr, &tv);

        for (int i = 0; i < nRet; i++)
        {
            
        }

        if (nRet > 0)
        {
            /*When someone connects or comunicates with a message over a
            dedicated connection*/
            std::cout << "The sockets ready for read call\n";
            if (FD_ISSET(nSocket, &fe))
            {
                std::cout << "There is an exception just go away from here\n";
            }
            if (FD_ISSET(nSocket, &fw))
            {
                std::cout << "There is somthing we have to write\n";
            }
            if (FD_ISSET(nSocket, &fr))
            {
                std::cout << "Somthing from the client has come up please read it\n";
                ProcessRequest();
            }
        }
        else if (nRet == 0)
        {
            /*No connection or any communication request made or you can say
            that none of the socket descriptors are ready*/
            std::cout << "The sockets not ready\n";
            // exit(EXIT_SUCCESS);
        }
        else
        {
            /*It has failed and your application should show some useful message*/
            std::cout << "Connection Failed : Please retry after some time\n";
            // exit(EXIT_FAILURE);
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(2000));
    }
}