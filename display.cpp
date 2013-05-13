
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

#include "display.h"
#include "sdlapp.h"
#include <iostream>

SDLAppDisplay display;

SDLAppDisplay::SDLAppDisplay() {
    clearColour = vec4(0.0f,0.0f,0.0f,1.0f);
    enable_shaders=false;
    enable_alpha=false;
    vsync=false;
    resizable=false;
    frameless=false;
    multi_sample = 0;
    width = 0;
    height = 0;
    desktop_width   = 0;
    desktop_height  = 0;
    windowed_width  = 0;
    windowed_height = 0;
#if SDL_VERSION_ATLEAST(1,3,0)
    sdl_window = 0;
    gl_context = 0;
#else
    surface = 0;
#endif

}

SDLAppDisplay::~SDLAppDisplay() {
}

void SDLAppDisplay::setClearColour(vec3 colour) {
    setClearColour(vec4(colour, enable_alpha ? 0.0f : 1.0f));
}

void SDLAppDisplay::setClearColour(vec4 colour) {
    clearColour = colour;
}

int SDLAppDisplay::SDLFlags(bool fullscreen) {
    int flags = SDL_OPENGL | SDL_HWSURFACE | SDL_ANYFORMAT | SDL_DOUBLEBUF;
    if (frameless) flags |= SDL_NOFRAME;
    if (resizable && !fullscreen) flags |= SDL_RESIZABLE;
    if (fullscreen) flags |= SDL_FULLSCREEN;

    //SDL 1.3 vsync
#ifdef SDL_RENDERER_PRESENTVSYNC
    if (!vsync) flags |= SDL_RENDERER_PRESENTVSYNC;
#endif

    return flags;
}

void SDLAppDisplay::enableVsync(bool vsync) {
    this->vsync = vsync;
}

void SDLAppDisplay::enableShaders(bool enable) {
    enable_shaders = enable;
}

void SDLAppDisplay::enableResize(bool resizable) {
    this->resizable = resizable;
}

void SDLAppDisplay::enableAlpha(bool enable) {
    enable_alpha = enable;
}

void SDLAppDisplay::multiSample(int samples) {
    multi_sample = samples;
}

void SDLAppDisplay::setupExtensions() {

    GLenum err = glewInit();

    if (GLEW_OK != err) {
        /* Problem: glewInit failed, something is seriously wrong. */
        char glewerr[1024];
        snprintf(glewerr, 1024, "GLEW Error: %s", glewGetErrorString(err));

        throw SDLInitException(std::string(glewerr));
    }
}

bool SDLAppDisplay::multiSamplingEnabled() {
    int value;
    SDL_GL_GetAttribute( SDL_GL_MULTISAMPLEBUFFERS, &value );
    return value==1;
}

void SDLAppDisplay::setVideoMode(int width, int height, bool fullscreen) {
#if SDL_VERSION_ATLEAST(1,3,0)

    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);

    Uint32 flags = SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN;
    if(resizable && !fullscreen) flags |= SDL_WINDOW_RESIZABLE;
    if(fullscreen) flags |= SDL_WINDOW_FULLSCREEN;

    if(gl_context != 0) SDL_GL_DeleteContext(gl_context);
    if(sdl_window != 0) SDL_DestroyWindow(sdl_window);

    sdl_window = SDL_CreateWindow(
	gSDLAppTitle.c_str(), SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
        width, height, flags);


    if (!sdl_window) {
        std::string sdlerr(SDL_GetError());
        throw SDLInitException(sdlerr);
    }

    gl_context = SDL_GL_CreateContext(sdl_window);

    if(vsync) SDL_GL_SetSwapInterval(1);
#else
    int depth = 32;

    int flags = SDLFlags(fullscreen);

    if(vsync) SDL_GL_SetAttribute(SDL_GL_SWAP_CONTROL, 1);
    else SDL_GL_SetAttribute(SDL_GL_SWAP_CONTROL, 0);

    if(multi_sample > 0) {
        SDL_GL_SetAttribute(SDL_GL_MULTISAMPLEBUFFERS, 1);
        SDL_GL_SetAttribute(SDL_GL_MULTISAMPLESAMPLES, (GLuint) multi_sample);
    }

    if(enable_alpha) {
        SDL_GL_SetAttribute(SDL_GL_ALPHA_SIZE, 8);
    }

#ifdef _WIN32
    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
    surface = SDL_SetVideoMode(width, height, depth, flags);
#else
    if(enable_shaders) {
        SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
        surface = SDL_SetVideoMode(width, height, depth, flags);
    } else {
        depth = 24;
        SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 16);
        surface = SDL_SetVideoMode(width, height, depth, flags);
    }
#endif

    if (!surface) {
        if (multi_sample > 0) {
#ifndef _WIN32
            // Retry without multi-sampling before failing
            std::cerr << "Failed to set video mode: " << SDL_GetError() << std::endl
                      << "Trying again without multi-sampling" << std::endl;
#endif
            multi_sample = 0;
            SDL_GL_SetAttribute(SDL_GL_MULTISAMPLEBUFFERS, 0);
            SDL_GL_SetAttribute(SDL_GL_MULTISAMPLESAMPLES, 0);
            surface = SDL_SetVideoMode(width, height, depth, flags);
        }

        if (!surface) {
            std::string sdlerr(SDL_GetError());
            throw SDLInitException(sdlerr);
        }
    }
#endif

    setupExtensions();
}

void SDLAppDisplay::toggleFullscreen() {

    int width  = this->width;
    int height = this->height;

    if(!fullscreen) {

        //save windowed width and height
        windowed_width  = width;
        windowed_height = height;

        int fullscreen_width  = desktop_width;
        int fullscreen_height = desktop_height;

        float aspect_ratio = fullscreen_width / (float) fullscreen_height;

        // if the aspect ratio suggests the person is using multiple monitors
        // find a supported resolution with a lower aspect ratio with the same
        // fullscreen height

#if SDL_VERSION_ATLEAST(1,3,0)
        // TODO: do something with the 1.3 API here
#else
        if(aspect_ratio >= 2.5) {

            SDL_Rect** modes = SDL_ListModes(0, SDLFlags(true));

            if(modes != (SDL_Rect**)0 && modes != (SDL_Rect**)-1) {

                for (int i=0; modes[i]; i++) {
                    if(modes[i]->h == fullscreen_height && (modes[i]->w/(float)modes[i]->h) < 2.5) {
                        fullscreen_width = modes[i]->w;
                        break;
                    }
                }
            }
        }
#endif

        width  = fullscreen_width;
        height = fullscreen_height;

    } else {
        //switch back to window dimensions, if known
        if(windowed_width != 0) {
            width  = windowed_width;
            height = windowed_height;
        }
    }

    fullscreen = !fullscreen;

    setVideoMode(width, height, fullscreen);

    int resized_width, resized_height;

#if SDL_VERSION_ATLEAST(1,3,0)
    SDL_GetWindowSize(sdl_window, &resized_width, &resized_height);
#else
    const SDL_VideoInfo* display_info = SDL_GetVideoInfo();

    resized_width  = display_info->current_w;
    resized_height = display_info->current_h;
#endif

    //set viewport to match what we ended up on
    glViewport(0, 0, resized_width, resized_height);

    this->width  = resized_width;
    this->height = resized_height;
}

void SDLAppDisplay::resize(int width, int height) {

    int resized_width, resized_height;

#if SDL_VERSION_ATLEAST(1,3,0)
    SDL_GetWindowSize(sdl_window, &resized_width, &resized_height);
#else
    setVideoMode(width, height, fullscreen);

    const SDL_VideoInfo* display_info = SDL_GetVideoInfo();

    resized_width  = display_info->current_w;
    resized_height = display_info->current_h;
#endif

    //set viewport to match what we ended up on
    glViewport(0, 0, resized_width, resized_height);

    this->width  = resized_width;
    this->height = resized_height;
}

void SDLAppDisplay::init(std::string window_title, int width, int height, bool fullscreen) {

    if(SDL_Init(SDL_INIT_TIMER | SDL_INIT_VIDEO) != 0) {
        throw SDLInitException(SDL_GetError());
    }

    const SDL_VideoInfo* display_info = SDL_GetVideoInfo();

    //save the desktop resolution
    desktop_width  = display_info->current_w;
    desktop_height = display_info->current_h;

    //initialize width and height to desktop resolution if un-specified
    if(!width) width   = desktop_width;
    if(!height) height = desktop_height;

    atexit(SDL_Quit);

    SDL_EnableUNICODE(1);

#if SDL_VERSION_ATLEAST(1,3,0)
#else
    SDL_WM_SetCaption(window_title.c_str(),0);
#endif

    setVideoMode(width, height, fullscreen);

    //get actual opengl viewport
    GLint viewport[4];
    glGetIntegerv( GL_VIEWPORT, viewport );
    
    this->width      = viewport[2];
    this->height     = viewport[3];
    this->fullscreen = fullscreen;

    glViewport(0, 0, this->width, this->height);
}

void SDLAppDisplay::quit() {

#if SDL_VERSION_ATLEAST(1,3,0)
    if(gl_context != 0) SDL_GL_DeleteContext(gl_context);
    if(sdl_window != 0) SDL_DestroyWindow(sdl_window);
#endif

    texturemanager.purge();
    shadermanager.purge();
    fontmanager.purge();
    fontmanager.destroy();
}

void SDLAppDisplay::update() {
#if SDL_VERSION_ATLEAST(1,3,0)
    SDL_GL_SwapWindow(sdl_window);
#else
    SDL_GL_SwapBuffers();
#endif
}

void SDLAppDisplay::clear() {
    glClearColor(clearColour.x, clearColour.y, clearColour.z, clearColour.w);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void SDLAppDisplay::mode3D(float fov, float znear, float zfar) {
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(fov, (GLfloat)width/(GLfloat)height, znear, zfar);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
}

void SDLAppDisplay::mode2D() {
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(0, width, height, 0, -1.0, 1.0);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
}

void SDLAppDisplay::push2D() {
    glMatrixMode(GL_PROJECTION);
        glPushMatrix();
        glLoadIdentity();
            glOrtho(0, display.width, display.height, 0, -1.0, 1.0);

    glMatrixMode(GL_MODELVIEW);
        glPushMatrix();
        glLoadIdentity();
}

void SDLAppDisplay::pop2D() {
        glMatrixMode(GL_PROJECTION);
        glPopMatrix();
        glMatrixMode(GL_MODELVIEW);
        glPopMatrix();
}

vec4 SDLAppDisplay::currentColour() {
    vec4 colour;
    glGetFloatv(GL_CURRENT_COLOR, glm::value_ptr(colour));
    return colour;
}

vec3 SDLAppDisplay::project(vec3 pos) {
    GLint viewport[4];
    GLdouble modelview[16];
    GLdouble projection[16];
    GLdouble winX, winY, winZ;

    glGetDoublev( GL_MODELVIEW_MATRIX, modelview );
    glGetDoublev( GL_PROJECTION_MATRIX, projection );
    glGetIntegerv( GL_VIEWPORT, viewport );

    gluProject( pos.x, pos.y, pos.z, modelview, projection, viewport, &winX, &winY, &winZ);

    winY = (float)viewport[3] - winY;

    return vec3((float) winX, (float) winY, (float) winZ);
}

vec3 SDLAppDisplay::unproject(vec2 pos) {
    GLint viewport[4];
    GLdouble modelview[16];
    GLdouble projection[16];
    GLfloat winX, winY, winZ;
    GLdouble posX, posY, posZ;

    glGetDoublev( GL_MODELVIEW_MATRIX, modelview );
    glGetDoublev( GL_PROJECTION_MATRIX, projection );
    glGetIntegerv( GL_VIEWPORT, viewport );

    winX = pos.x;
    winY = (float)viewport[3] - pos.y;
    glReadPixels( int(winX), int(winY), 1, 1, GL_DEPTH_COMPONENT, GL_FLOAT, &winZ );
    gluUnProject( winX, winY, winZ, modelview, projection, viewport, &posX, &posY, &posZ);

    return vec3((float) posX, (float) posY, (float) posZ);
}
