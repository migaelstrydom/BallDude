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
#include "TreeUnit.h"
#include "OpenGLUnit.h"
#include <vector>
#include <algorithm>
//#include <iostream>
#include <cmath>
//---------------------------------------------------------------------------
#define SEGMENTS 10
//---------------------------------------------------------------------------
using namespace std;

struct CStem {
  vector<CStem*> children;
  CStem* parent;
  Vector3 start, szaxis, ezaxis;   // zaxis must be normalized.
  double length, fullLength;
  double radius;
  int CurveRes;
  double Curve;
  Vector3 CurveNormal;    // must be perpendicular to zaxis
  double SegSplits;
  int level;

  CStem(Vector3 s, Vector3 sz, double c, Vector3 cn, int cr)
      : parent(NULL), start(s), szaxis(sz), CurveRes(cr), Curve(c), CurveNormal(cn) {
    ezaxis = szaxis.Rotate(Curve, CurveNormal);
    level = 0;
  }

  void fillPoints(vector<Vector3> &points, vector<Vector3> &normals, vector<int> &indices) {
    int pointsstart = points.size();
    Vector3 center = start;
    Vector3 sor;
    Vector3 as, bs;
    double sr = radius;
    sor = szaxis;
    as = CurveNormal;
    bs = sor ^ as;

    for (int i = 0; i < SEGMENTS; i++) {
      double ct = cos(double(i)/SEGMENTS * 2 * M_PI);
      double st = sin(double(i)/SEGMENTS * 2 * M_PI);

      points.push_back(center + as*ct*sr + bs*st*sr);
      normals.push_back(as*ct + bs*st);
    }

    double splits = SegSplits;

    for (int j = 0; j < CurveRes; j++) {
      sor = sor.Rotate(Curve/CurveRes, CurveNormal);
      bs = sor ^ as;
      center += sor*(length/CurveRes);
      sr = radius * (1.0 - j*length/(fullLength*CurveRes));
      splits += SegSplits;

      for (int i = 0; i < SEGMENTS; i++) {
        double ct = cos(double(i)/SEGMENTS * 2 * M_PI);
        double st = sin(double(i)/SEGMENTS * 2 * M_PI);

        points.push_back(center + as*ct*sr + bs*st*sr);
        normals.push_back(as*ct + bs*st);
      }

      while(level > 0 && splits >= 1.0) {
        double theta = split(center, sor, sr*0.8, CurveRes - j - 1);
        splits -= 1.0;
        sor = sor.Rotate(theta - Curve/CurveRes, CurveNormal);
      }
    }

    for (int j = 0; j < CurveRes; j++) {
      for (int i = 0; i < SEGMENTS; i++) {
        indices.push_back(pointsstart + i);
        indices.push_back(pointsstart + ((i + 1) % SEGMENTS));
        indices.push_back(pointsstart + SEGMENTS + i);

        indices.push_back(pointsstart + ((i + 1) % SEGMENTS));
        indices.push_back(pointsstart + SEGMENTS + ((i + 1) % SEGMENTS));
        indices.push_back(pointsstart + SEGMENTS + i);
      }
      pointsstart += SEGMENTS;
    }
    // cout << children.size() << endl;
    for (int i = 0; i < (int)children.size(); i++) {
      children[i]->fillPoints(points, normals, indices);
    }
  }

  double split(Vector3 pos, Vector3 ori, double rad, int segleft) {
    CurveNormal = CurveNormal.Rotate(double(rand() % 1000)/(1000.0)*2.0*M_PI, ori);
    level--;
    double theta = 0.4;
    CStem *cl = new CStem(pos, ori.Rotate(-theta, CurveNormal), Curve, -CurveNormal, segleft);
    cl->fullLength = segleft*fullLength/CurveRes;
    cl->length = segleft*length/CurveRes;
    cl->radius = rad;
    cl->SegSplits = SegSplits;
    cl->level = level;
    children.push_back(cl);
    return theta;
  }
};
//---------------------------------------------------------------------------
BDTree::BDTree() {
  vector<Vector3> p, n;
  vector<int> ind;

  CStem stem(Vector3(0.0, 0.0, 0.0), Vector3(0.0, 1.0, 0.0), 0.75, Vector3(1.0, 0.0, 0.0), 50);
  stem.length = 10.0;
  stem.fullLength = 10.0;
  stem.radius = 0.5;
  stem.SegSplits = 0.1;
  stem.level = 5;

  stem.fillPoints(p, n, ind);

  Vector3 *points = new Vector3[p.size()];
  for (int i = 0; i < (int)p.size(); i++) {
    points[i] = p[i];
  }

  normals = new Vector3[p.size()];
  for (int i = 0; i < p.size(); i++) {
    normals[i] = n[i];
  }

  unsigned *indices = new unsigned int[ind.size()];

  for (int i = 0; i < (int)ind.size(); i++) {
    indices[i] = ind[i];
  }

  treeModel = new BDBDMModel();
  treeModel->SetVerts(points, p.size());
  treeModel->SetIndices(indices, ind.size()/3);
  // std::cout << p.size() << std::endl;
  // Shadow
  shadow = new BDShadow();
  shadow->MakeGraph(treeModel);
  shadow->CalcShadow(Vector3(329.983, 942.809, 47.1405));
}
//---------------------------------------------------------------------------
BDTree::~BDTree() {
  delete treeModel;
  delete shadow;
  delete[] normals;
}
//---------------------------------------------------------------------------
void BDTree::OnAnimate(double deltaTime) {
}
//---------------------------------------------------------------------------
void BDTree::OnDraw(BDCamera* Cam) {
  shadow->position = position;
  glTranslated(position.x, position.y, position.z);
  glEnableClientState(GL_NORMAL_ARRAY);
  glNormalPointer(GL_DOUBLE, 0, normals);

  glColor3ub(165, 48, 0);

  treeModel->StaticDraw();
  glDisableClientState(GL_NORMAL_ARRAY);

  // glCallList(1);
}
//---------------------------------------------------------------------------
void BDTree::OnCollision(DrawableObject* Object, float) {
}
//---------------------------------------------------------------------------
bool BDTree::Collision(BDCollision* col) {
  col->ColType = ObjType;

  return false;
}
//---------------------------------------------------------------------------
void BDTree::OnPrepare(float deltaTime) {
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
