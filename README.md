# Chat Application using Socket_Programming in C++

This project is a client-server chat application written in C++. It allows clients to connect to a server , end and receieve messages. Clients can send messages to specific recipients by name and can also change the recipient during the chat. The server keeps a record of the connected clients and handles routing messages accordingly. All the conversations between different users are stored in a text file.

## Features

- **Server**:
  - Listens for incoming connections on port `8080`.
  - Handles multiple clients and routes messages to the correct recipient.
  - Logs chat messages to a file (`chat_history.txt`).
  
- **Client**:
  - Connects to the server and sends messages to other clients by specifying their name.
  - Allows the user to change the recipient dynamically during the chat.
  - Displays incoming messages in real-time.

- **Chat-History**:
  - Stores Chat history [Date , Time, Name of the sender and the message] in a text file.
  - Attatched a sample Chat History File for reference.
## Requirements

- A Linux-based OS (e.g., Ubuntu).
- A C++ compiler (e.g., `g++`).
- Basic C++ libraries (e.g., `iostream`, `thread`, `mutex`)
