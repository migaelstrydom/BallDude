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
#include "OctTree.h"
#include <cstring>  // memset
//---------------------------------------------------------------------------
namespace BDOctTree {
  std::vector<BDOctNode> Nodes;

  Triangle *tris;

  unsigned int MinTris;

  double MinNodeSize;

  BDOctNode CreateSubNodes(BDOctNode);
  void DetermineNeighbours();
  int AreNeighbours(unsigned int, unsigned int);

  Vector3 RayTrace(Vector3, Vector3);
  Vector3 TraceThroughNode(int, Vector3, Vector3, std::vector<Triangle*>&);
  Triangle *TracedTri; // The tri with which ray collided. NULL if none

  inline bool PointInBox(float x1, float x2, float y1, float y2,
                         float z1, float z2, float px, float py,
                         float pz);
};
//---------------------------------------------------------------------------
void BDOctTree::CreateOctTree(Triangle *t, const int numtris) {
  tris = t;
  Nodes.push_back(BDOctNode());

  Nodes[0].x1 = Nodes[0].x2 = tris[0].a->x;
  Nodes[0].y1 = Nodes[0].y2 = tris[0].a->y;
  Nodes[0].z1 = Nodes[0].z2 = tris[0].a->z;

  Triangle *lastTri = &tris[numtris];
  for (Triangle *index = tris; index != lastTri; index++) {
    if (index->a->x < Nodes[0].x1)
      Nodes[0].x1 = index->a->x;
    if (index->b->x < Nodes[0].x1)
      Nodes[0].x1 = index->b->x;
    if (index->c->x < Nodes[0].x1)
      Nodes[0].x1 = index->c->x;

    if (index->a->x > Nodes[0].x2)
      Nodes[0].x2 = index->a->x;
    if (index->b->x > Nodes[0].x2)
      Nodes[0].x2 = index->b->x;
    if (index->c->x > Nodes[0].x2)
      Nodes[0].x2 = index->c->x;


    if (index->a->y < Nodes[0].y1)
      Nodes[0].y1 = index->a->y;
    if (index->b->y < Nodes[0].y1)
      Nodes[0].y1 = index->b->y;
    if (index->c->y < Nodes[0].y1)
      Nodes[0].y1 = index->c->y;

    if (index->a->y > Nodes[0].y2)
      Nodes[0].y2 = index->a->y;
    if (index->b->y > Nodes[0].y2)
      Nodes[0].y2 = index->b->y;
    if (index->c->y > Nodes[0].y2)
      Nodes[0].y2 = index->c->y;


    if (index->a->z < Nodes[0].z1)
      Nodes[0].z1 = index->a->z;
    if (index->b->z < Nodes[0].z1)
      Nodes[0].z1 = index->b->z;
    if (index->c->z < Nodes[0].z1)
      Nodes[0].z1 = index->c->z;

    if (index->a->z > Nodes[0].z2)
      Nodes[0].z2 = index->a->z;
    if (index->b->z > Nodes[0].z2)
      Nodes[0].z2 = index->b->z;
    if (index->c->z > Nodes[0].z2)
      Nodes[0].z2 = index->c->z;

    Nodes[0].tris.push_back(index);
  }

  float rootsize = Nodes[0].x2 - Nodes[0].x1;
  if (rootsize < Nodes[0].y2 - Nodes[0].y1)
    rootsize = Nodes[0].y2 - Nodes[0].y1;
  if (rootsize < Nodes[0].z2 - Nodes[0].z1)
    rootsize = Nodes[0].z2 - Nodes[0].z1;

  rootsize *= 0.5;
  float mid = (Nodes[0].x2 + Nodes[0].x1);
  BDOctTree::Nodes[0].x1 = mid * 0.5 - rootsize;
  BDOctTree::Nodes[0].x2 = mid * 0.5 + rootsize;
  mid = (Nodes[0].y2 + Nodes[0].y1);
  BDOctTree::Nodes[0].y1 = mid * 0.5 - rootsize;
  BDOctTree::Nodes[0].y2 = mid * 0.5 + rootsize;
  mid = (Nodes[0].z2 + Nodes[0].z1);
  BDOctTree::Nodes[0].z1 = mid * 0.5 - rootsize;
  BDOctTree::Nodes[0].z2 = mid * 0.5 + rootsize;

  // Calculate size
  // Nodes[0].size = sqrt(12*rootsize*rootsize);

  Nodes[0] = CreateSubNodes(Nodes[0]);
  DetermineNeighbours();
}
//---------------------------------------------------------------------------
void BDOctTree::DeleteOctTree() {
  Nodes.clear();
}
//---------------------------------------------------------------------------
BDOctTree::BDOctNode BDOctTree::CreateSubNodes(BDOctNode node) {
  for (int i = 0; i < 6; i++)
    node.neighbour[i] = 0;

  if ((node.tris.size() <= MinTris) || ((node.x2 - node.x1) <= MinNodeSize)) {
    memset(node.children, 0, sizeof(node.children[0]) * 8);
    return node;
  }

  float midx = (node.x1 + node.x2) * 0.5,
      midy = (node.y1 + node.y2) * 0.5,
      midz = (node.z1 + node.z2) * 0.5;

  std::vector<Triangle*>::iterator i;
  BDOctNode dummy;

  for (short index = 0; index < 8; index++) {
    dummy.x1 = ((index % 2) == 0) ? node.x1 : midx;
    dummy.x2 = ((index % 2) == 0) ? midx : node.x2;
    dummy.y1 = (index < 4) ? node.y1 : midy;
    dummy.y2 = (index < 4) ? midy : node.y2;
    dummy.z1 = ((index < 2) || (index == 4) || (index == 5)) ? node.z1 : midz;     //0,1,4,5
    dummy.z2 = ((index < 2) || (index == 4) || (index == 5)) ? midz : node.z2;

    // dummy.size = node.size * 0.25;

    BDAABB box(dummy.ToBox());

    for (i = node.tris.begin(); i != node.tris.end(); i++) {
      if (box.Intersect(*(*i)))
        dummy.tris.push_back(*i);
    }

    Nodes.push_back(CreateSubNodes(dummy));
    // Put this here because more sub nodes were just created!!!
    node.children[index] = Nodes.size() - 1;
    dummy.tris.clear();
  }

  return node;
}
//---------------------------------------------------------------------------
void BDOctTree::DrawNodes(std::vector<int> nodes) {
  std::vector<int>::iterator i;

  glLineWidth(1.0);

  BDOctNode *index;
  for (i = nodes.begin(); i != nodes.end(); i++) {
    index = &Nodes[*i];
    glBegin(GL_LINES);
    glVertex3f((*index).x1, (*index).y1, (*index).z1);
    glVertex3f((*index).x2, (*index).y1, (*index).z1);

    glVertex3f((*index).x2, (*index).y1, (*index).z1);
    glVertex3f((*index).x2, (*index).y1, (*index).z2);

    glVertex3f((*index).x2, (*index).y1, (*index).z2);
    glVertex3f((*index).x1, (*index).y1, (*index).z2);

    glVertex3f((*index).x1, (*index).y1, (*index).z2);
    glVertex3f((*index).x1, (*index).y1, (*index).z1);


    glVertex3f((*index).x1, (*index).y2, (*index).z1);
    glVertex3f((*index).x2, (*index).y2, (*index).z1);

    glVertex3f((*index).x2, (*index).y2, (*index).z1);
    glVertex3f((*index).x2, (*index).y2, (*index).z2);

    glVertex3f((*index).x2, (*index).y2, (*index).z2);
    glVertex3f((*index).x1, (*index).y2, (*index).z2);

    glVertex3f((*index).x1, (*index).y2, (*index).z2);
    glVertex3f((*index).x1, (*index).y2, (*index).z1);


    glVertex3f((*index).x1, (*index).y1, (*index).z1);
    glVertex3f((*index).x1, (*index).y2, (*index).z1);

    glVertex3f((*index).x2, (*index).y1, (*index).z1);
    glVertex3f((*index).x2, (*index).y2, (*index).z1);

    glVertex3f((*index).x2, (*index).y1, (*index).z2);
    glVertex3f((*index).x2, (*index).y2, (*index).z2);

    glVertex3f((*index).x1, (*index).y1, (*index).z2);
    glVertex3f((*index).x1, (*index).y2, (*index).z2);
    glEnd();
  }
}
//---------------------------------------------------------------------------
std::vector<int> BDOctTree::SphereInNode(BDOctNode* node, float x, float y, float z, float sz) {
  std::vector<int> nodelist, dummy;

  for (short index = 0; index < 8; index++) {
    if (PointInBox(Nodes[node->children[index]].x1 - sz, Nodes[node->children[index]].x2 + sz,
                   Nodes[node->children[index]].y1 - sz, Nodes[node->children[index]].y2 + sz,
                   Nodes[node->children[index]].z1 - sz, Nodes[node->children[index]].z2 + sz,
                   x, y, z)) {
      // If this child is the smallest
      if (Nodes[node->children[index]].children[0] == 0) {
        nodelist.push_back(node->children[index]);
      } else {
        dummy = SphereInNode(&Nodes[node->children[index]], x, y, z, sz);
        while(dummy.size() > 0) {
          nodelist.push_back(dummy.back());
          dummy.pop_back();
        }
      }
    }
  }

  return nodelist;
}
//---------------------------------------------------------------------------
bool BDOctTree::PointInBox(float x1, float x2, float y1, float y2,
                           float z1, float z2, float px, float py,
                           float pz) {
  if ((x1 <= px) && (x2 >= px) && (y1 <= py) && (y2 >= py) &&
      (z1 <= pz) && (z2 >= pz))
    return true;

  return false;
}
//---------------------------------------------------------------------------
std::vector<int> BDOctTree::NodeVisible(BDOctNode *node, BDCamera* cam) {
  std::vector<int> nodelist, dummy;

  for (short index = 0; index < 8; index++) {
    BDCamera::InFrustum vis = cam->BoxInFrustum(Nodes[node->children[index]].ToBox());
    if (vis != BDCamera::IFNONE) {
      // If this child is the smallest or entire node is visible
      if ((Nodes[node->children[index]].children[0] == 0) ||
          (vis == BDCamera::IFALL)) {
        nodelist.push_back(node->children[index]);
      } else {
        dummy = NodeVisible(&Nodes[node->children[index]], cam);
        while(dummy.size() > 0) {
          nodelist.push_back(dummy.back());
          dummy.pop_back();
        }
      }
    }
  }

  return nodelist;
}
//---------------------------------------------------------------------------
std::vector<int> BDOctTree::BoxInNode(BDOctNode* node, BDAABB* box) {
  std::vector<int> nodelist, dummy;

  for (short index = 0; index < 8; index++) {
    BDAABB nbox(Nodes[node->children[index]].ToBox());

    if (nbox.Intersect(*box)) {
      // If this child is the smallest
      if (Nodes[node->children[index]].children[0] == 0) {
        nodelist.push_back(node->children[index]);
      } else {
        dummy = BoxInNode(&Nodes[node->children[index]], box);
        while(dummy.size() > 0) {
          nodelist.push_back(dummy.back());
          dummy.pop_back();
        }
      }
    }
  }

  return nodelist;
}
//---------------------------------------------------------------------------
void BDOctTree::DetermineNeighbours() {
  unsigned int i, i2;
  int n;

  // Check if nodes are neighbours
  for (i = 0; i < Nodes.size(); i++) {
    for (i2 = 0; i2 < Nodes.size(); i2++) {
      if (i != i2) {
        if ((n = AreNeighbours(i, i2)) != -1) {
          Nodes[i].neighbour[n] = i2;
        }
      }
    }
  }
}
//---------------------------------------------------------------------------
int BDOctTree::AreNeighbours(unsigned int i, unsigned int i2) {
  float widthi = Nodes[i].x2 - Nodes[i].x1;
  float widthi2 = Nodes[i2].x2 - Nodes[i2].x1;
  int rval = -1;

  if (widthi > widthi2) {
    return -1;
  }


  if ((Nodes[i2].y1 <= Nodes[i].y1) && (Nodes[i2].y2 >= Nodes[i].y2) &&
      (Nodes[i2].z1 <= Nodes[i].z1) && (Nodes[i2].z2 >= Nodes[i].z2)) {
    if (Nodes[i].x1 == Nodes[i2].x2)
      rval = 0;

    if (Nodes[i].x2 == Nodes[i2].x1)
      rval = 1;
  }

  if ((Nodes[i2].x1 <= Nodes[i].x1) && (Nodes[i2].x2 >= Nodes[i].x2) &&
      (Nodes[i2].z1 <= Nodes[i].z1) && (Nodes[i2].z2 >= Nodes[i].z2)) {
    if (Nodes[i].y1 == Nodes[i2].y2)
      rval = 2;

    if (Nodes[i].y2 == Nodes[i2].y1)
      rval = 3;
  }

  if ((Nodes[i2].x1 <= Nodes[i].x1) && (Nodes[i2].x2 >= Nodes[i].x2) &&
      (Nodes[i2].y1 <= Nodes[i].y1) && (Nodes[i2].y2 >= Nodes[i].y2)) {
    if (Nodes[i].z1 == Nodes[i2].z2)
      rval = 4;

    if (Nodes[i].z2 == Nodes[i2].z1)
      rval = 5;
  }

  if (rval != -1) {
    widthi = Nodes[Nodes[i].neighbour[rval]].x2 - Nodes[Nodes[i].neighbour[rval]].x1;
    widthi2 = Nodes[i2].x2 - Nodes[i2].x1;

    if (widthi2 > widthi)
      rval = -1;
  }

  return rval;
}
//---------------------------------------------------------------------------
Vector3 BDOctTree::RayTrace(Vector3 pos, Vector3 dir) {
  int pnode = PointInNode(&Nodes[0], pos.x, pos.y, pos.z);
  // Keep a list of tested triangles to clear pclist afterwards
  std::vector<Triangle*> tested;

  TracedTri = NULL;

  Vector3 p = TraceThroughNode(pnode, pos, dir, tested);

  for (std::vector<Triangle*>::iterator i = tested.begin(); i != tested.end(); i++) {
    (*i)->pclist = false;
  }

  return p;
}
//---------------------------------------------------------------------------
Vector3 BDOctTree::TraceThroughNode(int n, Vector3 pos, Vector3 dir,
                                    std::vector<Triangle*>& tested) {
  // Nodes[n].ToBox().Draw();
  // Check if collides with triangle
  Vector3 p = pos;
  Vector3 pc = pos + Vector3(0.0, 10*(Nodes[0].x2 - Nodes[0].x1), 0.0); // potential collider
  bool bCollided = false;
  for (std::vector<Triangle*>::iterator i = Nodes[n].tris.begin(); i != Nodes[n].tris.end(); i++) {
    if (!(*i)->pclist) {
      (*i)->pclist = true;
      tested.push_back(*i);
      p = (*i)->RayIntersection(pos, dir);
      if (p != pos) {
        bCollided = true;
        if ((pc - pos).SquaredLength() > (p - pos).SquaredLength()) {
          pc = p;
          TracedTri = *i;
        }
      }
    }
  }
  if (bCollided)
    return pc;

  // Find neighbour
  bool bx = (dir.x < 0);
  float x = bx ? Nodes[n].x1 : Nodes[n].x2;
  bool by = (dir.y < 0);
  float y = by ? Nodes[n].y1 : Nodes[n].y2;
  bool bz = (dir.z < 0);
  float z = bz ? Nodes[n].z1 : Nodes[n].z2;

  if (dir.x != 0.0)
    x = (x - pos.x) / dir.x;
  else
    x = (Nodes[0].x2 - Nodes[0].x1) * 2;

  if (dir.y != 0.0)
    y = (y - pos.y) / dir.y;
  else
    y = (Nodes[0].x2 - Nodes[0].x1) * 2;

  if (dir.z != 0.0)
    z = (z - pos.z) / dir.z;
  else
    z = (Nodes[0].x2 - Nodes[0].x1) * 2;

  int neighbour = 0;
  if ((x < y) && (x < z)) {
    neighbour = bx ? Nodes[n].neighbour[0] : Nodes[n].neighbour[1];
    pos += dir * x;
  } else {
    if (y < z) {
      neighbour = by ? Nodes[n].neighbour[2] : Nodes[n].neighbour[3];
      pos += dir * y;
    } else {
      neighbour = bz ? Nodes[n].neighbour[4] : Nodes[n].neighbour[5];
      pos += dir * z;
    }
  }

  if (neighbour == 0)
    return pos;
  else
    return TraceThroughNode(neighbour, pos, dir, tested);
}
//---------------------------------------------------------------------------
int BDOctTree::PointInNode(BDOctNode* node, float x, float y, float z) {
  int child;
  if (x < Nodes[node->children[0]].x2) {
    if (y < Nodes[node->children[0]].y2) {
      if (z < Nodes[node->children[0]].z2) {
        child = 0;
      } else {
        child = 2;
      }
    } else {
      if (z < Nodes[node->children[0]].z2) {
        child = 4;
      } else {
        child = 6;
      }
    }
  } else {
    if (y < Nodes[node->children[0]].y2) {
      if (z < Nodes[node->children[0]].z2) {
        child = 1;
      } else {
        child = 3;
      }
    } else {
      if (z < Nodes[node->children[0]].z2) {
        child = 5;
      } else {
        child = 7;
      }
    }
  }

  if (Nodes[node->children[child]].children[0] == 0) {
    return node->children[child];
  } else {
    return PointInNode(&Nodes[node->children[child]], x, y, z);
  }
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
