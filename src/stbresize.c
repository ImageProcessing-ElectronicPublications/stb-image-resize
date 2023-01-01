#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "image-stb.h"
#include "bicubic.h"
#include "biakima.h"
#include "biline.h"

#define RESIZE_VERSION "1.2"

void resize_usage(char* prog, float ratio, int method)
{
    printf("StbResize version %s.\n", RESIZE_VERSION);
    printf("usage: %s [options] image_in out.png\n", prog);
    printf("options:\n");
    printf("  -m NUM    method: 0 - bicubic, 1 - biakima, -1 - biline (default %d)\n", method);
    printf("  -r N.M    sample ratio (default %f)\n", ratio);
    printf("  -h        show this help message and exit\n");
}

int main(int argc, char **argv)
{
    float ratio = 1.0f;
    int method = 0;
    int fhelp = 0;
    int opt;
    while ((opt = getopt(argc, argv, ":m:r:h")) != -1)
    {
        switch(opt)
        {
        case 'm':
            method = atoi(optarg);
            break;
        case 'r':
            ratio = atof(optarg);
            if (ratio < 0.0f)
            {
                fprintf(stderr, "Bad argument\n");
                fprintf(stderr, "ratio = %f\n", ratio);
                return 1;
            }
            break;
        case 'h':
            fhelp = 1;
            break;
        case ':':
            fprintf(stderr, "ERROR: option needs a value\n");
            return 2;
            break;
        case '?':
            fprintf(stderr, "ERROR: unknown option: %c\n", optopt);
            return 3;
            break;
        }
    }
    if(optind + 2 > argc || fhelp)
    {
        resize_usage(argv[0], ratio, method);
        return 0;
    }
    const char *src_name = argv[optind];
    const char *dst_name = argv[optind + 1];

    int height, width, channels;

    printf("Load: %s\n", src_name);
    stbi_uc* img = NULL;
    if (!(img = stbi_load(src_name, &width, &height, &channels, STBI_rgb_alpha)))
    {
        fprintf(stderr, "ERROR: not read image: %s\n", src_name);
        return 1;
    }
    printf("image: %dx%d:%d\n", width, height, channels);
    unsigned char* data = NULL;
    if (!(data = (unsigned char*)malloc(height * width * channels * sizeof(unsigned char))))
    {
        fprintf(stderr, "ERROR: not use memmory\n");
        return 1;
    }
    size_t ki = 0, kd = 0;
    for (int y = 0; y < height; y++)
    {
        for (int x = 0; x < width; x++)
        {
            for (int d = 0; d < channels; d++)
            {
                data[kd + d] = (unsigned char)img[ki + d];
            }
            ki += STBI_rgb_alpha;
            kd += channels;
        }
    }
    stbi_image_free(img);

    int resize_height = (int)((float)height * ratio + 0.5f);
    int resize_width = (int)((float)width * ratio + 0.5f);
    printf("resize: %dx%d:%d\n", resize_width, resize_height, channels);

    unsigned char *resize_data = NULL;
    if (!(resize_data = (unsigned char*)malloc(resize_height * resize_width * channels * sizeof(unsigned char))))
    {
        return 2;
    }

    if (method == -1)
    {
        printf("method: biline\n");
        ResizeImageBiLine(data, height, width, channels, ratio, resize_data);
    }
    else if (method == 1)
    {
        printf("method: biakima\n");
        ResizeImageBiAkima(data, height, width, channels, ratio, resize_data);
    }
    else
    {
        printf("method: bicubic\n");
        ResizeImageBiCubic(data, height, width, channels, ratio, resize_data);
    }

    printf("Save png: %s\n", dst_name);
    if (!(stbi_write_png(dst_name, resize_width, resize_height, channels, resize_data, resize_width * channels)))
    {
        fprintf(stderr, "ERROR: not write image: %s\n", dst_name);
        return 1;
    }

    free(resize_data);
    free(data);
    return 0;
}
