/*
 * Copyright © 2011 by Jesper Juhl
 * Licensed under the terms of the GNU General Public License (GPL) version 2.
 */

#include <SDL.h>
#include "highscorestate.hh"

HighscoreState::HighscoreState()
{
}

HighscoreState::~HighscoreState()
{
}

STATE_CHANGE HighscoreState::handleKey(const SDL_KeyboardEvent&)
{
  return NO_CHANGE;
}

STATE_CHANGE  HighscoreState::update(Uint32)
{
  return NO_CHANGE;
}

void HighscoreState::draw(SDL_Surface*)
{
}
