

#include <SDL2/SDL.h>
#include <stdio.h>

#define MAX_THINGS 100

typedef struct {
  int id;            // unique ID for each thing
  float x, y;        // position
  float vx, vy;      // velocity
  int width, height; // size
} Thing;

typedef struct {
  Thing things[MAX_THINGS]; // list of things
  int thing_count;          // nb of active things

  // user input states
  int key_up, key_down, key_left, key_right;
  int mouse_x, mouse_y;
  int mouse_button_pressed;
} GameState;

void draw_rectangle(SDL_Renderer *renderer, float x, float y, int width,
                    int height) {
  SDL_Rect rect;
  rect.x = (int)x;
  rect.y = (int)y;
  rect.w = width;
  rect.h = height;

  SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
  SDL_RenderFillRect(renderer, &rect);
}

void render_objects(GameState *game, SDL_Renderer *renderer) {
  SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
  SDL_RenderClear(renderer);

  for (int i = 0; i < game->thing_count; i++) {
    Thing *obj = &game->things[i];
    draw_rectangle(renderer, obj->x, obj->y, obj->width, obj->height);
  }

  SDL_RenderPresent(renderer);
}

void add_thing(GameState *game, int x, int y, int width, int height, float vx,
               float vy) {
  if (game->thing_count >= MAX_THINGS)
    return;

  Thing *obj = &game->things[game->thing_count++];
  obj->id = game->thing_count;
  obj->x = x;
  obj->y = y;
  obj->vx = vx;
  obj->vy = vy;
  obj->width = width;
  obj->height = height;
}

void update_objects(GameState *game, float delta_time) {
  for (int i = 0; i < game->thing_count; i++) {
    Thing *obj = &game->things[i];
    obj->x += obj->vx * delta_time;
    obj->y += obj->vy * delta_time;
  }
}

void handle_input(GameState *game) {
  SDL_Event event;
  while (SDL_PollEvent(&event)) {
    if (event.type == SDL_QUIT) {
      exit(0);
    }

    const Uint8 *state = SDL_GetKeyboardState(NULL);
    game->key_up = state[SDL_SCANCODE_UP];
    game->key_down = state[SDL_SCANCODE_DOWN];
    game->key_left = state[SDL_SCANCODE_LEFT];
    game->key_right = state[SDL_SCANCODE_RIGHT];

    if (event.type == SDL_MOUSEMOTION) {
      game->mouse_x = event.motion.x;
      game->mouse_y = event.motion.y;
    }
    if (event.type == SDL_MOUSEBUTTONDOWN) {
      game->mouse_button_pressed = 1;
    }
    if (event.type == SDL_MOUSEBUTTONUP) {
      game->mouse_button_pressed = 0;
    }
  }
}

void game_loop(GameState *game, SDL_Renderer *renderer) {
  while (1) {
    float delta_time = 0.016f;

    handle_input(game);
    update_objects(game, delta_time);
    render_objects(game, renderer);

    SDL_Delay(16);
  }
}
