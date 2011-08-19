/*
 * Copyright © 2011 by Jesper Juhl
 * Licensed under the terms of the GNU General Public Licence (GPL) version 2.
 */

#ifndef BNB_HELPSTATE_HH
#define BNB_HELPSTATE_HH

#include <SDL.h>
#include "states.hh"

class HelpState : public State {
public:
  HelpState();
  ~HelpState();
  STATE_CHANGE handleKey(const SDL_KeyboardEvent& key);
  STATE_CHANGE update(Uint32 delta_time);
  void draw(SDL_Surface* screen);
};

#endif
