/*
https://github.com/heptagonhust/bicubic-image-resize/issues/9
*/

#ifndef GAUSS_H_
#define GAUSS_H_

#include <math.h>

#ifdef GAUSS_STATIC
#define GAUSSAPI static
#else
#define GAUSSAPI extern
#endif

#ifdef __cplusplus
extern "C" {
#endif
GAUSSAPI float GaussBlurFilterY (unsigned char *src, int height, int width, int channels, float radius);
GAUSSAPI float GaussBlurFilterX (unsigned char *src, int height, int width, int channels, float radius);
GAUSSAPI float GaussBlurFilter(unsigned char *src, int height, int width, int channels, float radiusy, float radiusx);
#ifdef __cplusplus
}
#endif

#ifdef GAUSS_IMPLEMENTATION

void GaussLineMatrix (float *cmatrix, float radius)
{
    float std_dev;
    float sum, t, tt;
    int i, j, iradius;

    if (radius < 0)
    {
        radius = -radius;
    }
    std_dev = radius;
    iradius = (int)(2.0f * radius + 0.5f) + 1;
    if (iradius > 1)
    {
        for (i = 0; i < iradius; i++)
        {
            sum = 0;
            for (j = 0; j <= 50; j++)
            {
                t = (float)i;
                t -= 0.5f;
                t += 0.02f * j;
                tt = -(t * t) / (2.0f * std_dev * std_dev);
                sum += exp (tt);
            }
            cmatrix[i] = sum / 50.0f;
        }
        sum = cmatrix[0];
        for (i = 1; i < iradius; i++)
        {
            sum += 2.0f * cmatrix[i];
        }
        for (i = 0; i < iradius; i++)
        {
            cmatrix[i] = cmatrix[i] / sum;
        }
    }
    else
    {
        cmatrix[0] = 1.0f;
    }
}

GAUSSAPI float GaussBlurFilterY (unsigned char *src, int height, int width, int channels, float radius)
{
    int iradius, y, x, yp, yn, i, dval;
    size_t k, kp, kn, line = width * channels;
    unsigned int d;
    float imc, sc, gaussval = 0.0f;
    float *gaussmat;
    unsigned char *temp = NULL;

    if (radius < 0)
    {
        radius = -radius;
    }
    iradius = (int)(2.0f * radius + 0.5f) + 1;

    if (!(gaussmat = (float*)malloc((iradius) * sizeof(float))))
    {
        return 0.0f;
    }
    if (!(temp = (unsigned char*)malloc(height * width * channels * sizeof(unsigned char))))
    {
        return 0.0f;
    }

    GaussLineMatrix (gaussmat, radius);

    if (iradius > 1)
    {
        k = 0;
        for (y = 0; y < height; y++)
        {
            for (x = 0; x < width; x++)
            {
                for (d = 0; d < channels; d++)
                {
                    kp = k;
                    kn = k;
                    imc = (float)src[k + d];
                    sc = imc * gaussmat[0];
                    for (i = 1; i < iradius; i++)
                    {
                        yp = y - i;
                        if (yp < 0)
                        {
                            yp = 0;
                        }
                        else
                        {
                            kp -= line;
                        }
                        yn = y + i;
                        if (yn < height)
                        {
                            kn += line;
                        }
                        else
                        {
                            yn = 0;
                        }
                        imc = (float)src[kp + d];
                        sc += imc * gaussmat[i];
                        imc = (float)src[kn + d];
                        sc += imc * gaussmat[i];
                    }
                    sc += 0.5f;
                    sc = (sc < 0.0f) ? 0.0f : (sc < 255.0f) ? sc : 255.0f;
                    temp[k + d] = (unsigned char)sc;
                }
                k += channels;
            }
        }
        k = 0;
        for (y = 0; y < height; y++)
        {
            for (x = 0; x < width; x++)
            {
                for (d = 0; d < channels; d++)
                {
                    dval = (int)src[k] - (int)temp[k];
                    gaussval += (dval < 0) ? -dval : dval;
                    src[k] = temp[k];
                    k++;
                }
            }
        }
        gaussval /= (float)k;
    }

    free(temp);
    free(gaussmat);

    return gaussval;
}

GAUSSAPI float GaussBlurFilterX (unsigned char *src, int height, int width, int channels, float radius)
{
    int iradius, y, x, xp, xn, i, dval;
    size_t k, kp, kn;
    unsigned int d;
    float imc, sc, gaussval = 0.0f;
    float *gaussmat;
    unsigned char *temp = NULL;

    if (radius < 0)
    {
        radius = -radius;
    }
    iradius = (int)(2.0f * radius + 0.5f) + 1;

    if (!(gaussmat = (float*)malloc((iradius) * sizeof(float))))
    {
        return 0.0f;
    }
    if (!(temp = (unsigned char*)malloc(height * width * channels * sizeof(unsigned char))))
    {
        return 0.0f;
    }

    GaussLineMatrix (gaussmat, radius);

    if (iradius > 1)
    {
        k = 0;
        for (y = 0; y < height; y++)
        {
            for (x = 0; x < width; x++)
            {
                for (d = 0; d < channels; d++)
                {
                    kp = k;
                    kn = k;
                    imc = (float)src[k + d];
                    sc = imc * gaussmat[0];
                    for (i = 1; i < iradius; i++)
                    {
                        xp = x - i;
                        if (xp < 0)
                        {
                            xp = 0;
                        }
                        else
                        {
                            kp -= channels;
                        }
                        xn = x + i;
                        if (xn < width)
                        {
                            kn += channels;
                        }
                        else
                        {
                            xn = 0;
                        }
                        imc = (float)src[kp + d];
                        sc += imc * gaussmat[i];
                        imc = (float)src[kn + d];
                        sc += imc * gaussmat[i];
                    }
                    sc += 0.5f;
                    sc = (sc < 0.0f) ? 0.0f : (sc < 255.0f) ? sc : 255.0f;
                    temp[k + d] = (unsigned char)sc;
                }
                k += channels;
            }
        }
        k = 0;
        for (y = 0; y < height; y++)
        {
            for (x = 0; x < width; x++)
            {
                for (d = 0; d < channels; d++)
                {
                    dval = (int)src[k] - (int)temp[k];
                    gaussval += (dval < 0) ? -dval : dval;
                    src[k] = temp[k];
                    k++;
                }
            }
        }
        gaussval /= (float)k;
    }

    free(temp);
    free(gaussmat);

    return gaussval;
}

GAUSSAPI float GaussBlurFilter(unsigned char *src, int height, int width, int channels, float radiusy, float radiusx)
{
    float gaussval = 0.0f;
    gaussval = GaussBlurFilterY(src, height, width, channels, radiusy);
    gaussval += GaussBlurFilterX(src, height, width, channels, radiusx);
    gaussval /= 255.0f;

    return gaussval;
}

#endif /* GAUSS_IMPLEMENTATION */

#endif /* GAUSS_H_ */
