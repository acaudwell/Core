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
#include "util.h"

#include <boost/format.hpp>
#include <list>
#include <map>
#include <string>
#include <fstream>
#include <sstream>

enum { SHADER_UNIFORM_FLOAT,
       SHADER_UNIFORM_BOOL,
       SHADER_UNIFORM_SAMPLER_1D,
       SHADER_UNIFORM_SAMPLER_2D,
       SHADER_UNIFORM_INT,
       SHADER_UNIFORM_VEC2,
       SHADER_UNIFORM_VEC3,
       SHADER_UNIFORM_VEC4,
       SHADER_UNIFORM_MAT3,
       SHADER_UNIFORM_MAT4,
       SHADER_UNIFORM_VEC2_ARRAY,
       SHADER_UNIFORM_VEC3_ARRAY,
       SHADER_UNIFORM_VEC4_ARRAY
};

class Shader;
class ShaderPass;

class ShaderUniform {
protected:
    std::string name;
    GLint location;
    Shader* shader;
    int uniform_type;
    std::string type_name;
    bool modified;
    bool initialized;
    bool baked;
public:

    ShaderUniform(Shader* shader, const std::string& name, int uniform_type, const std::string& type_name);

    virtual void unload();

    int getType() { return uniform_type; };

    virtual void write(std::string& content) const {};

    const std::string& getName() const;
    bool  isInitialized() const { return initialized; };
    GLint getLocation();

    bool isBaked() const    { return baked; };
    bool isModified() const { return modified; };

    void setInitialized(bool initialized) { this->initialized = initialized; };

    virtual void apply() {};

    virtual void setBaked(bool baked);
    virtual void setModified(bool modified) { this->modified = modified; };
};

class FloatShaderUniform : public ShaderUniform {
    float value;
public:
    FloatShaderUniform(Shader* shader, const std::string& name, float value = 0.0f);

    void write(std::string& content) const;

    void apply();
    void setValue(float value);
    float getValue() const;
};

class IntShaderUniform : public ShaderUniform {
    int value;
public:
    IntShaderUniform(Shader* shader, const std::string& name, int value = 0);

    void write(std::string& content) const;

    void apply();
    void setValue(int value);
    float getValue() const;
};

class BoolShaderUniform : public ShaderUniform {
    bool value;
public:
    BoolShaderUniform(Shader* shader, const std::string& name, bool value = false);

    void write(std::string& content) const;

    void apply();
    void setValue(bool value);
    float getValue() const;
};

class Sampler1DShaderUniform : public ShaderUniform {
    int value;
public:
    Sampler1DShaderUniform(Shader* shader, const std::string& name, int value = 0);

    void write(std::string& content) const;

    void setBaked(bool baked);

    void apply();
    void setValue(int value);
    float getValue() const;
};

class Sampler2DShaderUniform : public ShaderUniform {
    int value;
public:
    Sampler2DShaderUniform(Shader* shader, const std::string& name, int value = 0);

    void write(std::string& content) const;

    void setBaked(bool baked);

    void apply();
    void setValue(int value);
    float getValue() const;
};

class Vec2ShaderUniform : public ShaderUniform {
    vec2 value;
public:
    Vec2ShaderUniform(Shader* shader, const std::string& name, const vec2& value = vec2(0.0f)) ;

    void write(std::string& content) const;

    void apply();
    void setValue(const vec2& value);
    const vec2& getValue() const;
};

class Vec3ShaderUniform : public ShaderUniform {
    vec3 value;
public:
    Vec3ShaderUniform(Shader* shader, const std::string& name, const vec3& value = vec3(0.0f));

    void write(std::string& content) const;

    void apply();
    void setValue(const vec3& value);
    const vec3& getValue() const;
};

class Vec4ShaderUniform : public ShaderUniform {
    vec4 value;
public:
    Vec4ShaderUniform(Shader* shader, const std::string& name, const vec4& value = vec4(0.0f));

    void write(std::string& content) const;

    void apply();
    void setValue(const vec4& value);
    const vec4& getValue() const;
};

class Mat3ShaderUniform : public ShaderUniform {
    mat3 value;
public:
    Mat3ShaderUniform(Shader* shader, const std::string& name, const mat3& value = mat3(1.0f));

    void write(std::string& content) const;

    void apply();
    void setValue(const mat3& value);
    const mat3& getValue() const;
};

class Mat4ShaderUniform : public ShaderUniform {
    mat4 value;
public:
    Mat4ShaderUniform(Shader* shader, const std::string& name, const mat4& value = mat4(1.0f));

    void write(std::string& content) const;

    void apply();
    void setValue(const mat4& value);
    const mat4& getValue() const;
};

class Vec2ArrayShaderUniform : public ShaderUniform {
    vec2* value;
    size_t length;

    void copyValue(const vec2* value);
    void copyValue(const std::vector<vec2>& value);
public:
    Vec2ArrayShaderUniform(Shader* shader, const std::string& name, size_t length, const vec2* value = 0);
    ~Vec2ArrayShaderUniform();

    void write(std::string& content) const;

    void apply();

    void setValue(const vec2* value);
    void setValue(const std::vector<vec2>& value);

    const vec2* getValue() const;
};

class Vec3ArrayShaderUniform : public ShaderUniform {
    vec3* value;
    size_t length;

    void copyValue(const vec3* value);
    void copyValue(const std::vector<vec3>& value);
public:
    Vec3ArrayShaderUniform(Shader* shader, const std::string& name, size_t length, const vec3* value = 0);
    ~Vec3ArrayShaderUniform();

    void write(std::string& content) const;

    void apply();

    void setValue(const vec3* value);
    void setValue(const std::vector<vec3>& value);

    const vec3* getValue() const;
};

class Vec4ArrayShaderUniform : public ShaderUniform {
    vec4* value;
    size_t length;

    void copyValue(const vec4* value);
    void copyValue(const std::vector<vec4>& value);
public:
    Vec4ArrayShaderUniform(Shader* shader, const std::string& name, size_t length, const vec4* value = 0);
    ~Vec4ArrayShaderUniform();

    void write(std::string& content) const;

    void apply();

    void setValue(const vec4* value);
    void setValue(const std::vector<vec4>& value);

    const vec4* getValue() const;
};

class ShaderPart {

    std::string filename;

    std::string raw_source;
    std::string processed_source;

    std::map<std::string,std::string> defines;

    void preprocess();
    void loadSourceFile();

    void substitute(std::string& source, const std::string& name, const std::string& value);
    void applyDefines(std::string& source);
public:
    ShaderPart();

    void setSourceFile(const std::string& filename);
    void setSource(const std::string& source);

    void reload();

    void define(const std::string& name);
    void define(const std::string& name, const char *value, ...);
    void define(const std::string& name, const std::string& value);

    bool isDefined(const std::string& name);

    const std::string& getSource();
};

class ShaderPass {
    GLint       shader_object_type;
    std::string shader_object_desc;
    GLenum      shader_object;

    int version;
    std::map<std::string,std::string> extensions;

    Shader* parent;

    std::string source;
    std::string shader_object_source;

    std::list<ShaderUniform*> uniforms;

    bool errorContext(const char* log_message, std::string& context);

    bool preprocess(const std::string& line);
public:
    ShaderPass(Shader* parent, GLint shader_object_type, const std::string& shader_object_desc);
    virtual ~ShaderPass();

    GLint getType() { return shader_object_type; };

    void toString(std::string& out);

    void unload();
    void compile();

    void checkError();

    ShaderUniform* addArrayUniform(const std::string& name, const std::string& type, size_t length);
    ShaderUniform* addUniform(const std::string& name, const std::string& type);

    virtual void attachTo(GLenum program);

    void includeSource(const std::string& source);
    void includeFile(const std::string& filename);
};

class VertexShader : public ShaderPass {
public:
    VertexShader(Shader* parent);
};

class FragmentShader : public ShaderPass {
public:
    FragmentShader(Shader* parent);
};

class GeometryShader : public ShaderPass {
public:
    GeometryShader(Shader* parent);

    void attachTo(GLenum program);
};

class Shader : public Resource {

    std::map<std::string, ShaderUniform*>  uniforms;
    std::map<std::string,std::string> substitutions;

    std::string prefix;
    GLenum program;
    bool dynamic_compile;

    void checkProgramError();

    void setDefaults();
    void loadPrefix();
public:
    VertexShader*   vertex_shader;
    GeometryShader* geometry_shader;
    FragmentShader* fragment_shader;

    Shader();
    Shader(const std::string& prefix);
    ~Shader();

    GLenum getProgram();

    void clear();

    void load();
    void reload(bool force = false);

    void unload();

    ShaderPass* grabShaderPass(GLenum shader_object_type);

    void includeSource(GLenum shader_object_type, const std::string& source);
    void includeFile(GLenum shader_object_type,   const std::string& filename);

    static void substitute(std::string& source, const std::string& name, const std::string& value);

    void addSubstitute(const std::string& name, const char *value, ...);
    void applySubstitutions(std::string& source);

    void addUniform(ShaderUniform* uniform);
    ShaderUniform* getUniform(const std::string& name);

    void setDynamicCompile(bool dynamic_compile);
    bool needsCompile();

    void applyUniforms();

    void setBool(const std::string& name, bool value);
    void setInteger (const std::string& name, int value);
    void setSampler1D(const std::string& name, int value);
    void setSampler2D(const std::string& name, int value);
    void setFloat(const std::string& name, float value);
    void setVec2 (const std::string& name, const vec2& value);
    void setVec3 (const std::string& name, const vec3& value);
    void setVec4 (const std::string& name, const vec4& value);
    void setMat3 (const std::string& name, const mat3& value);
    void setMat4 (const std::string& name, const mat4& value);

    void setVec2Array(const std::string& name, vec2* value);
    void setVec2Array(const std::string& name, std::vector<vec2>& value);

    void setVec3Array(const std::string& name, vec3* value);
    void setVec3Array(const std::string& name, std::vector<vec3>& value);

    void setVec4Array(const std::string& name, vec4* value);
    void setVec4Array(const std::string& name, std::vector<vec4>& value);

    void setBaked(const std::string& name, bool baked);
    void setBakedUniforms(bool baked);

    void bind();
    void unbind();

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
    void reload(bool force = false);
};

extern ShaderManager shadermanager;

#endif
