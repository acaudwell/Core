#include "timezone.h"

#include <string>

std::string stored_env_tz;

void store_env_tz() {
    //check if TZ is set, store current value
    if(stored_env_tz.empty()) {
        char* current_tz_env = getenv("TZ");
        if(current_tz_env != 0) {
            stored_env_tz  = std::string("TZ=");
            stored_env_tz += std::string(current_tz_env);
        }
    }
}

void set_utc_tz() {
   //change TZ to UTC
   putenv((char*)"TZ=UTC");
   tzset();
}

void unset_utc_tz() {
    if(!stored_env_tz.empty()) {
        putenv((char*)stored_env_tz.c_str());
    } else {
#ifndef _WIN32
        unsetenv("TZ");
#else
        putenv((char*)"TZ=");
#endif
    }
    tzset();
}
