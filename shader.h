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

#include "gl.h"
#include "vectors.h"
#include "resource.h"
#include "display.h"
#include "sdlapp.h"
#include "regex.h"

#include <map>
#include <string>
#include <fstream>
#include <sstream>

class Shader : public Resource {

    std::map<std::string, GLint> varMap;
    std::map<GLenum, std::string> srcMap;

    GLenum geom_input_type;
    GLenum geom_output_type;
    GLuint geom_max_vertices;

    GLenum program;

    GLint getVarLocation(const std::string& name);

    bool preprocess(GLenum shaderType, const std::string& line);

    GLenum compile(GLenum shaderType);

    void checkShaderError(GLenum shaderType, GLenum shaderRef);
    void checkProgramError();

    void setDefaults();
public:
    Shader();
    Shader(const std::string& prefix);
    ~Shader();

    GLenum getProgram();
    GLenum getVertexShader();
    GLenum getFragmentShader();

    void load();
    void unload();

    void includeSource(GLenum shaderType, const std::string& source);
    bool includeFile(GLenum shaderType,   const std::string& filename);

    void geometrySettings(GLenum input_type, GLenum  output_type, GLuint max_vertices);

    void setInteger (const std::string& name, int value);
    void setFloat(const std::string& name, float value);
    void setVec2 (const std::string& name, const vec2& value);
    void setVec3 (const std::string& name, const vec3& value);
    void setVec4 (const std::string& name, const vec4& value);
    void setMat3 (const std::string& name, const mat3& value);
    void setMat4 (const std::string& name, const mat4& value);

    void use();
};

class ShaderManager : public ResourceManager {
public:
    bool warnings;

    ShaderManager();
    Shader* grab(const std::string& shader_prefix);

    void enableWarnings(bool warnings);

    void manage(Shader* shader);

    void unload();
    void reload();
};

extern ShaderManager shadermanager;

#endif
