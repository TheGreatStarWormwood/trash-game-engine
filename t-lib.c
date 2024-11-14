
#include <SDL2/SDL.h>
#include <SDL2/SDL_scancode.h>
#include <SDL2/SDL_ttf.h>
#include <stdio.h>
#include <stdlib.h>



typedef struct {
  float x;
  float y;
} Tvector;

typedef struct {
  float x, y;
} point;

Tvector *malloc_Tvector() {
  Tvector *vec = malloc(sizeof(Tvector));
  vec->x = 0;
  vec->y = 0;

  return vec;
}

float calc_distance(float x1, float y1, float x2, float y2) {
  return sqrt((x2 - x1) * (x2 - x1) + (y2 - y1) * (y2 - y1));
}





