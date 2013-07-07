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
#ifndef _TEXTURE_H
#define _TEXTURE_H
//---------------------------------------------------------------------------
#include <SDL/SDL_image.h>
#include <GL/gl.h>
#include <string>
//---------------------------------------------------------------------------
class BDTexture {
private:
  SDL_Surface *surface;

public:
  int width;
  int height;
  unsigned char* data;
  GLenum pixel_format;

  BDTexture() : surface(NULL) { }
  ~BDTexture() { Unload(); }

  bool LoadTexture(const std::string filename, bool alpha = false);
  void Unload();
};
//---------------------------------------------------------------------------
#endif
//---------------------------------------------------------------------------
