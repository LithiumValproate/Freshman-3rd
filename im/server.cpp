#include "server.h"
#include <iostream>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>

Server::Server(int port) : port_(port), server_fd_(-1), running_(false) {}

Server::~Server() {
    stop();
}

void Server::start() {
    running_ = true;
    server_thread_ = std::thread(&Server::run, this);
}

void Server::stop() {
    running_ = false;
    if (server_thread_.joinable()) server_thread_.join();
    if (server_fd_ != -1) close(server_fd_);
}

void Server::run() {
    server_fd_ = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd_ == -1) {
        std::cerr << "Socket creation failed!" << std::endl;
        return;
    }
    sockaddr_in addr{};
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = INADDR_ANY;
    addr.sin_port = htons(port_);
    if (bind(server_fd_, (struct sockaddr*)&addr, sizeof(addr)) < 0) {
        std::cerr << "Bind failed!" << std::endl;
        return;
    }
    listen(server_fd_, 5);
    std::cout << "Server started on port " << port_ << std::endl;
    while (running_) {
        int client_fd = accept(server_fd_, nullptr, nullptr);
        if (client_fd >= 0) {
            std::cout << "Client connected." << std::endl;
            // 简单回显逻辑
            char buffer[1024] = {0};
            ssize_t n = read(client_fd, buffer, sizeof(buffer));
            if (n > 0) {
                write(client_fd, buffer, n);
            }
            close(client_fd);
        }
    }
}

