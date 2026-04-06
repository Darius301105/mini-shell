# mini-shell

A simple Unix-like shell written in C, built as part of a shell workshop and extended with additional functionality.

## Features

This shell currently supports:

- running external commands
- pipelines using `|`
- input/output redirection support from the workshop skeleton
- built-in `exit`
- built-in `cd`
- built-in `help`
- built-in `pwd`
- ignoring empty input lines
- dynamic prompt showing the current working directory

## Built-in commands

- `exit` - exits the shell
- `cd [dir]` - changes the current directory
- `pwd` - prints the current working directory
- `help` - displays available built-in commands

## Example

```bash
mini-shell:/home/user/shell-workshop$ pwd
/home/user/shell-workshop

mini-shell:/home/user/shell-workshop$ cd ..
mini-shell:/home/user$ pwd
/home/user

mini-shell:/home/user$ ls | grep shell
mini-shell

## Build
Compile the project with: make

## Run
Run the shell with ./shell

## Project structure
shell.c - main shell logic
utils.c - helper functions
utils.h - declarations for helper functions
Makefile - build configuration

## Notes
This project was based on the shell-workshop skeleton and adapted for compatibility with modern Ubuntu toolchains.

Additional improvements implemented in this version include:
Ubuntu Makefile compatibility fix
built-in shell commands
dynamic prompt
better interactive behavior for empty input
