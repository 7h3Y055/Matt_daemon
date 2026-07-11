#!/bin/bash

# Define color codes
GREEN='\033[0;32m'
RED='\033[0;31m'
YELLOW='\033[0;33m'
CYAN='\033[0;36m'
NC='\033[0m' # No Color

# Ensure script is run as root
if [ "$EUID" -ne 0 ]; then
  echo -e "${RED}Error: This test script must be run as root (e.g., using sudo).${NC}"
  echo "Usage: sudo $0"
  exit 1
fi

LOCK_FILE_PATH="/var/lock/matt_daemon.lock"
DAEMON_BIN="./Matt_daemon"
PORT=4242

echo -e "${CYAN}=== 1. Cleaning up active daemon instances ===${NC}"
pkill -x Matt_daemon 2>/dev/null
rm -f "$LOCK_FILE_PATH"
sleep 1

echo -e "${CYAN}=== 2. Rebuilding the project ===${NC}"
make re >/dev/null 2>&1
if [ ! -f "$DAEMON_BIN" ]; then
  echo -e "${RED}Error: Compilation failed${NC}"
  exit 1
fi

echo -e "${CYAN}=== 3. Starting the daemon ===${NC}"
$DAEMON_BIN
if [ $? -ne 0 ]; then
  echo -e "${RED}Error: Failed to start the daemon${NC}"
  exit 1
fi
sleep 1

# Check if lock file exists and read PID
if [ ! -f "$LOCK_FILE_PATH" ]; then
  echo -e "${RED}Error: Lock file not created${NC}"
  exit 1
fi
DAEMON_PID=$(cat "$LOCK_FILE_PATH")
echo "Daemon running under PID: $DAEMON_PID"

echo -e "${CYAN}=== 4. Checking port $PORT listening ===${NC}"
if ! lsof -i :$PORT -t > /dev/null; then
  echo -e "${RED}Error: Daemon is not listening on port $PORT!${NC}"
  pkill -x Matt_daemon 2>/dev/null
  exit 1
fi
echo -e "${GREEN}Port $PORT is open and listening.${NC}"

echo -e "${CYAN}=== 5. Testing concurrent client connections (Max 3) ===${NC}"
# Open 1st client using bash /dev/tcp
if ! exec 3<>/dev/tcp/127.0.0.1/$PORT; then
  echo -e "${RED}Error: Could not open 1st connection${NC}"
  pkill -x Matt_daemon 2>/dev/null
  exit 1
fi
echo "Opened 1st client connection"
sleep 0.2

# Open 2nd client
if ! exec 4<>/dev/tcp/127.0.0.1/$PORT; then
  echo -e "${RED}Error: Could not open 2nd connection${NC}"
  exec 3>&-
  pkill -x Matt_daemon 2>/dev/null
  exit 1
fi
echo "Opened 2nd client connection"
sleep 0.2

# Open 3rd client
if ! exec 5<>/dev/tcp/127.0.0.1/$PORT; then
  echo -e "${RED}Error: Could not open 3rd connection${NC}"
  exec 3>&-
  exec 4>&-
  pkill -x Matt_daemon 2>/dev/null
  exit 1
fi
echo "Opened 3rd client connection"
sleep 0.2

# Attempt to open 4th client - should be rejected immediately
if exec 6<>/dev/tcp/127.0.0.1/$PORT 2>/dev/null; then
  sleep 0.5
  # Try to write/read. If the connection was closed by the server, writing or checking will fail.
  if echo "test" >&6 2>/dev/null; then
    # Some OS buffers might accept write but let's check if it remains active
    # Reading from it should return EOF immediately
    if read -t 1 -u 6 line 2>/dev/null; then
      echo -e "${RED}Error: 4th client connection remained open and accepted data!${NC}"
      exec 3>&-
      exec 4>&-
      exec 5>&-
      exec 6>&-
      pkill -x Matt_daemon 2>/dev/null
      exit 1
    else
      # Check if EOF was read (exit code 1, but read didn't timeout or did fail)
      # Usually if EOF is reached, read returns exit code > 128 (timeout) or 1 (EOF).
      # Let's verify by checking if daemon is still alive
      echo -e "${GREEN}4th client was rejected (closed by server).${NC}"
    fi
  else
    echo -e "${GREEN}4th client was rejected (write failed).${NC}"
  fi
  exec 6>&-
else
  echo -e "${GREEN}4th client connection failed/rejected immediately.${NC}"
fi

echo -e "${CYAN}=== 6. Sending text message to daemon ===${NC}"
echo "Hello from Client 1" >&3
sleep 0.5

echo -e "${CYAN}=== 7. Sending 'quit' command to trigger daemon exit ===${NC}"
echo "quit" >&3
sleep 1

# Close file descriptors
exec 3>&-
exec 4>&-
exec 5>&-

# Check if daemon terminated and lock file is removed
if ps -p "$DAEMON_PID" > /dev/null; then
  echo -e "${RED}Error: Daemon process is still running after 'quit' command!${NC}"
  pkill -x Matt_daemon 2>/dev/null
  exit 1
fi
echo -e "${GREEN}Daemon process terminated successfully.${NC}"

if [ -f "$LOCK_FILE_PATH" ]; then
  echo -e "${RED}Error: Lock file was not cleaned up automatically on exit!${NC}"
  rm -f "$LOCK_FILE_PATH"
  exit 1
fi
echo -e "${GREEN}Lock file was cleaned up successfully.${NC}"

echo -e "${GREEN}=== ALL CONNECTION TESTS PASSED SUCCESSFULLY ===${NC}"
exit 0
