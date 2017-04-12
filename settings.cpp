/*
    Copyright (C) 2009 Andrew Caudwell (acaudwell@gmail.com)

    This program is free software; you can redistribute it and/or
    modify it under the terms of the GNU General Public License
    as published by the Free Software Foundation; either version
    3 of the License, or (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/
#include "sdlapp.h"
#include "settings.h"
#include "regex.h"
#include "timezone.h"

Regex SDLAppSettings_rect_regex("^([0-9.]+)x([0-9.]+)$");
Regex SDLAppSettings_viewport_regex("^([0-9.]+)x([0-9.]+)(!)?$");

SDLAppSettings::SDLAppSettings() {
    setDisplayDefaults();

    default_section_name = "settings";

    //conf entries in other sections
    conf_sections["viewport"]           = "display";
    conf_sections["windowed"]           = "display";
    conf_sections["fullscreen"]         = "display";
    conf_sections["frameless"]          = "display";
    conf_sections["screen"]             = "display";
    conf_sections["window-position"]    = "display";
    conf_sections["multi-sampling"]     = "display";
    conf_sections["output-ppm-stream"]  = "display";
    conf_sections["output-framerate"]   = "display";
    conf_sections["transparent"]        = "display";
    conf_sections["no-vsync"]           = "display";

    //translate args
    arg_aliases["f"]   = "fullscreen";
    arg_aliases["w"]   = "windowed";
    arg_aliases["o"]   = "output-ppm-stream";
    arg_aliases["r"]   = "output-framerate";

    //boolean args
    arg_types["viewport"]          = "string";
    arg_types["windowed"]          = "bool";
    arg_types["screen"]            = "int";
    arg_types["window-position"]   = "string";
    arg_types["fullscreen"]        = "bool";
    arg_types["frameless"]         = "bool";
    arg_types["transparent"]       = "bool";
    arg_types["multi-sampling"]    = "bool";
    arg_types["no-vsync"]          = "bool";
    arg_types["output-ppm-stream"] = "string";
    arg_types["output-framerate"]  = "int";

}

void SDLAppSettings::setDisplayDefaults() {
    display_width  = 1024;
#ifdef __APPLE__
    display_height = 640;
#else
    display_height = 768;
#endif
    fullscreen     = false;
    frameless      = false;
    multisample    = false;
    transparent    = false;
    resizable      = true;
    vsync          = true;

    screen = -1;

    window_x = -1;
    window_y = -1;

    output_ppm_filename = "";
    output_framerate    = 60;
}

void SDLAppSettings::exportDisplaySettings(ConfFile& conf) {

    ConfSection* section = new ConfSection("display");

    char viewportbuff[256];
    snprintf(viewportbuff, 256, "%dx%d%s", display_width, display_height, resizable ? "" : "!" );

    std::string viewport = std::string(viewportbuff);

    section->setEntry(new ConfEntry("viewport", viewport));

    if(fullscreen)
        section->setEntry(new ConfEntry("fullscreen", fullscreen));
    else {
        if(frameless)
            section->setEntry(new ConfEntry("frameless", frameless));

        if(window_x >= 0 && window_y >= 0) {
            char windowbuff[256];
            snprintf(windowbuff, 256, "%dx%d", window_x, window_y);
            section->setEntry(new ConfEntry("window-position", std::string(windowbuff)));
        }
    }

    if(screen > 0) {
        section->setEntry(new ConfEntry("screen", screen));
    }

    if(multisample)
        section->setEntry(new ConfEntry("multi-sampling", multisample));

    if(!vsync) {
        section->setEntry(new ConfEntry("no-vsync", true));
    }

    conf.setSection(section);
}

bool SDLAppSettings::parseRectangle(const std::string& value, int& x, int& y) {

    std::vector<std::string> matches;

    if(SDLAppSettings_rect_regex.match(value, &matches)) {

        x = atoi(matches[0].c_str());
        y = atoi(matches[1].c_str());

        return true;
    }

    return false;
}

bool SDLAppSettings::parseViewport(const std::string& value, int& x, int& y, bool& no_resize) {

    std::vector<std::string> matches;

    if(SDLAppSettings_viewport_regex.match(value, &matches)) {
        x = atoi(matches[0].c_str());
        y = atoi(matches[1].c_str());

        if(matches.size()>2) no_resize = true;

        if(x>0 && y>0) return true;
    }

    return false;
}


void SDLAppSettings::parseArgs(int argc, char *argv[], ConfFile& conffile, std::vector<std::string>* files) {

    std::vector<std::string> arguments;

    for (int i=1; i<argc; i++) {
        arguments.push_back(argv[i]);
    }

    parseArgs(arguments, conffile, files);
}

//apply args to a conf file
void SDLAppSettings::parseArgs(const std::vector<std::string>& arguments, ConfFile& conffile, std::vector<std::string>* files) {

    std::map<std::string, std::string>::iterator findit;

    for(int i=0;i<arguments.size();i++) {
        std::string args = arguments[i];

        //remove leading hyphens
        bool is_option = false;

        while(args.size()>1 && args[0] == '-') {
            args = args.substr(1, args.size()-1);
            is_option = true;
        }

        if(args.size()==0) continue;

        if(!is_option) {
            if(files!=0) {
                files->push_back(args);
            }
            continue;
        }

        //translate args with aliases
        if((findit = arg_aliases.find(args)) != arg_aliases.end()) {
            args = findit->second;
        }

        //NUMBERxNUMBER is a magic alias for viewport
        if(args.size()>1 && args.rfind("x") != std::string::npos) {

            std::string displayarg = args;

            int width  = 0;
            int height = 0;
            bool no_resize = false;

            if(parseViewport(displayarg, width, height, no_resize)) {
                if(width>0 && height>0) {

                    ConfSection* display_settings = conffile.getSection("display");

                    if(!display_settings) {
                        display_settings = conffile.addSection("display");
                    }

                    display_settings->setEntry("viewport", displayarg);

                    continue;
                }
            }
        }

        //get type

        std::string arg_type;
        if((findit = arg_types.find(args)) != arg_types.end()) {
            arg_type = findit->second;
        } else {
            std::string unknown_option = std::string("unknown option ") + args;
            throw ConfFileException(unknown_option, "", 0);
        }

        //get value (or set to true for booleans)

        std::string argvalue;
        if(arg_type == "bool") argvalue = "true";
        else if((i+1)<arguments.size()) argvalue = arguments[++i];

        //determine section
        std::string section_name = default_section_name;
        if((findit = conf_sections.find(args)) != conf_sections.end()) {
            section_name = findit->second;
        }

        //command line options dont go into the conf file
        if(section_name == "command-line") {
            commandLineOption(args, argvalue);
            continue;
        }

        //get section(s) of this type

        ConfSectionList* sections = conffile.getSections(section_name);

        if(sections == 0) {
            conffile.addSection(section_name);
            sections = conffile.getSections(section_name);
        }

        //apply to section

        for(ConfSectionList::iterator it = sections->begin(); it != sections->end(); it++) {

            ConfSection* section = *it;

            if(arg_type == "multi-value") {
                section->addEntry(args, argvalue);
            } else {
                section->setEntry(args, argvalue);
            }
        }
    }
}

bool SDLAppSettings::parseDateTime(const std::string& datetime, time_t& timestamp) {

    int timezone_offset = 0;

    Regex timestamp_regex("^(\\d{4})-(\\d{2})-(\\d{2})(?: (\\d{1,2}):(\\d{2})(?::(\\d{2}))?)?(?: ([+-])(\\d{1,2}))?$");

    std::vector<std::string> results;

    if(!timestamp_regex.match(datetime, &results) || results.size() < 3) return false;

    struct tm timeinfo;
    memset(&timeinfo, 0, sizeof(timeinfo));

    timeinfo.tm_isdst = -1;
    timeinfo.tm_year = atoi(results[0].c_str()) - 1900;
    timeinfo.tm_mon  = atoi(results[1].c_str()) - 1;
    timeinfo.tm_mday = atoi(results[2].c_str());

    // optional: hours, minutes and seconds
    if(results.size() >= 5) {
        timeinfo.tm_hour = atoi(results[3].c_str());
        timeinfo.tm_min  = atoi(results[4].c_str());
        if(results.size() >= 6) {
            timeinfo.tm_sec  = atoi(results[5].c_str());
        }
    }

    // optional: timezone (optional)
    if(results.size() >= 8) {

        int tz_hour = atoi(results[7].c_str());
        int tz_min  = 0;

        if(results.size() >= 9) {
            tz_min = atoi(results[8].c_str());
        }

        int tz_offset = tz_hour * 3600 + tz_min * 60;

        if(results[6] == "-") {
            tz_offset = -tz_offset;
        }
        
        timestamp = mktime_utc(&timeinfo);
        
        timestamp -= tz_offset;
        
    } else {
        timestamp = mktime(&timeinfo);
    }

    return true;
}

void SDLAppSettings::importDisplaySettings(ConfFile& conffile) {

    setDisplayDefaults();

    ConfSection* display_settings = conffile.getSection("display");

    if(display_settings == 0) return;

    ConfEntry* entry = 0;

    bool viewport_specified = false;

    if((entry = display_settings->getEntry("viewport")) != 0) {

        std::string viewport = entry->getString();

        int width  = 0;
        int height = 0;
        bool no_resize = false;

        if(parseViewport(viewport, width, height, no_resize)) {
            display_width   = width;
            display_height  = height;
            if(no_resize) resizable = false;

            viewport_specified = true;
        } else {
            conffile.invalidValueException(entry);
        }
    }

    if((entry = display_settings->getEntry("window-position")) != 0) {
        std::string window_position = entry->getString();

        if(!parseRectangle(window_position, window_x, window_y)) {
            conffile.invalidValueException(entry);
        }
    }

    if((entry = display_settings->getEntry("screen")) != 0) {
        screen = entry->getInt();

        if(screen < 1) {
            conffile.invalidValueException(entry);
        }
    }

    if(display_settings->getBool("multi-sampling")) {
        multisample = true;
    }

    if(display_settings->getBool("fullscreen")) {
        fullscreen = true;
    }

    if(display_settings->getBool("windowed")) {
        fullscreen = false;
    }

    if(display_settings->getBool("frameless") && !fullscreen) {
        frameless = true;
    }

    // default to use desktop resolution for fullscreen unless specified
    if(fullscreen && !viewport_specified) {
        display_width  = 0;
        display_height = 0;
    }

    if(display_settings->getBool("transparent")) {
        transparent = true;
    }

    if(display_settings->getBool("no-vsync")) {
        vsync = false;
    }

    if((entry = display_settings->getEntry("output-ppm-stream")) != 0) {

        if(!entry->hasValue()) {
            conffile.entryException(entry, "specify ppm output file or '-' for stdout");
        }

        output_ppm_filename = entry->getString();

    }

    if((entry = display_settings->getEntry("output-framerate")) != 0) {

        if(!entry->hasValue()) {
             conffile.entryException(entry, "specify framerate (25,30,60)");
        }

        output_framerate = entry->getInt();

        if(   output_framerate != 25
            && output_framerate != 30
            && output_framerate != 60) {
            conffile.entryException(entry, "supported framerates are 25,30,60");
        }
    }
}
