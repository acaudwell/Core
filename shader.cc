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

#include <boost/format.hpp>
#include <stdarg.h>

#ifndef USE_MGL_NAMESPACE
#include "gl.h"
#endif

#include "logger.h"

ShaderManager shadermanager;

//ShaderManager

ShaderManager::ShaderManager() {
}

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
        throw ShaderException("Cannot manage shader with no resource name");
    }

    if(resources[shader->resource_name] != 0) {
        throw ShaderException(str(boost::format("A shader resource already exists under the name '%s'") % shader->resource_name));
    }

    resources[shader->resource_name] = shader;

    shader->addref();
}

void ShaderManager::unload() {
    for(std::map<std::string, Resource*>::iterator it= resources.begin(); it!=resources.end();it++) {
        ((Shader*)it->second)->unload();
    }
}

void ShaderManager::reload(bool force) {
    for(std::map<std::string, Resource*>::iterator it= resources.begin(); it!=resources.end();it++) {
        ((Shader*)it->second)->reload(force);
    }
}

// ShaderPass

ShaderPass::ShaderPass(Shader* parent, int shader_object_type, const std::string& shader_object_desc) :
    AbstractShaderPass(parent, shader_object_type, shader_object_desc) {
}

ShaderPass::~ShaderPass() {
    unload();
}

void ShaderPass::unload() {
    if(shader_object!=0) glDeleteShader(shader_object);
    shader_object = 0;
}

void ShaderPass::attachTo(unsigned int program) {
    glAttachShader(program, shader_object);
}


void ShaderPass::checkError() {
    if(!shader_object) return;

    int compile_success;
    glGetShaderiv(shader_object, GL_COMPILE_STATUS, &compile_success);

    int info_log_length;
    glGetShaderiv(shader_object, GL_INFO_LOG_LENGTH, &info_log_length);

    const char* resource_desc = !parent->resource_name.empty() ? parent->resource_name.c_str() : "???";

    if(info_log_length > 1) {
        char info_log[info_log_length];

        glGetShaderInfoLog(shader_object, info_log_length, &info_log_length, info_log);

        std::string context;
        if(!errorContext(info_log, context))
            context = shader_object_source;

        if(!compile_success) {
            throw ShaderException(str(boost::format("%s shader '%s' failed to compile:\n%s\n%s")
                 % shader_object_desc % resource_desc % ((const char*)info_log) % context),
                 shader_object_source);
        }

        if(Logger::getDefault()->getLevel() == LOG_LEVEL_WARN) {
            warnLog("%s shader '%s':\n%s\n%s",
                            shader_object_desc.c_str(),
                            resource_desc,
                            info_log,
                            context.c_str());

        }

        return;
    }

    if(!compile_success) {
        throw ShaderException(str(boost::format("%s shader '%s' failed to compile") % shader_object_desc % resource_desc), shader_object_source);
    }
}

void ShaderPass::compile() {

    if(!shader_object) shader_object = glCreateShader(shader_object_type);

    if(source.empty()) return;

    shader_object_source.clear();

    toString(shader_object_source);

    // apply subsitutions
    parent->applySubstitutions(shader_object_source);

    for(ShaderUniform* u: uniforms) {
        u->setModified(false);
    }

    //fprintf(stderr, "src:\n%s", shader_object_source.c_str());

    const char* source_ptr = shader_object_source.c_str();
    int source_len = shader_object_source.size();

    glShaderSource(shader_object, 1, (const GLchar**) &source_ptr, &source_len);
    glCompileShader(shader_object);

    checkError();
}


// Shader

Shader::Shader(const std::string& prefix) : AbstractShader(prefix) {

    loadPrefix();
}

Shader::Shader() : AbstractShader() {
}

Shader::~Shader() {
    clear();
}

void Shader::unload() {
    if(program != 0) glDeleteProgram(program);
    program = 0;

    for(std::map<std::string, ShaderUniform*>::iterator it= uniforms.begin(); it!=uniforms.end();it++) {
        it->second->unload();
    }
}

void Shader::load() {
    //fprintf(stderr, "load\n");

    if(program !=0) unload();

    if(vertex_shader != 0)   vertex_shader->compile();
    if(geometry_shader != 0) geometry_shader->compile();
    if(fragment_shader != 0) fragment_shader->compile();

    program = glCreateProgram();

    if(vertex_shader!=0)   vertex_shader->attachTo(program);
    if(geometry_shader!=0) geometry_shader->attachTo(program);
    if(fragment_shader!=0) fragment_shader->attachTo(program);

    glLinkProgram(program);

    checkProgramError();

    if(vertex_shader  != 0)  vertex_shader->unload();
    if(geometry_shader != 0) geometry_shader->unload();
    if(fragment_shader != 0) fragment_shader->unload();
}

void Shader::loadPrefix() {

    if(vertex_shader != 0) delete vertex_shader;
    vertex_shader = 0;

    if(fragment_shader != 0) delete fragment_shader;
    fragment_shader = 0;

    std::string shader_dir = shadermanager.getDir();

    std::string vertex_file   = shader_dir + prefix + std::string(".vert");
    std::string fragment_file = shader_dir + prefix + std::string(".frag");

    vertex_shader = new ShaderPass(this, GL_VERTEX_SHADER, "vertex");
    vertex_shader->includeFile(vertex_file);

    fragment_shader = new ShaderPass(this, GL_FRAGMENT_SHADER, "fragment");
    fragment_shader->includeFile(fragment_file);

    load();
}

void Shader::checkProgramError() {

    int link_success;
    glGetProgramiv(program, GL_LINK_STATUS, &link_success);

    int info_log_length;
    glGetProgramiv(program, GL_INFO_LOG_LENGTH, &info_log_length);

    const char* resource_desc = !resource_name.empty() ? resource_name.c_str() : "???";

    if(info_log_length > 1) {
        char info_log[info_log_length];
        glGetProgramInfoLog(program, info_log_length, &info_log_length, info_log);

        if(!link_success) {
            errorLog("shader '%s' linking error:\n%s", resource_desc, info_log);
        } else if(Logger::getDefault()->getLevel() == LOG_LEVEL_WARN) {
            warnLog("shader '%s' warning:\n%s", resource_desc, info_log);
        }
    }

    if(!link_success) {
          throw ShaderException(str(boost::format("shader '%s' failed to link") % resource_desc));
    }
}

void Shader::bind() {
    glUseProgram(program);
}

void Shader::unbind() {
    glUseProgram(0);
}

int Shader::getUniformLocation(const std::string& uniform_name) {
    return glGetUniformLocation( program, uniform_name.c_str() );
}

void Shader::applyUniform(ShaderUniform* u) {

    int location = u->getLocation();

    if(location == -1) {
        if(Logger::getDefault()->getLevel() == LOG_LEVEL_PEDANTIC) {
            pedanticLog("shader '%s': invalid uniform '%s'", (!resource_name.empty() ? resource_name.c_str() : "???"), u->getName().c_str());
        }
        return;
    }

    switch(u->getType()) {
        case SHADER_UNIFORM_INT:
            glUniform1i(location, ((IntShaderUniform*)u)->getValue());
            break;
        case SHADER_UNIFORM_FLOAT:
            glUniform1f(location, ((FloatShaderUniform*)u)->getValue());
            break;
        case SHADER_UNIFORM_BOOL:
            glUniform1i(location, ((BoolShaderUniform*)u)->getValue());
            break;
        case SHADER_UNIFORM_SAMPLER_1D:
            glUniform1i(location, ((Sampler1DShaderUniform*)u)->getValue());
            break;
        case SHADER_UNIFORM_SAMPLER_2D:
            glUniform1i(location, ((Sampler2DShaderUniform*)u)->getValue());
            break;
        case SHADER_UNIFORM_VEC2:
            glUniform2fv(location, 1, glm::value_ptr(((Vec2ShaderUniform*)u)->getValue()));
            break;
        case SHADER_UNIFORM_VEC3:
            glUniform3fv(location, 1, glm::value_ptr(((Vec3ShaderUniform*)u)->getValue()));
            break;
        case SHADER_UNIFORM_VEC4:
            glUniform4fv(location, 1, glm::value_ptr(((Vec4ShaderUniform*)u)->getValue()));
            break;
        case SHADER_UNIFORM_MAT3:
            glUniformMatrix3fv(location, 1, 0, glm::value_ptr(((Mat3ShaderUniform*)u)->getValue()));
            break;
        case SHADER_UNIFORM_MAT4:
            glUniformMatrix4fv(location, 1, 0, glm::value_ptr(((Mat4ShaderUniform*)u)->getValue()));
            break;
        case SHADER_UNIFORM_VEC2_ARRAY:
            glUniform2fv(location, ((Vec2ArrayShaderUniform*)u)->getLength(), glm::value_ptr(((Vec2ArrayShaderUniform*)u)->getValue()[0]));
            break;
        case SHADER_UNIFORM_VEC3_ARRAY:
            glUniform3fv(location, ((Vec3ArrayShaderUniform*)u)->getLength(), glm::value_ptr(((Vec3ArrayShaderUniform*)u)->getValue()[0]));
            break;
        case SHADER_UNIFORM_VEC4_ARRAY:
            glUniform4fv(location, ((Vec4ArrayShaderUniform*)u)->getLength(), glm::value_ptr(((Vec4ArrayShaderUniform*)u)->getValue()[0]));
            break;
        default:
            throw ShaderException(str(boost::format("unsupported uniform type %d") % u->getType()));
            break;
    }
}

AbstractShaderPass* Shader::grabShaderPass(unsigned int shader_object_type) {

    AbstractShaderPass* shader_pass = 0;

    switch(shader_object_type) {
        case GL_VERTEX_SHADER:
            if(!vertex_shader) vertex_shader = new ShaderPass(this, GL_VERTEX_SHADER, "vertex");
            shader_pass = vertex_shader;
            break;
        case GL_GEOMETRY_SHADER_ARB:
            if(!geometry_shader) geometry_shader = new ShaderPass(this, GL_GEOMETRY_SHADER_ARB, "geometry");
            shader_pass = geometry_shader;
            break;
        case GL_FRAGMENT_SHADER:
            if(!fragment_shader) fragment_shader = new ShaderPass(this, GL_FRAGMENT_SHADER, "fragment");
            shader_pass = fragment_shader;
            break;
    }

    return shader_pass;
}
