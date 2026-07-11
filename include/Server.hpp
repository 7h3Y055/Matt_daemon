#ifndef SERVER_HPP
#define SERVER_HPP

#include "ClientManager.hpp"

class Server {
public:
    Server(int port = 4242);
    Server(const Server& other);
    Server& operator=(const Server& other);
    ~Server();

    bool init();
    void run();
    void stop();

private:
    int _port;
    int _serverFd;
    bool _running;
    ClientManager _clientManager;

    void _acceptConnection();
};

#endif // SERVER_HPP
