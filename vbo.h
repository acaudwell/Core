/*
    Copyright (c) 2011 Andrew Caudwell (acaudwell@gmail.com)
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

#ifndef CORE_VBO_H
#define CORE_VBO_H


#include <stack>
#include <vector>

#include "gl.h"
#include "vectors.h"
#include "logger.h"

class VBO {
public:
    GLuint id;
    GLenum buffer_type;

    int capacity;

    VBO(GLenum buffer_type = GL_ARRAY_BUFFER) : buffer_type(buffer_type) {
        capacity = 0;
        id       = 0;
    }

    ~VBO() {
        unload();
    }

    void init() {
        if(!id) glGenBuffers(1, &id);
    }

    void unload() {
        capacity = 0;
        if(id != 0) {
            glDeleteBuffers(1, &id);
            id = 0;
        }
    }

    void bind() {
        if(!id) init();
        glBindBuffer(buffer_type, id);
    }

    void buffer(int item_count, int item_size, int item_capacity, GLvoid* data, GLenum usage) {

        bind();

        if(capacity < item_count) {
            capacity = item_capacity;
            glBufferData(buffer_type, capacity * item_size, data, usage);
        } else {
            glBufferSubData(buffer_type, 0, item_count * item_size, data);
        }

        unbind();
    }

    void unbind() {
        glBindBuffer(buffer_type, 0);
    }
};

//note this should be 32 bytes (8x4 bytes)
class quadbuf_vertex {
public:
    quadbuf_vertex() {};
    quadbuf_vertex(const vec2& pos, const vec4& colour, const vec2& texcoord) : pos(pos), colour(colour), texcoord(texcoord) {};

    vec2 pos;
    vec4 colour;
    vec2 texcoord;
};

//maintain ranges corresponding to each texture
class quadbuf_tex {
public:
    quadbuf_tex() {};
    quadbuf_tex(int start_index, GLuint textureid) : start_index(start_index), textureid(textureid) {};
    int start_index;
    GLuint textureid;
};

class quadbuf {

    quadbuf_vertex* data;
    int vertex_capacity;

    std::vector<quadbuf_tex> textures;

    VBO buf;

    int vertex_count;

    void resize(int new_size);
public:
    quadbuf(int data_size = 0);
    ~quadbuf();

    void unload();
    void reset();

    size_t vertices();
    size_t capacity();
    size_t texture_changes();

    void add(GLuint textureid, const vec2& pos, const vec2& dims, const vec4& colour);
    void add(GLuint textureid, const vec2& pos, const vec2& dims, const vec4& colour, const vec4& texcoord);
    void add(GLuint textureid, const quadbuf_vertex& v1, const quadbuf_vertex& v2, const quadbuf_vertex& v3, const quadbuf_vertex& v4);

    void update();
    void draw();
};

#endif
