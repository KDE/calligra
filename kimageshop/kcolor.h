/*
 *  kcolor.h - part of KImageShop
 *
 *  Copyright (c) 1999 Matthias Elter <me@kde.org>
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */

#ifndef __kcolor_h__
#define __kcolor_h__

typedef unsigned char uchar;

class KColor
{
 public:
  enum colorModel { RGB, HSV, LAB, CMYK };

  KColor();
  ~KColor();
  KColor(int a, int b, int c,  colorModel m = RGB);
  KColor(int c, int m, int y,  int k);
  KColor &operator=( const KColor & );

  void setRGB (uchar R, uchar G, uchar B);
  void setHSV (int H, uchar S, uchar V);
  void setLAB (int L, int a, int b);
  void setCMYK (uchar C, uchar M, uchar Y, uchar K);

  void rgb (uchar *R, uchar *G, uchar *B);
  void hsv (int *H, uchar *S, uchar *V);
  void lab (int *L, int *a, int *b);
  void cmyk (uchar *C, uchar *M, uchar *Y, uchar *K);
  
  static void RGBtoHSV(uchar R, uchar G, uchar B, int *H, uchar *S, uchar *V);
  static void RGBtoLAB(uchar R, uchar G, uchar B, int *L, int *a, int *b);
  static void RGBtoCMYK(uchar R, uchar G, uchar B, uchar *C, uchar *M, uchar *Y, uchar *K);

  static void HSVtoRGB(int H, uchar S, uchar V, uchar *R, uchar *G, uchar *B);
  static void HSVtoLAB(int H, uchar S, uchar V, int *L, int *a, int *b);
  static void HSVtoCMYK(int H, uchar S, uchar V, uchar *C, uchar *M, uchar *Y, uchar*K);

  static void LABtoRGB(int L, int a, int b, uchar *R, uchar *G, uchar *B);
  static void LABtoHSV(int L, int a, int b, int *H, uchar *S, uchar *V);
  static void LABtoCMYK(int L, int a, int b, uchar *C, uchar *M, uchar *Y, uchar*K);

  static void CMYKtoRGB(uchar C, uchar M, uchar Y, uchar K, uchar *R, uchar *G, uchar *B);
  static void CMYKtoHSV(uchar C, uchar M, uchar Y, uchar K, int *H, uchar *S, uchar *V);
  static void CMYKtoLAB(uchar C, uchar M, uchar Y, uchar K, int *L, int *a, int *b);

  static const KColor black();
  static const KColor white();
  static const KColor gray();
  static const KColor lightGray();
  static const KColor darkGray();
  static const KColor red();
  static const KColor darkRed();
  static const KColor green();
  static const KColor darkGreen();
  static const KColor blue();
  static const KColor darkBlue();
  static const KColor cyan();
  static const KColor darkCyan();
  static const KColor magenta();
  static const KColor darkMagenta();
  static const KColor yellow();
  static const KColor darkYellow();

 protected:
  void calcRGB();
  void calcHSV();
  void calcLAB();
  void calcCMYK();

 private:
  uchar m_R, m_G, m_B;      // RGB
  uchar m_C, m_M, m_Y, m_K; // CMYK
  int m_H; uchar m_S, m_V;  // HSV
  int m_L, m_a, m_b;        // LAB

  colorModel m_nativeModel;
};

inline const KColor KColor::white()
{ return KColor(255,255,255,RGB); }

inline const KColor KColor::black()
{ return KColor(0,0,0,RGB); }

inline const KColor KColor::gray()
{ return KColor(160,160,164,RGB); }

inline const KColor KColor::lightGray()
{ return KColor(192,192,192,RGB); }

inline const KColor KColor::darkGray()
{ return KColor(128,128,128,RGB); }

inline const KColor KColor::red()
{ return KColor(255,0,0,RGB); }

inline const KColor KColor::darkRed()
{ return KColor(128,0,0,RGB); }

inline const KColor KColor::green()
{ return KColor(0,255,0,RGB); }

inline const KColor KColor::darkGreen()
{ return KColor(0,128,0,RGB); }

inline const KColor KColor::blue()
{ return KColor(0,0,255,RGB); }

inline const KColor KColor::darkBlue()
{ return KColor(0,0,128,RGB); }

inline const KColor KColor::cyan()
{ return KColor(0,255,255,RGB); }

inline const KColor KColor::darkCyan()
{ return KColor(0,128,128,RGB); }

inline const KColor KColor::magenta()
{ return KColor(255,0,255,RGB); }

inline const KColor KColor::darkMagenta()
{ return KColor(128,0,128,RGB); }

inline const KColor KColor::yellow()
{ return KColor(255,255,0,RGB); }

inline const KColor KColor::darkYellow()
{ return KColor(128,128,0,RGB); }

#endif
