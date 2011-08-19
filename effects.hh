/*
 * Copyright © 2011 by Jesper Juhl
 * Licensed under the terms of the GNU General Public License (GPL) version 2.
 */

#ifndef BNB_EFFECTS_HH
#define BNB_EFFECTS_HH

#include <SDL.h>

/*
 * effects that objects can give to the player
 */
struct Effect {
  Effect() : score(0), life(0) { }
  Sint16 score;
  Sint16 life;
};

#endif
