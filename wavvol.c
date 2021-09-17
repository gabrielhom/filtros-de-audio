#include <getopt.h>
#include <stdlib.h>

#include "wav.h"
#include "io.h"

static int set_vol(const struct header h, int16_t *samples, const float level)
{
    if (level < 0.0f || level > 10.0f)
        return 0;
    
    size_t length = sample_count(h);
    int32_t value;
    for (size_t i = 0; i < length; i++)
    {
        value = samples[i] * level;
        samples[i] = DELIMIT(value);
    }
    return 1;
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
    int16_t *samples = get_samples(input, header);
    
        // função do filtro
    if (!set_vol(header, samples, level))
    {
        fprintf(stderr, "Volume inválido!\n");
        exit(1);
    }
    write_file(output, header, samples);
    cleanup(input, output, samples);
}