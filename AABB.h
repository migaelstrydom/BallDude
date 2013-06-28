/************************************************************************
 * Copyright 2013 Migael Strydom
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 ************************************************************************/
//---------------------------------------------------------------------------
#ifndef AABB_H_
#define AABB_H_
//---------------------------------------------------------------------------
#include "Triangle.h"
#include <GL/gl.h>
//---------------------------------------------------------------------------
// Axis-aligned bounding box
class BDAABB
{
 private:

  inline void ProjectBox(const Vector3& rkD, double& rfMin, double& rfMax) const {
    double fDdC = rkD * center;
    double fR = extent[0] * fabs(rkD.x) + extent[1] * fabs(rkD.y) +
                extent[2] * fabs(rkD.z);
    rfMin = fDdC - fR;
    rfMax = fDdC + fR;
  }

  inline void ProjectTriangle(const Vector3& rkD, const Triangle tri,
                              double& rfMin, double& rfMax) const
  {
    rfMin = rkD * (*tri.a);
    rfMax = rfMin;

    double fDot = rkD * (*tri.b);
    if(fDot < rfMin)
      rfMin = fDot;
    else if(fDot > rfMax)
      rfMax = fDot;

    fDot = rkD * (*tri.c);
    if(fDot < rfMin)
      rfMin = fDot;
    else if(fDot > rfMax)
      rfMax = fDot;
  }

 public:
  Vector3 center;

  float extent[3];

  BDAABB()
  {
    extent[0] = 0.0;
    extent[1] = 0.0;
    extent[2] = 0.0;
  }

  BDAABB(Vector3 c, float x, float y, float z)
    : center(c)
  {
    extent[0] = x;
    extent[1] = y;
    extent[2] = z;
  }

  //returns true if point is in box
  const bool Intersect(const Vector3& p) const
  {
    return (fabs(p.x - center.x) <= extent[0]) &&
        (fabs(p.y - center.y) <= extent[1]) &&
        (fabs(p.z - center.z) <= extent[2]);
  }

  //returns true if this AABB is overlapping 'box'
  const bool Intersect(const BDAABB& box) const
  {
    const Vector3 T = box.center - center;    //vector from A to B
    return (fabs(T.x) <= (extent[0] + box.extent[0])) &&
        (fabs(T.y) <= (extent[1] + box.extent[1])) &&
        (fabs(T.z) <= (extent[2] + box.extent[2]));
  }

  const bool Intersect(const Triangle tri) const
  {
    double fMin0, fMax0, fMin1, fMax1;
    Vector3 kD, akE[3];

    // test direction of triangle normal
    akE[0] = *tri.b - *tri.a;
    akE[1] = *tri.c - *tri.a;
    kD = akE[0] ^ (akE[1]);
    fMin0 = kD * (*tri.a);
    fMax0 = fMin0;
    ProjectBox(kD, fMin1, fMax1);
    if ((fMax1 < fMin0) || (fMax0 < fMin1))
      return false;

    // test direction of box faces
    for (int i = 0; i < 3; i++)
    {
      kD = Vector3((i == 0) ? 1.0 : 0.0,
                   (i == 1) ? 1.0 : 0.0,
                   (i == 2) ? 1.0 : 0.0);

      ProjectTriangle(kD,tri,fMin0,fMax0);
      double fDdC = kD * (center);
      fMin1 = fDdC - extent[i];
      fMax1 = fDdC + extent[i];
      if(fMax1 < fMin0 || fMax0 < fMin1)
        return false;
    }

    // test direction of triangle-box edge cross products
    akE[2] = akE[1] - akE[0];
    for (int i0 = 0; i0 < 3; i0++)
    {
      for (int i1 = 0; i1 < 3; i1++)
      {
        kD = akE[i0] ^ (Vector3((i1 == 0) ? 1.0 : 0.0,
                                (i1 == 1) ? 1.0 : 0.0,
                                (i1 == 2) ? 1.0 : 0.0));
        ProjectTriangle(kD,tri,fMin0,fMax0);
        ProjectBox(kD,fMin1,fMax1);
        if ((fMax1 < fMin0) || (fMax0 < fMin1))
          return false;
      }
    }

    return true;
  }

  const bool InFrontOfPlane(BDPlane *Plane) const
  {
    float dist = Plane->DistanceToPlane(center);
    return (dist >= 0) || (-dist <=
                           (fabs(extent[0]*Plane->N.x) + fabs(extent[1]*Plane->N.y) +
                            fabs(extent[2]*Plane->N.z)));
  }

  // TEMP
  void Draw() const
  {
    glBegin(GL_LINES);
    glVertex3f(center.x - extent[0], center.y - extent[1], center.z - extent[2]);
    glVertex3f(center.x + extent[0], center.y - extent[1], center.z - extent[2]);

    glVertex3f(center.x + extent[0], center.y - extent[1], center.z - extent[2]);
    glVertex3f(center.x + extent[0], center.y - extent[1], center.z + extent[2]);

    glVertex3f(center.x + extent[0], center.y - extent[1], center.z + extent[2]);
    glVertex3f(center.x - extent[0], center.y - extent[1], center.z + extent[2]);

    glVertex3f(center.x - extent[0], center.y - extent[1], center.z + extent[2]);
    glVertex3f(center.x - extent[0], center.y - extent[1], center.z - extent[2]);


    glVertex3f(center.x - extent[0], center.y + extent[1], center.z - extent[2]);
    glVertex3f(center.x + extent[0], center.y + extent[1], center.z - extent[2]);

    glVertex3f(center.x + extent[0], center.y + extent[1], center.z - extent[2]);
    glVertex3f(center.x + extent[0], center.y + extent[1], center.z + extent[2]);

    glVertex3f(center.x + extent[0], center.y + extent[1], center.z + extent[2]);
    glVertex3f(center.x - extent[0], center.y + extent[1], center.z + extent[2]);

    glVertex3f(center.x - extent[0], center.y + extent[1], center.z + extent[2]);
    glVertex3f(center.x - extent[0], center.y + extent[1], center.z - extent[2]);


    glVertex3f(center.x - extent[0], center.y - extent[1], center.z - extent[2]);
    glVertex3f(center.x - extent[0], center.y + extent[1], center.z - extent[2]);

    glVertex3f(center.x + extent[0], center.y - extent[1], center.z - extent[2]);
    glVertex3f(center.x + extent[0], center.y + extent[1], center.z - extent[2]);

    glVertex3f(center.x + extent[0], center.y - extent[1], center.z + extent[2]);
    glVertex3f(center.x + extent[0], center.y + extent[1], center.z + extent[2]);

    glVertex3f(center.x - extent[0], center.y - extent[1], center.z + extent[2]);
    glVertex3f(center.x - extent[0], center.y + extent[1], center.z + extent[2]);
    glEnd();
  }
};
//---------------------------------------------------------------------------
#endif  // AABB_H_
