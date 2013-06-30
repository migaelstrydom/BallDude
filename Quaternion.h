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
#ifndef QuaternionH
#define QuaternionH
//---------------------------------------------------------------------------
#include <cmath>
#include "Vector.h"
//---------------------------------------------------------------------------
class BDQuaternion {
 public:
  Vector3 v;
  double s;

  BDQuaternion() : s(0.0) {}

  BDQuaternion(double x, double y, double z, double w)
    : v(x, y, z), s(w) {
  }

  BDQuaternion(const Vector3 &a, double t) {
    t *= 0.5;
    v = a*sin(t);
    s = cos(t);
  }

  const BDQuaternion operator*(const BDQuaternion &q) const {
    Vector3 r = v ^ q.v + q.s*v + s*q.v;

    return BDQuaternion(s*q.v.x + q.s*v.x + v.y*q.v.z - v.z*q.v.y,
                        s*q.v.y + q.s*v.y + v.z*q.v.x - v.x*q.v.z,
                        s*q.v.z + q.s*v.z + v.x*q.v.y - v.y*q.v.x,
                        s*q.s - v * q.v);
  }

  // post multiply by scalar
  const BDQuaternion operator*(const double &c) const {
    return BDQuaternion(v.x*c, v.y*c, v.z*c, s*c);
  }

  // pre multiply by scalar
  friend inline const BDQuaternion operator*(const double &c, const BDQuaternion &qat) {
    return qat*c;
  }

  // divide by scalar
  const BDQuaternion operator/(double c) const {
    c = 1/c;

    return BDQuaternion(v.x*c, v.y*c, v.z*c, s*c);
  }

  double Norm() const {
    return v.SquaredLength() + s*s;
  }

  const BDQuaternion Conjugate() const {
    return BDQuaternion(-v.x, -v.y, -v.z, s);
  }

  const BDQuaternion Inverse() const {
    return Conjugate()/Norm();
  }

  // only works for unit quaternions
  void ToMatrix(double *m) {
    m[0] = 1.0 - 2.0*(v.y*v.y + v.z*v.z);
    m[1] = 2.0*(v.x*v.y + s*v.z);
    m[2] = 2.0*(v.x*v.z - s*v.y);
    m[3] = 0.0;

    m[4] = 2.0*(v.x*v.y - s*v.z);
    m[5] = 1.0 - 2.0*(v.x*v.x + v.z*v.z);
    m[6] = 2.0*(v.y*v.z + s*v.x);
    m[7] = 0.0;

    m[8] = 2.0*(s*v.y + v.x*v.z);
    m[9] = 2.0*(v.y*v.z - s*v.x);
    m[10] = 1.0 - 2.0*(v.x*v.x + v.y*v.y);
    m[11] = 0.0;

    m[12] = 0.0;
    m[13] = 0.0;
    m[14] = 0.0;
    m[15] = 1.0;
  }
};
//---------------------------------------------------------------------------
#endif
