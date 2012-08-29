#include "timer.h"

// GLTimer

GLTimer::GLTimer() {
    query_id      = 0;
    query_value   = 0;
    running       = false;
}

GLTimer::GLTimer(const std::string& name) : name(name) {
    query_id      = 0;
    query_value   = 0;
    running       = false;    
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
    running = false;    
}

void GLTimer::start() {
    if(running) return;
    
    if(!query_id) glGenQueries( 1, &query_id );
    
    glBeginQuery(GL_TIME_ELAPSED, query_id);

    running = true;
}

void GLTimer::stop() {
    if(running) glEndQuery(GL_TIME_ELAPSED);
}

const std::string& GLTimer::getName() const {
    return name;
}

GLuint64 GLTimer::getValue() const {
    return query_value;
}

int GLTimer::getMillis() const {
    return query_value / 1000000;
}

bool GLTimer::check() {
    if(!running) return false;
    
    GLuint64 elapsed;
    GLint    available = 0;

    glGetQueryObjectiv(query_id, GL_QUERY_RESULT_AVAILABLE, &available);

    if(!available) return false;
    
    glGetQueryObjectui64v(query_id, GL_QUERY_RESULT, &elapsed);

    query_value = elapsed;

    running = false;
    
    return true;
}
