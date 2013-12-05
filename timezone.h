#ifndef CORE_TIMEZONE_H
#define CORE_TIMEZONE_H

#include <time.h>

extern "C" {
    void init_tz();
    void set_utc_tz();
    void unset_utc_tz();
    
    time_t mktime_utc(struct tm* timeinfo);
};

#endif
