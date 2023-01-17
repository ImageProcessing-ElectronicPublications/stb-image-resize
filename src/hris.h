/*
https://github.com/heptagonhust/bicubic-image-resize/issues/9
*/

#ifndef HRIS_H_
#define HRIS_H_

#ifdef HRIS_STATIC
#define HRISAPI static
#else
#define HRISAPI extern
#endif

#ifdef __cplusplus
extern "C" {
#endif
HRISAPI void ResizeImageHRIS (unsigned char *src, int height, int width, int channels, int smode, unsigned char *res);
HRISAPI void ResizeImageMean (unsigned char *src, int height, int width, int channels, int reduce, unsigned char *res);
#ifdef __cplusplus
}
#endif

#ifdef HRIS_IMPLEMENTATION

HRISAPI void ResizeImageHRIS (unsigned char *src, int height, int width, int channels, int smode, unsigned char *res)
{
    unsigned d;
    int y, x, y2, x2, i, j, yf, xf;
    int h2, w2;
    float b[9], r[9];
    float k0[2], k1[3], k2[3], imx;
    size_t k[25], kf, ki;

    if (smode < 2)
    {
        smode = 2;
    }
    if (smode > 3)
    {
        smode = 3;
    }
    h2 = height * smode;
    w2 = width * smode;

    if (smode == 2)
    {
        k0[0] = 0.750f;
        k0[1] = 0.250f;
        k1[0] = k0[0] * k0[0];
        k1[1] = k0[0] * k0[1];
        k1[2] = k0[1] * k0[1];
        k2[0] = k1[0];
        k2[1] = k1[1] / 2.0f;
        k2[2] = k1[2] / 4.0f;
    }
    else
    {
        k0[0] = 2.0f / 3.0f;
        k0[1] = 1.0f / 3.0f;
        k1[0] = k0[0] * k0[0];
        k1[1] = k0[0] * k0[1];
        k1[2] = k0[1] * k0[1];
        k2[0] = (1.0f + 4.0f * k0[0] + 4.0f * k1[0]) / 9.0f;
        k2[1] = (k0[1] + 2.0f * k1[1]) / 9.0f;
        k2[2] = k1[2] / 9.0f;
    }

    for (d = 0; d < channels; d++)
    {
        for (y = 0; y < height; y++)
        {
            for (x = 0; x < width; x++)
            {
                kf = 0;
                for (i = -2; i < 3; i++)
                {
                    yf = ((y + i) < 0) ? 0 : ((y + i) < height) ? (y + i) : (height - 1);
                    for (j = -2; j < 3; j++)
                    {
                        xf = ((x + j) < 0) ? 0 : ((x + j) < width) ? (x + j) : (width - 1);
                        k[kf] = (yf * width + xf) * channels + d;
                        kf++;
                    }
                }
                imx = (float)src[k[0]];
                b[0] = -(k2[2] * imx);
                imx = (float)src[k[1]];
                b[0] -= (k2[1] * imx);
                b[1] = -(k2[2] * imx);
                imx = (float)src[k[2]];
                b[0] -= (k2[2] * imx);
                b[1] -= (k2[1] * imx);
                b[2] = -(k2[2] * imx);
                imx = (float)src[k[3]];
                b[1] -= (k2[2] * imx);
                b[2] -= (k2[1] * imx);
                imx = (float)src[k[4]];
                b[2] -= (k2[2] * imx);
                imx = (float)src[k[5]];
                b[0] -= (k2[1] * imx);
                b[3] = -(k2[2] * imx);
                imx = (float)src[k[6]];
                b[0] += ((2.0 - k2[0]) * imx);
                b[1] -= (k2[1] * imx);
                b[3] -= (k2[1] * imx);
                b[4] = -(k2[2] * imx);
                imx = (float)src[k[7]];
                b[0] -= (k2[1] * imx);
                b[1] += ((2.0 - k2[0]) * imx);
                b[2] -= (k2[1] * imx);
                b[3] -= (k2[2] * imx);
                b[4] -= (k2[1] * imx);
                b[5] = -(k2[2] * imx);
                imx = (float)src[k[8]];
                b[1] -= (k2[1] * imx);
                b[2] += ((2.0 - k2[0]) * imx);
                b[4] -= (k2[2] * imx);
                b[5] -= (k2[1] * imx);
                imx = (float)src[k[9]];
                b[2] -= (k2[1] * imx);
                b[5] -= (k2[2] * imx);
                imx = (float)src[k[10]];
                b[0] -= (k2[2] * imx);
                b[3] -= (k2[1] * imx);
                b[6] = -(k2[2] * imx);
                imx = (float)src[k[11]];
                b[0] -= (k2[1] * imx);
                b[1] -= (k2[2] * imx);
                b[3] += ((2.0 - k2[0]) * imx);
                b[4] -= (k2[1] * imx);
                b[6] -= (k2[1] * imx);
                b[7] = -(k2[2] * imx);
                imx = (float)src[k[12]];
                b[0] -= (k2[2] * imx);
                b[1] -= (k2[1] * imx);
                b[2] -= (k2[2] * imx);
                b[3] -= (k2[1] * imx);
                b[4] += ((2.0 - k2[0]) * imx);
                b[5] -= (k2[1] * imx);
                b[6] -= (k2[2] * imx);
                b[7] -= (k2[1] * imx);
                b[8] = -(k2[2] * imx);
                imx = (float)src[k[13]];
                b[1] -= (k2[2] * imx);
                b[2] -= (k2[1] * imx);
                b[4] -= (k2[1] * imx);
                b[5] += ((2.0 - k2[0]) * imx);
                b[7] -= (k2[2] * imx);
                b[8] -= (k2[1] * imx);
                imx = (float)src[k[14]];
                b[2] -= (k2[2] * imx);
                b[5] -= (k2[1] * imx);
                b[8] -= (k2[2] * imx);
                imx = (float)src[k[15]];
                b[3] -= (k2[2] * imx);
                b[6] -= (k2[1] * imx);
                imx = (float)src[k[16]];
                b[3] -= (k2[1] * imx);
                b[4] -= (k2[2] * imx);
                b[6] += ((2.0 - k2[0]) * imx);
                b[7] -= (k2[1] * imx);
                imx = (float)src[k[17]];
                b[3] -= (k2[2] * imx);
                b[4] -= (k2[1] * imx);
                b[5] -= (k2[2] * imx);
                b[6] -= (k2[1] * imx);
                b[7] += ((2.0 - k2[0]) * imx);
                b[8] -= (k2[1] * imx);
                imx = (float)src[k[18]];
                b[4] -= (k2[2] * imx);
                b[5] -= (k2[1] * imx);
                b[7] -= (k2[1] * imx);
                b[8] += ((2.0 - k2[0]) * imx);
                imx = (float)src[k[19]];
                b[5] -= (k2[2] * imx);
                b[8] -= (k2[1] * imx);
                imx = (float)src[k[20]];
                b[6] -= (k2[2] * imx);
                imx = (float)src[k[21]];
                b[6] -= (k2[1] * imx);
                b[7] -= (k2[2] * imx);
                imx = (float)src[k[22]];
                b[6] -= (k2[2] * imx);
                b[7] -= (k2[1] * imx);
                b[8] -= (k2[2] * imx);
                imx = (float)src[k[23]];
                b[7] -= (k2[2] * imx);
                b[8] -= (k2[1] * imx);
                imx = (float)src[k[24]];
                b[8] -= (k2[2] * imx);

                if (smode == 3)
                {
                    r[0] = (k1[0] * b[4] + k1[1] * (b[1] + b[3]) + k1[2] * b[0]);
                    r[1] = (k0[0] * b[4] + k0[1] * b[1]);
                    r[2] = (k1[0] * b[4] + k1[1] * (b[1] + b[5]) + k1[2] * b[2]);
                    r[3] = (k0[0] * b[4] + k0[1] * b[3]);
                    r[4] = (float)src[k[12]];
                    r[5] = (k0[0] * b[4] + k0[1] * b[5]);
                    r[6] = (k1[0] * b[4] + k1[1] * (b[7] + b[3]) + k1[2] * b[6]);
                    r[7] = (k0[0] * b[4] + k0[1] * b[7]);
                    r[8] = (k1[0] * b[4] + k1[1] * (b[7] + b[5]) + k1[2] * b[8]);

                    y2 = y * 3;
                    x2 = x * 3;

                    kf = 0;
                    for (i = 0; i < 3; i++)
                    {
                        yf = y2 + i;
                        for (j = 0; j < 3; j++)
                        {
                            xf = x2 + j;
                            ki = (yf * w2 + xf) * channels + d;
                            r[kf] += 0.5f;
                            res[ki] = (unsigned char)((r[kf] < 0.0f) ? 0 : (r[kf] < 255.0f) ? r[kf] : 255);
                            kf++;
                        }
                    }
                }
                else
                {
                    r[0] = (k1[0] * b[4] + k1[1] * (b[1] + b[3]) + k1[2] * b[0]);
                    r[1] = (k1[0] * b[4] + k1[1] * (b[1] + b[5]) + k1[2] * b[2]);
                    r[2] = (k1[0] * b[4] + k1[1] * (b[7] + b[3]) + k1[2] * b[6]);
                    r[3] = (k1[0] * b[4] + k1[1] * (b[7] + b[5]) + k1[2] * b[8]);

                    y2 = y * 2;
                    x2 = x * 2;

                    kf = 0;
                    for (i = 0; i < 2; i++)
                    {
                        yf = y2 + i;
                        for (j = 0; j < 2; j++)
                        {
                            xf = x2 + j;
                            ki = (yf * w2 + xf) * channels + d;
                            r[kf] += 0.5f;
                            res[ki] = (unsigned char)((r[kf] < 0.0f) ? 0 : (r[kf] < 255.0f) ? r[kf] : 255);
                            kf++;
                        }
                    }
                }
            }
        }
    }
}

HRISAPI void ResizeImageMean (unsigned char *src, int height, int width, int channels, int reduce, unsigned char *res)
{
    unsigned int y, x, d, y0, x0, y1, x1, yf, xf;
    size_t k, kf, sum, n;

    if (reduce > 1)
    {
        unsigned int widthr = (width + reduce - 1) / reduce;
        unsigned int heightr = (height + reduce - 1) / reduce;
        for (d = 0; d < channels; d++)
        {
            k = d;
            for (y = 0; y < heightr; y++)
            {
                y0 = y * reduce;
                y1 = y0 + reduce;
                y1 = (y1 < height) ? y1 : height;
                for (x = 0; x < widthr; x++)
                {
                    x0 = x * reduce;
                    x1 = x0 + reduce;
                    x1 = (x1 < width) ? x1 : width;
                    sum = 0;
                    n = 0;
                    for (yf = y0; yf < y1; yf++)
                    {
                        for (xf = x0; xf < x1; xf++)
                        {
                            kf = (yf * width + xf) * channels + d;
                            sum += (size_t)src[kf];
                            n++;
                        }
                    }
                    sum = (n > 0) ? ((sum + n / 2) / n) : 0;
                    res[k] = (unsigned char)((sum < 0) ? 0 : (sum < 255) ? sum : 255);
                    k += channels;
                }
            }
        }
    }
}

#endif /* HRIS_IMPLEMENTATION */

#endif /* HRIS_H_ */
