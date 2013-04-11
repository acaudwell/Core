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

class Shader;

class ShaderPass : public AbstractShaderPass {
public:
    ShaderPass(Shader* parent, int shader_object_type, const std::string& shader_object_desc);
    ~ShaderPass();

    void attachTo(unsigned int program);
    void unload();
    void compile();
    void checkError();
};

class Shader : public AbstractShader {
protected:
    void checkProgramError();
public:
    Shader();
    Shader(const std::string& prefix);
    ~Shader();

    void applyUniform(ShaderUniform* u);

    int getUniformLocation(const std::string& uniform_name);

    void loadPrefix();

    void load();
    void unload();

    void bind();
    void unbind();

    AbstractShaderPass* grabShaderPass(unsigned int shader_object_type);
};

class ShaderManager : public ResourceManager {
public:
    ShaderManager();
    Shader* grab(const std::string& shader_prefix);

    void manage(Shader* shader);

    void unload();
    void reload(bool force = false);
};

extern ShaderManager shadermanager;
