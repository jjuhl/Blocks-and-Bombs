#include <iostream>
#include <cstdlib>
#include <SDL.h>
#include <libconfig.h++>
#include "bbengine.hh"
#include "config.h"

// ensure that SDL is always shut down properly, no matter how we terminate
class SDLQuit {
public:
  SDLQuit() {}
  ~SDLQuit() { SDL_Quit(); }
};

int main(int argc, char* argv[])
{
  Uint32 sdl_flags = SDL_INIT_TIMER|SDL_INIT_VIDEO;
  int err = SDL_Init(sdl_flags);
  if (err) {
    std::cerr << "Unable to initialize SDL: " << SDL_GetError() << std::endl;
    return EXIT_FAILURE;
  }
  SDLQuit sq;

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
