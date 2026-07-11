#include "ClientManager.hpp"
#include <unistd.h>
#include <sys/socket.h>
#include <iostream>
#include <algorithm>

ClientManager::ClientManager(size_t maxClients) : _maxClients(maxClients) {}

ClientManager::ClientManager(const ClientManager& other)
    : _maxClients(other._maxClients), _clients(other._clients), _clientBuffers(other._clientBuffers) {}

ClientManager& ClientManager::operator=(const ClientManager& other) {
    if (this != &other) {
        _maxClients = other._maxClients;
        _clients = other._clients;
        _clientBuffers = other._clientBuffers;
    }
    return *this;
}

ClientManager::~ClientManager() {
    for (int fd : _clients) {
        ::close(fd);
    }
}

bool ClientManager::addClient(int clientFd) {
    if (_clients.size() >= _maxClients) {
        std::cerr << "Connection rejected: maximum limit of " << _maxClients << " clients reached." << std::endl;
        ::close(clientFd);
        return false;
    }
    _clients.push_back(clientFd);
    _clientBuffers[clientFd] = "";
    std::cout << "Client connected (FD: " << clientFd << ")" << std::endl;
    return true;
}

void ClientManager::removeClient(int clientFd) {
    auto it = std::find(_clients.begin(), _clients.end(), clientFd);
    if (it != _clients.end()) {
        _clients.erase(it);
        _clientBuffers.erase(clientFd);
        ::close(clientFd);
        std::cout << "Client disconnected (FD: " << clientFd << ")" << std::endl;
    }
}

const std::vector<int>& ClientManager::getClients() const {
    return _clients;
}

bool ClientManager::hasSpace() const {
    return _clients.size() < _maxClients;
}

bool ClientManager::handleClientData(int clientFd) {
    char buf[1024];
    ssize_t bytesRead = ::recv(clientFd, buf, sizeof(buf) - 1, 0);
    
    if (bytesRead <= 0) {
        // Connection closed or error
        removeClient(clientFd);
        return true; 
    }
    
    buf[bytesRead] = '\0';
    _clientBuffers[clientFd] += buf;
    
    std::string& clientBuf = _clientBuffers[clientFd];
    size_t pos;
    
    // Process line-by-line
    while ((pos = clientBuf.find('\n')) != std::string::npos) {
        std::string line = clientBuf.substr(0, pos);
        clientBuf.erase(0, pos + 1);
        
        // Strip trailing carriage return '\r' (for telnet/nc compatibility)
        if (!line.empty() && line.back() == '\r') {
            line.pop_back();
        }
        
        // Log or print the received message
        std::cout << "Received from client FD " << clientFd << ": " << line << std::endl;
        
        if (line == "quit") {
            std::cout << "Quit command received from client FD " << clientFd << ". Initiating shutdown." << std::endl;
            return false; // Signal that daemon should stop
        }
    }
    
    return true;
}
