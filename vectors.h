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

#ifndef VECTORS_H
#define VECTORS_H

#define GLM_FORCE_RADIANS

// the following are required for compatibility with GLM 0.9.9
#define GLM_FORCE_CTOR_INIT
#define GLM_ENABLE_EXPERIMENTAL

#include <glm/glm.hpp>

#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/matrix_access.hpp>
#include <glm/gtx/norm.hpp>

using glm::vec2;
using glm::vec3;
using glm::vec4;
using glm::mat3;
using glm::mat4;

vec2 rotate_vec2(const vec2& v, float s, float c);

vec2 normalise(const vec2& v);
vec3 normalise(const vec3& v);
vec4 normalise(const vec4& v);

class lerp2 : public glm::vec2 {
public:
    vec2 p;
    vec2 l;

    lerp2() : vec2(), p(), l() {
    }

    const lerp2& snap() {
        p = *this;
        return *this;
    }

    static vec2 lerp(const vec2& a, const vec2& b, float n) {
        return a + (b - a) * n;
    }

    const vec2& lerp(float n) {
        l = p + (*this - p) * n;
        return l;
    }
    
    const lerp2& operator= (const vec2& vec) {
        this->x = vec.x;
        this->y = vec.y;
        return *this;
    }
};

class lerp3 : public vec3 {
public:
    vec3 p;
    vec3 l;

    lerp3() : vec3(), p(), l() {
    }

    const lerp3& snap() {
        p = *this;
        return *this;
    }

    static vec3 lerp(const vec3& a, const vec3& b, float n) {
        return a + (b - a) * n;
    }

    const vec3& lerp(float n) {
        l = p + (*this - p) * n;
        return l;
    }

    const lerp3& operator= (const vec3& vec) {
        this->x = vec.x;
        this->y = vec.y;
        this->z = vec.z;
        return *this;
    }
};

#endif
