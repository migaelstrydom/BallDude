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
#ifndef TriangleH
#define TriangleH
//---------------------------------------------------------------------------
#include "Plane.h"
#include "DrawableObject.h"
//---------------------------------------------------------------------------
enum ColType {NOCOLLISION, SPLANE, TRIANGLE};

class Triangle {
 private:
  BDPlane plab, plbc, plca;   // Planes at edges
  double lab, lbc, lca;       // lengths of sides

  Vector3 ClosestPointOnTriangle(Vector3 p) {
    Vector3 Rab = ClosestPointOnLine(a, b, p, lab);
    Vector3 Rbc = ClosestPointOnLine(b, c, p, lbc);
    Vector3 Rca = ClosestPointOnLine(c, a, p, lca);

    float RabLen = (p - Rab).SquaredLength();
    float RbcLen = (p - Rbc).SquaredLength();
    float RcaLen = (p - Rca).SquaredLength();

    if (RabLen < RbcLen)
      return (RabLen < RcaLen) ? Rab : Rca;
    else
      return (RbcLen < RcaLen) ? Rbc : Rca;
  }

  bool IntersectSphere(Vector3 rO, Vector3 rV, Vector3 sO, double sR,
		       Vector3 &point) {
    Vector3 sr_dist = sO - rO;
    double v = sr_dist * rV;
    double disc = sR*sR - (sr_dist*sr_dist - v*v);

    // If there was no intersection, return false
    if (disc < 0.0)
      return false;

    double d = sqrt(disc);
    point = rO + rV * (v - d);

    return true;
  }

  Vector3 ClosestPointOnLine(Vector3 *a, Vector3 *b, Vector3 p, double vlength) {
    Vector3 c = p - *a;
    Vector3 V = (*b - *a);
    double d = vlength;
    V /= vlength;
    double t = V * c;

    if (t < 0) return *a;
    if (t > d) return *b;

    V *= t;
    return *a + V;
  }

 public:
  Vector3 *a, *b, *c;       // Vertices
  BDPlane Plane;          // Plane on which tri lies

  bool drawn;    // Specifies whether the tri has been drawn this frame
  bool pclist;   // Specifies whether the tri is in the pclist for this frame

  float friction;   // friction factor of triangle

  BDCollision cCollision;

  Triangle() {
    drawn = false;
    pclist = false;
  }

  Triangle(Vector3* x, Vector3* y, Vector3* z)
    : a(x), b(y), c(z), Plane(*x, *y, *z) {
    plab = BDPlane(*x, *y, (*y + Plane.N));
    plbc = BDPlane(*y, *z, (*z + Plane.N));
    plca = BDPlane(*z, *x, (*x + Plane.N));
    lab = (*x - *y).length();
    lbc = (*y - *z).length();
    lca = (*z - *x).length();
    drawn = false;
    pclist = false;
  }

  ~Triangle() {
  }

  ColType CollidesWithObject(float deltaTime, DrawableObject* obj) {
    if ((obj->velocity == Vector3(0.0, 0.0, 0.0)) ||
       (Plane.DistanceToPlane(obj->position) < 0.0))
      return NOCOLLISION;

    Vector3 pInt = Plane.RayIntersection(obj->position, Plane.N);

    Vector3 sInt;
    ColType rval = TRIANGLE;
    if ((plab.DistanceToPlane(pInt) > 0) || (plbc.DistanceToPlane(pInt) > 0) ||
       (plca.DistanceToPlane(pInt) > 0)) {
      cCollision = BDCollision(this, 0, pInt);
      return SPLANE;
    } else {
      sInt = obj->position - (Plane.N * obj->size);
    }

    if (Plane.DistanceToPlane(sInt + (obj->velocity * deltaTime)) >= 0.0)
      return NOCOLLISION;

    cCollision = BDCollision(this, Plane, pInt);
    if (!obj->Collision(&cCollision))
      return NOCOLLISION;

    return rval;
  }

  ColType CollidesWithSplane(float deltaTime, DrawableObject* obj) {
    Vector3 sInt;
    cCollision.pInt = ClosestPointOnTriangle(cCollision.pInt);
    if (!IntersectSphere(cCollision.pInt, -obj->velocity.WithLength(1.0),
			 obj->position, obj->size, sInt))
      return NOCOLLISION;

    BDPlane splane((obj->position - cCollision.pInt).WithLength(1.0), cCollision.pInt);
    cCollision.Plane = splane;

    if (splane.DistanceToPlane(sInt + (obj->velocity * deltaTime)) >= 0.0)
      return NOCOLLISION;

    if (!obj->Collision(&cCollision))
      return NOCOLLISION;

    return SPLANE;
  }

  // Finds ray intersection with tri from FRONT
  Vector3 RayIntersection(Vector3 pos, Vector3 dir) {
    // Vector3 p = Plane.RayIntersection(pos, dir);
    // Code from Plane.RayIntersection. Don't call function because distance is needed
    double d = Plane.DistanceToPlane(pos);
    if (d < 0.0)
      return pos;

    double a = Plane.N * dir;
    if (a == 0)
      return pos;     // ray is parallel to plane

    Vector3 p = pos - dir * (d / a);
    Vector3 m = p - pos;

    if (dir.x != 0.0) {
      if ((dir.x < 0.0) != (m.x < 0.0))
	return pos;
    } else if (dir.y != 0.0) {
      if ((dir.y < 0.0) != (m.y < 0.0))
	return pos;
    } else {
      if ((dir.z < 0.0) != (m.z < 0.0))
	return pos;
    }

    // Compare with 0.001 to avoid floating point round-off errors
    if (plab.DistanceToPlane(p) > 0.001)
      return pos;

    if (plbc.DistanceToPlane(p) > 0.001)
      return pos;

    if (plca.DistanceToPlane(p) > 0.001)
      return pos;

    return p;
  }
};
//---------------------------------------------------------------------------
#endif

