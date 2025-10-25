# dsa-assignment-2
## Overview

A Linux-based packet monitoring tool that captures, analyzes, and replays network traffic using custom Stack and Queue implementations. Parses Ethernet, IPv4, IPv6, TCP, and UDP protocols.

---

## Prerequisites

- **OS**: Linux (Ubuntu/Debian/Fedora)
- **Access**: sudo/root privileges
- **Compiler**: g++ (version 4.8+)
- **Network**: Active interface (eth0, wlan0, etc.)

---

## Installation & Execution

### Build Program
```bash
# Navigate to project folder
cd NetworkMonitor

# Compile
g++ -std=c++11 -pthread -o network_monitor main.cpp -Wall

# Verify compilation
ls -la network_monitor
```

### Run Program
```bash
# IMPORTANT: Must use sudo
sudo ./network_monitor

# Enter your network interface when prompted
# Example: wlan0, eth0, enp0s3
```

---

## Finding Your Network Interface

```bash
# List all interfaces
ip link show

# Common names:
# wlan0   → WiFi
# eth0    → Ethernet
# enp0s3  → VirtualBox
# ens33   → VMware
```

---

## Program Features

| Option | Function | Description |
|--------|----------|-------------|
| **1** | Capture | Records packets continuously (default 60s) |
| **2** | Display | Shows packet list with IDs, IPs, timestamps |
| **3** | Details | Displays dissected protocol layers |
| **4** | Filter | Selects packets by source/destination IP |
| **5** | Summary | Shows filtered packets with delays |
| **6** | Replay | Resends filtered packets with timing |
| **7** | Retry | Re-attempts failed packets (max 2 tries) |
| **8** | Stats | Displays system statistics |
| **0** | Exit | Closes program |

---

## Basic Usage Workflow

```
1. Start program → sudo ./network_monitor
2. Enter interface → enp0s3
3. Capture packets → Option 1 (60 seconds)
4. View captured → Option 2
5. Filter traffic → Option 4 (enter source & dest IPs)
6. Replay packets → Option 6
```

---

## Example Session

```bash
$ sudo ./network_monitor

>> Network interface: wlan0

    Select option: 1
    >> Capture duration: 30
    [Captures 30 seconds of traffic]

    Select option: 2
    [Displays all captured packets]
    
    Select option: 4
    >> Source IP: 192.168.1.100
    >> Destination IP: 8.8.8.8
    [Filters matching packets]
    
    Select option: 6
    [Replays filtered packets]
```

---



## Quick Commands

```bash
# Build
g++ -std=c++11 -pthread -o network_monitor main.cpp

# Run
sudo ./network_monitor

# Clean
rm network_monitor

# Check interfaces
ip link show

# Generate traffic (for testing)
ping google.com
curl google.com
nslookup google.com
```

---

## Build Options

### Standard Build
```bash
g++ -std=c++11 -pthread -o network_monitor main.cpp
```

### With Optimization
```bash
g++ -std=c++11 -pthread -O2 -o network_monitor main.cpp
```

### Debug Build
```bash
g++ -std=c++11 -pthread -g -o network_monitor main.cpp
gdb ./network_monitor
```

---

