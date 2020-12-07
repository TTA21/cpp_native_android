//
// Created by USER on 05/12/2020.
//

#ifndef ANDROID_PROJECT_LIN_ALG_H
#define ANDROID_PROJECT_LIN_ALG_H

#include "data_structures.h"

void MultiplyMatrixVector(vec3d &i, vec3d &o, mat4x4 &m)
{
    o.x = i.x * m.m[0][0] + i.y * m.m[1][0] + i.z * m.m[2][0] + m.m[3][0];
    o.y = i.x * m.m[0][1] + i.y * m.m[1][1] + i.z * m.m[2][1] + m.m[3][1];
    o.z = i.x * m.m[0][2] + i.y * m.m[1][2] + i.z * m.m[2][2] + m.m[3][2];
    float w = i.x * m.m[0][3] + i.y * m.m[1][3] + i.z * m.m[2][3] + m.m[3][3];

    if (w != 0.0f)
    {
        o.x /= w; o.y /= w; o.z /= w;
    }
}

void Rotate_Z( mat4x4 &matRotZ , float theta ){
    matRotZ.m[0][0] = cosf( theta );
    matRotZ.m[0][1] = sinf( theta );
    matRotZ.m[1][0] = -sinf( theta );
    matRotZ.m[1][1] = cosf( theta );
    matRotZ.m[2][2] = 1;
    matRotZ.m[3][3] = 1;
}

void Rotate_X( mat4x4 &matRotX , float theta ){
    matRotX.m[0][0] = 1;
    matRotX.m[1][1] = cosf( theta );
    matRotX.m[1][2] = sinf( theta );
    matRotX.m[2][1] = -sinf( theta );
    matRotX.m[2][2] = cosf( theta );
    matRotX.m[3][3] = 1;
}

void Rotate_Y( mat4x4 &matRotY , float theta ){
    matRotY.m[0][0] = cosf( theta );
    matRotY.m[0][2] = sinf( theta );
    matRotY.m[1][1] = 1;
    matRotY.m[2][0] = -sinf( theta );
    matRotY.m[2][2] = cosf( theta );
}

void Apply_Transformation( triangle source , triangle &destination , mat4x4 transformation ){
    MultiplyMatrixVector(source.p[0], destination.p[0], transformation);
    MultiplyMatrixVector(source.p[1], destination.p[1], transformation);
    MultiplyMatrixVector(source.p[2], destination.p[2], transformation);
}

void Cross_Normal( vec3d &normal , triangle &triTranslated ){

    vec3d line1, line2;

    line1.x = triTranslated.p[1].x - triTranslated.p[0].x;
    line1.y = triTranslated.p[1].y - triTranslated.p[0].y;
    line1.z = triTranslated.p[1].z - triTranslated.p[0].z;

    line2.x = triTranslated.p[2].x - triTranslated.p[0].x;
    line2.y = triTranslated.p[2].y - triTranslated.p[0].y;
    line2.z = triTranslated.p[2].z - triTranslated.p[0].z;

    normal.x = line1.y * line2.z - line1.z * line2.y;
    normal.y = line1.z * line2.x - line1.x * line2.z;
    normal.z = line1.x * line2.y - line1.y * line2.x;

}

float Cross_View( vec3d normal , vec3d p ){

    return
    normal.x * (p.x - vCamera.x) +
    normal.y * (p.y - vCamera.y) +
    normal.z * (p.z - vCamera.z);

}

float Find_Brightness( vec3d &light_direction , vec3d &normal ){

    float l = sqrtf(light_direction.x*light_direction.x + light_direction.y*light_direction.y + light_direction.z*light_direction.z);
    light_direction.x /= l; light_direction.y /= l; light_direction.z /= l;

    // How similar is normal to light direction
    return normal.x * light_direction.x + normal.y * light_direction.y + normal.z * light_direction.z;

}

#endif //ANDROID_PROJECT_LIN_ALG_H
