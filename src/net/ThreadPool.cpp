#include <iostream>
#include <string>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <cstring>
#include <csignal>
#include <atomic>
#include <memory>
#include <vector>
#include <queue>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <future>
#include <functional>
#include <stdexcept>

namespace NetPulse {
    namespace Net {
        class ThreadPool {
        public:
            explicit ThreadPool(size_t threads) : stop_(false) {
                for (size_t i = 0; i < threads; ++i) {
                    workers_.emplace_back([this] {
                        while (true) {
                            std::function<void()> task;
                            {
                                std::unique_lock<std::mutex> lock(this->queue_mutex_);
                                this->cv_.wait(lock, [this] {
                                    return this->stop_ || !this->tasks_.empty();
                                });

                                if (this->stop_ && this->tasks_.empty()) {
                                    return;
                                }

                                task = std::move(this->tasks_.front());
                                this->tasks_.pop();
                            }
                            task();
                        }
                    });
                }
            }

            template<class F, class... Args>
            auto enqueue(F&& f, Args&&... args) 
                -> std::future<typename std::invoke_result<F, Args...>::type> {
                
                using return_type = typename std::invoke_result<F, Args...>::type;

                auto task = std::make_shared<std::packaged_task<return_type()>>(
                    std::bind(std::forward<F>(f), std::forward<Args>(args)...)
                );
                
                std::future<return_type> res = task->get_future();
                {
                    std::unique_lock<std::mutex> lock(queue_mutex_);

                    if (stop_) {
                        throw std::runtime_error("enqueue on stopped ThreadPool");
                    }

                    tasks_.emplace([task]() { (*task)(); });
                }
                cv_.notify_one();
                return res;
            }

            ~ThreadPool() {
                {
                    std::unique_lock<std::mutex> lock(queue_mutex_);
                    stop_ = true;
                }
                cv_.notify_all();
                for (std::thread &worker : workers_) {
                    if (worker.joinable()) {
                        worker.join();
                    }
                }
            }

        private:
            std::vector<std::thread> workers_;
            std::queue<std::function<void()>> tasks_;
            mutable std::mutex queue_mutex_;
            std::condition_variable cv_;
            bool stop_;
        };
    }

    class Server {
    public:
        Server(int port, size_t thread_pool_size = 4) 
            : port_(port), server_fd_(-1), running_(false), pool_(thread_pool_size) {}

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

            if (listen(server_fd_, 128) < 0) {
                close(server_fd_);
                server_fd_ = -1;
                return false;
            }

            running_ = true;
            return true;
        }

        void run() {
            if (server_fd_ < 0) return;

            while (running_) {
                int client_fd = accept(server_fd_, nullptr, nullptr);
                if (client_fd < 0) {
                    if (!running_) break;
                    continue;
                }

                pool_.enqueue([client_fd]() {
                    char buffer[1024] = {0};
                    read(client_fd, buffer, sizeof(buffer));

                    std::string body = "{\"core\": \"NetPulse Engine\", \"async\": true}";
                    std::string response =
                        "HTTP/1.1 200 OK\r\n"
                        "Content-Type: application/json\r\n"
                        "Content-Length: " + std::to_string(body.length()) + "\r\n"
                        "Connection: close\r\n"
                        "\r\n" + body;

                    send(client_fd, response.c_str(), response.length(), 0);
                    close(client_fd);
                });
            }
        }

        void stop() {
            if (running_) {
                running_ = false;
                if (server_fd_ >= 0) {
                    shutdown(server_fd_, SHUT_RDWR);
                    close(server_fd_);
                    server_fd_ = -1;
                }
            }
        }

    private:
        int port_;
        int server_fd_;
        std::atomic<bool> running_;
        Net::ThreadPool pool_;
    };
}

namespace {
    std::function<void(int)> shutdown_handler;
    void signal_handler(int signal) {
        if (shutdown_handler) {
            shutdown_handler(signal);
        }
    }
}

int main() {
    NetPulse::Server server(8080, 8);

    shutdown_handler = [&server](int) {
        server.stop();
    };

    struct sigaction sa{};
    sa.sa_handler = signal_handler;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = 0;

    sigaction(SIGINT, &sa, nullptr);
    sigaction(SIGTERM, &sa, nullptr);

    if (!server.start()) {
        return 1;
    }

    server.run();
    return 0;
}
