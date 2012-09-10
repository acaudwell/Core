#include "timer.h"

// GLTimer

GLTimer::GLTimer() {
    query_id      = 0;
    query_value   = 0;
    query_start   = 0;
    query_stop    = 0;
    cpu_time      = 0;
}

GLTimer::GLTimer(const std::string& name) : name(name) {
    query_id      = 0;
    query_value   = 0;
    query_start   = 0;
    query_stop    = 0;
    cpu_time      = 0;
}

GLTimer::~GLTimer() {
    unload();
}

void GLTimer::unload() {
    query_value = 0;
    if(query_id) {
        glDeleteQueries(1, &query_id);
        query_id = 0;
    }
    query_start = query_stop = 0; 
}

void GLTimer::start() {
    if(query_start > 0) return;
    
    query_start = SDL_GetTicks();
    
    if(!query_id) glGenQueries( 1, &query_id );
    
    glBeginQuery(GL_TIME_ELAPSED, query_id);

    query_stop = 0;
}

void GLTimer::stop() {
    if(!query_start || query_stop > 0) return;
    glEndQuery(GL_TIME_ELAPSED);
    query_stop = SDL_GetTicks();
}

const std::string& GLTimer::getName() const {
    return name;
}

GLuint64 GLTimer::getValue() const {
    return query_value;
}

Uint32 GLTimer::getGLMillis() const {
    return query_value / 1000000;
}

Uint32 GLTimer::getCPUMillis() const {
    return cpu_time;
}

bool GLTimer::check() {
    if(!query_start) return false;
    
    GLuint64 elapsed;
    GLint    available = 0;

    glGetQueryObjectiv(query_id, GL_QUERY_RESULT_AVAILABLE, &available);

    if(!available) return false;
    
    glGetQueryObjectui64v(query_id, GL_QUERY_RESULT, &elapsed);

    query_value = elapsed;
    cpu_time    = query_stop-query_start;
    query_start = query_stop = 0;
    
    return true;
}
