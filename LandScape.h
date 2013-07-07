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
#ifndef LandScapeH
#define LandScapeH
//---------------------------------------------------------------------------
#include "DrawableObject.h"
#include "Texture.h"
#include "Plane.h"
#include "LoadDataUnit.h"
#include "Triangle.h"

// TEMP
//#include "AABB.h"
//---------------------------------------------------------------------------
class BDWaterSurface;
class BDLandScape : public DrawableObject {
 private:
  // unsigned int Dlist;

  unsigned texID;

  Triangle *tris;
  unsigned int numtris;
  // unsigned int numtextris;      // The number of tris with textures
  // unsigned int textriindex;     // position at which tex'ed tris start in array

  // Vertex Arrays
  Vector3 *verts, *cols;//, *norms;
  unsigned int *indices;
  unsigned int numverts;
  float *texcos;  // tex coordinates
  bool bTexture;

  // TEMP
  // BDAABB drawbox;
  // std::vector<int> nl;  // list of nodes bd is in
  // END TEMP

  void OnDraw(BDCamera*);
  void CollisionLine(int, int, int, int, bool*, const int);
  // void SaveVertData();

 public:
  // BDLandScape();
  //~BDLandScape();

  void LoadLandScape(BDFile* File, unsigned);
  void Unload();
  void OnCollision(DrawableObject*, float);

  void WaterCollision(BDWaterSurface*);
};
//---------------------------------------------------------------------------
#endif
