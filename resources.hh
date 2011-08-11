#ifndef BNB_RESOURCES_HH
#define BNB_RESOURCES_HH

#include <string>
#include <map>
#include <vector>
#include <SDL.h>

enum BLOCK_COLOR { RED = 0, GREEN = 1, BLUE = 2, YELLOW = 3, PURPLE = 4, CYAN = 5 };

/*
  Base class for all game resources
*/
class Resource {
public:
  virtual ~Resource() { }
  virtual std::string name() const { return m_name; }
protected:
  Resource(const std::string& res_name) : m_name(res_name) { }
  const std::string m_name;
};

class AnimationResource : public Resource {
public:
  AnimationResource(std::map<std::string, std::string>& properties);
  ~AnimationResource()
  { SDL_FreeSurface(anim); }

  SDL_Surface* currentFrameSurface(Uint32 /* delta_time */) { return anim; }
  SDL_Rect currentFrameRect(Uint32 delta_time);
  Uint32 initialMsPerFrame() const { return initial_ms_per_frame; }
  Uint32 msPerFrame() const { return ms_per_frame; }
  AnimationResource& setMsPerFrame(Uint32 ms) { ms_per_frame = ms; return *this; }
private:
  Sint16 frame_w;
  Sint16 frame_h;
  Uint32 initial_ms_per_frame;
  Uint32 ms_per_frame;
  enum ANIM_LOOP_TYPE { NONE = 0, LOOP, PINGPONG } loop_type;
  SDL_Surface* anim;
  Uint32 current_frame;
  Uint32 current_frame_off;
  Uint32 last_frame;
  bool moving_forward;
};

class LevelResource : public Resource
{
public:
  LevelResource(const std::string& name,
                std::map<std::string, std::string>& properties,
                const std::vector<unsigned char>& level_map);
  ~LevelResource() { }
  std::vector<unsigned char> initialBoard() const;
  Uint32 randomSeed() const;
  SDL_Rect playerStartPos() const;
  Uint32 playerMoveDelay() const;
  Uint32 blockToWallDelay() const { return m_block_to_wall_delay; }
  Uint32 delayBetweenBlocks() const { return m_delay_between_blocks; }
  Uint32 remainingRed() const { return m_red_left; }
  Uint32 remainingGreen() const { return m_green_left; }
  Uint32 remainingBlue() const { return m_blue_left; }
  Uint32 remainingPurple() const { return m_purple_left; }
  Uint32 remainingYellow() const { return m_yellow_left; }
  Uint32 remainingCyan() const { return m_cyan_left; }
  Uint32 remainingArbitrary() const { return m_arbitrary_left; }
  Uint32 remainingTotal() const;
  void blockPickup(BLOCK_COLOR col);
  void failedBlockPickup();
private:
  Uint32 m_block_to_wall_delay;
  Uint32 m_delay_between_blocks;
  Uint32 m_successful_pickup_delay_reduction;
  Uint32 m_failed_pickup_delay_reduction;
  Uint32 m_red_left;
  Uint32 m_green_left;
  Uint32 m_blue_left;
  Uint32 m_purple_left;
  Uint32 m_yellow_left;
  Uint32 m_cyan_left;
  Uint32 m_arbitrary_left;
};

class ResourceLoader {
public:
  ResourceLoader() { }
  ~ResourceLoader() { }

  Resource* load(const std::string& resource_name);
  void unload(Resource* res);
};

SDL_Surface* IMG_LoadDisplayFormat(const std::string& file);

#endif
