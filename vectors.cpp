#include "vectors.h"

vec2 rotate_vec2(const vec2& v, float s, float c) {
    return vec2( v.x * c - v.y * s, v.x * s + v.y * c );
}

vec2 normalise(const vec2& v) {
    float l = glm::length(v);
    if(l > 0.0) return v / l;
    return v;
}

vec3 normalise(const vec3& v) {
    float l = glm::length(v);
    if(l > 0.0) return v / l;
    return v;
}

vec4 normalise(const vec4& v) {
    float l = glm::length(v);
    if(l > 0.0) return v / l;
    return v;
}
