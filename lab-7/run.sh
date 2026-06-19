#!/usr/bin/env bash
set -e

if [ ! -f build/lab7 ]; then
    echo "Executable not found. Run ./build.sh first."
    exit 1
fi

# Optional camera index argument (default 0).
# For DroidCam the virtual device is often not /dev/video0.
# Find it with: v4l2-ctl --list-devices
# Then run, for example: ./run.sh 2
./build/lab7 "$@"
