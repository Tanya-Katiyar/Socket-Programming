#include <iostream>
#include <cstring>
#include <vector>
#include <thread>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
using namespace std;

// Define a constant for the server port
const int PORT = 8080;

// Define a function for handling a client connection
void handle_client(int client_socket)
{
    // Create a buffer for receiving data from the client
    char buffer[1024];

    // Receive data from the client until the connection is closed
    while (true)
    {
        // Receive data from the client
        int bytes_received = recv(client_socket, buffer, sizeof(buffer), 0);

        // Check if the connection was closed
        if (bytes_received < 0)
        {
            break;
        }

        // Print the received data
        std::cout << "Received: " << buffer << std::endl;

        // Send the data back to the client
        send(client_socket, buffer, strlen(buffer), 0);
    }

    // Close the socket
    close(client_socket);
}

int main()
{
    // Create a socket
    int server_socket = socket(AF_INET, SOCK_STREAM, 0);

    // Check if the socket was created
    if (server_socket < 0)
    {
        std::cerr << "Failed to create socket" << std::endl;
        return 1;
    }

    // Set up the server address
    struct sockaddr_in server_addr;
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT);
    server_addr.sin_addr.s_addr = INADDR_ANY;

    // Bind the socket to the server address
    if (bind(server_socket, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0)
    {
        std::cerr << "Failed to bind socket" << std::endl;
        return 1;
    }

    // Listen for incoming connections
    if (listen(server_socket, 5) < 0)
    {
        std::cerr << "Failed to listen on socket" << std::endl;
        return 1;
    }

    std::cout << "Listening on port " << PORT << std::endl;

    // Accept incoming connections
    while (true)
    {
        // Create a socket to accept incoming connections
        int client_socket = accept(server_socket, NULL, NULL);

        // Check if the connection was accepted
        if (client_socket < 0)
        {
            std::cerr << "Failed to accept connection" << std::endl;
            continue;
        }

        // Create a new thread to handle the client connection
        std::thread handle_client_thread(handle_client, client_socket);

        // Detach the thread
        handle_client_thread.detach();
    }

    // Close the socket
    close(server_socket);

    return 0;
}