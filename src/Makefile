CC=gcc
FLAGS=-Wall -Wextra -Werror -std=c11 -lncurses
VALGRIND_FLAGS=--log-file="valgrind.txt" --tool=memcheck --leak-check=yes --track-origins=yes
LIB_NAME=s21_tetris.a

FILES_C=brick_game/tetris/*.c gui/cli/*.c main.c
FILES_H=brick_game/tetris/*.h
FILES_O=*.o
EXEC_FILES=tetris
PACKAGE_NAME=tetris-1.0

UNAME_S = $(shell uname)

ifeq ($(UNAME_S),Darwin)
OPEN_CMD=open
endif

ifeq ($(UNAME_S),Linux)
OPEN_CMD=xdg-open
FLAGS+=-lm -lsubunit
endif

.PHONY: all install uninstall s21_tetris.a build_library clean dvi dist clang valgrind

all: install

install: $(EXEC_FILES)

uninstall:
	rm -rf $(EXEC_FILES)

s21_tetris.a:
	$(CC) -c $(FILES_C) $(FLAGS)
	make build_library
	rm -rf $(FILES_O)

build_library:
	ar rcs $(LIB_NAME) $(FILES_O)

clean:
	rm -rf $(EXEC_FILES)
	rm -rf $(FILES_O) 
	rm -rf *.a 
	rm -rf *.info 
	rm -rf $(PACKAGE_NAME).tar.gz

dvi:
	$(OPEN_CMD) Documentation/Documentation.html

dist:
	mkdir -p $(PACKAGE_NAME)
	cp -a brick_game $(PACKAGE_NAME)
	cp -a gui $(PACKAGE_NAME)
	cp -a main.c $(PACKAGE_NAME)
	cp -a Documentation/Documentation.html $(PACKAGE_NAME)

	tar -cvzf $(PACKAGE_NAME).tar.gz $(PACKAGE_NAME)
	rm -rf $(PACKAGE_NAME)

clang:
	clang-format $(FILES_C) $(FILES_H) -n --style=Google

valgrind: $(EXEC_FILES)
	valgrind $(VALGRIND_FLAGS) ./$(EXEC_FILES)	

$(EXEC_FILES):
	$(CC) $(FILES_C) $(FLAGS) -o $(EXEC_FILES)
