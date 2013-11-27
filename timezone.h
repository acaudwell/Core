#ifndef CORE_TIMEZONE_H
#define CORE_TIMEZONE_H

extern "C" {
    void store_env_tz();
    void set_utc_tz();
    void unset_utc_tz();
};

#endif
