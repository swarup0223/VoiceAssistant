# VoiceAssistant

## Voice-Controlled Linux Desktop Assistant

VoiceAssistant is a **C++17 voice-controlled Linux desktop assistant** designed to demonstrate the integration of local speech recognition, natural-language command processing, command safety, Linux desktop automation, system monitoring, file management, process management, browser automation, and Linux kernel programming.

The project provides a controlled interface for interacting with selected Linux desktop operations through **keyboard or voice input**.

---

## ✨ Features

- 🎙️ Local speech recognition using **whisper.cpp**
- 🧠 Natural-language command parsing
- 🛡️ SafetyEngine for dangerous-command detection
- 🖥️ Controlled Linux application management
- 🌐 Browser opening and web search
- 🔎 Google search support
- ▶️ YouTube search support
- 🦊 Firefox WebDriver BiDi automation
- 📂 File creation, listing, and deletion
- ⚙️ Linux process management
- 📊 CPU, memory, disk, network, uptime, load, and kernel information
- 🔌 Custom Linux character device driver
- 🔄 User-space ↔ kernel-space communication
- 🔐 Destructive-action confirmation
- 🧩 Modular C++17 architecture
- 🐧 Linux system programming
- 🔧 Linux kernel module development
- 📚 Technical documentation

---

# 🏗️ System Architecture

VoiceAssistant follows a layered architecture separating voice recognition, safety validation, command interpretation, command execution, Linux operations, browser automation, and kernel communication.

```text
                         ┌───────────────────────┐
                         │         USER          │
                         │                       │
                         │  Keyboard / Voice     │
                         └───────────┬───────────┘
                                     │
                                     ▼
                         ┌───────────────────────┐
                         │      VoiceEngine      │
                         │      whisper.cpp      │
                         └───────────┬───────────┘
                                     │
                                     ▼
                         ┌───────────────────────┐
                         │     SafetyEngine      │
                         │  Security Validation  │
                         └───────────┬───────────┘
                                     │
                                     ▼
                         ┌───────────────────────┐
                         │     CommandParser     │
                         │   Intent Recognition  │
                         └───────────┬───────────┘
                                     │
                                     ▼
                         ┌───────────────────────┐
                         │    CommandExecutor    │
                         └───────────┬───────────┘
                                     │
                  ┌──────────────────┼──────────────────┐
                  │                  │                  │
                  ▼                  ▼                  ▼
          ┌──────────────┐   ┌──────────────┐   ┌───────────────┐
          │ Linux User   │   │   Browser    │   │ Device Driver │
          │ Space        │   │ Automation   │   │    Client     │
          └──────────────┘   └──────┬───────┘   └───────┬───────┘
                                    │                   │
                                    ▼                   ▼
                             Firefox WebDriver       /dev/voiceassistant
                                  BiDi                    │
                                                         ▼
                                               Linux Character Driver
                                                         │
                                                         ▼
                                                   Kernel Space
```

---

# 🔄 Command Processing Pipeline

Every command follows a controlled processing pipeline.

```text
User Input
    │
    ├────────────── Keyboard
    │
    └────────────── Voice
                     │
                     ▼
              Speech Recognition
                     │
                     ▼
              Recognized Text
                     │
                     ▼
              SafetyEngine
                     │
            ┌────────┴────────┐
            │                 │
          Unsafe             Safe
            │                 │
            ▼                 ▼
          BLOCK         CommandParser
                              │
                              ▼
                       Parsed Command
                              │
                              ▼
                       CommandExecutor
                              │
              ┌───────────────┼────────────────┐
              │               │                │
              ▼               ▼                ▼
        Linux Operations   Browser         Driver Client
                              │                │
                              ▼                ▼
                       Firefox BiDi       /dev/voiceassistant
                                               │
                                               ▼
                                          Kernel Driver
```

This separation ensures:

- Safety validation happens before execution.
- Parsing is independent from execution.
- Browser operations remain separated from system operations.
- Driver communication remains isolated from application logic.

---

# 🎙️ Voice Recognition

VoiceAssistant uses **whisper.cpp** for local speech-to-text processing.

```text
Microphone
    │
    ▼
Audio Capture
    │
    ▼
whisper.cpp
    │
    ▼
Recognized Text
    │
    ▼
SafetyEngine
    │
    ▼
CommandParser
    │
    ▼
CommandExecutor
```

The current application uses an English Whisper model.

The model is intentionally excluded from Git because Whisper model files are large.

The current runtime configuration uses:

```text
third_party/whisper.cpp/models/ggml-small.en.bin
```

---

# 🧠 Natural-Language Command Parsing

VoiceAssistant accepts different natural-language variations for supported intents.

For example:

```text
open firefox
Open Firefox
please open Firefox
launch Firefox
start Firefox
```

These can resolve to the same application-opening intent.

Supported command categories include:

```text
UNKNOWN
OPEN_APPLICATION
OPEN_URL
SEARCH_WEB
CLOSE_APPLICATION
SYSTEM_INFO
SHOW_PROCESSES
LIST_FILES
CREATE_FILE
DELETE_FILE
DISK_INFO
NETWORK_INFO
CPU_USAGE
MEMORY_USAGE
UPTIME
LOAD_AVERAGE
KERNEL_INFO
HELP
EXIT
```

---

# 🛡️ Safety Engine

VoiceAssistant validates commands before they reach the execution layer.

```text
Raw User Input
      │
      ▼
SafetyEngine
      │
      ├──────── Unsafe ────────► BLOCK
      │
      ▼
CommandParser
      │
      ▼
CommandExecutor
```

Dangerous command patterns include examples such as:

```text
rm -rf /
rm -rf /*
mkfs
dd if=
shutdown
reboot
poweroff
```

Example:

```text
User:
rm -rf /

VoiceAssistant:
VoiceAssistant Safety Engine:
This command has been blocked for security reasons.
```

The SafetyEngine is the primary application-level safety mechanism.

The Linux character device driver provides user-space ↔ kernel-space communication and is not the application's primary safety authority.

---

# 🖥️ Application Management

VoiceAssistant uses controlled application matching instead of unrestricted arbitrary application execution.

Supported application examples include:

```text
firefox
vscode
terminal
vlc
file manager
```

Examples:

```text
open firefox
open vscode
open terminal

close firefox
```

---

# 🌐 Web & Browser Commands

VoiceAssistant supports predefined web destinations:

```text
open google
open youtube
open github
open gmail
open whatsapp
open linkedin
open reddit
open chatgpt
```

Users can also open valid URLs:

```text
open https://example.com
```

Web searching:

```text
search google for Linux device drivers
search for C++ tutorials
```

YouTube search variations:

```text
search youtube for Python tutorials
search youtube Python tutorials
youtube search Python tutorials
find on youtube Python tutorials
play on youtube Python tutorials
```

Normal visible browser opening continues to use the existing Linux desktop browser mechanism.

---

# 🦊 Firefox WebDriver BiDi

VoiceAssistant includes a dedicated Firefox WebDriver BiDi client.

Source files:

```text
include/firefox_bidi_client.hpp
src/firefox_bidi_client.cpp
```

The BiDi layer supports:

- WebSocket connection
- WebDriver BiDi session creation
- Browsing-context discovery
- JavaScript evaluation
- Page navigation
- Session termination
- JSON-safe command construction

The automation layer is separate from the normal visible browser-opening functionality.

## Firefox BiDi Endpoint

The current Firefox automation endpoint is:

```text
ws://127.0.0.1:9222
```

Start Firefox for BiDi automation:

```bash
firefox --headless --no-remote \
  --remote-debugging-port 9222 \
  --remote-allow-hosts localhost \
  --remote-allow-origins http://localhost:9222 \
  -remote-allow-system-access \
  >/tmp/voiceassistant-firefox.log 2>&1 &
```

Verify the port:

```bash
ss -ltnp | grep ':9222'
```

Check the log:

```bash
cat /tmp/voiceassistant-firefox.log
```

The implemented BiDi functionality has been tested with:

```text
session.new
browsingContext.getTree
script.evaluate
browsingContext.navigate
session.end
```

Example architecture:

```text
VoiceAssistant
    │
    ▼
Firefox BiDi Client
    │
    ▼
WebSocket
    │
    ▼
Firefox
    │
    ▼
Web Page
```

---

# 📂 File Management

VoiceAssistant supports controlled file operations.

## List Files

```text
list files
```

## Create a File

```text
create file test.txt
```

## Delete a File

```text
delete file test.txt
```

Destructive file operations require confirmation.

Example:

```text
VoiceAssistant Confirmation Required
---------------------------------
Action: Delete file

Target: test.txt

Are you sure? (y/n):
```

---

# ⚙️ Process Management

VoiceAssistant can display running Linux processes.

```text
show processes
```

The ProcessManager provides the process-management functionality.

---

# 📊 System Monitoring

VoiceAssistant provides Linux system information and monitoring.

## System Information

```text
show system information
show system info
show system details
show system specifications
show system status
```

## CPU

```text
show cpu usage
```

## Memory

```text
show memory usage
```

## Disk

```text
show disk usage
show disk space
```

## Network

```text
show network information
```

## Uptime

```text
show uptime
```

## Load Average

```text
show load average
```

## Kernel Information

```text
show kernel information
```

---

# 🔌 Linux Character Device Driver

VoiceAssistant includes a custom Linux character device driver.

Driver:

```text
driver/voiceassistant_driver.c
```

Device:

```text
/dev/voiceassistant
```

The driver demonstrates:

- Linux kernel modules
- Character devices
- Major and minor numbers
- `cdev`
- File operations
- Device classes
- `device_create()`
- `copy_to_user()`
- `copy_from_user()`
- Kernel mutex synchronization
- Kernel logging
- Module initialization
- Module cleanup

---

# 🔄 User-Space ↔ Kernel-Space Communication

VoiceAssistant contains a dedicated driver client:

```text
include/device_driver_client.hpp
src/device_driver_client.cpp
```

Communication path:

```text
┌───────────────────────────────┐
│       VoiceAssistant C++           │
│          User Space           │
└───────────────┬───────────────┘
                │
                │ open()
                │ write()
                │ read()
                │ close()
                ▼
┌───────────────────────────────┐
│        /dev/voiceassistant         │
└───────────────┬───────────────┘
                │
                ▼
┌───────────────────────────────┐
│     Linux Character Driver    │
│          Kernel Space         │
└───────────────────────────────┘
```

Conceptually:

```cpp
DeviceDriverClient driver_("/dev/voiceassistant");

driver_.sendCommand("open firefox");
```

---

# 🔐 Driver Synchronization

The character driver protects shared command-buffer state using a Linux kernel mutex.

```text
Process A ──┐
Process B ──┼──► Mutex ──► Driver Buffer
Process C ──┘
```

This protects shared driver state from concurrent access.

---

# 🧱 Software Components

## VoiceEngine

Handles microphone input and speech recognition using whisper.cpp.

## SafetyEngine

Validates commands before execution.

## CommandParser

Converts natural-language input into structured command types.

## CommandExecutor

Coordinates command execution and driver communication.

## ApplicationManager

Handles controlled application launching and closing.

## BrowserManager

Handles normal desktop browser opening and web searching.

## FirefoxBidiClient

Provides Firefox WebDriver BiDi browser automation.

## FileManager

Handles supported file operations.

## ProcessManager

Provides process-related operations.

## SystemMonitor

Provides Linux system information and monitoring.

## DeviceDriverClient

Provides the user-space interface to:

```text
/dev/voiceassistant
```

## Linux Character Driver

Provides the kernel-space communication endpoint.

---

# 📁 Project Structure

```text
VoiceAssistant/
│
├── Makefile
├── README.md
├── .gitignore
│
├── build/
│
├── docs/
│   ├── architecture.md
│   └── installation.md
│
├── driver/
│   ├── Makefile
│   ├── README.md
│   └── voiceassistant_driver.c
│
├── include/
│   ├── application_manager.hpp
│   ├── browser_manager.hpp
│   ├── command_executor.hpp
│   ├── command_parser.hpp
│   ├── device_driver_client.hpp
│   ├── file_manager.hpp
│   ├── firefox_bidi_client.hpp
│   ├── process_manager.hpp
│   ├── safety_engine.hpp
│   ├── system_monitor.hpp
│   └── voice_engine.hpp
│
├── src/
│   ├── application_manager.cpp
│   ├── browser_manager.cpp
│   ├── command_executor.cpp
│   ├── command_parser.cpp
│   ├── device_driver_client.cpp
│   ├── file_manager.cpp
│   ├── firefox_bidi_client.cpp
│   ├── main.cpp
│   ├── process_manager.cpp
│   ├── safety_engine.cpp
│   ├── system_monitor.cpp
│   └── voice_engine.cpp
│
├── tests/
│
└── third_party/
    └── whisper.cpp/
```

---

# 🛠️ Technology Stack

| Technology | Purpose |
|---|---|
| **C++17** | Main application |
| **Linux** | Target operating system |
| **Linux Kernel** | Character device driver |
| **whisper.cpp** | Local speech recognition |
| **Firefox WebDriver BiDi** | Browser automation |
| **GCC / G++** | C++ compilation |
| **GNU Make** | Main build system |
| **CMake** | whisper.cpp build system |
| **Git** | Version control |
| **GitHub** | Source-code hosting |
| **WSL2** | Development environment |
| **Linux audio stack** | Voice capture |

---

# 📋 Requirements

VoiceAssistant requires a Linux environment with:

- GCC / G++
- GNU Make
- CMake
- Git
- Linux development tools
- Compatible Linux kernel development/source environment for the driver
- whisper.cpp dependencies
- Working audio input for voice functionality
- Firefox for browser automation

Verify the main tools:

```bash
uname -a
g++ --version
git --version
cmake --version
make --version
```

---

# 📥 Installation

## 1. Clone the Repository

```bash
git clone https://github.com/SRIYA-BOSE/VoiceAssistant.git
cd VoiceAssistant
```

Initialize the whisper.cpp submodule:

```bash
git submodule update --init --recursive
```

---

# 2. Build whisper.cpp

Enter the whisper.cpp directory:

```bash
cd third_party/whisper.cpp
```

Configure:

```bash
cmake -B build
```

Build:

```bash
cmake --build build -j1 --config Release
```

Return to the project root:

```bash
cd ../..
```

---

# 3. Add the Whisper Model

Place the required English Whisper model at:

```text
third_party/whisper.cpp/models/ggml-small.en.bin
```

The model is intentionally excluded from Git because of its size.

---

# 4. Build VoiceAssistant

From the project root:

```bash
make
```

The executable will be generated at:

```text
build/voiceassistant
```

For a clean rebuild:

```bash
make clean && make
```

---

# 🔧 Linux Character Driver Setup

## 1. Build the Driver

```bash
cd driver
make
```

The resulting module is:

```text
voiceassistant_driver.ko
```

Return to the project root:

```bash
cd ..
```

> The driver must be built against a compatible Linux kernel development/source environment.

## 2. Load the Driver

```bash
sudo insmod driver/voiceassistant_driver.ko
```

Verify:

```bash
lsmod | grep voiceassistant
```

Check the device:

```bash
ls -l /dev/voiceassistant
```

Check kernel messages:

```bash
dmesg | grep voiceassistant
```

## 3. Device Permissions

A udev rule can be configured as:

```text
KERNEL=="voiceassistant", MODE="0660", GROUP="users"
```

Reload the rules:

```bash
sudo udevadm control --reload-rules
sudo udevadm trigger
```

Verify:

```bash
ls -l /dev/voiceassistant
```

---

# 🦊 Firefox WebDriver BiDi Setup

Start Firefox with BiDi support:

```bash
firefox --headless --no-remote \
  --remote-debugging-port 9222 \
  --remote-allow-hosts localhost \
  --remote-allow-origins http://localhost:9222 \
  -remote-allow-system-access \
  >/tmp/voiceassistant-firefox.log 2>&1 &
```

Verify:

```bash
ss -ltnp | grep ':9222'
```

Check the Firefox log:

```bash
cat /tmp/voiceassistant-firefox.log
```

The BiDi endpoint should be available through:

```text
ws://127.0.0.1:9222
```

---

# ▶️ Running VoiceAssistant

Start the application:

```bash
./build/voiceassistant
```

VoiceAssistant presents two input modes:

```text
Select command input mode:
  1. Type a command
  2. Speak a command
```

---

# ⌨️ Keyboard Mode

Select:

```text
1
```

Then enter a command.

Examples:

```text
open firefox
open youtube
search google artificial intelligence
show system information
show cpu usage
show memory usage
list files
create file test.txt
show processes
```

---

# 🎙️ Voice Mode

Start:

```bash
./build/voiceassistant
```

Select:

```text
2
```

VoiceAssistant initializes whisper.cpp and waits for speech.

Example:

```text
Open Firefox
```

The recognized speech follows the same processing path as typed commands:

```text
Voice
  ↓
whisper.cpp
  ↓
Recognized Text
  ↓
SafetyEngine
  ↓
CommandParser
  ↓
CommandExecutor
```

---

# 🧪 Testing

## Application

```text
open firefox
```

## Browser

```text
open google
open youtube
```

## Google Search

```text
search google artificial intelligence
```

## YouTube Search

```text
search youtube python tutorial
```

## System Information

```text
show system information
show system status
```

## CPU

```text
show cpu usage
```

## Memory

```text
show memory usage
```

## Disk

```text
show disk usage
```

## Network

```text
show network information
```

## Processes

```text
show processes
```

## Files

```text
list files
```

## Create File

```text
create file test.txt
```

Verify:

```bash
ls -l test.txt
```

## Delete File

```text
delete file test.txt
```

Verify:

```bash
ls -l test.txt
```

## Help

```text
help
```

## Exit

```text
exit
```

---

# 🛡️ Safety Demonstration

Enter:

```text
rm -rf /
```

Expected:

```text
VoiceAssistant Safety Engine:
This command has been blocked for security reasons.
```

The dangerous command must not reach the execution stage.

```text
User Input
    ↓
SafetyEngine
    ↓
BLOCK
```

---

# 🔍 Driver Verification

Check whether the module is loaded:

```bash
lsmod | grep voiceassistant
```

Check the device:

```bash
ls -l /dev/voiceassistant
```

Check kernel messages:

```bash
dmesg | grep voiceassistant
```

Typical driver messages may include:

```text
voiceassistant: driver loaded
voiceassistant: device opened
voiceassistant: received command: open firefox
voiceassistant: device closed
```

These messages demonstrate communication between the VoiceAssistant application and the Linux character driver.

---

# 🎬 Evaluation Demonstration

A complete demonstration can follow this sequence.

## Step 1 — Verify the Driver

```bash
lsmod | grep voiceassistant
```

## Step 2 — Verify the Device

```bash
ls -l /dev/voiceassistant
```

## Step 3 — Start Firefox BiDi

```bash
firefox --headless --no-remote \
  --remote-debugging-port 9222 \
  --remote-allow-hosts localhost \
  --remote-allow-origins http://localhost:9222 \
  -remote-allow-system-access \
  >/tmp/voiceassistant-firefox.log 2>&1 &
```

Verify:

```bash
ss -ltnp | grep ':9222'
```

## Step 4 — Start VoiceAssistant

```bash
./build/voiceassistant
```

## Step 5 — Execute an Application Command

```text
open firefox
```

## Step 6 — Demonstrate Web Search

```text
search google artificial intelligence
```

## Step 7 — Demonstrate YouTube Search

```text
search youtube python tutorial
```

## Step 8 — Demonstrate System Monitoring

```text
show system status
```

## Step 9 — Demonstrate File Management

```text
create file demonstration.txt
list files
delete file demonstration.txt
```

## Step 10 — Demonstrate Safety

```text
rm -rf /
```

Expected:

```text
Command blocked for security reasons.
```

## Step 11 — Demonstrate Voice

Select:

```text
2
```

Then speak:

```text
Open Firefox
```

Complete pipeline:

```text
Voice
  ↓
whisper.cpp
  ↓
Recognized Text
  ↓
SafetyEngine
  ↓
CommandParser
  ↓
CommandExecutor
  ↓
Linux / Browser / Driver
```

---

# 🧠 Linux Concepts Demonstrated

## User Space

The C++ application handles:

- Voice recognition
- Command parsing
- Safety validation
- Application management
- Browser operations
- File operations
- Process management
- System monitoring
- Driver communication
- Firefox WebDriver BiDi automation

## Kernel Space

The Linux character driver handles:

- Character-device registration
- Major/minor numbers
- File operations
- Kernel buffer management
- User/kernel memory transfer
- Mutex synchronization
- Device lifecycle
- Kernel logging

---

# 🏛️ Software Architecture Concepts

VoiceAssistant demonstrates:

### Modular Architecture

Each major responsibility is implemented as an independent component.

### Separation of Concerns

Voice recognition, safety, parsing, execution, browser automation, system monitoring, file management, and driver communication are separated.

### Encapsulation

C++ classes expose focused interfaces for individual subsystems.

### Defensive Programming

Commands are validated before execution.

### Layered Architecture

```text
Input
  ↓
Recognition
  ↓
Safety
  ↓
Parsing
  ↓
Execution
  ↓
Linux / Browser / Driver
  ↓
Kernel
```

### User-Space / Kernel-Space Separation

Desktop operations remain in user space while the character driver demonstrates controlled kernel communication.

---

# 🔬 Linux Character Driver Concepts

The driver demonstrates concepts including:

```text
module_init()
module_exit()

alloc_chrdev_region()
cdev_init()
cdev_add()

class_create()
device_create()

copy_to_user()
copy_from_user()

mutex
```

File operations include:

```text
open
read
write
release
```

---

# ❓ Why a Character Device?

A Linux character device provides a standard file-like interface between user-space applications and kernel-space drivers.

VoiceAssistant uses:

```text
/dev/voiceassistant
```

The C++ application communicates with the driver through standard Linux operations:

```text
open()
read()
write()
close()
```

This creates a clear user-space ↔ kernel-space boundary suitable for demonstrating Linux systems programming concepts.

---

# 🔐 Public Repository Security

VoiceAssistant is intended to remain publicly accessible.

**Never commit sensitive information**, including:

```text
API keys
Access tokens
Passwords
Private SSH keys
Cloud credentials
.env files containing secrets
Authentication tokens
Private certificates
Personal credentials
```

Build artifacts and downloaded Whisper models should remain excluded through `.gitignore`.

Before pushing changes:

```bash
git status
git diff --cached
```

If a secret is accidentally committed:

1. Remove it from repository history.
2. Rotate the exposed credential.
3. Verify that the replacement credential is not stored in the repository.

---

# ⚠️ Current Limitations

- Application execution is controlled through predefined application matching.
- Speech recognition requires whisper.cpp and a local Whisper model.
- Voice capture depends on the Linux audio environment.
- Speech recognition quality depends on the selected model and microphone/audio conditions.
- Firefox BiDi requires Firefox to be running with the appropriate remote debugging configuration.
- The character driver is primarily a Linux kernel communication and educational component rather than a physical hardware driver.
- Driver compilation requires a compatible Linux kernel development/source environment.
- Kernel ABI changes may require rebuilding the driver.
- Some desktop applications may behave differently under WSL2/WSLg than on a native Linux desktop.
- The current system is intentionally command-driven rather than GUI-driven.

---

# 🔮 Future Improvements

Potential future improvements include:

- Wake-word detection
- Multilingual speech recognition
- Improved intent classification
- More desktop applications
- Advanced Firefox automation
- Browser tab management
- Page interaction through WebDriver BiDi
- Desktop notifications
- Persistent command history
- Improved microphone/audio handling
- Hardware microphone integration
- Driver event notifications
- `poll()` / `select()` support
- IOCTL-based driver commands
- More Linux system-management capabilities
- Additional safety policies
- Automated command-parser testing
- Unit testing
- Integration testing
- End-to-end voice testing

---

# 📊 Project Status

VoiceAssistant currently provides:

- ✅ C++17 Linux desktop assistant
- ✅ Local speech recognition
- ✅ Natural-language command parsing
- ✅ Safety validation
- ✅ Controlled application management
- ✅ Browser opening
- ✅ Google search
- ✅ YouTube search
- ✅ Firefox WebDriver BiDi client
- ✅ Firefox navigation
- ✅ JavaScript evaluation through BiDi
- ✅ File management
- ✅ Destructive-action confirmation
- ✅ Process management
- ✅ System monitoring
- ✅ CPU monitoring
- ✅ Memory monitoring
- ✅ Disk monitoring
- ✅ Network information
- ✅ Uptime information
- ✅ Load average
- ✅ Kernel information
- ✅ Custom Linux character device driver
- ✅ `/dev/voiceassistant` device interface
- ✅ User-space ↔ kernel-space communication
- ✅ Kernel synchronization using mutex
- ✅ Modular C++ architecture
- ✅ Git-based development workflow

---

# 📚 Documentation

Additional documentation:

```text
docs/
├── architecture.md
└── installation.md
```

Driver documentation:

```text
driver/
└── README.md
```

---

# 🌐 Repository

GitHub:

https://github.com/SRIYA-BOSE/VoiceAssistant

---

# 👨‍💻 Project Summary

## VoiceAssistant — Voice-Controlled Linux Desktop Assistant

VoiceAssistant demonstrates the integration of:

```text
C++17
   +
Linux System Programming
   +
Local Speech Recognition
   +
Natural-Language Command Processing
   +
Command Safety
   +
Linux Desktop Automation
   +
Firefox WebDriver BiDi
   +
Linux Character Device Driver
   +
User-Space / Kernel-Space Communication
```

The project demonstrates how a modern voice interface can be combined with:

- C++ software architecture
- Linux system programming
- Local AI-powered speech recognition
- Browser automation
- Defensive command execution
- Linux kernel programming

VoiceAssistant is designed as an academic, systems-programming, and demonstration project showing the interaction between a modern C++ application and Linux operating-system facilities.

---

# 📄 License

This project is intended for academic, educational, research, and demonstration purposes.
```
