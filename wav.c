#include <stdio.h>
#include <stdlib.h>

#include "wav.h"

// lê e retorna o cabeçalho do arquivo
extern struct header get_header(FILE *restrict stream)
{
    struct header header;
    if (fread(&header, sizeof(header), 1, stream) != 1)
    {
        fprintf(stderr, "Erro ao ler o header do arquivo!\n");
        exit(2);
    }
    return header;
}

// lê e retorna um vetor com as amostras do arquivo
extern int16_t *get_samples(FILE *restrict stream, const struct header h)
{
    int16_t *samples = malloc(h.data.subChunk2Size);
    if (samples == NULL)
    {
        perror("");
        exit(2);
    }
    if (fread(samples, h.data.subChunk2Size, 1, stream) != 1)
    {
        fprintf(stderr, "Erro ao ler os samples do arquivo!\n");
        exit(2);
    }
    return samples;
}

// quantidade de amostras do áudio
extern size_t sample_count(const struct header h)
{
    return h.data.subChunk2Size / h.fmt.block_align;
}

// atualiza o tamanho do chunkSize do RIFF, necessária em filtros como o cat
extern void update_riff_size(struct header *h)
{
    h->RIFF.chunkSize = h->data.subChunk2Size + sizeof(struct header) - sizeof(h->data.subChunk2ID) - sizeof(h->data.subChunk2Size);
}

extern int is_stereo(const struct header h)
{
    return h.fmt.num_channels == 2;
}

extern int is_mono(const struct header h)
{
    return h.fmt.num_channels == 1;
}

// verifica se há pelo menos uma amostras stereo
extern int stereo_required(struct header *headers, size_t file_count)
{
    for (size_t i = 0; i < file_count; i++)
    {
        if (is_stereo(headers[i]))
        {
            return 1;
        }
    }
    return 0;
}

// converte de mono para stereo as amostras e atualiza o header
extern void mono_to_stereo(struct header *h, int16_t **samples)
{
    h->fmt.num_channels = 2;
    h->data.subChunk2Size *= 2;
    // novo ponteiro para as amostras
    int16_t *new_ptr = malloc(h->data.subChunk2Size);
    if (new_ptr == NULL)
    {
        perror("");
        exit(2);
    }
    size_t length = sample_count(*h);
    // duplicando as amostras mono entre os 2 canais
    for (size_t i = 0, j = 0; i < length; i+= 2, j++)
    {
        new_ptr[i] = (*samples)[j];
        new_ptr[i + 1] = (*samples)[j];
    }
    // não precisamos mais do anterior
    free(*samples);
    *samples = new_ptr;
}