#include "FlipClock.h"

int calculateRadius(int width, int height, int percent) {
  float smallest = width < height ? width : height;
  if (percent > 100) {
    percent = 100;
  } else if (percent < 0) {
    percent = 0;
  }
  return (smallest / 2 * percent) / 100;
}

void SDL_RenderFillRoundedRect(SDL_Renderer *renderer, int x, int y, int w,
                               int h, int percent) {
  int radius = calculateRadius(w, h, percent);

  int x1 = x + radius;
  int x2 = x + w - radius;
  int y1 = y + radius;
  int y2 = y + h - radius;

  SDL_Rect rect = {x1, y, w - radius * 2, h};
  SDL_RenderFillRect(renderer, &rect);
  rect = (SDL_Rect){x, y1, w, h - radius * 2};
  SDL_RenderFillRect(renderer, &rect);

  SDL_RenderFillCircle(renderer, x1, y1, radius);
  SDL_RenderFillCircle(renderer, x2, y1, radius);
  SDL_RenderFillCircle(renderer, x1, y2, radius);
  SDL_RenderFillCircle(renderer, x2, y2, radius);
}

void SDL_RenderFillCircle(SDL_Renderer *renderer, int cx, int cy, int radius) {
  const int diameter = (radius * 2);

  int x = (radius - 1);
  int y = 0;
  int tx = 1;
  int ty = 1;
  int error = (tx - diameter);

  while (x >= y) {
    for (int i = cx - x; i <= cx + x; i++) {
      SDL_RenderDrawPoint(renderer, i, cy + y);
      SDL_RenderDrawPoint(renderer, i, cy - y);
    }
    for (int i = cx - y; i <= cx + y; i++) {
      SDL_RenderDrawPoint(renderer, i, cy + x);
      SDL_RenderDrawPoint(renderer, i, cy - x);
    }

    if (error <= 0) {
      ++y;
      error += ty;
      ty += 2;
    }
    if (error > 0) {
      --x;
      tx += 2;
      error += (tx - diameter);
    }
  }
}