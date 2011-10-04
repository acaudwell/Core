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
Regex Shader_uniform_def("^\\s*uniform\\s+(\\w)\\s+(\\w)\\s*;\\s*$")

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

ShaderUniform::ShaderUniform(Shader* shader, const std::string& name, int uniform_type)
    : shader(shader), name(name), location(-1), modified(false), baked(false), uniform_type(uniform_type) {
}

const std::string& ShaderUniform::getName() {
    return name;
}

int ShaderUniform::getLocation() {

    if(location != -1) return location;

    location = glGetUniformLocation( shader->getProgram(), name.c_str() );

    return location;
}

//FloatShaderUniform

FloatShaderUniform::FloatShaderUniform(Shader* shader, const std::string& name, float value) :
    value(value), ShaderUniform(shader, name, SHADER_UNIFORM_FLOAT) {
    type_name = "float";
}

void FloatShaderUniform::setValue(float value) {
    this->value = value;
    modified = true;
}

float FloatShaderUniform::getValue() const {
    return value;
}

void FloatShaderUniform::write(std::string& content) const {

    char buff[256];

    if(bake) {
        snprintf(buff, 256, "#define %s %.5f\n", name.c_str(), value);
    } else {
        snprintf(buff, 256, "uniform %s %s;\n", type_name.c_str(), name.c_str());
    }

    content += buff;
}

//IntShaderUniform

IntShaderUniform::IntShaderUniform(Shader* shader, const std::string& name, int value) :
    value(value), ShaderUniform(shader, name, SHADER_UNIFORM_INT) {
    type_name = "int";
}

void IntShaderUniform::setValue(float value) {
    this->value = value;
    modified = true;
}

float IntShaderUniform::getValue() const {
    return value;
}

void IntShaderUniform::write(std::string& content) const {

    char buff[256];

    if(bake) {
        snprintf(buff, 256, "#define %s %d\n", name.c_str(), value);
    } else {
        snprintf(buff, 256, "uniform %s %s;\n", type_name.c_str(), name.c_str());
    }

    content += buff;
}


//Vec2ShaderUniform

Vec2ShaderUniform::Vec2ShaderUniform(Shader* shader, const std::string& name, const vec2& value) :
    value(value), ShaderUniform(shader, name, SHADER_UNIFORM_VEC2) {
    type_name = "vec2";
}

void Vec2ShaderUniform::setValue(const vec2& value) {
    this->value = value;
    modified = true;
}

const vec2& Vec2ShaderUniform::getValue() const {
    return value;
}

void Vec2ShaderUniform::write(std::string& content) const {

    char buff[256];

    if(bake) {
        snprintf(buff, 256, "#define %s vec2(%.5f, %.5f)\n", name.c_str(), value.x, value.y);
    } else {
        snprintf(buff, 256, "uniform %s %s;\n", type_name.c_str(), name.c_str());
    }

    content += buff;
}

//Vec3ShaderUniform

Vec3ShaderUniform::Vec3ShaderUniform(Shader* shader, const std::string& name, const vec3& value) :
    value(value), ShaderUniform(shader, name, SHADER_UNIFORM_VEC3) {
    type_name = "vec3";
}


void Vec3ShaderUniform::setValue(const vec3& value) {
    this->value = value;
    modified = true;
}

const vec3& Vec3ShaderUniform::getValue() const {
    return value;
}

void Vec3ShaderUniform::write(std::string& content) const {

    char buff[256];

    if(bake) {
        snprintf(buff, 256, "#define %s vec3(%.5f, %.5f, %.5f)\n", name.c_str(), value.x, value.y, value.z);
    } else {
        snprintf(buff, 256, "uniform %s %s;\n", type_name.c_str(), name.c_str());
    }

    content += buff;
}

//Vec4ShaderUniform

Vec4ShaderUniform::Vec4ShaderUniform(Shader* shader, const std::string& name, const vec4& value) :
    value(value), ShaderUniform(shader, name, SHADER_UNIFORM_VEC4) {
    type_name = "vec4";
}

void Vec4ShaderUniform::setValue(const vec4& value) {
    this->value = value;
    modified = true;
}

const vec4& Vec4ShaderUniform::getValue() const {
    return value;
}

void Vec4ShaderUniform::write(std::string& content) const {

    char buff[256];

    if(bake) {
        snprintf(buff, 256, "#define %s vec4(%.5f, %.5f, %.5f, %.5f)\n", name.c_str(), value.x, value.y, value.z, value.w);
    } else {
        snprintf(buff, 256, "uniform %s %s;\n", type_name.c_str(), name.c_str());
    }

    content += buff;
}

//Mat3ShaderUniform

Mat3ShaderUniform::Mat3ShaderUniform(Shader* shader, const std::string& name, const mat3& value) :
    value(value), ShaderUniform(shader, name, SHADER_UNIFORM_MAT3) {
    type_name = "mat3";
}

void Mat3ShaderUniform::setValue(const mat3& value) {
    this->value = value;
    modified = true;
}

const mat3& Mat3ShaderUniform::getValue() const {
    return value;
}

void Mat3ShaderUniform::write(std::string& content) const {

    char buff[256];

    if(bake) {
        snprintf(buff, 256, "#define %s mat3(%.5f, %.5f, %.5f, %.5f, %.5f, %.5f, %.5f, %.5f, %.5f)\n",
                value[0][0], value[1][0], value[2][0].
                value[0][1], value[1][1], value[2][1],
                value[0][2], value[1][2], value[2][2]);
    } else {
        snprintf(buff, 256, "uniform %s %s;\n", type_name.c_str(), name.c_str());
    }

    content += buff;
}

//Mat4ShaderUniform

Mat4ShaderUniform::Mat4ShaderUniform(Shader* shader, const std::string& name, const mat4& value) :
    value(value), ShaderUniform(shader, name, SHADER_UNIFORM_MAT4) {
    type_name = "mat4";
}

void Mat4ShaderUniform::setValue(const mat4& value) {
    this->value = value;
    modified = true;
}

const mat4& Mat4ShaderUniform::getValue() const {
    return value;
}

void Mat4ShaderUniform::write(std::string& content) const {

    char buff[256];

    if(bake) {
        snprintf(buff, 256, "#define %s mat4(%.5f, %.5f, %.5f, %.5f, %.5f, %.5f, %.5f, %.5f, %.5f, %5f, %.5f, %.5f)\n",
                value[0][0], value[1][0], value[2][0], value[3][0],
                value[0][1], value[1][1], value[2][1], value[3][1],
                value[0][2], value[1][2], value[2][2], value[3][2]);
    } else {
        snprintf(buff, 256, "uniform %s %s;\n", type_name.c_str(), name.c_str());
    }

    content += buff;
}

//Shader
Shader::Shader(const std::string& prefix) : Resource(prefix) {

    std::string shader_dir = shadermanager.getDir();

    std::string vertex_file   = shader_dir + prefix + std::string(".vert");
    std::string fragment_file = shader_dir + prefix + std::string(".frag");

    VertexShader* vertex_shader = new VertexShader(this);
    vertex_shader->includeFile(vertex_file);
    parts[GL_VERTEX_SHADER]   = vertex_shader;

    FragmentShader* fragment_shader = new FragmentShader(this);
    fragment_shader->includeFile(fragment_file);
    parts[GL_FRAGMENT_SHADER] = fragment_shader;

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

//ShaderPart

ShaderPart::ShaderPart(Shader* shader, GLint part_type, const std::string& part_desc) : shader(shader), part_type(part_type), part_desc(part_desc) {
    shader_part = glCreateShader(part_type);
}

ShaderPart::~ShaderPart() {
    glDeleteShader(shader_part);
}

void ShaderPart::checkError() {

    GLint compile_success;
    glGetShaderiv(shader_part, GL_COMPILE_STATUS, &compile_success);

    GLint info_log_length;
    glGetShaderiv(shader_part, GL_INFO_LOG_LENGTH, &info_log_length);

    const char* resource_desc = !shader->resource_name.empty() ? shader->resource_name.c_str() : "???";

    if(info_log_length > 1) {
        char info_log[info_log_length];

        glGetShaderInfoLog(shader_part, info_log_length, &info_log_length, info_log);

        if(!compile_success) {
            throw SDLAppException("%s shader '%s' failed to compile:\n%s",
                                  part_desc.c_str(),
                                  resource_desc,
                                  info_log);
        }

        if(shadermanager.warnings) {
            fprintf(stderr, "%s shader '%s':\n%s",
                            part_desc.c_str(),
                            resource_desc,
                            info_log);
        }

        return;
    }

    if(!compile_success) {
        throw SDLAppException("%s shader '%s' failed to compile",
                              part_desc.c_str(),
                              resource_desc);
    }
}

void ShaderPart::compile() {

    if(source.empty()) return;

    const char* source_ptr = source.c_str();
    int source_len = source.size();

    glShaderSource(shader_part, 1, (const GLchar**) &source_ptr, &source_len);
    glCompileShader(shader_part);

    checkError();
}

bool ShaderPart::preprocess(const std::string& line) {

    std::vector<std::string> matches;

    if(Shader_pre_include.match(line, &matches)) {

        std::string include_file = shadermanager.getDir() + matches[0];

        includeFile(include_file);

        return true;
    }

    return false;
}

void ShaderPart::includeFile(const std::string& filename) {

    // get length
    std::ifstream in(filename.c_str());

    if(!in.is_open()) {
        throw SDLAppException("could not open '%s'", filename.c_str());
    }

    std::string line;
    while( std::getline(in,line) ) {
        if(!preprocess(shaderType, line)) {
            source += line;
            source += "\n";
        }
    }

    in.close();
}

VertexShader::VertexShader() : ShaderPart(GL_VERTEX_SHADER, "vertex") {
}

FragmentShader::FragmentShader() : ShaderPart(GL_FRAGMENT_SHADER, "fragment") {
}

GeometryShader::GeometryShader() : ShaderPart(GL_GEOMETRY_SHADER_EXT, "geometry") {
}

void Shader::includeSource(GLenum shaderType, const std::string& string) {

    std::stringstream in(string);

    std::string& output = source_code[shaderType];

    std::string line;
    while( std::getline(in,line) ) {
        if(!preprocess(shaderType, line)) {
            output += line;
            output += "\n";
        }
    }
}

ShaderPart* Shader::getShaderPart(GLenum part_type) {

    std::map<std::string, ShaderPart*>::iterator it = parts.find(part_type);

    if(it != uniforms.end()) {
        return it->second;
    }

    return 0;
}

void Shader::use() {
    glUseProgram(program);
}

GLenum Shader::getProgram() {
    return program;
}

ShaderUniform* Shader::getUniform(const std::string& name) {
    std::map<std::string, ShaderUniform*>::iterator it = uniforms.find(name);

    if(it != uniforms.end()) {
        return it->second;
    }
    return 0;
}

void Shader::setFloat(const std::string& name, float value) {
    ShaderUniform* uniform = getUniform(name);

    if(!uniform) return;

    glUniform1f(uniform->getLocation(), value);
}

void Shader::setVec2 (const std::string& name, const vec2& value) {
    ShaderUniform* uniform = getUniform(name);

    if(!uniform) return;

    glUniform2fv(uniform->getLocation(), 1, glm::value_ptr(value));
}

void Shader::setVec3 (const std::string& name, const vec3& value) {
    ShaderUniform* uniform = getUniform(name);

    if(!uniform) return;

    glUniform3fv(uniform->getLocation(), 1, glm::value_ptr(value));
}

void Shader::setVec4 (const std::string& name, const vec4& value) {
    ShaderUniform* uniform = getUniform(name);

    if(!uniform) return;

    glUniform4fv(uniform->getLocation(), 1, glm::value_ptr(value));
}

void Shader::setMat3 (const std::string& name, const mat3& value) {
    ShaderUniform* uniform = getUniform(name);

    if(!uniform) return;

    glUniformMatrix3fv(uniform->getLocation(), 1, 0, glm::value_ptr(value));
}

void Shader::setMat4 (const std::string& name, const mat4& value) {
    ShaderUniform* uniform = getUniform(name);

    if(!uniform) return;

    glUniformMatrix4fv(uniform->getLocation(), 1, 0, glm::value_ptr(value));
}

void Shader::setInteger (const std::string& name, int value) {
    ShaderUniform* uniform = getUniform(name);

    if(!uniform) return;

    glUniform1i(uniform->getLocation(), value);
}

//
void Shader::bake() {
    foreach(ShaderUniform* s, uniforms) {
        s->setBaked(true);
    }

    recompile();
}

