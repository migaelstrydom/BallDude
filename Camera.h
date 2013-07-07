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
#ifndef CameraH
#define CameraH
//---------------------------------------------------------------------------
#include <GL/gl.h>
#include <GL/glu.h>
#include <queue>

#include "Interpolate.h"
#include "Vector.h"
#include "Plane.h"
#include "BDShadow.h"
//---------------------------------------------------------------------------
class BDPlayer;
class BDInput;
class BDAABB;
class DrawableObject;
//---------------------------------------------------------------------------
// for comparing the distances of alpha objects from camera
class CmpAlphaObject {
 public:
  DrawableObject* ptr;
  float distfromcam;

CmpAlphaObject(DrawableObject* p, float d)
    : ptr(p), distfromcam(d) {
  }

  bool operator<(const CmpAlphaObject& c) const
  {
    return distfromcam < c.distfromcam;
  }
};
//---------------------------------------------------------------------------
class BDCamera {       // also handles organisation of alpha object for drawing at end
 private:
  BDPlayer *Player;
  BDInput *Input;
  float FollowSpeed;
  float DistFromPlayer;
  float DispFactor;      // this needs to be multiplied by the displacement the
  // camera needs to make to get the speed optimal

  float MinZVal, MaxZVal;
  Vector3 Speed; // The Speed at which the camera moves
  float ZoomOutGrad; // Used to determine the height of the camera
  float xmovement, ymovement;
  float timeleft;    // to add smoother mouse movement

  Vector3 LookAt;  // Point at which camera looks

  // Independent bezier curve movement
  bool IndependentMovement;
  void AnimateMovement(double);

  // Point in frustum test
  float dist;  // dist from near plane of last object tested
  BDPlane fRight, fLeft, fTop, fBottom, fFar, fNear;
  void CalcFrustum();

  // Handling of alpha objects
  std::priority_queue<CmpAlphaObject> alphas;

 public:
  enum InFrustum {IFNONE, IFSOME, IFALL};

  Vector3 Position;   // Contains absolute coordinates
  Vector3 Velocity; // Velocity
  DrawableObject *CamObject;       // for collision detection

  bool bDrawAlpha;          // if true then draw alphas

  // interpolation
  BDInterpolate Movement;

  BDCamera(BDPlayer*);
  ~BDCamera();

  void Animate(double deltaTime);

  void Draw();  // Position the camera (equivalent to draw objects)
  void DrawAlphaObjects(std::vector<BDShadow*> &);

  bool SphereInFrustum(Vector3, float);

  float GetDist() {return dist;} // Returns dist
  Vector3 GetDir() {return fNear.N;}
  float CalcDist(Vector3 pos) {return fNear.DistanceToPlane(pos);}

  InFrustum BoxInFrustum(BDAABB);

  void SetPlayer(BDPlayer*);

  inline Vector3 GetSpeed() {return Speed;}   // Set the velocity
  void SetInput(BDInput *i) {Input = i;}

  void MoveBezier();

  void AddAlphaObject(DrawableObject* obj) {
    alphas.push(CmpAlphaObject(obj, dist));
  }
};
//---------------------------------------------------------------------------
#endif
