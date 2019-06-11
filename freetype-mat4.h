#pragma once
#include <cstdlib>
#include <string>
#include <cmath>
#include "MathExt.h"

namespace ftgl {

typedef union
{
    float data[16];    /**< All compoments at once     */
    struct {
        float m00, m01, m02, m03;
        float m10, m11, m12, m13;
        float m20, m21, m22, m23;
        float m30, m31, m32, m33;
    };
} mat4;

inline void mat4_set_zero( mat4& self )
{
    memset( &self.data, 0, sizeof( mat4 ));
}

inline void mat4_set_identity( mat4& self )
{
    memset( &self.data, 0, sizeof( mat4 ));
    self.m00 = 1.0;
    self.m11 = 1.0;
    self.m22 = 1.0;
    self.m33 = 1.0;
}

inline void mat4_set_orthographic( mat4& self,
                       float left,   float right,
                       float bottom, float top,
                       float znear,  float zfar )
{
    if (left == right || bottom == top || znear == zfar)
        return;

    mat4_set_zero( self );

    self.m00 = +2.0f/(right-left);
    self.m30 = -(right+left)/(right-left);
    self.m11 = +2.0f/(top-bottom);
    self.m31 = -(top+bottom)/(top-bottom);
    self.m22 = -2.0f/(zfar-znear);
    self.m32 = -(zfar+znear)/(zfar-znear);
    self.m33 = 1.0f;
}

inline void mat4_set_frustum( mat4& self,
                  float left,   float right,
                  float bottom, float top,
                  float znear,  float zfar )
{
    if (left == right || bottom == top || znear == zfar)
        return;

    mat4_set_zero( self );

    self.m00 = (2.0f*znear)/(right-left);
    self.m20 = (right+left)/(right-left);

    self.m11 = (2.0f*znear)/(top-bottom);
    self.m21 = (top+bottom)/(top-bottom);

    self.m22 = -(zfar+znear)/(zfar-znear);
    self.m32 = -(2.0f*zfar*znear)/(zfar-znear);

    self.m23 = -1.0f;
}

inline void mat4_set_perspective( mat4& self,
                      float fovy,  float aspect,
                      float znear, float zfar)
{
    float h, w;

    if (znear == zfar)
        return;

    h = (float)tan(fovy / 360.0 * PI) * znear;
    w = h * aspect;

    mat4_set_frustum( self, -w, w, -h, h, znear, zfar );
}

inline void mat4_set_rotation( mat4& self,
                   float angle,
                   float x, float y, float z)
{
    float c, s, norm;

    c = (float)cos( PI*angle/180.0 );
    s = (float)sin( PI*angle/180.0 );
    norm = (float)sqrt(x*x+y*y+z*z);

    x /= norm; y /= norm; z /= norm;

    mat4_set_identity( self );

    self.m00 = x*x*(1-c)+c;
    self.m10 = y*x*(1-c)-z*s;
    self.m20 = z*x*(1-c)+y*s;

    self.m01 =  x*y*(1-c)+z*s;
    self.m11 =  y*y*(1-c)+c;
    self.m21 =  z*y*(1-c)-x*s;

    self.m02 = x*z*(1-c)-y*s;
    self.m12 = y*z*(1-c)+x*s;
    self.m22 = z*z*(1-c)+c;
}

inline void mat4_set_translation( mat4& self,
                      float x, float y, float z)
{
    mat4_set_identity( self );
    self.m30 = x;
    self.m31 = y;
    self.m32 = z;
}

inline void mat4_set_scaling( mat4& self,
                  float x, float y, float z)
{
    mat4_set_identity( self );
    self.m00 = x;
    self.m11 = y;
    self.m22 = z;
}

}
