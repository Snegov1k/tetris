#include "tetris.h"

void tetris() {
  GameInfo_t game;

  prepare(&game);
  start(&game);
  finish(&game);
}

void prepare(GameInfo_t *game) {
  reset(game);
  init_ncurses();

  game->field = create_field(FIELD_HEIGHT, FIELD_WIDTH);
  game->next = create_field(NEXT_HEIGHT, NEXT_WIDTH);

  generate_random_figure(&game->figure);
  generate_random_figure(&game->next_figure);
  display_next_figure(game);

  set_high_score_in_game(game);

  display_initial_screen(game);
}

void start(GameInfo_t *game) {
  while (!game->exit) {
    updateCurrentState(game);
    print_game(game);
  }
}

void finish(GameInfo_t *game) {
  if (game->over) {
    display_game_over();
  }

  free_field(game->field, FIELD_HEIGHT);
  free_field(game->next, NEXT_HEIGHT);
  endwin();
}

void reset(GameInfo_t *game) {
  game->field = NULL;
  game->next = NULL;
  game->score = 0;
  game->high_score = 0;
  game->level = 1;
  game->speed = 275;
  game->pause = 0;
  game->over = 0;
  game->exit = 0;
}

int **create_field(int height, int width) {
  int **field = allocate_field(height, width);
  fill_field(field, height, width);

  return field;
}

int **allocate_field(int height, int width) {
  int **field = (int **)malloc(height * sizeof(int *));

  if (field != NULL) {
    for (int i = 0; i < height; i++) {
      field[i] = (int *)malloc(width * sizeof(int));
    }
  }

  return field;
}

void fill_field(int **field, int height, int width) {
  for (int row = 0; row < height; row++) {
    for (int column = 0; column < width; column++) {
      put_specific_symbol_in_field(field, row, column, height, width);
    }
  }
}

void put_specific_symbol_in_field(int **field, int row, int column, int height,
                                  int width) {
  bool is_top_row = (row == 0);
  bool is_bottom_row = (row == height - 1);
  bool is_left_column = (column == 0);
  bool is_right_column = (column == width - 1);

  if (is_top_row || is_bottom_row) {
    field[row][column] = H_LINE;
  } else if (is_left_column || is_right_column) {
    field[row][column] = V_LINE;
  } else {
    field[row][column] = SPACE;
  }
}

void free_field(int **field, int height) {
  for (int i = 0; i < height; i++) {
    free(field[i]);
  }

  free(field);
}

void updateCurrentState(GameInfo_t *game) {
  UserAction_t action;

  userInput(&action);
  handle_user_input(game, action);
  gravity(game);
  check_game_over(game);
}

void generate_random_figure(Figure_t *figure) {
  figure->x = HORISONTAL_FIELD_MIDDLE;
  figure->y = 1;
  set_random_shape_to(figure);
}

void set_random_shape_to(Figure_t *figure) {
  int type = clock() % 7;
  memcpy(figure->shape, figures[type], sizeof(figures[type]));
}

void display_next_figure(GameInfo_t *game) {
  int(*shape)[SHAPE_SIZE] = game->next_figure.shape;
  int start_x = (NEXT_WIDTH - SHAPE_SIZE) / 2;
  int start_y = (NEXT_HEIGHT - SHAPE_SIZE) / 2;

  for (int i = 0; i < SHAPE_SIZE; i++) {
    for (int j = 0; j < SHAPE_SIZE; j++) {
      game->next[start_y + i][start_x + j] = shape[i][j];
    }
  }
}

void userInput(UserAction_t *action) {
  switch (getch()) {
    case 's':
    case 'S':
      *action = Start;
      break;
    case 'p':
    case 'P':
      *action = Pause;
      break;
    case 'q':
    case 'Q':
      *action = Terminate;
      break;
    case KEY_LEFT:
      *action = Left;
      break;
    case KEY_RIGHT:
      *action = Right;
      break;
    case KEY_DOWN:
      *action = Down;
      break;
    case KEY_UP:
      *action = Action;
      break;

    default:
      *action = -1;
  }
}

void handle_user_input(GameInfo_t *game, UserAction_t action) {
  if (action == (UserAction_t)-1) return;

  remove_figure(game);
  handle_user_action(game, action);
  place_figure(game);
}

void handle_user_action(GameInfo_t *game, UserAction_t action) {
  switch (action) {
    case Start:
      game->pause = 0;
      break;
    case Pause:
      game->pause = 1;
      break;
    case Terminate:
      game->exit = 1;
      break;
    case Left:
      move_figure_left(game);
      break;
    case Right:
      move_figure_right(game);
      break;
    case Down:
      move_figure_down(game);
      break;
    case Action:
      rotate_figure(game);
      break;
  }
}

void move_figure_left(GameInfo_t *game) {
  if (game->pause) return;

  game->figure.x -= can_move(*game, LEFT) ? 1 : 0;
}

void move_figure_right(GameInfo_t *game) {
  if (game->pause) return;

  game->figure.x += can_move(*game, RIGHT) ? 1 : 0;
}

void move_figure_down(GameInfo_t *game) {
  if (game->pause) return;

  while (can_move(*game, DOWN)) {
    remove_figure(game);
    game->figure.y += 1;
    place_figure(game);
  }
}

void rotate_figure(GameInfo_t *game) {
  if (game->pause) return;

  int temp[SHAPE_SIZE][SHAPE_SIZE];
  fill_and_rotate_clockwise(game, temp);

  if (can_place(game, temp)) {
    memcpy(game->figure.shape, temp, sizeof(temp));
  }
}

void fill_and_rotate_clockwise(GameInfo_t *game,
                               int temp[SHAPE_SIZE][SHAPE_SIZE]) {
  for (int i = 0; i < SHAPE_SIZE; i++) {
    for (int j = 0; j < SHAPE_SIZE; j++) {
      temp[j][SHAPE_SIZE - i - 1] = game->figure.shape[i][j];
    }
  }
}

bool can_place(GameInfo_t *game, int rotated_shape[SHAPE_SIZE][SHAPE_SIZE]) {
  bool ability = true;

  for (int i = 0; i < SHAPE_SIZE; i++) {
    for (int j = 0; j < SHAPE_SIZE; j++) {
      if (rotated_shape[i][j] != SPACE) {
        int new_x = game->figure.x + j;
        int new_y = game->figure.y + i;

        if ((new_x < 0 || new_x >= FIELD_WIDTH - 1) ||
            (new_y < 0 || new_y >= FIELD_HEIGHT - 1) ||
            game->field[new_y][new_x] != SPACE) {
          ability = false;
        }
      }
    }
  }

  return ability;
}

void gravity(GameInfo_t *game) {
  if (game->pause) return;

  if (can_move(*game, DOWN)) {
    remove_figure(game);
    game->figure.y += 1;
    place_figure(game);
  } else {
    place_figure(game);
    remove_completed_lines(game);
    drop_next_figure(game);
  }
}

void drop_next_figure(GameInfo_t *game) {
  game->figure = game->next_figure;
  generate_random_figure(&game->next_figure);
  display_next_figure(game);
}

bool can_move(GameInfo_t game, int direction) {
  bool is_free = true;
  int(*shape)[SHAPE_SIZE] = game.figure.shape;

  make_phantom_figure(&game, direction);

  for (int i = 0; i < SHAPE_SIZE; i++) {
    for (int j = 0; j < SHAPE_SIZE; j++) {
      if (shape[i][j] != SPACE) {
        check_space_around(game, &is_free, i, j);
      }
    }
  }

  return is_free;
}

void make_phantom_figure(GameInfo_t *game, int direction) {
  remove_figure(game);

  if (direction == RIGHT) {
    game->figure.x += 1;
  } else if (direction == LEFT) {
    game->figure.x -= 1;
  } else if (direction == DOWN) {
    game->figure.y += 1;
  }
}

void check_space_around(GameInfo_t game, bool *is_free, int i, int j) {
  int new_y = game.figure.y + i;
  int new_x = game.figure.x + j;

  if (game.field[new_y][new_x] != SPACE) {
    *is_free = false;
  }
}

void remove_completed_lines(GameInfo_t *game) {
  int lines[LINES_NUMBER];
  reset_lines_array(lines);

  find_completed_line(game, lines);

  if (lines_were_found(lines)) {
    clear_founded_lines(game, lines);
    shift_blocks_down(game);

    update_score(game, lines);
    update_level(game);
  }
}

void reset_lines_array(int lines[]) {
  for (int i = 0; i < LINES_NUMBER; i++) {
    lines[i] = -1;
  }
}

void find_completed_line(GameInfo_t *game, int lines[]) {
  int index = 0;

  for (int row = 1; row < FIELD_HEIGHT - 1; row++) {
    if (line_is_full(game, row)) {
      lines[index++] = row;
    }
  }
}

bool line_is_full(GameInfo_t *game, int row) {
  int borders = 2;
  int count = 0;

  for (int column = 1; column < FIELD_WIDTH - 1; column++) {
    count += game->field[row][column] != SPACE ? 1 : 0;
  }

  return (count == (FIELD_WIDTH - borders));
}

bool lines_were_found(int lines[]) {
  bool lines_found = false;

  for (int i = 0; i < LINES_NUMBER; i++) {
    if (lines[i] != -1) {
      lines_found = true;
    }
  }

  return lines_found;
}

void clear_founded_lines(GameInfo_t *game, int lines[]) {
  for (int index = 0; index < LINES_NUMBER; index++) {
    int line = lines[index];
    clear_line(game, line);
  }
}

void clear_line(GameInfo_t *game, int line) {
  if (line == -1) return;

  for (int column = 1; column < FIELD_WIDTH - 1; column++) {
    game->field[line][column] = SPACE;
  }
}

void shift_blocks_down(GameInfo_t *game) {
  for (int row = FIELD_HEIGHT - 2; row > 0; row--) {
    if (line_is_empty(game, row)) {
      int non_empty_line = find_first_non_empty_line_above(game, row);

      if (non_empty_line != -1) {
        copy_line(game, row, non_empty_line);
        clear_line(game, non_empty_line);
      }
    }
  }
}

bool line_is_empty(GameInfo_t *game, int row) {
  int count = 0;

  for (int column = 1; column < FIELD_WIDTH - 1; column++) {
    count += game->field[row][column] != SPACE ? 1 : 0;
  }

  return (count == 0);
}

int find_first_non_empty_line_above(GameInfo_t *game, int start_row) {
  int found_row = -1;

  for (int row = start_row - 1; row > 0; row--) {
    if (!line_is_empty(game, row)) {
      found_row = row;
      break;
    }
  }

  return found_row;
}

void copy_line(GameInfo_t *game, int dest_row, int src_row) {
  for (int column = 1; column < FIELD_WIDTH - 1; column++) {
    game->field[dest_row][column] = game->field[src_row][column];
  }
}

void update_score(GameInfo_t *game, int lines[]) {
  switch (count_filled_lines(lines)) {
    case 1:
      game->score += 100;
      break;
    case 2:
      game->score += 300;
      break;
    case 3:
      game->score += 700;
      break;
    case 4:
      game->score += 1500;
      break;
  }

  if (new_high_score(game)) {
    game->high_score = game->score;
    save_high_score(game);
  }
}

int count_filled_lines(int lines[]) {
  int count = 0;

  for (int i = 0; i < LINES_NUMBER; i++) {
    if (lines[i] != -1) {
      count++;
    }
  }

  return count;
}

bool new_high_score(GameInfo_t *game) {
  return (game->score > game->high_score);
}

void save_high_score(GameInfo_t *game) {
  const char *filename = "brick_game/tetris/high_score.txt";
  FILE *high_score_file = fopen(filename, "w");

  if (high_score_file != NULL) {
    fprintf(high_score_file, "%d", game->high_score);
    fclose(high_score_file);
  }
}

void update_level(GameInfo_t *game) {
  int score = game->score;

  if (score >= 6000) {
    game->level = 10;
    game->speed = 50;
  } else if (score >= 5400) {
    game->level = 9;
    game->speed = 75;
  } else if (score >= 4800) {
    game->level = 8;
    game->speed = 100;
  } else if (score >= 4200) {
    game->level = 7;
    game->speed = 125;
  } else if (score >= 3600) {
    game->level = 6;
    game->speed = 150;
  } else if (score >= 3000) {
    game->level = 5;
    game->speed = 175;
  } else if (score >= 2400) {
    game->level = 4;
    game->speed = 200;
  } else if (score >= 1800) {
    game->level = 3;
    game->speed = 225;
  } else if (score >= 1200) {
    game->level = 2;
    game->speed = 250;
  }
}

void remove_figure(GameInfo_t *game) {
  int(*shape)[SHAPE_SIZE] = game->figure.shape;

  for (int i = 0; i < SHAPE_SIZE; i++) {
    for (int j = 0; j < SHAPE_SIZE; j++) {
      if (shape[i][j] != SPACE) {
        remove_elem(game, i, j);
      }
    }
  }
}

void remove_elem(GameInfo_t *game, int i, int j) {
  int y = game->figure.y + i;
  int x = game->figure.x + j;
  game->field[y][x] = SPACE;
}

void place_figure(GameInfo_t *game) {
  int(*shape)[SHAPE_SIZE] = game->figure.shape;

  for (int i = 0; i < SHAPE_SIZE; i++) {
    for (int j = 0; j < SHAPE_SIZE; j++) {
      if (shape[i][j] != SPACE) {
        place_elem(game, i, j);
      }
    }
  }
}

void place_elem(GameInfo_t *game, int i, int j) {
  int y = game->figure.y + i;
  int x = game->figure.x + j;
  game->field[y][x] = game->figure.shape[i][j];
}

void check_game_over(GameInfo_t *game) {
  int max_field_row = 1;
  int max_spawn_row = 2;

  for (int cell = 1; cell < FIELD_WIDTH - 1; cell++) {
    if (game->field[max_field_row][cell] != SPACE &&
        (game->figure.y != max_spawn_row || can_move(*game, DOWN))) {
      game->over = 1;
      game->exit = 1;
    }
  }
}
