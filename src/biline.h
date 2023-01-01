/*
https://github.com/heptagonhust/bicubic-image-resize/issues/9
*/

#ifndef BILINE_H_
#define BILINE_H_

void BiLineFilter(unsigned char *pix, unsigned char *src, int height, int width, int channels, float y, float x)
{
    int d, xi, yi, xf, yf;
    float dx1, dy1, dx2, dy2;
    float sum[8] = {0.0f};
    size_t k[4];

    yi = (int)y;
    yi = (yi < 0) ? 0 : (yi < height) ? yi : (height - 1);
    xi = (int)x;
    xi = (xi < 0) ? 0 : (xi < width) ? xi : (width - 1);
    dy1 = y - yi;
    dx1 = x - xi;
    dy2 = 1.0 - dy1;
    dx2 = 1.0 - dx1;
    yf = (int)y + 1;
    yf = (yf < 0) ? 0 : (yf < height) ? yf : (height - 1);
    xf = (int)x + 1;
    xf = (xf < 0) ? 0 : (xf < width) ? xf : (width - 1);
    k[0] = (yi * width + xi) * channels;
    k[1] = (yi * width + xf) * channels;
    k[2] = (yf * width + xi) * channels;
    k[3] = (yf * width + xf) * channels;
    for (int d = 0; d < channels; d++)
    {
        sum[d] = (dy2 * (dx2 * (float)src[k[0] + d] + dx1 * (float)src[k[1] + d]) + dy1 * (dx2 * (float)src[k[2] + d] + dx1 * (float)src[k[3] + d]) + 0.5f);
        pix[d] = (unsigned char)((sum[d] < 0.0f) ? 0 : (sum[d] < 255.0f) ? sum[d] : 255);
    }
}


void ResizeImageBiLine(unsigned char *src, int height, int width, int channels, float ratio, unsigned char *res)
{
    int resize_height = (int)((float)height * ratio + 0.5f);
    int resize_width = (int)((float)width * ratio + 0.5f);
    unsigned char pix[8];

    size_t k = 0;
    for (int i = 0; i < resize_height; i++)
    {
        float src_y = ((float)i + 0.5f) / ratio - 0.5f;
        for (int j = 0; j < resize_width; j++)
        {
            float src_x = ((float)j + 0.5f) / ratio - 0.5f;
            BiLineFilter(pix, src, height, width, channels, src_y, src_x);
            for (int d = 0; d < channels; d++)
            {
                res[k] = pix[d];
                k++;
            }
        }
    }
}

#endif /* BILINE_H_ */
