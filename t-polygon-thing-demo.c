
#include "T-engine.c"
#include <linux/limits.h>
#include <time.h>

#define PLAYER_TYPE 5
#define CURSOR_TYPE 6
#define BULLET_TYPE 7
#define ZOMBIE_TYPE 8

#define WIDTH 800
#define HEIGHT 600

float player_x = 0;
float player_y = 0;

void chase_Player(GameState *game, Thing *thing) {

  Raycast *raycast = (Raycast *)thing->custom_Properties;

  float vx = player_x - thing->x;
  float vy = player_y - thing->y;

  normalize_Vector(&vx, &vy);

  thing->vx = vx * 100;
  thing->vy = vy * 100;

  raycast->dx = vx;
  raycast->dy = vy;

  for (int i = 0; i < MAX_THINGS; i++) {
    Thing *other = game->things[i];
    if (other == NULL) {
      continue;
    }
    if (check_bounding_box_collision(thing, other)) {
      if (other->type_id == BULLET_TYPE) {
        thing->color[0] = 100;
        break;
      }
    }
    thing->color[0] = 255;
  }
}

void shoot(GameState *game, Thing *thing, int mouse_x, int mouse_y) {
  float vx = mouse_x - thing->x;
  float vy = mouse_y - thing->y;

  normalize_Vector(&vx, &vy);

  add_thing(game, thing->x, thing->y, 2, 2, vx * 1000, vy * 1000, BULLET_TYPE,
            250, 250, 250, 255);
}

void update_Bullet(void *pgame, Thing *thing, float life) {

  GameState *game = (GameState *)pgame;

  if (thing->x >= WIDTH - thing->width / 2.0f) {
    destroy_thing(game, thing);
  }
  if (thing->x <= thing->width / 2.0f) {
    destroy_thing(game, thing);
  }

  if (thing->y >= HEIGHT - thing->height / 2.0f) {
    destroy_thing(game, thing);
  }
  if (thing->y <= thing->height / 2.0f) {
    destroy_thing(game, thing);
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

  if (thing->x >= 800 - thing->width / 2.0f) {
    thing->x = 800 - thing->width / 2.0f;
  }
  if (thing->x <= thing->width / 2.0f) {
    thing->x = thing->width / 2.0f;
  }

  if (thing->y >= 600 - thing->height / 2.0f) {
    thing->y = 600 - thing->height / 2.0f;
  }
  if (thing->y <= thing->height / 2.0f) {
    thing->y = thing->height / 2.0f;
  }
  player_x = thing->x;
  player_y = thing->y;

  for (int i = 0; i < MAX_THINGS; i++) {
    Thing *other = game->things[i];
    if (other == NULL) {
      continue;
    }
    if (check_bounding_box_collision(thing, other)) {
      if (other->type_id == ZOMBIE_TYPE) {
        thing->color[0] = 100;
        rotate_points(thing->poly, 1 * M_PI / 180.0);
        break;
      }
    }
    thing->color[0] = 255;
  }
}

void cursor_Update(void *pgame, Thing *thing, float delta_time) {

  GameState *game = (GameState *)pgame;

  thing->x = game->mouse_x;
  thing->y = game->mouse_y;
}

void update(void *pgame, Thing *thing, float delta_time) {
  switch (thing->type_id) {
  case PLAYER_TYPE:
    player_Update(pgame, thing, delta_time);
    break;
  case CURSOR_TYPE:
    cursor_Update(pgame, thing, delta_time);
    break;
  case BULLET_TYPE:
    update_Bullet(pgame, thing, 1.0f);
    break;

  case ZOMBIE_TYPE:
    chase_Player(pgame, thing);
    break;
  }
}

void player_Update_render(GameState *game, Thing *thing) {
  if (thing == NULL || thing->poly == NULL) {
    return;
  }

  double target_angle = get_angle_between_points(thing->x, thing->y,
                                                 game->mouse_x, game->mouse_y);

  double angle_diff = target_angle - thing->poly->angle;

  rotate_points(thing->poly, angle_diff);

  thing->poly->angle = target_angle;
}
void update_renderer(void *pgame, Thing *thing) {
  switch (thing->type_id) {
  case PLAYER_TYPE:
    // player_Update_render(pgame, thing);
    break;
  }
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

  game->on_update_renderer = update_renderer;

  game->on_update = update;

  Thing *player = add_thing(game, WIDTH / 2, HEIGHT / 2, 20, 20, 0.0f, 0.0f,
                            PLAYER_TYPE, 255, 0, 0, 255);

  Vector vertices;
  vector_setup(&vertices, 2, sizeof(point));

  vector_push_back(&vertices, create_point(0, -50));
  vector_push_back(&vertices, create_point(20, 50));
  vector_push_back(&vertices, create_point(-20, 50));
  Tpolygon *poly = create_polygon(&vertices);
  poly->angle = 0.0;
  poly->center.x = 100;
  poly->center.y = 100;

  player->poly = poly;

  rotate_points(poly, 90 * M_PI / 180.0);

  add_thing(game, 100, 100, 10, 10, 0.0f, 0.0f, CURSOR_TYPE, 100, 100, 0, 255);
  Thing *zombie =
      add_thing(game, 500, 500, 10, 10, 0.0f, 0.0f, 8, 255, 255, 0, 255);
  add_raycast_to_thing(zombie, 0, 10, 100);

  // print_thing_ids(game);

  game_loop(game, renderer);

  SDL_DestroyRenderer(renderer);
  SDL_DestroyWindow(win);
  SDL_Quit();

  return 0;
}
