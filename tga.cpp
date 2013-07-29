/*
    Copyright (c) 2012 Andrew Caudwell (acaudwell@gmail.com)
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

#include "tga.h"
#include "display.h"

#include <iostream>

TGAWriter::TGAWriter(int components)
    : components(components) {
    rle = true;
    out = 0;

    rle_count = 0;
    raw_count = 0;
}

void TGAWriter::writeRLE(int pixel_count, const char* pixel) {

    rle_count += pixel_count;

    while(pixel_count > 0) {
        int write_count = std::min(128, pixel_count);

        unsigned char counter_byte = (write_count-1) | 0x80;
        out->write((char*)&counter_byte, 1);
        out->write(pixel, components);

        pixel_count -= write_count;
    }
}

void TGAWriter::writeRaw(std::vector<char>& buffer, int start, int pixel_count) {

    raw_count += pixel_count;

    int written = 0;

    while(pixel_count > 0) {
        int write_count = std::min(128, pixel_count);

        char counter_byte = write_count-1;

        out->write((char*)&counter_byte, 1);

        for(int i=0; i < write_count; i++) {
            out->write(&(buffer[start+(i+written)*components]), components);
        }

        pixel_count -= write_count;
        written += write_count;
    }
}

bool TGAWriter::open(const std::string& filename) {

    out = new std::ofstream(filename.c_str(), std::ios::out | std::ios::binary);

    if(out->fail()) {
        delete out;
        out = 0;
        return false;
    }

    return true;
}

void TGAWriter::close() {
    ((std::fstream*)out)->close();
}

void TGAWriter::setOutputStream(std::ostream* out) {
    this->out = out;
}

void TGAWriter::screenshot(const std::string& filename) {

    if(!open(filename)) return;

    std::vector<char> buffer;
    buffer.resize(display.width * display.height * components);

    capture(buffer);
    writeTGA(buffer);

    close();
}

void TGAWriter::writeHeader() {

    char image_type = rle ? 10 : 2;

    const char tga_header[12] = { 0, 0, image_type, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
    short width           = display.width;
    short height          = display.height;
    char  imagedescriptor = components==4 ? 8 : 0;

    char bpp = components * 8;

    out->write(tga_header, 12);
    out->write((char*)&width,  sizeof(short));
    out->write((char*)&height, sizeof(short));
    out->write(&bpp, 1);
    out->write(&imagedescriptor, 1);
}

void TGAWriter::capture(std::vector<char>& buffer) {
    GLenum pixel_format = components == 4 ? GL_BGRA : GL_BGR;
    glReadPixels(0, 0, display.width, display.height, pixel_format, GL_UNSIGNED_BYTE, &(buffer[0]));
}

void TGAWriter::writeTGA(std::vector<char>& buffer) {
    writeHeader();
    writeScanlines(buffer, display.width, display.height);
}

void TGAWriter::writeScanlines(std::vector<char>& buffer, int width, int height) {

    if(!rle) {
        out->write(&(buffer[0]), buffer.size());
    } else {

        for(int y=0; y<height; y++) {

            int match_count  = 0;
            int last_written = -1;

            char* last_pixel;

            for(int x=0; x<width; x++) {

                char* pixel = &(buffer[(x + y*width)*components]);

                // nothing to compare to
                if(x == 0) {
                    last_pixel = pixel;
                    continue;
                }

                bool match =
                    (components == 4) ?
                        (pixel[0] == last_pixel[0] && pixel[1] == last_pixel[1] && pixel[2] == last_pixel[2] && pixel[3] == last_pixel[3])
                      : (pixel[0] == last_pixel[0] && pixel[1] == last_pixel[1] && pixel[2] == last_pixel[2]);

                if(match) {

                    match_count++;

                    // write any skipped pixels before the previous pixel
                    if(match_count > 0) {

                        int skipped = (x-(match_count)) - (last_written+1);

                        if(skipped > 0) {
                            writeRaw(buffer, (last_written + 1 + y*width)*components, skipped);
                            last_written = x-1;
                        }
                    }

                } else if(match_count > 0) {

                    writeRLE(match_count+1, last_pixel);
                    last_written = x-1;

                    match_count = 0;
                }

                last_pixel = pixel;
            }

            if(match_count > 0) {

                writeRLE(match_count+1, last_pixel);

            } else {

                int skipped = width - (last_written+1);

                if(skipped > 0) {
                    writeRaw(buffer, (last_written + 1 + y*width)*components, skipped);
                }
            }
        }
    }
}

// TGAExporter

TGAExporter::TGAExporter(const std::string& filename) {

    buffer1.resize(display.width * display.height * 3);
    buffer2.resize(display.width * display.height * 3);

    buffer_shared_ptr = 0;

    thread_state = TGA_EXPORTER_WAIT;

    cond   = SDL_CreateCond();
    mutex  = SDL_CreateMutex();

    if(filename == "-") {
        writer.setOutputStream(&std::cout);
    } else {
       this->filename = filename;

       if(!writer.open(filename)) {
            throw TGAExporterException(filename);
        }
    }

#if SDL_VERSION_ATLEAST(2,0,0)
    thread = SDL_CreateThread( TGAExporter::startThread, "tga_exporter", this );
#else
    thread = SDL_CreateThread( TGAExporter::startThread, this );
#endif
}

TGAExporter::~TGAExporter() {

    stop();

    SDL_DestroyCond(cond);
    SDL_DestroyMutex(mutex);

    buffer_shared_ptr = 0;

    if(!filename.empty()) {
        writer.close();
    }
}

int TGAExporter::startThread(void *exporter) {
    (static_cast<TGAExporter*>(exporter))->run();
    return 0;
}

void TGAExporter::run() {

    SDL_mutexP(mutex);

    while(thread_state != TGA_EXPORTER_EXIT) {

        thread_state = TGA_EXPORTER_WAIT;

        while (thread_state == TGA_EXPORTER_WAIT) {
            SDL_CondWait(cond, mutex);
        }

        if (thread_state != TGA_EXPORTER_WRITE) break;

        if (buffer_shared_ptr != 0) {
            writer.writeTGA(*buffer_shared_ptr);
        }
    }

    thread_state = TGA_EXPORTER_STOPPED;

    SDL_mutexV(mutex);
}

void TGAExporter::stop() {
    if(!thread) return;

    if(thread_state == TGA_EXPORTER_STOPPED || thread_state == TGA_EXPORTER_EXIT) return;

    SDL_mutexP(mutex);

        thread_state = TGA_EXPORTER_EXIT;

        SDL_CondSignal(cond);

    SDL_mutexV(mutex);
   
    SDL_WaitThread(thread, 0);
    
    thread = 0;
}

void TGAExporter::capture() {

    std::vector<char>* next_pixel_ptr = (buffer_shared_ptr == &buffer1) ?
        &buffer2 : &buffer1;

    writer.capture(*next_pixel_ptr);

    SDL_mutexP(mutex);

        buffer_shared_ptr = next_pixel_ptr;
        thread_state      = TGA_EXPORTER_WRITE;

    SDL_CondSignal(cond);
    SDL_mutexV(mutex);
}
