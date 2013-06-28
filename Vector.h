/************************************************************************
 * Copyright 2013 Migael Strydom, Timothy Stranex
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
/************************************************************************
 *
 * Vector.h
 * Vector class and implementation
 *
 * 2006-08-30  Migael Strydom    Created
 * 2006-09-02  Timothy Stranex   Added << operator
 * 2006-09-10  Timothy Stranex   Added post divide by scalar
 * 2006-09-14  Migael Strydom    Added SquaredLength
 * 2013-06-28  Migael Strydom    Converted this to a 3-component vector
 *                               class.
 *
 ************************************************************************/

#ifndef VECTOR_H
#define VECTOR_H

#include <cmath>
#include <ostream>

#define DEG2RAD(a)	(M_PI/180*(a))
#define RAD2DEG(a)	(180/M_PI*(a))

typedef double vectype;

class Vector3
{
 public:
  vectype x, y, z;

  //constructor
  Vector3(vectype px = 0, vectype py = 0, vectype pz = 0)
    : x(px), y(py), z(pz) {
  }

  // vector index
  vectype &operator[](const long idx) {
    return *((&x)+idx);
  }

  // vector assignment
  const Vector3 &operator=(const Vector3 &vec) {
    x = vec.x;
    y = vec.y;
    z = vec.z;

    return *this;
  }

  // vector equality
  const bool operator==(const Vector3 &vec) const {
    return ((x == vec.x) && (y == vec.y) && (z == vec.z));
  }

  // vector inequality
  const bool operator!=(const Vector3 &vec) const {
    return !(*this == vec);
  }

  // vector add
  const Vector3 operator+(const Vector3 &vec) const {
    return Vector3(x + vec.x, y + vec.y, z + vec.z);
  }

  // vector increment
  const Vector3& operator+=(const Vector3& vec) {
    x += vec.x;
    y += vec.y;
    z += vec.z;
    return *this;
  }

  // vector subtraction
  const Vector3 operator-(const Vector3& vec) const {
    return Vector3(x - vec.x, y - vec.y, z - vec.z);
  }

  // vector negation
  const Vector3 operator-() const {
    return Vector3(-x, -y, -z);
  }

  // vector decrement
  const Vector3 &operator-=(const Vector3& vec) {
    x -= vec.x;
    y -= vec.y;
    z -= vec.z;

    return *this;
  }

  // scalar self-multiply
  const Vector3 &operator*=(const vectype &s) {
    x *= s;
    y *= s;
    z *= s;

    return *this;
  }

  // scalar self-divide
  const Vector3 &operator/=(const vectype &s) {
    const vectype recip = 1/s; // faster to divide only once

    x *= recip;
    y *= recip;
    z *= recip;

    return *this;
  }

  // post multiply by scalar
  const Vector3 operator*(const vectype &s) const {
    return Vector3(x*s, y*s, z*s);
  }

  // pre multiply by scalar
  friend inline const Vector3 operator*(const vectype &s, const Vector3 &vec) {
    return vec*s;
  }

  // post divide by a scalar
  const Vector3 operator/(const vectype &s) const {
    const vectype recip = 1/s; // faster to divide only once
    return Vector3(x*recip, y*recip, z*recip);
  }

  //dot vectors
  const vectype operator*(const Vector3& vec) const {
    return x*vec.x + y*vec.y + z*vec.z;
  }

  //cross vectors
  const Vector3 operator^(const Vector3& vec) const {
    return Vector3(y*vec.z - z*vec.y, z*vec.x - x*vec.z, x*vec.y - y*vec.x);
  }

  //returns the length of the vector
  const vectype length() const {
    return std::sqrt((*this)*(*this));
  }

  // returns the length of the vector squared
  // this is useful for comparisons because expensive square root
  // needn't be calculated
  const vectype SquaredLength() const {
    return (*this)*(*this);
  }

  const Vector3 WithLength(const vectype &new_length) const {
    return (*this)*(new_length/this->length());
  }

  const Vector3 &SetLength(const vectype &new_length) {
    (*this) *= (new_length/this->length());
    return *this;
  }

  const double Angle(const Vector3& normal) const {
    return acos(*this * normal);
  }

  /**
   * Reflects the vector about normal.
   */
  const Vector3 Reflection(const Vector3& normal) const {
    const Vector3 vec(this->WithLength(1.0));     // normalize this vector
    return (vec - normal * 2.0 * (vec * normal)) * this->length();
  }

  /**
   * Rotate "angle" radians about "normal".
   */
  const Vector3 Rotate(const double angle, const Vector3& normal) const	{
    const double cosine = cos(angle);
    const double sine = sin(angle);

    return Vector3((*this) * cosine + (*this ^ normal) * sine +
                   ((normal * *this) * (1.0f - cosine)) * normal);
  }


  /**
   * Print the vector to the stream in the form <x,y,z>.
   */
  friend std::ostream& operator<<(std::ostream &os, const Vector3 &v) {
    os << "<" << v.x << "," << v.y << "," << v.z << "," << ">";
    return os;
  }
};

#endif
