#include "gui_logic.h"
#include "../chess_logic.h"
#include "gui.h"
#include <SDL3/SDL.h>
#include <SDL3_ttf/SDL_ttf.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* فقط اینجا global هستند */
static Button Buttons[MAX_BUTTONS];
static int buttons_count = 0;

SDL_Color DEFAULT_BTN_COL = {192, 255, 110, 255};
SDL_Color DEFAULT_BTN_SHADOW = {64, 115, 5, 255};
SDL_Color DEFAULT_ALERT_COL = {255, 255, 255, 255};

int add_button(const char *name, const char *label, int px, int py, int width,
               int height, int border_radius) {
  if (buttons_count >= MAX_BUTTONS) {
    printf("ERROR: button limit reached\n");
    return -1;
  }

  Button *b = &Buttons[buttons_count];

  b->id = buttons_count;

  strncpy(b->name, name, MAX_NAME_LEN - 1);
  strncpy(b->label, label, MAX_NAME_LEN - 1);
  b->name[MAX_NAME_LEN - 1] = '\0';

  b->px = px;
  b->py = py;
  b->width = width;
  b->height = height;
  b->is_hovered = 0;
  b->is_active = 0;
  b->border_radius = border_radius;

  buttons_count++;
  return b->id;
}

Button *get_button(int id) {
  if (id < 0 || id >= buttons_count)
    return NULL;

  return &Buttons[id];
}

int get_buttons_count(void) { return buttons_count; }

SDL_Surface *get_button_surface(Button *button) {
  SDL_Surface *btn_surface = SDL_CreateSurface(button->width, button->height,
                                               SDL_PIXELFORMAT_ARGB8888);
  uint32_t *p = btn_surface->pixels;
  for (int x = 0; x < btn_surface->w; x++)
    for (int y = 0; y < btn_surface->h; y++)
      if (button->is_active)
        p[y * btn_surface->w + x] = DEFAULT_BTN_ACTIVE_BACKCOL;
      else if (button->is_hovered)
        p[y * btn_surface->w + x] = DEFAULT_BTN_HOVER_BACKCOL;
      else
        p[y * btn_surface->w + x] = DEFAULT_BTN_BACKCOL;
  for (int x = -button->border_radius; x < button->border_radius; x++)
    for (int y = -button->border_radius; y < button->border_radius; y++) {
      if ((x * x) + (y * y) >= button->border_radius * button->border_radius &&
          x < 0 && y < 0 && y + button->border_radius - 1 >= 0 &&
          x + button->border_radius - 1 >= 0)
        p[(y + button->border_radius - 1) * btn_surface->w + x +
          button->border_radius - 1] = 0x00000000;
    }
  for (int x = -button->border_radius; x < button->border_radius; x++)
    for (int y = -button->border_radius; y < button->border_radius; y++) {
      if ((x * x) + (y * y) >= button->border_radius * button->border_radius &&
          x > 0 && y < 0 && y + button->border_radius - 1 >= 0 &&
          x + button->width - button->border_radius + 1 <= button->width)
        p[(y + button->border_radius - 1) * btn_surface->w + x + button->width -
          button->border_radius + 1] = 0x00000000;
    }
  for (int x = -button->border_radius; x < button->border_radius; x++)
    for (int y = -button->border_radius; y < button->border_radius; y++) {
      if ((x * x) + (y * y) >= button->border_radius * button->border_radius &&
          x < 0 && y > 0 &&
          y + button->height - button->border_radius <= button->height &&
          x + button->border_radius - 1 >= 0)
        p[(y + button->height - button->border_radius) * btn_surface->w + x +
          button->border_radius - 1] = 0x00000000;
    }
  for (int x = -button->border_radius; x < button->border_radius; x++)
    for (int y = -button->border_radius; y < button->border_radius; y++) {
      if ((x * x) + (y * y) >= button->border_radius * button->border_radius &&
          x > 0 && y > 0 &&
          y + button->height - button->border_radius <= button->height &&
          x + button->width - button->border_radius >= 0)
        p[(y + button->height - button->border_radius) * btn_surface->w + x +
          button->width - button->border_radius] = 0x00000000;
    }
  SDL_Surface *txt_shadaw_surface =
      get_text_surface(button->label, 20, DEFAULT_BTN_FONT, DEFAULT_BTN_SHADOW);
  put_surface(p, button->width, button->height, txt_shadaw_surface,
              (button->width - txt_shadaw_surface->w) / 2 + 2,
              (button->height - txt_shadaw_surface->h) / 2 + 2,
              txt_shadaw_surface->w, txt_shadaw_surface->h);
  SDL_DestroySurface(txt_shadaw_surface);
  SDL_Surface *txt_surface =
      get_text_surface(button->label, 20, DEFAULT_BTN_FONT, DEFAULT_BTN_COL);
  put_surface(p, button->width, button->height, txt_surface,
              (button->width - txt_surface->w) / 2,
              (button->height - txt_surface->h) / 2, txt_surface->w,
              txt_surface->h);
  SDL_DestroySurface(txt_surface);
  return btn_surface;
}

SDL_Surface *get_alert_surface(char *text, int width, int height) {
  SDL_Surface *alert_surface =
      SDL_CreateSurface(width, height, SDL_PIXELFORMAT_ARGB8888);
  uint32_t *p = alert_surface->pixels;
  int border_radius = 20;
  for (int x = 0; x < alert_surface->w; x++)
    for (int y = 0; y < alert_surface->h; y++)
      p[y * alert_surface->w + x] = 0xFF9E1F06;
  for (int x = -border_radius; x < border_radius; x++)
    for (int y = -border_radius; y < border_radius; y++) {
      if ((x * x) + (y * y) >= border_radius * border_radius && x < 0 &&
          y < 0 && y + border_radius - 1 >= 0 && x + border_radius - 1 >= 0)
        p[(y + border_radius - 1) * alert_surface->w + x + border_radius - 1] =
            0x00000000;
    }
  for (int x = -border_radius; x < border_radius; x++)
    for (int y = -border_radius; y < border_radius; y++) {
      if ((x * x) + (y * y) >= border_radius * border_radius && x > 0 &&
          y < 0 && y + border_radius - 1 >= 0 &&
          x + width - border_radius + 1 <= width)
        p[(y + border_radius - 1) * alert_surface->w + x + width -
          border_radius + 1] = 0x00000000;
    }
  for (int x = -border_radius; x < border_radius; x++)
    for (int y = -border_radius; y < border_radius; y++) {
      if ((x * x) + (y * y) >= border_radius * border_radius && x < 0 &&
          y > 0 && y + height - border_radius <= height &&
          x + border_radius - 1 >= 0)
        p[(y + height - border_radius) * alert_surface->w + x + border_radius -
          1] = 0x00000000;
    }
  for (int x = -border_radius; x < border_radius; x++)
    for (int y = -border_radius; y < border_radius; y++) {
      if ((x * x) + (y * y) >= border_radius * border_radius && x > 0 &&
          y > 0 && y + height - border_radius <= height &&
          x + width - border_radius >= 0)
        p[(y + height - border_radius) * alert_surface->w + x + width -
          border_radius] = 0x00000000;
    }

  SDL_Surface *txt_surface =
      get_text_surface(text, 20, PROGRAM_ALERT_FONT, DEFAULT_ALERT_COL);
  put_surface(p, width, height, txt_surface, (width - txt_surface->w) / 2, 10,
              txt_surface->w, txt_surface->h);
  SDL_DestroySurface(txt_surface);
  return alert_surface;
}

uint32_t *get_chess_board(int size) {
  uint32_t *surface = malloc(size * size * sizeof(uint32_t));
  for (int x = 0; x < size; x++)
    for (int y = 0; y < size; y++) {
      if (((x / (size / 8)) + (y / (size / 8))) % 2 == 0)
        surface[y * size + x] = CHESS_WHITE_COLOR;
      else
        surface[y * size + x] = CHESS_BLACK_COLOR;
    }
  return surface;
}