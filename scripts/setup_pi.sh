#!/bin/bash
# setup_pi.sh — One-shot setup for the RTES training course
# Run on a fresh Raspbian (Bullseye/Bookworm) install
set -e

echo "============================================="
echo " RTES Training — Raspberry Pi Setup"
echo "============================================="

echo ""
echo "[1/5] Updating package lists..."
sudo apt update

echo ""
echo "[2/5] Installing build tools (gcc, make, git, cmake)..."
sudo apt install -y build-essential git cmake

echo ""
echo "[3/5] Installing POSIX threads dev headers..."
sudo apt install -y libpthread-stubs0-dev

echo ""
echo "[4/5] Installing camera tools (v4l-utils, fswebcam, ffmpeg)..."
sudo apt install -y v4l-utils fswebcam ffmpeg

echo ""
echo "[5/5] Installing OpenCV C/C++ development libraries..."
sudo apt install -y opencv-data libopencv-dev python3-opencv

echo ""
echo "[+] Installing stress testing and tracing tools..."
sudo apt install -y stress htop trace-cmd

echo ""
echo "============================================="
echo " Setup complete!"
echo "============================================="
echo ""
echo "Verify your environment:"
echo "  gcc --version"
echo "  pkg-config --modversion opencv4"
echo "  v4l2-ctl --list-devices"
echo ""
echo "Quick RT check — compile and run:"
echo "  cd day03_pi_setup && make && sudo ./test_rt"
echo ""
