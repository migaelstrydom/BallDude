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
#include "FontUnit.h"
#include <GL/glu.h>
#include <vector>
//---------------------------------------------------------------------------
#if SDL_BYTEORDER == SDL_BIG_ENDIAN
#define RMASK 0xFF000000
#define GMASK 0x00FF0000
#define BMASK 0x0000FF00
#define AMASK 0x000000FF
#else
#define RMASK 0x000000FF
#define GMASK 0x0000FF00
#define BMASK 0x00FF0000
#define AMASK 0xFF000000
#endif
#define FONT_SIZE 23
#define FONT_PIXEL_HEIGHT   32  //64 the height that the above font size is in pixels
//---------------------------------------------------------------------------
MSFont::MSFont() {
  bInited = false;
}
//---------------------------------------------------------------------------
MSFont::MSFont(const char* name) {
  bInited = false;
  Init(name);
}
//---------------------------------------------------------------------------
MSFont::~MSFont() {
  UnInit();
}
//---------------------------------------------------------------------------
void MSFont::InitSystem() {
  // initialise engine if not already initialised
  if (!TTF_WasInit()) {
    if (TTF_Init() == -1)
      throw BDError("Unable to initialise TTF!");
  }
}
//---------------------------------------------------------------------------
void MSFont::DeInitSystem() {
  // quit the font system
  if (TTF_WasInit())
    TTF_Quit();
}
//---------------------------------------------------------------------------
void MSFont::Init(const char *name) {
  if (bInited)
    throw BDError("Font already initialised!");

  // means this specific font was initialised
  bInited = true;

  // now initialise this specific font
  font = TTF_OpenFont(name, FONT_SIZE);
  if (font == NULL) {
    throw BDError(TTF_GetError());
  }
}
//---------------------------------------------------------------------------
void MSFont::UnInit() {
  if (bInited) {
    TTF_CloseFont(font);

    font = NULL;
    bInited = false;
  }
}
//---------------------------------------------------------------------------
void MSFont::DisplayTextBox(MSTextTexture tt) {
  // MESSAGE BOX
  // Push the neccessary Matrices in the stack
  glMatrixMode(GL_PROJECTION);
  glPushMatrix();
  glLoadIdentity();
  glOrtho(0.0, 640.0, 480.0, 0.0, -1.0, 1.0);

  glMatrixMode(GL_MODELVIEW);
  glPushMatrix();
  glLoadIdentity();

  glPushAttrib(GL_ENABLE_BIT);
  // Always Draw in Front
  glDisable(GL_DEPTH_TEST);
  glDisable(GL_CULL_FACE);
  glDisable(GL_LIGHTING);

  glEnable(GL_BLEND);

  glTranslatef(320.0, 400.0, 0.0);
  glColor4f(0.0, 0.5, 1.0, 0.65);
  glRectf(-300.0, -60.0, 300.0, 60.0);

  glDisable(GL_TEXTURE_2D);
  glLineWidth(3.0);
  glColor4f(0.0, 0.0, 1.0, 0.65);
  glBegin(GL_LINE_LOOP);
  glVertex3f(-300.0, -60.0, 0.0);
  glVertex3f(300.0, -60.0, 0.0);
  glVertex3f(300.0, 60.0, 0.0);
  glVertex3f(-300.0, 60.0, 0.0);
  glEnd();

  glEnable(GL_ALPHA_TEST);
  glAlphaFunc(GL_GREATER, 0);
  glEnable(GL_TEXTURE_2D);
  glBindTexture(GL_TEXTURE_2D, tt.texID);
  float xpos = float(tt.tW)/2;
  float ypos = float(tt.tH)/2;
  glColor3f(0.85, 0.925, 1.0);

  glBegin(GL_POLYGON);
  glTexCoord2f(0.0, 0.0);
  glVertex3f(-xpos, -ypos, 0.0);
  glTexCoord2f(float(tt.tW)/tt.Width, 0.0);
  glVertex3f(xpos, -ypos, 0.0);
  glTexCoord2f(float(tt.tW)/tt.Width, float(tt.tH)/tt.Height);
  glVertex3f(xpos, ypos, 0.0);
  glTexCoord2f(0.0, float(tt.tH)/tt.Height);
  glVertex3f(-xpos, ypos, 0.0);
  glEnd();


  // Return to previous Matrix and Attribute states
  glMatrixMode(GL_PROJECTION);
  glPopMatrix();
  glMatrixMode(GL_MODELVIEW);
  glPopMatrix();
  glPopAttrib();
  glDisable(GL_BLEND);
}
//---------------------------------------------------------------------------
SDL_Surface* MSFont::CreateSurface(std::string txt) {
  SDL_Color col = { 0xFF, 0xFF, 0xFF, 0xFF };
  SDL_Surface *ts = TTF_RenderText_Blended(font, txt.c_str(), col);
  SDL_Surface *surf;
  int w = 1, h = 1;

  while(w < ts->w)
    w <<= 1;

  while(h < ts->h)
    h <<= 1;

  surf = SDL_CreateRGBSurface(SDL_SWSURFACE, w, h, 32, RMASK, GMASK, BMASK, AMASK);

  if (surf == NULL) {
    throw BDError("Out of memory! MSFont::CreateSurface");
  }

  SDL_Rect rect;
  rect.x = 0;
  rect.y = 0;

  SDL_SetAlpha(ts, 0, 0);
  SDL_BlitSurface(ts, NULL, surf, &rect);
  SDL_FreeSurface(ts);

  return surf;
}
//---------------------------------------------------------------------------
// returns the rhs text coordinate, left being 0.0.
MSTextTexture MSFont::CreateTextImage(std::string txt) {
  MSTextTexture tt;
  SDL_Color col = { 0xFF, 0xFF, 0xFF, 0xFF };
  SDL_Surface *ts = TTF_RenderText_Blended(font, txt.c_str(), col);
  SDL_Surface *surf;
  int w = 1, h = 1;

  while(w < ts->w)
    w <<= 1;

  while(h < ts->h)
    h <<= 1;

  surf = SDL_CreateRGBSurface(SDL_SWSURFACE, w, h, 32, RMASK, GMASK, BMASK, AMASK);

  if (surf == NULL) {
    throw BDError("Out of memory! MSFont::CreateSurface");
  }

  SDL_Rect rect;
  rect.x = 0;
  rect.y = 0;

  SDL_SetAlpha(ts, 0, 0);
  SDL_BlitSurface(ts, NULL, surf, &rect);

  tt.tW = ts->w;
  tt.tH = FONT_PIXEL_HEIGHT;

  SDL_FreeSurface(ts);

  // create texture
  glGenTextures(1, &tt.texID);
  glBindTexture(GL_TEXTURE_2D, tt.texID);
  gluBuild2DMipmaps(GL_TEXTURE_2D,
                    GL_RGBA,
                    surf->w, surf->h,
                    GL_RGBA,
                    GL_UNSIGNED_BYTE,
                    surf->pixels);

  SDL_FreeSurface(surf);
  return tt;
}
//---------------------------------------------------------------------------
// Takes the requested width & height in characters and returns the
// width & height in pixels
MSTextTexture MSFont::CreateTextBoxImage(std::string txt, int width) {
  int i;
  int pixelwidth = 0, idx, prev, height;
  std::vector<std::pair<int, int> > ws;
  MSTextTexture tt;

  txt += " ";
  // divide up text
  prev = 0;
  i = 0;
  while(i != txt.size()-1) {
    i = std::min(prev + width, (int)txt.size()-1);
    while(!isspace(txt[i]) && (i > prev))
      i--;

    while(isspace(txt[i]) && (i > prev))
      i--;

    if (i == prev)
      i = std::min(prev + width, (int)txt.size()-1);

    ws.push_back(std::pair<int, int>(prev, i));

    if (i - prev + 1 > pixelwidth) {
      pixelwidth = i - prev + 1;
      idx = ws.size()-1;
    }

    if (i < txt.size()-1)
      i++;
    while(isspace(txt[i]) && (i < txt.size()-1))
      i++;

    prev = i;
  }

  if (ws.size() == 0)
    throw BDError("Invalid text in textbox!");

  // initialise some stuff
  SDL_Rect rect;
  SDL_Surface *lsurf, *tsurf;
  SDL_Color col = { 0xFF, 0xFF, 0xFF, 0xFF };
  lsurf = TTF_RenderText_Blended(font,
                                 txt.substr(ws[idx].first,
                                            ws[idx].second-ws[idx].first+1).c_str(),
                                 col);

  // find height
  height = 1;
  while(height < ws.size())
    height <<= 1;

  height *= FONT_PIXEL_HEIGHT;
  tt.tH = ws.size()*FONT_PIXEL_HEIGHT;
  tt.Height = height;

  // find width
  width = 1;
  while(width < lsurf->w)
    width <<= 1;
  tt.tW = lsurf->w;
  tt.Width = width;

  tsurf = SDL_CreateRGBSurface(SDL_SWSURFACE, width, height, 32,
                               RMASK, GMASK, BMASK, AMASK);
  if (tsurf == NULL) {
    throw BDError("Out of memory! MSFont::CreateSurface");
  }

  width = lsurf->w;

  rect.x = 0;
  rect.y = idx*FONT_PIXEL_HEIGHT;
  SDL_SetAlpha(lsurf, 0, 0);
  SDL_BlitSurface(lsurf, NULL, tsurf, &rect);
  SDL_FreeSurface(lsurf);

  for (i = 0; i < ws.size(); i++) {
    if (i == idx)
      continue;

    lsurf = TTF_RenderText_Blended(font,
                                   txt.substr(ws[i].first, ws[i].second-ws[i].first+1).c_str(), col);

    rect.x = (width - lsurf->w) >> 1;
    rect.y = i*FONT_PIXEL_HEIGHT;
    SDL_SetAlpha(lsurf, 0, 0);
    SDL_BlitSurface(lsurf, NULL, tsurf, &rect);
    SDL_FreeSurface(lsurf);
  }

  // create texture
  glGenTextures(1, &tt.texID);
  glBindTexture(GL_TEXTURE_2D, tt.texID);
  gluBuild2DMipmaps(GL_TEXTURE_2D,
                    GL_RGBA,
                    tsurf->w, tsurf->h,
                    GL_RGBA,
                    GL_UNSIGNED_BYTE,
                    tsurf->pixels);

  SDL_FreeSurface(tsurf);
  return tt;
}
//---------------------------------------------------------------------------
