#include <iostream>
#include <sstream>
#include <iomanip>
#include <algorithm>
#include <cstdlib>
#include <typeinfo>
#include <time.h>
#include <SDL.h>
#include <SDL_image.h>
#include "except.hh"
#include "textwriter.hh"
#include "resources.hh"
#include "playstate.hh"
#include "effects.hh"
#include "config.h"

Board::Board(ResourceLoader& loader, Uint32)
  : m_loader(loader), m_width(16), m_height(16),
    m_grid(IMG_LoadDisplayFormat("grid-square.png")),
    m_player(new Player(this, 14, 14)),
    m_level(dynamic_cast<LevelResource*>(loader.load("levels/level-0001.res"))),
    m_board(m_width * m_height), m_newObjects(), m_deadObjects(), m_freeTiles(),
    m_block_time(0)
{
  for (std::vector<GameObject*>::iterator it = m_board.begin();
       it != m_board.end(); ++it) {
    // XXX: We want to load a proper level here
    *it = 0;
  }
  srand(time(0));
}

Board::~Board()
{
  SDL_FreeSurface(m_grid);
  delete m_player;
}

void Board::addGameObject(GameObject* new_obj)
{
  // no more free tiles? too bad, too sad...
  if (m_freeTiles.empty())
    return;

  m_newObjects.insert(new_obj);
  m_freeTiles.erase(std::remove(m_freeTiles.begin(), m_freeTiles.end(),
                                std::make_pair(new_obj->x(), new_obj->y())),
                    m_freeTiles.end());
}

void Board::removeGameObject(GameObject* dead_obj)
{
  m_deadObjects.insert(dead_obj);
  m_freeTiles.push_back(std::make_pair(dead_obj->x(), dead_obj->y()));
}

void Board::update(Uint32 delta_time)
{
  // Process all active objects
  for (std::vector<GameObject*>::iterator it = m_board.begin();
       it != m_board.end(); ++it) {
    if (!*it)
      continue;
    (*it)->update(delta_time);
  }

  // remove all newly dead objects from the board
  for (std::vector<GameObject*>::iterator it = m_board.begin();
       it != m_board.end(); ++it) {
    if (!*it)
      continue;
    std::set<GameObject*>::iterator dead = m_deadObjects.find(*it);
    if (dead != m_deadObjects.end()) {
      *it = 0;
      m_deadObjects.erase(dead);
      delete *dead;
    }
  }

  // add any new objects to the board
  for (std::set<GameObject*>::iterator it = m_newObjects.begin();
       it != m_newObjects.end(); ++it) {
    if (m_board[(*it)->y() * m_width + (*it)->x()] != 0)
      throw Exception("Trying to create new game object at already occupied location.");
    m_board[(*it)->y() * m_width + (*it)->x()] = *it;
  }
  m_newObjects.clear();

  // Update the player
  m_player->update(delta_time);

  // Update the free list
  // This is inefficient.
  m_freeTiles.clear();
  bool has_block = false;
  for (Uint16 y = 0; y < m_height; ++y) {
    for (Uint16 x = 0; x < m_width; ++x) {
      const GameObject* go = m_board[y * m_width + x];
      if (go == 0) {
        if (m_player->x() == x && m_player->y() == y)
          continue;
        m_freeTiles.push_back(std::make_pair(x, y));
      } else if (dynamic_cast<const Block*>(go)) {
        has_block = true;
      }
    }
  }

  // if time between blocks for this level has passed,
  // add a new block
  m_block_time += delta_time;
  if (m_block_time > m_level->delayBetweenBlocks()) {
    newBlock();
    m_block_time = 0;
  }

  // if there are no blocks on the board, add one
  if (!has_block) {
    newBlock();
  }

  // If player collides with object, pass on effects
  GameObject* collider = m_board[m_player->y() * m_width + m_player->x()];
  if (collider)
    collider->collision(m_player);

  if (boxedIn()) {
    Effect e;
    e.life = -1;
    m_player->setEffects(e);
  }
}

void Board::centerDraw(const GameObject* obj, const SDL_Rect& srect, SDL_Rect& drect)
{
  if (srect.w == m_grid->w) {
    drect.x = obj->x() * m_grid->w + m_grid->w;
  } else if (srect.w > m_grid->w) {
    drect.x = obj->x() * m_grid->w + m_grid->w;
    drect.x -= (m_grid->w - obj->x()) / 2;
  } else {
    drect.x = obj->x() * m_grid->w + m_grid->w;
    drect.x += (m_grid->w - obj->x()) / 2;
  }

  if (srect.h == m_grid->h) {
    drect.y = obj->y() * m_grid->h + m_grid->h;
  } else if (srect.h > m_grid->h) {
    drect.y = obj->y() * m_grid->w + m_grid->h;
    drect.y -= (m_grid->h - obj->y()) / 2;
  } else {
    drect.y = obj->y() * m_grid->w + m_grid->h;
    drect.y += (m_grid->h - obj->y()) / 2;
  }
}

void Board::draw(SDL_Surface* screen)
{
  // draw the game grid
  const int w_off = m_grid->w;
  const int h_off = m_grid->h;
  for (Uint16 h = 0; h < m_height; ++h) {
    for (Uint16 w = 0; w < m_width; ++w) {
      SDL_Rect r;
      r.x = h * m_grid->h + h_off;
      r.y = w * m_grid->w + w_off;
      SDL_BlitSurface(m_grid, 0, screen, &r);
    }
  }

  // draw all game objects
  SDL_Rect srect;
  SDL_Rect drect;
  SDL_Surface* surf;
  for (std::vector<GameObject*>::iterator it = m_board.begin();
       it != m_board.end(); ++it) {
    if (!*it)
      continue;

    (*it)->draw(&surf, &srect);
    centerDraw(*it, srect, drect);
    SDL_BlitSurface(surf, &srect, screen, &drect);
  }

  // Draw the player on top
  m_player->draw(&surf, &srect);
  centerDraw(m_player, srect, drect);
  SDL_BlitSurface(surf, &srect, screen, &drect);
}

std::vector<std::pair<Uint16, Uint16> > Board::freeTiles() const
{
  return m_freeTiles;
}

void Board::newBlock()
{
    std::vector<std::pair<Uint16, Uint16> > free_blocks = freeTiles();
    if (free_blocks.empty())
      return;

    std::pair<Uint16, Uint16> new_block = free_blocks[rand() % free_blocks.size()];
    switch (rand() % 6) {
    case 0:
      new Block(this, new_block.first, new_block.second,
                *dynamic_cast<AnimationResource*>(loader().load("animations/red-animation.res")),
                RED, m_level->blockToWallDelay());
      break;
    case 1:
      new Block(this, new_block.first, new_block.second,
                *dynamic_cast<AnimationResource*>(loader().load("animations/green-animation.res")),
                GREEN, m_level->blockToWallDelay());
      break;
    case 2:
      new Block(this, new_block.first, new_block.second,
                *dynamic_cast<AnimationResource*>(loader().load("animations/blue-animation.res")),
                BLUE, m_level->blockToWallDelay());
      break;
    case 3:
      new Block(this, new_block.first, new_block.second,
                *dynamic_cast<AnimationResource*>(loader().load("animations/yellow-animation.res")),
                YELLOW, m_level->blockToWallDelay());
      break;
    case 4:
      new Block(this, new_block.first, new_block.second,
                *dynamic_cast<AnimationResource*>(loader().load("animations/purple-animation.res")),
                PURPLE, m_level->blockToWallDelay());
      break;
    case 5:
      new Block(this, new_block.first, new_block.second,
                *dynamic_cast<AnimationResource*>(loader().load("animations/cyan-animation.res")),
                CYAN, m_level->blockToWallDelay());
      break;
    }
}

bool Board::isBlocked(Uint16 test_x, Uint16 test_y)
{
  if (m_board[test_y * m_width + test_x] != 0)
    return m_board[test_y * m_width + test_x]->isBlocked();

  return false;
}

bool Board::boxedIn() const
{
  // If the player is surrounded by walls on all sides, the player loses a life
  int wallCount = 0;
  // check left
  if (m_player->x() == 0) {
    ++wallCount;
  } else {
    Wall* w = dynamic_cast<Wall*>(m_board[m_player->y() * m_width + m_player->x() - 1]);
    if (w)
      ++wallCount;
  }
  // check right
  if (m_player->x() == m_width - 1) {
    ++wallCount;
  } else {
    Wall* w = dynamic_cast<Wall*>(m_board[m_player->y() * m_width + m_player->x() + 1]);
    if (w)
      ++wallCount;
  }
  // check up
  if (m_player->y() == 0) {
    ++wallCount;
  } else {
    Wall* w = dynamic_cast<Wall*>(m_board[(m_player->y() - 1) * m_width + m_player->x()]);
    if (w)
      ++wallCount;
  }
  // check down
  if (m_player->y() == m_height - 1) {
    ++wallCount;
  } else {
    Wall* w = dynamic_cast<Wall*>(m_board[(m_player->y() + 1) * m_width + m_player->x()]);
    if (w)
      ++wallCount;
  }

  if (wallCount == 4)
    return true;

  return false;
}

Block::Block(Board* board, Uint16 x, Uint16 y, AnimationResource& anim, BLOCK_COLOR col,
             Sint32 timeout)
  : GameObject(board, x, y), m_col(col), m_anim(anim),
    m_current_frame(m_anim.currentFrameSurface(0)),
    m_current_frame_rect(m_anim.currentFrameRect(0)), m_start_timeout(timeout),
    m_timeout(m_start_timeout)
{
  m_board->addGameObject(this);
}

Block::~Block()
{
}

void Block::update(Uint32 delta_time)
{
  m_current_frame = m_anim.currentFrameSurface(delta_time);
  m_current_frame_rect = m_anim.currentFrameRect(delta_time);
  m_timeout -= delta_time;
  if (m_timeout <= 0) {
    m_board->removeGameObject(this);
    new Wall(m_board, m_x, m_y);
    m_board->level()->failedBlockPickup();
    return;
  }

  // If time is running out, speed up animation
  const Sint32 low_time = m_start_timeout / 3;
  if (m_timeout < low_time) {
    Uint32 time_cut = m_anim.initialMsPerFrame() / 1.5;
    time_cut *= 1.0 - (static_cast<double>(m_timeout) / low_time);
    m_anim.setMsPerFrame(m_anim.initialMsPerFrame() - time_cut);
  }
}

void Block::draw(SDL_Surface** surface, SDL_Rect* rect)
{
  *surface = m_current_frame;
  *rect = m_current_frame_rect;
}

void Block::collision(GameObject* other)
{
  Player* player = dynamic_cast<Player*>(other);
  if (!player)
    return;

  if (m_col != player->color())
    return;

  Effect e;
  e.score = 1000;
  if (m_timeout > 0)
    e.score += 1000.0 * (100.0 / m_start_timeout * m_timeout / 100);
  player->setEffects(e);
  m_board->level()->blockPickup(m_col);
  m_board->removeGameObject(this);
}

Wall::Wall(Board* board, Uint16 x, Uint16 y)
  : GameObject(board, x, y), m_current_frame(IMG_LoadDisplayFormat("wall.png")),
    m_current_frame_rect()
{
  m_current_frame_rect.x = 0;
  m_current_frame_rect.y = 0;
  m_current_frame_rect.w = m_current_frame->w;
  m_current_frame_rect.h = m_current_frame->h;
  m_board->addGameObject(this);
}

Wall::~Wall()
{
}

void Wall::update(Uint32)
{
}

void Wall::draw(SDL_Surface** surface, SDL_Rect* rect)
{
  *surface = m_current_frame;
  SDL_Rect r = m_current_frame_rect;
  *rect = r;
}

void Wall::collision(GameObject* other)
{
  Player* player = dynamic_cast<Player*>(other);
  if (!player)
    return;

  // Ok, the only way a player can collide with a wall is if the wall
  // materialized from a block while the player was occupying the
  // tile. If this happens the player loses a life.

  Effect e;
  e.life = -1;
  player->setEffects(e);
}

Player::Player(Board* board, Uint16 x, Uint16 y)
  : GameObject(board, x, y),
    m_direction(NONE), m_move_delay(120), m_time_since_move(0),
    m_top(RED), m_bottom(PURPLE), m_up(BLUE), m_down(CYAN), m_left(GREEN),
    m_right(YELLOW),
    m_top_img(IMG_LoadDisplayFormat("cube-top.png")),
    m_up_img(IMG_LoadDisplayFormat("cube-up.png")),
    m_down_img(IMG_LoadDisplayFormat("cube-down.png")),
    m_left_img(IMG_LoadDisplayFormat("cube-left.png")),
    m_right_img(IMG_LoadDisplayFormat("cube-right.png")),
    m_frame(SDL_CreateRGBSurface(SDL_SWSURFACE|SDL_SRCALPHA, 32, 32, 32, 0, 0, 0, 0)),
    m_score(0), m_life(3)
{
  if (!m_frame)
    throw Exception("Unable to create frame surface for player: "
                    + std::string(SDL_GetError()));
}

Player::~Player()
{
  SDL_FreeSurface(m_frame);
  SDL_FreeSurface(m_right_img);
  SDL_FreeSurface(m_left_img);
  SDL_FreeSurface(m_down_img);
  SDL_FreeSurface(m_up_img);
  SDL_FreeSurface(m_top_img);
}

void Player::update(Uint32 delta_time)
{
  m_time_since_move += delta_time;

  if (m_time_since_move < m_move_delay)
    return;

  enum BLOCK_COLOR tmp;
  switch (m_direction) {
  case NONE:
    return;

  case UP:
    // roll the cube up (if that field is not blocked and we are not at edge of board)
    if (m_y == 0)
      break;
    if (m_board->isBlocked(m_x, m_y - 1))
      break;
    setPos(m_x, m_y - 1);
    // update the colors
    tmp = m_bottom;
    m_bottom = m_up;
    m_up = m_top;
    m_top = m_down;
    m_down = tmp;
    break;

  case DOWN:
    // roll the cube down
    if (m_y == m_board->height() - 1)
      break;
    if (m_board->isBlocked(m_x, m_y + 1))
      break;
    setPos(m_x, m_y + 1);
    tmp = m_bottom;
    m_bottom = m_down;
    m_down = m_top;
    m_top = m_up;
    m_up = tmp;
    break;

  case LEFT:
    // roll the cube left
    if (m_x == 0)
      break;
    if (m_board->isBlocked(m_x - 1, m_y))
      break;
    setPos(m_x - 1, m_y);
    tmp = m_bottom;
    m_bottom = m_left;
    m_left = m_top;
    m_top = m_right;
    m_right = tmp;
    break;

  case RIGHT:
    // roll the cube right
    if (m_x == m_board->width() - 1)
      break;
    if (m_board->isBlocked(m_x + 1, m_y))
      break;
    setPos(m_x + 1, m_y);
    tmp = m_bottom;
    m_bottom = m_right;
    m_right = m_top;
    m_top = m_left;
    m_left = tmp;
    break;
  }

  // We don't count hitting a wall or the edge of the board as a move,
  // so only reset the time since last move here.
  m_time_since_move = 0;
}

void Player::draw(SDL_Surface** surface, SDL_Rect* rect)
{
  // clear our frame surface
  Uint32 col = SDL_MapRGBA(m_frame->format, 0, 0, 0, 0);
  if (SDL_FillRect(m_frame, 0, col))
    throw Exception("Clearing player frame failed: "
                    + std::string(SDL_GetError()));

  SDL_Rect src;
  SDL_Rect dst;

  // src y is always 0 and x can be determined by the color
  src.y = 0;
  src.x = 32 * static_cast<int>(m_up);
  src.w = 32;
  src.h = m_up_img->h;
  dst.x = 0;
  dst.y = 0;
  dst.w = 32;
  dst.h = m_up_img->h;
  SDL_BlitSurface(m_up_img, &src, m_frame, &dst);

  src.x = 5 * static_cast<int>(m_left);
  src.w = 5;
  src.h = m_left_img->h;
  dst.x = 0;
  dst.y = 0;
  dst.w = 5;
  dst.h = m_left_img->h;
  SDL_BlitSurface(m_left_img, &src, m_frame, &dst);

  src.x = 22 * static_cast<int>(m_top);
  src.w = 22;
  src.h = m_top_img->h;
  dst.x = 5;
  dst.y = 5;
  dst.w = 22;
  dst.h = m_top_img->h;
  SDL_BlitSurface(m_top_img, &src, m_frame, &dst);

  src.x = 5 * static_cast<int>(m_right);
  src.w = 5;
  src.h = m_right_img->h;
  dst.x = 27;
  dst.y = 0;
  dst.w = 5;
  dst.h = m_right_img->h;
  SDL_BlitSurface(m_right_img, &src, m_frame, &dst);

  src.x = 32 * static_cast<int>(m_down);
  src.w = 32;
  src.h = m_down_img->h;
  dst.x = 0;
  dst.y = 27;
  dst.w = 32;
  dst.h = m_down_img->h;
  SDL_BlitSurface(m_down_img, &src, m_frame, &dst);

  SDL_Rect r;
  r.x = 0;
  r.y = 0;
  r.w = m_frame->w;
  r.h = m_frame->h;
  *rect = r;
  *surface = m_frame;
}

void Player::setEffects(const Effect& e)
{
  m_score += e.score;
  m_life += e.life;
}

PlayState::PlayState()
  : m_background(IMG_LoadDisplayFormat("game-background.png")),
    m_status_background(0), m_pause_background(0),
    m_textWriter(new TextWriter("whitrabt.ttf", 20)),
    m_resourceLoader(), m_board(m_resourceLoader, 1), m_paused(false)
{
  Uint32 rmask, gmask, bmask, amask;
  // SDL interprets each pixel as a 32-bit number, so our masks must
  // depend on the endianness (byte order) of the machine
#if SDL_BYTEORDER == SDL_BIG_ENDIAN
    rmask = 0xff000000;
    gmask = 0x00ff0000;
    bmask = 0x0000ff00;
    amask = 0x000000ff;
#else
    rmask = 0x000000ff;
    gmask = 0x0000ff00;
    bmask = 0x00ff0000;
    amask = 0xff000000;
#endif

  SDL_Rect dstrect;
  // our width is the fixed screen width minus the max width for the
  // board, minus 2 times a grid square for a border around the board
  // minus 1 grid square for a right hand border for us.
  dstrect.w = 800 - 32*16 - 32*2 - 32;
  // height is max height of the game board
  dstrect.h = 16*32;
  // start one grid square down
  dstrect.y = 32;
  // and one grid square right of the board
  dstrect.x = 32*16 + 32*2;

  SDL_Surface* tmp = SDL_CreateRGBSurface(SDL_SWSURFACE|SDL_SRCALPHA,
                                          dstrect.w, dstrect.h, 32,
                                          rmask, gmask, bmask, amask);
  SDL_FillRect(tmp, 0, SDL_MapRGBA(tmp->format, 0, 0, 0, 127));
  m_status_background = SDL_DisplayFormatAlpha(tmp);
  SDL_FreeSurface(tmp);

  tmp = SDL_CreateRGBSurface(SDL_SWSURFACE|SDL_SRCALPHA,
                             800, 600, 32, rmask, gmask, bmask, amask);
  SDL_FillRect(tmp, 0, SDL_MapRGBA(tmp->format, 0, 0, 0, 127));
  m_pause_background = SDL_DisplayFormatAlpha(tmp);
  SDL_FreeSurface(tmp);

  SDL_Color col = { 50, 250, 50, 0 };
  m_textWriter->setFontColor(col);
}

PlayState::~PlayState()
{
  delete m_textWriter;
  SDL_FreeSurface(m_pause_background);
  SDL_FreeSurface(m_status_background);
  SDL_FreeSurface(m_background);
}

STATE_CHANGE PlayState::handleKey(const SDL_KeyboardEvent& key)
{
  switch (key.keysym.sym) {
  case SDLK_UP:
    if (key.type == SDL_KEYDOWN) {
      m_board.player()->goUp();
      return NO_CHANGE;
    }
    if (m_board.player()->direction() == UP)
      m_board.player()->stop();
    break;
  case SDLK_DOWN:
    if (key.type == SDL_KEYDOWN) {
      m_board.player()->goDown();
      return NO_CHANGE;
    }
    if (m_board.player()->direction() == DOWN)
      m_board.player()->stop();
    break;
  case SDLK_LEFT:
    if (key.type == SDL_KEYDOWN) {
      m_board.player()->goLeft();
      return NO_CHANGE;
    }
    if (m_board.player()->direction() == LEFT)
      m_board.player()->stop();
    break;
  case SDLK_RIGHT:
    if (key.type == SDL_KEYDOWN) {
      m_board.player()->goRight();
      return NO_CHANGE;
    }
    if (m_board.player()->direction() == RIGHT)
      m_board.player()->stop();
    break;
  case SDLK_ESCAPE:
    return GOTO_MENU;
  case SDLK_PAUSE:
  case SDLK_p:
    if (key.type == SDL_KEYDOWN) {
      m_paused = !m_paused;
    }
    break;
  default:
    break;
  }

  return NO_CHANGE;
}

STATE_CHANGE PlayState::update(Uint32 delta_time)
{
  if (m_paused) {
    updatePause();
    return NO_CHANGE;
  }

  const Uint16 playerLife = m_board.player()->livesLeft();
  m_board.update(delta_time);
  // Check if the player has lost a life
  if (m_board.player()->livesLeft() < playerLife) {
    std::cout << "player died" << std::endl;
  }


  return NO_CHANGE;
}

void PlayState::draw(SDL_Surface* screen)
{
  SDL_BlitSurface(m_background, 0, screen, 0);

  m_board.draw(screen);
  drawStatusArea(screen);
  if (m_paused)
    drawPause(screen);
}

void PlayState::drawScore(SDL_Surface* screen)
{
  SDL_Rect r;
  // FIXME: These hardcoded constants need to go away
  r.x = 18 * 32 + 8;
  r.y = 32 + 8;
  std::stringstream out;
  out << std::setw(8);
  out.fill('0');
  out << m_board.player()->score();
  m_textWriter->render(screen, &r, "Score: " + out.str());
}

void PlayState::drawStatusArea(SDL_Surface* screen)
{
  // Draw our status area background.

  SDL_Rect dstrect;
  // our width is the fixed screen width minus the max width for the
  // board, minus 2 times a grid square for a border around the board
  // minus 1 grid square for a right hand border for us.
  dstrect.w = 800 - 32*16 - 32*2 - 32;
  // height is max height of the game board
  dstrect.h = 16*32;
  // start one grid square down
  dstrect.y = 32;
  // and one grid square right of the board
  dstrect.x = 32*16 + 32*2;

  SDL_BlitSurface(m_status_background, 0, screen, &dstrect);
  drawScore(screen);
}

void PlayState::updatePause()
{
}

void PlayState::drawPause(SDL_Surface* screen)
{
  SDL_BlitSurface(m_pause_background, 0, screen, 0);
  const std::string render_text1("Game Paused");
  const std::string render_text2("Press \"Pause\" or \"P\" to continue.");
  const int skip = m_textWriter->lineSkip();
  m_textWriter->setFontColor(PAUSE_COLOR);
  m_textWriter->setPointSize(30);
  SDL_Rect r1 = m_textWriter->sizeText(render_text1);
  SDL_Rect r2 = m_textWriter->sizeText(render_text2);
  r1.y = (screen->h - r1.h - r2.h - skip) / 2;
  r1.x = (screen->w - r1.w) / 2;
  m_textWriter->render(screen, &r1, render_text1);
  r2.y = r1.y + r1.h + skip;
  r2.x = (screen->w - r2.w) / 2;
  m_textWriter->render(screen, &r2, render_text2);

  m_textWriter->setPointSize(20);
}
