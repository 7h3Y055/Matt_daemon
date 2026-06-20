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

echo -e "${CYAN}=== 3. Starting the daemon ===${NC}"
# Since Matt_daemon forks, the parent returns immediately and child goes to bg.
$DAEMON_BIN
EXIT_CODE=$?

if [ $EXIT_CODE -ne 0 ]; then
  echo -e "${RED}Error: Failed to start the daemon (exit code: $EXIT_CODE)${NC}"
  exit 1
fi

# Wait a moment for the child process to initialize and write the lock file
sleep 1

echo -e "${CYAN}=== 4. Verifying running processes and lock file ===${NC}"
# Check if lock file exists
if [ ! -f "$LOCK_FILE_PATH" ]; then
  echo -e "${RED}Error: Lock file $LOCK_FILE_PATH was not created!${NC}"
  pkill -x Matt_daemon 2>/dev/null
  exit 1
fi
echo -e "${GREEN}Lock file created successfully at $LOCK_FILE_PATH${NC}"

# Read PID from lock file
LOCK_PID=$(cat "$LOCK_FILE_PATH")
echo "PID stored in lock file: $LOCK_PID"

# Verify that a process with this PID is running and it is Matt_daemon
if ! ps -p "$LOCK_PID" > /dev/null; then
  echo -e "${RED}Error: Process $LOCK_PID is not running!${NC}"
  pkill -x Matt_daemon 2>/dev/null
  rm -f "$LOCK_FILE_PATH"
  exit 1
fi
echo -e "${GREEN}Verified: Daemon process is running with PID $LOCK_PID.${NC}"

echo -e "${CYAN}=== 5. Attempting to start a second instance ===${NC}"
# This should fail because the lock file is locked.
# Note: Since the parent process forks and exits with 0 immediately,
# the command exit code will be 0. However, the child process will print the lock error
# and exit with 1 in the background. Because the child shares the stderr descriptor,
# the command output capturing will wait until the child exits and closes it,
# letting us capture the error message.
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

echo -e "${CYAN}=== 6. Terminating the daemon ===${NC}"
# Kill the daemon process using the PID from the lock file
kill "$LOCK_PID" 2>/dev/null
# Wait a moment for cleanup
sleep 1

# Check if lock file was removed by the destructor/unlock
if [ -f "$LOCK_FILE_PATH" ]; then
  echo -e "${YELLOW}Warning: Lock file was not cleaned up automatically. Cleaning up manually...${NC}"
  rm -f "$LOCK_FILE_PATH"
else
  echo -e "${GREEN}Lock file was cleaned up successfully.${NC}"
fi

# Verify process is no longer running
if ps -p "$LOCK_PID" > /dev/null; then
  echo -e "${RED}Error: Daemon process is still running!${NC}"
  kill -9 "$LOCK_PID" 2>/dev/null
  exit 1
fi
echo -e "${GREEN}Daemon process terminated successfully.${NC}"

echo -e "${GREEN}=== ALL TESTS PASSED SUCCESSFULLY ===${NC}"
exit 0
