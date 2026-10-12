#include "../main.h"
#include "gui_logic.h"
#include <SDL3/SDL.h>
#include <SDL3_ttf/SDL_ttf.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

int gui_running = 1;
char *alert_text;
int alert_time = 0;
uint32_t alpha_blend(uint32_t src, uint32_t dst) {
  uint8_t sa = src >> 24;
  if (sa == 255)
    return src;
  if (sa == 0)
    return dst;

  uint8_t sr = (src >> 16) & 0xFF;
  uint8_t sg = (src >> 8) & 0xFF;
  uint8_t sb = src & 0xFF;

  uint8_t dr = (dst >> 16) & 0xFF;
  uint8_t dg = (dst >> 8) & 0xFF;
  uint8_t db = dst & 0xFF;

  uint8_t r = (sr * sa + dr * (255 - sa)) / 255;
  uint8_t g = (sg * sa + dg * (255 - sa)) / 255;
  uint8_t b = (sb * sa + db * (255 - sa)) / 255;

  return (0xFF << 24) | (r << 16) | (g << 8) | b;
}
void put_surface(uint32_t *dest, int dw, int dh, SDL_Surface *src, int px,
                 int py, int pw, int ph) {
  uint32_t *sp = (uint32_t *)src->pixels;

  float wnesbat = (float)pw / (float)src->w;
  float hnesbat = (float)ph / (float)src->h;
  for (int y = 0; y < ph; y++) {
    for (int x = 0; x < pw; x++) {
      int dx = px + x;
      int dy = py + y;

      if (dx < 0 || dy < 0 || dx >= dw || dy >= dh)
        continue;

      uint32_t p = sp[(int)(y / hnesbat) * src->w + (int)(x / wnesbat)];
      uint8_t a = p >> 24;
      if (a) {
        uint32_t *d = &dest[dy * dw + dx];
        *d = alpha_blend(p, *d);
      }
    }
  }
}
void put_pixels(uint32_t *dest, int dw, int dh, uint32_t *pixels, int px,
                int py, int pw, int ph) {
  uint32_t *sp = (uint32_t *)pixels;

  for (int y = 0; y < ph; y++) {
    for (int x = 0; x < pw; x++) {
      int dx = px + x;
      int dy = py + y;

      if (dx < 0 || dy < 0 || dx >= dw || dy >= dh)
        continue;

      uint32_t p = sp[y * pw + x];
      uint8_t a = p >> 24;
      if (a) {
        uint32_t *d = &dest[dy * dw + dx];
        *d = alpha_blend(p, *d);
      }
    }
  }
}
SDL_Surface *get_text_surface(char *text, int size, char *fontname,
                              SDL_Color color) {
  TTF_Font *font = TTF_OpenFont(fontname, size);
  if (!font) {
    printf("Font error: %s\n", SDL_GetError());
  }
  SDL_Surface *text_surface =
      TTF_RenderText_Blended(font, text, strlen(text), color);

  SDL_Surface *text_surface32 =
      SDL_ConvertSurface(text_surface, SDL_PIXELFORMAT_ARGB8888);
  TTF_CloseFont(font);
  SDL_DestroySurface(text_surface);
  return text_surface32;
}

uint64_t last_tick;
void run_gui(char window_name[], int win_width, int win_height,
             int refresh_rate) {

  SDL_Init(SDL_INIT_VIDEO);
  TTF_Init();
  SDL_Window *window = SDL_CreateWindow(window_name, win_width, win_height, 0);

  SDL_Renderer *renderer = SDL_CreateRenderer(window, NULL);

  SDL_Texture *texture =
      SDL_CreateTexture(renderer, SDL_PIXELFORMAT_ARGB8888,
                        SDL_TEXTUREACCESS_STREAMING, win_width, win_height);

  uint32_t *pixels = malloc(win_width * win_height * sizeof(uint32_t));

  bool mouse_down = false;
  int mouse_x = 0, mouse_y = 0;
  int chess_board_size = 512;
  int chess_board_x = (win_width - chess_board_size) / 2;
  int chess_board_y = (win_height - chess_board_size) / 2;
  uint32_t *chess_pixels = get_chess_board(chess_board_size);
  uint32_t *yellow_square = malloc((chess_board_size / 8) *
                                   (chess_board_size / 8) * sizeof(uint32_t));
  for (int x = 0; x < (chess_board_size / 8); x++)
    for (int y = 0; y < (chess_board_size / 8); y++)
      if (x <= 3 || y <= 3 || x >= (chess_board_size / 8) - 3 ||
          y >= (chess_board_size / 8) - 3)
        yellow_square[y * (chess_board_size / 8) + x] = 0xFFFFEA00;
      else
        yellow_square[y * (chess_board_size / 8) + x] = 0x00000000;
  uint32_t *red_square = malloc((chess_board_size / 8) *
                                (chess_board_size / 8) * sizeof(uint32_t));
  for (int x = 0; x < (chess_board_size / 8); x++)
    for (int y = 0; y < (chess_board_size / 8); y++)
      if (x <= 3 || y <= 3 || x >= (chess_board_size / 8) - 3 ||
          y >= (chess_board_size / 8) - 3)
        red_square[y * (chess_board_size / 8) + x] = 0xFFB50000;
      else
        red_square[y * (chess_board_size / 8) + x] = 0x00000000;
  uint32_t *gray_circle = malloc((chess_board_size / 8) *
                                 (chess_board_size / 8) * sizeof(uint32_t));
  for (int x = -(chess_board_size / 16); x < (chess_board_size / 16); x++)
    for (int y = -(chess_board_size / 16); y < (chess_board_size / 16); y++)
      if ((x * x) + (y * y) <= 100)
        gray_circle[(y + chess_board_size / 16) * (chess_board_size / 8) + x +
                    chess_board_size / 16] = 0xAA808080;
      else
        gray_circle[(y + chess_board_size / 16) * (chess_board_size / 8) + x +
                    chess_board_size / 16] = 0x00000000;
  uint32_t *red_circle = malloc((chess_board_size / 8) *
                                (chess_board_size / 8) * sizeof(uint32_t));
  for (int x = -(chess_board_size / 16); x < (chess_board_size / 16); x++)
    for (int y = -(chess_board_size / 16); y < (chess_board_size / 16); y++)
      if ((x * x) + (y * y) <= 100)
        red_circle[(y + chess_board_size / 16) * (chess_board_size / 8) + x +
                   chess_board_size / 16] = 0xAA880000;
      else
        red_circle[(y + chess_board_size / 16) * (chess_board_size / 8) + x +
                   chess_board_size / 16] = 0x00000000;
  uint32_t *end_dialog_rectangle =
      malloc(win_width * win_height * sizeof(uint32_t));
  for (int x = 0; x < win_width; x++)
    for (int y = 0; y < win_height; y++)
      end_dialog_rectangle[y * win_width + x] = 0xAA000000;
  for (int x = 0; x < win_width; x++)
    for (int y = 0; y < win_height; y++)
      end_dialog_rectangle[y * win_width + x] = 0x55000000;
  for (int x = (win_width - 300) / 2 + 5; x < (win_width - 300) / 2 + 300; x++)
    for (int y = (win_height - 400) / 2 + 5; y < (win_height - 400) / 2 + 400;
         y++)
      end_dialog_rectangle[y * win_width + x] = 0xAA000000;
  for (int x = (win_width - 300) / 2; x < (win_width - 300) / 2 + 295; x++)
    for (int y = (win_height - 400) / 2; y < (win_height - 400) / 2 + 395; y++)
      end_dialog_rectangle[y * win_width + x] = 0xFFFFFFFF;
  SDL_Color white = {255, 255, 255, 255};
  SDL_Color black = {0, 0, 0, 255};
  SDL_Surface *end_dialoge_title = get_text_surface(
      "بازی تمام شد!", 40, "./GUI/fonts/DANA-BLACK.TTF", black);
  put_surface(end_dialog_rectangle, win_width, win_height, end_dialoge_title,
              (win_width - 300) / 2 + (300 - end_dialoge_title->w) / 2,
              (win_height - 400) / 2 + 20, end_dialoge_title->w,
              end_dialoge_title->h);
  // uint32_t *red_circle = malloc((chess_board_size / 8) *
  //                               (chess_board_size / 8) * sizeof(uint32_t));
  // for (int x = -(chess_board_size / 16); x < (chess_board_size / 16); x++)
  //   for (int y = -(chess_board_size / 16); y < (chess_board_size / 16); y++)
  //     if ((x * x) + (y * y) <= 100)
  //       red_circle[(y + chess_board_size / 16) * (chess_board_size / 8) + x +
  //                  chess_board_size / 16] = 0xAA880000;
  //     else
  //       red_circle[(y + chess_board_size / 16) * (chess_board_size / 8) + x +
  //                  chess_board_size / 16] = 0x00000000;
  SDL_Surface *title_surface =
      get_text_surface("بازی شطرنج", 70, PROGRAM_TITLE_FONT, black);
  SDL_Surface *white_title =
      get_text_surface("بازیکن سفید", 35, PROGRAM_PLAYER_TITLE_FONT, black);
  int white_title_x = chess_board_x / 2 - white_title->w / 2;
  SDL_Surface *black_title =
      get_text_surface("بازیکن سیاه", 35, PROGRAM_PLAYER_TITLE_FONT, black);
  int black_title_x = chess_board_x + chess_board_size +
                      (win_width - chess_board_x - chess_board_size) / 2 -
                      black_title->w / 2;
  uint8_t *current_moves_avaible = malloc(sizeof(uint8_t) * 3 * 64);
  memset(current_moves_avaible, 8, 63 * 3 * sizeof(uint8_t));
  int has_to_be_newed = 0;
  int last_moves_piece_index = 40;
  while (gui_running) {
    SDL_Event ev;
    while (SDL_PollEvent(&ev)) {

      if (ev.type == SDL_EVENT_QUIT)
        gui_running = false;

      if (ev.type == SDL_EVENT_MOUSE_BUTTON_DOWN &&
          ev.button.button == SDL_BUTTON_LEFT) {
        mouse_down = true;
        for (int i = 0; i < get_buttons_count(); i++) {
          Button *button = get_button(i);
          if (mouse_x > button->px && mouse_x < button->px + button->width &&
              mouse_y > button->py && mouse_y < button->py + button->height) {
            button->is_active = 1;
          }
        }
        Piece *pieces = get_pieces();
        int selected_piece_index = 0;
        int is_new_selected = 0;
        int old_selected_index = -1;
        for (int i = 0; i < 32; i++) {
          Piece piece = pieces[i];
          int piece_x = chess_board_x + piece.x * 64;
          int piece_y = chess_board_y + piece.y * 64;
          if (piece.is_selected)
            old_selected_index = i;
          if (piece.color == get_turn() && mouse_x >= piece_x &&
              mouse_x < piece_x + chess_board_size / 8 && mouse_y >= piece_y &&
              mouse_y < piece_y + chess_board_size / 8) {
            selected_piece_index = i;
            is_new_selected = 1;
          }
        }
        if (is_new_selected)
          for (int i = 0; i < 32; i++) {
            if (i != selected_piece_index) {
              pieces[i].is_selected = 0;
              pieces[i].is_dragging = 0;
            } else {
              pieces[i].is_selected = 1;
              pieces[i].is_dragging = 1;
            }
          }
        else {
          if (old_selected_index != -1)
            if (mouse_x > chess_board_x && mouse_y > chess_board_y &&
                mouse_x < chess_board_x + chess_board_size &&
                mouse_y < chess_board_y + chess_board_size) {
              int move_result =
                  move(&pieces[old_selected_index],
                       (mouse_x - chess_board_x) / (chess_board_size / 8),
                       (mouse_y - chess_board_y) / (chess_board_size / 8));
              if (move_result == 0) {
                pieces[old_selected_index].is_selected = 0;
                set_turn(get_turn() == 'b' ? 'w' : 'b');
                memset(current_moves_avaible, 8, 64 * 3 * sizeof(uint8_t));
              } else if (move_result == 1) {
                alert_text = "حرکت غیر مجاز است";
                alert_time = 2;
              }
            }
        }
      }
      if (ev.type == SDL_EVENT_MOUSE_BUTTON_UP &&
          ev.button.button == SDL_BUTTON_LEFT) {
        mouse_down = false;
        for (int i = 0; i < get_buttons_count(); i++) {
          Button *button = get_button(i);
          if (mouse_x >= button->px && mouse_x <= button->px + button->width &&
              mouse_y >= button->py && mouse_y <= button->py + button->height &&
              button->is_active) {
            button_clicked(button->name);
          }
          button->is_active = 0;
        }
        Piece *pieces = get_pieces();
        for (int i = 0; i < 32; i++) {
          if (pieces[i].is_dragging) {
            if (mouse_x > chess_board_x && mouse_y > chess_board_y &&
                mouse_x < chess_board_x + chess_board_size &&
                mouse_y < chess_board_y + chess_board_size) {
              int move_result =
                  move(&pieces[i],
                       (mouse_x - chess_board_x) / (chess_board_size / 8),
                       (mouse_y - chess_board_y) / (chess_board_size / 8));
              if (move_result == 0) {
                pieces[i].is_selected = 0;
                set_turn(get_turn() == 'b' ? 'w' : 'b');
                memset(current_moves_avaible, 8, 64 * 3 * sizeof(uint8_t));
              } else if (move_result == 1 &&
                         ((mouse_x - chess_board_x) / (chess_board_size / 8) !=
                              pieces[i].x ||
                          (mouse_y - chess_board_y) / (chess_board_size / 8) !=
                              pieces[i].y)) {
                alert_text = "حرکت غیر مجاز است";
                alert_time = 2;
              }
            }
          }
          pieces[i].is_dragging = 0;
        }
      }

      if (ev.type == SDL_EVENT_MOUSE_MOTION) {
        mouse_x = ev.motion.x;
        mouse_y = ev.motion.y;
      }
    }

    for (int i = 0; i < win_width * win_height; i++)
      pixels[i] = 0xFFFFFFFF;
    char *program_status = get_program_status();

    if (strcmp(program_status, "main_menu") == 0) {
      put_surface(pixels, win_width, win_height, title_surface,
                  (win_width - title_surface->w) / 2, 50, title_surface->w,
                  title_surface->h);
      for (int i = 0; i < 2; i++) {
        Button *button = get_button(i);
        if (mouse_x > button->px && mouse_x < button->px + button->width &&
            mouse_y > button->py && mouse_y < button->py + button->height)
          button->is_hovered = 1;
        else
          button->is_hovered = 0;
        SDL_Surface *btn_surface = get_button_surface(button);
        put_surface(pixels, win_width, win_height, btn_surface, button->px,
                    button->py, button->width, button->height);
        SDL_DestroySurface(btn_surface);
      }
    } else if (strcmp(program_status, "playing") == 0 ||
               strcmp(program_status, "ended") == 0) {
      put_pixels(pixels, win_width, win_height, chess_pixels, chess_board_x,
                 chess_board_y, chess_board_size, chess_board_size);
      put_surface(pixels, win_width, win_height, white_title, white_title_x,
                  chess_board_y, white_title->w, white_title->h);
      put_surface(pixels, win_width, win_height, black_title, black_title_x,
                  chess_board_y, black_title->w, black_title->h);
      char *white_timer_text = get_timer_text('w');
      SDL_Surface *white_timer_surface =
          get_text_surface(white_timer_text, 40, PROGRAM_TIMER_FONT, black);
      put_surface(pixels, win_width, win_height, white_timer_surface,
                  chess_board_x / 2 - white_timer_surface->w / 2,
                  chess_board_y + 60, white_timer_surface->w,
                  white_timer_surface->h);
      char *black_timer_text = get_timer_text('b');
      SDL_Surface *black_timer_surface =
          get_text_surface(black_timer_text, 40, PROGRAM_TIMER_FONT, black);

      put_surface(pixels, win_width, win_height, black_timer_surface,
                  chess_board_x + chess_board_size + chess_board_x / 2 -
                      black_timer_surface->w / 2,
                  chess_board_y + 60, black_timer_surface->w,
                  black_timer_surface->h);
      SDL_DestroySurface(white_timer_surface);
      SDL_DestroySurface(black_timer_surface);
      // get_text_surface("", 35, PROGRAM_PLAYER_TITLE_FONT, black);
      free(white_timer_text);
      free(black_timer_text);
      if (mouse_x >= chess_board_x &&
          mouse_x <= chess_board_x + chess_board_size &&
          mouse_y >= chess_board_y &&
          mouse_y <= chess_board_y + chess_board_size)
        put_pixels(pixels, win_width, win_height, yellow_square,
                   chess_board_x + ((mouse_x - chess_board_x) / 64) * 64,
                   chess_board_y + ((mouse_y - chess_board_y) / 64) * 64,
                   chess_board_size / 8, chess_board_size / 8);
      Piece *pieces = get_pieces();
      for (int i = 0; i < 32; i++) {
        Piece piece = pieces[i];
        int piece_x = chess_board_x + piece.x * 64;
        int piece_y = chess_board_y + piece.y * 64;
        int is_any_selected = 0;
        if (piece.is_selected && i != last_moves_piece_index) {

          last_moves_piece_index = i;
          get_regular_moves(current_moves_avaible, &pieces[i]);
          has_to_be_newed = 1;
        }
        if (!piece.is_removed)
          if (piece.is_dragging) {
            put_surface(pixels, win_width, win_height, piece.img,
                        mouse_x - chess_board_size / 16,
                        mouse_y - chess_board_size / 16, 64, 64);
          } else {
            put_surface(pixels, win_width, win_height, piece.img, piece_x,
                        piece_y, 64, 64);
            if (piece.is_selected) {
              put_pixels(pixels, win_width, win_height, yellow_square, piece_x,
                         piece_y, chess_board_size / 8, chess_board_size / 8);
              is_any_selected = 1;
            }
            if (piece.is_checked)
              put_pixels(pixels, win_width, win_height, red_square, piece_x,
                         piece_y, chess_board_size / 8, chess_board_size / 8);
          }
      }
      for (int i = 0; current_moves_avaible[i] != 8; i += 3) {
        put_pixels(
            pixels, win_width, win_height,
            current_moves_avaible[i + 2] == 0 ? gray_circle : red_circle,
            chess_board_x + current_moves_avaible[i] * chess_board_size / 8,
            chess_board_y + current_moves_avaible[i + 1] * chess_board_size / 8,
            chess_board_size / 8, chess_board_size / 8);
      }
      // if (mouse_down) {
      // } else {
      //   put_surface(pixels, win_width, win_height, bmp32, 0, 0, 64, 64);
      // }
      if (strcmp(program_status, "ended") == 0) {
        put_pixels(pixels, win_width, win_height, end_dialog_rectangle, 0, 0,
                   win_width, win_height);
        SDL_Surface *end_dialoge_subtitle =
            get_text_surface(get_winner() == 'b' ? "برنده بازی: بازیکن سیاه"
                                                 : "برنده بازی: بازیکن سفید",
                             20, "./GUI/fonts/DANA-REGULAR.TTF", black);
        put_surface(pixels, win_width, win_height, end_dialoge_subtitle,
                    (win_width - 300) / 2 + (300 - end_dialoge_subtitle->w) / 2,
                    (win_height - 400) / 2 + 70, end_dialoge_subtitle->w,
                    end_dialoge_subtitle->h);
        for (int i = 2; i < 4; i++) {
          Button *button = get_button(i);
          if (mouse_x > button->px && mouse_x < button->px + button->width &&
              mouse_y > button->py && mouse_y < button->py + button->height)
            button->is_hovered = 1;
          else
            button->is_hovered = 0;
          SDL_Surface *btn_surface = get_button_surface(button);
          put_surface(pixels, win_width, win_height, btn_surface, button->px,
                      button->py, button->width, button->height);
          SDL_DestroySurface(btn_surface);
        }
      }
    }
    if (alert_time > 0) {
      SDL_Surface *alert_surface =
          get_alert_surface(alert_text, chess_board_size,
                            (win_height - chess_board_size) / 2 - 40);
      put_surface(pixels, win_width, win_height, alert_surface,
                  (win_width - chess_board_size) / 2, 20, alert_surface->w,
                  alert_surface->h);
      SDL_DestroySurface(alert_surface);
    }
    SDL_UpdateTexture(texture, NULL, pixels, win_width * sizeof(uint32_t));

    SDL_RenderClear(renderer);
    SDL_RenderTexture(renderer, texture, NULL, NULL);
    SDL_RenderPresent(renderer);
    uint64_t now = SDL_GetTicks();

    if (strcmp(program_status, "playing") == 0)
      if (now - last_tick >= 1000) {
        second_passed();
        last_tick += 1000;
        // printf("is b checked:%d", is_check('b'));
        // printf("is w checked:%d", is_check('w'));
        if (alert_time > 0)
          alert_time--;
      }
    SDL_Delay(1000 / refresh_rate);
  }

  free(pixels);

  SDL_DestroySurface(title_surface);
  free(chess_pixels);
  SDL_DestroyTexture(texture);
  SDL_DestroyRenderer(renderer);
  SDL_DestroyWindow(window);
  TTF_Quit();
  SDL_Quit();
}
void stop_gui() { gui_running = 0; }
void reset_timer() { last_tick = SDL_GetTicks(); }