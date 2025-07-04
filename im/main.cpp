#include "server.h"
#include <iostream>
#include <csignal>

Server* g_server = nullptr;

void signal_handler(int) {
    if (g_server) {
        std::cout << "\nStopping server..." << std::endl;
        g_server->stop();
    }
}

int main() {
    int port = 12345;
    Server server(port);
    g_server = &server;
    std::signal(SIGINT, signal_handler);
    server.start();
    if (server_thread_.joinable()) server_thread_.join();
    std::cout << "Server exited." << std::endl;
    return 0;
}

