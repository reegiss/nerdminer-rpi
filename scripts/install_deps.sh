#!/usr/bin/env bash
# install_deps.sh — Install build dependencies for nerdminer-rpi
#
# Supported: Raspberry Pi OS (Debian/Ubuntu-based)
# Usage: bash scripts/install_deps.sh

set -euo pipefail

echo "[*] Updating package lists..."
sudo apt update

echo "[*] Installing build tools..."
sudo apt install -y build-essential cmake

echo "[*] Installing Boost (Asio, system)..."
sudo apt install -y libboost-all-dev

echo "[*] Installing OpenSSL development headers..."
sudo apt install -y libssl-dev

echo "[*] Installing nlohmann/json..."
sudo apt install -y nlohmann-json3-dev

echo "[✓] All dependencies installed successfully."
