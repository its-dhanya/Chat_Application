#include<iostream>
#include<thread>
#include<vector>
#include<map>
#include<string>
#include<fstream>
#include<mutex>
#include<sstream>
#include<ctime>
#include<netinet/in.h>
#include<cstring>
#include<unistd.h>

#define PORT 8080  // Define the server port

std::mutex file;  // Mutex to protect log file access
std::mutex client;  // Mutex to protect client data
std::map<int, std::string> clientnames;  // Map for client socket and name
std::map<std::string, int> socket_name;  // Map for client name and socket

// Log chat message to a file
void log_msg(const std::string &sender, const std::string &receiver, const std::string &message) {
    std::lock_guard<std::mutex> lock(file);  // Ensure thread safety for file writing
    
    std::ofstream logfile("/Users/dhanyavenkatesh/DESIS OOPS ASSIGNMENT/chat_history.txt", std::ios::app);
    if (!logfile.is_open()) {
        std::cerr << "Error: Unable to open file for logging." << std::endl;
        return;
    }

    // Get current timestamp
    time_t now = time(0);
    char *dt = ctime(&now);
    dt[strlen(dt) - 1] = '\0';  // Remove newline from timestamp

    // Log the message
    logfile << "[" << dt << "] " << sender << " -> " << receiver << ": " << message << std::endl;
}

// Handle communication with a connected client
void clienthandle(int cli_sock) {
    char buffer[1024] = {0};
    
    read(cli_sock, buffer, 1024);  // Read client name
    std::string cliname(buffer);
    
    // Store client information in maps
    {
        std::lock_guard<std::mutex> lock(client);
        clientnames[cli_sock] = cliname;
        socket_name[cliname] = cli_sock;
    }
    
    std::cout << cliname << " connected.\n";
    
    while (true) {
        memset(buffer, 0, sizeof(buffer));  // Reset buffer
        
        // Read incoming message
        long valread1 = read(cli_sock, buffer, 1024);
        if (valread1 <= 0) {  // Client disconnected
            std::cout << cliname << " disconnected.\n";
            close(cli_sock);
            {
                std::lock_guard<std::mutex> lock(client);
                socket_name.erase(cliname);
                clientnames.erase(cli_sock);
            }
            break;
        }
        
        // Process the message
        std::string message(buffer);
        std::istringstream iss(message);
        std::string receiver, msg;
        getline(iss, receiver, '|');
        getline(iss, msg);
        
        {
            std::lock_guard<std::mutex> lock(client);  // Lock client data
            if (socket_name.find(receiver) != socket_name.end()) {
                int reci_sock = socket_name[receiver];  // Get receiver's socket
                std::string form_msg = cliname + ": " + msg;
                send(reci_sock, form_msg.c_str(), form_msg.size(), 0);  // Send message
                
                log_msg(cliname, receiver, msg);  // Log the message
            } else {
                std::string errmsg = "Receiver not found!";
                send(cli_sock, errmsg.c_str(), errmsg.size(), 0);  // Send error message
            }
        }
    }
}

// Main server function
int main() {
    int servfd, newsock;
    struct sockaddr_in address;
    int opt = 1;
    int addrlen = sizeof(address);
    
    // Create and configure the server socket
    servfd = socket(AF_INET, SOCK_STREAM, 0);
    setsockopt(servfd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);
    
    bind(servfd, (struct sockaddr*)&address, addrlen);  // Bind the socket
    listen(servfd, 10);  // Listen for incoming connections
    
    std::cout << "Server is running on port " << PORT << "...\n";
    
    while (true) {
        newsock = accept(servfd, (struct sockaddr*)&address, (socklen_t*)&addrlen);  // Accept a client connection
        
        // Create a new thread to handle the client
        std::thread(clienthandle, newsock).detach();  // Detach to allow independent execution
    }
    
    return 0;
}
