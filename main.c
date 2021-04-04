#include "emulator8080.h"
#include "disassembler8080.h"
#include "utils.h"

#include <assert.h>
#include <errno.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>


state8080 load_rom(FILE *f, size_t fsize)
{

    // 64KiB
    uint8_t *buffer = malloc(0x10000);
    if (buffer == NULL) {
        printf("malloc() failed. Errno: %s.\n", strerror(errno));
        exit(1);
    }
    memset(buffer, 0, 0x10000);

    if (fread(buffer, sizeof(uint8_t), fsize, f) != fsize) {
        printf("fread() failed. Errno: %s.\n", strerror(errno));
        exit_and_free(buffer);
    }

    state8080 state = {
        .a = 0,
        .b = 0,
        .c = 0,
        .d = 0,
        .e = 0,
        .h = 0,
        .l = 0,
        .pc = 0,
        .sp = 0,
        .memory = buffer,
        .cf = {
            .s = 0,
            .z = 0,
            .ac = 0,
            .p = 0,
            .cy = 0
        },
        .interrupts_enabled = 0
    };

    return state;
}

void get_instr_opbytes(char *str_ptr, uint8_t *opbytes)
{
    printf("%s", str_ptr);
    for (uint32_t c = 0; c < 3; ++c) {
        char *token;
        token = strsep(&str_ptr, "\n\t ");
        opbytes[c] = (uint8_t) strtol(token, NULL, 16);
    }
}

state8080 init_state_mem(uint8_t *opbytes)
{
    FILE *ram = open_f("test/ram.dat");
    size_t fsize = get_fsize(ram);
    state8080 state = load_rom(ram, fsize);

    memcpy(&state.memory[13], opbytes, 3);
    state.pc = 0x000d;

    return state;
}

void compare_states(state8080 *source, state8080 *target)
{
    bool halt = false;

    if (source->a != target->a) {
        printf("Register a not equal! Source: %02x Target: %02x\n", source->a, target->a);
        halt = true;
    }
    
    if (source->b != target->b) {
        printf("Register b not equal! Source: %02x Target: %02x\n", source->b, target->b);
        halt = true;    
    }
    
    if (source->c != target->c) {
        printf("Register c not equal! Source: %02x Target: %02x\n", source->c, target->c);
        halt = true;    
    }
    
    if (source->d != target->d) {
        printf("Register d not equal! Source: %02x Target: %02x\n", source->d, target->d);
        halt = true;    
    }

    if (source->e != target->e) {
        printf("Register e not equal! Source: %02x Target: %02x\n", source->e, target->e);
        halt = true;    
    }
    
    if (source->h != target->h) {
        printf("Register h not equal! Source: %02x Target: %02x\n", source->h, target->h);
        halt = true;    
    }

    if (source->l != target->l) {
        printf("Register l not equal! Source: %02x Target: %02x\n", source->l, target->l);
        halt = true;    
    }

    if (source->cf.cy != target->cf.cy) {
        printf("Flag cy not equal! Source: %u Target: %u\n", source->cf.cy, target->cf.cy);
        halt = true;    
    }

    if (source->cf.p != target->cf.p) {
        printf("Flag p not equal! Source: %u Target: %u\n", source->cf.p, target->cf.p);
        halt = true;    
    }

    if (source->cf.ac != target->cf.ac) {
        printf("Flag ac not equal! Source: %u Target: %u\n", source->cf.ac, target->cf.ac);
        halt = true;    
    }

    if (source->cf.z != target->cf.z) {
        printf("Flag z not equal! Source: %u Target: %u\n", source->cf.z, target->cf.z);
        halt = true;    
    }

    if (source->cf.s != target->cf.s) {
        printf("Flag s not equal! Source: %u Target: %u\n", source->cf.s, target->cf.s);
        halt = true;    
    }

    if (halt) {
        exit(1);
    }
}

void parse_state(char *str_ptr, state8080 *state)
{
    char *token;
    while((token = strsep(&str_ptr, "\n\t ")) != NULL) {
        char *token_cpy = token;
        char *reg = strsep(&token_cpy, ":"); 
        
        if (strcmp(reg, "a") == 0) {
            state->a = (uint8_t) strtol(token_cpy, NULL, 16);
        } else if (strcmp(reg, "b") == 0) {
            state->b = (uint8_t) strtol(token_cpy, NULL, 16);
        } else if (strcmp(reg, "c") == 0) {
            state->c = (uint8_t) strtol(token_cpy, NULL, 16);
        } else if (strcmp(reg, "d") == 0) {
            state->d = (uint8_t) strtol(token_cpy, NULL, 16);
        } else if (strcmp(reg, "e") == 0) {
            state->e = (uint8_t) strtol(token_cpy, NULL, 16);
        } else if (strcmp(reg, "h") == 0) {
            state->h = (uint8_t) strtol(token_cpy, NULL, 16);
        } else if (strcmp(reg, "l") == 0) {
            state->l = (uint8_t) strtol(token_cpy, NULL, 16);
        } else if (strcmp(reg, "sp") == 0) {
            state->sp = (uint16_t) strtol(token_cpy, NULL, 16);
        } else if (strcmp(reg, "sf") == 0) {
            state->cf.s = (uint8_t) (atoi(token_cpy) != 0);
        } else if (strcmp(reg, "zf") == 0) {
            state->cf.z = (uint8_t) (atoi(token_cpy) != 0);
        } else if (strcmp(reg, "af") == 0) {
            state->cf.ac = (uint8_t) (atoi(token_cpy) != 0);
        } else if (strcmp(reg, "pf") == 0) {
            state->cf.p = (uint8_t) (atoi(token_cpy) != 0);
        } else if (strcmp(reg, "cf") == 0) {
            state->cf.cy = (uint8_t) (atoi(token_cpy) != 0);
        }
    }
}

void exec_test_case(char **line_ptr, size_t *n, FILE *f)
{
    uint8_t opbytes[3];
    state8080 state;
    for (uint32_t i = 0; i < 5; ++i) {
        if (getline(line_ptr, n, f) == -1) {
            return;
        }

        char *line_cpy = *line_ptr;
        printf("%s", *line_ptr); 

        assert(line_ptr != NULL);
        char *h_str = strsep(&line_cpy, "\n\t ");
        printf("%s\n", h_str); 
        printf("%s", line_cpy); 

        if (strcmp(h_str, "inst") == 0) {
            get_instr_opbytes(line_cpy, opbytes);
        } else if (strcmp(h_str, "pre") == 0) {
            state = init_state_mem(opbytes);
            parse_state(line_cpy, &state);
            emulate8080(&state);
        } else if (strcmp(h_str, "post") == 0) {
            state8080 target_state;
            parse_state(line_cpy, &target_state);
            compare_states(&state, &target_state);
        } else if (strcmp(h_str, "ram") == 0) {
            // TODO
        } else if (strcmp(h_str, "io") == 0) {
            // TODO
        }
    }

    free(state.memory);
}

int32_t main(int32_t argc, char *argv[])
{
    if (argc < 2) {
        printf("Usage: %s <filename>\n", argv[0]);
        exit(1);
    }

    if (strcmp(argv[1], "-t") == 0) {
        FILE *f = open_f("test/test_cases.txt");
        char *line_ptr = NULL;
        size_t n = 0; 
        
        int count = 10;

        while (count > 0) {
            printf("------ %d\n", count);
            exec_test_case(&line_ptr, &n, f);
            count--;
        }

        return 0;
    }

    FILE *f = open_f(argv[1]);
    size_t fsize = get_fsize(f);
    state8080 state = load_rom(f, fsize);
    
    if (argc > 2 && strcmp(argv[2], "-d") == 0) {
        size_t bc = 0;

        // Disassemble 
        while (bc < fsize) {
            bc += disassemble_op8080(state.memory, bc);
        }
    } else {
        // Emulate
        bool emulate = true;
        int i = 0;
        while (emulate) {
            printf("cycle: %d\n", i);
            emulate = emulate8080(&state);
            i++;
        }
    }

    if (fclose(f) != 0) {
        printf("fclose() failed. Errno: %s.\n", strerror(errno));
        exit_and_free(state.memory);
    }

    free(state.memory);

    return 0;
}
