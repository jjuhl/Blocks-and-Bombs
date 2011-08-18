#include <string>
#include <map>
#include <fstream>
#include <iostream>
#include <sstream>
#include <cstdlib>
#include <SDL.h>
#include <SDL_image.h>
#include "except.hh"
#include "resources.hh"
#include "config.h"

AnimationResource::AnimationResource(std::map<std::string, std::string>& properties)
  : Resource(properties["name"]), current_frame(0), current_frame_off(0),
    moving_forward(true)
{
  frame_w = strtoul(properties["width"].c_str(), 0, 10);
  frame_h = strtoul(properties["height"].c_str(), 0, 10);
  initial_ms_per_frame = strtoul(properties["ms_per_frame"].c_str(), 0, 10);
  ms_per_frame = initial_ms_per_frame;
  loop_type = NONE;
  if (!strcmp(properties["loop_type"].c_str(), "loop"))
    loop_type = LOOP;
  else if (!strcmp(properties["loop_type"].c_str(), "pingpong"))
    loop_type = PINGPONG;

  anim = IMG_LoadDisplayFormat(properties["frames"].c_str());

  last_frame = anim->w / frame_w - 1;
}

SDL_Rect AnimationResource::currentFrameRect(Uint32 delta_time)
{
  SDL_Rect retval;

  retval.w = frame_w;
  retval.h = frame_h;
  retval.y = 0;

  switch (loop_type) {
  case NONE: {
    if (current_frame == last_frame) {
      break;
    }
    // not at last frame yet, let's see where we are at
    current_frame += (current_frame_off + delta_time) / ms_per_frame;
    if (current_frame > last_frame)
      current_frame = last_frame;
    current_frame_off = (current_frame_off + delta_time) % ms_per_frame;
    break;
  }
  case LOOP: {
    current_frame += (current_frame_off + delta_time) / ms_per_frame;
    if (current_frame > last_frame)
      current_frame %= last_frame;
    current_frame_off = (current_frame_off + delta_time) % ms_per_frame;
    break;
  }
  case PINGPONG: {
    if (moving_forward) {
      current_frame += (current_frame_off + delta_time) / ms_per_frame;
      if (current_frame > last_frame) {
        current_frame = last_frame - (current_frame % last_frame);
        moving_forward = false;
      }
      current_frame_off = (current_frame_off + delta_time) % ms_per_frame;
      break;
    }
    // moving backwards
    Uint32 frame_skip = (current_frame_off + delta_time) / ms_per_frame;
    if (current_frame >= frame_skip) {
      current_frame -= frame_skip;
    } else {
      current_frame = (frame_skip - current_frame);
      moving_forward = true;
    }

    current_frame_off = (current_frame_off + delta_time) % ms_per_frame;
    break;
  }
  }

  retval.x = current_frame * frame_w;
  return retval;
}

LevelResource::LevelResource(const std::string& name,
                             std::map<std::string, std::string>& properties,
                             const std::vector<unsigned char>& level_map)
  : Resource(name), m_block_to_wall_delay(10000), m_delay_between_blocks(5500),
    m_successful_pickup_delay_reduction(25), m_failed_pickup_delay_reduction(120)
{
}

std::vector<unsigned char> LevelResource::initialBoard() const
{
  std::vector<unsigned char> v;
  return v;
}

Uint32 LevelResource::randomSeed() const
{
  return 42;
}

SDL_Rect LevelResource::playerStartPos() const
{
  SDL_Rect r;
  r.x = r.y = 14;
  r.w = r.h = 0;
  return r;
}

Uint32 LevelResource::playerMoveDelay() const
{
  return 120;
}


Uint32 LevelResource::remainingTotal() const
{
  return remainingRed() + remainingGreen() + remainingBlue()
    + remainingPurple() + remainingYellow() + remainingCyan()
    + remainingArbitrary();
}

void LevelResource::blockPickup(BLOCK_COLOR col)
{
  Uint32 old_delay = m_block_to_wall_delay;
  m_block_to_wall_delay -= m_successful_pickup_delay_reduction;
  if (m_block_to_wall_delay > old_delay)
    m_block_to_wall_delay = 0;

  old_delay = m_delay_between_blocks;
  m_delay_between_blocks -= m_successful_pickup_delay_reduction * 1.1;
  if (m_delay_between_blocks > old_delay)
    m_delay_between_blocks = 0;
}

void LevelResource::failedBlockPickup()
{
  Uint32 old_delay = m_block_to_wall_delay;
  m_block_to_wall_delay -= m_failed_pickup_delay_reduction;
  if (m_block_to_wall_delay > old_delay)
    m_block_to_wall_delay = 0;

  old_delay = m_delay_between_blocks;
  m_delay_between_blocks -= m_failed_pickup_delay_reduction * 1.2;
  if (m_delay_between_blocks > old_delay)
    m_delay_between_blocks = 0;
}

Resource* ResourceLoader::load(const std::string& resource_name)
{
  // XXX: Hack

  if (resource_name == "levels/level-0001.res") {
    std::map<std::string, std::string> a;
    std::vector<unsigned char> b;
    return new LevelResource("level-0001", a, b);
  }

  std::map<std::string, std::string> properties;

  std::string resource_filename = std::string(RESOURCES_DIR) + resource_name;

  std::string line;
  std::ifstream file(resource_filename.c_str());
  while (std::getline(file, line)) {
    std::string token;
    std::istringstream tokens(line);
    while (tokens >> token) {
        std::size_t pos = token.find('=');
        if (pos != std::string::npos) {
            properties[token.substr(0, pos)] = token.substr(pos + 1);
        }
    }
  }
  properties["name"] = resource_name;

  return new AnimationResource(properties);
}

void ResourceLoader::unload(Resource* res)
{
  // at some point we want to reference count our resources so this
  // delete will be dependant on the refcount being zero
  delete res;
}

SDL_Surface* IMG_LoadDisplayFormat(const std::string& file)
{
  std::string filename = std::string(RESOURCES_DIR) + "images/" + file;
  SDL_Surface* tmp = IMG_Load(filename.c_str());
  if (!tmp)
    throw Exception("Failed to load image '" + filename + "': " + std::string(IMG_GetError()));

  SDL_Surface* ret = SDL_DisplayFormatAlpha(tmp);
  SDL_FreeSurface(tmp);
  if (!ret)
    throw Exception("Failed to convert image '" + filename + "': " + std::string(IMG_GetError()));

  return ret;
}
