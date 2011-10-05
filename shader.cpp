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
Regex Shader_uniform_def("^\\s*uniform\\s+(\\w+)\\s+(\\w+)\\s*;\\s*$");

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

//ShaderUniform

ShaderUniform::ShaderUniform(Shader* shader, const std::string& name, int uniform_type, const std::string& type_name)
    : shader(shader), name(name), location(-1), modified(false), baked(false), uniform_type(uniform_type), type_name(type_name) {
}

void ShaderUniform::unload() {
    location = -1;
}

const std::string& ShaderUniform::getName() const {
    return name;
}

GLint ShaderUniform::getLocation() {

    // TODO: (re-)compiling the shader should break the uniform location caching.

    if(location != -1) return location;

    location = glGetUniformLocation( shader->getProgram(), name.c_str() );

    return location;
}

//FloatShaderUniform

FloatShaderUniform::FloatShaderUniform(Shader* shader, const std::string& name, float value) :
    value(value), ShaderUniform(shader, name, SHADER_UNIFORM_FLOAT, "float") {
}

void FloatShaderUniform::setValue(float value) {
    this->value = value;
    modified = true;
    glUniform1f(getLocation(), value);
}

float FloatShaderUniform::getValue() const {
    return value;
}

void FloatShaderUniform::write(std::string& content) const {

    char buff[256];

    if(baked) {
        snprintf(buff, 256, "#define %s %.5f\n", name.c_str(), value);
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
    this->value = value;
    modified = true;
    glUniform1i(getLocation(), value);
}

float IntShaderUniform::getValue() const {
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
    value(value), ShaderUniform(shader, name, SHADER_UNIFORM_INT, "bool") {
}

void BoolShaderUniform::setValue(bool value) {
    this->value = value;
    modified = true;
    glUniform1i(getLocation(), value);
}

float BoolShaderUniform::getValue() const {
    return value;
}

void BoolShaderUniform::write(std::string& content) const {

    char buff[256];

    if(baked) {
        snprintf(buff, 256, "#define %s %d\n", name.c_str(), value);
    } else {
        snprintf(buff, 256, "uniform %s %s;\n", type_name.c_str(), name.c_str());
    }

    content += buff;
}

//Sampler2DShaderUniform

Sampler2DShaderUniform::Sampler2DShaderUniform(Shader* shader, const std::string& name, int value) :
    value(value), ShaderUniform(shader, name, SHADER_UNIFORM_INT, "sampler2D") {
}

void Sampler2DShaderUniform::setValue(int value) {
    this->value = value;
    modified = true;
    glUniform1i(getLocation(), value);
}

float Sampler2DShaderUniform::getValue() const {
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
    this->value = value;
    modified = true;
    glUniform2fv(getLocation(), 1, glm::value_ptr(value));
}

const vec2& Vec2ShaderUniform::getValue() const {
    return value;
}

void Vec2ShaderUniform::write(std::string& content) const {

    char buff[256];

    if(baked) {
        snprintf(buff, 256, "#define %s vec2(%.5f, %.5f)\n", name.c_str(), value.x, value.y);
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
    this->value = value;
    modified = true;
    glUniform3fv(getLocation(), 1, glm::value_ptr(value));
}

const vec3& Vec3ShaderUniform::getValue() const {
    return value;
}

void Vec3ShaderUniform::write(std::string& content) const {

    char buff[256];

    if(baked) {
        snprintf(buff, 256, "#define %s vec3(%.5f, %.5f, %.5f)\n", name.c_str(), value.x, value.y, value.z);
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
    this->value = value;
    modified = true;
    glUniform4fv(getLocation(), 1, glm::value_ptr(value));
}

const vec4& Vec4ShaderUniform::getValue() const {
    return value;
}

void Vec4ShaderUniform::write(std::string& content) const {

    char buff[256];

    if(baked) {
        snprintf(buff, 256, "#define %s vec4(%.5f, %.5f, %.5f, %.5f)\n", name.c_str(), value.x, value.y, value.z, value.w);
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
    this->value = value;
    modified = true;
    glUniformMatrix3fv(getLocation(), 1, 0, glm::value_ptr(value));
}

const mat3& Mat3ShaderUniform::getValue() const {
    return value;
}

void Mat3ShaderUniform::write(std::string& content) const {

    char buff[256];

    if(baked) {
        snprintf(buff, 256, "#define %s mat3(%.5f, %.5f, %.5f, %.5f, %.5f, %.5f, %.5f, %.5f, %.5f)\n", name.c_str(),
                value[0][0], value[0][1], value[0][2],
                value[1][0], value[1][1], value[1][2],
                value[2][0], value[2][1], value[2][2]);

    } else {
        snprintf(buff, 256, "uniform %s %s;\n", type_name.c_str(), name.c_str());
    }

    content += buff;
}

//Mat4ShaderUniform

Mat4ShaderUniform::Mat4ShaderUniform(Shader* shader, const std::string& name, const mat4& value) :
    value(value), ShaderUniform(shader, name, SHADER_UNIFORM_MAT4, "mat4") {
}

void Mat4ShaderUniform::setValue(const mat4& value) {
    this->value = value;
    modified = true;
    glUniformMatrix4fv(getLocation(), 1, 0, glm::value_ptr(value));
}

const mat4& Mat4ShaderUniform::getValue() const {
    return value;
}

void Mat4ShaderUniform::write(std::string& content) const {

    char buff[256];

    if(baked) {
        snprintf(buff, 256, "#define %s mat4(%.5f, %.5f, %.5f, %.5f, %.5f, %.5f, %.5f, %.5f, %.5f, %.5f, %.5f, %.5f, %.5f, %5f, %.5f, %.5f)\n", name.c_str(),
                value[0][0], value[0][1], value[0][2], value[0][3],
                value[1][0], value[1][1], value[1][2], value[1][3],
                value[2][0], value[2][1], value[2][2], value[2][3],
                value[3][0], value[3][1], value[3][2], value[3][3]);
    } else {
        snprintf(buff, 256, "uniform %s %s;\n", type_name.c_str(), name.c_str());
    }

    content += buff;
}

//ShaderPass

ShaderPass::ShaderPass(Shader* parent, GLint shader_object_type, const std::string& shader_object_desc) : parent(parent), shader_object_type(shader_object_type), shader_object_desc(shader_object_desc) {
    shader_object = 0;
}

ShaderPass::~ShaderPass() {
    unload();
}

void ShaderPass::unload() {
    if(shader_object!=0) glDeleteShader(shader_object);
    shader_object = 0;
}

void ShaderPass::attachTo(GLenum program) {
    glAttachShader(program, shader_object);
}

void ShaderPass::checkError() {
    if(!shader_object) return;

    GLint compile_success;
    glGetShaderiv(shader_object, GL_COMPILE_STATUS, &compile_success);

    GLint info_log_length;
    glGetShaderiv(shader_object, GL_INFO_LOG_LENGTH, &info_log_length);

    const char* resource_desc = !parent->resource_name.empty() ? parent->resource_name.c_str() : "???";

    if(info_log_length > 1) {
        char info_log[info_log_length];

        glGetShaderInfoLog(shader_object, info_log_length, &info_log_length, info_log);

        if(!compile_success) {
            throw SDLAppException("%s shader '%s' failed to compile:\n%s",
                                  shader_object_desc.c_str(),
                                  resource_desc,
                                  info_log);
        }

        if(shadermanager.warnings) {
            fprintf(stderr, "%s shader '%s':\n%s",
                            shader_object_desc.c_str(),
                            resource_desc,
                            info_log);
        }

        return;
    }

    if(!compile_success) {
        throw SDLAppException("%s shader '%s' failed to compile",
                              shader_object_desc.c_str(),
                              resource_desc);
    }
}

void ShaderPass::compile() {

    if(!shader_object) shader_object = glCreateShader(shader_object_type);

    if(source.empty()) return;

    std::string shader_object_src;

    foreach(ShaderUniform* u, uniforms) {
        u->write(shader_object_src);
    }

    shader_object_src += source;

    debugLog("src:\n%s", shader_object_src.c_str());

    const char* source_ptr = shader_object_src.c_str();
    int source_len = shader_object_src.size();

    glShaderSource(shader_object, 1, (const GLchar**) &source_ptr, &source_len);
    glCompileShader(shader_object);

    checkError();
}

//add uniform, unless parent Shader has this in which case link to it
void ShaderPass::addUniform(const std::string& name, const std::string& type, bool baked) {

    ShaderUniform* uniform = 0;

    if((uniform = parent->getUniform(name)) == 0) {

        if(type == "float") {
            uniform = new FloatShaderUniform(parent, name);
        } else if(type == "int") {
            uniform = new IntShaderUniform(parent, name);
        } else if(type == "bool") {
            uniform = new BoolShaderUniform(parent, name);
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
            throw SDLAppException("unsupported shader uniform type '%s'", type.c_str());
        }

        if(baked) uniform->setBaked(true);

        parent->addUniform(uniform);
    }

    uniforms.push_back(uniform);
}

bool ShaderPass::preprocess(const std::string& line) {

    std::vector<std::string> matches;

    if(Shader_pre_include.match(line, &matches)) {

        std::string include_file = shadermanager.getDir() + matches[0];

        includeFile(include_file);

        return true;
    }

    if(Shader_uniform_def.match(line, &matches)) {
        std::string uniform_type = matches[0];
        std::string uniform_name = matches[1];

        addUniform(uniform_name, uniform_type);

        return true;
    }

    return false;
}

void ShaderPass::includeFile(const std::string& filename) {

    // get length
    std::ifstream in(filename.c_str());

    if(!in.is_open()) {
        throw SDLAppException("could not open '%s'", filename.c_str());
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

GeometryShader::GeometryShader(Shader* parent, GLenum input_type, GLenum output_type, GLuint max_vertices)
    : input_type(input_type), output_type(output_type), max_vertices(max_vertices),
      ShaderPass(parent, GL_GEOMETRY_SHADER_EXT, "geometry") {
}

void GeometryShader::attachTo(GLenum program) {
    ShaderPass::attachTo(program);

    glProgramParameteriEXT(program, GL_GEOMETRY_INPUT_TYPE_EXT,   input_type);
    glProgramParameteriEXT(program, GL_GEOMETRY_OUTPUT_TYPE_EXT,  output_type);
    glProgramParameteriEXT(program, GL_GEOMETRY_VERTICES_OUT_EXT, max_vertices);
}

//Shader

Shader::Shader(const std::string& prefix) : Resource(prefix) {

    setDefaults();

    std::string shader_dir = shadermanager.getDir();

    std::string vertex_file   = shader_dir + prefix + std::string(".vert");
    std::string fragment_file = shader_dir + prefix + std::string(".frag");

    vertex_shader = new VertexShader(this);
    vertex_shader->includeFile(vertex_file);

    fragment_shader = new FragmentShader(this);
    fragment_shader->includeFile(fragment_file);

    load();
}

Shader::Shader() {
    setDefaults();
}

void Shader::setDefaults() {
    vertex_shader   = 0;
    fragment_shader = 0;
    geometry_shader = 0;
    program = 0;
}

Shader::~Shader() {
    unload();

    for(std::map<std::string, ShaderUniform*>::iterator it= uniforms.begin(); it!=uniforms.end();it++) {
        delete it->second;
    }
    uniforms.clear();
}

void Shader::unload() {
    if(program != 0) glDeleteProgram(program);
    program = 0;

    for(std::map<std::string, ShaderUniform*>::iterator it= uniforms.begin(); it!=uniforms.end();it++) {
        it->second->unload();
    }
}

void Shader::load() {
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

void Shader::use() {
    glUseProgram(program);
}

GLenum Shader::getProgram() {
    return program;
}

void Shader::addUniform(ShaderUniform* uniform) {

    if(getUniform(uniform->getName()) != 0) {
        throw SDLAppException("shader already has a uniform named '%s'", uniform->getName().c_str() );
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

ShaderPass* Shader::grabShaderPass(GLenum shader_object_type) {

    ShaderPass* shader_pass = 0;

    switch(shader_object_type) {
        case GL_VERTEX_SHADER:
            if(!vertex_shader) vertex_shader = new VertexShader(this);
            shader_pass = vertex_shader;
            break;
        case GL_GEOMETRY_SHADER_EXT:
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


void Shader::includeSource(GLenum shader_object_type, const std::string& source) {

    ShaderPass* pass = grabShaderPass(shader_object_type);

    pass->includeSource(source);
}

void Shader::includeFile(GLenum shader_object_type, const std::string& filename) {

    ShaderPass* pass = grabShaderPass(shader_object_type);

    pass->includeFile(filename);
}

void Shader::setInteger (const std::string& name, int value) {
    ShaderUniform* uniform = getUniform(name);

    if(!uniform || uniform->getType() != SHADER_UNIFORM_INT) return;

    ((IntShaderUniform*)uniform)->setValue(value);
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

void Shader::bake() {

    for(std::map<std::string, ShaderUniform*>::iterator it= uniforms.begin(); it!=uniforms.end();it++) {
        it->second->setBaked(true);
    }
    load();
}

