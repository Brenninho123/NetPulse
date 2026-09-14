#include <iostream>
#include <string>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <cstring>

namespace NetPulse {
    class Server {
    public:
        Server(int port) : port_(port), server_fd_(-1) {}

        ~Server() {
            stop();
        }

        bool start() {
            server_fd_ = socket(AF_INET, SOCK_STREAM, 0);
            if (server_fd_ < 0) {
                return false;
            }

            int opt = 1;
            setsockopt(server_fd_, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

            sockaddr_in address{};
            address.sin_family = AF_INET;
            address.sin_addr.s_addr = INADDR_ANY;
            address.sin_port = htons(port_);

            if (bind(server_fd_, reinterpret_cast<sockaddr*>(&address), sizeof(address)) < 0) {
                close(server_fd_);
                server_fd_ = -1;
                return false;
            }

            if (listen(server_fd_, 10) < 0) {
                close(server_fd_);
                server_fd_ = -1;
                return false;
            }

            return true;
        }

        void run() {
            if (server_fd_ < 0) return;

            std::cout << "NetPulse core running on port " << port_ << "..." << std::endl;

            while (true) {
                int client_fd = accept(server_fd_, nullptr, nullptr);
                if (client_fd < 0) {
                    continue;
                }

                char buffer[1024] = {0};
                read(client_fd, buffer, sizeof(buffer));

                std::string body = "{\"core\": \"NetPulse Engine\", \"active\": true}";
                std::string response =
                    "HTTP/1.1 200 OK\r\n"
                    "Content-Type: application/json\r\n"
                    "Content-Length: " + std::to_string(body.length()) + "\r\n"
                    "Connection: close\r\n"
                    "\r\n" + body;

                send(client_fd, response.c_str(), response.length(), 0);
                close(client_fd);
            }
        }

        void stop() {
            if (server_fd_ >= 0) {
                close(server_fd_);
                server_fd_ = -1;
            }
        }

    private:
        int port_;
        int server_fd_;
    };
}

int main() {
    NetPulse::Server server(8080);
    if (!server.start()) {
        return 1;
    }
    server.run();
    return 0;
}
