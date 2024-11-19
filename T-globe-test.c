
#include "T-engine.c"
#include <linux/limits.h>
#include <threads.h>
#include <unistd.h>

#define WIDTH 1080
#define HEIGHT 680

#define PLAYER_TYPE 5
#define CURSOR_TYPE 6
#define BULLET_TYPE 7
#define ZOMBIE_TYPE 8

int main() {
  GlobeState *globe = init_GlobeState(WIDTH, HEIGHT);

  GameState *game = malloc_GameState();

  add_thing(game, WIDTH / 2, HEIGHT / 2, 20, 20, 0.0f, 0.0f, PLAYER_TYPE, 255,
            0, 0, 255);

  game_loop(globe);

  return 0;
}
