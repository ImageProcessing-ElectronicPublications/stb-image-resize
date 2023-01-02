/*
https://github.com/heptagonhust/bicubic-image-resize/issues/9
*/

#ifndef GSAMPLE_H_
#define GSAMPLE_H_

void ResizeImageGSample(unsigned char *src, int height, int width, int channels, int resize_height, int resize_width, unsigned char *res)
{
    unsigned int d;
    float xFactor = (float)width / (float)resize_width;
    float yFactor = (float)height / (float)resize_height;
    float y0, x0, yn, xn, y0t, x0t, ynt, xnt, yt, xt, dyt, dxt;
    float fyt, fxt, s, ss[8], sim[8], gy, gx, gc;
    int y, x, y0i, x0i, yni, xni, i, j, yti, xti, ypr, xpr, ynx, xnx;
    size_t ky[3], k[9];

    for (y = 0; y < resize_height; y++)
    {
        y0 = (float)y;
        y0 *= yFactor;
        y0i = (int)y0;
        yn = y0 + yFactor;
        yni = (int)yn;
        if ((float)yni < yn)
        {
            yni++;
        }
        for (x = 0; x < resize_width; x++)
        {
            x0 = (float)x;
            x0 *= xFactor;
            x0i = (int)x0;
            xn = x0 + xFactor;
            xni = (int)xn;
            if ((float)xni < xn)
            {
                xni++;
            }
            for (d = 0; d < channels; d++)
            {
                sim[d] = 0.0f;
                ss[d] = 0.0f;
            }
            for (i = y0i; i < yni; i++)
            {
                y0t = (float)i;
                if (y0t < y0)
                {
                    y0t = y0;
                }
                ynt = (float)(i + 1);
                if (ynt > yn)
                {
                    ynt = yn;
                }
                yt = (y0t + ynt) * 0.5f;
                yti = (int)yt;
                if (yti >= height)
                {
                    yti = height - 1;
                }
                dyt = ynt - y0t;
                fyt = yt - yti - 0.5f;
                ypr = yti - 1;
                if (ypr < 0)
                {
                    ypr = 0;
                }
                ynx = yti + 1;
                if (ynx >= height)
                {
                    ynx = height - 1;
                }
                ky[0] = ypr * width;
                ky[1] = yti * width;
                ky[2] = ynx * width;
                for (j = x0i; j < xni; j++)
                {
                    x0t = (float)j;
                    if (x0t < x0)
                    {
                        x0t = x0;
                    }
                    xnt = (float)(j + 1.0f);
                    if (xnt > xn)
                    {
                        xnt = xn;
                    }
                    xt = (x0t + xnt) * 0.5f;
                    xti = (int)xt;
                    if (xti >= width)
                    {
                        xti = width - 1;
                    }
                    dxt = xnt - x0t;
                    fxt = xt - xti - 0.5f;
                    xpr = xti - 1;
                    if (xpr < 0)
                    {
                        xpr = 0;
                    }
                    xnx = xti + 1;
                    if (xnx >= width)
                    {
                        xnx = width - 1;
                    }
                    k[0] = (ky[0] + xpr) * channels;
                    k[1] = (ky[0] + xti) * channels;
                    k[2] = (ky[0] + xnx) * channels;
                    k[3] = (ky[1] + xpr) * channels;
                    k[4] = (ky[1] + xti) * channels;
                    k[5] = (ky[1] + xnx) * channels;
                    k[6] = (ky[2] + xpr) * channels;
                    k[7] = (ky[2] + xti) * channels;
                    k[8] = (ky[2] + xnx) * channels;
                    for (d = 0; d < channels; d++)
                    {
                        gy = (float)src[k[7] + d];
                        gy += (float)src[k[7] + d];
                        gy += (float)src[k[8] + d];
                        gy += (float)src[k[6] + d];
                        gy -= (float)src[k[1] + d];
                        gy -= (float)src[k[1] + d];
                        gy -= (float)src[k[2] + d];
                        gy -= (float)src[k[0] + d];
                        gy /= 8.0f;
                        gx = (float)src[k[5] + d];
                        gx += (float)src[k[5] + d];
                        gx += (float)src[k[8] + d];
                        gx += (float)src[k[2] + d];
                        gx -= (float)src[k[3] + d];
                        gx -= (float)src[k[3] + d];
                        gx -= (float)src[k[0] + d];
                        gx -= (float)src[k[6] + d];
                        gx /= 8.0f;
                        gc = (float)src[k[4] + d];
                        gc += fyt * gy;
                        gc += fxt * gx;
                        s = dxt * dyt;
                        ss[d] += s;
                        sim[d] += (gc * s);
                    }
                }
            }
            for (d = 0; d < channels; d++)
            {
                if (ss[d] < 0.0f || ss[d] > 0.0f)
                {
                    sim[d] /= ss[d];
                }
                sim[d] += 0.5f;
                res[(y * resize_width + x) * channels + d] = (unsigned char)((sim[d] < 0.0f) ? 0 : (sim[d] < 255.0f) ? sim[d] : 255);
            }
        }
    }
}

#endif /* GSAMPLE_H_ */
