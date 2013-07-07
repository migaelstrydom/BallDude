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
#include "OpenGLUnit.h"
#include <SDL/SDL.h>
#include <GL/glu.h>
//---------------------------------------------------------------------------
#define MSGL_DEPTHBITS 16
#define MSGL_STENCILBITS 8
//---------------------------------------------------------------------------
namespace MSOpenGL {
// TEMP
std::string msg;
// END TEMP

// Window creation
int Width;
int Height;
int ColorDepth;
bool FullScreen;
int VideoFlags;
SDL_Surface* MainWindow;

void SetupPixelFormat();
// end window creation

// misc.
void InvertAffine(float mat[16]);
};
//---------------------------------------------------------------------------
void MSOpenGL::Init() {
  SetupPixelFormat();
}
//---------------------------------------------------------------------------
void MSOpenGL::MakeViewport(int w, int h) {
  if (h == 0)
    h = 1;

  glViewport(0, 0, w, h);      // define the viewport
  glMatrixMode(GL_PROJECTION);    // prepare for and then
  glLoadIdentity();               // define the projection

  gluPerspective(40, (float) w / (float) h, 1.0, 600.0);

  glMatrixMode(GL_MODELVIEW);  // back to modelview matrix

  glLoadIdentity();
}
//---------------------------------------------------------------------------
void MSOpenGL::MakeWindow(int pwidth, int pheight, int bpp) {
  FullScreen = false;

  Width = pwidth;
  Height = pheight;
  ColorDepth = bpp;

  MainWindow = SDL_SetVideoMode(Width, Height, ColorDepth, VideoFlags);

  if (MainWindow == NULL) {
    throw BDError(std::string("Failed to Create Window : ") + SDL_GetError());
  }
  SDL_WM_SetCaption("BallDude", "BallDude");
  MakeViewport(Width, Height);
}
//---------------------------------------------------------------------------
void MSOpenGL::MakeFullScreen(int pwidth, int pheight, int bpp) {
  VideoFlags |= SDL_FULLSCREEN;
  MakeWindow(pwidth, pheight, bpp);
  FullScreen = true;

  SDL_ShowCursor(false);
}
//---------------------------------------------------------------------------
void MSOpenGL::Shutdown() {
  SDL_Quit();

  FullScreen = false;
}
//---------------------------------------------------------------------------
void MSOpenGL::SetupPixelFormat() {
  VideoFlags = SDL_OPENGL;
  VideoFlags|= SDL_HWPALETTE;

  const SDL_VideoInfo * VideoInfo = SDL_GetVideoInfo();

  if (VideoInfo == NULL) {
    std::string e = "Failed getting Video Info : ";
    e += SDL_GetError();
    throw BDError(e);
  }

  if (VideoInfo->hw_available)
    VideoFlags |= SDL_HWSURFACE;
  else
    VideoFlags |= SDL_SWSURFACE;


  if (VideoInfo->blit_hw)
    VideoFlags |= SDL_HWACCEL;

  SDL_GL_SetAttribute( SDL_GL_DOUBLEBUFFER, 1 );
  SDL_GL_SetAttribute( SDL_GL_DEPTH_SIZE,   MSGL_DEPTHBITS);
  SDL_GL_SetAttribute( SDL_GL_STENCIL_SIZE, MSGL_STENCILBITS);
  SDL_GL_SetAttribute( SDL_GL_ACCUM_RED_SIZE, 0);
  SDL_GL_SetAttribute( SDL_GL_ACCUM_GREEN_SIZE, 0);
  SDL_GL_SetAttribute( SDL_GL_ACCUM_BLUE_SIZE, 0);
  SDL_GL_SetAttribute( SDL_GL_ACCUM_ALPHA_SIZE, 0);
}
//---------------------------------------------------------------------------
void MSOpenGL::InvertAffine(float mat[16]) {
  mat[3] = mat[7] = mat[11] = mat[12] = mat[13] = mat[14] = 0.0f;
  float dum;

  dum = mat[1];
  mat[1] = mat[4];
  mat[4] = dum;

  dum = mat[2];
  mat[2] = mat[8];
  mat[8] = dum;

  dum = mat[6];
  mat[6] = mat[9];
  mat[9] = dum;
}
//---------------------------------------------------------------------------
void MSOpenGL::SetTextureMatrixReflection(float p) {
  float M[16];
  float CTPSR[16] = {-0.5*p, 0, 0, 0,
                     0.5*p, 0.5*p, 1, p,
                     0, 0.5*p, 0, 0,
                     0.5*p*p, 0.5*p*p, p, p*p};

  // Grab the modelview matrix
  glGetFloatv(GL_MODELVIEW_MATRIX, M);

  InvertAffine(M);

  glMatrixMode(GL_TEXTURE);
  glLoadIdentity();

  glMultMatrixf(CTPSR);
  glMultMatrixf(M);
  glMatrixMode(GL_MODELVIEW);
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
