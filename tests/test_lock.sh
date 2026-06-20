#!/bin/bash

# If we are not root, configure test mode
if [ "$EUID" -ne 0 ]; then
  echo "Not running as root. Running in user-space test mode..."
  export BYPASS_ROOT_CHECK=1
  export LOCK_FILE_PATH="$(pwd)/tests/matt_daemon.lock"
else
  export LOCK_FILE_PATH="/var/lock/matt_daemon.lock"
fi

echo "=== Building project ==="
make re

if [ ! -f Matt_daemon ]; then
  echo "Error: Compilation failed"
  exit 1
fi

echo "=== Testing lock file at $LOCK_FILE_PATH ==="

# Make sure lock file does not exist initially
rm -f "$LOCK_FILE_PATH"

# Start the first instance in the background
echo "Starting first instance in the background..."
./Matt_daemon > /dev/null &
PID1=$!

# Wait a moment for it to initialize and write the lock file
sleep 1

# Check if lock file exists
if [ ! -f "$LOCK_FILE_PATH" ]; then
  echo "Error: Lock file was not created!"
  kill -9 $PID1 2>/dev/null
  exit 1
fi

echo "Lock file created successfully at $LOCK_FILE_PATH"
echo -n "Lock file content (PID): "
cat "$LOCK_FILE_PATH"

# Check if PID in the lock file matches
LOCK_PID=$(cat "$LOCK_FILE_PATH")
if [ "$LOCK_PID" -ne "$PID1" ]; then
  echo "Error: Lock file PID ($LOCK_PID) does not match background process PID ($PID1)!"
  kill -9 $PID1 2>/dev/null
  exit 1
fi
echo "Lock file PID matches background process PID ($PID1)"

# Attempt to start a second instance
echo "Attempting to start second instance..."
SECOND_OUT=$(./Matt_daemon 2>&1)
EXIT_CODE=$?

if [ $EXIT_CODE -eq 0 ]; then
  echo "Error: Second instance started successfully but should have failed!"
  kill -9 $PID1 2>/dev/null
  exit 1
fi

echo "Second instance failed as expected with exit code $EXIT_CODE"
echo "Error output of second instance:"
echo "$SECOND_OUT"

# Verify error message matches standard requirement
EXPECTED_ERR="Cant open $LOCK_FILE_PATH: File exists"
if [[ "$SECOND_OUT" != *"$EXPECTED_ERR"* ]]; then
  echo "Error: Unexpected error message! Expected it to contain: '$EXPECTED_ERR'"
  kill -9 $PID1 2>/dev/null
  exit 1
fi
echo "Error message verified: contains '$EXPECTED_ERR'"

# Terminate the first instance
echo "Terminating first instance (PID $PID1)..."
kill -9 $PID1 2>/dev/null
wait $PID1 2>/dev/null

# Clean up lock file manually if kill -9 was used (since it bypasses destructor)
rm -f "$LOCK_FILE_PATH"

echo "=== ALL TESTS PASSED SUCCESSFULLY ==="
