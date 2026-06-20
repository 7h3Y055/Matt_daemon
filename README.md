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

---

## 🛠️ Implementation Roadmap

### 🚀 Main Process (Preparation & Initialization)
*These steps run in the foreground to verify the environment before daemonizing.*

- [ ] **`1` Check Root Privilege**
  - Ensure the program is executed with superuser rights (`geteuid() == 0`).
- [ ] **`3` Check Running Instance (Lock File)**
  - Check `/var/lock/matt_daemon.lock` using `flock()` to prevent running multiple concurrent daemon instances.
- [ ] **`2` Create Daemon Process**
  - Fork and detach the process from the controlling terminal (`fork()`, `setsid()`, closing std fds, etc.).

---

### ♾️ Child Process (Core Daemon Loop)
*These steps execute in the background daemon process.*

- [ ] **`8` Register Signal Handlers**
  - Intercept termination signals (`SIGINT`, `SIGTERM`, etc.) to trigger a clean shutdown.
- [ ] **`5` Prepare Log File**
  - Set up the file `/var/log/matt_daemon/matt_daemon.log` to record system events.
- [ ] **`4` Listen to Port 4242**
  - Initialize the server TCP socket and bind to port `4242` to accept incoming connections.
- [ ] **`7` Handle Client Connections (Max 3)**
  - Listen for client connections and enforce a maximum limit of 3 concurrent active clients.
- [ ] **`6` Handle Clean Exit ("quit")**
  - Monitor socket input and shut down the daemon cleanly when a client sends the `"quit"` command.