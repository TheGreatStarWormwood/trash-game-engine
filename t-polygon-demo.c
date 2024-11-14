
#include "T-engine.c"
#include <SDL2/SDL_render.h>

#define WIDTH 1080
#define HEIGHT 640

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

  SDL_Color color = {.r = 255, .g = 255, .b = 255, .a = 255};

  Vector vertices;
  vector_setup(&vertices, 2, sizeof(point));

  vector_push_back(&vertices, create_point(10, 10));
  vector_push_back(&vertices, create_point(10, 100));
  // vector_push_back(&vertices, create_point(100, 100));
  // vector_push_back(&vertices, create_point(100, 10));

  Tpolygon *poly = create_polygon(&vertices);

  rotate_points(poly, 90.0);

  printf("center: %f, %f", poly->center.x, poly->center.y);

  int running = 1;
  SDL_Event event;

  while (running) {
    while (SDL_PollEvent(&event)) {
      if (event.type == SDL_QUIT) {
        running = 0;
      }
    }

    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderClear(renderer);

    draw_filled_polygon(poly, color, renderer);

    SDL_RenderPresent(renderer);
  }

  free_polygon(poly);
  vector_clear(&vertices);
  vector_destroy(&vertices);
  free(game);
  SDL_DestroyRenderer(renderer);
  SDL_DestroyWindow(win);
  SDL_Quit();

  return 0;
}
