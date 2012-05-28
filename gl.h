#ifndef CORE_GL_H
#define CORE_GL_H

#include <GL/glew.h>

#define NO_SDL_GLEXT
#include "SDL.h"
#include "SDL_opengl.h"
#endif

#define PRINT_GL_ERRORS

#ifdef PRINT_GL_ERRORS
#define glCheckError() { \
    int gl_error = glGetError(); \
    if(gl_error != GL_NO_ERROR) warnLog("GL error %s at %s:%d", gluErrorString(gl_error), __FILE__, __LINE__); \
}
#else
#define glCheckError()
#endif

#include <ft2build.h>
#include FT_FREETYPE_H
#include FT_GLYPH_H
#include "ftgl/FTUnicode.h"

#include "vectors.h"


