#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>

#include "wav.h"
#include "io.h"

// função para facilitar a impressão das tags já que elas não são strings
static void print_tag(const char tag[4])
{
    printf("\"");
    for (int i = 0; i < 4; i++)
        printf("%c", tag[i]);
    printf("\"\n");
}

static void print_info(const struct header h)
{
    printf("riff tag\t(%lu bytes): ", sizeof(h.RIFF.chunkID));
    print_tag(h.RIFF.chunkID);
    printf("riff size\t(%lu bytes): %u\n", sizeof(h.RIFF.chunkSize), h.RIFF.chunkSize);
    printf("wave tag\t(%lu bytes): ", sizeof(h.RIFF.format));
    print_tag(h.RIFF.format);
    printf("form tag\t(%lu bytes): ", sizeof(h.fmt.subChunk1ID));
    print_tag(h.fmt.subChunk1ID);
    printf("fmt_size\t(%lu bytes): %u\n", sizeof(h.fmt.subChunk1Size), h.fmt.subChunk1Size);
    printf("audio_format\t(%lu bytes): %u\n", sizeof(h.fmt.audio_format), h.fmt.audio_format);
    printf("num_channels\t(%lu bytes): %u\n", sizeof(h.fmt.num_channels), h.fmt.num_channels);
    printf("sample_rate:\t(%lu bytes): %u\n", sizeof(h.fmt.sample_rate), h.fmt.sample_rate);
    printf("byte_rate\t(%lu bytes): %u\n", sizeof(h.fmt.byte_rate), h.fmt.byte_rate);
    printf("block_align\t(%lu bytes): %u\n", sizeof(h.fmt.block_align), h.fmt.block_align);
    printf("bits_per_sample\t(%lu bytes): %u\n", sizeof(h.fmt.bits_per_sample), h.fmt.bits_per_sample);
    printf("data tag\t(%lu bytes): ", sizeof(h.data.subChunk2ID));
    print_tag(h.data.subChunk2ID);
    printf("data size\t(%lu bytes): %u\n", sizeof(h.data.subChunk2Size), h.data.subChunk2Size);
    printf("bytes per sample\t : %u\n", h.fmt.bits_per_sample / 8U);
    printf("samples per channel\t : %u\n", h.data.subChunk2Size / (h.fmt.subChunk1Size / 8U) / h.fmt.num_channels);
}

int main(int argc, char **argv)
{
    char *filename = NULL;
    int opt;
    while ((opt = getopt(argc, argv, "i:")) != -1)
    {
        if (opt == 'i')
        {
            filename = optarg;
        }
    }
    FILE *wav = stdin;
    handle_input(filename, &wav);
    struct header header = get_header(wav);
    
    // função do filtro
    print_info(header);
    
    fclose(wav);
}
