#include "shader_common.h"

Regex Shader_pre_version("^\\s*#version\\s*(\\d+)\\s*");
Regex Shader_pre_extension("^\\s*#extension\\s*([a-zA-Z0-9_]+)\\s+:\\s+(enable|require|warn|disable)\\s*$");
Regex Shader_pre_include("^\\s*#include\\s*\"([^\"]+)\"");
Regex Shader_uniform_def("^\\s*uniform\\s+(\\w+)\\s+(\\w+)(?:\\[(\\d+)\\])?\\s*;\\s*(?://\\s*(.+))?$");
Regex Shader_error_line("\\b\\d*\\((\\d+)\\) : error ");
Regex Shader_error2_line("\\bERROR: \\d+:(\\d+):");
Regex Shader_error3_line("^\\d+:(\\d+)\\(\\d+\\): error");
Regex Shader_warning_line("\\b\\d*\\((\\d+)\\) : warning ");
Regex Shader_redefine_line(" (?:defined|declaration) at \\d*\\((\\d+)\\)");
