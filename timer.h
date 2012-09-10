#ifndef CORE_TIMER_H
#define CORE_TIMER_H

#include <string>

#include "gl.h"

class GLTimer {
    Uint32   query_start;
    Uint32   query_stop;
    Uint32   cpu_time;    
    GLuint64 query_value;
    GLuint   query_id;
    std::string name;
public:
    GLTimer();
    GLTimer(const std::string& name);
    ~GLTimer();

    void start();
    void stop();
    bool check();
    
    const std::string& getName() const;
    GLuint64 getValue() const;
    Uint32   getGLMillis() const;
    Uint32   getCPUMillis() const;

    void unload();
};

#endif
