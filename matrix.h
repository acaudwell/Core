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

#ifndef MATRIX_H
#define MATRIX_H

#include "vectors.h"

#include <cmath>

template<class T> class mat3 {
public:
    T matrix[3][3];

    mat3() {
    }

    mat3(const vec3<T>& a, const vec3<T>& b, const vec3<T>& c) {
        matrix[0][0] = a.x; matrix[0][1] = b.x; matrix[0][2] = c.x;
        matrix[1][0] = a.y; matrix[1][1] = b.y; matrix[1][2] = c.y;
        matrix[2][0] = a.z; matrix[2][1] = b.z; matrix[2][2] = c.z;
    }

    vec3<T> X() {
        return vec3<T>(matrix[0][0], matrix[1][0], matrix[2][0]);
    }

    vec3<T> Y() {
        return vec3<T>(matrix[0][1], matrix[1][1], matrix[2][1]);
    }

    vec3<T> Z() {
        return vec3<T>(matrix[0][2], matrix[1][2], matrix[2][2]);
    }

    friend vec3<T> operator* (const vec3<T>& vec, const mat3<T>& m) {

        return vec3<T>(vec.x * m.matrix[0][0] + vec.y * m.matrix[0][1] + vec.z * m.matrix[0][2],
                       vec.x * m.matrix[1][0] + vec.y * m.matrix[1][1] + vec.z * m.matrix[1][2],
                       vec.x * m.matrix[2][0] + vec.y * m.matrix[2][1] + vec.z * m.matrix[2][2]);
    }

    mat3<T> transpose() {
        return mat3<T>( vec3<T>(matrix[0][0], matrix[0][1], matrix[0][2]),
                        vec3<T>(matrix[1][0], matrix[1][1], matrix[1][2]),
                        vec3<T>(matrix[2][0], matrix[2][1], matrix[2][2]));        
    }

    mat3<T> operator -() {
        return mat3<T>( vec3<T>(-matrix[0][0], -matrix[1][0], -matrix[2][0]),
                        vec3<T>(-matrix[0][1], -matrix[1][1], -matrix[2][1]),
                        vec3<T>(-matrix[0][2], -matrix[1][2], -matrix[2][2]));             
    }

    operator T*() const {
            return (T*) &matrix;
    }
};

typedef mat3<float> mat3f;

#endif
