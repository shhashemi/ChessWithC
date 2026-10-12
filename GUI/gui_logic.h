#ifndef GUI_LOGIC_H
#define GUI_LOGIC_H

#include <SDL3/SDL.h>
#include <SDL3_ttf/SDL_ttf.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_BUTTONS 50
#define MAX_NAME_LEN 64
#define DEFAULT_BTN_BACKCOL 0xFF58CB52
#define DEFAULT_BTN_HOVER_BACKCOL 0xFF3DEB42
#define DEFAULT_BTN_ACTIVE_BACKCOL 0xFF13AD17
#define DEFAULT_BTN_FONT "./GUI/fonts/DANA-BLACK.TTF"
#define PROGRAM_TITLE_FONT "./GUI/fonts/DANA-FAT.TTF"
#define PROGRAM_PLAYER_TITLE_FONT "./GUI/fonts/DANA-EXTRABLACK.TTF"
#define PROGRAM_TIMER_FONT "./GUI/fonts/AGENCYB.TTF"
#define PROGRAM_ALERT_FONT "./GUI/fonts/DANA-MEDIUM.TTF"
#define CHESS_WHITE_COLOR 0xFFBCAE87
#define CHESS_BLACK_COLOR 0xFF52623B

typedef struct {
  int id;
  char name[MAX_NAME_LEN];
  char label[MAX_NAME_LEN];
  int px;
  int py;
  int width;
  int height;
  int is_hovered;
  int is_active;
  int border_radius;
} Button;

/* API */
int add_button(const char *name, const char *label, int px, int py, int width,
               int height, int border_radius);
Button *get_button(int id);
int get_buttons_count(void);
SDL_Surface *get_button_surface(Button *button);
uint32_t *get_chess_board(int size);
SDL_Surface *get_alert_surface(char *text, int width, int height);
#endif
