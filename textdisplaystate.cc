/*
 * Copyright © 2011 by Jesper Juhl
 * Licensed under the terms of the GNU General Public License (GPL) version 2.
 */

#include <SDL.h>
#include "textdisplaystate.hh"
#include "resources.hh"
#include <sstream>
#include <iterator>

#include <iostream>

TextDisplayState::TextDisplayState(const std::string& text)
  : m_text(text),
    m_background(IMG_LoadDisplayFormat("default-background.png")),
    m_textWriter(new TextWriter("whitrabt.ttf", 20)),
    m_lines(),
    m_curLine(),
    m_margin(20),
    m_reachedEnd(true)
{
}

TextDisplayState::~TextDisplayState()
{
  SDL_FreeSurface(m_background);
  delete m_textWriter;
}

STATE_CHANGE TextDisplayState::handleKey(const SDL_KeyboardEvent& key)
{
  if (key.type == SDL_KEYUP)
    return NO_CHANGE;

  switch (key.keysym.sym) {
  case SDLK_DOWN:
    if (!m_reachedEnd)
      m_curLine++;
    return NO_CHANGE;
  case SDLK_UP:
    if (m_curLine != m_lines.begin())
      m_curLine--;
    return NO_CHANGE;
  default:
    break;
  }

  // All other keys means the user is tired of reading
  return GOTO_MENU;
}

STATE_CHANGE TextDisplayState::update(Uint32)
{
  return NO_CHANGE;
}

void TextDisplayState::draw(SDL_Surface* screen)
{
  const SDL_Color color = { 50, 250, 50, 0 };
  SDL_BlitSurface(m_background, 0, screen, 0);

  if (m_lines.empty())
    parseLines(screen->w);

  m_reachedEnd = true;

  Uint16 y = m_margin;

  // Iterate over each line of text and draw it to the screen using
  // the TextWriter class.
  for (std::vector<std::string>::const_iterator it = m_curLine;
       it != m_lines.end(); ++it) {
    SDL_Rect rect = m_textWriter->sizeText(*it);
    if ((y += rect.h + m_textWriter->lineSkip()) >= screen->h - m_margin) {
      // If we were to draw one more line of text, we would exceed the
      // screen height. Get out and remember that there is more text
      // to be displayed if the user presses the down key.
      m_reachedEnd = false;
      break;
    }

    rect.y = y;
    rect.x = m_margin;
    m_textWriter->setFontColor(color);
    m_textWriter->render(screen, &rect, *it);
  }
}

void TextDisplayState::parseLines(Uint16 width)
{
  // Split the input text into lines to respect newlines and then
  // build up lines of string not exceeding the width obtained from
  // the screen.
  std::istringstream istream(m_text);
  std::string input;
  while(std::getline(istream, input)) {
    std::istringstream lstream(input, std::istringstream::in);
    std::string line;
    std::string word;
    while (lstream >> word) {
      SDL_Rect rect = m_textWriter->sizeText(line.empty() ? word : line + " " + word);
      if (rect.w + 2 * m_margin > width) {
        m_lines.push_back(line);
        line = word;
      } else {
        line += line.empty() ? word : " " + word;
      }
    }
    m_lines.push_back(line);
  }
  m_curLine = m_lines.begin();
}
