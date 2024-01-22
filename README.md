# Shell built with C interacting with Unix process API

# Introduction

Smallsh is a simple Linux shell designed to handle commands by creating new processes. It provides support for input and output redirection, as well as the ability to
run commands in both foreground and background. Additionally, Smallsh includes custom signal handling for SIGINT (interrupt) to gracefully kill child foreground processes
and SIGTSTP (stop) for disabling and reenabling background processes. The ampersand (&) is ignored when running commands.

# Features

* Command execution in foreground and background.
* Input and output redirection.
* Custom signal handling for graceful termination (SIGINT) and background process suspension/resumption (SIGTSTP).
* Ignoring the ampersand (&) for background processes.

# Compilation

To compile Smallsh, use the following command with gcc:
gcc -o smallsh smallsh.c

# Usage

To run Smallsh, use the following command:
./smallsh

# Command Syntax

The syntax for running commands in Smallsh is similar to other Linux shells. You can execute commands with or without arguments, redirect input and output, and run commands in 
the background.

# Foreground Process Termination (Custom SIGINT Handling):

Press Ctrl + C to gracefully terminate the foreground process.

# Background Process Suspension/Resumption (Custom SIGTSTP Handling):

Press Ctrl + Z to suspend or resume background processes.
