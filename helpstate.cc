#include <SDL.h>
#include "helpstate.hh"

HelpState::HelpState()
{
}

HelpState::~HelpState()
{
}

STATE_CHANGE HelpState::handleKey(const SDL_KeyboardEvent& key)
{
  return NO_CHANGE;
}

STATE_CHANGE HelpState::update(Uint32 delta_time)
{
  return NO_CHANGE;
}

void HelpState::draw(SDL_Surface* screen)
{
}
