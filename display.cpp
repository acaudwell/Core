
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

#if SDL_VERSION_ATLEAST(2,0,0)
#include "SDL_syswm.h"
#endif

SDLAppDisplay display;

SDLAppDisplay::SDLAppDisplay() {
    clear_colour    = vec4(0.0f,0.0f,0.0f,1.0f);
    zbuffer_depth   = 16;
    enable_alpha    = false;
    vsync           = false;
    resizable       = false;
    frameless       = false;
    multi_sample    = 0;
    width           = 0;
    height          = 0;
    desktop_width   = 0;
    desktop_height  = 0;
    windowed_width  = 0;
    windowed_height = 0;
#if SDL_VERSION_ATLEAST(2,0,0)
    sdl_window = 0;
    gl_context = 0;

    framed_width  = 0;
    framed_height = 0;
    framed_x      = 0;
    framed_y      = 0;
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
    clear_colour = colour;
}

Uint32 SDLAppDisplay::SDLWindowFlags(bool fullscreen) {
#if SDL_VERSION_ATLEAST(2,0,0)
    Uint32 flags = SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN;

    if (frameless) flags |= SDL_WINDOW_BORDERLESS;
    if (resizable && !frameless) flags |= SDL_WINDOW_RESIZABLE;
    if (fullscreen) flags |= SDL_WINDOW_FULLSCREEN;
#else
    Uint32 flags = SDL_OPENGL | SDL_HWSURFACE | SDL_ANYFORMAT | SDL_DOUBLEBUF;

    if (frameless) flags |= SDL_NOFRAME;
    if (resizable && !fullscreen) flags |= SDL_RESIZABLE;
    if (fullscreen) flags |= SDL_FULLSCREEN;
#endif
    return flags;
}

void SDLAppDisplay::enableVsync(bool vsync) {
    this->vsync = vsync;
}

void SDLAppDisplay::setZBufferDepth(int zbuffer_depth) {
    this->zbuffer_depth = zbuffer_depth;
}

void SDLAppDisplay::enableResize(bool resizable) {
    this->resizable = resizable;
}

void SDLAppDisplay::enableFrameless(bool frameless) {
    this->frameless = frameless;
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

#if SDL_VERSION_ATLEAST(2,0,0) && defined(_WIN32)
WNDPROC window_proc = 0;

LRESULT CALLBACK window_filter_proc(HWND wnd, UINT msg, WPARAM wparam, LPARAM lparam) {

   if (msg == WM_SYSCOMMAND && (wparam & 0xfff0) == SC_KEYMENU) {
        return 0;
   }

   return CallWindowProc(window_proc, wnd, msg, wparam, lparam);
}
#endif

void SDLAppDisplay::setVideoMode(int width, int height, bool fullscreen, int screen) {
#if SDL_VERSION_ATLEAST(2,0,0)

    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE,   zbuffer_depth);

    if(multi_sample > 0) {
        SDL_GL_SetAttribute(SDL_GL_MULTISAMPLEBUFFERS, 1);
        SDL_GL_SetAttribute(SDL_GL_MULTISAMPLESAMPLES, (GLuint) multi_sample);
    }

    if(enable_alpha) {
        SDL_GL_SetAttribute(SDL_GL_ALPHA_SIZE, 8);
    }

    Uint32 flags = SDLWindowFlags(fullscreen);

    if(gl_context != 0) SDL_GL_DeleteContext(gl_context);


    int position_x = -1;
    int position_y = -1;

    int display_index = -1;

    if(sdl_window != 0) {
        display_index = SDL_GetWindowDisplayIndex(sdl_window);
        SDL_GetWindowPosition(sdl_window, &position_x, &position_y);
        SDL_DestroyWindow(sdl_window);

    } else if(screen > 0 && screen <= SDL_GetNumVideoDisplays()) {
        display_index = screen-1;
    }

    if(display_index != -1) {
        sdl_window = SDL_CreateWindow(gSDLAppTitle.c_str(), SDL_WINDOWPOS_UNDEFINED_DISPLAY(display_index), SDL_WINDOWPOS_UNDEFINED_DISPLAY(display_index), width, height, flags);

        if(sdl_window && position_x >= 0 && position_y >= 0) {
            SDL_SetWindowPosition(sdl_window, position_x, position_y);
        }

    } else {
        sdl_window = SDL_CreateWindow(gSDLAppTitle.c_str(), SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, width, height, flags);
    }

    if (!sdl_window) {

        // retry without multi-sampling enabled
        if(multi_sample > 0) {
            multi_sample = 0;
            SDL_GL_SetAttribute(SDL_GL_MULTISAMPLEBUFFERS, 0);
            SDL_GL_SetAttribute(SDL_GL_MULTISAMPLESAMPLES, 0);

            if(display_index != -1) {
                sdl_window = SDL_CreateWindow(gSDLAppTitle.c_str(), SDL_WINDOWPOS_UNDEFINED_DISPLAY(display_index), SDL_WINDOWPOS_UNDEFINED_DISPLAY(display_index), width, height, flags);

                if(sdl_window && position_x >= 0 && position_y >= 0) {
                    SDL_SetWindowPosition(sdl_window, position_x, position_y);
                }
            } else {
                sdl_window = SDL_CreateWindow(gSDLAppTitle.c_str(), SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, width, height, flags);
            }
        }

        if(!sdl_window) {
            std::string sdlerr(SDL_GetError());
            throw SDLInitException(sdlerr);
        }
    }

    gl_context = SDL_GL_CreateContext(sdl_window);

    if(!gl_context) {
        std::string sdlerr(SDL_GetError());
        throw SDLInitException(sdlerr);
    }

    if(vsync) SDL_GL_SetSwapInterval(1);
    else SDL_GL_SetSwapInterval(0);

#else
    int bpp = 32;

    int flags = SDLWindowFlags(fullscreen);

    if(vsync) SDL_GL_SetAttribute(SDL_GL_SWAP_CONTROL, 1);
    else SDL_GL_SetAttribute(SDL_GL_SWAP_CONTROL, 0);

    if(multi_sample > 0) {
        SDL_GL_SetAttribute(SDL_GL_MULTISAMPLEBUFFERS, 1);
        SDL_GL_SetAttribute(SDL_GL_MULTISAMPLESAMPLES, (GLuint) multi_sample);
    }

    if(enable_alpha) {
        SDL_GL_SetAttribute(SDL_GL_ALPHA_SIZE, 8);
    }

    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, zbuffer_depth);
    surface = SDL_SetVideoMode(width, height, bpp, flags);

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
            surface = SDL_SetVideoMode(width, height, bpp, flags);
        }

        if (!surface) {
            std::string sdlerr(SDL_GetError());
            throw SDLInitException(sdlerr);
        }
    }
#endif

    setupExtensions();

#if SDL_VERSION_ATLEAST(2,0,0) && defined(_WIN32)
    // suppress 'ding' noise when doing alt+key combinations
    // solution from: http://forums.libsdl.org/viewtopic.php?t=6075

    SDL_SysWMinfo sys_window_info;

    SDL_VERSION(&sys_window_info.version);

    if(SDL_GetWindowWMInfo(sdl_window, &sys_window_info)) {
        HWND wnd = sys_window_info.info.win.window;
        window_proc = (WNDPROC) GetWindowLongPtr(wnd, GWLP_WNDPROC);
        SetWindowLongPtr(wnd, GWLP_WNDPROC, (LONG_PTR) &window_filter_proc);
    }
#endif
}

void SDLAppDisplay::getFullscreenResolution(int& width, int& height) {

    int fullscreen_width  = desktop_width;
    int fullscreen_height = desktop_height;

#if SDL_VERSION_ATLEAST(2,0,0)
    // TODO: SDL2 api will have a nice way to do this ...
#else
    float aspect_ratio = fullscreen_width / (float) fullscreen_height;

    if(aspect_ratio > 2.5) {

        SDL_Rect** modes = SDL_ListModes(0, SDLWindowFlags(true));

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
}

void SDLAppDisplay::toggleFullscreen() {

    int width  = this->width;
    int height = this->height;

    if(!fullscreen) {

        //save windowed width and height
        windowed_width  = width;
        windowed_height = height;

        getFullscreenResolution(width, height);

    } else {
        //switch back to window dimensions, if known
        if(windowed_width != 0) {
            width  = windowed_width;
            height = windowed_height;
        }
    }

    fullscreen = !fullscreen;

    int resized_width, resized_height;

#if SDL_VERSION_ATLEAST(2,0,0)
    SDL_SetWindowFullscreen(sdl_window, fullscreen ? SDL_WINDOW_FULLSCREEN_DESKTOP : 0);
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

void SDLAppDisplay::toggleFrameless() {
#if SDL_VERSION_ATLEAST(2,0,0)
    if(fullscreen) return;

    frameless = !frameless;

    if(frameless) {

        int position_x, position_y;
        SDL_GetWindowPosition(sdl_window, &position_x, &position_y);

        framed_width  = width;
        framed_height = height;
        framed_x      = position_x;
        framed_y      = position_y;

#ifdef _WIN32
        SDL_SysWMinfo sys_window_info;
        SDL_VERSION(&sys_window_info.version);

        if(SDL_GetWindowWMInfo(sdl_window, &sys_window_info)) {

            //make the new window equal the size of the old window including frame

            HWND wnd = sys_window_info.info.win.window;

            RECT rect;
            GetWindowRect(wnd, &rect);

            position_x = rect.left;
            position_y = rect.top;

            width  = rect.right - rect.left;
            height = rect.bottom - rect.top;
        }
#endif

        //work around window position changing when when frame is toggled
        //related bug: https://bugzilla.libsdl.org/show_bug.cgi?id=2791

        SDL_SetWindowBordered(sdl_window, SDL_FALSE);
        SDL_SetWindowSize(sdl_window, width, height);
        SDL_SetWindowPosition(sdl_window, position_x, position_y);

        //window needs to be recreated to remove SDL_WINDOW_RESIZABLE flag
        //otherwise there is still a weird border

        setVideoMode(width, height, fullscreen);

    } else {

#ifdef _WIN32
        // handle computing framed window position
        // if launched in frameless mode initially
        if(framed_width == 0) {
            SDL_SysWMinfo sys_window_info;
            SDL_VERSION(&sys_window_info.version);

            if(SDL_GetWindowWMInfo(sdl_window, &sys_window_info)) {

                HWND wnd = sys_window_info.info.win.window;

                RECT old_rect;
                GetWindowRect(wnd, &old_rect);

                SDL_SetWindowBordered(sdl_window, SDL_TRUE);

                RECT new_rect;
                GetWindowRect(wnd, &new_rect);

                SDL_GetWindowSize(sdl_window, &framed_width, &framed_height);
                SDL_GetWindowPosition(sdl_window, &framed_x, &framed_y);

                int width_delta  = (new_rect.right - new_rect.left) - (old_rect.right - old_rect.left);
                int height_delta = (new_rect.bottom - new_rect.top) - (old_rect.bottom - old_rect.top);

                framed_width  = width - width_delta;
                framed_height = height - height_delta;

                framed_x += width_delta;
                framed_y += height_delta;

                // HACK: account for the resizable windows border being 2 pixels wider

                if(resizable) {
                    framed_x += 2;
                    framed_y += 2;
                }
            }
        }
#endif
        SDL_SetWindowBordered(sdl_window, SDL_TRUE);

        if(framed_width > 0) {
            width  = framed_width;
            height = framed_height;
        }

        SDL_SetWindowSize(sdl_window, width, height);

        if(framed_width > 0) {
            SDL_SetWindowPosition(sdl_window, framed_x, framed_y);
        }

        setVideoMode(width, height, fullscreen);
    }
#endif
}

bool SDLAppDisplay::isFullscreen() const {
    return fullscreen;
}

bool SDLAppDisplay::isFrameless() const {
    return frameless;
}

void SDLAppDisplay::resize(int width, int height) {

    int resized_width, resized_height;

#if SDL_VERSION_ATLEAST(2,0,0)
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

void SDLAppDisplay::init(std::string window_title, int width, int height, bool fullscreen, int screen) {

    if(SDL_Init(SDL_INIT_TIMER | SDL_INIT_VIDEO) != 0) {
        throw SDLInitException(SDL_GetError());
    }


#if SDL_VERSION_ATLEAST(2,0,0)

    // check screen is valid
    if(screen <= 0 || screen > SDL_GetNumVideoDisplays()) {
        screen = -1;
    }

    SDL_Rect display_rect;
    SDL_GetDisplayBounds(screen > 0 ? screen-1 : 0, &display_rect);

    desktop_width  = display_rect.w;
    desktop_height = display_rect.h;

#else
    const SDL_VideoInfo* display_info = SDL_GetVideoInfo();

    //save the desktop resolution
    desktop_width  = display_info->current_w;
    desktop_height = display_info->current_h;
#endif

    //initialize width and height to desktop resolution if un-specified
    if(!width || !height) {
        if(fullscreen) {
            getFullscreenResolution(width, height);
        } else {
            if(!width) width   = desktop_width;
            if(!height) height = desktop_height;
        }
    }

    atexit(SDL_Quit);

#if SDL_VERSION_ATLEAST(2,0,0)

#else
    SDL_EnableUNICODE(1);
    SDL_WM_SetCaption(window_title.c_str(),0);
#endif

    setVideoMode(width, height, fullscreen, screen);

    //get actual opengl viewport
    GLint viewport[4];
    glGetIntegerv( GL_VIEWPORT, viewport );

    this->width      = viewport[2];
    this->height     = viewport[3];
    this->fullscreen = fullscreen;

    glViewport(0, 0, this->width, this->height);
}

void SDLAppDisplay::quit() {

#if SDL_VERSION_ATLEAST(2,0,0)
    if(gl_context != 0) SDL_GL_DeleteContext(gl_context);
    if(sdl_window != 0) SDL_DestroyWindow(sdl_window);
#endif

    texturemanager.purge();
    shadermanager.purge();
    fontmanager.purge();
    fontmanager.destroy();
}

void SDLAppDisplay::update() {
#if SDL_VERSION_ATLEAST(2,0,0)
    SDL_GL_SwapWindow(sdl_window);
#else
    SDL_GL_SwapBuffers();
#endif
}

void SDLAppDisplay::clear() {
    glClearColor(clear_colour.x, clear_colour.y, clear_colour.z, clear_colour.w);
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
