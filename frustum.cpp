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

#include "frustum.h"

// Lighthouse 3D Frustum tutorial:
// http://www.lighthouse3d.com/opengl/viewfrustum/index.php?gasource

Frustum::Frustum() {
}

Frustum::Frustum(const vec3& source, const vec3& target, const vec3& up, float fov, float near_distance, float far_distance) {
    update(source, target, up, fov, near_distance, far_distance);
}

void Frustum::update(const vec3& source, const vec3& target, const vec3& up, float fov, float near_distance, float far_distance) {
    updatePerspective(fov, near_distance, far_distance);
    updateView(source, target, up);
}

void Frustum::updatePerspective(float fov, float near_distance, float far_distance) {

    this->near_distance = near_distance;
    this->far_distance  = far_distance;

    view_ratio    = (float) display.width / (float) display.height;

    float toa = (float) tan(fov * 0.5 * DEGREES_TO_RADIANS);

    near_half_height = near_distance * toa;
    near_half_width  = near_half_height * view_ratio;
    far_half_height  = far_distance  * toa;
    far_half_width   = far_half_height * view_ratio;
}

void Frustum::updateView(const vec3& source, const vec3& target, const vec3& up) {

    vec3 view_ray = normalise(target - source);

    vec3 horiz_normal = normalise(glm::cross(view_ray,up));

    vec3 vert_normal = glm::cross(horiz_normal, view_ray);

    //calculate the positions of the 8 points that make up
    //the viewing frustum and then use them to create the 6 planes

    vec3 near_centre = source + view_ray * near_distance;
    vec3 far_centre  = source + view_ray * far_distance;

    vec3 near_horiz_offset = horiz_normal * near_half_width;
    vec3 near_vert_offset  = vert_normal  * near_half_height;

    near_top_left     = near_centre + near_vert_offset - near_horiz_offset;
    near_top_right    = near_centre + near_vert_offset + near_horiz_offset;
    near_bottom_left  = near_centre - near_vert_offset - near_horiz_offset;
    near_bottom_right = near_centre - near_vert_offset + near_horiz_offset;

    vec3 far_horiz_offset = horiz_normal * far_half_width;
    vec3 far_vert_offset  = vert_normal  * far_half_height;

    far_top_left     = far_centre + far_vert_offset - far_horiz_offset;
    far_top_right    = far_centre + far_vert_offset + far_horiz_offset;
    far_bottom_left  = far_centre - far_vert_offset - far_horiz_offset;
    far_bottom_right = far_centre - far_vert_offset + far_horiz_offset;

    //top
    planes[0] = Plane(near_top_right,    near_top_left,     far_top_left);

    //bottom
    planes[1] = Plane(near_bottom_left,  near_bottom_right, far_bottom_right);

    //left
    planes[2] = Plane(near_top_left,     near_bottom_left,  far_bottom_left);

    //right
    planes[3] = Plane(near_bottom_right, near_top_right,    far_bottom_right);

    //near
    planes[4] = Plane(near_top_left,     near_top_right,    near_bottom_right);

    //far
    planes[5] = Plane(far_top_right,     far_top_left,      far_bottom_left);
}

bool Frustum::contains(const vec3& p) const {

    for(int i=0; i < 6; i++) {
        float dist = planes[i].distance(p);

        if(dist < 0) return false;
    }

    return true;
}

bool Frustum::intersects(const Bounds3D& bounds) const {

    vec3 corner;

    for(int i=0; i<6; i++) {

        corner.x = planes[i].normal.x > 0.0 ? bounds.max.x : bounds.min.x;
        corner.y = planes[i].normal.y > 0.0 ? bounds.max.y : bounds.min.y;
        corner.z = planes[i].normal.z > 0.0 ? bounds.max.z : bounds.min.z;

        if (planes[i].distance(corner) < 0.0) return false;
    }

    return true;

}

bool Frustum::intersects(const Bounds2D& bounds, float z) const {

    vec3 corner;

    for(int i=0; i<6; i++) {

        corner.x = planes[i].normal.x > 0.0 ? bounds.max.x : bounds.min.x;
        corner.y = planes[i].normal.y > 0.0 ? bounds.max.y : bounds.min.y;
        corner.z = z;

        if (planes[i].distance(corner) < 0.0) return false;
    }

    return true;
}


