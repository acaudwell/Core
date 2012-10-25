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

#include "vectors.h"
#include "resource.h"

#include <list>
#include <map>
#include <string>
#include <fstream>
#include <sstream>
#include <vector>

#include "shader_common.h"

class __ShaderException : public std::exception {
protected:
    std::string message;
public:
    __ShaderException(const char* str, ...);
    __ShaderException(const std::string& message);

    ~__ShaderException() throw () {};

    virtual const char* what() const throw() { return message.c_str(); }
};

class __Shader;
class __ShaderPass;

class __ShaderUniform {
protected:
    std::string name;
    std::string comment;
    int  location;
    __Shader* shader;
    int uniform_type;
    std::string type_name;
    bool modified;
    bool initialized;
    bool baked;
public:

    __ShaderUniform(__Shader* shader, const std::string& name, int uniform_type, const std::string& type_name);

    virtual void unload();

    int getType() { return uniform_type; };

    virtual void write(std::string& content) const {};

    const std::string& getName() const;
    bool  isInitialized() const { return initialized; };
    int   getLocation();

    bool isBaked() const    { return baked; };
    bool isModified() const { return modified; };

    void setComment(const std::string& comment);
    const std::string& getComment() const;

    void setInitialized(bool initialized) { this->initialized = initialized; };

    virtual void apply() {};
    
    virtual void setBaked(bool baked);
    virtual void setModified(bool modified) { this->modified = modified; };
};

class __FloatShaderUniform : public __ShaderUniform {
    float value;
public:
    __FloatShaderUniform(__Shader* shader, const std::string& name, float value = 0.0f);

    void write(std::string& content) const;

    void apply();
    void setValue(float value);
    float getValue() const;
};

class __IntShaderUniform : public __ShaderUniform {
    int value;
public:
    __IntShaderUniform(__Shader* shader, const std::string& name, int value = 0);

    void write(std::string& content) const;

    void apply();
    void setValue(int value);
    float getValue() const;
};

class __BoolShaderUniform : public __ShaderUniform {
    bool value;
public:
    __BoolShaderUniform(__Shader* shader, const std::string& name, bool value = false);

    void write(std::string& content) const;

    void apply();
    void setValue(bool value);
    float getValue() const;
};

class __Sampler1DShaderUniform : public __ShaderUniform {
    int value;
public:
    __Sampler1DShaderUniform(__Shader* shader, const std::string& name, int value = 0);

    void write(std::string& content) const;

    void setBaked(bool baked);

    void apply();
    void setValue(int value);
    int getValue() const;
};

class __Sampler2DShaderUniform : public __ShaderUniform {
    int value;
public:
    __Sampler2DShaderUniform(__Shader* shader, const std::string& name, int value = 0);

    void write(std::string& content) const;

    void setBaked(bool baked);

    void apply();
    void setValue(int value);
    int getValue() const;
};

class __Vec2ShaderUniform : public __ShaderUniform {
    vec2 value;
public:
    __Vec2ShaderUniform(__Shader* shader, const std::string& name, const vec2& value = vec2(0.0f)) ;

    void write(std::string& content) const;

    void apply();
    void setValue(const vec2& value);
    const vec2& getValue() const;
};

class __Vec3ShaderUniform : public __ShaderUniform {
    vec3 value;
public:
    __Vec3ShaderUniform(__Shader* shader, const std::string& name, const vec3& value = vec3(0.0f));

    void write(std::string& content) const;

    void apply();
    void setValue(const vec3& value);
    const vec3& getValue() const;
};

class __Vec4ShaderUniform : public __ShaderUniform {
    vec4 value;
public:
    __Vec4ShaderUniform(__Shader* shader, const std::string& name, const vec4& value = vec4(0.0f));

    void write(std::string& content) const;

    void apply();
    void setValue(const vec4& value);
    const vec4& getValue() const;
};

class __Mat3ShaderUniform : public __ShaderUniform {
    mat3 value;
public:
    __Mat3ShaderUniform(__Shader* shader, const std::string& name, const mat3& value = mat3(1.0f));

    void write(std::string& content) const;

    void apply();
    void setValue(const mat3& value);
    const mat3& getValue() const;
};

class __Mat4ShaderUniform : public __ShaderUniform {
    mat4 value;
public:
    __Mat4ShaderUniform(__Shader* shader, const std::string& name, const mat4& value = mat4(1.0f));

    void write(std::string& content) const;

    void apply();
    void setValue(const mat4& value);
    const mat4& getValue() const;
};

class __Vec2ArrayShaderUniform : public __ShaderUniform {
    vec2* value;
    size_t length;

    void copyValue(const vec2* value);
    void copyValue(const std::vector<vec2>& value);
public:
    __Vec2ArrayShaderUniform(__Shader* shader, const std::string& name, size_t length, const vec2* value = 0);
    ~__Vec2ArrayShaderUniform();

    void write(std::string& content) const;

    void apply();

    void setValue(const vec2* value);
    void setValue(const std::vector<vec2>& value);

    const vec2* getValue() const;
};

class __Vec3ArrayShaderUniform : public __ShaderUniform {
    vec3* value;
    size_t length;

    void copyValue(const vec3* value);
    void copyValue(const std::vector<vec3>& value);
public:
    __Vec3ArrayShaderUniform(__Shader* shader, const std::string& name, size_t length, const vec3* value = 0);
    ~__Vec3ArrayShaderUniform();

    void write(std::string& content) const;

    void apply();

    void setValue(const vec3* value);
    void setValue(const std::vector<vec3>& value);

    const vec3* getValue() const;
};

class __Vec4ArrayShaderUniform : public __ShaderUniform {
    vec4* value;
    size_t length;

    void copyValue(const vec4* value);
    void copyValue(const std::vector<vec4>& value);
public:
    __Vec4ArrayShaderUniform(__Shader* shader, const std::string& name, size_t length, const vec4* value = 0);
    ~__Vec4ArrayShaderUniform();

    void write(std::string& content) const;

    void apply();

    void setValue(const vec4* value);
    void setValue(const std::vector<vec4>& value);

    const vec4* getValue() const;
};

class __ShaderPart {

    std::string filename;

    std::string raw_source;
    std::string processed_source;

    std::map<std::string,std::string> defines;

    void preprocess();
    void loadSourceFile();

    void substitute(std::string& source, const std::string& name, const std::string& value);
    void applyDefines(std::string& source);
public:
    __ShaderPart();

    void setSourceFile(const std::string& filename);
    void setSource(const std::string& source);

    void reload();
    void reset();

    void define(const std::string& name);
    void define(const std::string& name, const char *value, ...);
    void define(const std::string& name, const std::string& value);

    bool isDefined(const std::string& name);

    const std::string& getSource();
};

class __ShaderPass {
    int          shader_object_type;
    std::string  shader_object_desc;
    unsigned int shader_object;

    int version;
    std::map<std::string,std::string> extensions;

    __Shader* parent;

    std::string source;
    std::string shader_object_source;

    std::list<__ShaderUniform*> uniforms;

    bool errorContext(const char* log_message, std::string& context);

    bool preprocess(const std::string& line);
public:
    __ShaderPass(__Shader* parent, int shader_object_type, const std::string& shader_object_desc);
    virtual ~__ShaderPass();

    int getType() { return shader_object_type; };

    bool isEmpty();

    void toString(std::string& out);
    const std::string& getObjectSource();

    void unload();
    void compile();

    void checkError();

    __ShaderUniform* addArrayUniform(const std::string& name, const std::string& type, size_t length);
    __ShaderUniform* addUniform(const std::string& name, const std::string& type);

    virtual void attachTo(unsigned int program);

    void includeSource(const std::string& source);
    void includeFile(const std::string& filename);
};

class __VertexShader : public __ShaderPass {
public:
    __VertexShader(__Shader* parent);
};

class __FragmentShader : public __ShaderPass {
public:
    __FragmentShader(__Shader* parent);
};

class __GeometryShader : public __ShaderPass {
public:
    __GeometryShader(__Shader* parent);

    void attachTo(unsigned int program);
};

class __Shader : public Resource {
protected:
    std::map<std::string, __ShaderUniform*>  uniforms;
    std::map<std::string,std::string> substitutions;

    std::string prefix;
    unsigned int program;
    bool dynamic_compile;

    void checkProgramError();

    void setDefaults();
    void loadPrefix();
public:
    __VertexShader*   vertex_shader;
    __GeometryShader* geometry_shader;
    __FragmentShader* fragment_shader;

    __Shader();
    __Shader(const std::string& prefix);
    ~__Shader();

    unsigned int getProgram();

    void clear();

    void load();
    void reload(bool force = false);

    bool isEmpty();

    void unload();

    __ShaderPass* grabShaderPass(unsigned int shader_object_type);

    void includeSource(unsigned int shader_object_type, const std::string& source);
    void includeFile(unsigned int shader_object_type,   const std::string& filename);

    static void substitute(std::string& source, const std::string& name, const std::string& value);

    void addSubstitute(const std::string& name, const char *value, ...);
    void applySubstitutions(std::string& source);

    void addUniform(__ShaderUniform* uniform);
    __ShaderUniform* getUniform(const std::string& name);

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

class __ShaderManager : public ResourceManager {
public:
    __ShaderManager();
    __Shader* grab(const std::string& shader_prefix);

    void manage(__Shader* shader);

    void unload();
    void reload(bool force = false);
};
