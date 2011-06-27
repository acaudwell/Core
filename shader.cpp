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

    shaderProg = 0;
    
    load();
}

Shader::Shader() {
}

Shader::~Shader() {
    unload();
}

void Shader::unload() {
    if(shaderProg != 0) glDeleteProgram(shaderProg);
    shaderProg = 0;
}

void Shader::load() {
    if(shaderProg !=0) unload();    
    
    GLenum vertexShader   = compile(GL_VERTEX_SHADER);
    GLenum fragmentShader = compile(GL_FRAGMENT_SHADER);

    shaderProg = glCreateProgram();

    glAttachShader(shaderProg, fragmentShader);
    glAttachShader(shaderProg, vertexShader);

    glLinkProgram(shaderProg);

    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);
}

void Shader::checkError(GLenum shaderType, GLenum shaderRef) {

    GLint compile_success;
    glGetShaderiv(shaderRef, GL_COMPILE_STATUS, &compile_success);

    GLint info_log_length;
    glGetShaderiv(shaderRef, GL_INFO_LOG_LENGTH, &info_log_length);

    if(info_log_length > 1) {
        char info_log[info_log_length];

        glGetShaderInfoLog(shaderRef, info_log_length, &info_log_length, info_log);

        if(!compile_success) {
            throw SDLAppException("%s shader '%s' failed to compile:\n%s",
                                  (shaderType == GL_VERTEX_SHADER ? "vertex" : "fragment"),
                                  (!resource_name.empty() ? resource_name.c_str() : "???"),
                                  info_log);
        }

        if(shadermanager.debug) {
            fprintf(stderr, "%s shader '%s':\n%s",
                            (shaderType == GL_VERTEX_SHADER ? "vertex" : "fragment"),
                            (!resource_name.empty() ? resource_name.c_str() : "???"),
                            info_log);
        }

        return;
    }

    if(!compile_success) {
        throw SDLAppException("%s shader '%s' failed to compile",
                              (shaderType == GL_VERTEX_SHADER ? "vertex" : "fragment"),
                              (!resource_name.empty() ? resource_name.c_str() : "???"));
    }
}

GLenum Shader::compile(GLenum shaderType) {

    GLenum shaderRef = glCreateShader(shaderType);

    std::string& src = srcMap[shaderType];

    const char* source_ptr = src.c_str();
    int source_len = src.size();

    glShaderSource(shaderRef, 1, (const GLchar**) &source_ptr, &source_len);

    glCompileShader(shaderRef);

    checkError(shaderType, shaderRef);

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
    glUseProgram(shaderProg);
}

GLenum Shader::getProgram() {
    return shaderProg;
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
    GLint loc = getVarLocation(name);
    glUniform4fv(loc, 1, value);
}

void Shader::setMat3 (const std::string& name, const mat3f& value) {
    GLint loc = getVarLocation(name);
    glUniformMatrix3fv(loc, 1, 0, value);
}

void Shader::setInteger (const std::string& name, int value) {
    GLint loc =  getVarLocation(name);
    glUniform1i(loc, value);
}



