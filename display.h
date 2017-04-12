
/*
    Copyright (c) 2008 Andrew Caudwell (acaudwell@gmail.com)
    All rights reserved.

    Redistribution and use in source and binary forms, with or without
    modification, are permitted provided that the following conditions
    are met:
    1. Redistributions of source code must retain the above copyright
       notice, this list of conditions and the following disclaimer.
    2. Redistributions in binary form must reproduce the above copyright
       notice, this list of conditions and the following disclaimer in the
       documentation and/or other materials provided with the distribution.
    3. The name of the author may not be used to endorse or promote products
       derived from this software without specific prior written permission.

    THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR
    IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
    OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
    IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT,
    INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
    NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
    DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
    THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
    (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
    THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#ifndef SDLAPP_DISPLAY_H
#define SDLAPP_DISPLAY_H

#include "gl.h"

#include "shader.h"
#include "logger.h"
#include "vectors.h"
#include "texture.h"
#include "fxfont.h"

#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

#include <vector>
#include <string>
#include <exception>

class SDLInitException : public std::exception {
protected:
    std::string error;
public:
    SDLInitException(const std::string& error) : error(error) {}
    virtual ~SDLInitException() throw () {};

    virtual const char* what() const throw() { return error.c_str(); }
};

class SDLAppDisplay {

    bool enable_alpha;
    bool resizable;
    bool fullscreen;
    bool frameless;
    bool vsync;

    int  zbuffer_depth;
    int  multi_sample;

    Uint32 SDLWindowFlags(bool fullscreen);

    void setupExtensions();
public:
    int width, height;
    int desktop_width, desktop_height;
    int windowed_width, windowed_height;

#if SDL_VERSION_ATLEAST(2,0,0)
    SDL_Window*   sdl_window;
    SDL_GLContext gl_context;

    int framed_width, framed_height;
    int framed_x, framed_y;
#else
    SDL_Surface *surface;
#endif
    vec4 clear_colour;

    SDLAppDisplay();
    ~SDLAppDisplay();

    void   getFullscreenResolution(int& width, int& height);
    void   toggleFullscreen();
    void   toggleFrameless();

    bool   isFullscreen() const;
    bool   isFrameless() const;

    void   resize(int width, int height);

    void   init(std::string window_title, int width, int height, bool fullscreen, int screen = -1);
    void   setVideoMode(int width, int height, bool fullscreen, int screen = -1);

    bool   multiSamplingEnabled();

    void   quit();

    void   update();
    void   clear();

    void   setClearColour(vec3 colour);
    void   setClearColour(vec4 colour);

    void   setZBufferDepth(int zbuffer_depth);

    void   enableVsync(bool vsync);
    void   enableAlpha(bool enable);
    void   enableResize(bool resizable);
    void   enableFrameless(bool frameless);

    void   multiSample(int sample);

    void   mode3D(float fov, float znear, float zfar);
    void   mode2D();

    void   push2D();
    void   pop2D();

    vec4  currentColour();

    vec3 project(vec3 pos);
    vec3 unproject(vec2 pos);
};

extern SDLAppDisplay display;

#endif
