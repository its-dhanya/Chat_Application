#include <iostream>
#include <thread>
#include <string>
#include <unistd.h>
#include <arpa/inet.h>
#include <mutex>

#define PORT 8080

std::mutex print_mutex;  // Mutex to handle concurrent access to std::cout

// Function to handle incoming messages from the server
void receivemsg(int sock) {
    char buffer[1024] = {0};
    while (true) {
        memset(buffer, 0, sizeof(buffer));  // Clear buffer before each read
        int valread = read(sock, buffer, 1024);  // Read incoming message
        if (valread > 0) {
            std::lock_guard<std::mutex> lock(print_mutex);  // Lock to prevent race condition with printing
            std::cout << "\r" << buffer << std::endl;  // Print the received message
            std::cout.flush();  // Ensure immediate output
        }
    }
}

int main() {
    int clisock;
    struct sockaddr_in servaddr;

    // Create a socket for communication
    clisock = socket(AF_INET, SOCK_STREAM, 0);
    if (clisock < 0) {
        std::cerr << "Error: Socket creation failed" << std::endl;
        return 1;
    }

    // Set up the server address
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(PORT);  // Use the defined port
    inet_pton(AF_INET, "127.0.0.1", &servaddr.sin_addr);  // Connect to localhost

    // Attempt to connect to the server
    if (connect(clisock, (struct sockaddr*)&servaddr, sizeof(servaddr)) < 0) {
        std::cerr << "Error: Connection to server failed" << std::endl;
        return 1;
    }

    std::string name;
    std::cout << "Enter your chat name: ";
    std::cin >> name;  // Get the user's name
    send(clisock, name.c_str(), name.size(), 0);  // Send the name to the server

    // Start a new thread to receive messages from the server
    std::thread(receivemsg, clisock).detach();  // Detach thread so it runs independently

    std::string receiver;
    std::cout << "Specify the recipient's name: ";
    std::cin >> receiver;  // Get the recipient's name
    std::cin.ignore();  // Ignore leftover newline from previous input

    while (true) {
        std::string msg;
        std::getline(std::cin, msg);  // Get the message to send

        // Change recipient if the user types '/change'
        if (msg == "/change") {
            std::cout << "Enter new recipient name: ";
            std::getline(std::cin, receiver);
            continue;  // Skip sending the message, continue with the new recipient
        }

        // Format the message and send it to the server
        std::string fullmsg = receiver + "|" + msg;
        if (send(clisock, fullmsg.c_str(), fullmsg.size(), 0) < 0) {
            std::cerr << "Error: Message failed to send." << std::endl;
        }
    }

    close(clisock);  // Close the socket when done
    return 0;
}
