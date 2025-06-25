/***
allc_time_t functions

Kaj Munhoz Arfvidsson, 2024
***/

// Module Dependencies {{{1
// ========================

#define ALLC_IMPL // For developmnet

#ifdef ALLC_IMPL
#   define ALLC_TIME_IMPL
#endif

#include "../types.h"
#include "../cstr.h"
#include "../strbuf.h"

#include <time.h>
#include <unistd.h>

// Declarations {{{1
// =================

#ifndef ALLC_TIME__GUARD
#define ALLC_TIME__GUARD

// allc_time_t {{{1
// ----------------

enum allc_time_e {
    ALLC_TIME_HOUR,
    ALLC_TIME_MINUTE,
    ALLC_TIME_SECOND,
    ALLC_TIME_MILLISECOND,
    ALLC_TIME_MICROSECOND,
    ALLC_TIME_NANOSECOND,
};

// enum allc_timeprecision_e {
//     ALLC_TIMEPRECISION_SECOND,
// };

struct allc_time_s {
    // allc_u32_t second;
    // allc_u32_t nanosecond;
    time_t time;
};

#ifdef ALLC_TIME__IMPL
typedef time_t allc_time_t;
#else
typedef void *allc_time_t;
#endif

struct allc_time_s allc_time_now()
{
    time_t now;
    time(&now);
    return (struct allc_time_s) {
        .time = now,
    };
}

struct allc_time_s allc_time_add(struct allc_time_s t1, struct allc_time_s t2)
{
    return (struct allc_time_s) {
        .time = t1.time + t2.time,
    };
}

struct allc_time_s allc_time_diff(struct allc_time_s t1, struct allc_time_s t2)
{
    return (struct allc_time_s) {
        .time = t1.time - t2.time,
    };
}

allc_u32_t allc_time_get(enum allc_time_e unit, struct allc_time_s time)
{
    struct tm *tm = localtime(&time.time);
    switch (unit) {
        case ALLC_TIME_HOUR:
            return tm->tm_hour;
        case ALLC_TIME_MINUTE:
            return tm->tm_min;
        case ALLC_TIME_SECOND:
            return tm->tm_sec;
        case ALLC_TIME_MILLISECOND:
            return 0;
        case ALLC_TIME_MICROSECOND:
            return 0;
        case ALLC_TIME_NANOSECOND:
            return 0;
    }
    return 0;
}

void allc_time_sleep(struct allc_time_s time) {
    sleep(time.time);
}

StrBuf allc_time_format(String format, struct allc_time_s time);

#endif // ALLC_TIME__GUARD
// }}}1

// vim: ai et ts=4 sw=0 fdl=99 fdm=marker
