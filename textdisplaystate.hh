/*
 * Class for displaying larger amounts of text. Will automatically
 * wrap the text and react to key up and down for scrolling the text.
 *
 * Copyright © 2011 by Jesper Juhl
 * Licensed under the terms of the GNU General Public License (GPL) version 2.
 */

#ifndef BNB_TEXTDISPLAYSTATE_HH
#define BNB_TEXTDISPLAYSTATE_HH

#include <SDL.h>
#include <vector>
#include "states.hh"
#include "textwriter.hh"

class TextDisplayState : public State {
public:
  TextDisplayState(const std::string& text);
  ~TextDisplayState();
  STATE_CHANGE handleKey(const SDL_KeyboardEvent& key);
  STATE_CHANGE update(Uint32 delta_time);
  void draw(SDL_Surface* screen);

private:
  TextDisplayState(const TextDisplayState&);
  TextDisplayState& operator=(const TextDisplayState&);
  void parseLines(Uint16 width);

  const std::string m_text;
  SDL_Surface* m_background;
  TextWriter* m_textWriter;
  std::vector<std::string> m_lines;
  std::vector<std::string>::const_iterator m_curLine;
  Uint16 m_margin;
  bool m_reachedEnd;
};

#endif
