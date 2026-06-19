#!/usr/bin/env bash
set -e

# Works on Debian-based distributions, including Kali Linux and Ubuntu.
sudo apt update
sudo apt install -y build-essential cmake gcc g++ make libopencv-dev v4l-utils

echo "Dependencies installed successfully."
