#include "T-engine.c"

#define PLAYER 1
#define CURSOR 2
#define PROJ 3

void shoot(GameState *game, Thing *thing, int mouse_x, int mouse_y) {
  float vx = mouse_x - thing->x;
  float vy = mouse_y - thing->y;

  double magnitude = sqrt((double)(vx * vx + vy * vy));
  if (magnitude != 0) {
    vx = (vx / magnitude) * 1000;
    vy = (vy / magnitude) * 1000;
  } else {
    vx = 0;
    vy = 0;
  }

  add_thing(game, thing->x, thing->y, 2, 2, vx, vy, PROJ);
}

void update_Bullet(void *pgame, Thing *thing, float life) {

  GameState *game = (GameState *)pgame;

  if (thing->x >= 800 - thing->width) {
    thing->x = 800 - thing->width;
  }
  if (thing->x <= 0) {
    thing->x = 0;
  }

  if (thing->y >= 600 - thing->height) {
    thing->y = 600 - thing->height;
  }
  if (thing->y <= 0) {
    thing->y = 0;
  }
}

void player_Update(void *pgame, Thing *thing, float delta_time) {

  GameState *game = (GameState *)pgame;

  if (game->key_up) {
    thing->vy = -500;
  }
  if (game->key_down) {
    thing->vy = 500;
  } else {
    thing->vy /= 1.1;
  }

  if (game->key_right) {
    thing->vx = 500;
  }
  if (game->key_left) {
    thing->vx = -500;
  } else {
    thing->vx /= 1.1;
  }

  if (game->mouse_button_pressed) {
    shoot(game, thing, game->mouse_x, game->mouse_y);
  }

  if (thing->x >= 800 - thing->width) {
    thing->x = 800 - thing->width;
  }
  if (thing->x <= 0) {
    thing->x = 0;
  }

  if (thing->y >= 600 - thing->height) {
    thing->y = 600 - thing->height;
  }
  if (thing->y <= 0) {
    thing->y = 0;
  }
}

void cursor_Update(void *pgame, Thing *thing, float delta_time) {

  GameState *game = (GameState *)pgame;

  thing->x = game->mouse_x;
  thing->y = game->mouse_y;
}

void update(void *pgame, Thing *thing, float delta_time) {
  switch (thing->type_id) {
  case 1:
    player_Update(pgame, thing, delta_time);
    break;
  case 2:
    cursor_Update(pgame, thing, delta_time);
    break;
  case 3:
    update_Bullet((GameState *)pgame, thing, 1.0f);
    break;
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

  add_thing(&game, 100, 100, 20, 20, 0.0f, 0.0f, PLAYER);
  add_thing(&game, 100, 100, 10, 10, 0.0f, 0.0f, CURSOR);

  game.on_update = update;

  game_loop(&game, renderer);

  SDL_DestroyRenderer(renderer);
  SDL_DestroyWindow(win);
  SDL_Quit();

  return 0;
}
