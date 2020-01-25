#ifndef CORE_GL_H
#define CORE_GL_H

#include <GL/glew.h>

#define PRINT_GL_ERRORS

#ifdef PRINT_GL_ERRORS
#define glCheckError() { \
    int gl_error = glGetError(); \
    if(gl_error != GL_NO_ERROR) warnLog("GL error %s at %s:%d", gluErrorString(gl_error), __FILE__, __LINE__); \
}
#else
#define glCheckError()
#endif

#endif


