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

#ifdef USE_MGL_NAMESPACE
#define GL_GLEXT_PROTOTYPES
#include "../MGL/osmesa.h"
#else
#include "gl.h"
#endif

#include "logger.h"

ShaderManager shadermanager;

//ShaderException

ShaderException::ShaderException(const char* str, ...) {

    va_list vl;
    char msg[65536];

    va_start(vl, str);
        vsnprintf(msg, 65536, str, vl);
    va_end(vl);

    message = std::string(msg);
}

ShaderException::ShaderException(const std::string& message) : message(message) {}   

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
        throw ShaderException("A shader resource already exists under the name '%s'", shader->resource_name.c_str());
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

//ShaderUniform

ShaderUniform::ShaderUniform(Shader* shader, const std::string& name, int uniform_type, const std::string& type_name)
    : shader(shader), name(name), location(-1), initialized(false), modified(false), baked(false), uniform_type(uniform_type), type_name(type_name) {
}

void ShaderUniform::unload() {
    location = -1;
}

const std::string& ShaderUniform::getName() const {
    return name;
}

int ShaderUniform::getLocation() {

    // TODO: (re-)compiling the shader should break the uniform location caching.

    if(location != -1) return location;

    location = glGetUniformLocation( shader->getProgram(), name.c_str() );

    return location;
}

void ShaderUniform::setBaked(bool baked) {
    if(this->baked == baked) return;
    this->baked = baked;
    modified = true;
}

void ShaderUniform::setComment(const std::string& comment) {
    this->comment = comment;
}
    
const std::string& ShaderUniform::getComment() const {
    return comment;
}
    

//FloatShaderUniform

FloatShaderUniform::FloatShaderUniform(Shader* shader, const std::string& name, float value) :
    value(value), ShaderUniform(shader, name, SHADER_UNIFORM_FLOAT, "float") {
}

void FloatShaderUniform::setValue(float value) {
    if(baked && this->value == value) return;

    this->value = value;
    modified = true;
    initialized = true;
}

void FloatShaderUniform::apply() {
    glUniform1f(getLocation(), value);
}

float& FloatShaderUniform::getValue() {
    return value;
}

void FloatShaderUniform::write(std::string& content) const {

    char buff[256];

    if(baked) {
//        snprintf(buff, 256, "const %s %s = %e;\n", type_name.c_str(), name.c_str(), value);
        snprintf(buff, 256, "#define %s %e\n", name.c_str(), value);
    } else {
        snprintf(buff, 256, "uniform %s %s;\n", type_name.c_str(), name.c_str());
    }

    content += buff;
}

//IntShaderUniform

IntShaderUniform::IntShaderUniform(Shader* shader, const std::string& name, int value) :
    value(value), ShaderUniform(shader, name, SHADER_UNIFORM_INT, "int") {
}

void IntShaderUniform::setValue(int value) {
    if(baked && this->value == value) return;

    this->value = value;
    modified = true;
    initialized = true;
}

void IntShaderUniform::apply() {
    glUniform1i(getLocation(), value);
}

int& IntShaderUniform::getValue() {
    return value;
}

void IntShaderUniform::write(std::string& content) const {

    char buff[256];

    if(baked) {
        snprintf(buff, 256, "#define %s %d\n", name.c_str(), value);
    } else {
        snprintf(buff, 256, "uniform %s %s;\n", type_name.c_str(), name.c_str());
    }

    content += buff;
}

//BoolShaderUniform

BoolShaderUniform::BoolShaderUniform(Shader* shader, const std::string& name, bool value) :
    value(value), ShaderUniform(shader, name, SHADER_UNIFORM_BOOL, "bool") {
}

void BoolShaderUniform::setValue(bool value) {
    if(baked && this->value == value) return;

    this->value = value;
    modified = true;
    initialized = true;
}

void BoolShaderUniform::apply() {
    glUniform1i(getLocation(), value);
}

bool& BoolShaderUniform::getValue() {
    return value;
}

void BoolShaderUniform::write(std::string& content) const {

    char buff[256];

    if(baked) {
        snprintf(buff, 256, "#define %s %s\n", name.c_str(), value ? "true" : "false");
    } else {
        snprintf(buff, 256, "uniform %s %s;\n", type_name.c_str(), name.c_str());
    }

    content += buff;
}

//Sampler1DShaderUniform

Sampler1DShaderUniform::Sampler1DShaderUniform(Shader* shader, const std::string& name, int value) :
    value(value), ShaderUniform(shader, name, SHADER_UNIFORM_SAMPLER_1D, "sampler1D") {
}

void Sampler1DShaderUniform::setValue(int value) {
    if(baked && this->value == value) return;

    this->value = value;
    modified = true;
    initialized = true;
}

void Sampler1DShaderUniform::setBaked(bool baked) {
}

void Sampler1DShaderUniform::write(std::string& content) const {
    char buff[256];
    snprintf(buff, 256, "uniform %s %s;\n", type_name.c_str(), name.c_str());
    content += buff;
}

void Sampler1DShaderUniform::apply() {
    glUniform1i(getLocation(), value);
}

int& Sampler1DShaderUniform::getValue() {
    return value;
}

//Sampler2DShaderUniform

Sampler2DShaderUniform::Sampler2DShaderUniform(Shader* shader, const std::string& name, int value) :
    value(value), ShaderUniform(shader, name, SHADER_UNIFORM_SAMPLER_2D, "sampler2D") {
}

void Sampler2DShaderUniform::setValue(int value) {
    if(baked && this->value == value) return;

    this->value = value;
    modified = true;
    initialized = true;
}

void Sampler2DShaderUniform::apply() {
    glUniform1i(getLocation(), value);
}

int& Sampler2DShaderUniform::getValue() {
    return value;
}

//cant be baked
void Sampler2DShaderUniform::setBaked(bool baked) {
}

void Sampler2DShaderUniform::write(std::string& content) const {
    char buff[256];
    snprintf(buff, 256, "uniform %s %s;\n", type_name.c_str(), name.c_str());
    content += buff;
}


//Vec2ShaderUniform

Vec2ShaderUniform::Vec2ShaderUniform(Shader* shader, const std::string& name, const vec2& value) :
    value(value), ShaderUniform(shader, name, SHADER_UNIFORM_VEC2, "vec2") {
}

void Vec2ShaderUniform::setValue(const vec2& value) {
    if(baked && this->value == value) return;

    this->value = value;
    modified = true;
    initialized = true;
}

void Vec2ShaderUniform::apply() {
    glUniform2fv(getLocation(), 1, glm::value_ptr(value));
}

vec2& Vec2ShaderUniform::getValue() {
    return value;
}

void Vec2ShaderUniform::write(std::string& content) const {

    char buff[256];

    if(baked) {
        snprintf(buff, 256, "#define %s vec2(%e, %e)\n", name.c_str(), value.x, value.y);
    } else {
        snprintf(buff, 256, "uniform %s %s;\n", type_name.c_str(), name.c_str());
    }

    content += buff;
}

//Vec3ShaderUniform

Vec3ShaderUniform::Vec3ShaderUniform(Shader* shader, const std::string& name, const vec3& value) :
    value(value), ShaderUniform(shader, name, SHADER_UNIFORM_VEC3, "vec3") {
}


void Vec3ShaderUniform::setValue(const vec3& value) {
    if(baked && this->value == value) return;

    this->value = value;
    modified = true;
    initialized = true;
}

void Vec3ShaderUniform::apply() {
    glUniform3fv(getLocation(), 1, glm::value_ptr(value));
}


vec3& Vec3ShaderUniform::getValue() {
    return value;
}

void Vec3ShaderUniform::write(std::string& content) const {

    char buff[256];

    if(baked) {
        snprintf(buff, 256, "#define %s vec3(%e, %e, %e)\n", name.c_str(), value.x, value.y, value.z);
    } else {
        snprintf(buff, 256, "uniform %s %s;\n", type_name.c_str(), name.c_str());
    }

    content += buff;
}

//Vec4ShaderUniform

Vec4ShaderUniform::Vec4ShaderUniform(Shader* shader, const std::string& name, const vec4& value) :
    value(value), ShaderUniform(shader, name, SHADER_UNIFORM_VEC4, "vec4") {
}

void Vec4ShaderUniform::setValue(const vec4& value) {
    if(baked && this->value == value) return;

    this->value = value;
    modified = true;
    initialized = true;
}

void Vec4ShaderUniform::apply() {
    glUniform4fv(getLocation(), 1, glm::value_ptr(value));
}

vec4& Vec4ShaderUniform::getValue() {
    return value;
}

void Vec4ShaderUniform::write(std::string& content) const {

    char buff[256];

    if(baked) {
        snprintf(buff, 256, "#define %s vec4(%e, %e, %e, %e)\n", name.c_str(), value.x, value.y, value.z, value.w);
    } else {
        snprintf(buff, 256, "uniform %s %s;\n", type_name.c_str(), name.c_str());
    }

    content += buff;
}

//Mat3ShaderUniform

Mat3ShaderUniform::Mat3ShaderUniform(Shader* shader, const std::string& name, const mat3& value) :
    value(value), ShaderUniform(shader, name, SHADER_UNIFORM_MAT3, "mat3") {
}

void Mat3ShaderUniform::setValue(const mat3& value) {
    if(baked && this->value == value) return;

    this->value = value;
    modified = true;
    initialized = true;
}

void Mat3ShaderUniform::apply() {
    glUniformMatrix3fv(getLocation(), 1, 0, glm::value_ptr(value));
}

mat3& Mat3ShaderUniform::getValue() {
    return value;
}

void Mat3ShaderUniform::write(std::string& content) const {

    char buff[1024];

    if(baked) {
        snprintf(buff, 1024, "#define %s mat3(%e, %e, %e, %e, %e, %e, %e, %e, %e)\n", name.c_str(),
                value[0][0], value[0][1], value[0][2],
                value[1][0], value[1][1], value[1][2],
                value[2][0], value[2][1], value[2][2]);

    } else {
        snprintf(buff, 1024, "uniform %s %s;\n", type_name.c_str(), name.c_str());
    }

    content += buff;
}

//Mat4ShaderUniform

Mat4ShaderUniform::Mat4ShaderUniform(Shader* shader, const std::string& name, const mat4& value) :
    value(value), ShaderUniform(shader, name, SHADER_UNIFORM_MAT4, "mat4") {
}

void Mat4ShaderUniform::setValue(const mat4& value) {
    if(baked && this->value == value) return;

    this->value = value;
    modified = true;
    initialized = true;
}

void Mat4ShaderUniform::apply() {
    glUniformMatrix4fv(getLocation(), 1, 0, glm::value_ptr(value));
}

mat4& Mat4ShaderUniform::getValue() {
    return value;
}

void Mat4ShaderUniform::write(std::string& content) const {

    char buff[1024];

    if(baked) {
        snprintf(buff, 1024, "#define %s mat4(%e, %e, %e, %e, %e, %e, %e, %e, %e, %e, %e, %e, %e, %e, %e, %e)\n", name.c_str(),
                value[0][0], value[0][1], value[0][2], value[0][3],
                value[1][0], value[1][1], value[1][2], value[1][3],
                value[2][0], value[2][1], value[2][2], value[2][3],
                value[3][0], value[3][1], value[3][2], value[3][3]);
    } else {
        snprintf(buff, 1024, "uniform %s %s;\n", type_name.c_str(), name.c_str());
    }

    content += buff;
}


//Vec2ArrayShaderUniform

Vec2ArrayShaderUniform::Vec2ArrayShaderUniform(Shader* shader, const std::string& name, size_t length, const vec2* value) :
    length(length), ShaderUniform(shader, name, SHADER_UNIFORM_VEC2_ARRAY, "vec2") {
    this->value = new vec2[length];
    if(value != 0) copyValue(value);
}

Vec2ArrayShaderUniform::~Vec2ArrayShaderUniform() {
    if(value) delete[] value;
}

vec2* Vec2ArrayShaderUniform::getValue() {
    return value;
}

void Vec2ArrayShaderUniform::copyValue(const vec2* value) {
    for(size_t i=0; i<length; i++) {
        this->value[i] = value[i];
    }
}

void Vec2ArrayShaderUniform::copyValue(const std::vector<vec2>& value) {
    for(size_t i=0; i<length; i++) {
        this->value[i] = value[i];
    }
}

void Vec2ArrayShaderUniform::setValue(const vec2* value) {
    if(baked) {
        bool match = true;

        for(size_t i=0;i<length;i++) {
            if(value[i] != this->value[i]) {
                match = false;
                break;
            }
        }

        if(match) return;
    }

    copyValue(value);

    modified = true;
    initialized = true;
}

void Vec2ArrayShaderUniform::setValue(const std::vector<vec2>& value) {
    if(baked) {
        bool match = true;

        for(size_t i=0;i<length;i++) {
            if(value[i] != this->value[i]) {
                match = false;
                break;
            }
        }

        if(match) return;
    }

    copyValue(value);

    modified = true;
    initialized = true;
}

void Vec2ArrayShaderUniform::apply() {
    glUniform2fv(getLocation(), length, glm::value_ptr(value[0]));
}

void Vec2ArrayShaderUniform::write(std::string& content) const {

    char buff[1024];

    if(baked) {
        snprintf(buff, 1024, "%s[%ld] %s = %s[] (\n", type_name.c_str(), length, name.c_str(), type_name.c_str());

        content += buff;

        for(size_t i=0; i<length; i++) {
            snprintf(buff, 1024, "    %s(%e, %e)", type_name.c_str(), value[i].x, value[i].y);
            content += buff;
            if(i<length-1) content += ",\n";
            else           content += "\n);\n";
        }

    } else {
        snprintf(buff, 1024, "uniform %s %s[%ld];\n", type_name.c_str(), name.c_str(), length);
        content += buff;
    }
}

//Vec3ArrayShaderUniform

Vec3ArrayShaderUniform::Vec3ArrayShaderUniform(Shader* shader, const std::string& name, size_t length, const vec3* value) :
    length(length), ShaderUniform(shader, name, SHADER_UNIFORM_VEC3_ARRAY, "vec3") {
    this->value = new vec3[length];
    if(value != 0) copyValue(value);
}

Vec3ArrayShaderUniform::~Vec3ArrayShaderUniform() {
    if(value) delete[] value;
}

vec3* Vec3ArrayShaderUniform::getValue() {
    return value;
}

void Vec3ArrayShaderUniform::copyValue(const vec3* value) {
    for(size_t i=0; i<length; i++) {
        this->value[i] = value[i];
    }
}

void Vec3ArrayShaderUniform::copyValue(const std::vector<vec3>& value) {
    for(size_t i=0; i<length; i++) {
        this->value[i] = value[i];
    }
}

void Vec3ArrayShaderUniform::setValue(const vec3* value) {
    if(baked) {
        bool match = true;

        for(size_t i=0;i<length;i++) {
            if(value[i] != this->value[i]) {
                match = false;
                break;
            }
        }

        if(match) return;
    }

    copyValue(value);

    modified = true;
    initialized = true;
}

void Vec3ArrayShaderUniform::setValue(const std::vector<vec3>& value) {
    if(baked) {
        bool match = true;

        for(size_t i=0;i<length;i++) {
            if(value[i] != this->value[i]) {
                match = false;
                break;
            }
        }

        if(match) return;
    }

    copyValue(value);

    modified = true;
    initialized = true;
}

void Vec3ArrayShaderUniform::apply() {
    glUniform3fv(getLocation(), length, glm::value_ptr(value[0]));
}

void Vec3ArrayShaderUniform::write(std::string& content) const {

    char buff[1024];

    if(baked) {
        snprintf(buff, 1024, "%s[%ld] %s = %s[] (\n", type_name.c_str(), length, name.c_str(), type_name.c_str());

        content += buff;

        for(size_t i=0; i<length; i++) {
            snprintf(buff, 1024, "    %s(%e, %e, %e)", type_name.c_str(), value[i].x, value[i].y, value[i].z);
            content += buff;
            if(i<length-1) content += ",\n";
            else           content += "\n);\n";
        }

    } else {
        snprintf(buff, 1024, "uniform %s %s[%ld];\n", type_name.c_str(), name.c_str(), length);
        content += buff;
    }
}

//Vec4ArrayShaderUniform

Vec4ArrayShaderUniform::Vec4ArrayShaderUniform(Shader* shader, const std::string& name, size_t length, const vec4* value) :
    length(length), ShaderUniform(shader, name, SHADER_UNIFORM_VEC4_ARRAY, "vec4") {
    this->value = new vec4[length];
    if(value != 0) copyValue(value);
}

Vec4ArrayShaderUniform::~Vec4ArrayShaderUniform() {
    if(value) delete[] value;
}

vec4* Vec4ArrayShaderUniform::getValue() {
    return value;
}

void Vec4ArrayShaderUniform::copyValue(const std::vector<vec4>& value) {
    for(size_t i=0; i<length; i++) {
        this->value[i] = value[i];
    }
}

void Vec4ArrayShaderUniform::copyValue(const vec4* value) {
    for(size_t i=0; i<length; i++) {
        this->value[i] = value[i];
    }
}

void Vec4ArrayShaderUniform::setValue(const vec4* value) {
    if(baked) {
        bool match = true;

        for(size_t i=0;i<length;i++) {
            if(value[i] != this->value[i]) {
                match = false;
                break;
            }
        }

        if(match) return;
    }

    copyValue(value);

    modified = true;
    initialized = true;
}

void Vec4ArrayShaderUniform::setValue(const std::vector<vec4>& value) {
    if(baked) {
        bool match = true;

        for(size_t i=0;i<length;i++) {
            if(value[i] != this->value[i]) {
                match = false;
                break;
            }
        }

        if(match) return;
    }

    copyValue(value);

    modified = true;
    initialized = true;
}

void Vec4ArrayShaderUniform::apply() {
    glUniform4fv(getLocation(), length, glm::value_ptr(value[0]));
}

void Vec4ArrayShaderUniform::write(std::string& content) const {

    char buff[1024];

    if(baked) {
        snprintf(buff, 1024, "%s[%ld] %s = %s[] (\n", type_name.c_str(), length, name.c_str(), type_name.c_str());

        content += buff;

        for(size_t i=0; i<length; i++) {
            snprintf(buff, 1024, "    %s(%e, %e, %e, %e)", type_name.c_str(), value[i].x, value[i].y, value[i].z, value[i].w);
            content += buff;
            if(i<length-1) content += ",\n";
            else           content += "\n);\n";
        }

    } else {
        snprintf(buff, 1024, "uniform %s %s[%ld];\n", type_name.c_str(), name.c_str(), length);
        content += buff;
    }

}

//ShaderPart

ShaderPart::ShaderPart() {
}

void ShaderPart::setSourceFile(const std::string& filename) {
    this->filename = filename;
    loadSourceFile();
}

void ShaderPart::loadSourceFile() {

    processed_source.clear();
    raw_source.clear();

    // get length
    std::ifstream in(filename.c_str());

    if(!in.is_open()) {
        throw ShaderException("could not open '%s'", filename.c_str());
    }

    std::string line;
    while( std::getline(in,line) ) {
        raw_source += line;
        raw_source += "\n";
    }

    in.close();
}

void ShaderPart::reload() {
    loadSourceFile();
}

void ShaderPart::reset() {
    processed_source.clear();
    defines.clear();
}

void ShaderPart::setSource(const std::string& source) {
    raw_source = source;
}

void ShaderPart::substitute(std::string& source, const std::string& name, const std::string& value) {

    std::string::size_type next_match;

    for(next_match = source.find(name);
        next_match != std::string::npos;
        next_match = source.find(name, next_match)) {
        source.replace(next_match, name.length(), value);
        next_match += value.length();
    }
}

void ShaderPart::applyDefines(std::string& source) {

    for(std::map<std::string, std::string>::iterator it = defines.begin(); it != defines.end(); it++) {
        substitute(source, it->first, it->second);
    }
}

void ShaderPart::define(const std::string& name, const std::string& value) {
    define(name, value.c_str());
}

void ShaderPart::define(const std::string& name, const char *value, ...) {

    va_list vl;
    char sub[65536];

    char* buffer = sub;

    va_start(vl, value);
        int string_size = vsnprintf(sub, sizeof(sub), value, vl);

        if(string_size > sizeof(sub)) {
            buffer = new char[string_size];
            string_size = vsnprintf(buffer, string_size, value, vl);
        }
    va_end(vl);

    defines[name] = buffer;

    if(buffer != sub) delete[] buffer;
}

void ShaderPart::define(const std::string& name) {
    define(name, "");
}

bool ShaderPart::isDefined(const std::string& name) {
    std::map<std::string,std::string>::iterator it = defines.find(name);
    return (it != defines.end());
}

void ShaderPart::preprocess() {

    processed_source.clear();

    // NOTE: this preprocessor only handles basic ifdef / endif blocks

    std::vector<std::string> matches;

    std::stringstream in(raw_source);

    std::string line;
    bool skipdef = false;

    while( std::getline(in,line) ) {
        if(Shader_ifdef.match(line, &matches)) {
            if( (matches[0] == "ifdef" && !isDefined(matches[1])) || (matches[0] == "ifndef" && isDefined(matches[1])) ) {
                skipdef = true;
            }
            continue;
        }
        if(Shader_endif.match(line, &matches)) {
            skipdef = false;
            continue;
        }

        if(!skipdef) {
            applyDefines(line);

            processed_source.append(line);
            processed_source.append("\n");
        }
    }
}

const std::string& ShaderPart::getSource() {
    if(processed_source.empty()) preprocess();
    return processed_source;
}

//ShaderPass

ShaderPass::ShaderPass(Shader* parent, int shader_object_type, const std::string& shader_object_desc) : parent(parent), shader_object_type(shader_object_type), shader_object_desc(shader_object_desc) {
    shader_object = 0;
    version = 0;
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

bool ShaderPass::errorContext(const char* log_message, std::string& context) {

    std::vector<std::string> matches;

    if(   !Shader_error_line.match(log_message, &matches)
       && !Shader_error2_line.match(log_message, &matches)
       && !(Logger::getDefault()->getLevel() == LOG_LEVEL_WARN && Shader_warning_line.match(log_message, &matches)))
        return false;

    int line_no = atoi(matches[0].c_str());

    std::stringstream in(shader_object_source);

    int i = 1;
    int amount = 3;

    char line_detail[1024];

    std::string line;
    while( std::getline(in,line) ) {

        if(i==line_no || (i<line_no && i+amount>=line_no) || (i>line_no && i-amount<=line_no)) {
            snprintf(line_detail, 1024, "%s%4d | %s\n", (i==line_no ? "-> ": "   "), i, line.c_str());
            context += line_detail;
        }
        i++;
    }

    return true;
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
        errorContext(info_log, context);

        if(!compile_success) {
            throw ShaderException("%s shader '%s' failed to compile:\n%s\n%s",
                                  shader_object_desc.c_str(),
                                  resource_desc,
                                  info_log,
                                  context.c_str());



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
        throw ShaderException("%s shader '%s' failed to compile",
                              shader_object_desc.c_str(),
                              resource_desc);
    }
}

void ShaderPass::toString(std::string& out) {
    if(version!=0) {
        out.append(str(boost::format("#version %d\n") % version));
    }

    for(std::map<std::string, std::string>::iterator it = extensions.begin(); it != extensions.end(); it++) {
        out.append(str(boost::format("#extension %s : %s\n") % it->first % it->second));
    }

    for(ShaderUniform* u : uniforms) {
        u->write(out);
    }

    out.append(source);
}


const std::string& ShaderPass::getObjectSource() {
    return shader_object_source;
}

bool ShaderPass::isEmpty() {
    return source.empty();
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

//add uniform, unless parent Shader has this in which case link to it
ShaderUniform* ShaderPass::addArrayUniform(const std::string& name, const std::string& type, size_t length) {

    ShaderUniform* uniform = 0;

    if((uniform = parent->getUniform(name)) == 0) {

        if(type == "vec2") {
            uniform = new Vec2ArrayShaderUniform(parent, name, length);
        } else if(type == "vec3") {
            uniform = new Vec3ArrayShaderUniform(parent, name, length);
        } else if(type == "vec4") {
            uniform = new Vec4ArrayShaderUniform(parent, name, length);
        } else {
            throw ShaderException("shader uniform arrays for type '%s' not implemented", type.c_str());
        }

        uniform->setInitialized(false);

        parent->addUniform(uniform);
    }

    uniforms.push_back(uniform);

    return uniform;
}

ShaderUniform* ShaderPass::addUniform(const std::string& name, const std::string& type) {

    ShaderUniform* uniform = 0;

    if((uniform = parent->getUniform(name)) == 0) {

        if(type == "float") {
            uniform = new FloatShaderUniform(parent, name);
        } else if(type == "int") {
            uniform = new IntShaderUniform(parent, name);
        } else if(type == "bool") {
            uniform = new BoolShaderUniform(parent, name);
        } else if(type == "sampler1D") {
            uniform = new Sampler1DShaderUniform(parent, name);
        } else if(type == "sampler2D") {
            uniform = new Sampler2DShaderUniform(parent, name);
        } else if(type == "vec2") {
            uniform = new Vec2ShaderUniform(parent, name);
        } else if(type == "vec3") {
            uniform = new Vec3ShaderUniform(parent, name);
        } else if(type == "vec4") {
            uniform = new Vec4ShaderUniform(parent, name);
        } else if(type == "mat3") {
            uniform = new Mat3ShaderUniform(parent, name);
        } else if(type == "mat4") {
            uniform = new Mat4ShaderUniform(parent, name);
        } else {
            throw ShaderException("unsupported shader uniform type '%s'", type.c_str());
        }

        uniform->setInitialized(false);

        parent->addUniform(uniform);
    }

    uniforms.push_back(uniform);

    return uniform;
}

bool ShaderPass::preprocess(const std::string& line) {

    std::vector<std::string> matches;

    if(Shader_pre_version.match(line, &matches)) {
        version = atoi(matches[0].c_str());
        return true;
    }

    if(Shader_pre_extension.match(line, &matches)) {
        extensions[matches[0]] = matches[1];
        return true;
    }

#ifndef USE_MGL_NAMESPACE
    if(Shader_pre_include.match(line, &matches)) {

        std::string include_file = shadermanager.getDir() + matches[0];

        includeFile(include_file);

        return true;
    }
#endif
    
    if(Shader_uniform_def.match(line, &matches)) {
        std::string uniform_type = matches[0];
        std::string uniform_name = matches[1];
                
        ShaderUniform* uniform = 0;
                
        if(matches.size() > 2 && !matches[2].empty()) {
            size_t uniform_length = atoi(matches[2].c_str());
            uniform = addArrayUniform(uniform_name, uniform_type, uniform_length);
        } else {
            uniform = addUniform(uniform_name, uniform_type);
        }

        if(matches.size() > 3 && !matches[3].empty()) uniform->setComment(matches[3]);

        return true;
    }

    return false;
}

void ShaderPass::includeFile(const std::string& filename) {

    // get length
    std::ifstream in(filename.c_str());

    if(!in.is_open()) {
        throw ShaderException("could not open '%s'", filename.c_str());
    }

    std::string line;
    while( std::getline(in,line) ) {
        if(!preprocess(line)) {
            source += line;
            source += "\n";
        }
    }

    in.close();
}

void ShaderPass::includeSource(const std::string& string) {

    std::stringstream in(string);

    std::string line;
    while( std::getline(in,line) ) {
        if(!preprocess(line)) {
            source += line;
            source += "\n";
        }
    }
}

VertexShader::VertexShader(Shader* parent) : ShaderPass(parent, GL_VERTEX_SHADER, "vertex") {
}

FragmentShader::FragmentShader(Shader* parent) : ShaderPass(parent, GL_FRAGMENT_SHADER, "fragment") {
}

GeometryShader::GeometryShader(Shader* parent) : ShaderPass(parent, GL_GEOMETRY_SHADER_ARB, "geometry") {
}

void GeometryShader::attachTo(unsigned int program) {
    ShaderPass::attachTo(program);
}

//Shader

Shader::Shader(const std::string& prefix)
    : prefix(prefix), Resource(prefix) {

    setDefaults();

    loadPrefix();
}

Shader::Shader() {
    setDefaults();
}

void Shader::loadPrefix() {

    if(vertex_shader != 0) delete vertex_shader;
    vertex_shader = 0;

    if(fragment_shader != 0) delete fragment_shader;
    fragment_shader = 0;

// TODO: make this work with MGL
#ifndef USE_MGL_NAMESPACE
    std::string shader_dir = shadermanager.getDir();
#else
    std::string shader_dir = "";   
#endif
    std::string vertex_file   = shader_dir + prefix + std::string(".vert");
    std::string fragment_file = shader_dir + prefix + std::string(".frag");

    vertex_shader = new VertexShader(this);
    vertex_shader->includeFile(vertex_file);

    fragment_shader = new FragmentShader(this);
    fragment_shader->includeFile(fragment_file);

    load();
}

void Shader::setDynamicCompile(bool dynamic_compile) {
    this->dynamic_compile = dynamic_compile;
}

void Shader::setDefaults() {
    vertex_shader   = 0;
    fragment_shader = 0;
    geometry_shader = 0;
    program = 0;
    dynamic_compile = false;
}

Shader::~Shader() {
    clear();
}

void Shader::clear() {
    unload();

    for(std::map<std::string, ShaderUniform*>::iterator it= uniforms.begin(); it!=uniforms.end();it++) {
        delete it->second;
    }
    uniforms.clear();

    if(vertex_shader != 0)   delete vertex_shader;
    if(geometry_shader != 0) delete geometry_shader;
    if(fragment_shader != 0) delete fragment_shader;

    vertex_shader   = 0;
    geometry_shader = 0;
    fragment_shader = 0;
}

void Shader::unload() {
    if(program != 0) glDeleteProgram(program);
    program = 0;

    for(std::map<std::string, ShaderUniform*>::iterator it= uniforms.begin(); it!=uniforms.end();it++) {
        it->second->unload();
    }
}

bool Shader::isEmpty() {

    if(   (!vertex_shader   || vertex_shader->isEmpty())
       && (!fragment_shader || fragment_shader->isEmpty())
       && (!geometry_shader || geometry_shader->isEmpty())) {
        return true;
    }

    return false;
}

void Shader::reload(bool force) {
    if(isEmpty()) return;

    if(force && !prefix.empty()) {
        loadPrefix();
    } else {
        load();
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
          throw ShaderException("shader '%s' failed to link", resource_desc);
    }
}

void Shader::bind() {
    glUseProgram(program);
}

void Shader::unbind() {
    glUseProgram(0);
}

void Shader::use() {

    if(Logger::getDefault()->getLevel() == LOG_LEVEL_PEDANTIC) {
        for(std::map<std::string, ShaderUniform*>::iterator it= uniforms.begin(); it!=uniforms.end();it++) {
            ShaderUniform* u = it->second;

            if(!u->isInitialized()) pedanticLog("shader '%s': uniform '%s' was never initialized", (!resource_name.empty() ? resource_name.c_str() : "???"), u->getName().c_str());
        }
    }

    if(dynamic_compile && needsCompile()) {
        unbind();
        load();
        infoLog("shader '%s' recompiled", resource_name.c_str());
    }

    bind();

    applyUniforms();
}

unsigned int Shader::getProgram() {
    return program;
}

void Shader::addUniform(ShaderUniform* uniform) {

    if(getUniform(uniform->getName()) != 0) {
        throw ShaderException("shader already has a uniform named '%s'", uniform->getName().c_str() );
    }

    uniforms[uniform->getName()] = uniform;
}

ShaderUniform* Shader::getUniform(const std::string& name) {
    std::map<std::string, ShaderUniform*>::iterator it = uniforms.find(name);

    if(it != uniforms.end()) {
        return it->second;
    }
    return 0;
}

ShaderPass* Shader::grabShaderPass(unsigned int shader_object_type) {

    ShaderPass* shader_pass = 0;

    switch(shader_object_type) {
        case GL_VERTEX_SHADER:
            if(!vertex_shader) vertex_shader = new VertexShader(this);
            shader_pass = vertex_shader;
            break;
        case GL_GEOMETRY_SHADER_ARB:
            if(!geometry_shader) geometry_shader = new GeometryShader(this);
            shader_pass = geometry_shader;
            break;
        case GL_FRAGMENT_SHADER:
            if(!fragment_shader) fragment_shader = new FragmentShader(this);
            shader_pass = fragment_shader;
            break;
    }

    return shader_pass;
}


void Shader::includeSource(unsigned int shader_object_type, const std::string& source) {

    ShaderPass* pass = grabShaderPass(shader_object_type);

    pass->includeSource(source);
}

void Shader::includeFile(unsigned int shader_object_type, const std::string& filename) {

    ShaderPass* pass = grabShaderPass(shader_object_type);

    pass->includeFile(filename);
}

void Shader::addSubstitute(const std::string& name, const char *value, ...) {

    va_list vl;
    char sub[65536];

    char* buffer = sub;

    va_start(vl, value);
        int string_size = vsnprintf(sub, sizeof(sub), value, vl);

        if(string_size > sizeof(sub)) {
            buffer = new char[string_size];
            string_size = vsnprintf(buffer, string_size, value, vl);
        }
    va_end(vl);

    substitutions[name] = buffer;

    if(buffer != sub) delete[] buffer;
}

void Shader::substitute(std::string& source, const std::string& name, const std::string& value) {

    std::string::size_type next_match;

    for(next_match = source.find(name);
        next_match != std::string::npos;
        next_match = source.find(name, next_match)) {
        source.replace(next_match, name.length(), value);
        next_match += value.length();
    }
}

void Shader::applySubstitutions(std::string& source) {

    for(std::map<std::string, std::string>::iterator it = substitutions.begin(); it != substitutions.end(); it++) {
        substitute(source, it->first, it->second);
    }
}

void Shader::setBool (const std::string& name, bool value) {
    ShaderUniform* uniform = getUniform(name);

    if(!uniform || uniform->getType() != SHADER_UNIFORM_BOOL) return;

    ((BoolShaderUniform*)uniform)->setValue(value);
}

void Shader::setInteger (const std::string& name, int value) {
    ShaderUniform* uniform = getUniform(name);

    if(!uniform || uniform->getType() != SHADER_UNIFORM_INT) return;

    ((IntShaderUniform*)uniform)->setValue(value);
}

void Shader::setSampler1D (const std::string& name, int value) {
    ShaderUniform* uniform = getUniform(name);

    if(!uniform || uniform->getType() != SHADER_UNIFORM_SAMPLER_1D) return;

    ((Sampler1DShaderUniform*)uniform)->setValue(value);
}

void Shader::setSampler2D (const std::string& name, int value) {
    ShaderUniform* uniform = getUniform(name);

    if(!uniform || uniform->getType() != SHADER_UNIFORM_SAMPLER_2D) return;

    ((Sampler2DShaderUniform*)uniform)->setValue(value);
}

void Shader::setFloat(const std::string& name, float value) {
    ShaderUniform* uniform = getUniform(name);

    if(!uniform || uniform->getType() != SHADER_UNIFORM_FLOAT) return;

    ((FloatShaderUniform*)uniform)->setValue(value);
}

void Shader::setVec2 (const std::string& name, const vec2& value) {
    ShaderUniform* uniform = getUniform(name);

    if(!uniform || uniform->getType() != SHADER_UNIFORM_VEC2) return;

    ((Vec2ShaderUniform*)uniform)->setValue(value);
}

void Shader::setVec3 (const std::string& name, const vec3& value) {
    ShaderUniform* uniform = getUniform(name);

    if(!uniform || uniform->getType() != SHADER_UNIFORM_VEC3) return;

    ((Vec3ShaderUniform*)uniform)->setValue(value);
}

void Shader::setVec2Array (const std::string& name, vec2* value) {
    ShaderUniform* uniform = getUniform(name);

    if(!uniform || uniform->getType() != SHADER_UNIFORM_VEC2_ARRAY) return;

    ((Vec2ArrayShaderUniform*)uniform)->setValue(value);
}

void Shader::setVec2Array (const std::string& name, std::vector<vec2>& value) {
    ShaderUniform* uniform = getUniform(name);

    if(!uniform || uniform->getType() != SHADER_UNIFORM_VEC2_ARRAY) return;

    ((Vec2ArrayShaderUniform*)uniform)->setValue(value);
}

void Shader::setVec3Array (const std::string& name, vec3* value) {
    ShaderUniform* uniform = getUniform(name);

    if(!uniform || uniform->getType() != SHADER_UNIFORM_VEC3_ARRAY) return;

    ((Vec3ArrayShaderUniform*)uniform)->setValue(value);
}

void Shader::setVec3Array (const std::string& name, std::vector<vec3>& value) {
    ShaderUniform* uniform = getUniform(name);

    if(!uniform || uniform->getType() != SHADER_UNIFORM_VEC3_ARRAY) return;

    ((Vec3ArrayShaderUniform*)uniform)->setValue(value);
}

void Shader::setVec4Array (const std::string& name, vec4* value) {
    ShaderUniform* uniform = getUniform(name);

    if(!uniform || uniform->getType() != SHADER_UNIFORM_VEC4_ARRAY) return;

    ((Vec4ArrayShaderUniform*)uniform)->setValue(value);
}

void Shader::setVec4Array (const std::string& name, std::vector<vec4>& value) {
    ShaderUniform* uniform = getUniform(name);

    if(!uniform || uniform->getType() != SHADER_UNIFORM_VEC4_ARRAY) return;

    ((Vec4ArrayShaderUniform*)uniform)->setValue(value);
}

void Shader::setVec4 (const std::string& name, const vec4& value) {
    ShaderUniform* uniform = getUniform(name);

    if(!uniform || uniform->getType() != SHADER_UNIFORM_VEC4) return;

    ((Vec4ShaderUniform*)uniform)->setValue(value);
}

void Shader::setMat3 (const std::string& name, const mat3& value) {
    ShaderUniform* uniform = getUniform(name);

    if(!uniform || uniform->getType() != SHADER_UNIFORM_MAT3) return;

    ((Mat3ShaderUniform*)uniform)->setValue(value);
}

void Shader::setMat4 (const std::string& name, const mat4& value) {
    ShaderUniform* uniform = getUniform(name);

    if(!uniform || uniform->getType() != SHADER_UNIFORM_MAT4) return;

    ((Mat4ShaderUniform*)uniform)->setValue(value);
}

void Shader::setBaked(const std::string& name, bool baked) {
    ShaderUniform* uniform = getUniform(name);

    if(!uniform) return;

    uniform->setBaked(baked);
}

void Shader::setBakedUniforms(bool baked) {
    for(std::map<std::string, ShaderUniform*>::iterator it= uniforms.begin(); it!=uniforms.end();it++) {
        it->second->setBaked(baked);
    }
}


void Shader::getUniforms(std::list<ShaderUniform*>& uniform_list) {
    for(std::map<std::string, ShaderUniform*>::iterator it= uniforms.begin(); it!=uniforms.end();it++) {
        uniform_list.push_back(it->second);
    }
}

void Shader::applyUniforms() {
    for(std::map<std::string, ShaderUniform*>::iterator it= uniforms.begin(); it!=uniforms.end();it++) {
        if(!it->second->isBaked()) it->second->apply();
    }
}

bool Shader::needsCompile() {

    for(std::map<std::string, ShaderUniform*>::iterator it= uniforms.begin(); it!=uniforms.end();it++) {
        ShaderUniform* u = it->second;

        if(u->isBaked() && u->isModified()) {
            //infoLog("baked uniform %s needs update", u->getName().c_str());
            return true;
        }
    }

    return false;
}
