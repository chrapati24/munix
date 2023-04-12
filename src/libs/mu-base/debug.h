#pragma once

#include <string.h>

#include "fmt.h"
#include "macro.h"
#include "types.h"

#define non_null$(x) ({                             \
    if ((uintptr_t)x == 0)                          \
    {                                               \
        panic("Assertion failed (" #x " is null)"); \
    }                                               \
    x;                                              \
})

typedef enum
{
    DEBUG_NONE,
    DEBUG_INFO,
    DEBUG_WARN,
    DEBUG_PANIC,

    DEBUG_EVENT_LENGTH
} DebugEvent;

void __debug_impl(cstr filename, usize lineno, DebugEvent event, cstr fmt, FmtArgs args);

#define debug(EVENT, FMT, ...) __debug_impl(__FILENAME__, __LINE__, EVENT, FMT, PRINT_ARGS(__VA_ARGS__));

#define debug_info(FMT, ...) debug(DEBUG_INFO, FMT, __VA_ARGS__)

#define debug_warn(FMT, ...) debug(DEBUG_WARN, FMT, __VA_ARGS__)

#define assert(COND, FMT, ...) \
    ({if (!(COND)) {__debug_impl(__FILENAME__, __LINE__, DEBUG_PANIC, FMT, PRINT_ARGS(__VA_ARGS__)); } })

#define panic(FMT, ...) \
    ({__debug_impl(__FILENAME__, __LINE__, DEBUG_PANIC, FMT, PRINT_ARGS(__VA_ARGS__)); \
    unreachable(); })