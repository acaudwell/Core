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

#ifndef LOGGER_H
#define LOGGER_H

#include <map>
#include <deque>
#include <string>

enum logger_level { LOG_LEVEL_OFF, LOG_LEVEL_ERROR, LOG_LEVEL_CONSOLE, LOG_LEVEL_INFO, LOG_LEVEL_SCRIPT, LOG_LEVEL_DEBUG, LOG_LEVEL_WARN, LOG_LEVEL_PEDANTIC };

class LoggerMessage {

public:
    LoggerMessage(int level, const std::string& message);

    int level;
    std::string message;
};

class Logger {
protected:
    std::deque<LoggerMessage> history;
    int hist_capacity;
    FILE* stream;
    int level;
    static Logger* default_logger;
    int message_count;
public:
    static Logger* getDefault();

    const std::deque<LoggerMessage>& getHistory() const;

    void setLevel(int level)   { this->level = level; };
    int getLevel() const { return level; }

    int getMessageCount();

    void setHistoryCapacity(int hist_capacity);

    Logger(int level, FILE* stream, int history_capacity = 0);

    void init(int level, FILE* stream, int history_capacity);

    void message(int level, const std::string& message);
};

void warnLog(const char *args, ...);
void debugLog(const char *args, ...);
void infoLog(const char *args, ...);
void errorLog(const char *args, ...);
void consoleLog(const char *args, ...);
void scriptLog(const char *args, ...);
void pedanticLog(const char *args, ...);

#endif
