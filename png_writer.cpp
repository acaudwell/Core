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

#include "png_writer.h"
#include "display.h"

#include <iostream>

#define PNG_SKIP_SETJMP_CHECK
#include <png.h>

PNGWriter::PNGWriter(int components)
    : components(components) {
    out = 0;
}

bool PNGWriter::open(const std::string& filename) {

    out = new std::ofstream(filename.c_str(), std::ios::out | std::ios::binary);

    if(out->fail()) {
        delete out;
        out = 0;
        return false;
    }

    return true;
}

void PNGWriter::close() {
    ((std::fstream*)out)->close();
}

void PNGWriter::setOutputStream(std::ostream* out) {
    this->out = out;
}

void PNGWriter::screenshot(const std::string& filename) {

    if(!open(filename)) return;

    std::vector<char> buffer;
    buffer.resize(display.width * display.height * components);

    capture(buffer);
    writePNG(buffer);

    close();
}

void PNGWriter::capture(std::vector<char>& buffer) {
    GLenum pixel_format = components == 4 ? GL_RGBA : GL_RGB;
    glPixelStorei(GL_PACK_ALIGNMENT, 1);
    glReadPixels(0, 0, display.width, display.height, pixel_format, GL_UNSIGNED_BYTE, &(buffer[0]));
}

void png_writer_write_data(png_structp png_ptr, png_bytep data, png_size_t length) {
    std::ostream* out = (std::ostream*) png_get_io_ptr(png_ptr);
    if (!out->write((char*)data, length)) png_error(png_ptr, "png_writer_write_data error");
}

void png_writer_flush_data(png_structp png_ptr) {
    std::ostream *out = (std::ostream*) png_get_io_ptr(png_ptr);
    if (!out->flush()) png_error(png_ptr, "png_writer_flush_data error");
}

void PNGWriter::writePNG(std::vector<char>& buffer) {

    png_structp png_ptr  = png_create_write_struct(PNG_LIBPNG_VER_STRING, 0, 0, 0);

    if(!png_ptr) throw PNGExporterException("png_create_write_struct failed");

    png_set_write_fn(png_ptr, out, png_writer_write_data, png_writer_flush_data);

    png_infop info_ptr = png_create_info_struct(png_ptr);

    if(!info_ptr) throw PNGExporterException("png_create_info_struct failed");

    if(setjmp(png_jmpbuf(png_ptr))) {
        throw PNGExporterException("setjmp failed");
    }

    int colour_type = (components == 4) ? PNG_COLOR_TYPE_RGB_ALPHA : PNG_COLOR_TYPE_RGB;

    png_set_IHDR(png_ptr, info_ptr, display.width, display.height, 8, colour_type, PNG_INTERLACE_NONE, PNG_COMPRESSION_TYPE_BASE, PNG_FILTER_TYPE_BASE);

    std::vector<png_bytep> rows(display.height);
    for (int i = 0; i < display.height; i++) {
        rows[i] = (png_bytep) &(buffer[(display.height-i-1) * components * display.width]);
    }

    png_write_info(png_ptr, info_ptr);
    png_write_image(png_ptr, &(rows[0]));
    png_write_end(png_ptr, 0);

    png_destroy_write_struct(&png_ptr, &info_ptr);
}

// PNGExporter

PNGExporter::PNGExporter(const std::string& filename) {

    buffer1.resize(display.width * display.height * 3);
    buffer2.resize(display.width * display.height * 3);

    buffer_shared_ptr = 0;

    thread_state = PNG_EXPORTER_WAIT;

    cond   = SDL_CreateCond();
    mutex  = SDL_CreateMutex();

    if(filename == "-") {
        writer.setOutputStream(&std::cout);
    } else {
       this->filename = filename;

       if(!writer.open(filename)) {
            throw PNGExporterException(filename);
        }
    }

#if SDL_VERSION_ATLEAST(2,0,0)
    thread = SDL_CreateThread( PNGExporter::startThread, "png_exporter", this );
#else
    thread = SDL_CreateThread( PNGExporter::startThread, this );
#endif
}

PNGExporter::~PNGExporter() {

    stop();

    SDL_DestroyCond(cond);
    SDL_DestroyMutex(mutex);

    buffer_shared_ptr = 0;

    if(!filename.empty()) {
        writer.close();
    }
}

int PNGExporter::startThread(void *exporter) {
    (static_cast<PNGExporter*>(exporter))->run();
    return 0;
}

void PNGExporter::run() {

    SDL_mutexP(mutex);

    while(thread_state != PNG_EXPORTER_EXIT) {

        thread_state = PNG_EXPORTER_WAIT;

        while (thread_state == PNG_EXPORTER_WAIT) {
            SDL_CondWait(cond, mutex);
        }

        if (thread_state != PNG_EXPORTER_WRITE) break;

        if (buffer_shared_ptr != 0) {
            writer.writePNG(*buffer_shared_ptr);
        }
    }

    thread_state = PNG_EXPORTER_STOPPED;

    SDL_mutexV(mutex);
}

void PNGExporter::stop() {
    if(!thread) return;

    if(thread_state == PNG_EXPORTER_STOPPED || thread_state == PNG_EXPORTER_EXIT) return;

    SDL_mutexP(mutex);

        thread_state = PNG_EXPORTER_EXIT;

        SDL_CondSignal(cond);

    SDL_mutexV(mutex);

    SDL_WaitThread(thread, 0);
    
    thread = 0;
}

void PNGExporter::capture() {

    std::vector<char>* next_pixel_ptr = (buffer_shared_ptr == &buffer1) ?
        &buffer2 : &buffer1;

    writer.capture(*next_pixel_ptr);

    SDL_mutexP(mutex);

        buffer_shared_ptr = next_pixel_ptr;
        thread_state      = PNG_EXPORTER_WRITE;

    SDL_CondSignal(cond);
    SDL_mutexV(mutex);
}
