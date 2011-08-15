#ifndef BNB_MENUSTATE_HH
#define BNB_MENUSTATE_HH

#include <string>
#include <list>
#include <SDL.h>
#include "states.hh"
#include "textwriter.hh"

const SDL_Color COLOR_OF_ACTIVE = { 50, 250, 50, 0 };
const SDL_Color COLOR_OF_INACTIVE = { 10, 110, 10, 0 };

class MenuState : public State {
public:
  MenuState();
  ~MenuState();
  STATE_CHANGE handleKey(const SDL_KeyboardEvent& key);
  STATE_CHANGE update(Uint32 delta_time);
  void draw(SDL_Surface* screen);
private:
  MenuState(const MenuState&);
  MenuState& operator=(const MenuState&);
  SDL_Surface* m_background;
  TextWriter* m_textWriter;

  enum MENU_ACTION {
    NEW_GAME = 0,
    SHOW_HIGHSCORE,
    SHOW_HELP,
    QUIT_GAME
  };

  class MenuItem {
  public:
    MenuItem(const std::string& text, const SDL_Color& color,
             bool current, enum MENU_ACTION action)
      : txt(text), col(color), cur(current), act(action) { }
    const std::string txt;
    SDL_Color col;
    bool cur;
    MENU_ACTION act;
  };

  std::list<MenuItem> m_items;
};

#endif
