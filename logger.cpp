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

#include "logger.h"
     
std::map<int,std::string> log_levels = boost::assign::map_list_of
    (LOG_LEVEL_WARN,     " WARN" )
    (LOG_LEVEL_DEBUG,    "DEBUG" )
    (LOG_LEVEL_INFO,     " INFO" )
    (LOG_LEVEL_ERROR,    "ERROR" )
    (LOG_LEVEL_INFINITY, "????"  );

// LoggerMessage

LoggerMessage::LoggerMessage(int level, const std::string& message)
    : level(level), message(message) {
}

// Logger

Logger* Logger::getDefault() {
    return default_logger;
}

Logger::Logger(int level, FILE* stream, int hist_capacity) {
    init(level, stream, hist_capacity);
}

void Logger::init(int level, FILE* stream, int hist_capacity) {
    this->level         = level;
    this->stream        = stream;
    this->hist_capacity = hist_capacity;
}

void Logger::message(int level, const std::string& message) {

    if(this->level > level) return;

    if(stream != 0) {
        fprintf(stderr, "%s: %s\n", log_levels[level].c_str(), message.c_str());
    }

    if(!hist_capacity) return;

    while(history.size() >= hist_capacity) {
        history.pop_front();
    }

    history.push_back(LoggerMessage(level, message));
}

const std::deque<LoggerMessage>& Logger::getHistory() const {
    return history;
}

void Logger::setHistoryCapacity(int hist_capacity) {
    this->hist_capacity = hist_capacity;
}

void warnLog(const char *str, ...) {

    Logger* logger = Logger::getDefault();

    if(!logger || logger->getLevel() > LOG_LEVEL_WARN) return;

    char msgbuff[65536];

    va_list vl;

    va_start(vl, str);
        vsnprintf(msgbuff, 65536, str, vl);
    va_end(vl);

    logger->message( LOG_LEVEL_WARN, msgbuff );
}

void debugLog(const char *str, ...) {

    Logger* logger = Logger::getDefault();

    if(!logger || logger->getLevel() > LOG_LEVEL_DEBUG) return;

    char msgbuff[65536];

    va_list vl;

    va_start(vl, str);
        vsnprintf(msgbuff, 65536, str, vl);
    va_end(vl);

    logger->message( LOG_LEVEL_DEBUG, msgbuff );
}

void infoLog(const char *str, ...) {

    Logger* logger = Logger::getDefault();

    if(!logger || logger->getLevel() > LOG_LEVEL_INFO) return;

    char msgbuff[65536];

    va_list vl;

    va_start(vl, str);
        vsnprintf(msgbuff, 65536, str, vl);
    va_end(vl);

    logger->message( LOG_LEVEL_INFO, msgbuff );
}

void errorLog(const char *str, ...) {

    Logger* logger = Logger::getDefault();
    
    if(!logger || logger->getLevel() > LOG_LEVEL_ERROR) return;

    char msgbuff[65536];

    va_list vl;

    va_start(vl, str);
        vsnprintf(msgbuff, 65536, str, vl);
    va_end(vl);

    logger->message( LOG_LEVEL_ERROR, msgbuff );
}

Logger* Logger::default_logger = new Logger(LOG_LEVEL_ERROR, stderr, 0);
