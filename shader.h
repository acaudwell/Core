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

#ifndef GLSL_SHADER_H
#define GLSL_SHADER_H

#define __Shader                 Shader
#define __ShaderPass             ShaderPass
#define __ShaderPart             ShaderPart
#define __ShaderManager          ShaderManager
#define __VertexShader           VertexShader
#define __FragmentShader         FragmentShader
#define __GeometryShader         GeometryShader
#define __ShaderException        ShaderException
#define __ShaderUniform          ShaderUniform
#define __FloatShaderUniform     FloatShaderUniform
#define __BoolShaderUniform      BoolShaderUniform
#define __Sampler1DShaderUniform Sampler1DShaderUniform
#define __Sampler2DShaderUniform Sampler2DShaderUniform
#define __IntShaderUniform       IntShaderUniform
#define __Vec2ShaderUniform      Vec2ShaderUniform
#define __Vec3ShaderUniform      Vec3ShaderUniform
#define __Vec4ShaderUniform      Vec4ShaderUniform
#define __Mat3ShaderUniform      Mat3ShaderUniform
#define __Mat4ShaderUniform      Mat4ShaderUniform
#define __Vec2ArrayShaderUniform Vec2ArrayShaderUniform
#define __Vec3ArrayShaderUniform Vec3ArrayShaderUniform
#define __Vec4ArrayShaderUniform Vec4ArrayShaderUniform

#include "shader.hh"

extern ShaderManager shadermanager;

#endif
