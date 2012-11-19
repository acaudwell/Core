#ifndef SHADER_COMMON_H
#define SHADER_COMMON_H

#include "regex.h"

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
extern Regex Shader_ifdef;
extern Regex Shader_endif;

#endif
