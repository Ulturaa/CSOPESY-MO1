========================================================================

 1. PREREQUISITES & COMPILATION

========================================================================



Ensure you have a C++11 compliant compiler installed (such as GCC/MinGW-w64).



To compile the application into a single executable binary, run the following

command from your PowerShell or terminal in the project directory:



  g++ -std=c++11 -pthread main.cpp FCFSScheduler.cpp RoundRobinScheduler.cpp SystemEmulator.cpp -o csopesy.exe -static



To run the program:



  .\csopesy.exe





========================================================================

 2. CONFIGURATION (`config.txt`)

========================================================================



The emulator relies on `config.txt` in the root directory for initial system setup.



Sample Parameters:

------------------------------------------------------------------------

num-cpu 4               # Number of CPU cores available [1 to 128]

scheduler "rr"          # Scheduling algorithm: "rr" (Round Robin) or "fcfs"

quantum-cycles 5        # Time quantum per core (only applies to Round Robin)

batch-process-freq 5    # CPU cycle interval for background task generation

min-ins 500             # Minimum simulated instruction lines per process

max-ins 1000            # Maximum simulated instruction lines per process

delays-per-exec 50      # Execution delay in ticks (set > 0 to watch live context switching)

------------------------------------------------------------------------





========================================================================

 3. CLI COMMANDS REFERENCE

========================================================================



--- Main Menu Console (`root:\>`) ---



- `initialize`         : Reads `config.txt`, sets up CPU core worker threads, and unlocks CLI features.

- `scheduler-start`    : Starts the background process generator to continuously spawn batch processes.

- `scheduler-stop`     : Pauses the background process generator.

- `screen -ls`         : Displays real-time CPU utilization %, active cores vs available cores, running process details, and finished process logs.

- `screen -s <name>`   : Spawns a new custom process with `<name>` and immediately attaches to its process screen.

- `screen -r <name>`   : Attaches to an existing active or running background process named `<name>`.

- `report-util`        : Writes the current CPU utilization statistics and all process history to `csopesy-log.txt`.

- `exit`               : Gracefully stops all worker threads and closes the emulator.



--- Process Sub-Shell View (`root:\>` inside screen) ---



- `process-smi`        : Refreshes and displays current process logs, current line execution progress, core ID, and timestamped print outputs.

- `exit`               : Detaches from the sub-shell view and returns to the main menu console.





========================================================================

 4. STEP-BY-STEP VERIFICATION & DEMO GUIDE

========================================================================



Follow these exact testing procedures to verify system features and algorithm correctness:



--- Test 1: System Gatekeeper Check ---

1. Run `.\csopesy.exe`.

2. Type `screen -ls` before initializing.

   * Expected Result: The system will block execution and prompt: "Please initialize the application first using 'initialize'."



--- Test 2: Preemptive Scheduling (Round Robin) ---

1. Open `config.txt` and set: `scheduler "rr"` and `delays-per-exec 50`.

2. Run `.\csopesy.exe` -> Type `initialize`.

3. Type `scheduler-start` -> Wait a few seconds -> Type `screen -ls`.

4. Pick an active process (e.g., `p03`) and attach to it: `screen -r p03`.

5. Type `process-smi` multiple times.

   * Expected Proof: Look at the `Core:<id>` column in the logs. You will see `p03` hopping across different CPU cores (e.g., Core 1 -> Core 2 -> Core 0 -> Core 3) as its time quantum expires and it is preempted.

6. Type `exit` to detach.



--- Test 3: Non-Preemptive Scheduling (FCFS) ---

1. Open `config.txt` and set: `scheduler "fcfs"` and `delays-per-exec 50`.

2. Run `.\csopesy.exe` -> Type `initialize`.

3. Type `scheduler-start` -> Type `screen -ls`.

4. Pick an active process (e.g., `p01`) and attach to it: `screen -r p01`.

5. Type `process-smi` multiple times.

   * Expected Proof: Every log entry for `p01` will show execution locked strictly to a single core (e.g., `Core:0`) from start to finish without any core migration.

6. Type `exit` to detach.



--- Test 4: Error Handling ---

1. From the main menu, try attaching to a non-existent process: `screen -r fake_proc`.

   * Expected Result: Output prints "Process fake_proc not found." without crashing the console.



--- Test 5: Report Log Generation ---

1. Type `scheduler-stop` to pause process spawning.

2. Type `report-util`.

   * Expected Result: Output prints "Report generated at C:/csopesy-log.txt!" and writes system execution history to `csopesy-log.txt`.

3. Type `exit` to exit cleanly.
