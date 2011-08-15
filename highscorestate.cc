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
