/*
https://github.com/heptagonhust/bicubic-image-resize/issues/10
*/

#ifndef BIAKIMA_H_
#define BIAKIMA_H_

void BiAkimaFilter(unsigned char *pix, unsigned char *src, int height, int width, int channels, float y, float x)
{
    int i, j, d, xi, yi, xf, yf;
    float dx, dy, zp, zf, a, b;
    float Cc, C[6], m[6], t[2];
    int Ci, pt[8];

    yi = (int)y;
    dy = y - yi;
    yi = (yi < 0) ? 0 : (yi < height) ? yi : (height - 1);
    xi = (int)x;
    dx = x - xi;
    xi = (xi < 0) ? 0 : (xi < width) ? xi : (width - 1);
    for(d = 0; d < channels; d++)
    {
        if (dy > 0.0f)
        {
            for(i = -2; i < 4; i++)
            {
                yf = (int)y + i;
                yf = (yf < 0) ? 0 : (yf < height) ? yf : (height - 1);
                if (dx > 0.0f)
                {
                    zp = 0.0f;
                    for(j = -2; j < 4; j++)
                    {
                        xf = (int)x + j;
                        xf = (xf < 0) ? 0 : (xf < width) ? xf : (width - 1);
                        zf = src[(width * yf + xf) * channels + d];
                        m[j + 2] = zf - zp;
                        zp = zf;
                    }
                    for(j = 0; j < 2; j++)
                    {
                        a = (m[j + 3] > m[j + 4]) ? (m[j + 3] - m[j + 4]) : (m[j + 4] - m[j + 3]);
                        b = (m[j + 2] > m[j + 1]) ? (m[j + 2] - m[j + 1]) : (m[j + 1] - m[j + 2]);
                        t[j] = ((a + b) > 0) ? ((a * m[j + 2] + b * m[j + 3]) / (a + b)) : (0.5f * (m[j + 2] + m[j + 3]));
                    }
                    zf = src[(width * yf + xi) * channels + d];
                    C[i + 2] = zf + (t[0] + ((m[3] + m[3] + m[3] - t[0] - t[0] - t[1]) + (t[0] + t[1] - m[3] - m[3]) * dx) * dx) * dx;
                }
                else
                {
                    xf = xi;
                    C[i + 2] = src[(width * yf + xf) * channels + d];
                }
            }
            zp = 0.0f;
            for(i = -2; i < 4; i++)
            {
                m[i + 2] = C[i + 2] - zp;
                zp = C[i + 2];
            }
            for(i = 0; i < 2; i++)
            {
                a = (m[i + 3] > m[i + 4]) ? (m[i + 3] - m[i + 4]) : (m[i + 4] - m[i + 3]);
                b = (m[i + 2] > m[i + 1]) ? (m[i + 2] - m[i + 1]) : (m[i + 1] - m[i + 2]);
                t[i] = ((a + b) > 0) ? ((a * m[i + 2] + b * m[i + 3]) / (a + b)) : (0.5f * (m[i + 2] + m[i + 3]));
            }
            Cc = C[2] + (t[0] + ((m[3] + m[3] + m[3] - t[0] - t[0] - t[1]) + (t[0] + t[1] - m[3] - m[3]) * dy) * dy) * dy;
        }
        else
        {
            yf = yi;
            if (dx > 0.0f)
            {
                zp = 0.0f;
                for(j = -2; j < 4; j++)
                {
                    xf = (int)x + j;
                    xf = (xf < 0) ? 0 : (xf < width) ? xf : (width - 1);
                    zf = src[(width * yf + xf) * channels + d];
                    m[j + 2] = zf - zp;
                    zp = zf;
                }
                for(j = 0; j < 2; j++)
                {
                    a = (m[j + 3] > m[j + 4]) ? (m[j + 3] - m[j + 4]) : (m[j + 4] - m[j + 3]);
                    b = (m[j + 2] > m[j + 1]) ? (m[j + 2] - m[j + 1]) : (m[j + 1] - m[j + 2]);
                    t[j] = ((a + b) > 0) ? ((a * m[j + 2] + b * m[j + 3]) / (a + b)) : (0.5f * (m[j + 2] + m[j + 3]));
                }
                zf = src[(width * yf + xi) * channels + d];
                Cc = zf + (t[0] + ((m[3] + m[3] + m[3] - t[0] - t[0] - t[1]) + (t[0] + t[1] - m[3] - m[3]) * dx) * dx) * dx;
            }
            else
            {
                xf = xi;
                Cc = src[(width * yf + xf) * channels + d];
            }
        }
        Ci = (int)(Cc + 0.5f);
        pt[d] = (Ci < 0) ? 0 : (Ci > 255) ? 255 : Ci;
    }
    for (int d = 0; d < channels; d++)
    {
        pix[d] = (unsigned char)((pt[d] < 0.0f) ? 0 : (pt[d] < 255.0f) ? pt[d] : 255);
    }
}

void ResizeImageBiAkima(unsigned char *src, int height, int width, int channels, float ratio, unsigned char *res)
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
            BiAkimaFilter(pix, src, height, width, channels, src_y, src_x);
            for (int d = 0; d < channels; d++)
            {
                res[k] = pix[d];
                k++;
            }
        }
    }
}

#endif /* BIAKIMA_H_ */
