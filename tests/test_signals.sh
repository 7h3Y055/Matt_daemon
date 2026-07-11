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
LOG_FILE_PATH="/var/log/matt_daemon/matt_daemon.log"
DAEMON_BIN="./Matt_daemon"

echo -e "${CYAN}=== 1. Cleaning up existing daemon and log file ===${NC}"
pkill -x Matt_daemon 2>/dev/null
rm -f "$LOCK_FILE_PATH"
rm -f "$LOG_FILE_PATH"

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

# Check if log file contains the started message
if [ ! -f "$LOG_FILE_PATH" ]; then
  echo -e "${RED}Error: Log file not created at $LOG_FILE_PATH${NC}"
  pkill -x Matt_daemon 2>/dev/null
  exit 1
fi

if ! grep -q "Started." "$LOG_FILE_PATH"; then
  echo -e "${RED}Error: 'Started.' message not found in log file!${NC}"
  pkill -x Matt_daemon 2>/dev/null
  exit 1
fi
echo -e "${GREEN}Verified: 'Started.' message is logged.${NC}"

echo -e "${CYAN}=== 4. Sending SIGINT (2) to the daemon ===${NC}"
kill -2 "$DAEMON_PID"
sleep 1

# Check if daemon exited and cleaned up lock
if ps -p "$DAEMON_PID" > /dev/null; then
  echo -e "${RED}Error: Daemon is still running after SIGINT!${NC}"
  pkill -x Matt_daemon 2>/dev/null
  exit 1
fi

if [ -f "$LOCK_FILE_PATH" ]; then
  echo -e "${RED}Error: Lock file was not cleaned up after SIGINT!${NC}"
  rm -f "$LOCK_FILE_PATH"
  exit 1
fi

# Check logs for signal message
if ! grep -q "Signal 2 received." "$LOG_FILE_PATH"; then
  echo -e "${RED}Error: 'Signal 2 received.' not found in logs!${NC}"
  cat "$LOG_FILE_PATH"
  exit 1
fi

if ! grep -q "Quitting." "$LOG_FILE_PATH"; then
  echo -e "${RED}Error: 'Quitting.' message not found in logs!${NC}"
  cat "$LOG_FILE_PATH"
  exit 1
fi

echo -e "${GREEN}Verified: SIGINT signal (2) and Quitting messages were logged correctly.${NC}"

echo -e "${CYAN}=== 5. Testing SIGTERM (15) ===${NC}"
# Start daemon again
$DAEMON_BIN
sleep 1
DAEMON_PID=$(cat "$LOCK_FILE_PATH")

# Send SIGTERM (15)
kill -15 "$DAEMON_PID"
sleep 1

# Check lock file removed
if [ -f "$LOCK_FILE_PATH" ]; then
  echo -e "${RED}Error: Lock file was not cleaned up after SIGTERM!${NC}"
  rm -f "$LOCK_FILE_PATH"
  exit 1
fi

# Check logs for signal 15
if ! grep -q "Signal 15 received." "$LOG_FILE_PATH"; then
  echo -e "${RED}Error: 'Signal 15 received.' not found in logs!${NC}"
  cat "$LOG_FILE_PATH"
  exit 1
fi
echo -e "${GREEN}Verified: SIGTERM signal (15) was logged correctly.${NC}"

echo -e "${GREEN}=== ALL SIGNAL TESTS PASSED SUCCESSFULLY ===${NC}"
exit 0
