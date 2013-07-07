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
#include "Texture.h"
#include <SDL.h>
#include <SDL/SDL_image.h>
#include <iostream>
//---------------------------------------------------------------------------
bool BDTexture::LoadTexture(const std::string filename, bool alpha) {
  //make sure that SDL was initialised
  if(!SDL_WasInit(0))
    return false;

  //load the image onto an SDL surface
  surface = IMG_Load((std::string("Data/Textures/")+filename).c_str());

  //if there was an error, report it and exit
  if (!surface) {
    return false;
  }

  // thanks to some bug that means I can't set GL_BGR
  int red_offset = 0;
  int blue_offset = 2;

  //get format of data
  if (surface->format->BytesPerPixel == 4) {
    if (surface->format->Rmask == 0x000000ff) {
      pixel_format = GL_RGBA;
      red_offset = 2;
      blue_offset = 0;
    } else {
      pixel_format = GL_RGBA;
    }
  } else if (surface->format->BytesPerPixel == 3) {
    if (surface->format->Rmask == 0x000000ff) {
      pixel_format = GL_RGB;
      red_offset = 2;
      blue_offset = 0;
    } else {
      pixel_format = GL_RGB;
    }
  } else {
    return false;
  }

  width = surface->w;
  height = surface->h;
  //data = surface->pixels;
  int bpp = surface->format->BytesPerPixel;
  data = new unsigned char[width*height*surface->format->BytesPerPixel];
  for(int i = 0; i < width; i++) {
    for(int j = 0; j < height; j++) {
      data[((width-1-i)*height+j)*bpp+0] =
        static_cast<unsigned char*>(surface->pixels)[(i*height+j)*bpp+blue_offset];
      data[((width-1-i)*height+j)*bpp+1] =
        static_cast<unsigned char*>(surface->pixels)[(i*height+j)*bpp+1];
      data[((width-1-i)*height+j)*bpp+2] =
        static_cast<unsigned char*>(surface->pixels)[(i*height+j)*bpp+red_offset];

      if(bpp == 4) {
        data[((width-1-i)*height+j)*bpp+3] =
            static_cast<unsigned char*>(surface->pixels)[(i*height+j)*bpp+3];
      }
    }
  }

  return true;
}
//---------------------------------------------------------------------------
void BDTexture::Unload() {
  if (surface) {
    SDL_FreeSurface(surface);
  }

  delete[] data;
  data = NULL;
  surface = NULL;
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
