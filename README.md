# HTTP Framework
![https_new](https://github.com/user-attachments/assets/7027c25f-a1a9-466e-9753-4144d5bf177d)


## Overview

This is a HTTP Framework written in C11 which supports both Windows and Linux.
It provides the following features and much more!

- Supports cookies and headers.
- Has support for Session-Based Authentication.
- Is multithreaded to support multiple clients simultaneously.
- Supports HTTPS (through OpenSSL)
- Supports sending and receiving JSON Data.
- Has built in database support (through SQlite3).
- Supports regex based routes.
- Can send and receive images
- Provides an API for creating, deleting and modifying files.
- Provides helper functions for cJSON.

## Building

### Dependencies
- CMake >= 3.16
- GCC/Clang/MSVC
- curl
- git


### Windows

#### Build Library and Dependencies
In the project's root directory run `misc\setup_windows.bat`


#### Compile examples
Run `misc\build_example.bat`

### Linux

#### Build Library and Dependencies
In the project's root directory run `misc/setup_linux.sh`


#### Compile examples
Run `misc/build_example.sh`

## Examples

In the examples directory there are two examples which show how the 
framework can be used, they are:

- A basic https server.
- A REST API.

## Docs

Documentation for all of the project's exported functions can be found
in the `docs` directory.
