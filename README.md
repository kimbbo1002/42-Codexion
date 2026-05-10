_This project has been created as part of the 42 curriculum by \<bokim>._

# Codexion

## Description
Codexion simulates a group of **concurrent** coders competing for limited resources, modeled using **POSIX threads**, **mutexes**, and careful scheduling strategies.

- **Overview**: 
	A set of coders is seated in a circular co-working hub around a shared Quantum Compiler. They cycle through three activities: compiling, debugging, and refactoring. On the table lie USB dongles—one per coder. To compile quantum code, a coder must acquire two dongles **simultaneously**, one from each side (left and right).

Because resources are shared and access is synchronized, coders must coordinate to avoid conflicts and starvation. Each coder needs to compile at regular intervals; if one goes too long without compiling, they burn out. The simulation ends as soon as any coder burns out due to insufficient access to the required resources.

## Instructions
### Build
- `make`: Compile the project
- `make clean`: Remove object files
- `make fclean`: Remove object files and binaries
- `make re`: Rebuild from scratch

### Usage
The program must be executed with the following arguments:
- **number_of_coders**

  Total number of coders in the simulation (also equal to the number of dongles).

- **time_to_burnout** (milliseconds)
  
  If a coder did not start compiling within

- **time_to_burnout** (milliseconds)
  
  Maximum time a coder can go without starting a new compile.

  If this time is exceeded since their last compile (or since the simulation start), the coder burns out.

- **time_to_compile** (milliseconds)
  
  Duration of the compiling phase.

  During this time, the coder must hold two dongles.

- **time_to_debug** (milliseconds)
  
  Time spent debugging after compiling.

- **time_to_refactor** (milliseconds)
  
  Time spent refactoring after debugging.

  Once refactoring is complete, the coder immediately attempts to acquire dongles again.

- **number_of_compiles_required**
  
  The simulation stops once all coders have compiled at least this many times.

- **dongle_cooldown** (milliseconds)
  
  Time a dongle remains unavailable after being released.

- **scheduler**
  Resource arbitration policy used when multiple coders request dongles:
  - `fifo`: First-In, First-Out (requests are served in arrival order)
  - `edf`: Earliest Deadline First (priority is given to the coder closest to burnout, based on: `last_compile_start + time_to_burnout`)

### Examples
  - **Normal Case**
  ```
  ./codexion 5 2000 200 100 100 5 50 fifo
  ```
  - **Burnout Case:**
  ```
  ./codexion 5 400 300 200 200 5 50 edf
  ```
  - **Single Coder Case:**
  ```
  ./codexion 1 800 200 200 200 3 50 fifo
  ```

## Blocking Cases Handled
### Deadlock Prevention
To prevent circular wait—the classic "Dining Philosophers" deadlock where every coder holds one dongle and waits forever for another—the system implements a **Resource Hierarchy Strategy**. Instead of always grabbing the left dongle first, the compile logic compares the memory addresses of the two required mutexes and always locks the one with the smaller address first. This ensures that at least one coder can always access both resources, breaking the cycle of dependency.

### Cooldown Handling
The `dongle_cooldown` mechanism ensures that dongles are not immediately reused, simulating a post-task reset period. By calling `controlled_sleep` within the `compile` routine, the coder retains possession of the dongles for the duration of the `dongle_cooldown` specified in the configuration. This prevents "resource hogging" by a single thread and allows for a more realistic distribution of work.

### Precise Burnout Detection
The **Monitor Thread** acts as a high-priority supervisor that constantly iterates through all coders. It calculates the time elapsed since each coder's `last_compile` timestamp. If `get_time() - last_compile` exceeds the `time_burnout` limit, the monitor immediately updates a global `running` flag protected by `stop_lock` to stop all simulation activities and logs the burnout event.

### Log Serialization
To prevent "garbled" or overlapping text in the terminal, all output is funneled through the `print_action` utility. This function utilizes a dedicated `print_lock` (`pthread_mutex_t`). By locking this mutex before calling `printf` and unlocking it only after the message is sent, the system ensures that every log entry is atomic and that timestamps remain in chronological order.

### Scheduler Implementation
The simulation supports two distinct scheduling modes via the scheduler flag:
- **FIFO (1):** Threads rely on the standard OS mutex queue and an initial even/odd `usleep` offset to desynchronize.
- **EDF (0 - Earliest Deadline First):** Before attempting to compile, coders execute `yield_edf`. This function checks the deadlines of immediate neighbors; if a neighbor is closer to "burning out" than the current thread, the current thread voluntarily calls `controlled_sleep` to yield resource priority to the thread at higher risk.

## Thread Sunchronization Mechanisms
### pthread_mutex_t
The system utilizes multiple mutexes to ensure thread safety across shared memory:
- **Dongle Locks:** Individual mutexes for each dongle to prevent simultaneous access.
- **Compile Lock:** Protects per-coder data like `last_compile` and `compile_count` to prevent data races during monitor checks.
- **Stop Lock:** Synchronizes the running status, ensuring that when one thread detects a stop condition, all others see it immediately via `get_running_status`.
- **Print Lock:** Ensures logs from different threads do not collide in the standard output.

## Resources
- [pthreads(7) — Linux manual page](https://man7.org/linux/man-pages/man7/pthreads.7.html)
- [How to create and join threads in C (pthreads)](https://www.youtube.com/watch?v=uA8X5zNOGw8)
- [Introduction To Threads (pthreads) | C Programming Tutorial](https://www.youtube.com/watch?v=ldJ8WGZVXZk)

### AI Usage
- Assisted with debugging and identifying concurrency issues.
- Helped clarify core concepts such as threading, mutex synchronization, and scheduling strategies.