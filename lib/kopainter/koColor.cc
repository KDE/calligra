/* This file is part of the KDE project
  Copyright (c) 1999 Matthias Elter (me@kde.org)
  Copyright (c) 2001-2002 Igor Janssen (rm@kde.org)

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License as published by the Free Software Foundation; either
   version 2 of the License, or (at your option) any later version.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this library; see the file COPYING.LIB.  If not, write to
   the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
   Boston, MA 02111-1307, USA.
*/

#include "koColor.h"

#include <math.h>

KoColor::KoColor()
{
  // initialise to black
  m_native = cs_RGB;
  /* RGB */
  m_R = 0;
  m_G = 0;
  m_B = 0;
  /* HSV */
  m_H =  m_V = 0;
  m_S = 100;
  /* CMYK */
  m_C = 0;
  m_Y = 0;
  m_M = 0;
  m_K = 0;
  /* Lab */
  m_L = 0;
  m_a = 0;
  m_b = 0;
}

KoColor::KoColor(int a, int b, int c, cSpace m)
{
  switch(m)
  {
  case cs_RGB:
    m_R = a;
    m_G = b;
    m_B = c;
    m_native = cs_RGB;
    rgbChanged();
    break;
  case cs_HSV:
    m_H = a;
    m_S = b;
    m_V = c;
    m_native = cs_HSV;
    hsvChanged();
    break;
  case cs_Lab:
    m_L = a;
    m_a = b;
    m_b = c;
    m_native = cs_Lab;
    labChanged();
    break;
  default:
    m_R = 0;
    m_G = 0;
    m_B = 0;
    m_native = cs_RGB;
    rgbChanged();
  }
}

KoColor::KoColor(int c, int m, int y, int k)
{
  m_C = c;
  m_M = m;
  m_Y = y;
  m_K = k;
  m_native = cs_CMYK;
  cmykChanged();
}

KoColor::KoColor(const QColor &c)
{
  m_R = c.red();
  m_G = c.green();
  m_B = c.blue();
  m_native = cs_RGB;
  rgbChanged();
}

KoColor::KoColor(const QString &name)
{

}

int KoColor::R() const
{
  if(!m_RGBvalid)
    calcRGB();
  return m_R;
}

int KoColor::G() const
{
  if(!m_RGBvalid)
    calcRGB();
  return m_G;
}

int KoColor::B() const
{
  if(!m_RGBvalid)
    calcRGB();
  return m_B;
}

int KoColor::H() const
{
  if(!m_HSVvalid)
    calcHSV();
  return m_H;
}

int KoColor::S() const
{
  if(!m_HSVvalid)
    calcHSV();
  return m_S;
}

int KoColor::V() const
{
  if(!m_HSVvalid)
    calcHSV();
  return m_V;
}

int KoColor::L() const
{
  if(!m_LABvalid)
    calcLAB();
  return m_L;
}

int KoColor::a() const
{
  if(!m_LABvalid)
    calcLAB();
  return m_a;
}

int KoColor::b() const
{
  if(!m_LABvalid)
    calcLAB();
  return m_b;
}

int KoColor::C() const
{
  if(!m_CMYKvalid)
    calcCMYK();
  return m_C;
}

int KoColor::M() const
{
  if(!m_CMYKvalid)
    calcCMYK();
  return m_M;
}

int KoColor::Y() const
{
  if(!m_CMYKvalid)
    calcCMYK();
  return m_Y;
}

int KoColor::K() const
{
  if(!m_CMYKvalid)
    calcCMYK();
  return m_K;
}

void KoColor::rgb(int *R, int *G, int *B) const
{
  if(!m_RGBvalid)
    calcRGB();
  
  *R = m_R;
  *G = m_G;
  *B = m_B;
}

void KoColor::hsv(int *H, int *S, int *V) const
{
  if(!m_HSVvalid)
    calcHSV();
  
  *H = m_H;
  *S = m_S;
  *V = m_V;
}

void KoColor::lab(int *L, int *a, int *b) const
{
  if(!m_LABvalid)
    calcLAB();
  
  *L = m_L;
  *a = m_a;
  *b = m_b;
}

void KoColor::cmyk(int *C, int *M, int *Y, int *K) const
{
  if(!m_CMYKvalid)
    calcCMYK();
  
  *C = m_C;
  *M = m_M;
  *Y = m_Y;
  *K = m_K;
}

QString KoColor::name() const
{
  QString s;
  switch(m_native)
  {
  case cs_RGB:
    s.sprintf("#%02x%02x%02x", R(), G(), B());
    break;
  case cs_HSV:
    s.sprintf("$%02x%02x%02x", H(), S(), V());
    break;
  case cs_CMYK:
    s.sprintf("@%02x%02x%02x", C(), M(), Y(), K());
    break;
  case cs_Lab:
    s.sprintf("*%02x%02x%02x", L(), a(), b());
    break;
  default:
    s.sprintf("#%02x%02x%02x", R(), G(), B());
  }
  return s;
}

QColor KoColor::color() const
{
  if(!m_RGBvalid)
    calcRGB();
  return QColor(m_R, m_G, m_B);
}

void KoColor::setRGB(int R, int G, int B)
{
  m_R = R;
  m_G = G;
  m_B = B;
  m_native = cs_RGB;
  rgbChanged();
}

void KoColor::setHSV(int H, int S, int V)
{
  m_H = H;
  m_S = S;
  m_V = V;
  m_native = cs_HSV;
  hsvChanged();
}

void KoColor::setLab(int L, int a, int b)
{
  m_L = L;
  m_a = a;
  m_b = b;
  m_native = cs_Lab;
  labChanged();
}

void KoColor::setCMYK(int C, int M, int Y, int K)
{
  m_C = C;
  m_M = M;
  m_Y = Y;
  m_K = K;
  m_native = cs_CMYK;
  cmykChanged();
}

void KoColor::setNamedColor(const QString &name)
{

}

void KoColor::setColor(const QColor &c)
{
  m_R = c.red();
  m_G = c.green();
  m_B = c.blue();
  m_native = cs_RGB;
  rgbChanged();
}

void KoColor::RGBtoHSV(int R, int G, int B, int *H, int *S, int *V)
{
  unsigned int max = R;
  unsigned int min = R;
  unsigned char maxValue = 0; // r=0, g=1, b=2
  
  // find maximum and minimum RGB values
  if(static_cast<unsigned int>(G) > max) { max = G; maxValue = 1; }
  if(static_cast<unsigned int>(B) > max) { max = B; maxValue = 2; }
  
  if(static_cast<unsigned int>(G) < min) min = G;
  if(static_cast<unsigned int>(B) < min ) min = B;
  
  int delta = max - min;
  *V = max; // value
  *S = max ? (510*delta+max)/(2*max) : 0; // saturation
  
  // calc hue
  if (*S == 0)
	*H = -1; // undefined hue
  else 
	{
	  switch (maxValue)
		{
	    case 0: // red
		  if (G >= B)
			*H = (120*(G-B)+delta)/(2*delta);
		  else
		    *H = (120*(G-B+delta)+delta)/(2*delta) + 300;
		  break;
	    case 1:	// green
		  if (B > R)
		    *H = 120 + (120*(B-R)+delta)/(2*delta);
		  else
		    *H = 60 + (120*(B-R+delta)+delta)/(2*delta);
		  break;
	    case 2:	// blue
		  if (R > G)
		    *H = 240 + (120*(R-G)+delta)/(2*delta);
		  else
		    *H = 180 + (120*(R-G+delta)+delta)/(2*delta);
		  break;
		}
    }
}

void KoColor::RGBtoLAB(int R, int G, int B, int *L, int *a, int *b)
{
  // Convert between RGB and CIE-Lab color spaces
  // Uses ITU-R recommendation BT.709 with D65 as reference white.
  // algorithm contributed by "Mark A. Ruzon" <ruzon@CS.Stanford.EDU>
  
  double X, Y, Z, fX, fY, fZ;
  
  X = 0.412453*R + 0.357580*G + 0.180423*B;
  Y = 0.212671*R + 0.715160*G + 0.072169*B;
  Z = 0.019334*R + 0.119193*G + 0.950227*B;
  
  X /= (255 * 0.950456);
  Y /=  255;
  Z /= (255 * 1.088754);
  
  if (Y > 0.008856)
	{
	  fY = pow(Y, 1.0/3.0);
	  *L = static_cast<int>(116.0*fY - 16.0 + 0.5);
	}
  else
	{
	  fY = 7.787*Y + 16.0/116.0;
	  *L = static_cast<int>(903.3*Y + 0.5);
	}
  
  if (X > 0.008856)
	fX = pow(X, 1.0/3.0);
  else
	fX = 7.787*X + 16.0/116.0;
  
  if (Z > 0.008856)
	fZ = pow(Z, 1.0/3.0);
  else
	fZ = 7.787*Z + 16.0/116.0;
  
  *a = static_cast<int>(500.0*(fX - fY) + 0.5);
  *b = static_cast<int>(200.0*(fY - fZ) + 0.5);
}

void KoColor::RGBtoCMYK(int R, int G, int B, int *C, int *M, int *Y, int *K)
{
  int min = (R < G) ? R : G;
  *K = (min < B) ? min : B;
  
  *C = 255-(R - *K);
  *M = 255-(G - *K);
  *Y = 255-(B - *K);
}

void KoColor::HSVtoRGB(int H, int S, int V, int *R, int *G, int *B)
{
  *R = *G = *B = V;
  
  if (S != 0 && H != -1) // chromatic
	{
	  if (H >= 360) // angle > 360
		H %= 360;
	  
	  unsigned int f = H % 60;
	  H /= 60;
	  unsigned int p = static_cast<unsigned int>(2*V*(255-S)+255)/510;
	  unsigned int q, t;
	  
	  if (H&1)
		{
		  q = static_cast<unsigned int>(2*V*(15300-S*f)+15300)/30600;
		  switch(H)
			{
			case 1: *R=static_cast<int>(q); *G=static_cast<int>(V), *B=static_cast<int>(p); break;
			case 3: *R=static_cast<int>(p); *G=static_cast<int>(q), *B=static_cast<int>(V); break;
			case 5: *R=static_cast<int>(V); *G=static_cast<int>(p), *B=static_cast<int>(q); break;
			}
		}
	  else
		{
		  t = static_cast<unsigned int>(2*V*(15300-(S*(60-f)))+15300)/30600;
		  switch(H)
			{
			case 0: *R=static_cast<int>(V); *G=static_cast<int>(t), *B=static_cast<int>(p); break;
			case 2: *R=static_cast<int>(p); *G=static_cast<int>(V), *B=static_cast<int>(t); break;
			case 4: *R=static_cast<int>(t); *G=static_cast<int>(p), *B=static_cast<int>(V); break;
			}
		}
	}
}

void KoColor::HSVtoLAB(int H, int S, int V, int *L, int *a, int *b)
{
  int R, G, B;
  HSVtoRGB(H, S, V, &R, &G, &B);
  RGBtoLAB(R, G, B, L, a, b);
}

void KoColor::HSVtoCMYK(int H, int S, int V, int *C, int *M, int *Y, int*K)
{
  int R, G, B;
  HSVtoRGB(H, S, V, &R, &G, &B);
  RGBtoCMYK(R, G, B, C, M, Y, K);
}

void KoColor::LABtoRGB(int L, int a, int b, int *R, int *G, int *B)
{
  // Convert between RGB and CIE-Lab color spaces
  // Uses ITU-R recommendation BT.709 with D65 as reference white.
  // algorithm contributed by "Mark A. Ruzon" <ruzon@CS.Stanford.EDU>
  
  double X, Y, Z, fX, fY, fZ;
  int RR, GG, BB;
  
  fY = pow((L + 16.0) / 116.0, 3.0);
  if (fY < 0.008856)
	fY = L / 903.3;
  Y = fY;
  
  if (fY > 0.008856)
	fY = pow(fY, 1.0/3.0);
  else
	fY = 7.787 * fY + 16.0/116.0;
  
  fX = a / 500.0 + fY;          
  if (fX > 0.206893)
	X = pow(fX, 3.0);
  else
	X = (fX - 16.0/116.0) / 7.787;
  
  fZ = fY - b /200.0;          
  if (fZ > 0.206893)
	Z = pow(fZ, 3.0);
  else
	Z = (fZ - 16.0/116.0) / 7.787;
  
  X *= (0.950456 * 255);
  Y *= 255;
  Z *= (1.088754 * 255);
  
  RR = static_cast<int>(3.240479*X - 1.537150*Y - 0.498535*Z + 0.5);
  GG = static_cast<int>(-0.969256*X + 1.875992*Y + 0.041556*Z + 0.5);
  BB = static_cast<int>(0.055648*X - 0.204043*Y + 1.057311*Z + 0.5);
  
  *R = RR < 0 ? 0 : RR > 255 ? 255 : RR;
  *G = GG < 0 ? 0 : GG > 255 ? 255 : GG;
  *B = BB < 0 ? 0 : BB > 255 ? 255 : BB;
}

void KoColor::LABtoHSV(int L, int a, int b, int *H, int *S, int *V)
{
  int R, G, B;
  LABtoRGB(L, a, b, &R, &G, &B);
  RGBtoHSV(R, G, B, H, S, V);
}

void KoColor::LABtoCMYK(int L, int a, int b, int *C, int *M, int *Y, int*K)
{
  int R, G, B;
  LABtoRGB(L, a, b, &R, &G, &B);
  RGBtoCMYK(R, G, B, C, M, Y, K);
}

void KoColor::CMYKtoRGB(int C, int M, int Y, int K, int *R, int *G, int *B)
{
  *R = 255-(C+K);
  *G = 255-(M+K);
  *B = 255-(Y+K);
}

void KoColor::CMYKtoHSV(int C, int M, int Y, int K, int *H, int *S, int *V)
{
  int R, G, B;
  CMYKtoRGB(C, M, Y, K, &R, &G, &B);
  RGBtoHSV(R, G, B, H, S, V);
}

void KoColor::CMYKtoLAB(int C, int M, int Y, int K, int *L, int *a, int *b)
{
  int R, G, B;
  CMYKtoRGB(C, M, Y, K, &R, &G, &B);
  RGBtoLAB(R, G, B, L, a, b);
}

void KoColor::calcRGB() const
{
  if ( m_RGBvalid )
    return;
  
  switch(m_native)
    {
    case cs_HSV:
      HSVtoRGB(m_H, m_S, m_V, &m_R, &m_G, &m_B);
      break;
    case cs_Lab:
      LABtoRGB(m_L, m_a, m_b, &m_R, &m_G, &m_B);
      break;
    case cs_CMYK:
      CMYKtoRGB(m_C, m_M, m_Y, m_K, &m_R, &m_G, &m_B);
      break;
    default:
      // should never happen!
      m_R = m_G = m_B = 0;
      break;
    }
  
  m_RGBvalid = true;
}

void KoColor::calcHSV() const
{
  if(m_HSVvalid)
    return;
  
  switch(m_native)
  {
    case cs_RGB:
      RGBtoHSV(m_R, m_G, m_B, &m_H, &m_S, &m_V);
      break;
    case cs_Lab:
      LABtoHSV(m_L, m_a, m_b, &m_H, &m_S, &m_V);
      break;
    case cs_CMYK:
      CMYKtoHSV(m_C, m_M, m_Y, m_K, &m_H, &m_S, &m_V);
      break;
    default:
      // should never happen!
      m_H =  m_S = 0;
      m_V = 100;
      break;
    }
  
  m_HSVvalid = true;
}

void KoColor::calcLAB() const
{
  if( m_LABvalid )
    return;
  
  switch ( m_native )
    {
    case cs_RGB:
      RGBtoLAB(m_R, m_G, m_B, &m_L, &m_a, &m_b);
      break;
    case cs_HSV:
      HSVtoLAB(m_H, m_S, m_V, &m_L, &m_a, &m_b);
      break;
    case cs_CMYK:
      CMYKtoLAB(m_C, m_M, m_Y, m_K, &m_L, &m_a, &m_b);
      break;
    default:
      // should never happen!
      m_L = 100;
      m_a = m_b = 0;
      break;
    }
  
  m_LABvalid = true;
}

void KoColor::calcCMYK() const
{
  if( m_CMYKvalid)
    return;
  
  switch ( m_native )
    {
    case cs_RGB:
      RGBtoCMYK(m_R, m_G, m_B, &m_C, &m_M, &m_Y, &m_K);
      break;
    case cs_Lab:
      LABtoCMYK(m_L, m_a, m_b, &m_C, &m_M, &m_Y, &m_K);
      break;
    case cs_HSV:
      HSVtoCMYK(m_H, m_S, m_V, &m_C, &m_M, &m_Y, &m_K);
      break;
    default:
      // should never happen!
      m_C = m_M = m_Y = m_K = 0;
      break;
    }

  m_CMYKvalid = true;
}

void KoColor::rgbChanged() const
{
  m_RGBvalid = true;
  m_HSVvalid = false;
  m_CMYKvalid = false;
  m_LABvalid = false;
}

void KoColor::hsvChanged() const
{
  m_RGBvalid = false;
  m_HSVvalid = true;
  m_CMYKvalid = false;
  m_LABvalid = false;
}

void KoColor::cmykChanged() const
{
  m_RGBvalid = false;
  m_HSVvalid = false;
  m_CMYKvalid = true;
  m_LABvalid = false;
}

void KoColor::labChanged() const
{
  m_RGBvalid = false;
  m_HSVvalid = false;
  m_CMYKvalid = false;
  m_LABvalid = true;
}
