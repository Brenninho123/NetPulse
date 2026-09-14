#include <iostream>
#include <string>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <cstring>

int main() {
    int server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd < 0) {
        return 1;
    }

    int opt = 1;
    setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    sockaddr_in address{};
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(8080);

    if (bind(server_fd, reinterpret_cast<sockaddr*>(&address), sizeof(address)) < 0) {
        close(server_fd);
        return 1;
    }

    if (listen(server_fd, 5) < 0) {
        close(server_fd);
        return 1;
    }

    std::cout << "NetPulse server listening on port 8080..." << std::endl;

    while (true) {
        int client_fd = accept(server_fd, nullptr, nullptr);
        if (client_fd < 0) {
            continue;
        }

        char buffer[1024] = {0};
        read(client_fd, buffer, sizeof(buffer));

        std::string payload = "{\"system\": \"NetPulse\", \"status\": \"online\"}";
        std::string response = 
            "HTTP/1.1 200 OK\r\n"
            "Content-Type: application/json\r\n"
            "Content-Length: " + std::to_string(payload.length()) + "\r\n"
            "Connection: close\r\n"
            "\r\n" + 
            payload;

        send(client_fd, response.c_str(), response.length(), 0);
        close(client_fd);
    }

    close(server_fd);
    return 0;
}
