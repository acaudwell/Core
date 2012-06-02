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

#include "vbo.h"

//quadbuf

quadbuf::quadbuf(int vertex_capacity) : vertex_capacity(vertex_capacity) {
    vertex_count = 0;

    data = vertex_capacity > 0 ? new quadbuf_vertex[vertex_capacity] : 0;

    //fprintf(stderr, "size of quadbuf_vertex = %d\n", sizeof(quadbuf_vertex));
}

quadbuf::~quadbuf() {
    if(data!=0) delete[] data;
}

void quadbuf::unload() {
    buf.unload();
}

void quadbuf::resize(int new_size) {

    quadbuf_vertex* _data = data;

    data = new quadbuf_vertex[new_size];

    for(int i=0;i<vertex_capacity;i++) {
        data[i] = _data[i];
    }

    vertex_capacity = new_size;

    if(_data != 0) delete[] _data;
}

void quadbuf::reset() {
    textures.resize(0);
    vertex_count = 0;
}

size_t quadbuf::vertices() {
    return vertex_count;
}

size_t quadbuf::capacity() {
    return vertex_capacity;
}

size_t quadbuf::texture_changes() {
    return textures.size();
}

vec4 quadbuf_default_texcoord(0.0f, 0.0f, 1.0f, 1.0f);

void quadbuf::add(GLuint textureid, const vec2& pos, const vec2& dims, const vec4& colour) {
    add(textureid, pos, dims, colour, quadbuf_default_texcoord);
}

void quadbuf::add(GLuint textureid, const vec2& pos, const vec2& dims, const vec4& colour, const vec4& texcoord) {
    //debugLog("%d: %.2f, %.2f, %.2f, %.2f\n", i, pos.x, pos.y, dims.x, dims.y);

    quadbuf_vertex v1(pos,                       colour, vec2(texcoord.x, texcoord.y));
    quadbuf_vertex v2(pos + vec2(dims.x, 0.0f), colour, vec2(texcoord.z, texcoord.y));
    quadbuf_vertex v3(pos + dims,                colour, vec2(texcoord.z, texcoord.w));
    quadbuf_vertex v4(pos + vec2(0.0f, dims.y), colour, vec2(texcoord.x, texcoord.w));

    int i = vertex_count;

    vertex_count += 4;

    if(vertex_count > vertex_capacity) {
        resize(vertex_count*2);
    }

    data[i]   = v1;
    data[i+1] = v2;
    data[i+2] = v3;
    data[i+3] = v4;

    if(textureid>0 && (textures.empty() || textures.back().textureid != textureid)) {
        textures.push_back(quadbuf_tex(i, textureid));
    }
}

void quadbuf::add(GLuint textureid, const quadbuf_vertex& v1, const quadbuf_vertex& v2, const quadbuf_vertex& v3, const quadbuf_vertex& v4) {

    int i = vertex_count;

    vertex_count += 4;

    if(vertex_count > vertex_capacity) {
        resize(vertex_count*2);
    }

    data[i]   = v1;
    data[i+1] = v2;
    data[i+2] = v3;
    data[i+3] = v4;

    if(textureid>0 && (textures.empty() || textures.back().textureid != textureid)) {
        textures.push_back(quadbuf_tex(i, textureid));
    }
}

void quadbuf::update() {
    if(vertex_count==0) return;

    //recreate buffer if less than the vertex_count
    buf.buffer( vertex_count, sizeof(quadbuf_vertex), vertex_capacity, &(data[0].pos.x), GL_DYNAMIC_DRAW );
}

void quadbuf::draw() {
    if(vertex_count==0) return;

    buf.bind();

    glEnableClientState(GL_VERTEX_ARRAY);
    glEnableClientState(GL_COLOR_ARRAY);
    glEnableClientState(GL_TEXTURE_COORD_ARRAY);

    glVertexPointer(2,   GL_FLOAT, sizeof(quadbuf_vertex), 0);
    glColorPointer(4,    GL_FLOAT, sizeof(quadbuf_vertex), (GLvoid*)8);  // offset pos (2x4 bytes)
    glTexCoordPointer(2, GL_FLOAT, sizeof(quadbuf_vertex), (GLvoid*)24); // offset pos + colour (2x4 + 4x4 bytes)

    int last_index = vertex_count-1;

    if(textures.empty()) {

         glDrawArrays(GL_QUADS, 0, vertex_count);

    } else {
        for(std::vector<quadbuf_tex>::iterator it = textures.begin(); it != textures.end();) {
            quadbuf_tex* tex = &(*it);

            int end_index;

            it++;

            if(it == textures.end()) {
                end_index = last_index;
            } else {
                end_index = (*it).start_index;
            }

            glBindTexture(GL_TEXTURE_2D, tex->textureid);
            glDrawArrays(GL_QUADS, tex->start_index, end_index - tex->start_index + 1);

            if(end_index==last_index) break;
        }
    }

    glDisableClientState(GL_VERTEX_ARRAY);
    glDisableClientState(GL_COLOR_ARRAY);
    glDisableClientState(GL_TEXTURE_COORD_ARRAY);

    buf.unbind();
}
