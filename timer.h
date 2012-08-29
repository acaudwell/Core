#ifndef CORE_TIMER_H
#define CORE_TIMER_H

#include <string>

#include "gl.h"

class GLTimer {

    GLuint64 query_value;
    GLuint   query_id;
    std::string name;
    bool running;
public:
    GLTimer();
    GLTimer(const std::string& name);
    ~GLTimer();

    void start();
    void stop();
    bool check();
    
    const std::string& getName() const;
    GLuint64 getValue() const;
    int      getMillis() const;
    
    void unload();
};

#endif
