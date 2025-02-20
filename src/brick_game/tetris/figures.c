#include "tetris.h"

const int figures[7][4][4] = {
    // Квадрат
    {{32, 32, 32, 32}, {32, 42, 42, 32}, {32, 42, 42, 32}, {32, 32, 32, 32}},
    // Прямая линия
    {{32, 32, 32, 32}, {42, 42, 42, 42}, {32, 32, 32, 32}, {32, 32, 32, 32}},
    // T
    {{32, 42, 32, 32}, {42, 42, 42, 32}, {32, 32, 32, 32}, {32, 32, 32, 32}},
    // L
    {{32, 32, 42, 32}, {42, 42, 42, 32}, {32, 32, 32, 32}, {32, 32, 32, 32}},
    // Обратная L
    {{42, 32, 32, 32}, {42, 42, 42, 32}, {32, 32, 32, 32}, {32, 32, 32, 32}},
    // Z
    {{32, 32, 32, 32}, {42, 42, 32, 32}, {32, 42, 42, 32}, {32, 32, 32, 32}},
    // Обратная Z
    {{32, 32, 32, 32}, {32, 42, 42, 32}, {42, 42, 32, 32}, {32, 32, 32, 32}}};
