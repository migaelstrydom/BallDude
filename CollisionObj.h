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
#ifndef COLLISIONOBJ_H_
#define COLLISIONOBJ_H_
//---------------------------------------------------------------------------
class Triangle;
class Vector3;
class BDPlane;
//---------------------------------------------------------------------------
class BDCollision {
 public:
  // Plane collision
  // triangle that makes up plane
  Triangle *triangle;

  // Plane
  BDPlane Plane;

  // point of intersection
  Vector3 pInt;

  // Sphere collision
  // It's radius
  float Size;

  // Position of the sphere
  Vector3 Position;

  int ColType;

  BDCollision() {
  }

  BDCollision(Triangle *tri, BDPlane p, Vector3 i)
      :triangle(tri), Plane(p), pInt(i) {
    ColType = 0;   // Triangle
  }

  BDCollision(Vector3 p, float sz)
      :Size(sz), Position(p) {
    ColType = 1;   // Object
  }
};
//---------------------------------------------------------------------------
#endif  // COLLISIONOBJ_H_
