
#include "T-engine.c"

#define WIDTH 1500
#define HEIGHT 900

#define BOID_COUNT 40

#define RULE1_INF 4000.0

#define RULE2_INF 150.0
#define RULE2_DIST 30

#define RULE3_INF 150.0

#define SPEED_MAX 50
#define SPEED_MIN 20

#define CENTERINFLUENCE 2000

int center_inf(GameState *game, Thing *thing) {
  Tvector center = {WIDTH / 2.0, HEIGHT / 2.0};

  thing->vx += (center.x - thing->x) / CENTERINFLUENCE;
  thing->vy += (center.y - thing->y) / CENTERINFLUENCE;

  return 0;
}

int rule1(GameState *game, Thing *thing) {
  Tvector centrom = {0.0, 0.0};
  int count = 0;
  float total_Weight = 0;

  for (int i = 0; i < MAX_THINGS; i++) {
    Thing *other = game->things[i];
    if (other == NULL || i == thing->id) {
      continue;
    }

    float distance = calc_distance(other->x, other->y, thing->x, thing->y);

    float weight = 1.0f / distance;

    total_Weight += weight;

    centrom.x += other->x * weight;
    centrom.y += other->y * weight;
    count++;
  }

  if (count > 0) {
    centrom.x /= count;
    centrom.y /= count;
  }

  thing->vx += (centrom.x - thing->x) / RULE1_INF;
  thing->vy += (centrom.y - thing->y) / RULE1_INF;

  return 0;
}

int rule2(GameState *game, Thing *thing) {

  Tvector vector = {0.0, 0.0};

  for (int i = 0; i < MAX_THINGS; i++) {
    Thing *other = game->things[i];
    if (other == NULL || i == thing->id) {
      continue;
    }

    float distance = calc_distance(thing->x, thing->y, other->x, other->y);

    if (distance < RULE2_DIST) {
      thing->color[1] = 255;
      vector.x -= other->x - thing->x;
      vector.y -= other->y - thing->y;
    } else {
      thing->color[1] = 0;
    }
  }

  thing->vx += vector.x / RULE2_INF;
  thing->vy += vector.y / RULE2_INF;

  return 0;
}

int rule3(GameState *game, Thing *thing) {
  Tvector centrom = {0.0, 0.0};
  int count = 0;

  for (int i = 0; i < MAX_THINGS; i++) {
    Thing *other = game->things[i];
    if (other == NULL || i == thing->id) {
      continue;
    }
    centrom.x += other->vx;
    centrom.y += other->vy;
    count++;
  }

  if (count > 0) {
    centrom.x /= count;
    centrom.y /= count;
  }

  thing->vx += (centrom.x - thing->vx) / RULE3_INF;
  thing->vy += (centrom.y - thing->vy) / RULE3_INF;

  return 0;
}

void update_boids(void *pgame, Thing *thing, float delta_time) {

  GameState *game = (GameState *)pgame;

  center_inf(game, thing);
  rule1(game, thing);
  rule2(game, thing);
  rule3(game, thing);

  thing->x += thing->vx * delta_time;
  thing->y += thing->vy * delta_time;

  float speed = sqrt(thing->vx * thing->vx + thing->vy * thing->vy);

  if (speed > SPEED_MAX) {
    thing->color[2] = 255;
    thing->vx = (thing->vx / speed) * SPEED_MAX;
    thing->vy = (thing->vy / speed) * SPEED_MAX;
  } else if (speed < SPEED_MIN) {
    thing->color[2] = 255;
    thing->vx = (thing->vx / speed) * SPEED_MIN;
    thing->vy = (thing->vy / speed) * SPEED_MIN;
  } else {
    thing->color[2] = 0;
  }

  if (thing->x < 0) {
    thing->x = 0;
    thing->vx = -thing->vx;
  } else if (thing->x > WIDTH) {
    thing->x = WIDTH;
    thing->vx = -thing->vx;
  }

  if (thing->y < 0) {
    thing->y = 0;
    thing->vy = -thing->vy;
  } else if (thing->y > HEIGHT) {
    thing->y = HEIGHT;
    thing->vy = -thing->vy;
  }
}

float random_float(float min, float max) {
  return min + (rand() / (float)RAND_MAX) * (max - min);
}

void generate_random_coordinates(float width, float height, float *x,
                                 float *y) {
  *x = random_float(0, width);
  *y = random_float(0, height);
}

void spawn_Boids(void *game) {
  for (int i = 0; i < BOID_COUNT; i++) {
    float *x = malloc(sizeof(int));
    float *y = malloc(sizeof(int));
    generate_random_coordinates(WIDTH, HEIGHT, x, y);
    add_thing(game, *x, *y, 5, 5, random_float(0, 50), random_float(0, 50), 6,
              255, 0, 0, 255);
    free(x);
    free(y);
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

  spawn_Boids(game);

  game->on_update = update_boids;

  game_loop(game, renderer);

  SDL_DestroyRenderer(renderer);
  SDL_DestroyWindow(win);
  SDL_Quit();

  return 0;
}
