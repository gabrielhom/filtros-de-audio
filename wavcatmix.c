#include <getopt.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "io.h"
#include "wav.h"

// retorna o header com mais amostras
static struct header largest(struct header *h, size_t file_count)
{
    uint32_t max = 0;
    for (size_t i = 1; i < file_count; i++)
    {
        if (h[i].data.subChunk2Size > h[max].data.subChunk2Size)
        {
            max = i;
        }
    }
    return h[max];
}

// concatena input em output, realocando output
static void concat(struct header input_header, int16_t *input,
                   struct header *output_header, int16_t **output)
{
    // nova struct que será o header da saída
    struct header temp = *output_header;

    // somando o tamanho das amostras da entrada com a saída
    temp.data.subChunk2Size += input_header.data.subChunk2Size;

    *output = realloc(*output, temp.data.subChunk2Size);
    if (output == NULL)
    {
        perror("");
        exit(2);
    }
    size_t input_length = sample_count(input_header);
    size_t output_length = sample_count(*output_header);

    for (size_t i = 0; i < input_length; i++)
    {
        (*output)[i + output_length] = input[i];
    }
    *output_header = temp;
}

// mixa input em output
static void mix(struct header input_header, int16_t *input,
                int16_t *output, size_t file_count)
{
    size_t input_length = sample_count(input_header);
    for (size_t i = 0; i < input_length; i++)
    {
        // dividindo pelo número de arquivos para evitar clipping
        output[i] += input[i] / file_count;
    }
}

int main(int argc, char **argv)
{
    char *output_file = NULL;
    int opt;
    while ((opt = getopt(argc, argv, "o:")) != -1)
    {
        if (opt == 'o')
        {
            output_file = optarg;
        }
    }
    FILE *input = stdin, *output = stdout;
    handle_output(output_file, &output);

    size_t file_count = argc - optind;
    struct header headers[file_count];
    int16_t *file_samples[file_count];

    // lendo o primeiro arquivo fora do loop para poder usar usa sample rate de base
    handle_input(argv[optind], &input);
    headers[0] = get_header(input);
    struct header *output_header = &headers[0];
    file_samples[0] = get_samples(input, *output_header);
    fclose(input);

    const uint32_t required_sample_rate = output_header->fmt.sample_rate;

    for (int i = optind + 1, j = 1; i < argc; i++)
    {
        handle_input(argv[i], &input);
        struct header input_header = get_header(input);
        // descartamos arquivos com sample rate diferente e avisamos na stderr
        if (input_header.fmt.sample_rate != required_sample_rate)
        {
            fprintf(stderr, "Arquivo %s com sample rate incompatível!\n"
                            "Sample rate requerida: %uhz\n"
                            "Sample rate do arquivo: %uhz\n",
                    argv[i],
                    required_sample_rate,
                    input_header.fmt.sample_rate);
            fclose(input);
            file_count--;
        }
        else
        {
            headers[j] = input_header;
            file_samples[j] = get_samples(input, input_header);
            j++;
            fclose(input);
        }
    }

    // se há pelo menos um arquivo stereo para ser concatenado
    if (stereo_required(headers, file_count))
    {
        for (size_t i = 0; i < file_count; i++)
        {
            // então converteremos todos os monos para stereo
            if (is_mono(headers[i]))
            {
                mono_to_stereo(&headers[i], &file_samples[i]);
            }
        }
    }
    int16_t *output_samples = NULL;

    // brincando um pouco com links para aproveitar as funções main quase idênticas
    if (strcmp("./wavcat", argv[0]) == 0)
    {

        // concatenando todos os arquivos no primeiro
        for (size_t i = 1; i < file_count; i++)
        {
            concat(headers[i], file_samples[i], output_header, &file_samples[0]);
        }
        output_samples = file_samples[0];
    }
    else if (strcmp("./wavmix", argv[0]) == 0)
    {
        // criando a saída com amostras em branco para mixar os outros nela
        *output_header = largest(headers, file_count);
        output_samples = calloc(sizeof(int16_t), output_header->data.subChunk2Size);
        for (size_t i = 0; i < file_count; i++)
        {
            mix(headers[i], file_samples[i], output_samples, file_count);
        }
    }
    else
    {
        abort();
    }
    update_riff_size(output_header);
    write_file(output, *output_header, output_samples);

    for (size_t i = 0; i < file_count; i++)
    {
        free(file_samples[i]);
    }
    fclose(output);
}