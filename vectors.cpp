#include "vectors.h"

vec2 rotate_vec2(const vec2& v, float s, float c) {
    return vec2( v.x * c - v.y * s, v.x * s + v.y * c );
}

vec2 normalize(const vec2& v) {
    float l = glm::length(v);
    if(l > 0.0) return v / l;
    return v;
}

vec3 normalize(const vec3& v) {
    float l = glm::length(v);
    if(l > 0.0) return v / l;
    return v;
}

vec4 normalize(const vec4& v) {
    float l = glm::length(v);
    if(l > 0.0) return v / l;
    return v;
}
