#!/bin/bash

# Define color codes
GREEN='\033[0;32m'
RED='\033[0;31m'
YELLOW='\033[0;33m'
CYAN='\033[0;36m'
NC='\033[0m' # No Color

# Ensure the script is run as root
if [ "$EUID" -ne 0 ]; then
  echo -e "${RED}Error: This test script must be run as root (e.g., using sudo).${NC}"
  echo "Usage: sudo $0"
  exit 1
fi

LOCK_FILE_PATH="/var/lock/matt_daemon.lock"
DAEMON_BIN="./Matt_daemon"

echo -e "${CYAN}=== 1. Cleaning up any active daemon instances ===${NC}"
pkill -x Matt_daemon 2>/dev/null
rm -f "$LOCK_FILE_PATH"

echo -e "${CYAN}=== 2. Rebuilding the project ===${NC}"
make re >/dev/null 2>&1
if [ ! -f "$DAEMON_BIN" ]; then
  echo -e "${RED}Error: Compilation failed${NC}"
  exit 1
fi


echo -e "${CYAN}=== 3. [Test Round 1: SIGINT (2)] Starting the daemon ===${NC}"
$DAEMON_BIN
if [ $? -ne 0 ]; then
  echo -e "${RED}Error: Failed to start the daemon${NC}"
  exit 1
fi
sleep 1

# Verify running and lockfile
if [ ! -f "$LOCK_FILE_PATH" ]; then
  echo -e "${RED}Error: Lock file not created${NC}"
  pkill -x Matt_daemon 2>/dev/null
  exit 1
fi
LOCK_PID_INT=$(cat "$LOCK_FILE_PATH")
echo "Daemon running under PID: $LOCK_PID_INT"

# Send SIGINT (2)
echo -e "${CYAN}Sending SIGINT (2) to PID $LOCK_PID_INT...${NC}"
kill -2 "$LOCK_PID_INT" 2>/dev/null
sleep 1

# Check if lock file was removed
if [ -f "$LOCK_FILE_PATH" ]; then
  echo -e "${RED}Error: Lock file was not cleaned up automatically by SIGINT handler!${NC}"
  rm -f "$LOCK_FILE_PATH"
  exit 1
else
  echo -e "${GREEN}Lock file was cleaned up automatically by SIGINT handler.${NC}"
fi

# Verify process terminated
if ps -p "$LOCK_PID_INT" > /dev/null; then
  echo -e "${RED}Error: Daemon process is still running after SIGINT!${NC}"
  kill -9 "$LOCK_PID_INT" 2>/dev/null
  exit 1
fi
echo -e "${GREEN}Daemon process terminated successfully after SIGINT.${NC}"


echo -e "${CYAN}=== 4. [Test Round 2: SIGTERM (15)] Starting the daemon ===${NC}"
$DAEMON_BIN
if [ $? -ne 0 ]; then
  echo -e "${RED}Error: Failed to start the daemon${NC}"
  exit 1
fi
sleep 1

if [ ! -f "$LOCK_FILE_PATH" ]; then
  echo -e "${RED}Error: Lock file $LOCK_FILE_PATH was not created!${NC}"
  pkill -x Matt_daemon 2>/dev/null
  exit 1
fi
echo -e "${GREEN}Lock file created successfully at $LOCK_FILE_PATH${NC}"

LOCK_PID_TERM=$(cat "$LOCK_FILE_PATH")
echo "PID stored in lock file: $LOCK_PID_TERM"

# Verify that process is running
if ! ps -p "$LOCK_PID_TERM" > /dev/null; then
  echo -e "${RED}Error: Process $LOCK_PID_TERM is not running!${NC}"
  pkill -x Matt_daemon 2>/dev/null
  rm -f "$LOCK_FILE_PATH"
  exit 1
fi
echo -e "${GREEN}Verified: Daemon process is running with PID $LOCK_PID_TERM.${NC}"


echo -e "${CYAN}=== 5. Attempting to start a second instance ===${NC}"
SECOND_OUT=$($DAEMON_BIN 2>&1)
EXPECTED_ERR="Cant open $LOCK_FILE_PATH: File exists"
if [[ "$SECOND_OUT" != *"$EXPECTED_ERR"* ]]; then
  echo -e "${RED}Error: Second instance started successfully or did not print expected lock error!${NC}"
  echo "Output was:"
  echo "$SECOND_OUT"
  pkill -x Matt_daemon 2>/dev/null
  rm -f "$LOCK_FILE_PATH"
  exit 1
fi
echo -e "${GREEN}Second instance failed to start as expected (printed: '$EXPECTED_ERR').${NC}"


echo -e "${CYAN}=== 6. Terminating the daemon with SIGTERM ===${NC}"
# Send SIGTERM (15)
kill -15 "$LOCK_PID_TERM" 2>/dev/null
sleep 1

# Check if lock file was removed
if [ -f "$LOCK_FILE_PATH" ]; then
  echo -e "${RED}Error: Lock file was not cleaned up automatically by SIGTERM handler!${NC}"
  rm -f "$LOCK_FILE_PATH"
  exit 1
else
  echo -e "${GREEN}Lock file was cleaned up automatically by SIGTERM handler.${NC}"
fi

# Verify process terminated
if ps -p "$LOCK_PID_TERM" > /dev/null; then
  echo -e "${RED}Error: Daemon process is still running after SIGTERM!${NC}"
  kill -9 "$LOCK_PID_TERM" 2>/dev/null
  exit 1
fi
echo -e "${GREEN}Daemon process terminated successfully after SIGTERM.${NC}"

echo -e "${GREEN}=== ALL TESTS PASSED SUCCESSFULLY ===${NC}"
exit 0
