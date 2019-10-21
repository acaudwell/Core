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

#include "stringhash.h"

int gStringHashSeed = 31;

int stringHash(const std::string& str) {

    int val = 0;
    int n = str.size();

    for (int i = 0; i < n; i++) {
        val = val + str[i] * (gStringHashSeed^(n-i));
    }

     if(val<0) {
         val = -val;
     }

    return val;
}

vec2 vec2Hash(const std::string& str) {
    int hash = stringHash(str);

    int x = ((hash/7) % 255) - 127;
    int y = ((hash/3) % 255) - 127;

    vec2 v = normalise(vec2(x, y));

    return v;
}

vec3 vec3Hash(const std::string& str) {
    int hash = stringHash(str);

    int x = ((hash/7) % 255) - 127;
    int y = ((hash/3) % 255) - 127;
    int z = hash % 255;

    vec3 v = normalise(vec3(x, y, z));

    return v;
}

vec3 colourHash(const std::string& str) {
    int hash = stringHash(str);

    if(hash == 0) hash++;

    int r = (hash/7) % 255;
    int g = (hash/3) % 255;
    int b = hash % 255;

    vec3 colour = normalise(vec3(r, g, b));

    return colour;
}
