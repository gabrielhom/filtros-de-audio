#include <getopt.h>

#include "wav.h"
#include "io.h"

// macro para fazer a troca das amostras
#define SWAP(a, b)                 \
    do                             \
    {                              \
        __typeof__(a) _temp = (a); \
        (a) = (b);                 \
        (b) = _temp;               \
    } while (0)

// inverte as amostras sem trocar os canais
static void reverse(struct header h, int16_t *samples)
{
    size_t length = sample_count(h);
    for (size_t i = 0; i < length / 2; i += h.fmt.num_channels)
    {
        // loop para lidar com qualquer número de canais
        for (size_t j = 0; j < h.fmt.num_channels; j++)
        {
            SWAP(samples[i + j], samples[length - i - j]);
        }
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
    reverse(header, samples);
    
    write_file(output, header, samples);
    cleanup(input, output, samples);
}