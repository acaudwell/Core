#include "timezone.h"

#include <string>
#include <stdlib.h>

std::string local_tz;
bool tz_initialized = false;

void init_tz() {
    if(tz_initialized) return;

    //check if TZ is set, store current value
    char* current_tz_env = getenv("TZ");
    if(current_tz_env != 0) {
        local_tz  = std::string("TZ=");
        local_tz += std::string(current_tz_env);
    }

    tz_initialized = true;
}

void set_utc_tz() {
   //change TZ to UTC
   putenv((char*)"TZ=UTC");
   tzset();
}

void unset_utc_tz() {
    if(!local_tz.empty()) {
        putenv((char*)local_tz.c_str());
    } else {
#ifndef _WIN32
        unsetenv("TZ");
#else
        putenv((char*)"TZ=");
#endif
    }
    tzset();
}

time_t mktime_utc(struct tm* timeinfo) {
    init_tz();

    set_utc_tz();

        time_t time_utc = mktime(timeinfo);

    unset_utc_tz();

    return time_utc;
}
