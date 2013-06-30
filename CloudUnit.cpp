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
#include "CloudUnit.h"
#include "OctTree.h"
#include "OpenGLUnit.h"
//#include <queue>

#define MAX_PARTICLES       63
#define MAX_DROPS           100
#define IDEAL_HEIGHT        10.0
#define PLAYER_SQ_RADIUS    1200.0   // square radius of distance between cloud & player
#define MAX_SPEED           20.0
#define HEIGHT_DAMP_COEFF   2
#define RESCUE_DAMP_COEFF   50//15
#define RESCUE_SPEED_COEFF  30
//---------------------------------------------------------------------------
class CmpPartDist {
 public:
  float dist;
  int idx;

  bool operator<(const CmpPartDist& c) const
  {
    return dist < c.dist;
  }
};
//---------------------------------------------------------------------------
BDCloud::BDCloud(Vector3 pos, unsigned int tid, Vector3* pp) {
  bAlpha = true;
  position = pos;
  texID = tid;
  playerPos = pp;

  ObjType = OBJ_OBJECT;

  RescueTime = 0.0;
  bCaught = false;

  size = 5.0;
  Particles = new BDParticle[MAX_PARTICLES];
  for (int i = 0; i < MAX_PARTICLES; i++) {
    Particles[i].position.x = double(rand() % 10000 - 5000)/1500.0;
    Particles[i].position.y = double(rand() % 10000 - 5000)/20000.0;
    Particles[i].position.z = double(rand() % 10000 - 5000)/1500.0;

    Particles[i].velocity.x = double(rand() % 10000 - 5000)/2000.0;
    Particles[i].velocity.y = 0.0;// double(rand() % 10000 - 5000)/10000.0;
    Particles[i].velocity.z = double(rand() % 10000 - 5000)/2000.0;
    Particles[i].acceleration = -Particles[i].position.WithLength(10.0);
    //((Particles[i].velocity % Particles[i].velocity) / size);
    Particles[i].size = double(rand()%100 + 50)/100.0;
  }

  Drops = new BDParticle[MAX_DROPS];
  for (int i = 0; i < MAX_DROPS; i++) {
    Emit(&Drops[i]);
    Drops[i].velocity.x = 0.0;
    Drops[i].velocity.y = double(rand() % 1000 - 500)/100.0 - 10.0;
    Drops[i].velocity.z = 0.0;
  }

  // SHADOW
  shadow = new BDShadow();
  shadow->InitSphere();
  shadow->SphereShadow(0.4, Vector3(.329983, .942809, .0471405));
}
//---------------------------------------------------------------------------
BDCloud::~BDCloud() {
  delete[] Particles;
  delete[] Drops;
  delete shadow;
}
//---------------------------------------------------------------------------
void BDCloud::OnAnimate(double deltaTime) {
  // first animate overall cloud movement
  // check distance from player
  Vector3 relpos = *playerPos - position;
  if (RescueTime <= 0.0)   // player does not need to be rescued
  {
    if (relpos.x*relpos.x + relpos.z*relpos.z > PLAYER_SQ_RADIUS) {
      // move towards player
      velocity += relpos*deltaTime;
      double vlen = velocity.length();
      if (vlen > MAX_SPEED) {
        vlen /= MAX_SPEED;
        velocity /= vlen;
      }
    } else {
      velocity *= 1-deltaTime;
    }
    position += velocity * deltaTime;
    position.y = (IDEAL_HEIGHT+Height) -
        ((IDEAL_HEIGHT+Height) - position.y)/(1 + HEIGHT_DAMP_COEFF*deltaTime);
  } else    // rescue the player!!
  {
    float sqlen = relpos.SquaredLength();
    if (bCaught) {
      position += velocity*deltaTime;
      *playerPos = position;
    } else   // get there quickly!
    {
      // position = (*playerPos) -
      //    (*playerPos - position)/(1 + RESCUE_DAMP_COEFF*RescueTime*RescueTime*deltaTime);
      double rescspeed = exp(RescueTime)*RESCUE_SPEED_COEFF*deltaTime;
      if (relpos.length() < rescspeed) {
        position = *playerPos;
        bCaught = true;
        velocity = (acceleration - position).WithLength(MAX_SPEED);
      } else {
        position += (*playerPos - position).WithLength(rescspeed);
      }
    }
  }

  // animate particles in the cloud
  BDParticle *idx, *last = &Particles[MAX_PARTICLES];
  for (idx = Particles; idx < last; idx++) {
    idx->acceleration = -idx->position.WithLength(5.0);
    idx->velocity += idx->acceleration * deltaTime;
    idx->position += idx->velocity * deltaTime;
  }
  last = &Drops[MAX_DROPS];
  for (idx = Drops; idx < last; idx++) {
    idx->position += idx->velocity * deltaTime;
    if (idx->position.y < idx->energy) {
      Emit(idx);
    }
  }
}
//---------------------------------------------------------------------------
void BDCloud::OnDraw(BDCamera* Cam) {
  int index;
  CmpPartDist cpd;
  std::priority_queue<CmpPartDist> pq;
  // sort according to distance
  for (index = 0; index < MAX_PARTICLES; index++) {
    cpd.dist = Cam->CalcDist(Particles[index].position);
    cpd.idx = index;
    pq.push(cpd);
  }
  glDisable(GL_LIGHTING);
  // only draw drops if not rescuing
  // if (RescueTime == 0.0)
  //{
  glColor3f(0.4, 0.727, 0.742);
  glPointSize(4.0);
  glBegin(GL_POINTS);
  for (index = 0; index < MAX_DROPS; index++) {
    glVertex3f(Drops[index].position.x, Drops[index].position.y, Drops[index].position.z);
  }
  glEnd();
  //}

  // all alpha objects will be tested automatically for visibility
  glEnable(GL_TEXTURE_2D);
  glEnable(GL_BLEND);
  glEnable(GL_ALPHA_TEST);
  glAlphaFunc(GL_GREATER, 0);
  // glDepthMask(GL_FALSE);

  shadow->position = position;
  glTranslated(position.x, position.y, position.z);

  // The modelview matrix
  float matrix[16];
  glGetFloatv(GL_MODELVIEW_MATRIX, matrix);

  // Get the correct vectors for billboarding
  Vector3 right(matrix[0], matrix[4], matrix[8]);
  Vector3 up(matrix[1], matrix[5], matrix[9]);

  // Bind the texture
  glBindTexture(GL_TEXTURE_2D, texID);

  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

  glColor3f(0.9, 0.9, 0.9);

  // Draw particles
  glBegin(GL_QUADS);
  // for (index = 0; index < MAX_PARTICLES; index++)
  while(!pq.empty()) {
    index = pq.top().idx;
    pq.pop();
    Vector3 Calc = Particles[index].position - ((right + up) * Particles[index].size);
    glTexCoord2f(0.0, 0.0);
    glVertex3d(Calc.x, Calc.y, Calc.z);

    Calc = Particles[index].position + ((right - up) * Particles[index].size);
    glTexCoord2f(1.0, 0.0);
    glVertex3d(Calc.x, Calc.y, Calc.z);

    Calc = Particles[index].position + ((right + up) * Particles[index].size);
    glTexCoord2f(1.0, 1.0);
    glVertex3d(Calc.x, Calc.y, Calc.z);

    Calc = Particles[index].position + ((up - right) * Particles[index].size);
    glTexCoord2f(0.0, 1.0);
    glVertex3d(Calc.x, Calc.y, Calc.z);
  }
  glEnd();

  glDisable(GL_TEXTURE_2D);
  glDisable(GL_BLEND);
  glDisable(GL_ALPHA_TEST);
  glDepthMask(GL_TRUE);
  glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
}
//---------------------------------------------------------------------------
void BDCloud::OnCollision(DrawableObject* Object, float) {
  /*BDCollision col;
    col.Position = position;
    col.ColType = ObjType;
    Object->(&col);*/

  if ((RescueTime != 0.0) && ((&Object->position) == playerPos)) {
    Object->acceleration = Vector3(0.0, 0.0, 0.0);
    if (RescueTime < 0.0) {
      Object->velocity = BDOctTree::Nodes[0].ToBox().center;
      Object->velocity -= Object->position;
      Object->velocity.SetLength(MAX_SPEED);
    } else {
      Object->velocity = Object->acceleration;
    }
  }
}
//---------------------------------------------------------------------------
bool BDCloud::Collision(BDCollision* col) {
  col->ColType = ObjType;

  return false;
}
//---------------------------------------------------------------------------
void BDCloud::OnPrepare(float deltaTime) {
  // Collisions
  /*Vector3 dummy = velocity;   // avoid landscape collisions
    velocity = Vector3(0.0, 0.0, 0.0);
    ((DrawableObject*)(parentNode->childNode->nextNode))->PerformCollisionDetection(this, deltaTime);
    velocity = dummy;*/

  if (BDOctTree::Nodes[0].ToBox().Intersect(*playerPos)) {
    if (RescueTime > 0.0) {
      RescueTime = -1.0;  // next collision, throw player
    } else {
      RescueTime = 0.0;
    }
  } else {
    if (RescueTime == 0.0) {
      acceleration = *playerPos;
      bCaught = false;
    }
    RescueTime += deltaTime;
  }

  if (BDOctTree::Nodes[0].ToBox().Intersect(position)) {
    Height = BDOctTree::RayTrace(position, Vector3(0.0, -1.0, 0.0)).y;
  } else {
    Height = position.y - IDEAL_HEIGHT;
  }
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
