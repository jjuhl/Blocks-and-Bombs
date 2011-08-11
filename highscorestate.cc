#include <SDL.h>
#include "highscorestate.hh"

HighscoreState::HighscoreState()
{
}

HighscoreState::~HighscoreState()
{
}

STATE_CHANGE HighscoreState::handleKey(const SDL_KeyboardEvent& key)
{
  return NO_CHANGE;
}

STATE_CHANGE  HighscoreState::update(Uint32 delta_time)
{
  return NO_CHANGE;
}

void HighscoreState::draw(SDL_Surface* screen)
{
}
