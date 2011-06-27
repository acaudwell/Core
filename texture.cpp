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

TextureManager texturemanager;

// texture manager

TextureManager::TextureManager() : ResourceManager() {
}

TextureResource* TextureManager::grabFile(const std::string& filename, bool mipmaps, bool clamp, bool trilinear) {
    return grab(filename, mipmaps, clamp, trilinear, true);
}

TextureResource* TextureManager::grab(const std::string& filename, bool mipmaps, bool clamp, bool trilinear, bool external) {

    Resource* r = resources[filename];

    if(r==0) {
        r = new TextureResource(filename, mipmaps, clamp, trilinear, external);

        resources[filename] = r;
    }
    r->addref();

    return (TextureResource*)r;
}

TextureResource* TextureManager::emptyTexture(const std::string& resource_name, int width, int height, GLenum format) {

    Resource* r = resources[resource_name];

    if(r!=0) {
        throw SDLAppException("A texture resource already exists under the name '%s'", resource_name.c_str());
    }

    r = new TextureResource(width, height, format);
    r->setResourceName(resource_name);
    
    resources[resource_name] = r;

    r->addref();

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

// texture resource

TextureResource::TextureResource(const std::string& filename, bool mipmaps, bool clamp, bool trilinear, bool external) : Resource(filename) {

    this->mipmaps   = mipmaps;
    this->clamp     = clamp;
    this->trilinear = trilinear;

    textureid = 0;

    //if doesnt have an absolute path, look in resource dir
    if(!external && !(filename.size() > 2 && filename[1] == ':') && !(filename.size() > 1 && filename[0] == '/')) {
        this->filename = texturemanager.getDir() + filename;
    } else {
        this->filename = filename;
    } 
   
    load();
}

TextureResource::TextureResource(int width, int height, GLenum format) {
    this->w      = width;
    this->h      = height;
    this->format = format;

    mipmaps   = false;
    clamp     = false;
    
    // ? 
    trilinear = false;
    
    load();
}

TextureResource::~TextureResource() {
    unload();
}

void TextureResource::unload() {
    if(textureid!=0) glDeleteTextures(1, &textureid);
    textureid=0;
}

void TextureResource::load() {

    if(textureid != 0) unload();
    
    if(filename.empty() && w!=0 && format != 0) {
        textureid = display.emptyTexture(w, h, format);    
        return;
    }
    
    debugLog("creating texture from %s\n", filename.c_str());

    SDL_Surface *surface = IMG_Load(filename.c_str());

    if(surface==0) throw TextureException(filename);

    w = surface->w;
    h = surface->h;

    //figure out image colour order
    format = colourFormat(surface);

    if(format==0) throw TextureException(filename);

    textureid = display.createTexture(w, h, mipmaps, clamp, trilinear, format, (unsigned int*) surface->pixels);

    SDL_FreeSurface(surface);
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
