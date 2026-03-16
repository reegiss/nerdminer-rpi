# Usage Guide — nerdminer-rpi

This document covers advanced usage, runtime output interpretation, and troubleshooting tips
for **nerdminer-rpi**.

---

## Table of Contents

- [Starting the miner](#starting-the-miner)
- [Understanding the output](#understanding-the-output)
- [Pool configuration](#pool-configuration)
- [Adjusting thread count](#adjusting-thread-count)
- [Monitoring hashrate](#monitoring-hashrate)
- [Troubleshooting](#troubleshooting)

---

## Starting the miner

After building the binary (see [README.md](../README.md#build)), run:

**Makefile build:**
```bash
./nerdminer-rpi
```

**CMake build:**
```bash
bin/nerdminer
```

To display help:

```bash
./nerdminer-rpi --help   # Makefile build
bin/nerdminer --help     # CMake build
```

---

## Understanding the output

```
====================================
      nerdminer-rpi - v0.1.0
      Raspberry Pi 4 Bitcoin Miner
====================================
Detected 4 CPU cores. Starting 4 mining threads.
Connecting to pool server...
Starting mining threads...
Thread 0 started mining loop.
Thread 1 started mining loop.
Thread 2 started mining loop.
Thread 3 started mining loop.
Response: {"id":0,"result":[[...]],"error":null}
Current job: abc123def456
Hashrate: 14200.00 H/s
```

| Line | Meaning |
|------|---------|
| `Detected N CPU cores` | Number of threads that will run in parallel |
| `Connecting to pool server...` | TCP connection to the configured Stratum pool |
| `Response: {...}` | Raw JSON response from the pool (subscribe/authorize) |
| `Current job: <id>` | A new `mining.notify` was received; mining restarts on this job |
| `Hashrate: X H/s` | Combined hash rate across all threads, reported every 5 seconds |
| `[*] Share accepted!` | The pool accepted the submitted share |
| `[!] Share rejected!` | The pool rejected the share (stale, low-difficulty, etc.) |

---

## Pool configuration

Edit the credentials in `src/main.cpp` before building:

```cpp
const std::string host     = "public-pool.io";   // pool hostname
const uint16_t    port     = 21496;               // Stratum port
const std::string user     = "<btc-address>.<worker>"; // e.g. bc1q...xyz.raspi4
const std::string password = "x";                // usually "x"
```

Popular public pools that support Stratum v1 solo mining:

| Pool | Host | Port |
|------|------|------|
| public-pool.io (solo) | `public-pool.io` | `21496` |
| ckpool (solo) | `solo.ckpool.org` | `3333` |

Rebuild the binary after changing credentials.

---

## Adjusting thread count

By default the miner uses all available hardware threads
(`std::thread::hardware_concurrency()`). On a Raspberry Pi 4 this equals **4**.

To override, modify the relevant line in `src/miner_session.cpp`:

```cpp
numThreads_ = std::thread::hardware_concurrency();
// Change to a fixed value, e.g.:
// numThreads_ = 2;
```

Reducing the thread count lowers CPU and thermal load at the cost of hash rate.

---

## Monitoring hashrate

Hashrate is printed to `stdout` every **5 seconds**:

```
Hashrate: 14200.00 H/s
```

To redirect miner output to a file for later analysis:

```bash
./nerdminer-rpi 2>&1 | tee miner.log   # Makefile build
bin/nerdminer 2>&1 | tee miner.log     # CMake build
```

---

## Troubleshooting

### Connection refused / Read error

- Verify the pool hostname and port are correct and reachable from your network.
- Test with: `nc -zv public-pool.io 21496`

### No jobs received after subscribing

- Some pools require a valid Bitcoin address in the worker field.
- Ensure `user` is set to `<valid-btc-address>.<worker-name>`.

### Low or zero hashrate

- Ensure the binary was compiled with optimization flags (`-O3`).
- Check CPU governor: `cat /sys/devices/system/cpu/cpu0/cpufreq/scaling_governor`  
- Set to performance mode: `sudo cpufreq-set -g performance`

### Build errors — missing headers

Run the dependency installer script:

```bash
bash scripts/install_deps.sh
```

Or install manually:

```bash
sudo apt install -y build-essential cmake libboost-all-dev libssl-dev nlohmann-json3-dev
```
