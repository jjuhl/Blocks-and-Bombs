/*
 * Copyright © 2011 by Jesper Juhl
 * Licensed under the terms of the GNU General Public License (GPL) version 2.
 */

#include <iostream>
#include <cstdlib>
#include <SDL.h>
#include <libconfig.h++>
#include "bbengine.hh"
#include "config.h"
#include "except.hh"

// ensure that SDL is always shut down properly, no matter how we terminate
class SDLWrap {
public:
  SDLWrap(Uint32 sdl_flags)
  {
    if (SDL_Init(sdl_flags))
      throw Exception("Unable to initialize SDL: "
                      + std::string(SDL_GetError()));
  }
  ~SDLWrap() { SDL_Quit(); }
};

int main(int argc, char* argv[])
{
  SDLWrap sdl(SDL_INIT_TIMER|SDL_INIT_VIDEO);

  SDL_WM_SetCaption("Blocks and Bombs", "Blocks and Bombs");
  // Setting the icon must happen before SDL_SetVideoMode
  SDL_Surface* icon = SDL_LoadBMP(RESOURCES_DIR"images/icon.bmp");
  if (icon) {
    SDL_WM_SetIcon(icon, NULL);
    SDL_FreeSurface(icon);
  }

  BBEngine app(800, 600, 32);
  return app.exec();
}
