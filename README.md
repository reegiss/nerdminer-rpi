# NerdMiner for Raspberry Pi

[![C/C++ CI](https://github.com/reegiss/nerdminer-rpi/actions/workflows/c-cpp.yml/badge.svg)](https://github.com/reegiss/nerdminer-rpi/actions/workflows/c-cpp.yml)
[![Version](https://img.shields.io/badge/version-0.1.0-blue.svg)](include/nerdminer/version.h)
[![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)](LICENSE)
[![Platform](https://img.shields.io/badge/platform-Raspberry%20Pi%204-red.svg)](https://www.raspberrypi.com/)
[![C++17](https://img.shields.io/badge/C%2B%2B-17-blue.svg)](https://en.cppreference.com/w/cpp/17)

> A lightweight, educational Bitcoin solo miner designed to run on Raspberry Pi 4,
> implementing the [Stratum v1](https://en.bitcoin.it/wiki/Stratum_mining_protocol) protocol.

---

## Table of Contents

- [About](#about)
- [Features](#features)
- [Requirements](#requirements)
- [Installation](#installation)
- [Build](#build)
- [Configuration](#configuration)
- [Usage](#usage)
- [Project Structure](#project-structure)
- [Contributing](#contributing)
- [License](#license)

---

## About

**nerdminer-rpi** is an open-source, single-node Bitcoin miner written in C++17, built for
the Raspberry Pi 4. It connects to any standard Stratum-compatible mining pool, receives work
via `mining.notify`, computes SHA-256d (double SHA-256) hashes across multiple CPU threads,
and submits valid shares back to the pool.

This project is primarily **educational** — it demonstrates how the Bitcoin mining pipeline
works end-to-end: Stratum communication, coinbase construction, Merkle root calculation, block
header serialisation, and proof-of-work verification.

> ⚠️ **Disclaimer:** CPU mining on a Raspberry Pi will not generate significant revenue.
> The expected hash rate is far below what modern ASIC miners produce.
> This project exists for learning and experimentation.

---

## Features

- 🔗 **Stratum v1 client** — subscribe, authorise, and submit shares
- 🔨 **Multi-threaded mining** — one thread per CPU core (auto-detected via `std::thread::hardware_concurrency`)
- 🔐 **SHA-256d** — double SHA-256 implemented with OpenSSL
- 📦 **Coinbase & Merkle root construction** — full block header assembly from pool data
- 📊 **Hashrate reporting** — periodic H/s output per mining session
- 🏗️ **CMake + Makefile builds** — easy integration with standard Linux toolchains
- 🍓 **Raspberry Pi 4 optimised** — `-O3 -march=armv8-a` flags on `aarch64`

---

## Requirements

### Hardware

| Component | Minimum |
|-----------|---------|
| Raspberry Pi | Model 4 (4 × Cortex-A72 cores) |
| RAM | 2 GB |
| Storage | 8 GB SD card (Raspberry Pi OS) |
| Network | Wired Ethernet (recommended) or Wi-Fi |

### Software

| Dependency | Version | Notes |
|------------|---------|-------|
| g++ / GCC | ≥ 9.0 | C++17 support required |
| CMake | ≥ 3.10 | Or use the provided Makefile |
| Boost.Asio | ≥ 1.74 | Networking (`libboost-all-dev`) |
| OpenSSL | ≥ 1.1 | SHA-256d (`libssl-dev`) |
| nlohmann/json | ≥ 3.10 | JSON parsing (`nlohmann-json3-dev`) |

---

## Installation

### 1. Clone the repository

```bash
git clone https://github.com/reegiss/nerdminer-rpi.git
cd nerdminer-rpi
```

### 2. Install dependencies

```bash
sudo apt update
sudo apt install -y \
    build-essential \
    cmake \
    libboost-all-dev \
    libssl-dev \
    nlohmann-json3-dev
```

> A convenience script is also provided:
> ```bash
> bash scripts/install_deps.sh
> ```

---

## Build

### Using CMake (recommended)

```bash
mkdir -p build && cd build
cmake ..
cmake --build .
```

The binary is placed at `bin/nerdminer`.

### Using Make

```bash
make
```

The binary is placed at `./nerdminer-rpi`.

### Raspberry Pi cross-compilation

When CMake detects `aarch64` (`CMAKE_SYSTEM_PROCESSOR`), it automatically applies:

```
-O3 -march=armv8-a
```

No extra flags are required when building natively on a Raspberry Pi 4.

---

## Configuration

Pool credentials are currently set in `src/main.cpp`:

```cpp
const std::string host     = "public-pool.io";
const uint16_t    port     = 21496;
const std::string user     = "<your-bitcoin-address>.<worker-name>";
const std::string password = "x";
```

| Field | Description |
|-------|-------------|
| `host` | Stratum pool hostname |
| `port` | Stratum port (TCP) |
| `user` | Your Bitcoin address + optional worker name separated by `.` |
| `password` | Typically `x` for most pools |

> **Tip:** Replace the hard-coded credentials with your own Bitcoin address and preferred pool
> before building.

---

## Usage

**Makefile build** (`./nerdminer-rpi`):

```
./nerdminer-rpi [options]

Options:
  -h, --help    Show this help message and exit
```

**CMake build** (`bin/nerdminer`):

```
bin/nerdminer [options]

Options:
  -h, --help    Show this help message and exit
```

Example run:

```
$ ./nerdminer-rpi        # Makefile build
$ bin/nerdminer          # CMake build
====================================
      nerdminer-rpi - v0.1.0
      Raspberry Pi 4 Bitcoin Miner
====================================
Detected 4 CPU cores. Starting 4 mining threads.
Connecting to pool server...
Starting mining threads...
Thread 0 started mining loop.
...
Hashrate: 12345.67 H/s
```

For more details see [docs/usage.md](docs/usage.md).

---

## Project Structure

```
nerdminer-rpi/
├── .github/
│   └── workflows/
│       └── c-cpp.yml          # CI pipeline
├── docs/
│   └── usage.md               # Extended usage documentation
├── include/
│   └── nerdminer/
│       ├── miner_job.h        # MiningJob struct & helpers
│       ├── miner_session.h    # MinerSession (orchestrator)
│       ├── nerdminer_block.h  # Block header & SHA-256d helpers
│       ├── stratum_client.h   # Stratum TCP client
│       └── version.h          # Project version constants
├── scripts/
│   └── install_deps.sh        # Dependency installer
├── src/
│   ├── main.cpp               # Entry point
│   ├── miner_job.cpp          # Job parsing (mining.notify)
│   ├── miner_session.cpp      # Thread management & mining loop
│   ├── nerdminer_block.cpp    # SHA-256d, block header, Merkle root
│   └── stratum/
│       └── stratum_client.cpp # TCP connection, send/receive
├── tests/
│   ├── CMakeLists.txt
│   └── test_main.cpp          # Unit test entry point
├── CMakeLists.txt
├── LICENSE                    # MIT License
├── makefile
└── README.md
```

---

## Contributing

Contributions are welcome! Please follow these steps:

1. Fork the repository
2. Create a feature branch: `git checkout -b feature/your-feature`
3. Commit your changes: `git commit -m "feat: add your feature"`
4. Push to your fork: `git push origin feature/your-feature`
5. Open a Pull Request against `main`

Please make sure your code:
- Compiles without warnings (`-Wall -Wextra`)
- Follows the existing C++17 code style
- Includes relevant test updates where applicable
- Uses English for all new user-facing messages and code comments

---

## License

This project is licensed under the **MIT License** — see the [LICENSE](LICENSE) file for details.

© 2025 [Regis Araujo Melo](https://github.com/reegiss)