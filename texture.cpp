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

#include "texture.h"
#include "display.h"

TextureManager texturemanager;

// texture manager

TextureManager::TextureManager() : ResourceManager() {
    resource_seq = 0;
    trilinear    = true;
}

TextureResource* TextureManager::grabFile(const std::string& filename, bool mipmaps, GLint wrap) {
    return grab(filename, mipmaps, wrap, true);
}

TextureResource* TextureManager::grab(const std::string& filename, bool mipmaps, GLint wrap, bool external) {

    TextureResource* r = 0;

    //look up this resource
    if((r = (TextureResource*) resources[filename]) != 0) {
        r->addref();
        return r;
    }

    r = new TextureResource(filename, mipmaps, wrap, external);
    r->load();

    addResource(r);

    return r;
}

void TextureManager::addResource(TextureResource* r) {

    if(r->resource_name.empty()) {
        char res_name[256];
        snprintf(res_name, 256, "__texture_resource_%d", ++resource_seq);

        std::string resource_name(res_name);

        r->setResourceName(resource_name);
    }

    resources[r->resource_name] = r;
    r->addref();
}

TextureResource* TextureManager::create(GLenum target) {

    TextureResource* r = new TextureResource();
    r->target = target;

    r->load();

    addResource(r);

    return (TextureResource*)r;

}

TextureResource* TextureManager::create(int width, int height, bool mipmaps, GLint wrap, GLenum format, GLubyte* data) {

    TextureResource* r = new TextureResource(width, height, mipmaps, wrap, format, data);
    r->load();

    addResource(r);

    return (TextureResource*)r;


}

void TextureManager::unload() {
    for(std::map<std::string, Resource*>::iterator it= resources.begin(); it!=resources.end();it++) {
        ((TextureResource*)it->second)->unload();
    }
}

void TextureManager::reload() {
    for(std::map<std::string, Resource*>::iterator it= resources.begin(); it!=resources.end();it++) {
        ((TextureResource*)it->second)->load();
    }
}

// TextureResource

TextureResource::TextureResource() {
    textureid = 0;
    w         = 0;
    h         = 0;
    format    = 0;
    data      = 0;
    wrap      = GL_CLAMP_TO_EDGE;
    target    = GL_TEXTURE_2D;
    mipmaps   = false;

    setDefaultFiltering();
}

TextureResource::TextureResource(const std::string& filename, bool mipmaps, GLint wrap, bool external) : Resource(filename) {

    this->mipmaps   = mipmaps;
    this->wrap     = wrap;

    data      = 0;
    format    = 0;
    textureid = 0;
    target    = GL_TEXTURE_2D;

    //if doesnt have an absolute path, look in resource dir
    if(!external && !(filename.size() > 2 && filename[1] == ':') && !(filename.size() > 1 && filename[0] == '/')) {
        this->filename = texturemanager.getDir() + filename;
    } else {
        this->filename = filename;
    }

    setDefaultFiltering();
}

TextureResource::TextureResource(int width, int height, bool mipmaps, GLint wrap, GLenum format, GLubyte* data) {
    this->w         = width;
    this->h         = height;
    this->data      = data;
    this->format    = format;
    this->mipmaps   = mipmaps;
    this->wrap      = wrap;
    this->target    = GL_TEXTURE_2D;

    textureid = 0;

    setDefaultFiltering();
}

TextureResource::~TextureResource() {
    unload();
}

void TextureResource::setDefaultFiltering() {

    if(mipmaps) {

        if(texturemanager.trilinear) {
            min_filter = GL_LINEAR_MIPMAP_LINEAR;
        } else {
            min_filter = GL_LINEAR_MIPMAP_NEAREST;
        }

        mag_filter = GL_LINEAR;

    } else {
        min_filter = GL_LINEAR;
        mag_filter = GL_LINEAR;
    }

}

void TextureResource::setFiltering(GLint min_filter, GLint mag_filter) {

    this->min_filter = min_filter;
    this->mag_filter = mag_filter;

    glTexParameteri(target, GL_TEXTURE_MIN_FILTER, min_filter);
    glTexParameteri(target, GL_TEXTURE_MAG_FILTER, mag_filter);
}

void TextureResource::setWrapStyle(GLint wrap) {

    this->wrap = wrap;

    glTexParameteri(target, GL_TEXTURE_WRAP_S, wrap);
    glTexParameteri(target, GL_TEXTURE_WRAP_T, wrap);
}


void TextureResource::unload() {
    if(textureid!=0) glDeleteTextures(1, &textureid);
    textureid=0;
}

void TextureResource::createTexture() {

    if(!textureid) glGenTextures(1, &textureid);

    glBindTexture(target, textureid);

    if(w != 0 && format != 0) {

        GLint internalFormat = 0;

        switch(format) {
            case GL_ALPHA:
                internalFormat = GL_ALPHA;
                break;
            case GL_LUMINANCE:
                internalFormat = GL_LUMINANCE;
                break;
            default:
                internalFormat = GL_RGBA;
                break;
        }

        if(mipmaps) {
            gluBuild2DMipmaps(target, internalFormat, w, h, format, GL_UNSIGNED_BYTE, data);
        } else {
            glTexImage2D(target, 0, internalFormat, w, h, 0, format, GL_UNSIGNED_BYTE, data);
        }
    }

    setFiltering(min_filter, mag_filter);
    setWrapStyle(wrap);
}

void TextureResource::reload() {
    load(true);
}

void TextureResource::load(bool reload) {

    if(textureid != 0) {
        if(!reload) return;
        debugLog("texture %d is being reloaded", textureid);
    }

    SDL_Surface *surface = 0;

    if(!filename.empty()) {
        debugLog("creating texture from %s", filename.c_str());

        surface = IMG_Load(filename.c_str());

        if(surface==0) throw TextureException(filename);

        w = surface->w;
        h = surface->h;

        //figure out image colour order
        format = colourFormat(surface);

        data = (GLubyte*) surface->pixels;

        if(format==0) throw TextureException(filename);
    }

    createTexture();

    if(surface != 0) {
        SDL_FreeSurface(surface);
        data = 0;
    }
}

GLenum TextureResource::colourFormat(SDL_Surface* surface) {

    int colours = surface->format->BytesPerPixel;
    int format  = 0;

    if (colours == 4) {
        if (surface->format->Rmask == 0x000000ff) {
            format = GL_RGBA;
        } else {
            format = GL_BGRA;
        }
    } else if (colours == 3) {
        if (surface->format->Rmask == 0x000000ff)
            format = GL_RGB;
        else
            format = GL_BGR;
    }

    return format;
}

void TextureResource::bind() {

    if(!textureid) load();
    glBindTexture(target, textureid);
}
