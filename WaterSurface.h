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
#ifndef WaterSurfaceH
#define WaterSurfaceH
//---------------------------------------------------------------------------
#include "DrawableObject.h"
#include "Vector.h"
//---------------------------------------------------------------------------
class BDWaterSurface : public DrawableObject {
 protected:
  Vector3 *z, *z1;
  bool *d;  // Waves should stop at obstacle, false = obstacle
  Vector3 *n; //<-- Normals
  int *indices; // index values to the order the tri's should be drawn
  const float fAmp;
  const float h; //<-- The cell width
  const float fPosDiv; // What to multiply by to get relx and rely
  const int MAXSEG;

  bool bDrawn;  //<-- If not drawn, don't calculate animations!!

  unsigned int texID;
  char *texData;

  virtual void OnAnimate(double deltaTime);
  virtual void OnDraw(BDCamera*);
  virtual void OnCollision(DrawableObject*, float);
  virtual void DrawLowDetail();

 public:
  BDWaterSurface(Vector3, const float, int, const int);
  ~BDWaterSurface();

  friend class BDLandScape;
};
//---------------------------------------------------------------------------
/*class BDWaterSplash : public BDParticleSystem
  {
  private:
  public:
  };       */
//---------------------------------------------------------------------------
#endif
