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
#ifndef DRAWABLEOBJECT_H_
#define DRAWABLEOBJECT_H_
//---------------------------------------------------------------------------
#include <vector>
#include "Node.h"
#include "Vector.h"
#include "Camera.h"
#include "CollisionObj.h"
#include "BDShadow.h"
//---------------------------------------------------------------------------
class DrawableObject : public BDNode {
 protected:
  bool bDelete;

  // perform basic physics on the object
  virtual void OnAnimate(double deltaTime) {
    velocity += acceleration * deltaTime;
    position += velocity * deltaTime;
  }

  // draw the object given the camera position
  virtual void OnDraw(BDCamera*) {}

  // collide with other objects
  virtual void OnCollision(DrawableObject *, float) {}

  // perform collision and other preparations for this object
  virtual void OnPrepare(float deltaTime) {}

 public:
  Vector3 position;             // object position
  Vector3 velocity;             // velocity of object
  Vector3 acceleration;         // acceleration
  float size;                   // size of bounding sphere (radius)
  enum {OBJ_TRIANGLE = 0,
        OBJ_OBJECT = 1,
        OBJ_MOVINGTRIANGLE = 2,
        OBJ_WATER,
        OBJ_FIRE,
        OBJ_FARTCLOUD,
        OBJ_EXPLOSION,
        OBJ_ENEMY,
        OBJ_BULLET} ObjType;

  // ALPHA OBJECTS' VARIABLES
  bool bAlpha;        // if true, be one of the last object to get drawn
  // END ALPHA OBJECTS' VARIABLES

  // SHADOW VARIABLES
  BDShadow *shadow;
  // END SHADOW VARIABLES

  DrawableObject();

  // draw object
  void Draw(BDCamera*, std::vector<BDShadow*> &);

  // animate object
  void Animate(double);

  // perform collision detection
  void PerformCollisionDetection(DrawableObject*, float);

  // prepare object
  void Prepare(float);

  // object Collided
  // returns true if it wants caller to handle (default) collision
  // BDCollision is updated to reflect properties of this object
  virtual bool Collision(BDCollision*);

  // Quick and simple sphere collision test
  inline bool SphereTest(DrawableObject*, float);
};
//---------------------------------------------------------------------------
#endif  // DRAWABLEOBJECT_H_
