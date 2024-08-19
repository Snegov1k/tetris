#ifndef TETRIS_H
#define TETRIS_H

#include <ncurses.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define FIELD_WIDTH 10
#define FIELD_HEIGHT 20
#define HORISONTAL_FIELD_MIDDLE (FIELD_WIDTH / 2 - 2)

#define NEXT_WIDTH 6
#define NEXT_HEIGHT 6
#define NEXT_FIRST_H_BORDER (FIELD_WIDTH + 5)
#define NEXT_FIRST_V_BORDER 11

#define SHAPE_SIZE 4

#define H_LINE 61
#define V_LINE 124
#define SPACE 32

#define PLACE true
#define REMOVE false

#define RIGHT 0
#define LEFT 1
#define DOWN 2

#define LINES_NUMBER (FIELD_HEIGHT - 2)

#define LEN(array) (sizeof(array) / sizeof(array[0]))

typedef struct {
  int x;
  int y;
  int shape[4][4];
} Figure_t;
extern const int figures[7][4][4];

typedef enum {
  Start,
  Pause,
  Terminate,
  Left,
  Right,
  Down,
  Action
} UserAction_t;

typedef struct {
  int **field;
  int **next;
  Figure_t figure;
  Figure_t next_figure;
  int score;
  int high_score;
  int level;
  int speed;
  int pause;
  int over;
  int exit;
} GameInfo_t;

// ------------------------------------------------------------LOGIC------------------------------------------------------------
void tetris();
void prepare(GameInfo_t *game);
void start(GameInfo_t *game);
void finish(GameInfo_t *game);
void reset(GameInfo_t *game);
int **create_field(int height, int width);
int **allocate_field(int height, int width);
void fill_field(int **field, int height, int width);
void put_specific_symbol_in_field(int **field, int row, int column, int height,
                                  int width);
void free_field(int **field, int height);
void updateCurrentState(GameInfo_t *game);
void generate_random_figure(Figure_t *figure);
void set_random_shape_to(Figure_t *figure);
void display_next_figure(GameInfo_t *game);
void userInput(UserAction_t *action);
void handle_user_input(GameInfo_t *game, UserAction_t action);
void handle_user_action(GameInfo_t *game, UserAction_t action);
void move_figure_left(GameInfo_t *game);
void move_figure_right(GameInfo_t *game);
void move_figure_down(GameInfo_t *game);
void rotate_figure(GameInfo_t *game);
void fill_and_rotate_clockwise(GameInfo_t *game,
                               int temp[SHAPE_SIZE][SHAPE_SIZE]);
bool can_place(GameInfo_t *game, int rotated_shape[SHAPE_SIZE][SHAPE_SIZE]);
void gravity(GameInfo_t *game);
void drop_next_figure(GameInfo_t *game);
bool can_move(GameInfo_t game, int direction);
void make_phantom_figure(GameInfo_t *game, int direction);
void check_space_around(GameInfo_t game, bool *is_free, int i, int j);
void remove_completed_lines(GameInfo_t *game);
void reset_lines_array(int lines[]);
void find_completed_line(GameInfo_t *game, int lines[]);
bool line_is_full(GameInfo_t *game, int row);
bool lines_were_found(int lines[]);
void clear_founded_lines(GameInfo_t *game, int lines[]);
void clear_line(GameInfo_t *game, int line);
void shift_blocks_down(GameInfo_t *game);
bool line_is_empty(GameInfo_t *game, int row);
int find_first_non_empty_line_above(GameInfo_t *game, int start_row);
void copy_line(GameInfo_t *game, int dest_row, int src_row);
void update_score(GameInfo_t *game, int lines[]);
int count_filled_lines(int lines[]);
bool new_high_score(GameInfo_t *game);
void save_high_score(GameInfo_t *game);
void update_level(GameInfo_t *game);
void remove_figure(GameInfo_t *game);
void remove_elem(GameInfo_t *game, int i, int j);
void place_figure(GameInfo_t *game);
void place_elem(GameInfo_t *game, int i, int j);
void check_game_over(GameInfo_t *game);

// ------------------------------------------------------------CLI------------------------------------------------------------
void init_ncurses();
void print_game(GameInfo_t *game);
void print_field(GameInfo_t *game);
void print_high_score(GameInfo_t *game);
void print_score(GameInfo_t *game);
int calculate_number_x(int number);
void print_level(GameInfo_t *game);
void print_next_field(GameInfo_t *game);
void print_menu();
void set_high_score_in_game(GameInfo_t *game);
int get_score_from_file(FILE *high_score_file);
bool file_is_empty(FILE *file);
void display_initial_screen(GameInfo_t *game);
extern const char *initial_screen[];
void display_game_over();
extern const char *game_over[];
void print_centered(WINDOW *win, int row, const char *str);

#endif  // TETRIS_H
