/*
    Copyright (c) 2013 Andrew Caudwell (acaudwell@gmail.com)
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

#ifndef PNG_WRITER_H
#define PNG_WRITER_H

#include "SDL_thread.h"

#include <fstream>
#include <vector>

class PNGWriter {
protected:
    std::ostream* out;
    size_t components;

    void init();
public:
    PNGWriter(int components = 3);

    bool open(const std::string& filename);
    void close();

    void setOutputStream(std::ostream* out);

    void screenshot(const std::string& filename);
    void capture(std::vector<char>& buffer);
    void writePNG(std::vector<char>& buffer);
};

enum png_exporter_state { PNG_EXPORTER_WAIT, PNG_EXPORTER_WRITE, PNG_EXPORTER_EXIT, PNG_EXPORTER_STOPPED };

class PNGExporter {
protected:
    PNGWriter writer;

    std::vector<char>* buffer_shared_ptr;

    std::vector<char> buffer1;
    std::vector<char> buffer2;

    SDL_cond*   cond;
    SDL_mutex*  mutex;
    SDL_Thread* thread;

    int thread_state;

    std::string filename;

    static int startThread(void *exporter);
public:
    PNGExporter(const std::string& filename);
    ~PNGExporter();

    void run();
    void stop();

    void capture();
};

class PNGExporterException : public std::exception {
protected:
    std::string filename;
public:
    PNGExporterException(const std::string& filename) : filename(filename) {};

    virtual ~PNGExporterException() throw () {};
    virtual const char* what() const throw() { return filename.c_str(); };
};

#endif
