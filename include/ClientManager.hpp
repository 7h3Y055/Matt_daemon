#ifndef CLIENTMANAGER_HPP
#define CLIENTMANAGER_HPP

#include <vector>
#include <string>
#include <map>

class ClientManager {
public:
    ClientManager(size_t maxClients = 3);
    ClientManager(const ClientManager& other);
    ClientManager& operator=(const ClientManager& other);
    ~ClientManager();

    bool addClient(int clientFd);
    void removeClient(int clientFd);
    
    // Returns true if execution should continue, false if a "quit" was received.
    bool handleClientData(int clientFd);

    const std::vector<int>& getClients() const;
    bool hasSpace() const;

private:
    size_t _maxClients;
    std::vector<int> _clients;
    std::map<int, std::string> _clientBuffers;
};

#endif // CLIENTMANAGER_HPP
