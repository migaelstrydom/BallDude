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
#ifndef PlaneH
#define PlaneH
//---------------------------------------------------------------------------
#include "Vector.h"
//---------------------------------------------------------------------------
class BDPlane {
 public:
  Vector3 N;      // plane normal
  double  D;      // plane shift constant

  // constructors

  // Ax + By + Cz - D = 0
  BDPlane(double a = 1, double b = 0, double c = 0, double d = 0) {
    N = Vector3(a, b, c);
    D = d;
  }

  // instantiate a plane with normal Normal and D = d
  BDPlane(const Vector3& normal, double d = 0) {
    N = normal;
    D = d;
  }

  // instantiate a copy of Plane
  BDPlane(const BDPlane& plane) {
    N = plane.N;
    D = plane.D;
  }

  // instantiate a plane with three points
  BDPlane(const Vector3& vertexA, const Vector3& vertexB, const Vector3& vertexC) {
    Vector3 normalA((vertexC - vertexA).WithLength(1.0));  // unit normal of C - A
    Vector3 normalB((vertexC - vertexB).WithLength(1.0));  // unit normal of C - B

    N = (normalA ^ normalB).WithLength(1.0);    // normalize cross product
    D = -vertexA * N;                 // calculate distance
  }

  // instantiate a plane with a normal and a point
  BDPlane(const Vector3& norm, const Vector3& point)
    : N(norm) {
    D = -N * point;
  }

  // assignment operator
  const BDPlane& operator=(const BDPlane& plane) {
    N = plane.N;
    D = plane.D;

    return *this;
  }

  // equality operator
  const bool operator==(const BDPlane& plane) const {
    return N == plane.N && D == plane.D;
  }

  // inequality operator
  const bool operator!=(const BDPlane& plane) const {
    return !(*this == plane);
  }

  // is point on this plane?
  const bool PointOnPlane(const Vector3& point) const {
    return DistanceToPlane(point) == 0;
  }

  // return the distance of point to the plane
  const double DistanceToPlane(const Vector3& point) const {
    return N * point + D;
  }

  // return the intersection point of the ray to this plane
  const Vector3 RayIntersection(const Vector3& rayPos, const Vector3& rayDir) const {
    const double a = N * rayDir;
    if (a == 0)
      return rayPos;     // ray is parallel to plane

    return rayPos - rayDir * (DistanceToPlane(rayPos) / a);
  }

  // are A, B, and C counterclockwise when you look from P?
  // ignore special cases
  static bool ccw(const Vector3& A, const Vector3 &B, const Vector3 &C, const Vector3 &P) {
    Vector3 c = B - A;
    Vector3 b = C - A;
    Vector3 n = c ^ b;
    double d = -n * A;

    return (d + n*P) > 0.0;
  }
};
//---------------------------------------------------------------------------
#endif
//---------------------------------------------------------------------------
