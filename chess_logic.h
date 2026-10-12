#include <SDL3/SDL.h>
#include <SDL3_ttf/SDL_ttf.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
  char name[50];
  SDL_Surface *img;
  int x;
  int y;
  char color;
  int moves_count;
  int is_removed;
  int is_selected;
  int is_dragging;
  int is_checked;
} Piece;

void init_pieces();
Piece *get_pieces();
Piece *get_removed_pieces(char color);
void get_moves(uint8_t *moves, Piece *piece);
int move(Piece *piece, int x, int y);
void set_program_status(const char *status);
char *get_program_status();
void second_passed();
char *get_timer_text(char color);
char get_turn();
void set_turn(char t);
int is_check(char color);
int simulate_move_and_check(Piece *piece, int x, int y);
void get_regular_moves(uint8_t *moves, Piece *piece);
char get_winner();
