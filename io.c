#include <stdio.h>
#include <stdlib.h>

#include "io.h"

static void handle_file(char *filename, FILE **restrict stream, const char *mode)
{
    // caso tenhamos o nome do aqruivo, não mexeremos no ponteiro para podermos usar o default
    if (filename != NULL)
    {
        *stream = fopen(filename, mode);
        if (*stream == NULL)
        {
            perror("");
            exit(1);
        }
    }
}

extern void handle_input(char *filename, FILE **restrict stream)
{
    handle_file(filename, stream, "rb");
}

extern void handle_output(char *filename, FILE **restrict stream)
{
    handle_file(filename, stream, "wb");
}

extern void write_file(FILE *restrict stream, const struct header h, int16_t *samples)
{
    fwrite(&h, sizeof(h), 1, stream);
    fwrite(samples, h.data.subChunk2Size, 1, stream);
}

extern void cleanup(FILE *restrict input, FILE *restrict output, int16_t *samples)
{
    fclose(input);
    fclose(output);
    free(samples);
}