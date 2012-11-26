#include "StdAfx.h"

#include <SDL.h>


DisplayImage::DisplayImage(void)
{
  SDL_Init(SDL_INIT_VIDEO);
  screen_ = SDL_SetVideoMode(1024, 768, 24, SDL_SWSURFACE);
  SDL_WM_SetCaption("MJPEG Stream", 0); 
  if (screen_ == NULL) {
    fprintf(stderr, "SDL_SetVideoMode failed: %s\n", SDL_GetError());
    exit(1);
  }
}


DisplayImage::~DisplayImage(void)
{
}


bool DisplayImage::show(Image image) {
  SDL_LockSurface(screen_);

  u8* screen_ptr = (u8*)screen_->pixels;
  u8* image_ptr(&image.image_data[0]);

  for (u32 y(0); y < image.h; ++y) {
    for (u32 x(0); x < image.w; ++x) {

      u8 r(*image_ptr++);
      u8 g(*image_ptr++);
      u8 b(*image_ptr++);

      screen_ptr[x * 3 + 0 + y * screen_->pitch] = b;
      screen_ptr[x * 3 + 1 + y * screen_->pitch] = g;
      screen_ptr[x * 3 + 2 + y * screen_->pitch] = r;
    }
  }

  SDL_UnlockSurface(screen_);
  SDL_Flip(screen_);

  SDL_Event event;
  if (SDL_PollEvent(&event)) {
    switch (event.type) {
    case SDL_QUIT:
      return false;
    }
  }
  return true;
}
