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
#ifndef BDShadowH
#define BDShadowH
//---------------------------------------------------------------------------
#include "Vector.h"
#include "BDMModelUnit.h"
#include <vector>
//---------------------------------------------------------------------------
struct CEdge {
  unsigned a, b;

  CEdge(unsigned ba, unsigned bb)
  : a(ba), b(bb) {
  }

  std::vector<unsigned> triangles;
};
//---------------------------------------------------------------------------
class BDShadow {
 private:
  BDBDMModel* shadow;
  BDBDMModel* caster;
  std::vector<CEdge> edges;

 public:
  Vector3 position;
  Vector3 lightSource;

  BDShadow();
  ~BDShadow();

  void MakeGraph(BDBDMModel*);
  void CalcShadow(Vector3);

  // sphere radius, light position
  void InitSphere();
  void SphereShadow(double, Vector3);

  BDBDMModel* GetModel() {return shadow; }
  void SetModel(BDBDMModel *m) {shadow = m;}
};
//---------------------------------------------------------------------------
#endif
