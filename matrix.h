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
    T m[3][3];

    mat3() {
    }

    mat3(const vec3<T>& a, const vec3<T>& b, const vec3<T>& c) {
        m[0][0] = a.x; m[0][1] = b.x; m[0][2] = c.x;
        m[1][0] = a.y; m[1][1] = b.y; m[1][2] = c.y;
        m[2][0] = a.z; m[2][1] = b.z; m[2][2] = c.z;
    }

    mat3(T m11, T m12, T m13,
         T m21, T m22, T m23,
         T m31, T m32, T m33) {
        m[0][0] = m11; m[0][1] = m21; m[0][2] = m31;
        m[1][0] = m12; m[1][1] = m22; m[1][2] = m32;
        m[2][0] = m13; m[2][1] = m23; m[2][2] = m33;
    }

    vec3<T> row(int i) {
        return vec3<T>(m[0][i], m[1][i], m[2][i]);
    }

    vec3<T> col(int i) {
        return vec3<T>(m[i][0], m[i][1], m[i][2]);
    }


    mat3<T> operator* (const mat3<T>& a) {

        mat3<T> mx;

        for(int i=0; i<3; i++)
            for(int j=0; j<3; j++)
                    mx.m[j][i] =  m[0][i] * a.m[j][0]
                                + m[1][i] * a.m[j][1]
                                + m[2][i] * a.m[j][2];

        return mx;
    }

    friend vec3<T> operator* (const vec3<T>& vec, const mat3<T>& m) {

        return vec3<T>(vec.x * m.m[0][0] + vec.y * m.m[0][1] + vec.z * m.m[0][2],
                       vec.x * m.m[1][0] + vec.y * m.m[1][1] + vec.z * m.m[1][2],
                       vec.x * m.m[2][0] + vec.y * m.m[2][1] + vec.z * m.m[2][2]);
    }

    mat3<T> transpose() {
        return mat3<T>( m[0][0], m[0][1], m[0][2],
                        m[1][0], m[1][1], m[1][2],
                        m[2][0], m[2][1], m[2][2]);
    }

    mat3<T> operator -() {
        return mat3<T>( -m[0][0], -m[1][0], -m[2][0],
                        -m[0][1], -m[1][1], -m[2][1],
                        -m[0][2], -m[1][2], -m[2][2]);
    }

    operator T*() const {
            return (T*) &m;
    }
};

template<class T> class mat4 {
public:
    T m[4][4];

    mat4() {
    }

    mat4(const vec3<T>& a, const vec3<T>& b, const vec3<T>& c) {
        m[0][0] = a.x; m[0][1] = b.x; m[0][2] = c.x; m[0][3] = 0.0;
        m[1][0] = a.y; m[1][1] = b.y; m[1][2] = c.y; m[1][3] = 0.0;
        m[2][0] = a.z; m[2][1] = b.z; m[2][2] = c.z; m[2][3] = 0.0;
        m[3][0] = 0.0; m[3][1] = 0.0; m[3][2] = 0.0; m[3][3] = 1.0;
    }

    mat4(T m11, T m12, T m13, T m14,
         T m21, T m22, T m23, T m24,
         T m31, T m32, T m33, T m34,
         T m41, T m42, T m43, T m44) {
        /*
        m[0][0] = m11; m[0][1] = m21; m[0][2] = m31; m[0][3] = m41;
        m[1][0] = m12; m[1][1] = m22; m[1][2] = m32; m[1][3] = m42;
        m[2][0] = m13; m[2][1] = m23; m[2][2] = m33; m[2][3] = m43;
        m[3][0] = m14; m[3][1] = m24; m[3][2] = m34; m[3][3] = m44;
        */
        m[0][0] = m11; m[0][1] = m12; m[0][2] = m13; m[0][3] = m14;
        m[1][0] = m21; m[1][1] = m22; m[1][2] = m23; m[1][3] = m24;
        m[2][0] = m31; m[2][1] = m32; m[2][2] = m33; m[2][3] = m34;
        m[3][0] = m41; m[3][1] = m42; m[3][2] = m43; m[3][3] = m44;

    }

    static mat4<T> scale(T n) {
        return mat4<T>(n,   0.0, 0.0, 0.0,
                       0.0, n,   0.0, 0.0,
                       0.0, 0.0, n,   0.0,
                       0.0, 0.0, 0.0, 1.0);
    }

    static mat4<T> translate(const vec3<T>& vec) {
        return mat4<T>(1.0, 0.0, 0.0, 0.0,
                       0.0, 1.0, 0.0, 0.0,
                       0.0, 0.0, 1.0, 0.0,
                       vec.x, vec.y, vec.z, 1.0);
    }

    static mat4<T> rotate(const vec3<T>& vec, float radians) {

        float s = sinf(radians);
        float c = cosf(radians);

        return mat4<T>(c + (1.0 - c) * vec.x * vec.x, (1.0 - c) * vec.x * vec.y - s * vec.z, (1.0 - c) * vec.x * vec.z + s * vec.y, 0.0,
                       (1.0 - c) * vec.x * vec.y + s * vec.z, c + (1.0 - c) * vec.y * vec.y, (1.0 - c) * vec.y * vec.z - s * vec.x, 0.0,
                       (1.0 - c) * vec.x * vec.z - s * vec.y, (1.0 - c) * vec.y * vec.z + s * vec.x, c + (1.0 - c) * vec.z * vec.z, 0.0,
                       0.0, 0.0, 0.0, 1.0);
    }

    vec4<T> row(int i) {
        return vec4<T>(m[0][i], m[1][i], m[2][i], m[3][i]);
    }

    vec4<T> col(int i) {
        return vec4<T>(m[i][0], m[i][1], m[i][2], m[i][3]);
    }

    mat4<T> operator* (const mat4<T>& a) {

        mat4<T> mx;

        for(int i=0; i<4; i++)
            for(int j=0; j<4; j++)
                    mx.m[j][i] =  m[0][i] * a.m[j][0]
                                + m[1][i] * a.m[j][1]
                                + m[2][i] * a.m[j][2]
                                + m[3][i] * a.m[j][3];

        return mx;
    }

    mat4<T> operator -() {
        return mat4<T>( -m[0][0], -m[1][0], -m[2][0], -m[3][0],
                        -m[0][1], -m[1][1], -m[2][1], -m[3][1],
                        -m[0][2], -m[1][2], -m[2][2], -m[3][2],
                        -m[0][3], -m[1][3], -m[2][3], -m[3][3]);
    }

    operator T*() const {
            return (T*) &m;
    }
};

typedef mat3<float> mat3f;
typedef mat4<float> mat4f;

#endif
