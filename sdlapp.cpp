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
HWND gSDLAppConsoleWindow = 0;
bool using_parent_console = false;

bool SDLAppAttachToConsole() {
    if(using_parent_console)      return true;
    if(gSDLAppConsoleWindow != 0) return false;

    // if TERM is set to msys, try and attach to console
    // could possibly add other supported TERMs here if there are any

    char* term = getenv("TERM");
    if(!term || strcmp(term, "msys")!=0) return false;

    if(AttachConsole(ATTACH_PARENT_PROCESS)) {

        // send stdout to console unless already redirected
        if (_fileno(stdout) == -1 || _get_osfhandle(fileno(stdout)) == -1) {
            freopen("conout$","w", stdout);
        }

        // send stderr to console unless already redirected
        if (_fileno(stderr) == -1 || _get_osfhandle(fileno(stderr)) == -1) {
            freopen("conout$","w", stderr);
        }

        using_parent_console = true;
    }

    return using_parent_console;
}

void SDLAppCreateWindowsConsole() {
    if(using_parent_console || gSDLAppConsoleWindow != 0) return;

    //try to attach to the available console if there is one

    if(SDLAppAttachToConsole()) return;

    //create a console on Windows so users can see messages
    //find an available name for our window

    char consoleTitle[512];
    int console_suffix = 0;
    sprintf(consoleTitle, "%s Console", gSDLAppTitle.c_str());

    while(FindWindow(0, consoleTitle)) {
        sprintf(consoleTitle, "%s Console %d", gSDLAppTitle.c_str(), ++console_suffix);
    }

    AllocConsole();
    SetConsoleTitle(consoleTitle);

    //redirect streams to console
    freopen("conin$", "r", stdin);
    freopen("conout$","w", stdout);
    freopen("conout$","w", stderr);

    gSDLAppConsoleWindow = 0;

    //wait for our console window
    while(gSDLAppConsoleWindow==0) {
        gSDLAppConsoleWindow = FindWindow(0, consoleTitle);
        SDL_Delay(100);
    }

    //disable the close button so the user cant crash the application
    HMENU hm = GetSystemMenu(gSDLAppConsoleWindow, false);
    DeleteMenu(hm, SC_CLOSE, MF_BYCOMMAND);
}

void SDLAppResizeWindowsConsole(int height) {
    if(gSDLAppConsoleWindow !=0) {
        RECT windowRect;
        if(GetWindowRect(gSDLAppConsoleWindow, &windowRect)) {
            float width = windowRect.right - windowRect.left;
            MoveWindow(gSDLAppConsoleWindow,windowRect.left,windowRect.top,width,height,true);
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
    SDLAppCreateWindowsConsole();
#endif

    printf("%s\n", msg.c_str());

#ifdef _WIN32
    if(gSDLAppConsoleWindow) {
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
    SDLAppCreateWindowsConsole();
#endif

    fprintf(stderr, "%s: %s\n", gSDLAppExec.c_str(), error.c_str());
    fprintf(stderr, "Try '%s --help' for more information.\n\n", gSDLAppExec.c_str());

#ifdef _WIN32
    if(gSDLAppConsoleWindow) {
        fprintf(stderr, "Press Enter\n");
        getchar();
    }
#endif

    exit(1);
}

#if !defined(_WIN32) && !defined(__APPLE__)
static Atom xa_targets;
static Atom xa_clipboard;

void SDLApp::initX11ClipboardEventFilter() {
    SDL_SysWMinfo wininfo;
    SDL_VERSION(&wininfo.version);
    SDL_GetWMInfo(&wininfo);    
    
    SDL_EventState(SDL_SYSWMEVENT, SDL_ENABLE);
    SDL_SetEventFilter(SDLApp::X11ClipboardEventFilter);

    xa_targets   = XInternAtom(wininfo.info.x11.display, "TARGETS",   False);
    xa_clipboard = XInternAtom(wininfo.info.x11.display, "CLIPBOARD", False);
}

int SDLApp::X11ClipboardEventFilter(const SDL_Event *event) {

    if ( event->type != SDL_SYSWMEVENT ) return 1;
    
    if( event->syswm.msg->event.xevent.type == SelectionRequest) {

        SDL_SysWMinfo wininfo;
        SDL_VERSION(&wininfo.version);
        SDL_GetWMInfo(&wininfo);

        XSelectionRequestEvent *req;
        XEvent snotify;

        req = &event->syswm.msg->event.xevent.xselectionrequest;

        snotify.xselection.type      = SelectionNotify;

        snotify.xselection.display   = req->display;
        snotify.xselection.selection = req->selection;
        snotify.xselection.requestor = req->requestor;
        snotify.xselection.time      = req->time;
        
        snotify.xselection.target    = None;
        snotify.xselection.property  = None;

        Atom supported_targets[] = {
            xa_targets,
            XA_STRING
        };
        
        if ( req->target == xa_targets ) {

            XChangeProperty (req->display, req->requestor, req->property, XA_ATOM, 32, PropModeReplace, (unsigned char*) supported_targets, sizeof(supported_targets) / sizeof (Atom));
            snotify.xselection.property = req->property;

        } else if( req->target == XA_STRING ) {

            unsigned char *selection_data;
            unsigned long selection_length;
            unsigned long overflow;
            int selection_format;
                
            if ( XGetWindowProperty(wininfo.info.x11.display, DefaultRootWindow(wininfo.info.x11.display),
                                    XA_CUT_BUFFER0, 0, INT_MAX/4, False, req->target,
                                    &snotify.xselection.target, &selection_format,
                                    &selection_length, &overflow, &selection_data) == Success ) {

                if ( snotify.xselection.target == req->target ) {

                    XChangeProperty(wininfo.info.x11.display, req->requestor, req->property,
                        snotify.xselection.target, selection_format, PropModeReplace, selection_data, selection_length);

                    snotify.xselection.property = req->property;
                }

                XFree(selection_data);
            }
        }
        
        XSendEvent(wininfo.info.x11.display,req->requestor,False,0,&snotify);
        XSync(wininfo.info.x11.display, False);           
    }

    return 1;
}

#endif

bool SDLApp::getClipboardText(std::string& text) {

    SDL_SysWMinfo wininfo;
    SDL_VERSION(&wininfo.version);
    SDL_GetWMInfo(&wininfo);

#ifdef __APPLE__

#elifdef _WIN32
    if(!IsClipboardFormatAvailable(CF_TEXT) || !OpenClipboard(wininfo.window)) return false;

    HGLOBAL handle = GetClipboardData(CF_TEXT);

    if (!handle) {
        CloseClipboard();
        return false;
    }

    const char* global_str = (const char*) GlobalLock(handle);

    text.assign(global_str);

    GlobalUnlock(handle);

    CloseClipboard();

    return true;
#else
    Window owner;
    Atom selection;

    wininfo.info.x11.lock_func();
    
    owner = XGetSelectionOwner(wininfo.info.x11.display, xa_clipboard);
    
    wininfo.info.x11.unlock_func();

    if ( (owner == None) || (owner == wininfo.info.x11.window) ) {

        owner     = DefaultRootWindow(wininfo.info.x11.display);
        selection = XA_CUT_BUFFER0;
        
    } else {

        owner = wininfo.info.x11.window;

        wininfo.info.x11.lock_func();
                
        selection = XInternAtom(wininfo.info.x11.display, "SDL_SELECTION", False);
        
        XConvertSelection(wininfo.info.x11.display, xa_clipboard, XA_STRING, selection, owner, CurrentTime);

        wininfo.info.x11.unlock_func();
        
        int selection_response = 0;
        SDL_Event event;

        while ( !selection_response ) {
            SDL_WaitEvent(&event);

            if ( event.type == SDL_SYSWMEVENT ) {
                XEvent xevent = event.syswm.msg->event.xevent;

                if ( (xevent.type == SelectionNotify) && (xevent.xselection.requestor == owner) )
                    selection_response = 1;
            }
        }
    }
    
    wininfo.info.x11.lock_func();

    unsigned char *selection_data;
    unsigned long selection_length;
    unsigned long overflow;
    int selection_format;
    Atom selection_type;
    
    bool assigned = false;
    
    if ( XGetWindowProperty(wininfo.info.x11.display, owner, selection, 0, INT_MAX/4,
                            False, XA_STRING, &selection_type, &selection_format,
                       &selection_length, &overflow, &selection_data) == Success ) {

        if ( selection_type == XA_STRING ) {
            text.assign((const char*)selection_data);
            assigned = true;
        }

        XFree(selection_data);
    }

    wininfo.info.x11.unlock_func();

    return assigned;
#endif
    return false;
}

void SDLApp::setClipboardText(const std::string& text) {
    SDL_SysWMinfo wininfo;
    SDL_VERSION(&wininfo.version);
    SDL_GetWMInfo(&wininfo);
#ifdef __APPLE__
    
#elifdef _WIN32

    if (!OpenClipboard(wininfo.window)) return;

    HANDLE handle  = GlobalAlloc((GMEM_MOVEABLE|GMEM_DDESHARE), text.size());

    if(!handle) {
        CloseClipboard();
        return;
    }

    char* global_str = (char*) GlobalLock(handle);

    strcpy(global_str, text.c_str());

    GlobalUnlock(handle);

    EmptyClipboard();

    SetClipboardData(CF_TEXT, handle);

    CloseClipboard();

#else
         
    wininfo.info.x11.lock_func();        

    XChangeProperty(wininfo.info.x11.display, DefaultRootWindow(wininfo.info.x11.display), XA_CUT_BUFFER0, XA_STRING, 8, PropModeReplace, (unsigned char*) text.c_str(), text.size());
        
    if(XGetSelectionOwner(wininfo.info.x11.display, xa_clipboard) != wininfo.info.x11.window ) {
        XSetSelectionOwner(wininfo.info.x11.display, xa_clipboard, wininfo.info.x11.window, CurrentTime);
    }
    
    wininfo.info.x11.unlock_func();
#endif
}

void SDLAppInit(std::string apptitle, std::string execname) {
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
    std::string exepath = std::string(szAppPath);

    int pos = exepath.rfind("\\");

    std::string path = exepath.substr(0, pos+1);
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

    fontmanager.init();
}

void SDLAppParseArgs(int argc, char *argv[], int* xres, int* yres, bool* fullscreen, std::vector<std::string>* otherargs) {

    for (int i=1; i<argc; i++) {
        debugLog("argv[%d] = %s\n", i, argv[i]);

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
                    debugLog("w=%d, h=%d\n",width,height);

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

int SDLApp::run() {

#if !defined(_WIN32) && !defined(__APPLE__)
    SDLApp::initX11ClipboardEventFilter();
#endif

    Uint32 msec=0, last_msec=0, buffer_msec=0, total_msec = 0;

    frame_count = 0;
    fps_updater = 0;

    if(!appFinished) init();

    msec = SDL_GetTicks();
    last_msec = msec;

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

            switch(event.type) {
                case SDL_QUIT:
                    quit();
                    break;

                case SDL_MOUSEMOTION:
                    mouseMove(&event.motion);
                    break;

#if SDL_VERSION_ATLEAST(1,3,0)
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
                    mouseClick(&event.button);
                    break;

                case SDL_MOUSEBUTTONUP:
                    mouseClick(&event.button);
                    break;

                case SDL_KEYDOWN:
                    keyPress(&event.key);
                    break;

                case SDL_KEYUP:
                    keyPress(&event.key);
                    break;

                default:
                    break;
            }
        }

        update(t, dt);

        //update display
        display.update();
        frame_count++;
    }

    return return_code;
}
