# Matt_daemon

A lightweight C++ system daemon that runs in the background, manages client connections on a specific port, and logs all activities cleanly. Built to understand the underlying principles of Unix daemonization and network socket programming.

## Project Structure

```
Matt_daemon/
├── Makefile
├── README.md
├── include/
│   ├── ClientManager.hpp
│   ├── Daemonizer.hpp
│   ├── LockFile.hpp
│   ├── MattDaemon.hpp
│   ├── Server.hpp
│   ├── SignalHandler.hpp
│   ├── Tintin_reporter.hpp
│   └── Utils.hpp
├── src/
│   ├── daemon/
│   │   ├── Daemonizer.cpp
│   │   ├── LockFile.cpp
│   │   ├── MattDaemon.cpp
│   │   └── SignalHandler.cpp
│   ├── logger/
│   │   └── Tintin_reporter.cpp
│   ├── network/
│   │   ├── ClientManager.cpp
│   │   └── Server.cpp
│   ├── utils/
│   │   └── Utils.cpp
│   └── main.cpp
└── Bonus/
```

## Features

- **True Daemonization**: Forking, session detaching (`setsid`), and standard file descriptor closure.
- **Single Instance Enforcement**: Uses file locking (`flock`) on `/var/lock/matt_daemon.lock` to prevent multiple instances from running simultaneously.
- **Port Listening**: Accepts TCP connections on port `4242`.
- **Client Management**: Limits concurrent active connections to exactly 3.
- **Graceful Shutdown**: Automatically intercepts system signals (e.g., `SIGINT`, `SIGTERM`) to trigger a clean exit, or exits immediately when a client sends the string `"quit"`.
- **System Logging (Tintin_reporter)**: Logs all startup/shutdown events, signals, and client inputs directly to `/var/log/matt_daemon/matt_daemon.log` with timestamped formatting.

## Build & Run

### Prerequisites
- A Unix-like operating system (Linux kernel > 3.14 recommended).
- `g++` and `make` installed.
- Root privileges are required to create the daemon locks and log directories in `/var`.

### Compilation
Run the following command to compile the project:
```bash
make
```

### Execution
You must run the daemon as root to ensure it can obtain the necessary system locks:
```bash
sudo ./Matt_daemon
```

If it successfully starts, it will detach and run in the background. You can monitor its activity by checking the log file:
```bash
tail -f /var/log/matt_daemon/matt_daemon.log
```

### Usage
You can connect to the daemon using netcat:
```bash
nc 127.0.0.1 4242
```
Any messages you type will be recorded in the log file.
To shut down the daemon cleanly, type:
```
quit
```

## Architecture

| Component | Responsibility |
| --- | --- |
| **MattDaemon** | Main application coordinator and lifecycle management. |
| **Server** | Socket creation, binding, and multiplexed listening (using `select`). |
| **ClientManager** | Handles the active client list and prevents exceeding the max limit. |
| **Daemonizer** | Safely detaches the process (`fork()`, `setsid()`, `chdir()`, close fds). |
| **LockFile** | Single instance enforcement via `/var/lock/matt_daemon.lock` using `flock()`. |
| **SignalHandler** | Intercepts system signals (`SIGINT`, `SIGTERM`, etc.) to terminate the loop safely. |
| **Tintin_reporter** | Logging component ensuring all system actions and user inputs are recorded properly. |
| **Utils** | Helper functions such as timestamp formatting. |