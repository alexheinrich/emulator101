SDL2 := /opt/sdl2
SDL2_INC := $(SDL2)/include
SDL2_LIB := $(SDL2)/lib
LIBS = $(SDL2_LIB)/libSDL2.a

GCC := gcc
C_FLAGS := -Og -g -Wall -Wextra -Wconversion -Wsign-conversion -I $(SDL2_INC)

SRC := main.c emulator8080.c debug8080.c disassembler8080.c utils8080.c test8080.c shift_register.c video_driver.c
OBJ := $(SRC:.c=.o)
OBJ_P := $(OBJ:%=build/%)

build/%.o: src/%.c $(wildcard *.h) Makefile
	$(GCC) $(C_FLAGS) -c -o $@ $<  
emulator8080: $(OBJ_P)
	$(GCC) -g $^ $(LIBS) -o emulator8080
run_test: emulator8080
	valgrind --leak-check=full --show-leak-kinds=all ./emulator8080 -t
all: emulator8080
clean:
	rm -rf build/*.o *.dSYM/ emulator8080
