/*
 *  kcolor.cc - part of KImageShop
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

#include <math.h>
#include <kcolor.h>

KColor::KColor()
{
  m_R = m_G = m_B = 0;
  m_nativeModel = RGB;
}

KColor::KColor(int a, int b, int c, colorModel m)
{
  switch (m)
	{
	case RGB:
	  m_R = static_cast<unsigned char>(a);
	  m_G = static_cast<unsigned char>(b);
	  m_B = static_cast<unsigned char>(c);
	  m_nativeModel = RGB;
	  break;
	case HSV:
	  m_H = a;
	  m_S = static_cast<unsigned char>(b);
	  m_V = static_cast<unsigned char>(c);
	  m_nativeModel = HSV;
	  break;
	case LAB:
	  m_L = a;
	  m_a = b;
	  m_b = c;
	  m_nativeModel = LAB;
	  break;
	default:
	  m_R = m_G = m_B = 0;
	  m_nativeModel = RGB;
	}
}

KColor::KColor(int c, int m, int y, int k)
{
  m_C = static_cast<unsigned char>(c);
  m_M = static_cast<unsigned char>(m);
  m_Y = static_cast<unsigned char>(y);
  m_K = static_cast<unsigned char>(k);
  m_nativeModel = CMYK;
}

KColor::~KColor() {}

KColor &KColor::operator=(const KColor &c)
{
  m_nativeModel = c.m_nativeModel;
  
  switch (c.m_nativeModel)
	{
	case RGB:
	  m_R = c.m_R;
	  m_G = c.m_G;
	  m_B = c.m_B;
	  break;
	case CMYK:
	  m_C = c.m_C;
	  m_M = c.m_M;
	  m_Y = c.m_Y;
	  m_K = c.m_K;
	  break;
	case HSV:
	  m_H = c.m_H;
	  m_S = c.m_S;
	  m_V = c.m_V;
	  break;
	case LAB:
	  m_L = c.m_L;
	  m_a = c.m_a;
	  m_b = c.m_b;
	  break;
	}
  return *this;
}

void KColor::setRGB (uchar R, uchar G, uchar B)
{
  m_R = R;
  m_G = G;
  m_B = B;
  m_nativeModel = RGB;
}

void KColor::setHSV (int H, uchar S, uchar V)
{
  m_H = H;
  m_S = S;
  m_V = V;
  m_nativeModel = HSV;
}

void KColor::setLAB (int L, int a, int b)
{
  m_L = L;
  m_a = a;
  m_b = b;
  m_nativeModel = LAB;
}

void KColor::setCMYK (uchar C, uchar M, uchar Y, uchar K)
{
  m_C = C;
  m_M = M;
  m_Y = Y;
  m_K = K;
  m_nativeModel = CMYK;
}

void KColor::rgb (uchar *R, uchar *G, uchar *B)
{
  if (!m_nativeModel == RGB)
	calcRGB();
  
  *R = m_R;
  *G = m_G;
  *B = m_B;
}

void KColor::hsv (int *H, uchar *S, uchar *V)
{
  if (!m_nativeModel == HSV)
  	calcHSV();
  
  *H = m_H;
  *S = m_S;
  *V = m_V;
}

void KColor::lab (int *L, int *a, int *b)
{
  if (!m_nativeModel == LAB)
  	calcLAB();
  
  *L = m_L;
  *a = m_a;
  *b = m_b;
}

void KColor::cmyk (uchar *C, uchar *M, uchar *Y, uchar *K)
{
  if (!m_nativeModel == CMYK)
  	calcCMYK();
  
  *C = m_C;
  *M = m_M;
  *Y = m_Y;
  *K = m_K;
}

void KColor::calcRGB()
{
  if(m_nativeModel == RGB)
	return;
  
  switch (m_nativeModel)
	{
	case HSV:
	  HSVtoRGB(m_H, m_S, m_V, &m_R, &m_G, &m_B);
	  break;
	case LAB:
	  LABtoRGB(m_L, m_a, m_b, &m_R, &m_G, &m_B);
	  break;
	case CMYK:
	  CMYKtoRGB(m_C, m_M, m_Y, m_K, &m_R, &m_G, &m_B);
	  break;
	default:
	  // should never happen!
	  m_R = m_B = m_G = 0;
	  break;
	}
}

void KColor::calcHSV()
{
  if(m_nativeModel == HSV)
	return;
  
  switch (m_nativeModel)
	{
	case RGB:
	  RGBtoHSV(m_R, m_G, m_B, &m_H, &m_S, &m_V);
	  break;
	case LAB:
	  LABtoHSV(m_L, m_a, m_b, &m_H, &m_S, &m_V);
	  break;
	case CMYK:
	  CMYKtoHSV(m_C, m_M, m_Y, m_K, &m_H, &m_S, &m_V);
	  break;
	default:
	  // should never happen!
	  m_H = -1; m_S = m_V = 0;
	  break;
	}
}

void KColor::calcLAB()
{
  if(m_nativeModel == LAB)
	return;
  
  switch (m_nativeModel)
	{
	case RGB:
	  RGBtoLAB(m_R, m_G, m_B, &m_L, &m_a, &m_b);
	  break;
	case HSV:
	  HSVtoLAB(m_H, m_S, m_V, &m_L, &m_a, &m_b);
	  break;
	case CMYK:
	  CMYKtoLAB(m_C, m_M, m_Y, m_K, &m_L, &m_a, &m_b);
	  break;
	default:
	  // should never happen!
	  m_L = m_a = m_b = 0;
	  break;
	}
}

void KColor::calcCMYK()
{
  if(m_nativeModel == CMYK)
	return;
  
  switch (m_nativeModel)
	{
	case RGB:
	  RGBtoCMYK(m_R, m_G, m_B, &m_C, &m_M, &m_Y, &m_K);
	  break;
	case LAB:
	  LABtoCMYK(m_L, m_a, m_b, &m_C, &m_M, &m_Y, &m_K);
	  break;
	case HSV:
	  HSVtoCMYK(m_H, m_S, m_V, &m_C, &m_M, &m_Y, &m_K);
	  break;
	default:
	  // should never happen!
	  m_C = m_M = m_Y = m_K = 0;
	  break;
	}
}

void KColor::RGBtoHSV(uchar R, uchar G, uchar B, int *H, uchar *S, uchar *V)
{
  unsigned int max, min = R;
  unsigned char maxValue = 0; // r=0, g=1, b=2
  
  // find maximum and minimum RGB values
  if (static_cast<unsigned int>(G) > max) { max = G; maxValue = 1; }
  if (static_cast<unsigned int>(B) > max) { max = B; maxValue = 2; }
  
  if (static_cast<unsigned int>(G) < min) min = G;
  if (static_cast<unsigned int>(B) < min ) min = B;
  
  int delta = max - min;
  *V = static_cast<unsigned char>(max); // value
  *S = static_cast<unsigned char>(max ? (510*delta+max)/(2*max) : 0); // saturation
  
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

void KColor::RGBtoLAB(uchar R, uchar G, uchar B, int *L, int *a, int *b)
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

void KColor::RGBtoCMYK(uchar R, uchar G, uchar B, uchar *C, uchar *M, uchar *Y, uchar *K)
{
  uchar min = (R < G) ? R : G;
  *K = (min < B) ? min : B;
  
  *C = 255-(R - *K);
  *M = 255-(G - *K);
  *Y = 255-(B - *K);
}

void KColor::HSVtoRGB(int H, uchar S, uchar V, uchar *R, uchar *G, uchar *B)
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

void KColor::HSVtoLAB(int H, uchar S, uchar V, int *L, int *a, int *b)
{
  uchar R, G, B;
  HSVtoRGB(H, S, V, &R, &G, &B);
  RGBtoLAB(R, G, B, L, a, b);
}

void KColor::HSVtoCMYK(int H, uchar S, uchar V, uchar *C, uchar *M, uchar *Y, uchar*K)
{
  uchar R, G, B;
  HSVtoRGB(H, S, V, &R, &G, &B);
  RGBtoCMYK(R, G, B, C, M, Y, K);
}

void KColor::LABtoRGB(int L, int a, int b, uchar *R, uchar *G, uchar *B)
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
  
  *R = static_cast<unsigned char>(RR < 0 ? 0 : RR > 255 ? 255 : RR);
  *G = static_cast<unsigned char>(GG < 0 ? 0 : GG > 255 ? 255 : GG);
  *B = static_cast<unsigned char>(BB < 0 ? 0 : BB > 255 ? 255 : BB);
}

void KColor::LABtoHSV(int L, int a, int b, int *H, uchar *S, uchar *V)
{
  uchar R, G, B;
  LABtoRGB(L, a, b, &R, &G, &B);
  RGBtoHSV(R, G, B, H, S, V);
}

void KColor::LABtoCMYK(int L, int a, int b, uchar *C, uchar *M, uchar *Y, uchar*K)
{
  uchar R, G, B;
  LABtoRGB(L, a, b, &R, &G, &B);
  RGBtoCMYK(R, G, B, C, M, Y, K);
}

void KColor::CMYKtoRGB(uchar C, uchar M, uchar Y, uchar K, uchar *R, uchar *G, uchar *B)
{
  *R = static_cast<unsigned char>(255-(C+K));
  *G = static_cast<unsigned char>(255-(M+K));
  *B = static_cast<unsigned char>(255-(Y+K));
}

void KColor::CMYKtoHSV(uchar C, uchar M, uchar Y, uchar K, int *H, uchar *S, uchar *V)
{
  uchar R, G, B;
  CMYKtoRGB(C, M, Y, K, &R, &G, &B);
  RGBtoHSV(R, G, B, H, S, V);
}

void KColor::CMYKtoLAB(uchar C, uchar M, uchar Y, uchar K, int *L, int *a, int *b)
{
  uchar R, G, B;
  CMYKtoRGB(C, M, Y, K, &R, &G, &B);
  RGBtoLAB(R, G, B, L, a, b);
}
