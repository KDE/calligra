/*
 *  Copyright (c) 2005 Boudewijn Rempt <boud@valdyas.org>
 *
 *  Based on koffice/libs/pigment/KoColorConversions.h
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU Lesser General Public License as published
 *  by the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU Lesser General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
 */

#ifndef _MSOOXML_CONVERSIONS_H_
#define _MSOOXML_CONVERSIONS_H_

#include <qglobal.h>
#include "pigment_export.h"

/**
 * A number of often-used conversions between color models
 */

// 8-bit integer versions. RGBSL are 0-255, H is 0-360.
extern "C" void rgb_to_hsv(int R, int G, int B, int *H, int *S, int *V);
extern "C" void hsv_to_rgb(int H, int S, int V, int *R, int *G, int *B);

// Floating point versions. RGBSL are 0-1, H is 0-360.
void RGBToHSV(float r, float g, float b, float *h, float *s, float *v);
void HSVToRGB(float h, float s, float v, float *r, float *g, float *b);

void RGBToHSL(float r, float g, float b, float *h, float *s, float *l);
void HSLToRGB(float h, float sl, float l, float *r, float *g, float *b);

void rgb_to_hls(quint8 r, quint8 g, quint8 b, float * h, float * l, float * s);

float hue_value(float n1, float n2, float hue);

void hls_to_rgb(float h, float l, float s, quint8 * r, quint8 * g, quint8 * b);

void rgb_to_hls(quint8 r, quint8 g, quint8 b, int * h, int * l, int * s);
void hls_to_rgb(int h, int l, int s, quint8 * r, quint8 * g, quint8 * b);

#endif // _MSOOXML_CONVERSIONS_H_
