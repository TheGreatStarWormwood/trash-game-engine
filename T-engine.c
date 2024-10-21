#include <SDL2/SDL.h>
#include <SDL2/SDL_scancode.h>
#include <SDL2/SDL_ttf.h>
#include <stdio.h>
#include <time.h>

#define MAX_THINGS 100
#define DEFAULT_TYPE 0
#define RAYCAST_TYPE 1

typedef struct {
  int id;            // unique ID for each thing
  int type_id;       // id to identify different types of things
  float x, y;        // position
  float vx, vy;      // velocity
  int width, height; // also use for bounding box purpoess
  void *custom_Properties;
} Thing;

typedef struct {
  Thing *thing;
  char text[];
} Text;

typedef struct {
  float dx, dy;
  float length;
} Raycast;

typedef struct {
  Thing **things;          // list of things
  int thing_count;         // nb of active things
  int *available_Indicies; // keep track of empty indicies in **things
  int av_i_count;          // nb of empty indicies

  // user input states
  int key_up, key_down, key_left, key_right;
  int mouse_x, mouse_y;
  int mouse_button_pressed;

  // custom hook for user defined functions
  void (*on_update)(void *game, Thing *thing, float delta_time);
} GameState;

void print_thing_ids(GameState *game) {
  printf("Active Thing IDs:\n");
  for (int i = 0; i < MAX_THINGS; i++) {
    Thing *thing = game->things[i];
    if (thing != NULL) {
      printf("Thing ID: %d\n", thing->id);
    } else {
      printf("NULL Thing at index: %d\n", i);
    }
  }
}

void normalize_Vector(float *vx, float *vy) {

  double magnitude = sqrt((double)(*vx * *vx + *vy * *vy));
  if (magnitude != 0) {
    *vx = (*vx / magnitude);
    *vy = (*vy / magnitude);
  } else {
    *vx = 0;
    *vy = 0;
  }
}

Thing *malloc_Thing() {

  Thing *thing = malloc(sizeof(Thing));

  if (thing == NULL) {
    return NULL;
  }

  thing->id = -1;
  thing->type_id = -1;
  thing->x = 0.0f;
  thing->y = 0.0f;
  thing->vx = 0.0f;
  thing->vy = 0.0f;
  thing->width = 10;
  thing->height = 10;
  thing->custom_Properties = NULL;

  return thing;
}

GameState *malloc_GameState() {
  GameState *game = malloc(sizeof(GameState));

  if (game == NULL) {
    return NULL;
  }

  memset(game, 0, sizeof(GameState));
  game->things = malloc(sizeof(Thing *) * MAX_THINGS);
  game->available_Indicies = malloc(sizeof(int) * MAX_THINGS);
  game->av_i_count = MAX_THINGS;

  if (game->things == NULL) {
    return NULL;
  }

  for (int i = 0; i < MAX_THINGS; i++) {
    game->things[i] = NULL;
    game->available_Indicies[i] = i;
  }

  return game;
}

Thing *add_thing(GameState *game, int x, int y, int width, int height, float vx,
                 float vy, int tid) {
  if (game->thing_count >= MAX_THINGS)
    return NULL;

  if (game->av_i_count == 0) {
    return NULL;
  }

  int index = game->available_Indicies[--game->av_i_count];

  Thing *obj = game->things[index];

  if (obj == NULL) {
    obj = malloc_Thing(); // Allocate a new Thing if the pointer is NULL
    game->things[index] = obj;
  }
  game->thing_count++;
  obj->id = index;
  obj->type_id = tid;
  obj->x = x;
  obj->y = y;
  obj->vx = vx;
  obj->vy = vy;
  obj->width = width;
  obj->height = height;

  // printf("index: %d\n", game->av_i_count);
  // printf("id of added object: %d\n", obj->id);
  return obj;
}

void draw_Text(SDL_Renderer *renderer, Text *text) {
  TTF_Font *Sans = TTF_OpenFont("Sans.ttf", 24);

  SDL_Color White = {255, 255, 255};

  SDL_Surface *surfaceMessage = TTF_RenderText_Solid(Sans, text->text, White);

  SDL_Texture *Message = SDL_CreateTextureFromSurface(renderer, surfaceMessage);

  SDL_Rect Message_rect;                // create a rect
  Message_rect.x = text->thing->x;      // controls the rect's x coordinate
  Message_rect.y = text->thing->y;      // controls the rect's y coordinte
  Message_rect.w = text->thing->width;  // controls the width of the rect
  Message_rect.h = text->thing->height; // controls the height of the rect
}

void draw_rectangle(SDL_Renderer *renderer, float x, float y, int width,
                    int height) {
  SDL_Rect rect;
  rect.x = (int)x - width / 2;
  rect.y = (int)y - height / 2;
  rect.w = width;
  rect.h = height;

  SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
  SDL_RenderFillRect(renderer, &rect);
}

void render_objects(GameState *game, SDL_Renderer *renderer) {
  SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
  SDL_RenderClear(renderer);
  for (int i = 0; i < MAX_THINGS; i++) {
    Thing *obj = game->things[i];
    if (obj == NULL || obj->id == -1) {
      continue;
    }
    // printf("drawing object of id: %d\n", obj->id);
    draw_rectangle(renderer, obj->x, obj->y, obj->width, obj->height);
  }

  SDL_RenderPresent(renderer);
}

int check_boundingbox_Collision(Thing *A, Thing *B) {}

int ray_intersects_bounding_box(Thing *source, Raycast *ray, Thing *target) {
  float x_min = target->x;
  float x_max = target->x + target->width;
  float y_min = target->y;
  float y_max = target->y + target->height;

  float ray_origin_x = source->x;
  float ray_origin_y = source->y;

  float t1 = (x_min - ray_origin_x) / ray->dx;
  float t2 = (x_max - ray_origin_x) / ray->dx;
  float t3 = (y_min - ray_origin_y) / ray->dy;
  float t4 = (y_max - ray_origin_y) / ray->dy;

  float t_min = fmax(fmin(t1, t2), fmin(t3, t4));
  float t_max = fmin(fmax(t1, t2), fmax(t3, t4));

  if (t_max < 0 || t_min > t_max) {
    return 0; // no intersection
  }

  return 1; // intersection
}

void add_raycast_to_thing(Thing *thing, float dx, float dy, float length) {
  Raycast *ray = (Raycast *)malloc(sizeof(Raycast));
  ray->dx = dx;
  ray->dy = dy;
  ray->length = length;
  thing->custom_Properties = ray; // Attach the raycast to the custom properties
}

void update_objects(GameState *game, float delta_time) {
  for (int i = 0; i < MAX_THINGS; i++) {
    Thing *thing = game->things[i];
    if (thing == NULL) {
      continue;
    }
    thing->x += thing->vx * delta_time;
    thing->y += thing->vy * delta_time;

    if (game->on_update) {
      game->on_update(game, thing, delta_time);
    }
  }
}

void handle_input(GameState *game) {
  SDL_Event event;
  while (SDL_PollEvent(&event)) {
    if (event.type == SDL_QUIT) {
      exit(0);
    }

    // printf("\n~~~~~~~~~~~~~~~~\nInputs:");
    const Uint8 *state = SDL_GetKeyboardState(NULL);
    game->key_up = state[SDL_SCANCODE_UP] || state[SDL_SCANCODE_W];
    // printf("key_up:%d\n", game->key_up);
    game->key_down = state[SDL_SCANCODE_DOWN] || state[SDL_SCANCODE_S];
    // printf("key_down:%d\n", game->key_down);
    game->key_left = state[SDL_SCANCODE_LEFT] || state[SDL_SCANCODE_A];
    // printf("key_left:%d\n", game->key_left);
    game->key_right = state[SDL_SCANCODE_RIGHT] || state[SDL_SCANCODE_D];
    // printf("key_right:%d\n", game->key_right);

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

void destroy_thing(GameState *game, Thing *thing) {
  int index = thing->id;
  printf("destroying thing of index: %d\n", index);
  // if (game->things[index] != NULL) {
  //   free(game->things[index]);
  // }

  game->things[index] = NULL;
  game->available_Indicies[game->av_i_count++] = index;
  game->thing_count--;
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
