#pragma once

#include "stdafx.h"
#include "SDL.h"

class DisplayImage
{
public:
  DisplayImage(void);
  ~DisplayImage(void);
  bool show(Image);
private:
  SDL_Surface* screen_;
};
