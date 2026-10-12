
#include "GUI/gui.h"
#include "GUI/gui_logic.h"
#include "chess_logic.h"
#include <SDL3/SDL.h>
#include <SDL3_ttf/SDL_ttf.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>

int main(void) {
  int win_width = 1200;
  int win_heigth = 700;
  int btn_start =
      add_button("start", "شروع", (win_width - 200) / 2, 200, 200, 50, 25);
  int btn_exit =
      add_button("exit", "خروج", (win_width - 200) / 2, 270, 200, 50, 25);
  int btn_restart = add_button("restart", "شروع دوباره", (win_width - 200) / 2,
                               270, 200, 50, 25);
  int btn_reexit =
      add_button("reexit", "خروج", (win_width - 200) / 2, 340, 200, 50, 25);
  run_gui("Chess", win_width, win_heigth, 180);
  return 0;
}

void button_clicked(const char *button_name) {
  if (strcmp(get_program_status(), "main_menu") == 0) {
    if (strcmp(button_name, "start") == 0) {
      init_pieces();
      reset_timer();
      set_program_status("playing");
    } else if (strcmp(button_name, "exit") == 0) {
      stop_gui();
    }
  } else if (strcmp(get_program_status(), "ended") == 0) {
    if (strcmp(button_name, "restart") == 0) {
      init_pieces();
      reset_timer();
      set_program_status("playing");
    } else if (strcmp(button_name, "reexit") == 0) {
      stop_gui();
    }
  }
}
