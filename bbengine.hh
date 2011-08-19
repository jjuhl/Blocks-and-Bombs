/*
 * BBEngine - the Blocks and Bombs engine.
 *
 * Copyright © 2011 by Jesper Juhl
 * Licensed under the terms of the GNU General Public License (GPL) version 2.
 */

#ifndef BNB_BBENGINE_HH
#define BNB_BBENGINE_HH

#include <SDL.h>
#include "states.hh"

class BBEngine;
typedef void (BBEngine::*BBEngineStateHandler)(const SDL_KeyboardEvent& k);

class BBEngine {
public:
  BBEngine(int width, int height, int bpp);
  ~BBEngine();
  // Get the show on the road. Return value is intended to be returned from main.
  int exec();

private:
  BBEngine(const BBEngine&);
  BBEngine& operator=(const BBEngine&);
  void changeStateTo(enum STATE_CHANGE new_state);
  SDL_TimerID m_updateTimer;
  SDL_Surface* m_screen;
  Uint32 m_lastUpdate;
  State* m_currentState;
};

Uint32 updateCallback(Uint32 interval, void* param);

#endif
