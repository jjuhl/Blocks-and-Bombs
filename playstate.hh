/*
 * Copyright © 2011 by Jesper Juhl
 * Licensed under the terms of the GNU General Public Licence (GPL) version 2.
 */

#ifndef BNB_PLAYSTATE_HH
#define BNB_PLAYSTATE_HH

#include <set>
#include <vector>
#include <SDL.h>
#include "textwriter.hh"
#include "resources.hh"
#include "effects.hh"
#include "states.hh"

const SDL_Color PAUSE_COLOR = { 50, 250, 50, 0 };

class GameObject;
class Player;

class Board {
public:
  Board(ResourceLoader& loader, Uint32 level);
  ~Board();

  // Add new game object at random position
  void addGameObject(GameObject* new_obj);
  void removeGameObject(GameObject* new_obj);

  void update(Uint32 delta_time);
  void centerDraw(const GameObject* obj, const SDL_Rect& srect, SDL_Rect& drect);
  void draw(SDL_Surface* screen);

  Uint16 width() { return m_width; }
  Uint16 height() { return m_height; }

  std::vector<std::pair<Uint16, Uint16> > freeTiles() const;
  void newBlock();

  bool isBlocked(Uint16 test_x, Uint16 test_y);

  Player* player() { return m_player; }
  LevelResource* level() { return m_level; }
  ResourceLoader& loader() { return m_loader; }

private:
  Board(const Board&);
  Board& operator=(const Board&);
  bool boxedIn() const;
  ResourceLoader m_loader;
  Uint16 m_width;
  Uint16 m_height;
  SDL_Surface* m_grid;

  Player* m_player;
  LevelResource* m_level;
  std::vector<GameObject*> m_board;
  std::set<GameObject*> m_newObjects;
  std::set<GameObject*> m_deadObjects;
  std::vector<std::pair<Uint16, Uint16> > m_freeTiles;

  Uint32 m_block_time;
};

class GameObject {
public:
  virtual ~GameObject() { }

  virtual void update(Uint32 delta_time) = 0;
  // This draw method is called by the Board class when drawing. This
  // method should not do any actual drawing itself, just return a
  // pointer to the surface to be used a source and a rectangle
  // describing the area of that surface the GameObject wishes to have
  // drawn. The Board class then takes care of centering this on the
  // tile the object occupies and do the actual drawing.
  virtual void draw(SDL_Surface** surface, SDL_Rect* rect) = 0;

  virtual Uint16 x() const { return m_x; }
  virtual Uint16 y() const { return m_y; }
  virtual void setPos(Uint16 x, Uint16 y) { m_x = x; m_y = y; }

  // Does this object block the grid square for the player?
  virtual bool isBlocked() { return false; }

  // handle collisions with player or other object
  virtual void collision(GameObject*) { }

  // Apply various changes (effects) to this object
  virtual void setEffects(const Effect&) { }

protected:
  GameObject(Board* board, Uint16 x, Uint16 y) : m_board(board), m_x(x), m_y(y)
  { }

  Board* m_board;
  Uint16 m_x;
  Uint16 m_y;
private:
  GameObject(const GameObject&);
  GameObject& operator=(const GameObject&);
};

class Block : public GameObject {
public:
  Block(Board* board, Uint16 x, Uint16 y, AnimationResource& anim, BLOCK_COLOR col,
        Sint32 timeout);
  virtual ~Block();

  BLOCK_COLOR color() { return m_col; }

  virtual void update(Uint32 delta_time);
  virtual void draw(SDL_Surface** surface, SDL_Rect* rect);
  virtual void collision(GameObject* other);
private:
  Block(const Block&);
  Block& operator=(const Block&);
  BLOCK_COLOR m_col;
  AnimationResource& m_anim;
  SDL_Surface* m_current_frame;
  SDL_Rect m_current_frame_rect;
  Sint32 m_start_timeout;
  Sint32 m_timeout;
};

class Wall : public GameObject {
public:
  Wall(Board* board, Uint16 x, Uint16 y);
  virtual ~Wall();

  virtual void update(Uint32 delta_time);
  virtual void draw(SDL_Surface** surface, SDL_Rect* rect);
  virtual void collision(GameObject*);

  virtual bool isBlocked() { return true; }

private:
  Wall(const Wall&);
  Wall& operator=(const Wall&);
  SDL_Surface* m_current_frame;
  SDL_Rect m_current_frame_rect;
};


enum PLAYER_DIRECTION { NONE = 0,
                        UP,
                        DOWN,
                        LEFT,
                        RIGHT };

class Player : public GameObject {
public:
  Player(Board* board, Uint16 x, Uint16 y);
  ~Player();

  void update(Uint32 delta_time);
  void draw(SDL_Surface** surface, SDL_Rect* rect);

  void goUp() { m_direction = UP; }
  void goDown() { m_direction = DOWN; }
  void goLeft() { m_direction = LEFT; }
  void goRight() { m_direction = RIGHT; }
  void stop() { m_direction = NONE; m_time_since_move = m_move_delay / 3; }

  BLOCK_COLOR color() const { return m_top; }
  PLAYER_DIRECTION direction() const { return m_direction; }
  Uint32 score() const { return m_score; }

  void setEffects(const Effect& e);

  Uint16 livesLeft() const { return m_life; }

private:
  Player(const Player&);
  Player& operator=(const Player&);
  PLAYER_DIRECTION m_direction;

  Uint32 m_move_delay;
  Uint32 m_time_since_move;

  // current configuration of the player
  enum BLOCK_COLOR m_top;
  enum BLOCK_COLOR m_bottom;
  enum BLOCK_COLOR m_up;
  enum BLOCK_COLOR m_down;
  enum BLOCK_COLOR m_left;
  enum BLOCK_COLOR m_right;

  // Our strips with the various cube pieces
  SDL_Surface* m_top_img;
  SDL_Surface* m_up_img;
  SDL_Surface* m_down_img;
  SDL_Surface* m_left_img;
  SDL_Surface* m_right_img;

  // composite frame to be drawn at update()
  SDL_Surface* m_frame;

  Uint32 m_score;
  Uint32 m_life;
};

class PlayState : public State {
public:
  PlayState();
  ~PlayState();
  STATE_CHANGE handleKey(const SDL_KeyboardEvent& key);
  STATE_CHANGE update(Uint32 delta_time);
  void draw(SDL_Surface* screen);
  bool isPaused() const { return m_paused; }
private:
  PlayState(const PlayState&);
  PlayState& operator=(const PlayState&);
  void drawScore(SDL_Surface* screen);
  void drawStatusArea(SDL_Surface* screen);
  void updatePause();
  void drawPause(SDL_Surface* screen);
  SDL_Surface* m_background;
  SDL_Surface* m_status_background;
  SDL_Surface* m_pause_background;
  TextWriter* m_textWriter;
  ResourceLoader m_resourceLoader;
  Board m_board;
  bool m_paused;
};

#endif
