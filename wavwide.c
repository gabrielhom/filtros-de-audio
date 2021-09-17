#include <getopt.h>
#include <stdlib.h>

#include "io.h"
#include "wav.h"

#define LEFT i
#define RIGHT i + 1

static void expand_stereo(struct header h, int16_t *samples, float level)
{
    size_t length = sample_count(h);
    int16_t diff;
    // aplicando em ambos os canais pois exigimos que o arquivo seja stereo
    for(size_t i = 0; i < length; i += 2)
    {
        diff = samples[RIGHT] - samples[LEFT];
        samples[LEFT] -= level * diff;
        samples[RIGHT] += level * diff;
    }
}

int main(int argc, char **argv)
{
    int opt;
    float level = 1.0;
    char *input_file = NULL, *output_file = NULL;
    while ((opt = getopt(argc, argv, "l:o:i:")) != -1)
    {
        switch (opt)
        {
        case 'i':
            input_file = optarg;
            break;
        case 'o':
            output_file = optarg;
            break;
        case 'l':
            level = atof(optarg);
        }
    }
    FILE *input = stdin, *output = stdout;
    handle_input(input_file, &input);
    handle_output(output_file, &output);
    struct header header = get_header(input);
    
    if (!is_stereo(header))
    {
        fprintf(stderr, "Arquivo não é estéreo!\n");
        exit(1);
    }
    int16_t *samples = get_samples(input, header);
    
    // função do filtro
    expand_stereo(header, samples, level);
    
    write_file(output, header, samples);
    cleanup(input, output, samples);
}