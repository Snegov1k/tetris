#include "../../brick_game/tetris/tetris.h"

void init_ncurses() {
  initscr();
  cbreak();
  noecho();
  nodelay(stdscr, TRUE);
  scrollok(stdscr, TRUE);
  curs_set(0);
  keypad(stdscr, TRUE);
}

void print_game(GameInfo_t *game) {
  clear();

  print_field(game);
  print_high_score(game);
  print_score(game);
  print_level(game);
  print_next_field(game);
  print_menu(game);

  refresh();
  napms(game->speed);
}

void print_field(GameInfo_t *game) {
  for (int row = 0; row < FIELD_HEIGHT; row++) {
    for (int column = 0; column < FIELD_WIDTH; column++) {
      printw("%c", game->field[row][column]);
    }
    printw("\n");
  }
}

void print_high_score(GameInfo_t *game) {
  int caption_x = 13;
  int caption_y = 1;
  mvprintw(caption_y, caption_x, "HIGH SCORE\n");

  int number_x = calculate_number_x(game->high_score);
  int number_y = 2;
  mvprintw(number_y, number_x, "%d\n", game->high_score);
}

void print_score(GameInfo_t *game) {
  int caption_x = 15;
  int caption_y = 4;
  mvprintw(caption_y, caption_x, "SCORE\n");

  int number_x = calculate_number_x(game->score);
  int number_y = 5;
  mvprintw(number_y, number_x, "%d\n", game->score);
}

int calculate_number_x(int number) {
  int number_x = 17;

  if (number >= 10000) {
    number_x = 15;
  } else if (number >= 100) {
    number_x = 16;
  }

  return number_x;
}

void print_level(GameInfo_t *game) {
  int caption_x = 15;
  int caption_y = 7;
  mvprintw(caption_y, caption_x, "LEVEL\n");

  int number_x = (game->level == 10) ? 16 : 17;
  int number_y = 8;
  mvprintw(number_y, number_x, "%d\n", game->level);
}

void print_next_field(GameInfo_t *game) {
  int caption_x = 16;
  int caption_y = 10;
  mvprintw(caption_y, caption_x, "NEXT\n");

  for (int row = 0; row < NEXT_HEIGHT; row++) {
    for (int column = 0; column < NEXT_WIDTH; column++) {
      mvprintw((NEXT_FIRST_V_BORDER + row), (NEXT_FIRST_H_BORDER + column),
               "%c", (char)game->next[row][column]);
    }
    printw("\n");
  }
}

void print_menu() {
  int menu_x = 0;
  int menu_y = FIELD_HEIGHT + 1;
  mvprintw(menu_y, menu_x, "press: p - pause, s - start, q - quit\n");
}

void set_high_score_in_game(GameInfo_t *game) {
  const char *filename = "brick_game/tetris/high_score.txt";
  FILE *high_score_file = fopen(filename, "r+");

  if (high_score_file == NULL) {
    high_score_file = fopen(filename, "w+");
  }

  game->high_score = get_score_from_file(high_score_file);

  fclose(high_score_file);
}

int get_score_from_file(FILE *high_score_file) {
  char score[10];

  if (file_is_empty(high_score_file)) {
    fprintf(high_score_file, "0");
    fflush(high_score_file);
    rewind(high_score_file);
  }

  fgets(score, sizeof(score), high_score_file);

  return atoi(score);
}

bool file_is_empty(FILE *file) {
  fseek(file, 0, SEEK_END);
  long size = ftell(file);
  rewind(file);

  return (size == 0);
}

void display_initial_screen(GameInfo_t *game) {
  clear();

  int start_row = (LINES - 6) / 2;
  for (int i = 0; i < 7; i++) {
    print_centered(stdscr, start_row + i, initial_screen[i]);
  }

  refresh();

  if (getchar() == 'q') {
    game->exit = 1;
  }
}

void display_game_over() {
  clear();

  int start_row = (LINES - 6) / 2;
  for (int i = 0; i < 6; i++) {
    print_centered(stdscr, start_row + i, game_over[i]);
  }

  refresh();
  napms(2000);
}

void print_centered(WINDOW *win, int row, const char *str) {
  int length = strlen(str);
  int col = (COLS - length) / 2;

  mvwprintw(win, row, col, "%s", str);
}
