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
#include "ResourceControlUnit.h"
#include "Texture.h"
#include "BDError.h"
#include <GL/gl.h>
//---------------------------------------------------------------------------
BDResourceControl::BDResourceControl() {
  ResFile = new BDFile();
  ResFile->ReadData("Resources.txt");

  // Textures
  NumTexes = ResFile->GetSizes("Textures");
  unsigned int *tex = new unsigned int[NumTexes];
  glGenTextures(NumTexes, tex);
  texID = tex[0];
  delete tex;

  UsedTexes.resize(NumTexes, false);
}
//---------------------------------------------------------------------------
BDResourceControl::~BDResourceControl() {
  for (unsigned i = texID; i < texID + NumTexes; i++)
    glDeleteTextures(1, &i);
}
//---------------------------------------------------------------------------
void BDResourceControl::Load() {
  glEnable(GL_TEXTURE_2D);
  BDTexture texture;

  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

  for (unsigned i = 0; i < NumTexes; i++) {
    if (UsedTexes[i]) {
      if (!texture.LoadTexture(
              ResFile->GetDatas("Textures", i).c_str(),
              ResFile->GetDatab("TexAlpha", i)))
        throw BDError("Unable to load texture! BDResourceControl::Load!");

      glBindTexture(GL_TEXTURE_2D, i + texID);

      gluBuild2DMipmaps(GL_TEXTURE_2D,
                        ResFile->GetDatab("TexAlpha", i) ? GL_RGBA : GL_RGB,
                        texture.width, texture.height,
                        ResFile->GetDatab("TexAlpha", i) ? GL_RGBA : GL_RGB,
                        GL_UNSIGNED_BYTE,
                        texture.data);

      texture.Unload();
    }
  }
  glDisable(GL_TEXTURE_2D);
  delete ResFile;
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
