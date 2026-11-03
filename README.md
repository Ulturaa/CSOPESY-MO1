# 🖥️ CSOPESY OS Emulator & Process Scheduler

![C++11](https://img.shields.io/badge/Language-C%2B%2B11-blue.svg)
![Threading](https://img.shields.io/badge/Concurrency-POSIX_Threads-green.svg)
![License](https://img.shields.io/badge/Academic-CSOPESY_Project-orange.svg)

A multi-threaded **Operating System Emulator** and **Process Scheduler CLI** written in C++11. This application simulates CPU core worker pools, dynamic dummy process instruction execution, preemptive (Round Robin) and non-preemptive (FCFS) scheduling algorithms, and Linux-style `screen` session multiplexing.

---

## 📌 Table of Contents
1. [Overview](#-overview)
2. [Features](#-features)
3. [Compilation & Execution](#-compilation--execution)
4. [Configuration (`config.txt`)](#-configuration-configtxt)
5. [CLI Commands Reference](#-cli-commands-reference)
6. [Step-by-Step Verification & Testing Guide](#-step-by-step-verification--testing-guide)
7. [Repository File Structure](#-repository-file-structure)
8. [Authors](#-authors)

---

## ⚙️ Overview

The **CSOPESY Emulator** bridges lower-level multi-threading with operating system scheduling concepts:
* **Hardware Simulation**: Configures CPU worker threads running concurrently based on user specifications.
* **Process Engine**: Evaluates instruction lines (`PRINT`, `DECLARE`, `ADD`, `SUBTRACT`, `SLEEP`, `FOR` loops) while keeping process memory bounded and safe.
* **Screen Multiplexing**: Emulates the Linux `screen` command, allowing users to spawn, inspect, attach, and detach process sub-shells cleanly.

---

## ✨ Features

- 🔒 **System Gatekeeper**: Blocks execution of CLI tasks until `initialize` reads configuration files.
- 🔄 **Dual Scheduling Engines**:
  - **First-Come, First-Served (FCFS)**: Non-preemptive execution where processes hold assigned cores until termination.
  - **Round-Robin (RR)**: Preemptive time-sliced execution that context-switches tasks across CPU worker threads after a set quantum.
- 📺 **Session Multiplexer**: Sub-shell navigation for active processes (`screen -s`, `screen -r`, `process-smi`).
- 📊 **Utilization Reporting**: Real-time stats display (`screen -ls`) and automated log file export (`report-util`).

---

## 🛠️ Compilation & Execution

### Prerequisites
* A C++11 compliant compiler (`g++` / MinGW-w64 on Windows or GCC on Linux).
* POSIX threads support (`-pthread`).

### Build Command

Run the following command in your PowerShell or terminal from the root directory:

```bash
g++ -std=c++11 -pthread main.cpp FCFSScheduler.cpp RoundRobinScheduler.cpp SystemEmulator.cpp -o csopesy.exe -static
```

### Running the Emulator

```powershell
.\csopesy.exe
```

---

## ⚙️ Configuration (`config.txt`)

The emulator reads `config.txt` upon running `initialize`. You can tweak system behaviors directly in this file:

```text
num-cpu 4
scheduler "rr"
quantum-cycles 5
batch-process-freq 5
min-ins 500
max-ins 1000
delays-per-exec 50
```

| Parameter | Type | Range | Description |
| :--- | :--- | :--- | :--- |
| `num-cpu` | `int` | `[1, 128]` | Total CPU worker threads allocated. |
| `scheduler` | `string` | `"fcfs"` / `"rr"` | Active scheduling algorithm. |
| `quantum-cycles` | `uint32` | `[1, 2^32]` | Quantum cycle time-slice allocated per process in Round Robin mode. |
| `batch-process-freq` | `uint32` | `[1, 2^32]` | CPU cycle interval for spawning batch processes in `scheduler-start`. |
| `min-ins` / `max-ins` | `uint32` | `[1, 2^32]` | Min/Max instruction line length generated per process. |
| `delays-per-exec` | `uint32` | `[0, 2^32]` | Execution delay per instruction line (set `> 0` to observe live context switching). |

---

## 💻 CLI Commands Reference

### Main Console Menu (`root:\>`)

| Command | Action |
| :--- | :--- |
| `initialize` | Loads `config.txt`, provisions CPU threads, and unlocks CLI commands. |
| `scheduler-start` | Starts the background thread generating batch processes (`p01`, `p02`, ...). |
| `scheduler-stop` | Stops batch process generation. |
| `screen -ls` | Displays live CPU core utilization %, cores used/available, running processes, and finished processes. |
| `screen -s <name>` | Spawns a custom process with `<name>` and immediately attaches to its sub-shell. |
| `screen -r <name>` | Attaches to a running process console named `<name>`. |
| `report-util` | Generates and exports full CPU utilization & process history to `csopesy-log.txt`. |
| `exit` | Gracefully shuts down CPU worker threads and closes the emulator. |

### Process Sub-Shell View (inside `screen`)

| Command | Action |
| :--- | :--- |
| `process-smi` | Prints process ID, total instruction lines, current execution line, and timestamped log outputs. |
| `exit` | Detachees from the current process screen back to the main console menu. |

---

## 🧪 Step-by-Step Verification & Testing Guide

Share this sequence with peer reviewers to demonstrate system features:

### 1. Gatekeeper Verification
1. Launch `.\csopesy.exe`.
2. Type `screen -ls` before initializing.
3. **Result**: System blocks access with `"Please initialize the application first using 'initialize'."`

### 2. Testing Round Robin (Preemptive Scheduling)
1. Open `config.txt` and verify parameters: `scheduler "rr"` and `delays-per-exec 50`.
2. Launch `.\csopesy.exe` $\rightarrow$ Run `initialize` $\rightarrow$ Run `scheduler-start`.
3. Wait 3 seconds, then run `screen -ls` to pick an active process (e.g., `p03`).
4. Re-attach to the process: `screen -r p03`.
5. Enter `process-smi` multiple times every few seconds.
   * 🎯 **Proof of Preemption**: Look at the `Core:<id>` column in the log entries. You will see `p03` migrating across multiple cores (e.g., `Core:1` $\rightarrow$ `Core:2` $\rightarrow$ `Core:0` $\rightarrow$ `Core:3`) as its quantum time expires.
6. Type `exit` to return to the main menu.

### 3. Testing FCFS (Non-Preemptive Scheduling)
1. Open `config.txt` and change the scheduler to: `scheduler "fcfs"` and `delays-per-exec 50`.
2. Launch `.\csopesy.exe` $\rightarrow$ Run `initialize` $\rightarrow$ Run `scheduler-start`.
3. Run `screen -ls` to pick an active process (e.g., `p01`).
4. Re-attach to the process: `screen -r p01`.
5. Enter `process-smi` multiple times.
   * 🎯 **Proof of Non-Preemption**: Every log entry for `p01` remains locked to a single core (e.g., `Core:0`) without migrating to any other core.
6. Type `exit` to return to the main menu.

### 4. Error Handling & Report Generation
1. Try attaching to a non-existent process: `screen -r non_existent`.
   * **Result**: Displays `"Process non_existent not found."` without crashing.
2. Stop process generation: `scheduler-stop`.
3. Save metrics: `report-util`.
   * **Result**: Generates a summary report in `csopesy-log.txt`.
4. Close application: `exit`.

---

## 📁 Repository File Structure

```text
├── FCFSScheduler.cpp       # FCFS scheduling queue implementation
├── FCFSScheduler.h
├── RoundRobinScheduler.cpp  # Round Robin scheduling queue & quantum preemption
├── RoundRobinScheduler.h
├── Scheduler.cpp           # Abstract scheduler base interface
├── Scheduler.h
├── SystemEmulator.cpp      # CLI interpreter, core manager, & screen multiplexer
├── SystemEmulator.h
├── Process.cpp             # Process state & dynamic instruction execution logic
├── Process.h
├── main.cpp                # Application entry point
├── config.h                # Structs for configuration parameters
├── instruction.h           # Instruction types & safe instruction generator
├── config.txt              # Active runtime configurations
└── README.md               # Documentation
```

---
