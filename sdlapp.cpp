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

#include "sdlapp.h"
#include "display.h"
#include "logger.h"
#include "SDL_syswm.h"

std::string gSDLAppResourceDir;
std::string gSDLAppConfDir;

#ifdef _WIN32
std::string gSDLAppPathSeparator = "\\";
#else
std::string gSDLAppPathSeparator = "/";
#endif

std::string gSDLAppTitle = "SDL App";
std::string gSDLAppExec  = "sdlapp";

#ifdef _WIN32

HWND SDLApp::console_window   = 0;
bool SDLApp::existing_console = false;

void SDLApp::showConsole(bool show) {
    if(!SDLApp::console_window) return;

    ShowWindow( SDLApp::console_window, show);
}

void SDLApp::initConsole() {
    if(SDLApp::console_window != 0) return;

    SDLApp::console_window = GetConsoleWindow();

    // check if this is a new console or not
    CONSOLE_SCREEN_BUFFER_INFO buffer_info;
    if(GetConsoleScreenBufferInfo( GetStdHandle( STD_OUTPUT_HANDLE ), &buffer_info )) {
        existing_console = !(buffer_info.dwCursorPosition.X==0 && buffer_info.dwCursorPosition.Y==0);
    // assume TERM env implies this is an existing terminal
    } else if(getenv("TERM") != 0) {
        existing_console = true;
    }

    // don't customize the console unless it was created for this program
    if(existing_console) return;

    //disable the close button so the user cant crash the application
    HMENU hm = GetSystemMenu(SDLApp::console_window, false);
    DeleteMenu(hm, SC_CLOSE, MF_BYCOMMAND);

    //set title

    char consoleTitle[512];
    int console_suffix = 0;
    sprintf(consoleTitle, "%s Console", gSDLAppTitle.c_str());

    while(FindWindow(0, consoleTitle)) {
        sprintf(consoleTitle, "%s Console %d", gSDLAppTitle.c_str(), ++console_suffix);
    }

    SetConsoleTitle(consoleTitle);
}

void SDLApp::resizeConsole(int height) {
    if(SDLApp::console_window !=0 && !existing_console) {
        RECT windowRect;
        if(GetWindowRect(SDLApp::console_window, &windowRect)) {
            float width = windowRect.right - windowRect.left;
            MoveWindow(SDLApp::console_window,windowRect.left,windowRect.top,width,height,true);
        }
    }
}

#endif

bool SDLAppDirExists(std::string dir) {
    struct stat st;
    return !stat(dir.c_str(), &st) && S_ISDIR(st.st_mode);
}

std::string SDLAppAddSlash(std::string path) {

    //append slash unless the path is empty
    if(path.size() && path[path.size()-1] != gSDLAppPathSeparator[0]) {
        path += gSDLAppPathSeparator;
    }

    return path;
}

//info message
void SDLAppInfo(std::string msg) {
#ifdef _WIN32
    SDLApp::showConsole(true);
#endif

    printf("%s\n", msg.c_str());

#ifdef _WIN32
    if(!SDLApp::existing_console) {
        printf("\nPress Enter\n");
        getchar();
    }
#endif

    exit(0);
}

//display error only
void SDLAppQuit(std::string error) {
    SDL_Quit();

#ifdef _WIN32
    SDLApp::showConsole(true);
#endif

    fprintf(stderr, "%s: %s\n", gSDLAppExec.c_str(), error.c_str());
    fprintf(stderr, "Try '%s --help' for more information.\n\n", gSDLAppExec.c_str());

#ifdef _WIN32
    if(!SDLApp::existing_console) {
        fprintf(stderr, "Press Enter\n");
        getchar();
    }
#endif

    exit(1);
}

bool SDLApp::getClipboardText(std::string& text) {

#if SDL_VERSION_ATLEAST(2,0,0)
    char* clipboard_text = SDL_GetClipboardText();

    if(clipboard_text!=0) {
        text = std::string(clipboard_text);
    } else {
        text.resize(0);
    }

    return true;
#else
    return false;
#endif
}

void SDLApp::setClipboardText(const std::string& text) {
#if SDL_VERSION_ATLEAST(2,0,0)
    SDL_SetClipboardText(text.c_str());
#endif
}

void SDLAppInit(std::string apptitle, std::string execname, std::string exepath) {
    gSDLAppTitle = apptitle;
    gSDLAppExec  = execname;

    std::string conf_dir     = "";
    std::string resource_dir = "data/";
    std::string fonts_dir    = "data/fonts/";
    std::string shaders_dir  = "data/shaders/";

#ifdef _WIN32

    char szAppPath[MAX_PATH];
    GetModuleFileName(0, szAppPath, MAX_PATH);

    // Extract directory
    std::string winexepath = std::string(szAppPath);

    int pos = winexepath.rfind("\\");

    std::string path = winexepath.substr(0, pos+1);
    conf_dir     = path + std::string("\\");
    resource_dir = path + std::string("\\data\\");
    fonts_dir    = path + std::string("\\data\\fonts\\");
    shaders_dir  = path + std::string("\\data\\shaders\\");
#else
    //get working directory
    char cwd_buff[1024];

    if(getcwd(cwd_buff, 1024) == cwd_buff) {
        conf_dir     = std::string(cwd_buff) + std::string("/");
        resource_dir = std::string(cwd_buff) + std::string("/") + resource_dir;
        fonts_dir    = std::string(cwd_buff) + std::string("/") + fonts_dir;
        shaders_dir  = std::string(cwd_buff) + std::string("/") + shaders_dir;
    }

#endif

#ifdef SDLAPP_CONF_DIR
    if (SDLAppDirExists(SDLAPP_CONF_DIR)) {
        conf_dir = SDLAPP_CONF_DIR;
    }
#endif

#ifdef SDLAPP_RESOURCE_DIR
    if (SDLAppDirExists(SDLAPP_RESOURCE_DIR)) {
        resource_dir = SDLAPP_RESOURCE_DIR;
        fonts_dir    = SDLAPP_RESOURCE_DIR + std::string("/fonts/");
        shaders_dir  = SDLAPP_RESOURCE_DIR + std::string("/shaders/");
    }
    else if(!exepath.empty()) {
        // make resource path relative to the directory of the executable
        // if the resource directory doesn't exist
        size_t pos = exepath.rfind("/");
        if (pos != std::string::npos) {
            std::string path = exepath.substr(0, pos+1);
            resource_dir = path + std::string("data/");
            fonts_dir    = path + std::string("data/fonts/");
            shaders_dir  = path + std::string("data/shaders/");
        }
    }
#endif

#ifdef SDLAPP_FONT_DIR
    if (SDLAppDirExists(SDLAPP_FONT_DIR)) {
        fonts_dir    = SDLAPP_FONT_DIR;
    }
#endif

    resource_dir  = SDLAppAddSlash(resource_dir);
    conf_dir      = SDLAppAddSlash(conf_dir);
    fonts_dir     = SDLAppAddSlash(fonts_dir);
    shaders_dir   = SDLAppAddSlash(shaders_dir);

    texturemanager.setDir(resource_dir);
    fontmanager.setDir(fonts_dir);
    shadermanager.setDir(shaders_dir);

    gSDLAppResourceDir = resource_dir;
    gSDLAppConfDir     = conf_dir;
    gSDLAppShaderDir   = shaders_dir;

    fontmanager.init();
}

void SDLAppParseArgs(int argc, char *argv[], int* xres, int* yres, bool* fullscreen, std::vector<std::string>* otherargs) {

    for (int i=1; i<argc; i++) {
        debugLog("argv[%d] = %s", i, argv[i]);

        std::string args(argv[i]);

        if (args == "-f") {
            *fullscreen = true;
            continue;
        }
        else if (args == "-w") {
            *fullscreen = false;
            continue;
        }

        //get video mode
        if(args.size()>1 && args[0] == '-' && args.rfind("x") != std::string::npos) {

            std::string displayarg = args;

            while(displayarg.size()>1 && displayarg[0] == '-') {
                displayarg = displayarg.substr(1, displayarg.size()-1);
            }

            size_t x = displayarg.rfind("x");

            if(x != std::string::npos) {
                std::string widthstr  = displayarg.substr(0, x);
                std::string heightstr = displayarg.substr(x+1);

                int width = atoi(widthstr.c_str());
                int height = atoi(heightstr.c_str());

                if(width>0 && height>0) {
                    debugLog("w=%d, h=%d",width,height);

                    *xres = width;
                    *yres = height;
                    continue;
                }
            }
        }

        // non display argument
        if(otherargs != 0) {
            otherargs->push_back(args);
        }
    }
}

SDLApp::SDLApp() {
    fps=0;
    return_code=0;
    appFinished=false;
    min_delta_msec = 8;
}

void SDLApp::updateFramerate() {
    if(fps_updater>0) {
        fps = (float)frame_count / (float)fps_updater * 1000.0f;
    } else {
        fps = 0;
    }
    fps_updater = 0;
    frame_count = 0;
}

bool SDLApp::isFinished() {
    return appFinished;
}

int SDLApp::returnCode() {
    return return_code;
}

void SDLApp::stop(int return_code) {
    this->return_code = return_code;
    appFinished=true;
}

bool SDLApp::handleEvent(SDL_Event& event) {

    switch(event.type) {
        case SDL_QUIT:
            quit();
            break;

        case SDL_MOUSEMOTION:
            mouseMove(&event.motion);
            break;

#if SDL_VERSION_ATLEAST(2,0,0)
        case SDL_TEXTINPUT:
            textInput(&event.text);
             break;

        case SDL_TEXTEDITING:
            textEdit(&event.edit);
            break;

        case SDL_MOUSEWHEEL:
            mouseWheel(&event.wheel);
            break;

        case SDL_WINDOWEVENT:
            if(event.window.event == SDL_WINDOWEVENT_RESIZED) {
                resize(event.window.data1, event.window.data2);
            }
            break;
#else
        case SDL_VIDEORESIZE:
            resize(event.resize.w, event.resize.h);
            break;
#endif

        case SDL_MOUSEBUTTONDOWN:
        case SDL_MOUSEBUTTONUP:
            mouseClick(&event.button);
            break;

        case SDL_KEYDOWN:
        case SDL_KEYUP:
            keyPress(&event.key);
            break;

        default:
            return false;
    }

    return true;
}

int SDLApp::run() {

    Uint32 msec=0, last_msec=0, buffer_msec=0, total_msec = 0;

    frame_count = 0;
    fps_updater = 0;

    if(!appFinished) init();

    msec = SDL_GetTicks();
    last_msec = msec;

#if SDL_VERSION_ATLEAST(2,0,0)
        //text input seems to be enabled by default, turn it off
        SDL_StopTextInput();
#endif

    while(!appFinished) {
        last_msec = msec;
        msec      = SDL_GetTicks();

        Uint32 delta_msec = msec - last_msec;

        // cant have delta ticks be less than 8ms
        buffer_msec += delta_msec;
        if(buffer_msec < min_delta_msec) {
            SDL_Delay(1);
            continue;
        }

        delta_msec = buffer_msec;
        buffer_msec =0;

        //determine time elapsed since last time we were here
        total_msec += delta_msec;

        float t  = total_msec / 1000.0f;
        float dt = delta_msec / 1000.0f;

        fps_updater += delta_msec;

        //update framerate if a second has passed
        if (fps_updater >= 1000) {
            updateFramerate();
        }

        //process new events
        SDL_Event event;
        while ( SDL_PollEvent(&event) ) {
            handleEvent(event);
        }

        update(t, dt);

        //update display
        display.update();
        frame_count++;
    }

    return return_code;
}
