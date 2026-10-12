
#include <SDL3/SDL.h>
#include <SDL3_ttf/SDL_ttf.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

void put_surface(uint32_t *dest, int dw, int dh, SDL_Surface *src, int px,
                 int py, int pw, int ph);
void put_pixels(uint32_t *dest, int dw, int dh, uint32_t *src, int px, int py,
                int pw, int ph);
void put_text(uint32_t *dest, int dw, int dh, char *text, int px, int py,
              int size, char *fontname, SDL_Color color);
uint32_t alpha_blend(uint32_t src, uint32_t dst);
SDL_Surface *get_text_surface(char *text, int size, char *fontname,
                              SDL_Color color);
void run_gui(char window_name[], int width, int height, int refresh_rate);
void stop_gui();
void reset_timer();