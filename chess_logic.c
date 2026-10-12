#include "chess_logic.h"
#include "GUI/gui.h"
#include <SDL3/SDL.h>
#include <SDL3_ttf/SDL_ttf.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
// pawn
char program_status[20] = "main_menu"; // main_menu - playing
Piece pieces[32];

SDL_Surface *black_pawn;
SDL_Surface *white_pawn;
SDL_Surface *black_gryphon;
SDL_Surface *white_gryphon;
SDL_Surface *black_dragon;
SDL_Surface *white_dragon;
SDL_Surface *black_king;
SDL_Surface *white_king;
SDL_Surface *black_queen;
SDL_Surface *white_queen;
SDL_Surface *black_thief;
SDL_Surface *white_thief;

char turn;

int white_timer = 0;
int black_timer = 0;
char winner;
int is_x_y_inrange(int px, int py, int x, int y);
void init_pieces() {
  turn = 'w';
  white_timer = 0;
  black_timer = 0;
  SDL_Surface *bmp = SDL_LoadBMP("./GUI/imgs/black-pawn.bmp");
  black_pawn = SDL_ConvertSurface(bmp, SDL_PIXELFORMAT_ARGB8888);
  bmp = SDL_LoadBMP("./GUI/imgs/white-pawn.bmp");
  white_pawn = SDL_ConvertSurface(bmp, SDL_PIXELFORMAT_ARGB8888);
  bmp = SDL_LoadBMP("./GUI/imgs/black-gryphon.bmp");
  black_gryphon = SDL_ConvertSurface(bmp, SDL_PIXELFORMAT_ARGB8888);
  bmp = SDL_LoadBMP("./GUI/imgs/white-gryphon.bmp");
  white_gryphon = SDL_ConvertSurface(bmp, SDL_PIXELFORMAT_ARGB8888);
  bmp = SDL_LoadBMP("./GUI/imgs/black-king.bmp");
  black_king = SDL_ConvertSurface(bmp, SDL_PIXELFORMAT_ARGB8888);
  bmp = SDL_LoadBMP("./GUI/imgs/white-king.bmp");
  white_king = SDL_ConvertSurface(bmp, SDL_PIXELFORMAT_ARGB8888);
  bmp = SDL_LoadBMP("./GUI/imgs/black-queen.bmp");
  black_queen = SDL_ConvertSurface(bmp, SDL_PIXELFORMAT_ARGB8888);
  bmp = SDL_LoadBMP("./GUI/imgs/white-queen.bmp");
  white_queen = SDL_ConvertSurface(bmp, SDL_PIXELFORMAT_ARGB8888);
  bmp = SDL_LoadBMP("./GUI/imgs/black-dragon.bmp");
  black_dragon = SDL_ConvertSurface(bmp, SDL_PIXELFORMAT_ARGB8888);
  bmp = SDL_LoadBMP("./GUI/imgs/white-dragon.bmp");
  white_dragon = SDL_ConvertSurface(bmp, SDL_PIXELFORMAT_ARGB8888);
  bmp = SDL_LoadBMP("./GUI/imgs/black-thief.bmp");
  black_thief = SDL_ConvertSurface(bmp, SDL_PIXELFORMAT_ARGB8888);
  bmp = SDL_LoadBMP("./GUI/imgs/white-thief.bmp");
  white_thief = SDL_ConvertSurface(bmp, SDL_PIXELFORMAT_ARGB8888);
  SDL_DestroySurface(bmp);
  memset(pieces, 0, 32 * sizeof(Piece));
  int i = 0;
  for (i = 0; i < 8; i++) {
    strcpy(pieces[i].name, "pawn");
    pieces[i].color = 'b';
    pieces[i].img = black_pawn;
    pieces[i].x = i;
    pieces[i].y = 1;
  }
  for (i = 8; i < 16; i++) {
    strcpy(pieces[i].name, "pawn");
    pieces[i].color = 'w';
    pieces[i].img = white_pawn;
    pieces[i].x = i - 8;
    pieces[i].y = 6;
  }
  for (int i = 16; i < 20; i++) {
    strcpy(pieces[i].name, "gryphon");
    pieces[i].color = (i - 16) / 2 == 0 ? 'b' : 'w';
    pieces[i].img = (i - 16) / 2 == 0 ? black_gryphon : white_gryphon;
    pieces[i].x = (i - 16) % 2 == 0 ? 0 : 7;
    pieces[i].y = (i - 16) / 2 == 0 ? 0 : 7;
  }
  for (int i = 20; i < 24; i++) {
    strcpy(pieces[i].name, "dragon");
    pieces[i].color = (i - 20) / 2 == 0 ? 'b' : 'w';
    pieces[i].img = (i - 20) / 2 == 0 ? black_dragon : white_dragon;
    pieces[i].x = (i - 20) % 2 == 0 ? 1 : 6;
    pieces[i].y = (i - 20) / 2 == 0 ? 0 : 7;
  }
  for (int i = 24; i < 28; i++) {
    strcpy(pieces[i].name, "thief");
    pieces[i].color = (i - 24) / 2 == 0 ? 'b' : 'w';
    pieces[i].img = (i - 24) / 2 == 0 ? black_thief : white_thief;
    pieces[i].x = (i - 24) % 2 == 0 ? 2 : 5;
    pieces[i].y = (i - 24) / 2 == 0 ? 0 : 7;
  }
  for (int i = 28; i < 30; i++) {
    strcpy(pieces[i].name, "king");
    pieces[i].color = (i - 28) % 2 == 0 ? 'b' : 'w';
    pieces[i].img = (i - 28) % 2 == 0 ? black_king : white_king;
    pieces[i].x = 3;
    pieces[i].y = (i - 28) % 2 == 0 ? 0 : 7;
  }
  for (int i = 30; i < 32; i++) {
    strcpy(pieces[i].name, "queen");
    pieces[i].color = (i - 30) % 2 == 0 ? 'b' : 'w';
    pieces[i].img = (i - 30) % 2 == 0 ? black_queen : white_queen;
    pieces[i].x = 4;
    pieces[i].y = (i - 30) % 2 == 0 ? 0 : 7;
  }
  for (int i = 0; i < 32; i++) {

    pieces[i].is_removed = 0;
    pieces[i].moves_count = 0;
    pieces[i].is_selected = 0;
    pieces[i].is_dragging = 0;
    pieces[i].is_checked = 0;
  }
}
void second_passed() {
  if (turn == 'w')
    white_timer++;
  else
    black_timer++;
}
char *get_timer_text(char color) {
  int seconds = color == 'w' ? white_timer : black_timer;
  if ((seconds / 60) / 100 == 0) {
    char *timer_text = malloc(sizeof(char) * 6);
    timer_text[2] = ':';
    timer_text[4] = seconds % 10 + '0';
    timer_text[3] = (seconds % 60) / 10 + '0';
    timer_text[1] = (seconds / 60) % 10 + '0';
    timer_text[0] = seconds / 600 + '0';
    timer_text[5] = 0;
    return timer_text;
  } else {
    char *timer_text = malloc(sizeof(char) * 7);
    timer_text[3] = ':';
    timer_text[5] = (seconds % 10) + '0';
    timer_text[4] = (seconds % 60) / 10 + '0';
    timer_text[2] = ((seconds / 60) % 10) + '0';
    timer_text[1] = ((seconds / 600) % 10) + '0';
    timer_text[0] = (seconds / 6000) + '0';
    timer_text[6] = 0;
    return timer_text;
  }
}
char get_turn() { return turn; }
void set_turn(char t) { turn = t; }
Piece *get_pieces() { return pieces; }
void get_moves(uint8_t *moves, Piece *piece) {
  for (int i = 0; i < 63; i++)
    moves[i] = 8;
  if (strcmp(piece->name, "pawn") == 0) {
    int moves_count = 0;
    if (piece->color == 'w') {
      int jumps = 0;
      for (int y = -1; y >= -2 + (piece->moves_count & 1); y--) {
        int is_any_piece_there = 0;
        for (int i = 0; i < 32; i++) {
          if (pieces[i].x == piece->x && pieces[i].y == piece->y + y) {
            is_any_piece_there = 1;
            jumps++;
            break;
          }
        }
        if (!is_any_piece_there) {
          moves[moves_count * 3] = piece->x;
          moves[(moves_count * 3) + 1] = piece->y + y;
          moves[(moves_count * 3) + 2] = 0;
          moves_count++;
        }
        if (jumps > 0)
          break;
      }
      for (int i = 0; i < 32; i++) {
        if (pieces[i].x == piece->x - 1 && pieces[i].y == piece->y - 1 &&
            pieces[i].color != piece->color) {
          moves[moves_count * 3] = piece->x - 1;
          moves[(moves_count * 3) + 1] = piece->y - 1;
          moves[(moves_count * 3) + 2] = 1;
          moves_count++;
          break;
        }
      }
      for (int i = 0; i < 32; i++) {
        if (pieces[i].x == piece->x + 1 && pieces[i].y == piece->y - 1 &&
            pieces[i].color != piece->color) {
          moves[moves_count * 3] = piece->x + 1;
          moves[(moves_count * 3) + 1] = piece->y - 1;
          moves[(moves_count * 3) + 2] = 1;
          moves_count++;
          break;
        }
      }
    } else if (piece->color == 'b') {
      int jumps = 0;
      for (int y = 1; y <= 2 - (piece->moves_count & 1); y++) {
        int is_any_piece_there = 0;
        for (int i = 0; i < 32; i++) {
          if (pieces[i].x == piece->x && pieces[i].y == piece->y + y) {
            is_any_piece_there = 1;
            jumps++;
            break;
          }
        }
        if (!is_any_piece_there) {
          moves[moves_count * 3] = piece->x;
          moves[(moves_count * 3) + 1] = piece->y + y;
          moves[(moves_count * 3) + 2] = 0;
          moves_count++;
        }
        if (jumps > 0)
          break;
      }
      for (int i = 0; i < 32; i++) {
        if (pieces[i].x == piece->x - 1 && pieces[i].y == piece->y + 1 &&
            pieces[i].color != piece->color) {
          moves[moves_count * 3] = piece->x - 1;
          moves[(moves_count * 3) + 1] = piece->y + 1;
          moves[(moves_count * 3) + 2] = 1;
          moves_count++;
          break;
        }
      }
      for (int i = 0; i < 32; i++) {
        if (pieces[i].x == piece->x + 1 && pieces[i].y == piece->y + 1 &&
            pieces[i].color != piece->color) {
          moves[moves_count * 3] = piece->x + 1;
          moves[(moves_count * 3) + 1] = piece->y + 1;
          moves[(moves_count * 3) + 2] = 1;
          moves_count++;
          break;
        }
      }
    }

  } else if (strcmp(piece->name, "gryphon") == 0) {
    int moves_count = 0;
    for (int x = -1; x <= 1; x += 2) {
      int jumps = 0;
      for (int y = 1; y <= 7; y++) {
        if (y == 0)
          jumps = 0;
        if (y != 0 && is_x_y_inrange(piece->x, piece->y, x, y)) {
          int is_any_piece_there = 0;
          for (int i = 0; i < 32; i++) {
            if (pieces[i].x == piece->x + x && pieces[i].y == piece->y + y) {
              is_any_piece_there = 1;
              if (pieces[i].color != piece->color) {
                moves[moves_count * 3] = piece->x + x;
                moves[(moves_count * 3) + 1] = piece->y + y;
                moves[(moves_count * 3) + 2] = 1;
                moves_count++;
              }
              jumps++;
            }
          }
          if (!is_any_piece_there) {
            moves[moves_count * 3] = piece->x + x;
            moves[(moves_count * 3) + 1] = piece->y + y;
            moves[(moves_count * 3) + 2] = 0;
            moves_count++;
          }
        }
        if (jumps > 0)
          break;
      }
      jumps = 0;
      for (int y = -1; y >= -7; y--) {
        if (y != 0 && is_x_y_inrange(piece->x, piece->y, x, y)) {
          int is_any_piece_there = 0;
          for (int i = 0; i < 32; i++) {
            if (pieces[i].x == piece->x + x && pieces[i].y == piece->y + y) {
              is_any_piece_there = 1;
              if (pieces[i].color != piece->color) {
                moves[moves_count * 3] = piece->x + x;
                moves[(moves_count * 3) + 1] = piece->y + y;
                moves[(moves_count * 3) + 2] = 1;
                moves_count++;
              }
              jumps++;
            }
          }
          if (!is_any_piece_there) {
            moves[moves_count * 3] = piece->x + x;
            moves[(moves_count * 3) + 1] = piece->y + y;
            moves[(moves_count * 3) + 2] = 0;
            moves_count++;
          }
        }
        if (jumps > 0)
          break;
      }
    }
    for (int y = -1; y <= 1; y += 2) {
      int jumps = 0;
      for (int x = 1; x <= 7; x++) {
        if (is_x_y_inrange(piece->x, piece->y, x, y)) {
          int is_any_piece_there = 0;
          for (int i = 0; i < 32; i++) {
            if (pieces[i].x == piece->x + x && pieces[i].y == piece->y + y) {
              is_any_piece_there = 1;
              if (pieces[i].color != piece->color) {
                moves[moves_count * 3] = piece->x + x;
                moves[(moves_count * 3) + 1] = piece->y + y;
                moves[(moves_count * 3) + 2] = 1;
                moves_count++;
              }
              jumps++;
            }
          }
          if (!is_any_piece_there) {
            moves[moves_count * 3] = piece->x + x;
            moves[(moves_count * 3) + 1] = piece->y + y;
            moves[(moves_count * 3) + 2] = 0;
            moves_count++;
          }
        }
        if (jumps > 0)
          break;
      }
      jumps = 0;
      for (int x = -1; x >= -7; x--) {
        if (is_x_y_inrange(piece->x, piece->y, x, y)) {
          int is_any_piece_there = 0;
          for (int i = 0; i < 32; i++) {
            if (pieces[i].x == piece->x + x && pieces[i].y == piece->y + y) {
              is_any_piece_there = 1;
              if (pieces[i].color != piece->color) {
                moves[moves_count * 3] = piece->x + x;
                moves[(moves_count * 3) + 1] = piece->y + y;
                moves[(moves_count * 3) + 2] = 1;
                moves_count++;
              }
              jumps++;
            }
          }
          if (!is_any_piece_there) {
            moves[moves_count * 3] = piece->x + x;
            moves[(moves_count * 3) + 1] = piece->y + y;
            moves[(moves_count * 3) + 2] = 0;
            moves_count++;
          }
        }
        if (jumps > 0)
          break;
      }
    }
  } else if (strcmp(piece->name, "dragon") == 0) {
    int moves_count = 0;
    for (int x = -1; x <= 1; x++)
      for (int y = -1; y <= 1; y++)
        if (is_x_y_inrange(piece->x, piece->y, x, y)) {
          int is_any_piece_there = 0;
          for (int i = 0; i < 32; i++) {
            if (pieces[i].x == piece->x + x && pieces[i].y == piece->y + y) {
              is_any_piece_there = 1;
              if (pieces[i].color != piece->color) {
                moves[moves_count * 3] = piece->x + x;
                moves[(moves_count * 3) + 1] = piece->y + y;
                moves[(moves_count * 3) + 2] = 1;
                moves_count++;
              }
            }
          }
          if (!is_any_piece_there) {
            moves[moves_count * 3] = piece->x + x;
            moves[(moves_count * 3) + 1] = piece->y + y;
            moves[(moves_count * 3) + 2] = 0;
            moves_count++;
          }
        }
    for (int x = -1; x <= 1; x += 2)
      for (int y = -2; y <= 2; y += 4)
        if (is_x_y_inrange(piece->x, piece->y, x, y)) {
          int is_any_piece_there = 0;
          for (int i = 0; i < 32; i++) {
            if (pieces[i].x == piece->x + x && pieces[i].y == piece->y + y) {
              is_any_piece_there = 1;
              if (pieces[i].color != piece->color) {
                moves[moves_count * 3] = piece->x + x;
                moves[(moves_count * 3) + 1] = piece->y + y;
                moves[(moves_count * 3) + 2] = 1;
                moves_count++;
              }
            }
          }
          if (!is_any_piece_there) {
            moves[moves_count * 3] = piece->x + x;
            moves[(moves_count * 3) + 1] = piece->y + y;
            moves[(moves_count * 3) + 2] = 0;
            moves_count++;
          }
        }
    for (int x = -2; x <= 2; x += 4)
      for (int y = -1; y <= 1; y += 2)
        if (is_x_y_inrange(piece->x, piece->y, x, y)) {
          int is_any_piece_there = 0;
          for (int i = 0; i < 32; i++) {
            if (pieces[i].x == piece->x + x && pieces[i].y == piece->y + y) {
              is_any_piece_there = 1;
              if (pieces[i].color != piece->color) {
                moves[moves_count * 3] = piece->x + x;
                moves[(moves_count * 3) + 1] = piece->y + y;
                moves[(moves_count * 3) + 2] = 1;
                moves_count++;
              }
            }
          }
          if (!is_any_piece_there) {
            moves[moves_count * 3] = piece->x + x;
            moves[(moves_count * 3) + 1] = piece->y + y;
            moves[(moves_count * 3) + 2] = 0;
            moves_count++;
          }
        }
  } else if (strcmp(piece->name, "thief") == 0) {
    int moves_count = 0;
    int jumps = 0;
    for (int x = 1; x <= 7; x++)
      if (is_x_y_inrange(piece->x, piece->y, x, x)) {
        int is_any_piece_there = 0;
        for (int i = 0; i < 32; i++) {
          if (pieces[i].x == piece->x + x && pieces[i].y == piece->y + x) {
            is_any_piece_there = 1;
            if (pieces[i].color != piece->color) {
              moves[moves_count * 3] = piece->x + x;
              moves[(moves_count * 3) + 1] = piece->y + x;
              moves[(moves_count * 3) + 2] = 1;
              moves_count++;
            }
            jumps++;
          }
        }
        if (!is_any_piece_there) {
          moves[moves_count * 3] = piece->x + x;
          moves[(moves_count * 3) + 1] = piece->y + x;
          moves[(moves_count * 3) + 2] = 0;
          moves_count++;
        }
        if (jumps > 1)
          break;
      }
    jumps = 0;
    for (int x = -1; x >= -7; x--)
      if (is_x_y_inrange(piece->x, piece->y, x, x)) {
        int is_any_piece_there = 0;
        for (int i = 0; i < 32; i++) {
          if (pieces[i].x == piece->x + x && pieces[i].y == piece->y + x) {
            is_any_piece_there = 1;
            if (pieces[i].color != piece->color) {
              moves[moves_count * 3] = piece->x + x;
              moves[(moves_count * 3) + 1] = piece->y + x;
              moves[(moves_count * 3) + 2] = 1;
              moves_count++;
            }
            jumps++;
          }
        }
        if (!is_any_piece_there) {
          moves[moves_count * 3] = piece->x + x;
          moves[(moves_count * 3) + 1] = piece->y + x;
          moves[(moves_count * 3) + 2] = 0;
          moves_count++;
        }
        if (jumps > 1)
          break;
      }
    jumps = 0;
    for (int x = 1; x <= 7; x++)
      if (is_x_y_inrange(piece->x, piece->y, x, -x)) {
        int is_any_piece_there = 0;
        for (int i = 0; i < 32; i++) {
          if (pieces[i].x == piece->x + x && pieces[i].y == piece->y - x) {
            is_any_piece_there = 1;
            if (pieces[i].color != piece->color) {
              moves[moves_count * 3] = piece->x + x;
              moves[(moves_count * 3) + 1] = piece->y - x;
              moves[(moves_count * 3) + 2] = 1;
              moves_count++;
            }
            jumps++;
          }
        }
        if (!is_any_piece_there) {
          moves[moves_count * 3] = piece->x + x;
          moves[(moves_count * 3) + 1] = piece->y - x;
          moves[(moves_count * 3) + 2] = 0;
          moves_count++;
        }
        if (jumps > 1)
          break;
      }
    jumps = 0;
    for (int x = -1; x >= -7; x--)
      if (is_x_y_inrange(piece->x, piece->y, x, -x)) {
        int is_any_piece_there = 0;
        for (int i = 0; i < 32; i++) {
          if (pieces[i].x == piece->x + x && pieces[i].y == piece->y - x) {
            is_any_piece_there = 1;
            if (pieces[i].color != piece->color) {
              moves[moves_count * 3] = piece->x + x;
              moves[(moves_count * 3) + 1] = piece->y - x;
              moves[(moves_count * 3) + 2] = 1;
              moves_count++;
            }
            jumps++;
          }
        }
        if (!is_any_piece_there) {
          moves[moves_count * 3] = piece->x + x;
          moves[(moves_count * 3) + 1] = piece->y - x;
          moves[(moves_count * 3) + 2] = 0;
          moves_count++;
        }
        if (jumps > 1)
          break;
      }
  } else if (strcmp(piece->name, "queen") == 0) {
    int moves_count = 0;
    int jumps = 0;
    for (int x = 1; x <= 7; x++)
      if (is_x_y_inrange(piece->x, piece->y, x, 0)) {
        int is_any_piece_there = 0;
        for (int i = 0; i < 32; i++) {
          if (pieces[i].x == piece->x + x && pieces[i].y == piece->y) {
            is_any_piece_there = 1;
            if (pieces[i].color != piece->color) {
              moves[moves_count * 3] = piece->x + x;
              moves[(moves_count * 3) + 1] = piece->y;
              moves[(moves_count * 3) + 2] = 1;
              moves_count++;
            }
            jumps++;
          }
        }
        if (!is_any_piece_there) {
          moves[moves_count * 3] = piece->x + x;
          moves[(moves_count * 3) + 1] = piece->y;
          moves[(moves_count * 3) + 2] = 0;
          moves_count++;
        }
        if (jumps > 0)
          break;
      }
    jumps = 0;
    for (int x = -1; x >= -7; x--)
      if (is_x_y_inrange(piece->x, piece->y, x, 0)) {
        int is_any_piece_there = 0;
        for (int i = 0; i < 32; i++) {
          if (pieces[i].x == piece->x + x && pieces[i].y == piece->y) {
            is_any_piece_there = 1;
            if (pieces[i].color != piece->color) {
              moves[moves_count * 3] = piece->x + x;
              moves[(moves_count * 3) + 1] = piece->y;
              moves[(moves_count * 3) + 2] = 1;
              moves_count++;
            }
            jumps++;
          }
        }
        if (!is_any_piece_there) {
          moves[moves_count * 3] = piece->x + x;
          moves[(moves_count * 3) + 1] = piece->y;
          moves[(moves_count * 3) + 2] = 0;
          moves_count++;
        }
        if (jumps > 0)
          break;
      }
    jumps = 0;
    for (int y = 1; y <= 7; y++)
      if (is_x_y_inrange(piece->x, piece->y, 0, y)) {
        int is_any_piece_there = 0;
        for (int i = 0; i < 32; i++) {
          if (pieces[i].x == piece->x && pieces[i].y == piece->y + y) {
            is_any_piece_there = 1;
            if (pieces[i].color != piece->color) {
              moves[moves_count * 3] = piece->x;
              moves[(moves_count * 3) + 1] = piece->y + y;
              moves[(moves_count * 3) + 2] = 1;
              moves_count++;
            }
            jumps++;
          }
        }
        if (!is_any_piece_there) {
          moves[moves_count * 3] = piece->x;
          moves[(moves_count * 3) + 1] = piece->y + y;
          moves[(moves_count * 3) + 2] = 0;
          moves_count++;
        }
        if (jumps > 0)
          break;
      }
    jumps = 0;
    for (int y = -1; y >= -7; y--)
      if (is_x_y_inrange(piece->x, piece->y, 0, y)) {
        int is_any_piece_there = 0;
        for (int i = 0; i < 32; i++) {
          if (pieces[i].x == piece->x && pieces[i].y == piece->y + y) {
            is_any_piece_there = 1;
            if (pieces[i].color != piece->color) {
              moves[moves_count * 3] = piece->x;
              moves[(moves_count * 3) + 1] = piece->y + y;
              moves[(moves_count * 3) + 2] = 1;
              moves_count++;
            }
            jumps++;
          }
        }
        if (!is_any_piece_there) {
          moves[moves_count * 3] = piece->x;
          moves[(moves_count * 3) + 1] = piece->y + y;
          moves[(moves_count * 3) + 2] = 0;
          moves_count++;
        }
        if (jumps > 0)
          break;
      }
    jumps = 0;
    for (int x = 1; x <= 7; x++)
      if (is_x_y_inrange(piece->x, piece->y, x, x)) {
        int is_any_piece_there = 0;
        for (int i = 0; i < 32; i++) {
          if (pieces[i].x == piece->x + x && pieces[i].y == piece->y + x) {
            is_any_piece_there = 1;
            if (pieces[i].color != piece->color) {
              moves[moves_count * 3] = piece->x + x;
              moves[(moves_count * 3) + 1] = piece->y + x;
              moves[(moves_count * 3) + 2] = 1;
              moves_count++;
            }
            jumps++;
          }
        }
        if (!is_any_piece_there) {
          moves[moves_count * 3] = piece->x + x;
          moves[(moves_count * 3) + 1] = piece->y + x;
          moves[(moves_count * 3) + 2] = 0;
          moves_count++;
        }
        if (jumps > 0)
          break;
      }
    jumps = 0;
    for (int x = -1; x >= -7; x--)
      if (is_x_y_inrange(piece->x, piece->y, x, x)) {
        int is_any_piece_there = 0;
        for (int i = 0; i < 32; i++) {
          if (pieces[i].x == piece->x + x && pieces[i].y == piece->y + x) {
            is_any_piece_there = 1;
            if (pieces[i].color != piece->color) {
              moves[moves_count * 3] = piece->x + x;
              moves[(moves_count * 3) + 1] = piece->y + x;
              moves[(moves_count * 3) + 2] = 1;
              moves_count++;
            }
            jumps++;
          }
        }
        if (!is_any_piece_there) {
          moves[moves_count * 3] = piece->x + x;
          moves[(moves_count * 3) + 1] = piece->y + x;
          moves[(moves_count * 3) + 2] = 0;
          moves_count++;
        }
        if (jumps > 0)
          break;
      }
    jumps = 0;
    for (int x = 1; x <= 7; x++)
      if (is_x_y_inrange(piece->x, piece->y, x, -x)) {
        int is_any_piece_there = 0;
        for (int i = 0; i < 32; i++) {
          if (pieces[i].x == piece->x + x && pieces[i].y == piece->y - x) {
            is_any_piece_there = 1;
            if (pieces[i].color != piece->color) {
              moves[moves_count * 3] = piece->x + x;
              moves[(moves_count * 3) + 1] = piece->y - x;
              moves[(moves_count * 3) + 2] = 1;
              moves_count++;
            }
            jumps++;
          }
        }
        if (!is_any_piece_there) {
          moves[moves_count * 3] = piece->x + x;
          moves[(moves_count * 3) + 1] = piece->y - x;
          moves[(moves_count * 3) + 2] = 0;
          moves_count++;
        }
        if (jumps > 0)
          break;
      }
    jumps = 0;
    for (int x = -1; x >= -7; x--)
      if (is_x_y_inrange(piece->x, piece->y, x, -x)) {
        int is_any_piece_there = 0;
        for (int i = 0; i < 32; i++) {
          if (pieces[i].x == piece->x + x && pieces[i].y == piece->y - x) {
            is_any_piece_there = 1;
            if (pieces[i].color != piece->color) {
              moves[moves_count * 3] = piece->x + x;
              moves[(moves_count * 3) + 1] = piece->y - x;
              moves[(moves_count * 3) + 2] = 1;
              moves_count++;
            }
            jumps++;
          }
        }
        if (!is_any_piece_there) {
          moves[moves_count * 3] = piece->x + x;
          moves[(moves_count * 3) + 1] = piece->y - x;
          moves[(moves_count * 3) + 2] = 0;
          moves_count++;
        }
        if (jumps > 0)
          break;
      }
    for (int x = -1; x <= 1; x += 2)
      for (int y = -2; y <= 2; y += 4)
        if (is_x_y_inrange(piece->x, piece->y, x, y)) {
          int is_any_piece_there = 0;
          for (int i = 0; i < 32; i++) {
            if (pieces[i].x == piece->x + x && pieces[i].y == piece->y + y) {
              is_any_piece_there = 1;
              if (pieces[i].color != piece->color) {
                moves[moves_count * 3] = piece->x + x;
                moves[(moves_count * 3) + 1] = piece->y + y;
                moves[(moves_count * 3) + 2] = 1;
                moves_count++;
              }
            }
          }
          if (!is_any_piece_there) {
            moves[moves_count * 3] = piece->x + x;
            moves[(moves_count * 3) + 1] = piece->y + y;
            moves[(moves_count * 3) + 2] = 0;
            moves_count++;
          }
        }
    for (int x = -2; x <= 2; x += 4)
      for (int y = -1; y <= 1; y += 2)
        if (is_x_y_inrange(piece->x, piece->y, x, y)) {
          int is_any_piece_there = 0;
          for (int i = 0; i < 32; i++) {
            if (pieces[i].x == piece->x + x && pieces[i].y == piece->y + y) {
              is_any_piece_there = 1;
              if (pieces[i].color != piece->color) {
                moves[moves_count * 3] = piece->x + x;
                moves[(moves_count * 3) + 1] = piece->y + y;
                moves[(moves_count * 3) + 2] = 1;
                moves_count++;
              }
            }
          }
          if (!is_any_piece_there) {
            moves[moves_count * 3] = piece->x + x;
            moves[(moves_count * 3) + 1] = piece->y + y;
            moves[(moves_count * 3) + 2] = 0;
            moves_count++;
          }
        }
  } else if (strcmp(piece->name, "king") == 0) {
    int moves_count = 0;
    int jumps = 0;
    for (int x = 1; x <= 2; x++)
      if (is_x_y_inrange(piece->x, piece->y, x, 0)) {
        int is_any_piece_there = 0;
        for (int i = 0; i < 32; i++) {
          if (pieces[i].x == piece->x + x && pieces[i].y == piece->y) {
            is_any_piece_there = 1;
            if (pieces[i].color != piece->color) {
              moves[moves_count * 3] = piece->x + x;
              moves[(moves_count * 3) + 1] = piece->y;
              moves[(moves_count * 3) + 2] = 1;
              moves_count++;
            }
            jumps++;
          }
        }
        if (!is_any_piece_there) {
          moves[moves_count * 3] = piece->x + x;
          moves[(moves_count * 3) + 1] = piece->y;
          moves[(moves_count * 3) + 2] = 0;
          moves_count++;
        }
        if (jumps > 0)
          break;
      }
    jumps = 0;
    for (int x = -1; x >= -2; x--)
      if (is_x_y_inrange(piece->x, piece->y, x, 0)) {
        int is_any_piece_there = 0;
        for (int i = 0; i < 32; i++) {
          if (pieces[i].x == piece->x + x && pieces[i].y == piece->y) {
            is_any_piece_there = 1;
            if (pieces[i].color != piece->color) {
              moves[moves_count * 3] = piece->x + x;
              moves[(moves_count * 3) + 1] = piece->y;
              moves[(moves_count * 3) + 2] = 1;
              moves_count++;
            }
            jumps++;
          }
        }
        if (!is_any_piece_there) {
          moves[moves_count * 3] = piece->x + x;
          moves[(moves_count * 3) + 1] = piece->y;
          moves[(moves_count * 3) + 2] = 0;
          moves_count++;
        }
        if (jumps > 0)
          break;
      }
    jumps = 0;
    for (int y = 1; y <= 2; y++)
      if (is_x_y_inrange(piece->x, piece->y, 0, y)) {
        int is_any_piece_there = 0;
        for (int i = 0; i < 32; i++) {
          if (pieces[i].x == piece->x && pieces[i].y == piece->y + y) {
            is_any_piece_there = 1;
            if (pieces[i].color != piece->color) {
              moves[moves_count * 3] = piece->x;
              moves[(moves_count * 3) + 1] = piece->y + y;
              moves[(moves_count * 3) + 2] = 1;
              moves_count++;
            }
            jumps++;
          }
        }
        if (!is_any_piece_there) {
          moves[moves_count * 3] = piece->x;
          moves[(moves_count * 3) + 1] = piece->y + y;
          moves[(moves_count * 3) + 2] = 0;
          moves_count++;
        }
        if (jumps > 0)
          break;
      }
    jumps = 0;
    for (int y = -1; y >= -2; y--)
      if (is_x_y_inrange(piece->x, piece->y, 0, y)) {
        int is_any_piece_there = 0;
        for (int i = 0; i < 32; i++) {
          if (pieces[i].x == piece->x && pieces[i].y == piece->y + y) {
            is_any_piece_there = 1;
            if (pieces[i].color != piece->color) {
              moves[moves_count * 3] = piece->x;
              moves[(moves_count * 3) + 1] = piece->y + y;
              moves[(moves_count * 3) + 2] = 1;
              moves_count++;
            }
            jumps++;
          }
        }
        if (!is_any_piece_there) {
          moves[moves_count * 3] = piece->x;
          moves[(moves_count * 3) + 1] = piece->y + y;
          moves[(moves_count * 3) + 2] = 0;
          moves_count++;
        }
        if (jumps > 0)
          break;
      }
    jumps = 0;
    for (int x = 1; x <= 2; x++)
      if (is_x_y_inrange(piece->x, piece->y, x, x)) {
        int is_any_piece_there = 0;
        for (int i = 0; i < 32; i++) {
          if (pieces[i].x == piece->x + x && pieces[i].y == piece->y + x) {
            is_any_piece_there = 1;
            if (pieces[i].color != piece->color) {
              moves[moves_count * 3] = piece->x + x;
              moves[(moves_count * 3) + 1] = piece->y + x;
              moves[(moves_count * 3) + 2] = 1;
              moves_count++;
            }
            jumps++;
          }
        }
        if (!is_any_piece_there) {
          moves[moves_count * 3] = piece->x + x;
          moves[(moves_count * 3) + 1] = piece->y + x;
          moves[(moves_count * 3) + 2] = 0;
          moves_count++;
        }
        if (jumps > 0)
          break;
      }
    jumps = 0;
    for (int x = -1; x >= -2; x--)
      if (is_x_y_inrange(piece->x, piece->y, x, x)) {
        int is_any_piece_there = 0;
        for (int i = 0; i < 32; i++) {
          if (pieces[i].x == piece->x + x && pieces[i].y == piece->y + x) {
            is_any_piece_there = 1;
            if (pieces[i].color != piece->color) {
              moves[moves_count * 3] = piece->x + x;
              moves[(moves_count * 3) + 1] = piece->y + x;
              moves[(moves_count * 3) + 2] = 1;
              moves_count++;
            }
            jumps++;
          }
        }
        if (!is_any_piece_there) {
          moves[moves_count * 3] = piece->x + x;
          moves[(moves_count * 3) + 1] = piece->y + x;
          moves[(moves_count * 3) + 2] = 0;
          moves_count++;
        }
        if (jumps > 0)
          break;
      }
    jumps = 0;
    for (int x = 1; x <= 2; x++)
      if (is_x_y_inrange(piece->x, piece->y, x, -x)) {
        int is_any_piece_there = 0;
        for (int i = 0; i < 32; i++) {
          if (pieces[i].x == piece->x + x && pieces[i].y == piece->y - x) {
            is_any_piece_there = 1;
            if (pieces[i].color != piece->color) {
              moves[moves_count * 3] = piece->x + x;
              moves[(moves_count * 3) + 1] = piece->y - x;
              moves[(moves_count * 3) + 2] = 1;
              moves_count++;
            }
            jumps++;
          }
        }
        if (!is_any_piece_there) {
          moves[moves_count * 3] = piece->x + x;
          moves[(moves_count * 3) + 1] = piece->y - x;
          moves[(moves_count * 3) + 2] = 0;
          moves_count++;
        }
        if (jumps > 0)
          break;
      }
    jumps = 0;
    for (int x = -1; x >= -2; x--)
      if (is_x_y_inrange(piece->x, piece->y, x, -x)) {
        int is_any_piece_there = 0;
        for (int i = 0; i < 32; i++) {
          if (pieces[i].x == piece->x + x && pieces[i].y == piece->y - x) {
            is_any_piece_there = 1;
            if (pieces[i].color != piece->color) {
              moves[moves_count * 3] = piece->x + x;
              moves[(moves_count * 3) + 1] = piece->y - x;
              moves[(moves_count * 3) + 2] = 1;
              moves_count++;
            }
            jumps++;
          }
        }
        if (!is_any_piece_there) {
          moves[moves_count * 3] = piece->x + x;
          moves[(moves_count * 3) + 1] = piece->y - x;
          moves[(moves_count * 3) + 2] = 0;
          moves_count++;
        }
        if (jumps > 0)
          break;
      }
  }
}
void get_regular_moves(uint8_t *moves, Piece *piece) {
  uint8_t *raw_moves = malloc(sizeof(uint8_t) * 64 * 3);
  for (int i = 0; i < 63; i++)
    moves[i] = 8;
  int moves_count = 0;
  get_moves(raw_moves, piece);
  for (int i = 0; raw_moves[i] < 8; i += 3) {
    if (!simulate_move_and_check(piece, raw_moves[i], raw_moves[i + 1])) {
      moves[moves_count * 3] = raw_moves[i];
      moves[moves_count * 3 + 1] = raw_moves[i + 1];
      moves[moves_count * 3 + 2] = raw_moves[i + 2];
      moves_count++;
    }
  }
  free(raw_moves);
}
int is_x_y_inrange(int px, int py, int x, int y) {
  return (x != 0 || y != 0) && px + x >= 0 && px + x < 8 && py + y >= 0 &&
         py + y < 8;
}
Piece *get_removed_pieces(char color) { return NULL; }
int move(Piece *piece, int x, int y) {
  if (piece->color != turn)
    return 2;
  uint8_t *avaible_moves = malloc(3 * 64 * sizeof(uint8_t));
  memset(avaible_moves, 8, 3 * 64 * sizeof(uint8_t));
  get_regular_moves(avaible_moves, piece);
  int is_any_moves = 0;
  for (int i = 0; avaible_moves[i] != 8; i += 3) {
    if (avaible_moves[i] == x && avaible_moves[i + 1] == y) {
      is_any_moves = 1;
      if (avaible_moves[i + 2] == 1) {
        for (int j = 0; j < 32; j++) {
          if (pieces[j].x == avaible_moves[i] &&
              pieces[j].y == avaible_moves[i + 1]) {
            pieces[j].is_removed = 1;
            pieces[j].x = 8;
            pieces[j].y = 8;
            break;
          }
        }
      }
      piece->x = avaible_moves[i];
      piece->y = avaible_moves[i + 1];
      break;
    }
  }
  free(avaible_moves);
  if (!is_any_moves)
    return 1;
  piece->moves_count++;

  for (int i = 0; i < 32; i++) {
    if (pieces[i].color == 'b' && strcmp(pieces[i].name, "king") == 0) {
      pieces[i].is_checked = is_check('b');
    }
  }
  for (int i = 0; i < 32; i++) {
    if (pieces[i].color == 'w' && strcmp(pieces[i].name, "king") == 0) {
      pieces[i].is_checked = is_check('w');
    }
  }
  int is_b_checkmate = 1;
  int is_w_checkmate = 1;
  uint8_t *moves = malloc(64 * 3 * sizeof(uint8_t));
  for (int i = 0; i < 32; i++) {
    if (pieces[i].color == 'b' && pieces[i].is_removed == 0) {
      get_regular_moves(moves, &pieces[i]);
      if (moves[0] != 8) {
        is_b_checkmate = 0;
      }
    } else if (pieces[i].color == 'w' && pieces[i].is_removed == 0) {
      get_regular_moves(moves, &pieces[i]);
      if (moves[0] != 8) {
        is_w_checkmate = 0;
      }
    }
  }
  if (is_b_checkmate) {
    winner = 'w';
    set_program_status("ended");
  } else if (is_w_checkmate) {
    winner = 'b';
    set_program_status("ended");
  }
  return 0;
}
// 0 = ok | 1 = invalid move | 2 = not ur turn
void set_program_status(const char *status) { strcpy(program_status, status); }
char *get_program_status() { return program_status; }
int is_check(char color) {
  Piece king;
  int is_checked = 0;
  for (int i = 0; i < 32; i++) {
    if (pieces[i].color == color && strcmp(pieces[i].name, "king") == 0) {
      king = pieces[i];
    }
  }
  for (int i = 0; i < 32; i++) {
    if (pieces[i].color != color && !pieces[i].is_removed) {
      uint8_t *moves = malloc(sizeof(uint8_t) * 64 * 3);
      get_moves(moves, &pieces[i]);
      for (int j = 0; j <= 64 * 3 && moves[j] < 8; j += 3) {
        if (moves[j] == king.x && moves[j + 1] == king.y) {
          is_checked = 1;
          break;
        }
      }
      free(moves);
    }
    if (is_checked)
      break;
  }

  return is_checked;
}
int simulate_move_and_check(Piece *piece, int x, int y) {
  int is_piece_removed = 0;
  Piece *removed_piece;
  for (int i = 0; i < 32; i++) {
    if (!pieces[i].is_removed && pieces[i].x == x && pieces[i].y == y &&
        pieces[i].color != piece->color) {
      removed_piece = &pieces[i];
      is_piece_removed = 1;
      pieces[i].is_removed = 1;
    }
  }
  int fx = piece->x, fy = piece->y;
  piece->x = x;
  piece->y = y;
  int ic = is_check(piece->color);
  piece->x = fx;
  piece->y = fy;
  if (is_piece_removed) {
    removed_piece->is_removed = 0;
  }
  return ic;
}
char get_winner() { return winner; }