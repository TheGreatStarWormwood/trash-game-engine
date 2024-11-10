
#include "T-engine.c"
#include <string.h>

#define WIDTH 640
#define HEIGHT 480

void spawn_Boids(void *game) {}

void update_boids(void *pgame, Thing *thing, float delta_time) {

  GameState *game = (GameState *)pgame;

  int *v1, *v2, *v3;
  memset(v1, 0, sizeof(int) * 2);
  memset(v2, 0, sizeof(int) * 2);
  memset(v3, 0, sizeof(int) * 2);
}

int main() {
  if (SDL_Init(SDL_INIT_VIDEO) != 0) {
    printf("SDL_Init Error: %s\n", SDL_GetError());
    return 1;
  }

  SDL_Window *win = SDL_CreateWindow("2D Game Engine", 100, 100, WIDTH, HEIGHT,
                                     SDL_WINDOW_SHOWN);
  if (win == NULL) {
    printf("SDL_CreateWindow Error: %s\n", SDL_GetError());
    SDL_Quit();
    return 1;
  }

  SDL_Renderer *renderer = SDL_CreateRenderer(
      win, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
  if (renderer == NULL) {
    printf("SDL_CreateRenderer Error: %s\n", SDL_GetError());
    SDL_DestroyWindow(win);
    SDL_Quit();
    return 1;
  }

  GameState *game = malloc_GameState();

  add_thing(game, WIDTH / 2, HEIGHT / 2, 5, 15, 0, -1, 6, 255, 255, 255, 255);

  game->on_update = update_boids;

  game_loop(game, renderer);

  SDL_DestroyRenderer(renderer);
  SDL_DestroyWindow(win);
  SDL_Quit();

  return 0;
}
