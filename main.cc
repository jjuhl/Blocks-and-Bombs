#include <iostream>
#include <cstdlib>
#include <SDL.h>
#include "options.hh"
#include "bbengine.hh"

// ensure that SDL is always shut down properly, no matter how we terminate
class SDLQuit {
public:
  SDLQuit() {}
  ~SDLQuit() { SDL_Quit(); }
};

int main(int argc, char** argv)
{
  Options my_options(argc, argv);

  Uint32 sdl_flags = SDL_INIT_TIMER|SDL_INIT_VIDEO;
  if (!my_options.hasOption("nosound"))
    sdl_flags |= SDL_INIT_AUDIO;
  int err = SDL_Init(sdl_flags);
  if (err) {
    std::cerr << "Unable to initialize SDL: " << SDL_GetError() << std::endl;
    return EXIT_FAILURE;
  }
  SDLQuit sq;

  SDL_WM_SetCaption("Blocks and Bombs", "Blocks and Bombs");
  // Setting the icon must happen before SDL_SetVideoMode
  SDL_Surface* icon = SDL_LoadBMP("resources/icon.bmp");
  if (icon) {
    SDL_WM_SetIcon(icon, NULL);
    SDL_FreeSurface(icon);
  }

  BBEngine app(800, 600, 32);
  return app.exec();
}
