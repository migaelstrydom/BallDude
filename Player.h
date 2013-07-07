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
#ifndef PlayerH
#define PlayerH
//---------------------------------------------------------------------------
#include "DrawableObject.h"
#include "InputUnit.h"
#include "Texture.h"
#include "LoadDataUnit.h"
#include "CollisionObj.h"
#include "FartCloudUnit.h"
#include "ResourceControlUnit.h"
//---------------------------------------------------------------------------
class BDPlayerAnimate {
 private:
  float breatheCounter;
  float breatheTime;
  float inhaleMax;

  float blinkTime;

  float EyeBlinkTime;
  float EyeBlinkLength;

  bool Jump;
  void AnimateJump(float);

 public:
  bool Blink;          // Time to blink!
  double ScaleX, ScaleY; // Scale values
  float NoseScale;

  BDPlayerAnimate(BDFile *File);

  void Update(float);
  void StartJump();
};
//---------------------------------------------------------------------------
class BDPlayerHealthBar {
 private:
  float fHealth;
  const float cfMaxHealth;
  float *p, *p2;
  const int NumPoints;

 public:
  BDPlayerHealthBar(const float mh)
    : fHealth(mh), cfMaxHealth(mh), NumPoints(63) {
    p = new float[NumPoints];
    p2 = new float[NumPoints];
    p[NumPoints/2] = 1.0f;
    /*for (int i = 4*NumPoints/9; i <= 5*NumPoints/9; i++) {
    // p[i] = (i%(NumPoints/5)) ? 1.0 : -1.0;
    p[i] = 1;
    }    */
  }

  ~BDPlayerHealthBar() {
    delete[] p;
    delete[] p2;
  }

  // returns true if dead
  bool AddHealth(float dh) {
    fHealth += dh;
    return fHealth < 0.0f;
  }

  void Draw();

  void Animate(double);
};
//---------------------------------------------------------------------------
class BDPlayer : public DrawableObject {
 private:
  float WalkSpeed;
  float AirMoveSpeed;
  float JumpSpeed;
  float FallSpeed;

  bool OnGround;                // Used to know when he can jump
  bool InWater;
  bool WaterSurface;  // Used to know when he can jump in water

  Vector3 direction;               // Unit vector to show direction BD is facing : Used to pass to Bullet

  int Dlist;

  // BDTexture *EyeTex;
  unsigned int EyeTexID;

  // FMOD::Sound *JumpSound;

  // BULLETS!!
  // FMOD::Sound *ShotSound;
  unsigned int BulletTexID;
  BDFile* BulletFile;

  unsigned int ExplosionTexID;

  // FARTCLOUD
  // FMOD::Sound *FlatoSound;
  // FMOD::Sound *BubbleSound;
  float FCKickBack;  // magnitude of velocity of BallDude after flato
  float FCTimeLeft;  // time before next flato can be emitted

  virtual void OnAnimate(double deltaTime);
  virtual void OnDraw(BDCamera*);
  virtual void OnPrepare(float deltaTime);

  // Textures
  void CreateEyeTex();
  void CompileDList();

  // Animations
  BDPlayerAnimate *Animate;

 public:
  float TurnDegree;  // Public to control it from scripts
  Vector3 *CamPos;
  BDFartCloudParam FartCloudParam;
  // Health
  BDPlayerHealthBar HealthBar;

  BDPlayer(BDResourceControl*);
  ~BDPlayer();

  // Collided with something
  bool Collision(BDCollision*);

  void CheckInput(BDInput*);
};
//---------------------------------------------------------------------------
#endif
