#ifndef WAV_H
#define WAV_H

#include <stdint.h>
#include <stdio.h>

// macro para evitar problemas de atribuição de valores fora dos limites do tipo
#define DELIMIT(value) (value) > INT16_MAX ? INT16_MAX : (value) < INT16_MIN ? INT16_MIN : (value)

// struct com o mesmo formato do header do arquivo WAV
struct header
{
    struct RIFF
    {
        char chunkID[4];
        uint32_t chunkSize;
        char format[4];
    } RIFF;
    struct fmt
    {
        char subChunk1ID[4];
        uint32_t subChunk1Size;
        uint16_t audio_format;
        uint16_t num_channels;
        uint32_t sample_rate;
        uint32_t byte_rate;
        uint16_t block_align;
        uint16_t bits_per_sample;
    } fmt;
    struct data
    {
        char subChunk2ID[4];
        uint32_t subChunk2Size;
    } data;
};

struct header get_header(FILE *restrict stream);
int16_t *get_samples(FILE *restrict stream, struct header h);
size_t sample_count(struct header h);
void update_riff_size(struct header *h);
int is_stereo(struct header h);
int is_mono(const struct header h);
int stereo_required(struct header *headers, size_t file_count);
void mono_to_stereo(struct header *h, int16_t **samples);

#endif
