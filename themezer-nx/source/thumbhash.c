/*
 * ThumbHash decoder adapted from datocms/fast_thumbhash.
 *
 * Original ThumbHash algorithm by Evan Wallace:
 * https://github.com/evanw/thumbhash
 *
 * Adapted from:
 * https://github.com/datocms/fast_thumbhash
 *
 * MIT License
 *
 * Copyright (c) 2023 Stefano Verna
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

#include "thumbhash.h"

#include <math.h>
#include <stdlib.h>

static double GetAspectRatio(const uint8_t *hash){
    int hasAlpha = (hash[2] & 0x80) != 0;
    int major = hasAlpha ? 5 : 7;
    int minor = hash[3] & 7;
    int isLandscape = (hash[4] & 0x80) != 0;
    int lx = isLandscape ? major : minor;
    int ly = isLandscape ? minor : major;

    return (double)lx / (double)ly;
}

static int ClampByte(double value){
    if (value < 0.0)
        return 0;
    if (value > 1.0)
        return 255;
    return (int)lround(value * 255.0);
}

static double *DecodeChannel(int nx, int ny, double scale, const uint8_t *hash, int acStart, int *acIndex){
    double *ac = calloc((size_t)nx * (size_t)ny, sizeof(double));
    int outIndex = 0;

    if (!ac)
        return NULL;

    for (int cy = 0; cy < ny; cy++){
        for (int cx = cy ? 0 : 1; cx * ny < nx * (ny - cy); cx++){
            double value = (hash[acStart + (*acIndex >> 1)] >> (((*acIndex)++ & 1) << 2)) & 15;
            ac[outIndex++] = (value / 7.5 - 1.0) * scale;
        }
    }

    return ac;
}

int ThumbHashToRGBA(const uint8_t *hash, size_t hashLen, int maxSize, uint8_t **rgba, int *width, int *height){
    if (!hash || !rgba || !width || !height || hashLen < 5 || maxSize <= 0)
        return 0;

    double ratio = GetAspectRatio(hash);
    *width = (int)lround(ratio > 1.0 ? maxSize : maxSize * ratio);
    *height = (int)lround(ratio > 1.0 ? maxSize / ratio : maxSize);

    if (*width < 1)
        *width = 1;
    if (*height < 1)
        *height = 1;

    uint32_t header24 = hash[0] | (hash[1] << 8) | (hash[2] << 16);
    uint32_t header16 = hash[3] | (hash[4] << 8);
    double lDc = (double)(header24 & 63) / 63.0;
    double pDc = (double)((header24 >> 6) & 63) / 31.5 - 1.0;
    double qDc = (double)((header24 >> 12) & 63) / 31.5 - 1.0;
    double lScale = (double)((header24 >> 18) & 31) / 31.0;
    int hasAlpha = (header24 >> 23) != 0;
    double pScale = (double)((header16 >> 3) & 63) / 63.0;
    double qScale = (double)((header16 >> 9) & 63) / 63.0;
    int isLandscape = (header16 >> 15) != 0;
    int lx = isLandscape ? (hasAlpha ? 5 : 7) : (header16 & 7);
    int ly = isLandscape ? (header16 & 7) : (hasAlpha ? 5 : 7);
    double aDc = hasAlpha && hashLen >= 6 ? (double)(hash[5] & 15) / 15.0 : 1.0;
    double aScale = hasAlpha && hashLen >= 6 ? (double)(hash[5] >> 4) / 15.0 : 0.0;
    int acStart = hasAlpha ? 6 : 5;
    int acIndex = 0;

    if (lx < 3)
        lx = 3;
    if (ly < 3)
        ly = 3;
    if (hasAlpha && hashLen < 6)
        return 0;

    double *lAc = DecodeChannel(lx, ly, lScale, hash, acStart, &acIndex);
    double *pAc = DecodeChannel(3, 3, pScale * 1.25, hash, acStart, &acIndex);
    double *qAc = DecodeChannel(3, 3, qScale * 1.25, hash, acStart, &acIndex);
    double *aAc = hasAlpha ? DecodeChannel(5, 5, aScale, hash, acStart, &acIndex) : NULL;
    uint8_t *pixels = malloc((size_t)(*width) * (size_t)(*height) * 4);

    if (!lAc || !pAc || !qAc || !pixels || (hasAlpha && !aAc)){
        free(lAc);
        free(pAc);
        free(qAc);
        free(aAc);
        free(pixels);
        return 0;
    }

    double fx[7] = {0};
    double fy[7] = {0};
    size_t out = 0;
    for (int y = 0; y < *height; y++){
        double py = ((double)y + 0.5) / *height;
        for (int x = 0; x < *width; x++){
            double px = ((double)x + 0.5) / *width;
            double l = lDc;
            double p = pDc;
            double q = qDc;
            double a = aDc;

            for (int cx = 0; cx < lx && cx < 7; cx++)
                fx[cx] = cos(M_PI * px * cx);
            for (int cy = 0; cy < ly && cy < 7; cy++)
                fy[cy] = cos(M_PI * py * cy);

            for (int cy = 0, j = 0; cy < ly; cy++){
                double fy2 = fy[cy] * 2.0;
                for (int cx = cy ? 0 : 1; cx * ly < lx * (ly - cy); cx++, j++)
                    l += lAc[j] * fx[cx] * fy2;
            }

            for (int cy = 0, j = 0; cy < 3; cy++){
                double fy2 = fy[cy] * 2.0;
                for (int cx = cy ? 0 : 1; cx < 3 - cy; cx++, j++){
                    double factor = fx[cx] * fy2;
                    p += pAc[j] * factor;
                    q += qAc[j] * factor;
                }
            }

            if (hasAlpha){
                for (int cy = 0, j = 0; cy < 5; cy++){
                    double fy2 = fy[cy] * 2.0;
                    for (int cx = cy ? 0 : 1; cx < 5 - cy; cx++, j++)
                        a += aAc[j] * fx[cx] * fy2;
                }
            }

            double b = l - 2.0 / 3.0 * p;
            double r = (3.0 * l - b + q) / 2.0;
            double g = r - q;

            pixels[out++] = ClampByte(r);
            pixels[out++] = ClampByte(g);
            pixels[out++] = ClampByte(b);
            pixels[out++] = ClampByte(a);
        }
    }

    free(lAc);
    free(pAc);
    free(qAc);
    free(aAc);

    *rgba = pixels;
    return 1;
}