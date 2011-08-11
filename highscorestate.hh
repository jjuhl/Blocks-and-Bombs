#ifndef BNB_HIGHSCORESTATE_HH
#define BNB_HIGHSCORESTATE_HH

#include <SDL.h>
#include "states.hh"

class HighscoreState : public State {
public:
  HighscoreState();
  ~HighscoreState();
  STATE_CHANGE handleKey(const SDL_KeyboardEvent& key);
  STATE_CHANGE update(Uint32 delta_time);
  void draw(SDL_Surface* screen);
};

#endif
