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

#ifndef TEXTURE_H
#define TEXTURE_H

#include "SDL_image.h"

#include "resource.h"
#include "gl.h"

#include <string>

class TextureException : public ResourceException {
public:
    TextureException(std::string& texture_file) : ResourceException(texture_file) {}
};

class TextureResource : public Resource {
    bool mipmaps;
    GLint wrap;
    GLint min_filter;
    GLint mag_filter;
    std::string filename;

    GLenum colourFormat(SDL_Surface* surface);
public:
    int w, h;
    GLenum target;
    GLenum format;
    GLuint textureid;
    GLubyte* data;

    TextureResource();
    TextureResource(int width, int height,  bool mipmaps, GLint wrap, GLenum format, GLubyte* data = 0);
    TextureResource(const std::string& filename, bool mipmaps, GLint wrap, bool external);

    void setWrapStyle(GLint wrap);

    void setFiltering(GLint min_filter, GLint mag_filter);
    void setDefaultFiltering();

    void bind();

    void createTexture();

    void reload();

    void load(bool reload = false);

    void unload();

    ~TextureResource();
};

class TextureManager : public ResourceManager {
    int  resource_seq;

    void addResource(TextureResource* r);
public:
    bool trilinear;

    TextureManager();

    TextureResource* grabFile(const std::string& filename, bool mipmaps = true, GLint wrap = GL_CLAMP_TO_EDGE);
    TextureResource*     grab(const std::string& filename, bool mipmaps = true, GLint wrap = GL_CLAMP_TO_EDGE, bool external_file = false);

    TextureResource* create(int width, int height, bool mipmaps, GLint wrap, GLenum format, GLubyte* data  = 0);
    TextureResource* create(GLenum target = GL_TEXTURE_2D);

    void unload();
    void reload();
};

extern TextureManager texturemanager;

#endif
