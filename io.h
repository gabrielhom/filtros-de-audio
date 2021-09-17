#ifndef IO_H
#define IO_H

#include <stdio.h>
#include <stdint.h>

#include "wav.h"

// trata a entrada 
void handle_input(char *filename, FILE **stream);

// trata a saída
void handle_output(char *filename, FILE **stream);

// fecha os arquivos de entrada e saída e libera a memória das amostras
void cleanup(FILE *restrict input, FILE *restrict output, int16_t *samples);

// escreve o arquivo wav
void write_file(FILE *restrict stream, struct header h, int16_t *samples);

#endif
