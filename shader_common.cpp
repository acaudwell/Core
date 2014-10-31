#include "shader_common.h"
#include "logger.h"

#include <boost/format.hpp>
#include <stdarg.h>
#include <fstream>
#include <sstream>

std::string gSDLAppShaderDir;

Regex Shader_pre_version("^\\s*#version\\s*(\\d+)\\s*");
Regex Shader_pre_extension("^\\s*#extension\\s*([a-zA-Z0-9_]+)\\s+:\\s+(enable|require|warn|disable)\\s*$");
Regex Shader_pre_include("^\\s*#include\\s*\"([^\"]+)\"");
Regex Shader_uniform_def("^\\s*uniform\\s+(\\w+)\\s+(\\w+)(?:\\[(\\d+)\\])?\\s*;\\s*(?://\\s*(.+))?$");
Regex Shader_error_line("\\b\\d*\\((\\d+)\\) : error ");
Regex Shader_error2_line("\\bERROR: \\d+:(\\d+):");
Regex Shader_error3_line("^\\d+:(\\d+)\\(\\d+\\): error");
Regex Shader_warning_line("\\b\\d*\\((\\d+)\\) : warning ");
Regex Shader_redefine_line(" (?:defined|declaration) at \\d*\\((\\d+)\\)");

//ShaderException

ShaderException::ShaderException(const std::string& message)
    : message(message) {
}

ShaderException::ShaderException(const std::string& message, const std::string& source)
    : message(message), source(source) {
}

const std::string& ShaderException::getSource() const {
    return source;
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
        throw ShaderException(str(boost::format("could not open '%s'") % filename));
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
    substitutions.clear();
}

void ShaderPart::setSource(const std::string& source) {
    raw_source = source;
}

void ShaderPart::applySubstitution(std::string& source, const std::string& name, const std::string& value) {

    std::string::size_type next_match;

    for(next_match = source.find(name);
        next_match != std::string::npos;
        next_match = source.find(name, next_match)) {
        source.replace(next_match, name.length(), value);
        next_match += value.length();
    }
}

void ShaderPart::substitute(const std::string& name, const char *value, ...) {

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
}

void ShaderPart::substitute(const std::string& name, const std::string& value) {
    substitute(name, value.c_str());
}

void ShaderPart::applySubstitutions(std::string& source) {

    for(std::map<std::string, std::string>::iterator it = substitutions.begin(); it != substitutions.end(); it++) {
        applySubstitution(source, it->first, it->second);
    }
}

void ShaderPart::applyDefines(std::string& source) {

    for(std::map<std::string, std::string>::iterator it = defines.begin(); it != defines.end(); it++) {
        source.append(str(boost::format("#define %s %s\n") % it->first % it->second));
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

void ShaderPart::preprocess() {

    processed_source.clear();

    // add defines at the top of the source

    applyDefines(processed_source);

    // do trivial substitutions

    std::vector<std::string> matches;

    std::stringstream in(raw_source);

    std::string line;
    bool skipdef = false;

    while( std::getline(in,line) ) {
        applySubstitutions(line);

        processed_source.append(line);
        processed_source.append("\n");
    }
}

const std::string& ShaderPart::getSource() {
    if(processed_source.empty()) preprocess();
    return processed_source;
}

//ShaderUniform

ShaderUniform::ShaderUniform(AbstractShader* shader, const std::string& name, int uniform_type, const std::string& type_name)
    : shader(shader), name(name), location(-1), initialized(false), modified(false), baked(false), uniform_type(uniform_type), type_name(type_name) {
}

void ShaderUniform::unload() {
    location = -1;
}

int ShaderUniform::getLocation() {

    if(location != -1) return location;

    location = shader->getUniformLocation( name.c_str() );

    return location;
}

const std::string& ShaderUniform::getName() const {
    return name;
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

FloatShaderUniform::FloatShaderUniform(AbstractShader* shader, const std::string& name, float value) :
    value(value), ShaderUniform(shader, name, SHADER_UNIFORM_FLOAT, "float") {
}

void FloatShaderUniform::setValue(float value) {
    if(baked && this->value == value) return;

    this->value = value;
    modified = true;
    initialized = true;
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

IntShaderUniform::IntShaderUniform(AbstractShader* shader, const std::string& name, int value) :
    value(value), ShaderUniform(shader, name, SHADER_UNIFORM_INT, "int") {
}

void IntShaderUniform::setValue(int value) {
    if(baked && this->value == value) return;

    this->value = value;
    modified = true;
    initialized = true;
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

BoolShaderUniform::BoolShaderUniform(AbstractShader* shader, const std::string& name, bool value) :
    value(value), ShaderUniform(shader, name, SHADER_UNIFORM_BOOL, "bool") {
}

void BoolShaderUniform::setValue(bool value) {
    if(baked && this->value == value) return;

    this->value = value;
    modified = true;
    initialized = true;
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

Sampler1DShaderUniform::Sampler1DShaderUniform(AbstractShader* shader, const std::string& name, int value) :
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

int& Sampler1DShaderUniform::getValue() {
    return value;
}

//Sampler2DShaderUniform

Sampler2DShaderUniform::Sampler2DShaderUniform(AbstractShader* shader, const std::string& name, int value) :
    value(value), ShaderUniform(shader, name, SHADER_UNIFORM_SAMPLER_2D, "sampler2D") {
}

void Sampler2DShaderUniform::setValue(int value) {
    if(baked && this->value == value) return;

    this->value = value;
    modified = true;
    initialized = true;
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

Vec2ShaderUniform::Vec2ShaderUniform(AbstractShader* shader, const std::string& name, const vec2& value) :
    value(value), ShaderUniform(shader, name, SHADER_UNIFORM_VEC2, "vec2") {
}

void Vec2ShaderUniform::setValue(const vec2& value) {
    if(baked && this->value == value) return;

    this->value = value;
    modified = true;
    initialized = true;
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

Vec3ShaderUniform::Vec3ShaderUniform(AbstractShader* shader, const std::string& name, const vec3& value) :
    value(value), ShaderUniform(shader, name, SHADER_UNIFORM_VEC3, "vec3") {
}


void Vec3ShaderUniform::setValue(const vec3& value) {
    if(baked && this->value == value) return;

    this->value = value;
    modified = true;
    initialized = true;
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

Vec4ShaderUniform::Vec4ShaderUniform(AbstractShader* shader, const std::string& name, const vec4& value) :
    value(value), ShaderUniform(shader, name, SHADER_UNIFORM_VEC4, "vec4") {
}

void Vec4ShaderUniform::setValue(const vec4& value) {
    if(baked && this->value == value) return;

    this->value = value;
    modified = true;
    initialized = true;
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

Mat3ShaderUniform::Mat3ShaderUniform(AbstractShader* shader, const std::string& name, const mat3& value) :
    value(value), ShaderUniform(shader, name, SHADER_UNIFORM_MAT3, "mat3") {
}

void Mat3ShaderUniform::setValue(const mat3& value) {
    if(baked && this->value == value) return;

    this->value = value;
    modified = true;
    initialized = true;
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

Mat4ShaderUniform::Mat4ShaderUniform(AbstractShader* shader, const std::string& name, const mat4& value) :
    value(value), ShaderUniform(shader, name, SHADER_UNIFORM_MAT4, "mat4") {
}

void Mat4ShaderUniform::setValue(const mat4& value) {
    if(baked && this->value == value) return;

    this->value = value;
    modified = true;
    initialized = true;
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

Vec2ArrayShaderUniform::Vec2ArrayShaderUniform(AbstractShader* shader, const std::string& name, size_t length, const vec2* value) :
    length(length), ShaderUniform(shader, name, SHADER_UNIFORM_VEC2_ARRAY, "vec2") {
    this->value.resize(length);
    if(value != 0) copyValue(value);
}

Vec2ArrayShaderUniform::~Vec2ArrayShaderUniform() {
}

const std::vector<vec2>& Vec2ArrayShaderUniform::getValue() {
    return value;
}

size_t Vec2ArrayShaderUniform::getLength() const {
    return length;
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

void Vec2ArrayShaderUniform::write(std::string& content) const {

    char buff[1024];

    if(baked) {
        content.append(str(boost::format("%s[%d] %s = %s[] (\n") % type_name % length % name % type_name));

        content += buff;

        for(size_t i=0; i<length; i++) {
            content.append(str(boost::format("    %s(%e, %e)") % type_name % value[i].x % value[i].y));

            if(i<length-1) content += ",\n";
            else           content += "\n);\n";
        }

    } else {
        content.append(str(boost::format("uniform %s %s[%d];\n") % type_name % name % length));
    }
}

//Vec3ArrayShaderUniform

Vec3ArrayShaderUniform::Vec3ArrayShaderUniform(AbstractShader* shader, const std::string& name, size_t length, const vec3* value) :
    length(length), ShaderUniform(shader, name, SHADER_UNIFORM_VEC3_ARRAY, "vec3") {
    this->value.resize(length);
    if(value != 0) copyValue(value);
}

Vec3ArrayShaderUniform::~Vec3ArrayShaderUniform() {
}

const std::vector<vec3>& Vec3ArrayShaderUniform::getValue() {
    return value;
}

size_t Vec3ArrayShaderUniform::getLength() const {
    return length;
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

void Vec3ArrayShaderUniform::write(std::string& content) const {

    char buff[1024];

    if(baked) {
        content.append(str(boost::format("%s[%d] %s = %s[] (\n") % type_name % length % name % type_name));

        for(size_t i=0; i<length; i++) {
            content.append(str(boost::format("    %s(%e, %e, %e)") % type_name % value[i].x % value[i].y % value[i].z));

            if(i<length-1) content += ",\n";
            else           content += "\n);\n";
        }

    } else {
        content.append(str(boost::format("uniform %s %s[%d];\n") % type_name % name % length));
    }
}

//Vec4ArrayShaderUniform

Vec4ArrayShaderUniform::Vec4ArrayShaderUniform(AbstractShader* shader, const std::string& name, size_t length, const vec4* value) :
    length(length), ShaderUniform(shader, name, SHADER_UNIFORM_VEC4_ARRAY, "vec4") {
    this->value.resize(length);
    if(value != 0) copyValue(value);
}

Vec4ArrayShaderUniform::~Vec4ArrayShaderUniform() {
}

const std::vector<vec4>& Vec4ArrayShaderUniform::getValue() {
    return value;
}

size_t Vec4ArrayShaderUniform::getLength() const {
    return length;
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

void Vec4ArrayShaderUniform::write(std::string& content) const {

    char buff[1024];

    if(baked) {
        content.append(str(boost::format("%s[%d] %s = %s[] (\n") % type_name % length % name % type_name));

        for(size_t i=0; i<length; i++) {
            content.append(str(boost::format("    %s(%e, %e, %e, %e)") % type_name % value[i].x % value[i].y % value[i].z % value[i].w));

            if(i<length-1) content += ",\n";
            else           content += "\n);\n";
        }

    } else {
        content.append(str(boost::format("uniform %s %s[%d];\n") % type_name % name % length));
    }

}

// AbstractShaderPass

AbstractShaderPass::AbstractShaderPass(AbstractShader* parent, int shader_object_type, const std::string& shader_object_desc)
    : parent(parent), shader_object_type(shader_object_type), shader_object_desc(shader_object_desc) {
    shader_object = 0;
    version = 0;
}

void AbstractShaderPass::showContext(std::string& context, int line_no, int amount) {

    std::stringstream in(shader_object_source);

    int i = 1;
    char line_detail[1024];
    std::string line;

    while( std::getline(in,line) ) {

        if(i==line_no || (i<line_no && i+amount>=line_no) || (i>line_no && i-amount<=line_no)) {
            snprintf(line_detail, 1024, "%s%4d | %s\n", (i==line_no ? "-> ": "   "), i, line.c_str());
            context += line_detail;
        }
        i++;
    }
}

bool AbstractShaderPass::errorContext(const char* log_message, std::string& context) {

    std::vector<std::string> matches;

    if(   !Shader_error_line.match(log_message, &matches)
       && !Shader_error2_line.match(log_message, &matches)
       && !Shader_error3_line.match(log_message, &matches)
       && !(Logger::getDefault()->getLevel() == LOG_LEVEL_WARN && Shader_warning_line.match(log_message, &matches)))
        return false;

    int line_no = atoi(matches[0].c_str());

    if(Shader_redefine_line.match(log_message, &matches)) {
        int redefine_line_no = atoi(matches[0].c_str());
        showContext(context, redefine_line_no, 3);
        context += "\n";
    }

    showContext(context, line_no, 3);

    return true;
}

void AbstractShaderPass::toString(std::string& out) {
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


const std::string& AbstractShaderPass::getObjectSource() {
    return shader_object_source;
}

bool AbstractShaderPass::isEmpty() {
    return source.empty();
}

std::list<ShaderUniform*>& AbstractShaderPass::getUniforms() {
    return uniforms;
}

//add uniform, unless parent Shader has this in which case link to it
ShaderUniform* AbstractShaderPass::addArrayUniform(const std::string& name, const std::string& type, size_t length) {

    ShaderUniform* uniform = 0;

    if((uniform = parent->getUniform(name)) == 0) {

        if(type == "vec2") {
            uniform = new Vec2ArrayShaderUniform(parent, name, length);
        } else if(type == "vec3") {
            uniform = new Vec3ArrayShaderUniform(parent, name, length);
        } else if(type == "vec4") {
            uniform = new Vec4ArrayShaderUniform(parent, name, length);
        } else {
            throw ShaderException(str(boost::format("shader uniform arrays for type '%s' not implemented") % type));
        }

        uniform->setInitialized(false);

        parent->addUniform(uniform);
    }

    uniforms.push_back(uniform);

    return uniform;
}

ShaderUniform* AbstractShaderPass::addUniform(const std::string& name, const std::string& type) {

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
            throw ShaderException(str(boost::format("unsupported shader uniform type '%s'") % type));
        }

        uniform->setInitialized(false);

        parent->addUniform(uniform);
    }

    uniforms.push_back(uniform);

    return uniform;
}

bool AbstractShaderPass::preprocess(const std::string& line) {

    std::vector<std::string> matches;

    if(Shader_pre_version.match(line, &matches)) {
        version = atoi(matches[0].c_str());
        return true;
    }

    if(Shader_pre_extension.match(line, &matches)) {
        extensions[matches[0]] = matches[1];
        return true;
    }

    if(Shader_pre_include.match(line, &matches)) {
        std::string include_file = gSDLAppShaderDir + matches[0];
        includeFile(include_file);

        return true;
    }

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

void AbstractShaderPass::includeFile(const std::string& filename) {

    // get length
    std::ifstream in(filename.c_str());

    if(!in.is_open()) {
        throw ShaderException(str(boost::format("could not open '%s'") % filename));
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

void AbstractShaderPass::includeSource(const std::string& string) {

    std::stringstream in(string);

    std::string line;
    while( std::getline(in,line) ) {
        if(!preprocess(line)) {
            source += line;
            source += "\n";
        }
    }
}

// AbstractShader

AbstractShader::AbstractShader(const std::string& prefix)
    : prefix(prefix), Resource(prefix) {
    setDefaults();
}

AbstractShader::AbstractShader() {
    setDefaults();
}

void AbstractShader::setDynamicCompile(bool dynamic_compile) {
    this->dynamic_compile = dynamic_compile;
}

void AbstractShader::setDefaults() {
    vertex_shader   = 0;
    fragment_shader = 0;
    geometry_shader = 0;
    program = 0;
    dynamic_compile = false;
}

void AbstractShader::clear() {
    unload();

    for(std::map<std::string, ShaderUniform*>::iterator it= uniforms.begin(); it!=uniforms.end();it++) {
        delete it->second;
    }
    uniforms.clear();
    uniform_list.clear();

    if(vertex_shader != 0)   delete vertex_shader;
    if(geometry_shader != 0) delete geometry_shader;
    if(fragment_shader != 0) delete fragment_shader;

    vertex_shader   = 0;
    geometry_shader = 0;
    fragment_shader = 0;
}

bool AbstractShader::isEmpty() {

    if(   (!vertex_shader   || vertex_shader->isEmpty())
       && (!fragment_shader || fragment_shader->isEmpty())
       && (!geometry_shader || geometry_shader->isEmpty())) {
        return true;
    }

    return false;
}

void AbstractShader::reload(bool force) {
    if(isEmpty()) return;

    if(force && !prefix.empty()) {
        loadPrefix();
    } else {
        load();
    }
}

void AbstractShader::use() {

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

unsigned int AbstractShader::getProgram() {
    return program;
}

void AbstractShader::addUniform(ShaderUniform* uniform) {

    if(getUniform(uniform->getName()) != 0) {
        throw ShaderException(str(boost::format("shader already has a uniform named '%s'") % uniform->getName()));
    }

    uniforms[uniform->getName()] = uniform;
    uniform_list.push_back(uniform);
}

ShaderUniform* AbstractShader::getUniform(const std::string& name) {
    std::map<std::string, ShaderUniform*>::iterator it = uniforms.find(name);

    if(it != uniforms.end()) {
        return it->second;
    }
    return 0;
}

void AbstractShader::includeSource(unsigned int shader_object_type, const std::string& source) {

    AbstractShaderPass* pass = grabShaderPass(shader_object_type);

    pass->includeSource(source);
}

void AbstractShader::includeFile(unsigned int shader_object_type, const std::string& filename) {

    AbstractShaderPass* pass = grabShaderPass(shader_object_type);

    pass->includeFile(filename);
}

void AbstractShader::addSubstitute(const std::string& name, const std::string& value) {
    substitutions[name] = value;
}

void AbstractShader::addSubstitute(const std::string& name, const char *value, ...) {

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

void AbstractShader::substitute(std::string& source, const std::string& name, const std::string& value) {

    std::string::size_type next_match;

    for(next_match = source.find(name);
        next_match != std::string::npos;
        next_match = source.find(name, next_match)) {
        source.replace(next_match, name.length(), value);
        next_match += value.length();
    }
}

void AbstractShader::applySubstitutions(std::string& source) {

    for(std::map<std::string, std::string>::iterator it = substitutions.begin(); it != substitutions.end(); it++) {
        substitute(source, it->first, it->second);
    }
}

void AbstractShader::setBool (const std::string& name, bool value) {
    ShaderUniform* uniform = getUniform(name);

    if(!uniform || uniform->getType() != SHADER_UNIFORM_BOOL) return;

    ((BoolShaderUniform*)uniform)->setValue(value);
}

void AbstractShader::setInteger (const std::string& name, int value) {
    ShaderUniform* uniform = getUniform(name);

    if(!uniform || uniform->getType() != SHADER_UNIFORM_INT) return;

    ((IntShaderUniform*)uniform)->setValue(value);
}

void AbstractShader::setSampler1D (const std::string& name, int value) {
    ShaderUniform* uniform = getUniform(name);

    if(!uniform || uniform->getType() != SHADER_UNIFORM_SAMPLER_1D) return;

    ((Sampler1DShaderUniform*)uniform)->setValue(value);
}

void AbstractShader::setSampler2D (const std::string& name, int value) {
    ShaderUniform* uniform = getUniform(name);

    if(!uniform || uniform->getType() != SHADER_UNIFORM_SAMPLER_2D) return;

    ((Sampler2DShaderUniform*)uniform)->setValue(value);
}

void AbstractShader::setFloat(const std::string& name, float value) {
    ShaderUniform* uniform = getUniform(name);

    if(!uniform || uniform->getType() != SHADER_UNIFORM_FLOAT) return;

    ((FloatShaderUniform*)uniform)->setValue(value);
}

void AbstractShader::setVec2 (const std::string& name, const vec2& value) {
    ShaderUniform* uniform = getUniform(name);

    if(!uniform || uniform->getType() != SHADER_UNIFORM_VEC2) return;

    ((Vec2ShaderUniform*)uniform)->setValue(value);
}

void AbstractShader::setVec3 (const std::string& name, const vec3& value) {
    ShaderUniform* uniform = getUniform(name);

    if(!uniform || uniform->getType() != SHADER_UNIFORM_VEC3) return;

    ((Vec3ShaderUniform*)uniform)->setValue(value);
}

void AbstractShader::setVec2Array (const std::string& name, vec2* value) {
    ShaderUniform* uniform = getUniform(name);

    if(!uniform || uniform->getType() != SHADER_UNIFORM_VEC2_ARRAY) return;

    ((Vec2ArrayShaderUniform*)uniform)->setValue(value);
}

void AbstractShader::setVec2Array (const std::string& name, std::vector<vec2>& value) {
    ShaderUniform* uniform = getUniform(name);

    if(!uniform || uniform->getType() != SHADER_UNIFORM_VEC2_ARRAY) return;

    ((Vec2ArrayShaderUniform*)uniform)->setValue(value);
}

void AbstractShader::setVec3Array (const std::string& name, vec3* value) {
    ShaderUniform* uniform = getUniform(name);

    if(!uniform || uniform->getType() != SHADER_UNIFORM_VEC3_ARRAY) return;

    ((Vec3ArrayShaderUniform*)uniform)->setValue(value);
}

void AbstractShader::setVec3Array (const std::string& name, std::vector<vec3>& value) {
    ShaderUniform* uniform = getUniform(name);

    if(!uniform || uniform->getType() != SHADER_UNIFORM_VEC3_ARRAY) return;

    ((Vec3ArrayShaderUniform*)uniform)->setValue(value);
}

void AbstractShader::setVec4Array (const std::string& name, vec4* value) {
    ShaderUniform* uniform = getUniform(name);

    if(!uniform || uniform->getType() != SHADER_UNIFORM_VEC4_ARRAY) return;

    ((Vec4ArrayShaderUniform*)uniform)->setValue(value);
}

void AbstractShader::setVec4Array (const std::string& name, std::vector<vec4>& value) {
    ShaderUniform* uniform = getUniform(name);

    if(!uniform || uniform->getType() != SHADER_UNIFORM_VEC4_ARRAY) return;

    ((Vec4ArrayShaderUniform*)uniform)->setValue(value);
}

void AbstractShader::setVec4 (const std::string& name, const vec4& value) {
    ShaderUniform* uniform = getUniform(name);

    if(!uniform || uniform->getType() != SHADER_UNIFORM_VEC4) return;

    ((Vec4ShaderUniform*)uniform)->setValue(value);
}

void AbstractShader::setMat3 (const std::string& name, const mat3& value) {
    ShaderUniform* uniform = getUniform(name);

    if(!uniform || uniform->getType() != SHADER_UNIFORM_MAT3) return;

    ((Mat3ShaderUniform*)uniform)->setValue(value);
}

void AbstractShader::setMat4 (const std::string& name, const mat4& value) {
    ShaderUniform* uniform = getUniform(name);

    if(!uniform || uniform->getType() != SHADER_UNIFORM_MAT4) return;

    ((Mat4ShaderUniform*)uniform)->setValue(value);
}

void AbstractShader::setBaked(const std::string& name, bool baked) {
    ShaderUniform* uniform = getUniform(name);

    if(!uniform) return;

    uniform->setBaked(baked);
}

void AbstractShader::setBakedUniforms(bool baked) {
    for(std::map<std::string, ShaderUniform*>::iterator it= uniforms.begin(); it!=uniforms.end();it++) {
        it->second->setBaked(baked);
    }
}


const std::list<ShaderUniform*>& AbstractShader::getUniforms() {
    return uniform_list;
}

void AbstractShader::applyUniforms() {
    for(std::map<std::string, ShaderUniform*>::iterator it= uniforms.begin(); it!=uniforms.end();it++) {
        ShaderUniform* u = it->second;
        if(!u->isBaked()) applyUniform(u);
    }
}

bool AbstractShader::needsCompile() {

    for(std::map<std::string, ShaderUniform*>::iterator it= uniforms.begin(); it!=uniforms.end();it++) {
        ShaderUniform* u = it->second;

        if(u->isBaked() && u->isModified()) {
            //infoLog("baked uniform %s needs update", u->getName().c_str());
            return true;
        }
    }

    return false;
}
