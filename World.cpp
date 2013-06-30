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
#include "World.h"
#include "WaterSurface.h"
#include "WaterRefract.h"
#include "FireUnit.h"
#include "BallUnit.h"
#include "KniveUnit.h"
#include "HealthCritterUnit.h"
#include "CloudUnit.h"
#include "TreeUnit.h"
//---------------------------------------------------------------------------
#define MUSICFADETIME 3.0
#define MUSICVOLUME 90
//---------------------------------------------------------------------------
BDWorld::BDWorld() {
  LandScape = NULL;
  Camera = new BDCamera(NULL);

  bLoaded = false;
}
//---------------------------------------------------------------------------
BDWorld::~BDWorld() {
  UnloadWorld();
  delete Camera;
}
//---------------------------------------------------------------------------
void BDWorld::LoadWorld(std::string sWorld) {
  ResCon = new BDResourceControl();
  AIEngine = NULL;

  BDFile File;
  File.ReadData(sWorld.c_str());

  if (File.GetSizes("ObjectModel"))
    File.LoadOBJ(File.GetDatas("ObjectModel").c_str());

  float bgcolor[4];
  for (short index = 0; index < 4; index++) {
    bgcolor[index] = File.GetDatad("BGColor", index);
  }

  float ambientlight[4];
  for (short index = 0; index < 4; index++) {
    ambientlight[index] = File.GetDatad("AmbientLight", index);
  }

  float lightcol[] = {1.0f, 1.0f, 1.0f, 1.0f};
  for (unsigned int index = 0; index < File.GetSized("LightPosition"); index++) {
    Lights.push_back(File.GetDatad("LightPosition", index));
    if ((index % 4) == 0) {
      glEnable(GL_LIGHT0 + index/4);
      glLightfv(GL_LIGHT0 + index/4, GL_DIFFUSE, lightcol);
    }
  }

  glClearColor(bgcolor[0], bgcolor[1], bgcolor[2], bgcolor[3]);
  glLightfv(GL_LIGHT0, GL_AMBIENT, ambientlight);
  // glLightfv(GL_LIGHT0, GL_POSITION, lightpos);
  glEnable(GL_LIGHT0);

  if (File.GetDatab("DrawFog")) {
    glEnable(GL_FOG);
    float fogcolor[4];
    for (short index = 0; index < 4; index++) {
      fogcolor[index] = File.GetDatad("FogColor", index);
    }
    glFogi(GL_FOG_MODE, GL_LINEAR);
    glFogfv(GL_FOG_COLOR, fogcolor);
    glFogf(GL_FOG_DENSITY, 0.1);
    glFogf(GL_FOG_START, 3.0);
    glFogf(GL_FOG_END, 450.0);
  } else
    glDisable(GL_FOG);

  LandScape = new BDLandScape();
  LandScape->LoadLandScape(&File, ResCon->GetTexID(File.GetDatai("LandScapeTexture")));

  // Water Surface
  for (unsigned int index = 0; index < File.GetSized("WaterSurface"); index += 4) {
    int ms = 32, tex = 0;
    if (File.GetSizei("WaterSurface")) {
      ms = File.GetDatai("WaterSurface", index / 2);
      tex = File.GetDatai("WaterSurface", index / 2 + 1);
    }

    BDWaterSurface *WaterSurface = new
        BDWaterSurface(Vector3(File.GetDatad("WaterSurface", index),
                               File.GetDatad("WaterSurface", index+1),
                               File.GetDatad("WaterSurface", index+2)),
                       File.GetDatad("WaterSurface", index+3),
                       ResCon->GetTexID(tex), ms);
    LandScape->AttachInFront(WaterSurface);
    LandScape->WaterCollision(WaterSurface);
  }

  // Water Refract
  for (unsigned int index = 0; index < File.GetSized("WaterRefract"); index += 5) {
    int ms = 32, tex1 = 0, tex2 = 0;
    if (File.GetSizei("WaterRefract")) {
      ms = File.GetDatai("WaterRefract", index / 5 * 3);
      tex1 = File.GetDatai("WaterRefract", index / 5 * 3 + 1);
      tex2 = File.GetDatai("WaterRefract", index / 5 * 3 + 2);
    }

    BDWaterRefract *WaterRefract = new
        BDWaterRefract(Vector3(File.GetDatad("WaterRefract", index),
                               File.GetDatad("WaterRefract", index+1),
                               File.GetDatad("WaterRefract", index+2)),
                       File.GetDatad("WaterRefract", index+3),
                       File.GetDatad("WaterRefract", index+4),
                       ResCon->GetTexID(tex1), ResCon->GetTexID(tex2), ms);
    LandScape->AttachInFront(WaterRefract);
    LandScape->WaterCollision(WaterRefract);
  }

  for (unsigned int index = 0; index < File.GetSized("Fire"); index += 3) {
    BDFire *Fire = new BDFire(Vector3(File.GetDatad("Fire", index),
                                      File.GetDatad("Fire", index+1),
                                      File.GetDatad("Fire", index+2)),
                              ResCon->GetTexID(File.GetDatai("FireTexture")));
    LandScape->AttachInFront(Fire);
  }

  // intialise player
  Player = new BDPlayer(ResCon);
  Player->position = Vector3(File.GetDatad("StartingPosition", 0),
                             File.GetDatad("StartingPosition", 1),
                             File.GetDatad("StartingPosition", 2));
  Camera->SetPlayer(Player);
  Player->CamPos = &Camera->Position;
  // attach player in correct position
  LandScape->AttachBehind(Player);

  // BALLS
  if (File.GetSized("Ball"))
    BDBall::Load();

  for (unsigned int index = 0; index < File.GetSized("Ball"); index += 4) {
    BDBall* ball = new BDBall(Vector3(File.GetDatad("Ball", index),
                                      File.GetDatad("Ball", index+1),
                                      File.GetDatad("Ball", index+2)),
                              ResCon->GetTexID(unsigned(File.GetDatad("Ball", index+3))));

    LandScape->AttachBehind(ball);
  }

  // KNIVES
  if (File.GetSized("Knive")) {
    BDKnive::Load();
    if (AIEngine == NULL)
      AIEngine = new BDAIEngine(&Player->position);
  }

  for (unsigned int index = 0; index < File.GetSized("Knive"); index += 4) {
    BDKnive* knive = new BDKnive();

    knive->position.x = File.GetDatad("Knive", index);
    knive->position.y = File.GetDatad("Knive", index+1);
    knive->position.z = File.GetDatad("Knive", index+2);

    LandScape->AttachBehind(knive);
    AIEngine->AddAIObject((BDAIObject*)knive);
  }

  // Health Critters
  if (File.GetSized("HealthCritter")) {
    BDHealthCritter::Load();
    if (AIEngine == NULL)
      AIEngine = new BDAIEngine(&Player->position);
  }

  for (unsigned int index = 0; index < File.GetSized("HealthCritter"); index += 4) {
    BDHealthCritter* HealthCritter = new BDHealthCritter(ResCon->GetTexID(unsigned(File.GetDatad("HealthCritter", index+3))));

    HealthCritter->position.x = File.GetDatad("HealthCritter", index);
    HealthCritter->position.y = File.GetDatad("HealthCritter", index+1);
    HealthCritter->position.z = File.GetDatad("HealthCritter", index+2);

    LandScape->AttachBehind(HealthCritter);
    AIEngine->AddAIObject((BDAIObject*)HealthCritter);
  }

  // Clouds
  for (unsigned index = 0; index < File.GetSized("Cloud"); index += 4) {
    Vector3 pos;
    pos.x = File.GetDatad("Cloud", index);
    pos.y = File.GetDatad("Cloud", index+1);
    pos.z = File.GetDatad("Cloud", index+2);

    BDCloud *cl = new BDCloud(pos, ResCon->GetTexID(File.GetDatad("Cloud", index+3)), &Player->position);
    LandScape->AttachBehind((DrawableObject*)cl);
  }

  // Tree
  BDTree *tree = new BDTree();
  tree->position = Vector3(0.0, 15.0, 5.0);
  LandScape->AttachBehind((DrawableObject*)tree);

  bPlayMusic = File.GetDatab("PlayMusic");
  // bgMusic = NULL;
  if (bPlayMusic) {
  }
  ResCon->Load();

  bLoaded = true;
}
//---------------------------------------------------------------------------
void BDWorld::UnloadWorld() {
  if (!bLoaded) return;

  // balls. Unload if one was loaded or not
  BDBall::Unload();
  BDKnive::Unload();

  LandScape->Unload();

  delete LandScape;
  LandScape = NULL;
  Lights.clear();

  if (AIEngine != NULL) {
    delete AIEngine;
    AIEngine = NULL;
  }

  delete ResCon;
  ResCon = NULL;

  bLoaded = false;
}
//---------------------------------------------------------------------------
void BDWorld::Animate(double deltaTime) {
  if (AIEngine)
    AIEngine->Update();

  // BDBall::DrawToPBuffer(deltaTime);
  LandScape->Animate(deltaTime);

  Camera->Animate(deltaTime);
  Camera->CamObject->position = Camera->Position;
  Camera->CamObject->velocity = Camera->Velocity;
  LandScape->OnCollision(Camera->CamObject, deltaTime);
  Camera->Position = Camera->CamObject->position;
  Camera->Velocity = Camera->CamObject->velocity;

  UpdateFadeToMusic(deltaTime);
}
//---------------------------------------------------------------------------
void BDWorld::Draw() {
  // Position the camera (has to come last of the animations)
  Camera->Draw();

  // LIGHTS
  for (unsigned int i = 0; i < Lights.size(); i += 4) {
    float *lightpos = &Lights[i];
    glLightfv(GL_LIGHT0 + i/4, GL_POSITION, lightpos);
  }

  std::vector<BDShadow*> shadows;

  LandScape->Draw(Camera, shadows);

  // BEGIN SHADOW CODE
  glPushAttrib(GL_ENABLE_BIT);
  glDisable(GL_LIGHTING);
  glEnable(GL_BLEND);
  glEnable(GL_STENCIL_TEST);
  glDepthMask(GL_FALSE);
  glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);
  // glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

  glColor4f(0.0,0.0,0.0, 0.5);

  // make drawing the shadow inc stencil buffer by 1
  glStencilFunc(GL_ALWAYS, 1, 0xFFFF);
  glStencilOp(GL_INCR, GL_KEEP, GL_INCR);
  // glStencilOp(GL_REPLACE, GL_REPLACE, GL_REPLACE);
  // glFrontFace(GL_CW);

  for (int n = 0; n < 2; n++) {
    for (int i = 0; i < (int)shadows.size(); i++) {
      glPushMatrix();
      glTranslatef(shadows[i]->position.x, shadows[i]->position.y, shadows[i]->position.z);
      shadows[i]->GetModel()->StaticDraw();
      glPopMatrix();
    }
    glFrontFace(GL_CW);
    // glStencilFunc(GL_ALWAYS, 1, 0xFFFF);
    glStencilOp(GL_DECR, GL_KEEP, GL_DECR);
  }
  glFrontFace(GL_CCW);
  glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);

  // draw alpha blended polygon covering screen
  glMatrixMode(GL_PROJECTION);
  glPushMatrix();
  glLoadIdentity();
  glOrtho(0.0, 1.0, 1.0, 0.0, -1.0, 1.0);

  glMatrixMode(GL_MODELVIEW);
  glPushMatrix();
  glLoadIdentity();
  glStencilFunc(GL_NOTEQUAL, 0, 0xFFFF);
  glStencilOp(GL_KEEP, GL_KEEP, GL_ZERO);

  glBegin(GL_POLYGON);
  glVertex3f(0.0, 0.0, 0.0);
  glVertex3f(0.0, 1.0, 0.0);
  glVertex3f(1.0, 1.0, 0.0);
  glVertex3f(1.0, 0.0, 0.0);
  glEnd();

  glPopMatrix();

  glMatrixMode(GL_PROJECTION);
  glPopMatrix();
  glMatrixMode(GL_MODELVIEW);
  glDepthMask(GL_TRUE);
  glPopAttrib();
  // END SHADOW CODE

  Camera->DrawAlphaObjects(shadows);
}
//---------------------------------------------------------------------------
void BDWorld::UpdateFadeToMusic(float deltaTime) {
  if ((!bPlayMusic) || (iFadeToMusic == iCurrMusic))
    return;
}
//---------------------------------------------------------------------------
bool BDWorld::FadeToMusic(int m) {
  if ((!bPlayMusic) || (m == iCurrMusic))
    return true;

  iFadeToMusic = m;
  return false;
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
