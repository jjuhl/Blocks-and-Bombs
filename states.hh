/*
 * base class for all states in the game; menu, play, help, whatever
 *
 * Copyright © 2011 by Jesper Juhl
 * Licensed under the terms of the GNU General Public Licence (GPL) version 2.
 */

#ifndef BNB_STATES_HH
#define BNB_STATES_HH

#include <SDL.h>

enum STATE_CHANGE {
  NO_CHANGE = 0,
  GOTO_MENU,
  GOTO_PLAY,
  GOTO_HELP,
  GOTO_HIGHSCORE
};

class State {
public:
  State() { }
  virtual ~State() { }
  // Called when a key press is received from the user
  // If keypress should result in major state change this can be
  // indicated by the return value - return NO_CHANGE if we should
  // stay in current state.
  virtual STATE_CHANGE handleKey(const SDL_KeyboardEvent& key) = 0;
  // called by engine regularly and should update state based on
  // elapsed time since last call (passed in argument.
  virtual STATE_CHANGE update(Uint32 delta_time) = 0;
  // must draw the current state to 'screen'. Should only redraw dirty
  // rectangles.
  virtual void draw(SDL_Surface* screen) = 0;
};

#endif
