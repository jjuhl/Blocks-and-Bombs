/*
 * Copyright © 2011 by Jesper Juhl
 * Licensed under the terms of the GNU General Public Licence (GPL) version 2.
 */

#include <SDL.h>
#include "helpstate.hh"

HelpState::HelpState()
{
}

HelpState::~HelpState()
{
}

STATE_CHANGE HelpState::handleKey(const SDL_KeyboardEvent&)
{
  return NO_CHANGE;
}

STATE_CHANGE HelpState::update(Uint32)
{
  return NO_CHANGE;
}

void HelpState::draw(SDL_Surface*)
{
}
