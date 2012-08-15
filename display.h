
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

#if defined(SDL_VIDEO_DRIVER_X11) && !defined(__MACOSX__)
#define SDLAPP_XWINDOWS
#include "X11/Xlib.h"
#endif

class SDLInitException : public std::exception {
protected:
    std::string error;
public:
    SDLInitException(const std::string& error) : error(error) {}
    virtual ~SDLInitException() throw () {};

    virtual const char* what() const throw() { return error.c_str(); }
};

class SDLAppDisplay {

    bool enable_shaders;
    bool enable_alpha;
    bool resizable;
    bool frameless;
    bool vsync;

    int  multi_sample;

#ifdef SDLAPP_XWINDOWS
    Window xwindow;
#endif

    int  SDLFlags(bool fullscreen);
    void setupExtensions();
public:
    bool screensaver;

    int width, height;
    int desktop_width, desktop_height;
    int windowed_width, windowed_height;

#if SDL_VERSION_ATLEAST(1,3,0)
    SDL_Window*   sdl_window;
    SDL_GLContext gl_context;
#else
    SDL_Surface *surface;
#endif

    bool fullscreen;
    vec4 clearColour;

    SDLAppDisplay();
    ~SDLAppDisplay();

    void   toggleFullscreen();
    void   resize(int width, int height);

    void   init(std::string window_title, int width, int height, bool fullscreen);
    void   setVideoMode(int width, int height, bool fullscreen);

    bool   multiSamplingEnabled();

    void   quit();

    void   update();
    void   clear();

    void   setClearColour(vec3 colour);
    void   setClearColour(vec4 colour);

#ifdef SDLAPP_XWINDOWS
    void  setXWindow(const Window& xwindow);
#endif

    void   enableShaders(bool enable);

    void   enableShaders(bool enable);
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
