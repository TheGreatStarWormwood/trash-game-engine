
#include "vector/vector.c"
#include "vector/vector.h"
#include <SDL2/SDL.h>
#include <SDL2/SDL_scancode.h>
#include <SDL2/SDL_ttf.h>
#include <float.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>

typedef struct {
  float x;
  float y;
} Tvector_t;

typedef struct {
  float x, y;
} point;

typedef struct {
  point *vertices;
  int vert_count;
  point center;
  double angle; // write in radian using M_PI
} Tpolygon;

float calc_distance(float x1, float y1, float x2, float y2) {
  return sqrt((x2 - x1) * (x2 - x1) + (y2 - y1) * (y2 - y1));
}

float get_angle_between_points(float x1, float y1, float x2, float y2) {
  return atan2(y2 - y1, x2 - x1);
}

Tvector_t *malloc_Tvector() {
  Tvector_t *vec = malloc(sizeof(Tvector_t));
  vec->x = 0;
  vec->y = 0;

  return vec;
}

Tvector_t *create_vector(float x, float y) {
  Tvector_t *vec = malloc_Tvector();
  vec->x = x;
  vec->y = y;
  return vec;
}

Tpolygon *malloc_polygon() {
  Tpolygon *polygon = malloc(sizeof(Tpolygon));

  return polygon;
}

point *create_point(float x, float y) {
  point *p = malloc(sizeof(point));
  p->x = x;
  p->y = y;
  return p;
}

Tpolygon *create_polygon(Vector *vertices) {
  Tpolygon *polygon = malloc_polygon();

  float minX = FLT_MAX, minY = FLT_MAX, maxX = 0, maxY = 0;
  polygon->vert_count = vertices->size;
  polygon->vertices = malloc(sizeof(point) * polygon->vert_count);
  for (int i = 0; i < polygon->vert_count; i++) {
    polygon->vertices[i] = *create_point(((point *)vector_get(vertices, i))->x,
                                         ((point *)vector_get(vertices, i))->y);
    if (polygon->vertices[i].x > maxX)
      maxX = polygon->vertices[i].x;
    if (polygon->vertices[i].x < minX)
      minX = polygon->vertices[i].x;
    if (polygon->vertices[i].y > maxY)
      maxY = polygon->vertices[i].y;
    if (polygon->vertices[i].y < minY)
      minY = polygon->vertices[i].y;
  }
  polygon->center.x = minX + ((maxX - minX) / 2);
  polygon->center.y = minY + ((maxY - minY) / 2);

  return polygon;
}

void free_polygon(Tpolygon *polygon) {
  free(polygon->vertices);
  free(polygon);
}

/*
  polygon stuff modified for my C implementation from
  https://stackoverflow.com/a/49512921
*/

int draw_filled_polygon(Tpolygon *poly, const SDL_Color color,
                        SDL_Renderer *renderer) {
  float startX, endX, leftSlope = 0, rightSlope = 0;
  float cntY;
  int topCnt, leftCnt, rightCnt;
  int numVerts = poly->vert_count;
  int numVertsProc = 1;

  // Find the top-most vertex relative to the center
  float topY = poly->vertices[0].y;
  topCnt = 0;
  for (int cnt = 1; cnt < numVerts; cnt++) {
    if (poly->vertices[cnt].y < topY) {
      topY = poly->vertices[cnt].y;
      topCnt = cnt;
    }
  }

  // Initialize counters for left and right traversal
  leftCnt = topCnt - 1;
  if (leftCnt < 0)
    leftCnt = numVerts - 1;

  rightCnt = topCnt + 1;
  if (rightCnt >= numVerts)
    rightCnt = 0;

  // Initialize starting positions and slopes
  startX = endX = poly->vertices[topCnt].x;
  cntY = poly->vertices[topCnt].y;

  if (poly->vertices[leftCnt].y != poly->vertices[topCnt].y)
    leftSlope = (poly->vertices[leftCnt].x - poly->vertices[topCnt].x) /
                (poly->vertices[leftCnt].y - poly->vertices[topCnt].y);
  if (poly->vertices[rightCnt].y != poly->vertices[topCnt].y)
    rightSlope = (poly->vertices[rightCnt].x - poly->vertices[topCnt].x) /
                 (poly->vertices[rightCnt].y - poly->vertices[topCnt].y);

  SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, color.a);

  // Main drawing loop
  while (numVertsProc < numVerts) {
    // Draw lines until reaching the next vertex on either side
    while (cntY < poly->vertices[leftCnt].y &&
           cntY < poly->vertices[rightCnt].y) {
      SDL_RenderDrawLine(renderer, (int)(startX + poly->center.x),
                         (int)(cntY + poly->center.y),
                         (int)(endX + poly->center.x),
                         (int)(cntY + poly->center.y));
      cntY++;
      startX += leftSlope;
      endX += rightSlope;
    }

    // Update left side if needed
    if (poly->vertices[leftCnt].y <= cntY) {
      topCnt = leftCnt;
      leftCnt--;
      if (leftCnt < 0)
        leftCnt = numVerts - 1;

      if (poly->vertices[leftCnt].y != poly->vertices[topCnt].y)
        leftSlope = (poly->vertices[leftCnt].x - poly->vertices[topCnt].x) /
                    (poly->vertices[leftCnt].y - poly->vertices[topCnt].y);
      startX = poly->vertices[topCnt].x;
      numVertsProc++;
    }

    // Update right side if needed
    if (poly->vertices[rightCnt].y <= cntY) {
      topCnt = rightCnt;
      rightCnt++;
      if (rightCnt == numVerts)
        rightCnt = 0;

      if (poly->vertices[rightCnt].y != poly->vertices[topCnt].y)
        rightSlope = (poly->vertices[rightCnt].x - poly->vertices[topCnt].x) /
                     (poly->vertices[rightCnt].y - poly->vertices[topCnt].y);
      endX = poly->vertices[topCnt].x;
      numVertsProc++;
    }

    // Draw the horizontal line at current Y level
    SDL_RenderDrawLine(
        renderer, (int)(startX + poly->center.x), (int)(cntY + poly->center.y),
        (int)(endX + poly->center.x), (int)(cntY + poly->center.y));
  }

  return 1;
}

void rotate_points(Tpolygon *poly, double angle) {
  double cos_angle = cos(angle);
  double sin_angle = sin(angle);
  double temp;

  for (int n = 0; n < poly->vert_count; n++) {
    temp = (poly->vertices[n].x * cos_angle - poly->vertices[n].y * sin_angle);
    poly->vertices[n].y =
        (poly->vertices[n].x * sin_angle + poly->vertices[n].y * cos_angle);
    poly->vertices[n].x = temp;
  }
}
