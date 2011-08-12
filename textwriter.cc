#include <iostream>
#include <string>
#include <SDL.h>
#include <SDL_ttf.h>
#include "except.hh"
#include "textwriter.hh"

TextWriter::TextWriter(const std::string& font, int ptsize)
  : m_fontFile(font), m_mode(BLENDED)
{
  m_font = TTF_OpenFont(m_fontFile.c_str(), ptsize);
  if (!m_font)
    throw Exception("Unable to open font '" + m_fontFile + "': "
                    + std::string(TTF_GetError()));
  memset(&m_color, 0, sizeof(m_color));
}

TextWriter::~TextWriter()
{
  TTF_CloseFont(m_font);
}

TextWriter& TextWriter::setRenderMode(enum RENDER_MODE mode)
{
  m_mode = mode;

  return *this;
}

TextWriter& TextWriter::setPointSize(int ptsize)
{
  TTF_CloseFont(m_font);
  m_font = TTF_OpenFont(m_fontFile.c_str(), ptsize);
  if (!m_font)
    throw Exception("Unable to re-open font '" + m_fontFile
                    + "' in order to change point size: "
                    + std::string(TTF_GetError()));

  return *this;
}

TextWriter& TextWriter::setFontColor(const SDL_Color& col)
{
  m_color = col;

  return *this;
}

SDL_Rect TextWriter::sizeText(const std::string& text)
{
  int w;
  int h;
  SDL_Rect retval;

  retval.x = 0;
  retval.y = 0;
  if (TTF_SizeText(m_font, text.c_str(), &w, &h) == -1) {
    retval.w = -1;
    retval.h = -1;
  }
  retval.w = w;
  retval.h = h;

  return retval;
}

int TextWriter::lineSkip()
{
  return TTF_FontLineSkip(m_font);
}

SDL_Surface* TextWriter::render(const std::string& text)
{
  SDL_Color tmp_col = m_color;
  SDL_Surface* text_surf;
  switch (m_mode) {
  case SOLID:
    text_surf = TTF_RenderText_Solid(m_font, text.c_str(), tmp_col);
    break;
  case BLENDED:
    text_surf = TTF_RenderText_Blended(m_font, text.c_str(), tmp_col);
    break;
  }
  return text_surf;
}

bool TextWriter::render(SDL_Surface* dest, SDL_Rect* dst, const std::string& text)
{
  SDL_Surface* text_surf = render(text);

  if (!text_surf)
    return false;

  int err = SDL_BlitSurface(text_surf, 0, dest, dst);
  SDL_FreeSurface(text_surf);

  return !!err;
}
