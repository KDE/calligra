/*
 *  color.h - part of KImageShop
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

#ifndef __color_h__
#define __color_h__

#include <qcolor.h>

enum colorModel { INDEXED, RGB, HSV, LAB, CMYK };

class KColor
{
 public:

  KColor();
  KColor(int a, int b, int c,  colorModel m = RGB);
  KColor(int c, int m, int y,  int k);
  KColor(const QColor&);

  void setRGB (int R, int G, int B);
  void setHSV (int H, int S, int V);
  void setLAB (int L, int a, int b);
  void setCMYK (int C, int M, int Y, int K);
  void setColor(const QColor&);

  void rgb (int *R, int *G, int *B) const;
  void hsv (int *H, int *S, int *V) const;
  void lab (int *L, int *a, int *b) const;
  void cmyk (int *C, int *M, int *Y, int *K) const;
  QColor color() const;

  colorModel native() const { return m_native; }

  int R() const;
  int G() const;
  int B() const;
  int h() const;
  int s() const;
  int v() const;
  int l() const;
  int a() const;
  int b() const;
  int c() const;
  int m() const;
  int y() const;
  int k() const;
  
  static void RGBtoHSV(int R, int G, int B, int *H, int *S, int *V);
  static void RGBtoLAB(int R, int G, int B, int *L, int *a, int *b);
  static void RGBtoCMYK(int R, int G, int B, int *C, int *M, int *Y, int *K);

  static void HSVtoRGB(int H, int S, int V, int *R, int *G, int *B);
  static void HSVtoLAB(int H, int S, int V, int *L, int *a, int *b);
  static void HSVtoCMYK(int H, int S, int V, int *C, int *M, int *Y, int*K);

  static void LABtoRGB(int L, int a, int b, int *R, int *G, int *B);
  static void LABtoHSV(int L, int a, int b, int *H, int *S, int *V);
  static void LABtoCMYK(int L, int a, int b, int *C, int *M, int *Y, int*K);

  static void CMYKtoRGB(int C, int M, int Y, int K, int *R, int *G, int *B);
  static void CMYKtoHSV(int C, int M, int Y, int K, int *H, int *S, int *V);
  static void CMYKtoLAB(int C, int M, int Y, int K, int *L, int *a, int *b);

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
  void calcRGB() const;
  void calcHSV() const;
  void calcCMYK() const;
  void calcLAB() const;

  void rgbChanged() const;
  void hsvChanged() const;
  void cmykChanged() const;
  void labChanged() const;

 private:
  /*
   * Mutable to make it possible for const objects to transform the native cModel
   * in functions like KColor::rgb(...) to the requested.
   */
  mutable int m_R, m_G, m_B;      // RGB
  mutable int m_C, m_M, m_Y, m_K; // CMYK
  mutable int m_H, m_S, m_V;      // HSV
  mutable int m_L, m_a, m_b;      // LAB

  mutable bool m_RGBvalid;
  mutable bool m_HSVvalid;
  mutable bool m_CMYKvalid;
  mutable bool m_LABvalid;

  colorModel m_native; 
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
