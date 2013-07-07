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
#ifndef WaterRefractH
#define WaterRefractH
//---------------------------------------------------------------------------
#include "DrawableObject.h"
#include "Vector.h"
#include "Texture.h"
#include "WaterSurface.h"
//---------------------------------------------------------------------------
class BDWaterRefract : public BDWaterSurface {
 private:
  struct TexCoord
  {
    float x, y;
  };

  TexCoord *tc;

  const float DEPTH;
  const float WALLINVDEPTH;

  BDPlane fPlane, lPlane, rPlane, tPlane, bPlane;

  unsigned int texID2; // Reflection tex ID

  virtual void OnDraw(BDCamera*);
  void SetupTexture();
  void UnsetupTexture();

 public:
  BDWaterRefract(Vector3, const float, const float, int, int, const int);
  ~BDWaterRefract();

  friend class BDLandScape;
};
//---------------------------------------------------------------------------
#endif
