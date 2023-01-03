/*
Reverse Interpolate Scale (RIS)
https://github.com/ImageProcessing-ElectronicPublications/imthreshold
*/

#ifndef RIS_H_
#define RIS_H_

#ifdef RIS_STATIC
#define RISAPI static
#else
#define RISAPI extern
#endif

#ifdef __cplusplus
extern "C" {
#endif
RISAPI float ImageReFilter(unsigned char *src, unsigned char *res, int height, int width, int channels, float pris);
#ifdef __cplusplus
}
#endif

#ifdef RIS_IMPLEMENTATION

RISAPI float ImageReFilter(unsigned char *src, unsigned char *res, int height, int width, int channels, float pris)
{
    unsigned d;
    float im, imf, imd, ims = 0.0f;

    size_t k = 0;
    for (int i = 0; i < height; i++)
    {
        for (int j = 0; j < width; j++)
        {
            for (int d = 0; d < channels; d++)
            {
                im = (float)src[k];
                imf = (float)res[k];
                imd = (im - imf) * pris;
                ims += (imd < 0) ? -imd : imd;
                imd += (im + 0.5f);
                res[k] = (unsigned char)((imd < 0.0f) ? 0 : (imd < 255.0f) ? imd : 255);
                k++;
            }
        }
    }
    ims /= (float)height;
    ims /= (float)width;
    ims /= (float)channels;

    return ims;
}

#endif /* RIS_IMPLEMENTATION */

#endif /* RIS_H_ */
