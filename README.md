# Simple Unix-like Shell Implementation

This project is a basic implementation of a Unix-like shell in C. It provides a command-line interface where users can execute various commands, offering a lightweight alternative to full-featured shells.

## Table of Contents
1. [Features](#features)
2. [Getting Started](#getting-started)
   - [Prerequisites](#prerequisites)
   - [Compilation](#compilation)
   - [Running the Shell](#running-the-shell)
3. [Usage](#usage)
   - [Built-in Commands](#built-in-commands)
   - [Exiting the Shell](#exiting-the-shell)
4. [Implementation Details](#implementation-details)
5. [Limitations](#limitations)
6. [Contributing](#contributing)
7. [License](#license)
8. [Acknowledgments](#acknowledgments)

## Features

- Basic command execution
- Built-in commands: `cd`, `pwd`, `ls`, `echo`, `clear`
- Command history
- Custom prompt displaying username, hostname, and current directory
- Rudimentary job control
- Basic signal handling

## Getting Started

### Prerequisites

To compile and run this shell, you need:

- GCC compiler
- Unix-like operating system (Linux, macOS)

### Compilation

To compile the shell, use the following command:

#bash
gcc -o myshell main.c


##Running the Shell
After compilation, run the shell using:
bashCopy./myshell
#Usage
The shell provides a prompt in the format:
Copy<username@hostname:~current_directory>
