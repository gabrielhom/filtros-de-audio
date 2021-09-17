#include <getopt.h>
#include <stdlib.h>

#include "wav.h"
#include "io.h"

static void echo(const struct header h, int16_t *samples, const double level, const int delay)
{
    // quantos samples / ms temos no arquivo
    size_t step = h.fmt.sample_rate / 1000 * h.fmt.num_channels;
    size_t distance = step * delay;
    int32_t value;
    size_t length = sample_count(h);
    for(size_t i = distance; i < length; i++)
    {
        value = samples[i] + samples[i - distance] * level;
        samples[i] = DELIMIT(value);
    }
}

int main(int argc, char **argv)
{
    char *input_file = NULL, *output_file = NULL;
    int delay = 0;
    double level = 0;
    int opt;
    while ((opt = getopt(argc, argv, "l:t:o:i:")) != -1)
    {
        switch (opt)
        {
        case 'i':
            input_file = optarg;
            break;
        case 'o':
            output_file = optarg;
            break;
        case 't':
            delay = atoi(optarg);
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
    echo(header, samples, level, delay);
    
    write_file(output, header, samples);
    cleanup(input, output, samples);
}