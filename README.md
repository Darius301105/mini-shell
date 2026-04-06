# mini-shell

A small Unix-like shell in C, used as a personal learning project to understand how shells work on Linux.

This repository was primarily a tutorial / workshop-based practice project for me. I used it to learn and experiment with concepts such as:

- reading input from the terminal
- running programs with `fork()` and `execvp()`
- waiting for child processes with `wait()`
- built-in commands like `cd` and `exit`
- pipelines
- environment variable expansion
- background execution with `&`
- signal handling with `SIGINT`

## Current features

This shell currently supports:

- external commands
- pipelines using `|`
- built-in `exit`
- built-in `cd`
- built-in `pwd`
- built-in `help`
- command history
- simple environment variable expansion (`$HOME`, `$USER`, etc.)
- `cd ~` and `cd ~/folder`
- background execution with `&`
- dynamic prompt showing the current working directory
- basic `SIGINT` handling

## Build

Compile with:

```bash
make

## Run

Run with: ./shell

## Notes

## Notes

This project started from a shell workshop skeleton and was used by me as a personal tutorial project.

I extended it step by step in order to better understand Linux processes, shell built-ins, pipes, environment variables, and signal handling.

After this learning project, my next goal is to build a simpler shell completely from scratch.
