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
#include "Player.h"
#include "BDError.h"
#include "LoadDataUnit.h"
#include "OpenGLUnit.h"
#include "BulletUnit.h"
#include "Triangle.h"
#include "OctTree.h"
#include "FartCloudUnit.h"
//---------------------------------------------------------------------------
BDPlayer::BDPlayer(BDResourceControl* ResCon)
    : HealthBar(2.0) {
  BDFile File;
  File.ReadData("Player.txt");

  WalkSpeed = File.GetDatad("WalkSpeed");

  AirMoveSpeed = File.GetDatad("AirMoveSpeed");

  acceleration.y = FallSpeed = File.GetDatad("FallSpeed"); // GRAVITY

  JumpSpeed = File.GetDatad("JumpSpeed");

  size = 0.5;//0.5;

  CompileDList();

  direction = Vector3(0.0, 0.0, -1.0);

  TurnDegree = 0;

  // JumpSound = ResCon->GetSoundPtr(File.GetDatai("JumpSound"));

  // BULLETS!!
  // ShotSound = ResCon->GetSoundPtr(File.GetDatai("ShotSound"));

  BulletFile = new BDFile();
  BulletFile->ReadData(File.GetDatas("BulletFile").c_str());

  BulletTexID = ResCon->GetTexID(BulletFile->GetDatai("BulletTexture"));

  ExplosionTexID = ResCon->GetTexID(BulletFile->GetDatai("ExplosionTexture"));

  FartCloudParam.texID = ResCon->GetTexID(File.GetDatai("FartCloudTexture"));

  FartCloudParam.ECtexID = ResCon->GetTexID(File.GetDatai("ExCloudTexture"));

  FartCloudParam.BubtexID = ResCon->GetTexID(File.GetDatai("BubTexture"));

  // FartCloud variables
  FartCloudParam.MaxNum = File.GetDatai("FCMaxNum");
  FartCloudParam.Force.x = File.GetDatad("FCForce", 0);
  FartCloudParam.Force.y = File.GetDatad("FCForce", 1);
  FartCloudParam.Force.z = File.GetDatad("FCForce", 2);
  FartCloudParam.PartSize = File.GetDatad("FCPartSize");
  FartCloudParam.Strength = File.GetDatai("FCStrength");
  FartCloudParam.ECStrength = File.GetDatai("ECStrength");
  FartCloudParam.EmitInterval = File.GetDatad("FCEmitInterval");
  FartCloudParam.LifeTime = File.GetDatad("FCLifeTime");
  FartCloudParam.BubPartSize = File.GetDatad("BubPartSize");

  // FlatoSound = ResCon->GetSoundPtr(File.GetDatai("FlatoSound"));

  // BubbleSound = ResCon->GetSoundPtr(File.GetDatai("BubbleSound"));

  FCKickBack = File.GetDatad("FCKickBack");
  FCTimeLeft = 0;

  // Animation
  Animate = new BDPlayerAnimate(&File);

  ObjType = OBJ_OBJECT;

  // create static shadow
  /*const int s_segs = 20;
    const double s_length = 50.0;
    const double hangle = 2*M_PI/s_segs;
    shadow = new BDBDMModel();

    Vector3 *verts = new Vector3[s_segs*2];
    Vector3 *vidx = verts;
    for (int i = 0; i < s_segs; i++) {
    vidx->x = size*sin(hangle*i);
    vidx->y = 0.0;
    vidx->z = size*cos(hangle*i);
    vidx++;
    }

    for (int i = 0; i < s_segs; i++) {
    vidx->x = size*sin(hangle*i);
    vidx->y = -s_length;
    vidx->z = size*cos(hangle*i);
    vidx++;
    }

    shadow->SetVerts(verts);
    shadow->NumVerts = s_segs*2;

    unsigned* indices = new unsigned[s_segs*6];
    unsigned *iidx = indices;

    for (int i = 0; i < s_segs; i++) {
    iidx[0] = i;
    iidx[1] = s_segs+i;
    iidx[2] = (i+1)%s_segs;
    iidx += 3;
    }

    for (int i = 0; i < s_segs; i++) {
    iidx[0] = s_segs+i;
    iidx[1] = s_segs+((i+1)%s_segs);
    iidx[2] = (i+1)%s_segs;
    iidx += 3;
    }

    shadow->SetIndices(indices);
    shadow->NumTris = 2*s_segs;*/

  shadow = new BDShadow();
  shadow->InitSphere();
  shadow->SphereShadow(size+0.01, Vector3(329.983, 942.809, 47.1405));
}
//---------------------------------------------------------------------------
BDPlayer::~BDPlayer() {
  glDeleteLists(Dlist, 3);
  glDeleteTextures(1, &EyeTexID);
  glDeleteTextures(1, &BulletTexID);
  glDeleteTextures(1, &ExplosionTexID);
  glDeleteTextures(1, &FartCloudParam.texID);
  glDeleteTextures(1, &FartCloudParam.ECtexID);
  glDeleteTextures(1, &FartCloudParam.BubtexID);

  if (Animate) delete Animate;

  if (BulletFile) delete BulletFile;

  if (shadow) delete shadow;
}
//---------------------------------------------------------------------------
void BDPlayer::OnAnimate(double deltaTime) {
  /*char str[200];
    sprintf(str,
    "x = %f\ny = %f\nz = %f\nvel.x = %f\nvel.y = %f\nvel.z = %f\nspd = %f\nacc.x = %f\nacc.y = %f\nacc.z = %f\nOnGround = %d",
    position.x, position.y, position.z,
    velocity.x, velocity.y, velocity.z, velocity.length(),
    acceleration.x, acceleration.y, acceleration.z, OnGround);
    MSOpenGL::msg = str;*/

  position += velocity * deltaTime;
  velocity += acceleration * deltaTime;
  // velocity.y -= 0.01;

  acceleration.y = FallSpeed;
  Animate->Update(deltaTime);

  // Fartcloud key press
  if (FCTimeLeft > 0.0f)
    FCTimeLeft -= deltaTime;

  HealthBar.Animate(deltaTime);
}
//---------------------------------------------------------------------------
void BDPlayer::OnDraw(BDCamera* cam) {
  // SHADOW LEFT OUT UNTIL PROPER IMPLEMENTATION CAN BE DONE
  /*Vector3 t = BDOctTree::RayTrace(position, Vector3(0.0, -1.0, 0.0));

    Vector3 camshad = BDOctTree::RayTrace(cam->Position, (t - cam->Position).UnitVector());

    if ((camshad - t).SquaredLength() < 0.5) {
    glPushMatrix();
    // glDepthFunc(GL_LEQUAL);
    glDisable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    glTranslated(t.x, t.y, t.z);
    glScalef(0.85, 0.0, 0.85);
    glColor4f(0.0, 0.0, 0.0, 0.2);
    glCallList(Dlist);
    // glDepthFunc(GL_LESS);
    glDisable(GL_BLEND);
    glEnable(GL_DEPTH_TEST);
    glPopMatrix();
    }    */

  shadow->position = position;
  // shadow->SphereShadow(size+0.01, Vector3(50.0,17.0,32.0)-position);

  glTranslated(position.x, position.y, position.z);
  glRotatef(TurnDegree, 0.0, 1.0, 0.0);

  // Outline
  /*glFrontFace(GL_CW);
    glPolygonMode(GL_FRONT, GL_LINE);
    glDisable(GL_LIGHTING);
    glLineWidth(3.0);
    glColor3f(0.0, 0.0, 0.0);
    glPushMatrix();
    glCallList(Dlist + 1);
    glCallList(Dlist + 2);
    glPopMatrix();  */

  glPushMatrix();
  glScalef(Animate->ScaleX, 1.0f, 1.0f);
  // glCallList(Dlist);
  // glPolygonMode(GL_FRONT, GL_FILL);
  // glFrontFace(GL_CCW);
  // The rest
  glColor3f(1.0, 0.0, 0.0);
  glCallList(Dlist);
  glPopMatrix();
  glColor3f(0.0f, 1.0f, 0.0f);
  glTranslatef(0.0, 0.0, -0.5);
  float invnose = 1.0f/Animate->NoseScale;
  glScalef(invnose, invnose, Animate->NoseScale);
  glCallList(Dlist + 1);
  glScalef(Animate->NoseScale, Animate->NoseScale, invnose);
  glTranslatef(-0.175, 0.25, 0.175);
  if (Animate->Blink)
    glRotatef(180.0f, 1.0f, 0.0f, 0.0f);

  glEnable(GL_TEXTURE_2D);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
  glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
  glBindTexture(GL_TEXTURE_2D, EyeTexID);
  glColor3f(0.5, 1.0, 1.0);
  glRotatef(-90.0, 1.0, 0.0, 0.0);
  glCallList(Dlist + 2);
  glDisable(GL_TEXTURE_2D);
  // glEnable(GL_LIGHTING);
}
//---------------------------------------------------------------------------
bool BDPlayer::Collision(BDCollision *Col) {
  if (Col->ColType == OBJ_TRIANGLE) {
    if (Col->Plane.N.y > 0.0)
      OnGround = true;

    if (Col->triangle->friction < Col->Plane.N.y) {
      // if (velocity.y < 0.0)
      // acceleration.y = 0.0;
    }

    /*if (Col->triangle->Plane.N.y <= Col->triangle->friction)  // Slope
      {
      position = Col->pInt + Col->Plane.N * size;
      float dy = Col->Plane.N.y;
      Col->Plane.N.y = 0.0;
      velocity = Col->Plane.RayIntersection(Col->pInt + velocity, Col->Plane.N) - Col->pInt;
      Col->Plane.N.y = dy;
      velocity = Col->Plane.RayIntersection(Col->pInt + velocity, Col->Plane.N) - Col->pInt;

      return false;
      } */

    // Scale Nose
    if (Col->Plane.N.y == 0.0) {
      Vector3 nosepos = position + 0.5*direction;
      float ndist = Col->Plane.DistanceToPlane(nosepos) * 2;
      if (ndist < Animate->NoseScale) {
        Animate->NoseScale = (ndist < 0.5) ? 0.5 : ndist;
      }
    }
  } else if (Col->ColType == OBJ_WATER) {
    InWater = true;
    OnGround = true;
    Col->Size = velocity.y * 0.2;

    WaterSurface = ((position.y + Col->Plane.D) > -size) && (velocity.y < 1.0) &&
        (velocity.y > -1.0);

    if (fabs(Col->Size) < 0.2) {
      Col->Size = 0.0;
      if (velocity.x*velocity.x + velocity.z*velocity.z > 1.0) {
        Col->Size = -0.4;
      }
    }
    return true;
  } else if (Col->ColType == OBJ_EXPLOSION) {
    if (acceleration.SquaredLength() <= 10000.0)
      acceleration += (position - Col->Position).WithLength(300.0);
  } else if (Col->ColType == OBJ_ENEMY) {
    Vector3 mov = (position - Col->Position).WithLength(30.0);
    mov.y = 0.0;
    velocity += mov;
  }

  return true;
}
//---------------------------------------------------------------------------
void BDPlayer::CheckInput(BDInput *Input) {
  // FOR FARTING
  if (Input->Key(SDLK_SPACE)) {
    if ((FCTimeLeft <= 0.0f)) {
      // fartpressed = true;
      FCTimeLeft = 0.3;
      FartCloudParam.bBubble = InWater;  // bubble if in water
      BDFartCloud *Cloud = new BDFartCloud(position - direction*size, direction, &FartCloudParam);
      Cloud->velocity += velocity*0.5;
      parent->AttachInFront(Cloud);
      Cloud->Explode();

      // play sound
      /*if (InWater) {

        } else {

        }*/

      // move forwards
      velocity += direction*FCKickBack;
    }
  }

  // FOR SHOOTING
  static bool shootpressed = false;
  if (Input->MouseLeftDown()) {
    if (!shootpressed) {
      shootpressed = true;
      position.y += 0.25;
      BDBullet *Bullet = new BDBullet(position, direction, BulletFile,
                                      BulletTexID, ExplosionTexID);
      Bullet->velocity += velocity;
      position.y -= 0.25;

      // attach as child next to player
      // if (next == this)
      parent->AttachBehind(Bullet);
      /*else {
        Bullet->parent = parent;
        Bullet->prev = this;
        Bullet->next = next;

        next->prev = Bullet;
        next = Bullet;
        }  */

      // ShotSound->SetPosition(0.0, 0.0, (position - *CamPos).length());
      // ShotSound->Play();
    }
  } else {
    shootpressed = false;
  }

  // FOR JUMPING
  static bool jumppressed = false;

  if (Input->MouseRightDown()) {
    if (((!jumppressed) && (OnGround)) && ((!InWater) || ((InWater) && (WaterSurface)))) {
      velocity.y = JumpSpeed;
      jumppressed = true;
      // OnGround = false;
      float pos[3] = { 0.0, 0.0, (position - *CamPos).length() / 10};
      // int chan = FSOUND_PlaySoundEx(FSOUND_FREE, JumpSound, NULL, true);
      // FSOUND_3D_SetAttributes(chan, pos, NULL);
      // FSOUND_SetPaused(chan, false);
      // return;
    }
  } else if (OnGround)
    jumppressed = false;

  // FOR MOVING
  bool Left = false, Right = false, Up = false, Down = false;

  if ((Input->Key(SDLK_a)) || (Input->Key(SDLK_LEFT)) || (Input->Key(SDLK_KP4))) {
    Left = true;
  }

  if ((Input->Key(SDLK_e)) || (Input->Key(SDLK_RIGHT)) || (Input->Key(SDLK_KP6))) {
    if (Left)
      Left = false;
    else
      Right = true;
  }

  if ((Input->Key(',')) || (Input->Key(SDLK_UP)) || (Input->Key(SDLK_KP8))) {
    Up = true;
  }

  if ((Input->Key(SDLK_o)) || (Input->Key(SDLK_DOWN)) || (Input->Key(SDLK_KP2))) {
    if (Up)
      Up = false;
    else
      Down = true;
  }

  Vector3 initvel = velocity;
  velocity.x = velocity.z = 0.0;
  // if ((OnGround) && (velocity.y > 0.0))
  velocity.y = 0.0;

  bool Calculated = false;
  Vector3 Forward;

  if (Left) {
    Forward = (position - *CamPos);
    Forward.y = 0;
    Forward.SetLength(1.0);
    Calculated = true;

    velocity += (Forward ^ Vector3(0.0, -1.0, 0.0)).WithLength(1.0);
    if (Up || Down) {
      if (Up)
        TurnDegree = 45;
      else
        TurnDegree = 135;
    } else
      TurnDegree = 90;
  } else if (Right) {
    Forward = (position - *CamPos);
    Forward.y = 0;
    Forward.SetLength(1.0);
    Calculated = true;

    velocity += (Forward ^ Vector3(0.0, 1.0, 0.0)).WithLength(1.0);

    if (Up || Down) {
      if (Up)
        TurnDegree = 315;
      else
        TurnDegree = 225;
    } else
      TurnDegree = 270;
  }

  if (Up) {
    if (!Calculated) {
      Forward = (position - *CamPos);
      Forward.y = 0;
      Forward.SetLength(1.0);
      TurnDegree = 0;
    }
    velocity += Forward;
  } else
    if (Down) {
      if (!Calculated) {
        Forward = (position - *CamPos);
        Forward.y = 0;
        Forward.SetLength(1.0);
        TurnDegree = 180;
      }
      velocity -= Forward;
    }

  // velocity.y = 0;

  if (velocity != Vector3(0.0, 0.0, 0.0)) {
    if ((Forward.z >= 0) && (Forward.x >= 0)) {
      TurnDegree += RAD2DEG(Forward.Angle(Vector3(0.0, 0.0, 1.0))) + 180;
    } else if ((Forward.z < 0) && (Forward.x >= 0)) {
      TurnDegree -= RAD2DEG(Forward.Angle(Vector3(0.0, 0.0, -1.0)));
    } else if ((Forward.z >= 0) && (Forward.x < 0)) {
      TurnDegree += RAD2DEG(Forward.Angle(Vector3(0.0, 0.0, -1.0)));
    } else if ((Forward.z < 0) && (Forward.x < 0)) {
      TurnDegree += RAD2DEG(Forward.Angle(Vector3(0.0, 0.0, -1.0)));
    }

    velocity.SetLength(1.0);
    direction = velocity;
    if (!OnGround) {
      velocity *= AirMoveSpeed;
    } else {
      velocity *= WalkSpeed;
    }
  }

  if (OnGround) {
    acceleration.x = (velocity.x - initvel.x) * WalkSpeed;
    acceleration.z = (velocity.z - initvel.z) * WalkSpeed;
  } else {
    acceleration.x = velocity.x;
    acceleration.z = velocity.z;
  }
  velocity = initvel;

  if (InWater)
    InWater = false;

  OnGround = false;
}
//---------------------------------------------------------------------------
void BDPlayer::CreateEyeTex() {
  BDTexture *EyeTex = new BDTexture();
  if (!EyeTex->LoadTexture("eye.bmp"))
    throw BDError("Unable to load Eye Texture!");

  glGenTextures(1, &EyeTexID);
  glBindTexture(GL_TEXTURE_2D, EyeTexID);

  gluBuild2DMipmaps(GL_TEXTURE_2D, EyeTex->pixel_format, EyeTex->width,
                    EyeTex->height, EyeTex->pixel_format, GL_UNSIGNED_BYTE,
                    EyeTex->data);

  delete EyeTex;
}
//---------------------------------------------------------------------------
void BDPlayer::CompileDList() {
  GLUquadricObj *Sphere = gluNewQuadric();

  glEnable(GL_TEXTURE_2D);
  CreateEyeTex();
  glDisable(GL_TEXTURE_2D);

  Dlist = glGenLists(3);
  glNewList(Dlist, GL_COMPILE);
  gluSphere(Sphere, size, 30, 21); //10, 7
  glEndList();
  glNewList(Dlist + 1, GL_COMPILE);
  gluSphere(Sphere, 0.25, 24, 15);    //8, 5
  glEndList();

  gluQuadricTexture(Sphere, GL_TRUE);
  // EYES
  glNewList(Dlist + 2, GL_COMPILE);
  gluSphere(Sphere, 0.2, 16, 10);  //8, 5
  glTranslatef(0.35, 0.0, 0.0);
  gluSphere(Sphere, 0.2, 16, 10);  //8, 5
  glEndList();
  gluDeleteQuadric(Sphere);
}
//---------------------------------------------------------------------------
void BDPlayer::OnPrepare(float deltaTime) {
  ((DrawableObject*)(parent->child->next))->PerformCollisionDetection(this, deltaTime);
}
//---------------------------------------------------------------------------
BDPlayerAnimate::BDPlayerAnimate(BDFile *File) {
  blinkTime = 0;
  EyeBlinkTime = File->GetDatad("EyeBlinkTime");
  EyeBlinkLength = File->GetDatad("EyeBlinkLength");
  Blink = false;

  breatheCounter = 0;
  breatheTime = File->GetDatad("BreatheTime");
  inhaleMax = File->GetDatad("InhaleMax");

  ScaleX = ScaleY = 1.0;
  NoseScale = 1.0f;
}
//---------------------------------------------------------------------------
void BDPlayerAnimate::Update(float deltaTime) {
  if (Jump)
    AnimateJump(deltaTime);

  blinkTime += deltaTime;

  if (blinkTime >= EyeBlinkTime) {
    blinkTime = 0;
    Blink = true;
  } else if (blinkTime >= EyeBlinkLength) {
    Blink = false;
  }

  breatheCounter += deltaTime;

  NoseScale += (1.0f - NoseScale) * deltaTime * 2;

  static bool Exhale = false;
  if (breatheCounter > breatheTime) {
    Exhale = !Exhale;
    breatheCounter = 0;
  }

  if (!Exhale) {
    double temp = (double(breatheCounter) / double(breatheTime)) * double(inhaleMax);
    ScaleX = 1.0 - double(inhaleMax) + temp;
  } else {
    double temp = (double(breatheCounter) / double(breatheTime)) * double(inhaleMax);
    ScaleX = 1.0 - temp;
  }
}
//---------------------------------------------------------------------------
void BDPlayerAnimate::StartJump() {
  Jump = true;
}
//---------------------------------------------------------------------------
void BDPlayerAnimate::AnimateJump(float) {
  Jump = true;
}
//---------------------------------------------------------------------------
void BDPlayerHealthBar::Draw() {
  // Get into proper projection mode
  glMatrixMode(GL_PROJECTION);
  glPushMatrix();
  glLoadIdentity();
  glOrtho(0.0, 640.0, 0.0, 480.0, -1.0, 1.0);
  glMatrixMode(GL_MODELVIEW);
  glPushMatrix();
  glLoadIdentity();

  glDisable(GL_LIGHTING);
  glEnable(GL_BLEND);
  glLineWidth(1.0f);
  glColor4f(1.0f - fHealth/cfMaxHealth, fHealth/cfMaxHealth, 1.0, 0.5);
  glTranslatef(470.0f, 440.0f, 0.0f);
  glScalef(150.0f, 40.0f, 1.0f);

  /*glBegin(GL_POLYGON);
    glVertex2f(-1.0f, 1.0f);
    glVertex2f(-1.0f, -1.0f);
    glVertex2f(1.0f, -1.0f);
    glVertex2f(1.0f, 1.0f);
    glEnd();  */

  glBegin(GL_LINE_STRIP);

  for (int i = 0; i < NumPoints; i++) {
    glVertex2f(i/float(NumPoints), p[i]);
  }

  glEnd();

  glDisable(GL_BLEND);
  glEnable(GL_LIGHTING);

  // return to original projection mode
  glMatrixMode(GL_PROJECTION);
  glPopMatrix();
  glMatrixMode(GL_MODELVIEW);
  glPopMatrix();
}
//---------------------------------------------------------------------------
void BDPlayerHealthBar::Animate(double deltaTime) {
  static float time = 0.0f;

  time += deltaTime;

  // fHealth = cos(time)/2 + 1.5f;
  /*if (GetAsyncKeyState('T')) {
    fHealth += deltaTime;
    }
    if (GetAsyncKeyState('Y')) {
    fHealth -= deltaTime;
    }*/
  if (fHealth > cfMaxHealth) {
    fHealth = cfMaxHealth;
  }
  if (fHealth < 0.0f) {
    fHealth = 0.0f;
  }

  int i;

  int aliverange = int(NumPoints*(1.0f - fHealth/cfMaxHealth)) + 1;

  for (i = 1; i < aliverange; i++)
    p2[i] = 0.0f;

  const float A = (0.5*deltaTime);
  const float B = 2 - 4*A;
  float aveh = 0.0f;
  for (i = aliverange; i < NumPoints-1; i++) {
    p2[i] = A * (p[i-1] + p[i+1]) + B * p[i] - p2[i];
    aveh += fabs(p2[i]);
    // clamp to avoid instability
    /*if (p2[i] > 1.5f) {
      p2[i] *= 1-deltaTime;
      } else if (p2[i] < -1.5f) {
      p2[i] *= 1-deltaTime;
      }   */
  }

  float *t = p2;
  p2 = p;
  p = t;

  if (fabs(aveh) < 0.0001f)
    return;

  aveh /= NumPoints - aliverange;
  aveh = 0.25f/aveh;

  for (i = aliverange; i < NumPoints-1; i++) {
    p[i] *= aveh;
  }
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------

