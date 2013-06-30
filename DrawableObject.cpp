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
#include "DrawableObject.h"
//---------------------------------------------------------------------------
DrawableObject::DrawableObject()
    : bDelete(false), bAlpha(false), shadow(NULL) {
}
//---------------------------------------------------------------------------
void DrawableObject::Draw(BDCamera* camera, std::vector<BDShadow*> &shadows) {
  if ((!bAlpha) || (camera->bDrawAlpha)) {
    if (shadow != NULL)
      shadows.push_back(shadow);

    glPushMatrix();
    OnDraw(camera);             // draw this object
    if (HasChild())             // draw children
      static_cast<DrawableObject*>(child->next)->Draw(camera, shadows);
    glPopMatrix();

    if (camera->bDrawAlpha)
      return;
  } else if (camera->SphereInFrustum(position, size)) {
    camera->AddAlphaObject(this);
    if (shadow != NULL)
      shadows.push_back(shadow);
  }

  // draw siblings
  if (HasParent() && !IsFirstChild())
    static_cast<DrawableObject*>(next)->Draw(camera, shadows);
}
//---------------------------------------------------------------------------
void DrawableObject::Animate(double deltaTime) {
  OnAnimate(deltaTime);         // animate this object

  // animate children
  if (HasChild())
    static_cast<DrawableObject*>(child->next)->Animate(deltaTime);

  // animate siblings
  if (HasParent() && !IsFirstChild())
    static_cast<DrawableObject*>(next)->Animate(deltaTime);

  if (bDelete) {
    delete this;
    return;
  }
}
//---------------------------------------------------------------------------
// perform collision detection
void DrawableObject::PerformCollisionDetection(DrawableObject *obj, float deltaTime) {
  if (!HasParent()) {
    OnCollision(obj, deltaTime);
    return;
  }

  // if this object's bounding sphere collides with obj's sphere
  if ((obj != ((DrawableObject*)this)) && SphereTest(obj, deltaTime)) {
    OnCollision(obj, deltaTime);
  }

  // test sibling collisions with obj
  if (!IsFirstChild()) {
    static_cast<DrawableObject*>(next)->PerformCollisionDetection(obj, deltaTime);
  } else {
    static_cast<DrawableObject*>(parent)->PerformCollisionDetection(obj, deltaTime);
  }
}
//---------------------------------------------------------------------------
// prepare object
void DrawableObject::Prepare(float deltaTime) {
  OnPrepare(deltaTime);

  if (HasChild())
    static_cast<DrawableObject*>(child->next)->Prepare(deltaTime);

  if (HasParent() && !IsFirstChild())
    static_cast<DrawableObject*>(next)->Prepare(deltaTime);
}
//---------------------------------------------------------------------------
// object Collided
// returns true if it wants caller to handle (default) collision
// BDCollision is updated to reflect properties of this object
bool DrawableObject::Collision(BDCollision*) {
  return true;
}
//---------------------------------------------------------------------------
// Sphere collision test
bool DrawableObject::SphereTest(DrawableObject* Object, float deltaTime) {
  // Relative position
  Vector3 dp = Object->position - position;
  // Minimal distance
  float r = size + Object->size;
  // dP^2-r^2
  float pp = dp.x * dp.x + dp.y * dp.y + dp.z * dp.z - r*r;

  return (pp < 0);
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
