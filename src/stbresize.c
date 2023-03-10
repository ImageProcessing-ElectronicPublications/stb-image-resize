#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <unistd.h>
#include <stb/stb_image.h>
#include <stb/stb_image_write.h>
#include "bicubic.h"
#include "biakima.h"
#include "biline.h"
#include "gsample.h"
#include "ris.h"
#include "hris.h"
#include "gauss.h"

#define RESIZE_VERSION "1.8"

void resize_usage(char* prog, int resize_height, int resize_width, float ratio, int method, float pris, int fris)
{
    printf("StbResize version %s.\n", RESIZE_VERSION);
    printf("usage: %s [options] image_in out.png\n", prog);
    printf("options:\n");
    printf("  -H NUM    resize height (default %d)\n", resize_height);
    printf("  -W NUM    resize width (default %d)\n", resize_width);
    printf("  -i        use RIS prescale (default %d)\n", fris);
    printf("  -m NUM    method: 0 - bicubic, 1 - biakima, -1 - biline, -2 - gsample (default %d)\n", method);
    printf("  -p N.M    part prefilter RIS (default %f)\n", pris);
    printf("  -r N.M    sample ratio (default %f)\n", ratio);
    printf("  -h        show this help message and exit\n");
}

int main(int argc, char **argv)
{
    int height, width, channels, y, x, d;
    int resize_height = 0, resize_width = 0;
    int ris_height, ris_width;
    float ratio = 1.0f;
    int method = 0, gaussrx = 0, gaussry = 0;
    float pris = 0.0f, vgauss, vris = 0.0f;
    int fris = 0;
    int fhelp = 0;
    int opt;
    size_t szorig, szdest, ki, kd;
    unsigned char *data = NULL, *resize_data = NULL, *ris_data = NULL;
    stbi_uc *img = NULL;

    while ((opt = getopt(argc, argv, ":H:W:im:p:r:h")) != -1)
    {
        switch(opt)
        {
        case 'H':
            resize_height = atoi(optarg);
            break;
        case 'W':
            resize_width = atoi(optarg);
            break;
        case 'i':
            fris = !fris;
            break;
        case 'm':
            method = atoi(optarg);
            break;
        case 'p':
            pris = atof(optarg);
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
        resize_usage(argv[0], resize_height, resize_width, ratio, method, pris, fris);
        return 0;
    }
    const char *src_name = argv[optind];
    const char *dst_name = argv[optind + 1];


    printf("Load: %s\n", src_name);
    if (!(img = stbi_load(src_name, &width, &height, &channels, STBI_rgb_alpha)))
    {
        fprintf(stderr, "ERROR: not read image: %s\n", src_name);
        return 1;
    }
    printf("image: %dx%d:%d\n", width, height, channels);
    if (!(data = (unsigned char*)malloc(height * width * channels * sizeof(unsigned char))))
    {
        fprintf(stderr, "ERROR: not use memmory\n");
        return 1;
    }
    ki = 0;
    kd = 0;
    for (y = 0; y < height; y++)
    {
        for (x = 0; x < width; x++)
        {
            for (d = 0; d < channels; d++)
            {
                data[kd + d] = (unsigned char)img[ki + d];
            }
            ki += STBI_rgb_alpha;
            kd += channels;
        }
    }
    stbi_image_free(img);
    szorig = (size_t)height * width;

    if ((resize_height == 0) && (resize_width == 0))
    {
        resize_height = (int)((float)height * ratio + 0.5f);
        resize_width = (int)((float)width * ratio + 0.5f);
    }
    else
    {
        if (resize_height == 0)
        {
            ratio = (float)resize_width / (float)width;
            resize_height = (int)((float)height * ratio + 0.5f);
        }
        if (resize_width == 0)
        {
            ratio = (float)resize_height / (float)height;
            resize_width = (int)((float)width * ratio + 0.5f);
        }
    }
    if ((resize_height == 0) || (resize_width == 0))
    {
        fprintf(stderr, "ERROR: bad target size %dx%d:%d\n", resize_width, resize_height, channels);
        return 1;
    }
    szdest = (size_t)resize_height * resize_width;

    if (fris)
    {
        ki = szdest / szorig;
        kd = szorig / szdest;
        if (kd > 3)
        {
			kd = sqrt(kd);
            ris_height = (height + kd - 1) / kd;
            ris_width = (width + kd - 1) / kd;
            if (!(resize_data = (unsigned char*)malloc(ris_height * ris_width * channels * sizeof(unsigned char))))
            {
                fprintf(stderr, "ERROR: not use memmory\n");
                return 2;
            }
            printf("method: mean\n");
            printf("down: %dx%d:%d\n", ris_width, ris_height, channels);
            ResizeImageMean (data, height, width, channels, kd, resize_data);
            free(data);
            height = ris_height;
            width = ris_width;
            if (!(data = (unsigned char*)malloc(height * width * channels * sizeof(unsigned char))))
            {
                fprintf(stderr, "ERROR: not use memmory\n");
                return 1;
            }
            kd = 0;
            for (y = 0; y < height; y++)
            {
                for (x = 0; x < width; x++)
                {
                    for (d = 0; d < channels; d++)
                    {
                        data[kd] = resize_data[kd];
                        kd++;
                    }
                }
            }
            free(resize_data);
        }
        else if (ki > 1)
        {
            while (ki > 1)
            {
                ris_height = height * 2;
                ris_width = width * 2;
                if (!(resize_data = (unsigned char*)malloc(ris_height * ris_width * channels * sizeof(unsigned char))))
                {
                    fprintf(stderr, "ERROR: not use memmory\n");
                    return 2;
                }
                printf("method: hris\n");
                printf("up: %dx%d:%d\n", ris_width, ris_height, channels);
                ResizeImageHRIS (data, height, width, channels, 2, resize_data);
                free(data);
                height = ris_height;
                width = ris_width;
                if (!(data = (unsigned char*)malloc(height * width * channels * sizeof(unsigned char))))
                {
                    fprintf(stderr, "ERROR: not use memmory\n");
                    return 1;
                }
                kd = 0;
                for (y = 0; y < height; y++)
                {
                    for (x = 0; x < width; x++)
                    {
                        for (d = 0; d < channels; d++)
                        {
                            data[kd] = resize_data[kd];
                            kd++;
                        }
                    }
                }
                free(resize_data);
                ki /= 4;
            }
        }
    }
    else
    {
        if ((method > -2) && ((resize_height < height) || (resize_width < width)))
        {
            printf("prefilter: Gauss\n");
            gaussry = (resize_height < height) ? (0.5f * (float)height / (float)resize_height) : 0.0f;
            gaussrx = (resize_width < width) ? (0.5f * (float)width / (float)resize_width) : 0.0f;
            vgauss = GaussBlurFilter(data, height, width, channels, gaussry, gaussrx);
            printf("  gauss-value: %f\n", vgauss);
        }
    }

    printf("resize: %dx%d:%d\n", resize_width, resize_height, channels);
    if (!(resize_data = (unsigned char*)malloc(resize_height * resize_width * channels * sizeof(unsigned char))))
    {
        fprintf(stderr, "ERROR: not use memmory\n");
        return 2;
    }

    if (method == -2)
    {
        printf("method: gsample\n");
        ResizeImageGSample(data, height, width, channels, resize_height, resize_width, resize_data);
    }
    else if (method == -1)
    {
        printf("method: biline\n");
        ResizeImageBiLine(data, height, width, channels, resize_height, resize_width, resize_data);
    }
    else if (method == 1)
    {
        printf("method: biakima\n");
        ResizeImageBiAkima(data, height, width, channels, resize_height, resize_width, resize_data);
    }
    else
    {
        printf("method: bicubic\n");
        ResizeImageBiCubic(data, height, width, channels, resize_height, resize_width, resize_data);
    }

    if (pris > 0.0f)
    {
        printf("prefilter: RIS\n");
        if (!(ris_data = (unsigned char*)malloc(height * width * channels * sizeof(unsigned char))))
        {
            fprintf(stderr, "ERROR: not use memmory\n");
            return 1;
        }
        ResizeImageGSample(resize_data, resize_height, resize_width, channels, height, width, ris_data);
        vris = ImageReFilter(data, ris_data, height, width, channels, pris);
        printf("  ris-value: %f\n", vris);
        if (method == -2)
        {
            ResizeImageGSample(ris_data, height, width, channels, resize_height, resize_width, resize_data);
        }
        else if (method == -1)
        {
            ResizeImageBiLine(ris_data, height, width, channels, resize_height, resize_width, resize_data);
        }
        else if (method == 1)
        {
            ResizeImageBiAkima(ris_data, height, width, channels, resize_height, resize_width, resize_data);
        }
        else
        {
            ResizeImageBiCubic(ris_data, height, width, channels, resize_height, resize_width, resize_data);
        }
        free(ris_data);
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
