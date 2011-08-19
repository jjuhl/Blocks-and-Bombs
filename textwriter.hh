/*
 * This class is intended to encapsulate all text writing with TTF
 * fonts for use in the BNBEngine.
 * TTF_Init() must have been called before instantiating this class.
 *
 * Copyright © 2011 by Jesper Juhl
 * Licensed under the terms of the GNU General Public Licence (GPL) version 2.
 */

#ifndef BNB_TEXTWRITER_HH
#define BNB_TEXTWRITER_HH

#include <string>
#include <SDL.h>
#include <SDL_ttf.h>

class TextWriter {
public:
  enum RENDER_MODE {
    SOLID = 0,
    BLENDED
  };

  TextWriter(const std::string& font, int ptsize = 16);
  virtual ~TextWriter();

  // Changes the render mode used with the underlying SDL_ttf
  // library. By default BLENDED mode is always used since it's the
  // nicest looking, but if you need more speed and can accept lower
  // quality text, then you can set the SOLID mode with this function
  // - the shaded mode provided by SDL_ttf is not available through
  // this function.
  TextWriter& setRenderMode(enum RENDER_MODE mode);

  // Set a new point size for the font. Note that this unloads the
  // current font and reloads it again at a new point size. This means
  // that it may fail (and you'll get an exception) if the font file
  // has gone away and in any case it's not fast. If you often need a
  // font at different point sizes it's often better to create
  // multiple instances of this class and specify different point
  // sizes to the constructor, but this function still exists for
  // those one-off size changes that sometimes creep up.
  TextWriter& setPointSize(int ptsize);

  // Sets the colour that text is drawn with. If never called, the
  // default will be black.
  TextWriter& setFontColor(const SDL_Color& col);

  // Returns an SDL_Rect with the 'w' and 'h' members set to the width
  // and height of the text as it would be if rendered with current
  // settings. The values of the 'x' and 'y' members of the returned
  // SDL_Rect are set to zero on success or -1 on error, in which case
  // the values of 'w' and 'h' are undefined.
  SDL_Rect sizeText(const std::string& text);

  // Returns the recommended spacing between lines for the font in
  // use.
  int lineSkip();

  // allocates a new surface and writes the text to it, then returns
  // the new surface. Caller is responsible for freeing
  // (SDL_FreeSurface) the returned surface. Returns a NULL pointer on
  // error.
  SDL_Surface* render(const std::string& text);

  // write text onto the given destination surface at location
  // indicated by the 'dst' rectangle - returns true on success, false
  // on errors.
  bool render(SDL_Surface* dest, SDL_Rect* dst, const std::string& text);

private:
  TextWriter(const TextWriter&);
  TextWriter& operator=(const TextWriter&);
  const std::string m_fontFile;
  RENDER_MODE m_mode;
  TTF_Font* m_font;
  SDL_Color m_color;
};

#endif
