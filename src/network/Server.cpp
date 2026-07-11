#include "Server.hpp"
#include "SignalHandler.hpp"
#include "Log.hpp"
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <fcntl.h>
#include <iostream>
#include <cstring>
#include <algorithm>
#include <errno.h>
#include <sstream>

Server::Server(int port) : _port(port), _serverFd(-1), _running(false) {}

Server::Server(const Server& other)
    : _port(other._port), _serverFd(-1), _running(false), _clientManager(other._clientManager) {}

Server& Server::operator=(const Server& other) {
    if (this != &other) {
        stop();
        _port = other._port;
        _serverFd = -1;
        _running = false;
        _clientManager = other._clientManager;
    }
    return *this;
}

Server::~Server() {
    stop();
}

bool Server::init() {
    _serverFd = ::socket(AF_INET, SOCK_STREAM, 0);
    if (_serverFd < 0) {
        Log::error(std::string("socket creation failed: ") + std::strerror(errno));
        return false;
    }

    int opt = 1;
    if (::setsockopt(_serverFd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0) {
        Log::error(std::string("setsockopt SO_REUSEADDR failed: ") + std::strerror(errno));
        ::close(_serverFd);
        _serverFd = -1;
        return false;
    }

    // Set non-blocking
    int flags = ::fcntl(_serverFd, F_GETFL, 0);
    if (flags < 0 || ::fcntl(_serverFd, F_SETFL, flags | O_NONBLOCK) < 0) {
        Log::error(std::string("setting socket to non-blocking failed: ") + std::strerror(errno));
        ::close(_serverFd);
        _serverFd = -1;
        return false;
    }

    struct sockaddr_in address;
    std::memset(&address, 0, sizeof(address));
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(_port);

    if (::bind(_serverFd, (struct sockaddr*)&address, sizeof(address)) < 0) {
        Log::error(std::string("bind failed: ") + std::strerror(errno));
        ::close(_serverFd);
        _serverFd = -1;
        return false;
    }

    if (::listen(_serverFd, 5) < 0) {
        Log::error(std::string("listen failed: ") + std::strerror(errno));
        ::close(_serverFd);
        _serverFd = -1;
        return false;
    }

    return true;
}

void Server::stop() {
    _running = false;
    if (_serverFd >= 0) {
        ::close(_serverFd);
        _serverFd = -1;
    }
}

void Server::_acceptConnection() {
    struct sockaddr_in address;
    socklen_t addrlen = sizeof(address);
    int newFd = ::accept(_serverFd, (struct sockaddr*)&address, &addrlen);
    if (newFd < 0) {
        return;
    }

    // Set new client socket to non-blocking
    int flags = ::fcntl(newFd, F_GETFL, 0);
    if (flags >= 0) {
        ::fcntl(newFd, F_SETFL, flags | O_NONBLOCK);
    }

    _clientManager.addClient(newFd);
}

void Server::run() {
    _running = true;

    while (_running && !SignalHandler::shouldExit()) {
        fd_set readFds;
        FD_ZERO(&readFds);
        FD_SET(_serverFd, &readFds);
        int maxFd = _serverFd;

        const std::vector<int>& clients = _clientManager.getClients();
        for (int clientFd : clients) {
            FD_SET(clientFd, &readFds);
            maxFd = std::max(maxFd, clientFd);
        }

        struct timeval timeout;
        timeout.tv_sec = 1;
        timeout.tv_usec = 0;

        int activity = ::select(maxFd + 1, &readFds, nullptr, nullptr, &timeout);

        if (activity < 0) {
            if (errno == EINTR) {
                continue;
            }
            Log::error(std::string("select failed: ") + std::strerror(errno));
            break;
        }

        if (activity == 0) {
            continue; // Timeout check for signal handler exit
        }

        if (FD_ISSET(_serverFd, &readFds)) {
            _acceptConnection();
        }

        std::vector<int> activeClients = clients;
        for (int clientFd : activeClients) {
            if (FD_ISSET(clientFd, &readFds)) {
                if (!_clientManager.handleClientData(clientFd)) {
                    _running = false;
                    break;
                }
            }
        }
    }

    if (SignalHandler::shouldExit()) {
        std::stringstream ss;
        ss << "Signal " << SignalHandler::getSignalReceived() << " received.";
        Log::info(ss.str());
    }
}
