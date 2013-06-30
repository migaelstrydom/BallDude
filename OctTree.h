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
#ifndef OctTreeH
#define OctTreeH
//---------------------------------------------------------------------------
#include <vector>
#include "Triangle.h"
#include "Camera.h"
#include "AABB.h"
//---------------------------------------------------------------------------
namespace BDOctTree {
  struct BDOctNode {
    std::vector<Triangle*> tris;

    // Index to children   <-- Possible cause of error! Incorrect referencing!!!!
    int children[8];

    // Index to neighbours
    int neighbour[6];
    // Neighbouring numbers
    //           (Y)^
    //               _____5
    //             /__2_ /|
    //   <-(-X)  0|  4  |1|   (X) ->
    //            |_____|/
    //               3


    float x1, x2, y1, y2, z1, z2;
    // float size;

    BDAABB ToBox() const {
      float extent = (x2 - x1) * 0.5;
      return BDAABB(Vector3((x1 + x2) * 0.5, (y1 + y2) * 0.5,
                            (z1 + z2) * 0.5),
                    extent, extent, extent);
    }
  };

  extern std::vector<BDOctNode> Nodes;

  extern unsigned int MinTris;     // Minimum tris per node

  extern double MinNodeSize;       // Minimum node size (Whichever one reached first)

  extern void CreateOctTree(Triangle *tris, const int numtris);

  extern void DeleteOctTree();

  // Quickly finds out if the sphere is in the node or its children
  // Returns a vector with indices of all the nodes the sphere is in
  extern std::vector<int> SphereInNode(BDOctNode*, float, float, float, float);

  extern int PointInNode(BDOctNode*, float, float, float);

  extern std::vector<int> BoxInNode(BDOctNode*, BDAABB*);

  extern std::vector<int> NodeVisible(BDOctNode*, BDCamera*);

  // Traces a ray given starting position and normalised dir and returns
  // triangle with which ray "collides"
  extern Vector3 RayTrace(Vector3, Vector3);
  extern Triangle *TracedTri;

  // TEMP - For debug purposes
  extern void DrawNodes(std::vector<int>);
};
//---------------------------------------------------------------------------
#endif
