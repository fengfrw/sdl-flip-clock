#include "FlipClock.h"

int draw_loop() {
  printf("##Start rendering loop\n");

  _G.lastTickCount = SDL_GetTicks();
  _G.curTickCount = _G.lastTickCount;

  float fps = 0.0f;
  int frame = _G.curTickCount;
  int frameCount = 0;

  while (_G.quit == 0) {
    _G.curTickCount = SDL_GetTicks();
    pollevent();
    handle_key();

    // sync frame rate
    if (_G.curTickCount - _G.lastTickCount < 1000 / FRAME_RATE) {
      continue;
    }

    // calculate FPS
    ++frameCount;
    if (_G.curTickCount - frame >= 1000) {
      fps = frameCount / ((_G.curTickCount - frame) / 1000.f);
      frameCount = 0;
      frame = _G.curTickCount;
    }

    // before draw
    RenderClear(_G.renderer, BG_COLOR);

    // draw
    draw_screen(fps);

    // after draw
    RenderPresent(_G.renderer, _G.window);
    _G.lastTickCount = _G.curTickCount;
  }

  printf("##Finish rendering loop\n");
  return 0;
}

int handle_key() {
  // MENU + SELECT -> Exit
  if (_G.KEY_STATUS[BTN_MENU_INDEX] && _G.KEY_STATUS[BTN_SELECT_INDEX]) {
    _G.quit = 1;
  }

  // START -> toggle `12/24` mode
  if (_G.KEY_STATUS[BTN_START_INDEX] &&
      _G.KEY_STATUS[BTN_START_INDEX] <= _G.curTickCount) {
    _G.KEY_STATUS[BTN_START_INDEX] = _G.curTickCount + INPUT_DELAY;
    if (is_active_mode(MODE_AMPM)) {
      _G.mode &= ~(1 << MODE_AMPM);
    } else {
      _G.mode |= 1 << MODE_AMPM;
    }
  }

  // MENU + X -> toggle `FPS` mode
  if (_G.KEY_STATUS[BTN_MENU_INDEX] &&
      _G.KEY_STATUS[BTN_MENU_INDEX] <= _G.curTickCount &&
      _G.KEY_STATUS[BTN_X_INDEX] &&
      _G.KEY_STATUS[BTN_X_INDEX] <= _G.curTickCount) {

    _G.KEY_STATUS[BTN_MENU_INDEX] = _G.curTickCount + INPUT_DELAY;
    _G.KEY_STATUS[BTN_X_INDEX] = _G.curTickCount + INPUT_DELAY;

    if (is_active_mode(MODE_FPS)) {
      _G.mode &= ~(1 << MODE_FPS);
    } else {
      _G.mode |= 1 << MODE_FPS;
    }
  }

  return 0;
}

#define FLIPCLOCK_HOUR 0
#define FLIPCLOCK_MINIUTE 1

int draw_screen(float fps) {
  struct tm *time = get_local_time();

  int x = _G.MARGIN_X;
  int y = (_G.DISPLAY_HEIGHT - _G.FLIP_SIZE) / 2;

  for (int layout = 0; layout < 2; ++layout) {
    int display_time_number =
        layout == FLIPCLOCK_HOUR ? time->tm_hour : time->tm_min;

    if (layout == FLIPCLOCK_HOUR && is_active_mode(MODE_AMPM)) {
      display_time_number %= 12;
      if (display_time_number == 0) {
        display_time_number = 12;
      }
    }

    draw_flip(x, y);
    draw_time(x, y, display_time_number);

    if (layout == FLIPCLOCK_HOUR && is_active_mode(MODE_AMPM)) {
      draw_ampm(x, y, time->tm_hour);
    }

    // Update Position
    x += _G.GAP + _G.FLIP_SIZE;
  }

  draw_divider();

  if (is_active_mode(MODE_FPS)) {
    draw_fps(fps);
  }

  return 0;
}

struct tm *get_local_time() {
  time_t ltime;
  ltime = time(&ltime);
  return localtime(&ltime);
}

void draw_flip(int x, int y) {
  RenderFillRoundedRect(_G.renderer, x, y, _G.FLIP_SIZE, _G.FLIP_SIZE, 10,
                        FLIP_COLOR);
}

void draw_time(int x, int y, int time) {
  char timeString[3] = {
      time / 10 + '0',
      time % 10 + '0',
      '\0',
  };

  SDL_Surface *textSurface =
      RenderText_Blended(_G.timeFont, timeString, FONT_COLOR);

  BlitSurface(
      textSurface, NULL, _G.renderer,
      &(SDL_Rect){x + _G.FLIP_PADDING + (_G.TIME_SIZE - textSurface->w) / 2,
                  y + _G.FLIP_PADDING + _G.timeFontDescent, textSurface->w,
                  textSurface->h});

  // free allocated memories
  SDL_FreeSurface(textSurface);
}

void draw_ampm(int x, int y, int hour) {
  SDL_Surface *textSurface =
      RenderText_Blended(_G.ampmFont, hour < 12 ? "AM" : "PM", FONT_COLOR);

  BlitSurface(textSurface, NULL, _G.renderer,
              &(SDL_Rect){x + _G.FLIP_PADDING / 2,
                          y + _G.FLIP_SIZE - _G.FLIP_PADDING / 2 -
                              _G.AMPM_SIZE + _G.ampmFontDescent,
                          textSurface->w, textSurface->h});

  // free allocated memories
  SDL_FreeSurface(textSurface);
}

void draw_divider() {
  RenderFillRect(_G.renderer,
                 &(SDL_Rect){0, _G.DISPLAY_HEIGHT / 2 - _G.DIVIDER,
                             _G.DISPLAY_WIDTH, _G.DIVIDER},
                 BG_COLOR);
}

void draw_fps(float fps) {
  char FPS[11] = {
      '\0',
  };

  sprintf(FPS, "FPS: %.2f", fps);
  SDL_Surface *textSurface = RenderText_Blended(_G.ampmFont, FPS, FONT_COLOR);
  BlitSurface(textSurface, NULL, _G.renderer,
              &(SDL_Rect){4, 4, textSurface->w, textSurface->h});

  // free allocated memories
  SDL_FreeSurface(textSurface);
}
