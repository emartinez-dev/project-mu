#include <math.h>
#include <stdio.h>
#include <string.h>

#include "wave.h"

typedef int16_t sample_t;
#define SAMPLE_MAX 32767

#define DURATION 1
#define SR 44100
#define NCHANNELS 1
#define NSAMPLES (NCHANNELS * DURATION * SR)

static sample_t buf[NSAMPLES];

int is_le(void)
{
  int i = 1;
  return *(char*)&i;
}

int main(void)
{
    struct wav_hdr hdr = {0};
    FILE *fp = fopen("output.wav", "wb");

    printf("Is little endian? %d\n", is_le());

    /* RIFF header */
    memcpy(&hdr.riff.id, "RIFF", 4);
    hdr.riff.size = 36 + NSAMPLES * sizeof(sample_t);
    memcpy(&hdr.riff.type, "WAVE", 4);

    /* FMT chunk */
    memcpy(&hdr.fmt.id, "fmt ", 4);
    hdr.fmt.size = 16;
    hdr.fmt.fmt_tag = 1; /* linear PCM */
    hdr.fmt.channels = NCHANNELS;
    hdr.fmt.samples_per_sec = SR;
    hdr.fmt.bytes_per_sec = NCHANNELS * SR * sizeof(sample_t);
    hdr.fmt.block_align = NCHANNELS * sizeof(sample_t);
    hdr.fmt.bits_per_sample = 8 * sizeof(sample_t);

    /* DATA header */
    memcpy(&hdr.data.id, "data", 4);
    hdr.data.size = NSAMPLES * sizeof(sample_t);

    fwrite(&hdr, sizeof(struct wav_hdr), 1, fp);

    printf("N samples: %d\n", NSAMPLES);
    for (size_t i = 0; i < NSAMPLES; ++i)
        buf[i] = lrint(SAMPLE_MAX * sin(2 * M_PI * 440 * i / SR));

    fwrite(buf, sizeof(buf), 1, fp);

    /* 1 byte padding if buf size is odd */
    if (sizeof(buf) % 2 == 1)
    {
        char nil = 0;
        fwrite(&nil, 1, 1, fp);
    }
    fclose(fp);
    return (0);
}