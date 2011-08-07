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

#include "shader.h"

ShaderManager shadermanager;

//ShaderManager

ShaderManager::ShaderManager() {
    warnings = false;
}

void ShaderManager::enableWarnings(bool warnings) {
    this->warnings = warnings;
}

Regex Shader_pre_include("^\\s*#include\\s*\"([^\"]+)\"");

Shader* ShaderManager::grab(const std::string& shader_prefix) {
    Resource* s = resources[shader_prefix];

    if(s==0) {
        s = new Shader(shader_prefix);
        resources[shader_prefix] = s;
    }

    s->addref();

    return (Shader*) s;
}

void ShaderManager::manage(Shader* shader) {

    if(shader->resource_name.empty()) {
        throw SDLAppException("Cannot manage shader with no resource name");
    }

    if(resources[shader->resource_name] != 0) {
        throw SDLAppException("A shader resource already exists under the name '%s'", shader->resource_name.c_str());
    }

    resources[shader->resource_name] = shader;

    shader->addref();
}

void ShaderManager::unload() {
    for(std::map<std::string, Resource*>::iterator it= resources.begin(); it!=resources.end();it++) {
        ((Shader*)it->second)->load();
    }
}

void ShaderManager::reload() {
    for(std::map<std::string, Resource*>::iterator it= resources.begin(); it!=resources.end();it++) {
        ((Shader*)it->second)->load();
    }
}

//Shader
Shader::Shader(const std::string& prefix) : Resource(prefix) {

    std::string shader_dir = shadermanager.getDir();

    std::string vertexFile   = shader_dir + prefix + std::string(".vert");
    std::string fragmentFile = shader_dir + prefix + std::string(".frag");

    includeFile(GL_VERTEX_SHADER,   vertexFile);
    includeFile(GL_FRAGMENT_SHADER, fragmentFile);

    setDefaults();

    load();
}

Shader::Shader() {
    setDefaults();
}

void Shader::setDefaults() {
    program = 0;

    geom_input_type    = GL_POINTS;
    geom_output_type   = GL_POINTS;
    geom_max_vertices  = 1;
}

Shader::~Shader() {
    unload();
}

void Shader::unload() {
    if(program != 0) glDeleteProgram(program);
    program = 0;
}

void Shader::geometrySettings(GLenum input_type, GLenum  output_type, GLuint max_vertices) {
    this->geom_input_type   = input_type;
    this->geom_output_type  = output_type;
    this->geom_max_vertices = max_vertices;
}

void Shader::load() {
    if(program !=0) unload();

    GLenum vertexShader   = compile(GL_VERTEX_SHADER);
    GLenum geometryShader = compile(GL_GEOMETRY_SHADER_EXT);
    GLenum fragmentShader = compile(GL_FRAGMENT_SHADER);

    program = glCreateProgram();

    if(vertexShader!=0)   glAttachShader(program, vertexShader);
    if(geometryShader!=0) glAttachShader(program, geometryShader);
    if(fragmentShader!=0) glAttachShader(program, fragmentShader);

    if(geometryShader != 0) {
        glProgramParameteriEXT(program, GL_GEOMETRY_INPUT_TYPE_EXT,   geom_input_type);
        glProgramParameteriEXT(program, GL_GEOMETRY_OUTPUT_TYPE_EXT,  geom_output_type);
        glProgramParameteriEXT(program, GL_GEOMETRY_VERTICES_OUT_EXT, geom_max_vertices);
    }

    glLinkProgram(program);

    checkProgramError();

    if(vertexShader!=0)   glDeleteShader(fragmentShader);
    if(geometryShader!=0) glDeleteShader(geometryShader);
    if(fragmentShader!=0) glDeleteShader(vertexShader);
}

void Shader::checkProgramError() {

    GLint link_success;
    glGetProgramiv(program, GL_LINK_STATUS, &link_success);

    GLint info_log_length;
    glGetProgramiv(program, GL_INFO_LOG_LENGTH, &info_log_length);

    const char* resource_desc = !resource_name.empty() ? resource_name.c_str() : "???";

    if(info_log_length > 1) {
        char info_log[info_log_length];

        glGetProgramInfoLog(program, info_log_length, &info_log_length, info_log);

        if(!link_success) {
            throw SDLAppException("shader '%s' failed to link:\n%s",
                                  resource_desc,
                                  info_log);
        }

        if(shadermanager.warnings) {
            fprintf(stderr, "shader '%s':\n%s",
                            resource_desc,
                            info_log);
        }

        return;
    }

    if(!link_success) {
        throw SDLAppException("shader '%s' failed to link",
                              resource_desc);
    }
}

void Shader::checkShaderError(GLenum shaderType, GLenum shaderRef) {

    GLint compile_success;
    glGetShaderiv(shaderRef, GL_COMPILE_STATUS, &compile_success);

    GLint info_log_length;
    glGetShaderiv(shaderRef, GL_INFO_LOG_LENGTH, &info_log_length);

    const char* shader_desc;

    switch(shaderType) {
        case GL_VERTEX_SHADER:
            shader_desc = "vertex";
            break;
        case GL_FRAGMENT_SHADER:
            shader_desc = "fragment";
            break;
        case GL_GEOMETRY_SHADER_EXT:
            shader_desc = "geometry";
            break;
    }

    const char* resource_desc = !resource_name.empty() ? resource_name.c_str() : "???";

    if(info_log_length > 1) {
        char info_log[info_log_length];

        glGetShaderInfoLog(shaderRef, info_log_length, &info_log_length, info_log);

        if(!compile_success) {
            throw SDLAppException("%s shader '%s' failed to compile:\n%s",
                                  shader_desc,
                                  resource_desc,
                                  info_log);
        }

        if(shadermanager.warnings) {
            fprintf(stderr, "%s shader '%s':\n%s",
                            shader_desc,
                            resource_desc,
                            info_log);
        }

        return;
    }

    if(!compile_success) {
        throw SDLAppException("%s shader '%s' failed to compile",
                              shader_desc,
                              resource_desc);
    }
}

GLenum Shader::compile(GLenum shaderType) {

    if(srcMap[shaderType].empty()) return 0;

    GLenum shaderRef = glCreateShader(shaderType);

    std::string& src = srcMap[shaderType];

    const char* source_ptr = src.c_str();
    int source_len = src.size();

    glShaderSource(shaderRef, 1, (const GLchar**) &source_ptr, &source_len);

    glCompileShader(shaderRef);

    checkShaderError(shaderType, shaderRef);

    return shaderRef;
}

bool Shader::preprocess(GLenum shaderType, const std::string& line) {

    std::vector<std::string> matches;

    if(Shader_pre_include.match(line, &matches)) {

        std::string include_file = shadermanager.getDir() + matches[0];

        includeFile(shaderType, include_file);

        return true;
    }

    return false;

}

void Shader::includeSource(GLenum shaderType, const std::string& string) {

    std::stringstream in(string);

    std::string& output = srcMap[shaderType];

    std::string line;
    while( std::getline(in,line) ) {
        if(!preprocess(shaderType, line)) {
            output += line;
            output += "\n";
        }
    }
}

bool Shader::includeFile(GLenum shaderType, const std::string& filename) {

    // get length
    std::ifstream in(filename.c_str());

    if(!in.is_open()) {
        throw SDLAppException("could not open '%s'", filename.c_str());
    }

    std::string& output = srcMap[shaderType];

    std::string line;
    while( std::getline(in,line) ) {
        if(!preprocess(shaderType, line)) {
            output += line;
            output += "\n";
        }
    }

    in.close();

    return true;
}

void Shader::use() {
    glUseProgram(program);
}

GLenum Shader::getProgram() {
    return program;
}

GLint Shader::getVarLocation(const std::string& name) {

    GLint loc = varMap[name] - 1;

    if(loc != -1) return loc;

    loc = glGetUniformLocation( program, name.c_str() );

    varMap[name] = loc + 1;

    return loc;
}

void Shader::setFloat(const std::string& name, float value) {
    GLint loc = getVarLocation(name);
    glUniform1f(loc, value);
}

void Shader::setVec2 (const std::string& name, const vec2f& value) {
    GLint loc = getVarLocation(name);
    glUniform2fv(loc, 1, value);
}

void Shader::setVec3 (const std::string& name, const vec3f& value) {
    GLint loc = getVarLocation(name);
    glUniform3fv(loc, 1, value);
}

void Shader::setVec4 (const std::string& name, const vec4f& value) {
    GLint loc = getVarLocation(name);
    glUniform4fv(loc, 1, value);
}

void Shader::setMat3 (const std::string& name, const mat3f& value) {
    GLint loc = getVarLocation(name);
    glUniformMatrix3fv(loc, 1, 0, value);
}

void Shader::setMat4 (const std::string& name, const mat4f& value) {
    GLint loc = getVarLocation(name);
    glUniformMatrix4fv(loc, 1, 0, value);
}

void Shader::setInteger (const std::string& name, int value) {
    GLint loc =  getVarLocation(name);
    glUniform1i(loc, value);
}



