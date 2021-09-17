#include <getopt.h>
#include <stdlib.h>

#include "wav.h"
#include "io.h"

// função para pegar o pico das amostras
static int16_t find_maximum(int16_t *samples, const size_t length)
{
    // supondo que a primeira amostra é a maior usando seu valor absoluto
    int16_t max = abs(samples[0]);
    for (size_t i = 1; i < length; i++)
    {
        if (abs(samples[i]) > max)
        {
            max = abs(samples[i]);
        }
    }
    return max;
}

static void normalize(const struct header h, int16_t *samples)
{
    size_t length = sample_count(h);
    int16_t peak = find_maximum(samples, length);
    const double ratio = INT16_MAX / peak;
    for (size_t i = 0; i < length; i++)
    {
        samples[i] *= ratio;
    }
}

int main(int argc, char **argv)
{
    char *input_file = NULL, *output_file = NULL;
    int opt;
    while ((opt = getopt(argc, argv, "o:i:")) != -1)
    {
        switch (opt)
        {
        case 'i':
            input_file = optarg;
            break;
        case 'o':
            output_file = optarg;
            break;
        }
    }
    FILE *input = stdin, *output = stdout;
    handle_input(input_file, &input);
    handle_output(output_file, &output);
    struct header header = get_header(input);
    int16_t *samples = get_samples(input, header);

    // função do filtro
    normalize(header, samples);
    
    write_file(output, header, samples);
    cleanup(input, output, samples);
}