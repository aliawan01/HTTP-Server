#pragma once

#include "platform.h"
#include "arena.h"

// Macros
#define internal static
#define global_variable static
#define local_persist static

// Byte Conversions
#define GB(bytes) (uint64_t)bytes*1024*1024*1024
#define MB(bytes) (uint64_t)bytes*1024*1024
#define KB(bytes) (uint64_t)bytes*1024

#define ArrayCount(array) sizeof(array)/sizeof(array[0])
#define Maximum(a, b) (a > b) ? a : b
#define Minimum(a, b) (a < b) ? a : b
#ifdef DEBUG_BUILD
#define Assert(condition)\
    if (!(condition)) { \
        fprintf(stderr, "Assertion in file: %s at line %d\n", __FILE__, __LINE__); \
        __debugbreak(); \
    }           
#else
#define Assert(condition)

#endif

static bool IsPowerOfTwo(uint64_t num) {
    return (num != 0) && (num & (num-1)) == 0;
}

typedef struct {
    Arena* permanent_arena;
    Arena* route_callback_arena;
    Arena* recycle_arena;
    Arena* scratch_pool[2];
    int    scratch_arena_num;
} Allocator;

typedef struct {
    sqlite3* database;
    bool     initialized;
} HTTPDatabase;

// TODO: Maybe? make sure this a global_variable after testing.
extern Allocator allocator;
extern HTTPDatabase http_database;

#define GetScratch(conflicting_arenas, num) ScratchGetFree(allocator.scratch_pool, allocator.scratch_arena_num, conflicting_arenas, num)

