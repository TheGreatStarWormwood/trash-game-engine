#include "T-engine.c"

void custom_on_update(Thing *thing, float delta_time) {
  if (thing->x <= 0 || thing->x >= 750) {
    thing->vx = -thing->vx;
  }

  if (thing->y <= 0 || thing->y >= 550) {
    thing->vy = -thing->vy;
  }
}

int main() {
  if (SDL_Init(SDL_INIT_VIDEO) != 0) {
    printf("SDL_Init Error: %s\n", SDL_GetError());
    return 1;
  }

  SDL_Window *win =
      SDL_CreateWindow("2D Game Engine", 100, 100, 800, 600, SDL_WINDOW_SHOWN);
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

  GameState game = {0};

  add_thing(&game, 0, 0, 20, 20, 0.0f, 0.0f);

  game.on_update = custom_on_update;

  game_loop(&game, renderer);

  SDL_DestroyRenderer(renderer);
  SDL_DestroyWindow(win);
  SDL_Quit();

  return 0;
}
