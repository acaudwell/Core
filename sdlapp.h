/*
    Copyright (c) 2009 Andrew Caudwell (acaudwell@gmail.com)
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

#ifndef SDLAPP_H
#define SDLAPP_H

#ifdef _WIN32

#ifdef _WIN32_WINNT
#undef _WIN32_WINNT
#endif

#define _WIN32_WINNT 0x0501
#include "windows.h"
#endif

#include "display.h"
#include "logger.h"

#include <stdexcept>
#include <vector>

extern std::string gSDLAppConfDir;
extern std::string gSDLAppResourceDir;
extern std::string gSDLAppPathSeparator;

extern std::string gSDLAppTitle;
extern std::string gSDLAppExec;

#ifdef _WIN32
extern HWND gSDLAppConsoleWindow;

bool SDLAppAttachToConsole();
void SDLAppCreateWindowsConsole();
void SDLAppResizeWindowsConsole(int height);
#endif

void SDLAppInfo(std::string msg);
void SDLAppQuit(std::string error);

void SDLAppInit(std::string apptitle, std::string execname);
bool SDLAppDirExists(std::string dir);
std::string SDLAppAddSlash(std::string path);

void SDLAppParseArgs(int argc, char *argv[], int* xres, int* yres, bool* fullscreen, std::vector<std::string>* otherargs = 0);

class SDLAppException : public std::exception {
protected:
    std::string message;
    bool showhelp;
public:
    SDLAppException(const char* str, ...) : showhelp(false) {

        va_list vl;
        char msg[4096];

        va_start(vl, str);
            vsnprintf(msg, 4096, str, vl);
        va_end(vl);

        message = std::string(msg);
    }

    SDLAppException(std::string message) : showhelp(false), message(message) {}

    ~SDLAppException() throw () {};

    bool showHelp() const { return showhelp; }
    void setShowHelp(bool showhelp) { this->showhelp = showhelp; };

    virtual const char* what() const throw() { return message.c_str(); }
};

class SDLApp {
    int frame_count;
    int fps_updater;
    int return_code;

    void updateFramerate();
protected:
    int  min_delta_msec;
    bool appFinished;
    void stop(int return_code);
public:
    float fps;

    SDLApp();
    virtual ~SDLApp() {};

    int run();

    virtual void resize(int width, int height) {};

    virtual void update(float t, float dt) {};
    virtual void init() {};

    virtual void logic(float t, float dt) {};
    virtual void draw(float t, float dt) {};

    virtual void quit() { appFinished = true; };
    
    virtual void mouseMove(SDL_MouseMotionEvent *e) {};
    virtual void mouseClick(SDL_MouseButtonEvent *e) {};
    virtual void keyPress(SDL_KeyboardEvent *e) {};

#if SDL_VERSION_ATLEAST(1,3,0)
    virtual void mouseWheel(SDL_MouseWheelEvent *e) {};
#endif

    int returnCode();
    bool isFinished();
};

#endif
