# Custom Shell Program in C++

## Introduction

This is a custom shell program implemented in C++. It provides a command-line interface for users to execute standard terminal commands along with custom implementations of essential shell features. The shell supports background processes, signal handling, and command history management.

## Features

- **User-Friendly Terminal Prompt**: Displays the username and hostname.
- **Directory Management**: Maintains a custom terminal home directory and relative paths.
- **Command Execution**: Executes standard terminal commands and handles background processes.
- **Signal Handling**:
  - `CTRL-Z`: Pushes a running foreground job into the background (changes state to stopped).
  - `CTRL-C`: Interrupts any running foreground job by sending the `SIGINT` signal.
  - `CTRL-D`: Logs out of the custom shell without affecting the actual terminal.
- **Command History**: Records the last 20 commands executed by the user and allows retrieval of recent commands.

## File Structure

### main.cpp
- Contains the terminal prompt display, including username and hostname.
- Manages the custom terminal home directory and relative paths.
- Implements execution of other terminal commands, including background process handling.
- Manages signal handling for `CTRL-Z`, `CTRL-C`, and `CTRL-D`.
- Calls corresponding custom functions based on user input.

### cd.cpp
- Implements the `cd` command using the `chdir()` system call.
- Implements the `pwd` command to print the current directory relative to the custom terminal.
- Implements the `echo` command to print content after the first non-space character.

### ls.cpp
- Implements `ls -l` and `ls -a` commands.
- Calls the appropriate function based on user-provided flags.
- Utilizes `stat`, `getpwuid()`, and `getgrgid()` to print detailed file information in long format.

### pinfo.cpp
- Checks if a program is running in the foreground or background by comparing the PID.
- Reads status, memory, and executable path from `/proc/pid/status` and `/proc/pid/exe`.
- Displays a `+` next to the status for foreground processes.
- Contains a search function to check if a process name exists within directories.

### ioredirect.cpp
- Handles input and output redirection based on symbols provided by the user.
- Creates or opens files for reading/writing and redirects file descriptors.
- Uses `execvp()` to implement commands with redirection.

### mypipe.cpp
- Implements piping through the `check_pipe()` function, managing input and output file descriptors.
- Handles commands with both piping and redirection.

### history.cpp
- Records each user command in a file named `maintain_history.txt`, maintaining the last 20 commands.
- Displays the last 10 commands when `history` is called, or a user-specified number if less than 20.

### others.cpp
- Contains utility functions for string tokenization based on delimiters.
- Checks if a command is a background process.
- Implements file search for command autocompletion within the current directory.

## Usage

1. **Compile the Program**: Open your terminal and run the following command to compile the code:
   ```bash
   make main
1. **Run the Program**: Execute the compiled program with
   ```bash
   ./a.out
