#include <iostream>
#include <string>
#include <cstdlib>
#include <SDL.h>
#include <SDL_ttf.h>
#include "except.hh"
#include "states.hh"
#include "menustate.hh"
#include "playstate.hh"
#include "helpstate.hh"
#include "highscorestate.hh"
#include "bbengine.hh"

BBEngine::BBEngine(int width, int height, int bpp)
  : m_updateTimer(0), m_screen(0), m_lastUpdate(SDL_GetTicks()), m_currentState(0)
{
  m_screen = SDL_SetVideoMode(width, height, bpp, SDL_SWSURFACE);
  if (!m_screen)
    throw Exception("Unable to set video mode: " + std::string(SDL_GetError()));

  if (!TTF_WasInit()) {
    if (TTF_Init() == -1)
      throw Exception("Unable to initialize SDL_ttf: " + std::string(TTF_GetError()));
  }

  m_currentState = new MenuState;
}

BBEngine::~BBEngine()
{
  delete m_currentState;
  if (m_updateTimer)
    SDL_RemoveTimer(m_updateTimer);
  if (TTF_WasInit())
    TTF_Quit();
}

int BBEngine::exec()
{
  SDL_Event event;

  // I've found that a common error that I personally make, when
  // launching the game from the commandline, is to inadvertently
  // generate an extra keypress and activate a menu item I did not
  // intend. Just to keep myself from getting annoyed by this we start
  // out by sleeping for 120ms and then draining the event queue
  // before we actually start to process anything - this seems to be
  // good enough to keep me from making that mistake too often.  It's
  // entirely pointless otherwise, and yes it's kinda stupid, but it
  // does the trick for me and I'm the one writing the game after all,
  // so I get to decide ;-)
  SDL_Delay(120);
  while (SDL_PollEvent(&event));

  // Get the update timer running - this is what drives our regular
  // updates/redraws.
  m_updateTimer = SDL_AddTimer(30, updateCallback, 0);

  // Check for input or timeout
  // This is the main event loop that drives everything
  bool quit_it = false;
  bool paused = false;
  PlayState* playstate = 0;
  while (!quit_it) {
    if (!SDL_WaitEvent(&event))
      throw Exception("Error while waiting for SDL events");

    Uint32 now = SDL_GetTicks();
    enum STATE_CHANGE new_state = NO_CHANGE;
    switch (event.type) {
    case SDL_KEYDOWN:
    case SDL_KEYUP: {
      // pass the key to the current state handler
      new_state = m_currentState->handleKey(event.key);
      if (new_state != NO_CHANGE)
        changeStateTo(new_state);
      break;
    }
    case SDL_QUIT:
      // user wants to quit
      quit_it = true;
      break;
    case SDL_USEREVENT:
      // This is our regular update timer, let's go update state and
      // redraw.

      // do updates relevant for the state that we are in
      new_state = m_currentState->update(now - m_lastUpdate);
      m_lastUpdate = now;
      if (new_state != NO_CHANGE)
        changeStateTo(new_state);

      // redraw now that we have potentially updated something
      m_currentState->draw(m_screen);

      // If we are in the play state and the game is paused, reduce
      // our timer tick to 150ms to save a bit of power until the user
      // unpauses.
      playstate = dynamic_cast<PlayState*>(m_currentState);
      if (playstate) {
        if (playstate->isPaused() && !paused) {
          paused = true;
          SDL_RemoveTimer(m_updateTimer);
          m_updateTimer = SDL_AddTimer(150, updateCallback, 0);
        } else if (!playstate->isPaused() && paused) {
          paused = false;
          SDL_RemoveTimer(m_updateTimer);
          m_updateTimer = SDL_AddTimer(30, updateCallback, 0);
        }
      }

      // and update the resulting screen
      SDL_Flip(m_screen);
      break;
    default:
      // unknown event type - really ought to be filtered out; maybe
      // add a warning print or something in the future.
      continue;
    }
  }

  return EXIT_SUCCESS;
}

void BBEngine::changeStateTo(enum STATE_CHANGE new_state)
{
  delete m_currentState;
  switch (new_state) {
  case NO_CHANGE:
    std::cerr << "error: NO_CHANGE state in changeStateTo" << std::endl;
    break;
  case GOTO_MENU:
    m_currentState = new MenuState();
    break;
  case GOTO_PLAY:
    m_currentState = new PlayState();
    break;
  case GOTO_HELP:
    m_currentState = new HelpState();
    break;
  case GOTO_HIGHSCORE:
    m_currentState = new HighscoreState();
    break;
  default:
    throw Exception("Unknown state in changeStateTo");
  }
}

Uint32 updateCallback(Uint32 interval, void*)
{
  // All we do here is just push a event into SDL's event queue so
  // that our main game loop will wake up regularly and update game
  // state and redraw etc.
  SDL_Event event;
  SDL_UserEvent userevent;
  userevent.type = SDL_USEREVENT;
  userevent.code = 0;
  userevent.data1 = NULL;
  userevent.data2 = NULL;

  event.type = SDL_USEREVENT;
  event.user = userevent;

  SDL_PushEvent(&event);
  return interval;
}
