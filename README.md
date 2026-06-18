# Matt_daemon
In this project, we will code a daemon.

```
Matt_daemon/
├── Makefile
├── README.md
├── include/
│   ├── MattDaemon.hpp
│   ├── Server.hpp
│   ├── ClientManager.hpp
│   ├── LockFile.hpp
│   ├── SignalHandler.hpp
│   ├── Daemonizer.hpp
│   ├── TintinReporter.hpp
│   ├── Logger.hpp
│   └── Utils.hpp
│
├── src/
│   ├── main.cpp
│   │
│   ├── daemon/
│   │   ├── MattDaemon.cpp
│   │   ├── Daemonizer.cpp
│   │   ├── LockFile.cpp
│   │   └── SignalHandler.cpp
│   │
│   ├── network/
│   │   ├── Server.cpp
│   │   └── ClientManager.cpp
│   │
│   ├── logger/
│   │   ├── TintinReporter.cpp
│   │   └── Logger.cpp
│   │
│   └── utils/
│       └── Utils.cpp
│
├── tests/
│   ├── test_connection.sh
│   ├── test_lock.sh
│   └── test_signals.sh
│
└── .gitignore
```


### Class responsibilities
| Class | Responsibility |
| --- | --- |
| MattDaemon | Main application coordinator |
| Server | Socket creation, bind, listen on 4242 |
| ClientManager | Handle max 3 clients |
| Daemonizer | fork(), setsid(), chdir(), close fds |
| LockFile | /var/lock/matt_daemon.lock management with flock() |
| SignalHandler | SIGINT, SIGTERM, SIGHUP, etc |
| TintinReporter | Logging API required by subject |
| Logger | Low-level file writing |
| Utils | Timestamp, string helpers |