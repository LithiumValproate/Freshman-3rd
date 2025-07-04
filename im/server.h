#pragma once

#include <string>
#include <thread>
#include <vector>

class Server {
public:
    Server(int port);
    ~Server();
    void start();
    void stop();
private:
    int port_;
    int server_fd_;
    bool running_;
    std::thread server_thread_;
    void run();
};

