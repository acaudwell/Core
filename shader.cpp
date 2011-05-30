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

Regex Shader_pre_include("\\s*#include\\s*\"([^\"]+)\"");

Shader* ShaderManager::grab(const std::string& shader_prefix) {
    Resource* s = resources[shader_prefix];

    if(s==0) {
        s = new Shader(shader_prefix);
        resources[shader_prefix] = s;
    }

    s->addref();

    return (Shader*) s;
}

//Shader
Shader::Shader(const std::string& prefix) : Resource(prefix) {

    std::string shader_dir = shadermanager.getDir();

    std::string vertexSrc   = shader_dir + prefix + std::string(".vert");
    std::string fragmentSrc = shader_dir + prefix + std::string(".frag");

    vertexShader   = load(vertexSrc,   GL_VERTEX_SHADER);
    fragmentShader = load(fragmentSrc, GL_FRAGMENT_SHADER);

    makeProgram();
}

Shader::~Shader() {
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);
    glDeleteProgram(shaderProg);
}

void Shader::makeProgram() {
    shaderProg = glCreateProgram();
    glAttachShader(shaderProg,fragmentShader);
    glAttachShader(shaderProg,vertexShader);
    glLinkProgram(shaderProg);
}

void Shader::checkError(const std::string& filename, GLenum shaderRef) {

    GLint compile_success;   
    glGetShaderiv(shaderRef, GL_COMPILE_STATUS, &compile_success);

    GLint info_log_length;
    glGetShaderiv(shaderRef, GL_INFO_LOG_LENGTH, &info_log_length);

    if(info_log_length > 1) {
        char info_log[info_log_length];

        glGetShaderInfoLog(shaderRef, info_log_length, &info_log_length, info_log);

        if(!compile_success) {
            throw SDLAppException("shader '%s' failed to compile: %s", filename.c_str(), info_log);
        }
        
        if(shadermanager.debug) {
            fprintf(stderr, "%s: %s\n", filename.c_str(), info_log);
        }

        return;
    }
    
    if(!compile_success) {
        throw SDLAppException("shader '%s' failed to compile", filename.c_str());
    }
}

GLenum Shader::load(const std::string& filename, GLenum shaderType) {

    std::string source;
    readSource(filename, source);

    if(source.empty()) {
        throw SDLAppException("could not read shader '%s'", filename.c_str());
    }

    GLenum shaderRef = glCreateShader(shaderType);

    const char* source_ptr = source.c_str();
    int source_len = source.size();

    glShaderSource(shaderRef, 1, (const GLchar**) &source_ptr, &source_len);

    glCompileShader(shaderRef);
    
    checkError(filename, shaderRef);

    return shaderRef;
}


bool Shader::preprocess(const std::string& line, std::string& output) {

    std::vector<std::string> matches;

    if(Shader_pre_include.match(line, &matches)) {

        std::string include_file = shadermanager.getDir() + matches[0];

        readSource(include_file, output);

        return true;
    }

    return false;

}

bool Shader::readSource(const std::string& file, std::string& output) {

    // get length
    std::ifstream in(file.c_str());

    if(!in.is_open()) {
        throw SDLAppException("could not open '%s'", file.c_str());
    }

    std::string line;
    while( std::getline(in,line) ) {
        if(!preprocess(line, output)) {            
            output += line;
            output += "\n";
        }
    }

    in.close();
    
    return true;
}

void Shader::use() {
    glUseProgram(shaderProg);
}

GLenum Shader::getProgram() {
    return shaderProg;
}

GLenum Shader::getVertexShader() {
    return vertexShader;
}

GLenum Shader::getFragmentShader() {
    return fragmentShader;
}

GLint Shader::getVarLocation(const std::string& name) {

    GLint loc = varMap[name] - 1;

    if(loc != -1) return loc;

    loc = glGetUniformLocation( shaderProg, name.c_str() );

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
    GLint loc =  getVarLocation(name);
    glUniform4fv(loc, 1, value);
}

void Shader::setInteger (const std::string& name, int value) {
    GLint loc =  getVarLocation(name);
    glUniform1i(loc, value);
}



