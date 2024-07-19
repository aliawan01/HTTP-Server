#pragma once

#define _CRT_SECURE_NO_WARNINGS

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif

#include <windows.h>
#include <intrin.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <iphlpapi.h>


#pragma comment(lib, "Ws2_32.lib")

// Macros
#define global static
#define localPersist static


// NOTE(ali): Project Header Files
#include "file_handling.h"
#include "http_request.h"