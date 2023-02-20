//! \file
//! \brief A graphical `assert` macro.
//! \author Will Blankemeyer

#pragma once

[[noreturn]] extern void _Assert(const char *, int, const char *, const char *);

#ifdef __GNUC__
#define likely(x)   __builtin_expect((x), 1)
#define unlikely(x) __builtin_expect((x), 0)
#else
#define likely(x)   (x)
#define unlikely(x) (x)
#endif

#ifdef NDEBUG
#define assert(_) ((void)0)
#else
#define assert(cond) \
    if (unlikely(!(cond))) { \
        _Assert(__FILE__, __LINE__, __func__, #cond); \
    }
#endif
