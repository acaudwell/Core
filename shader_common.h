#ifndef SHADER_COMMON_H
#define SHADER_COMMON_H

#include "resource.h"
#include "regex.h"
#include "vectors.h"

#include <map>
#include <list>
#include <string>
#include <exception>
#include <vector>

extern std::string gSDLAppShaderDir;

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

extern Regex Shader_pre_version;
extern Regex Shader_pre_extension;
extern Regex Shader_pre_include;
extern Regex Shader_uniform_def;
extern Regex Shader_error_line;
extern Regex Shader_error2_line;
extern Regex Shader_error3_line;
extern Regex Shader_warning_line;
extern Regex Shader_redefine_line;

class ShaderException : public std::exception {
protected:
    std::string message;
    std::string source;
public:
    ShaderException(const std::string& message);
    ShaderException(const std::string& message, const std::string& source);

    ~ShaderException() throw () {};

    virtual const char* what() const throw() { return message.c_str(); }

    const std::string& getSource() const;
};

class AbstractShader;

class ShaderUniform {
protected:
    std::string name;
    std::string comment;
    int  location;
    AbstractShader* shader;
    int uniform_type;
    std::string type_name;
    bool modified;
    bool initialized;
    bool baked;
public:

    ShaderUniform(AbstractShader* shader, const std::string& name, int uniform_type, const std::string& type_name);
    virtual ~ShaderUniform() {};

    virtual void unload();

    int getLocation();

    int getType() { return uniform_type; };

    virtual void write(std::string& content) const {};

    const std::string& getName() const;
    bool  isInitialized() const { return initialized; };

    bool isBaked() const    { return baked; };
    bool isModified() const { return modified; };

    void setComment(const std::string& comment);
    const std::string& getComment() const;

    void setInitialized(bool initialized) { this->initialized = initialized; };

    virtual void setBaked(bool baked);
    virtual void setModified(bool modified) { this->modified = modified; };
};

class FloatShaderUniform : public ShaderUniform {
    float value;
public:
    FloatShaderUniform(AbstractShader* shader, const std::string& name, float value = 0.0f);

    void write(std::string& content) const;

    void setValue(float value);

    float& getValue();
};

class IntShaderUniform : public ShaderUniform {
    int value;
public:
    IntShaderUniform(AbstractShader* shader, const std::string& name, int value = 0);

    void write(std::string& content) const;

    void setValue(int value);

    int& getValue();
};

class BoolShaderUniform : public ShaderUniform {
    bool value;
public:
    BoolShaderUniform(AbstractShader* shader, const std::string& name, bool value = false);

    void write(std::string& content) const;

    void setValue(bool value);

    bool& getValue();
};

class Sampler1DShaderUniform : public ShaderUniform {
    int value;
public:
    Sampler1DShaderUniform(AbstractShader* shader, const std::string& name, int value = 0);

    void write(std::string& content) const;

    void setBaked(bool baked);

    void setValue(int value);

    int& getValue();
};

class Sampler2DShaderUniform : public ShaderUniform {
    int value;
public:
    Sampler2DShaderUniform(AbstractShader* shader, const std::string& name, int value = 0);

    void write(std::string& content) const;

    void setBaked(bool baked);

    void setValue(int value);

    int& getValue();
};

class Vec2ShaderUniform : public ShaderUniform {
    vec2 value;
public:
    Vec2ShaderUniform(AbstractShader* shader, const std::string& name, const vec2& value = vec2(0.0f)) ;

    void write(std::string& content) const;

    void setValue(const vec2& value);

    vec2& getValue();
};

class Vec3ShaderUniform : public ShaderUniform {
    vec3 value;
public:
    Vec3ShaderUniform(AbstractShader* shader, const std::string& name, const vec3& value = vec3(0.0f));

    void write(std::string& content) const;

    void setValue(const vec3& value);

    vec3& getValue();
};

class Vec4ShaderUniform : public ShaderUniform {
    vec4 value;
public:
    Vec4ShaderUniform(AbstractShader* shader, const std::string& name, const vec4& value = vec4(0.0f));

    void write(std::string& content) const;

    void setValue(const vec4& value);

    vec4& getValue();
};

class Mat3ShaderUniform : public ShaderUniform {
    mat3 value;
public:
    Mat3ShaderUniform(AbstractShader* shader, const std::string& name, const mat3& value = mat3(1.0f));

    void write(std::string& content) const;

    void setValue(const mat3& value);

    mat3& getValue();
};

class Mat4ShaderUniform : public ShaderUniform {
    mat4 value;
public:
    Mat4ShaderUniform(AbstractShader* shader, const std::string& name, const mat4& value = mat4(1.0f));

    void write(std::string& content) const;

    void setValue(const mat4& value);

    mat4& getValue();
};

class Vec2ArrayShaderUniform : public ShaderUniform {
    std::vector<vec2> value;
    size_t length;

    void copyValue(const vec2* value);
    void copyValue(const std::vector<vec2>& value);
public:
    Vec2ArrayShaderUniform(AbstractShader* shader, const std::string& name, size_t length, const vec2* value = 0);
    ~Vec2ArrayShaderUniform();

    void write(std::string& content) const;

    void setValue(const vec2* value);
    void setValue(const std::vector<vec2>& value);

    const std::vector<vec2>& getValue();

    size_t getLength() const;
};

class Vec3ArrayShaderUniform : public ShaderUniform {
    std::vector<vec3> value;
    size_t length;

    void copyValue(const vec3* value);
    void copyValue(const std::vector<vec3>& value);
public:
    Vec3ArrayShaderUniform(AbstractShader* shader, const std::string& name, size_t length, const vec3* value = 0);
    ~Vec3ArrayShaderUniform();

    void write(std::string& content) const;

    void setValue(const vec3* value);
    void setValue(const std::vector<vec3>& value);

    const std::vector<vec3>& getValue();

    size_t getLength() const;
};

class Vec4ArrayShaderUniform : public ShaderUniform {
    std::vector<vec4> value;
    size_t length;

    void copyValue(const vec4* value);
    void copyValue(const std::vector<vec4>& value);
public:
    Vec4ArrayShaderUniform(AbstractShader* shader, const std::string& name, size_t length, const vec4* value = 0);
    ~Vec4ArrayShaderUniform();

    void write(std::string& content) const;

    void setValue(const vec4* value);
    void setValue(const std::vector<vec4>& value);

    const std::vector<vec4>& getValue();

    size_t getLength() const;
};

class ShaderPart {

    std::string filename;

    std::string raw_source;
    std::string processed_source;

    std::map<std::string,std::string> defines;
    std::map<std::string,std::string> substitutions;

    void preprocess();
    void loadSourceFile();

    void applySubstitution(std::string& source, const std::string& name, const std::string& value);

    void applyDefines(std::string& source);
    void applySubstitutions(std::string& source);
public:
    ShaderPart();

    void setSourceFile(const std::string& filename);
    void setSource(const std::string& source);

    void reload();
    void reset();

    void substitute(const std::string& name, const char *value, ...);
    void substitute(const std::string& name, const std::string& value);

    void define(const std::string& name);
    void define(const std::string& name, const char *value, ...);
    void define(const std::string& name, const std::string& value);

    const std::string& getSource();
};

class AbstractShaderPass {
protected:
    int          shader_object_type;
    std::string  shader_object_desc;
    unsigned int shader_object;

    int version;
    std::map<std::string,std::string> extensions;

    AbstractShader* parent;

    std::string source;
    std::string shader_object_source;

    std::list<ShaderUniform*> uniforms;

    void showContext(std::string& context, int line_no, int amount);
    bool errorContext(const char* log_message, std::string& context);

    bool preprocess(const std::string& line);
public:
    AbstractShaderPass(AbstractShader* parent, int shader_object_type, const std::string& shader_object_desc);
    virtual ~AbstractShaderPass() {};

    int getType() { return shader_object_type; };

    bool isEmpty();

    void toString(std::string& out);
    const std::string& getObjectSource();

    ShaderUniform* addArrayUniform(const std::string& name, const std::string& type, size_t length);
    ShaderUniform* addUniform(const std::string& name, const std::string& type);

    void includeSource(const std::string& source);
    void includeFile(const std::string& filename);

    std::list<ShaderUniform*>& getUniforms();

    virtual void attachTo(unsigned int program) = 0;
    virtual void unload() = 0;
    virtual void compile() = 0;
    virtual void checkError() = 0;

};

class AbstractShader : public Resource {
protected:
    std::map<std::string, ShaderUniform*>  uniforms;
    std::list<ShaderUniform*> uniform_list;
    std::map<std::string,std::string> substitutions;

    std::string prefix;
    unsigned int program;
    bool dynamic_compile;

    void setDefaults();

    virtual void loadPrefix() = 0;
    virtual void checkProgramError() = 0;
public:
    AbstractShaderPass* vertex_shader;
    AbstractShaderPass* geometry_shader;
    AbstractShaderPass* fragment_shader;

    AbstractShader();
    AbstractShader(const std::string& prefix);

    unsigned int getProgram();

    void clear();

    void reload(bool force = false);

    bool isEmpty();

    void includeSource(unsigned int shader_object_type, const std::string& source);
    void includeFile(unsigned int shader_object_type,   const std::string& filename);

    static void substitute(std::string& source, const std::string& name, const std::string& value);

    void addSubstitute(const std::string& name, const std::string& value);
    void addSubstitute(const std::string& name, const char *value, ...);

    void applySubstitutions(std::string& source);

    void addUniform(ShaderUniform* uniform);
    ShaderUniform* getUniform(const std::string& name);

    void setDynamicCompile(bool dynamic_compile);
    bool needsCompile();

    const std::list<ShaderUniform*>& getUniforms();

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

    virtual AbstractShaderPass* grabShaderPass(unsigned int shader_object_type) = 0;

    virtual void applyUniform(ShaderUniform* u) = 0;

    virtual int getUniformLocation(const std::string& uniform_name) = 0;

    virtual void load() = 0;
    virtual void unload() = 0;

    virtual void bind() = 0;
    virtual void unbind() = 0;

    void use();
};

#endif
