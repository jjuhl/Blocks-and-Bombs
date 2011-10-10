/*
 * Copyright © 2011 by Jesper Juhl
 * Licensed under the terms of the GNU General Public License (GPL) version 2.
 */

#include <SDL.h>
#include <SDL_image.h>
#include <iostream>
#include <list>
#include "except.hh"
#include "textwriter.hh"
#include "resources.hh"
#include "menustate.hh"
#include "config.h"

MenuState::MenuState()
  : m_background(IMG_LoadDisplayFormat("menu-background.png")),
    m_textWriter(new TextWriter("whitrabt.ttf", 40)), m_items()
{
  if (!m_background)
    throw Exception("Failed to load menu background: " + std::string(IMG_GetError()));

  // Build the menu
  m_items.push_back(MenuItem("New Game", COLOR_OF_ACTIVE, true, NEW_GAME));
  m_items.push_back(MenuItem("About", COLOR_OF_INACTIVE, false, SHOW_ABOUT));
  /*
  m_items.push_back(MenuItem("Show Highscore", COLOR_OF_INACTIVE, false, SHOW_HIGHSCORE));
  m_items.push_back(MenuItem("Show Help", COLOR_OF_INACTIVE, false, SHOW_HELP));
  */
  m_items.push_back(MenuItem("Quit Game", COLOR_OF_INACTIVE, false, QUIT_GAME));
}

MenuState::~MenuState()
{
  SDL_FreeSurface(m_background);
  delete m_textWriter;
}

STATE_CHANGE MenuState::handleKey(const SDL_KeyboardEvent& key)
{
  if (key.type == SDL_KEYUP)
    return NO_CHANGE;

  switch (key.keysym.sym) {
  case SDLK_ESCAPE: {
    SDL_Event event;
    event.type = SDL_QUIT;
    SDL_PushEvent(&event);
    break;
  }
  case SDLK_UP: {
    std::list<MenuItem>::iterator it = m_items.begin();
    for (; it != m_items.end(); ++it) {
      if (it->cur && it != m_items.begin()) {
        it->cur = false;
        --it;
        it->cur = true;
        break;
      }
    }
    break;
  }
  case SDLK_DOWN: {
    std::list<MenuItem>::iterator it = m_items.begin();
    for (; it != m_items.end(); ++it) {
      if (it->cur) {
        std::list<MenuItem>::iterator it_tmp = it;
        ++it_tmp;
        if (it_tmp != m_items.end()) {
          it->cur = false;
          ++it;
          it->cur = true;
        }
        break;
      }
    }
    break;
  }
  case SDLK_RETURN:
  case SDLK_KP_ENTER:
  case SDLK_SPACE: {
    std::list<MenuItem>::iterator it = m_items.begin();
    for (; it != m_items.end(); ++it) {
      if (it->cur) {
        switch (it->act) {
        case QUIT_GAME: {
          SDL_Event event;
          event.type = SDL_QUIT;
          SDL_PushEvent(&event);
          break;
        }
        case NEW_GAME:
          return GOTO_PLAY;
        case SHOW_HIGHSCORE:
          return GOTO_HIGHSCORE;
        case SHOW_HELP:
          return GOTO_HELP;
        case SHOW_ABOUT:
          return GOTO_ABOUT;
        }
        break;
      }
    }
  }
  default:
    // A key we don't handle - ignore.
    break;
  }

  return NO_CHANGE;
}

STATE_CHANGE MenuState::update(Uint32 delta_time)
{
  // Update the color of all menu items relative to time passed
  long tmpr = (COLOR_OF_ACTIVE.r - COLOR_OF_INACTIVE.r)
    * (delta_time / 300.0);
  long tmpg = (COLOR_OF_ACTIVE.g - COLOR_OF_INACTIVE.g)
    * (delta_time / 300.0);
  long tmpb = (COLOR_OF_ACTIVE.b - COLOR_OF_INACTIVE.b)
    * (delta_time / 300.0);

  std::list<MenuItem>::iterator it = m_items.begin();
  for (; it != m_items.end(); ++it) {
    if (it->cur) {
      if ((it->col.r + tmpr) > COLOR_OF_ACTIVE.r)
        it->col.r = COLOR_OF_ACTIVE.r;
      else
        it->col.r += tmpr;

      if ((it->col.g + tmpg) > COLOR_OF_ACTIVE.g)
        it->col.g = COLOR_OF_ACTIVE.g;
      else
        it->col.g += tmpg;

      if ((it->col.b + tmpb) > COLOR_OF_ACTIVE.b)
        it->col.b = COLOR_OF_ACTIVE.b;
      else
        it->col.b += tmpb;
    } else {
      if ((it->col.r - tmpr) < COLOR_OF_INACTIVE.r)
        it->col.r = COLOR_OF_INACTIVE.r;
      else
        it->col.r -= tmpr;

      if ((it->col.g - tmpg) < COLOR_OF_INACTIVE.g)
        it->col.g = COLOR_OF_INACTIVE.g;
      else
        it->col.g -= tmpg;

      if ((it->col.b - tmpb) < COLOR_OF_INACTIVE.b)
        it->col.b = COLOR_OF_INACTIVE.b;
      else
        it->col.b -= tmpb;
    }
  }

  return NO_CHANGE;
}

void MenuState::draw(SDL_Surface* screen)
{
  SDL_BlitSurface(m_background, 0, screen, 0);

  // Draw the menu text
  int y_off = 222;
  std::list<MenuItem>::iterator it = m_items.begin();
  for (; it != m_items.end(); ++it) {
    std::string render_text = it->txt;
    if (it->cur)
      render_text = "> " + render_text + " <";
    SDL_Rect r = m_textWriter->sizeText(render_text);
    r.y = y_off;
    r.x = (screen->w - r.w) / 2;
    m_textWriter->setFontColor(it->col);
    m_textWriter->render(screen, &r, render_text);
    y_off += r.h;
    y_off += m_textWriter->lineSkip();
  }
}
