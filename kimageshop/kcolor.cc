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

#include <kcolor.h>

KColor::KColor()
{
  m_r = m_g = m_b = 0;
  RGBdirty = false;
  HSVdirty = LABdirty = CMYKdirty = true;
}

KColor::KColor(int a, int b, int c, int d, colorModel m)
{
  switch (m)
	{
	case RGB:
	  m_r = a;
	  m_g = b;
	  m_b = c;
	  RGBdirty = false;
	  HSVdirty = LABdirty = CMYKdirty = true;
	  break;
	case HSV:
	  m_h = a;
	  m_s = b;
	  m_v = c;
	  HSVdirty = false;
	  RGBdirty = LABdirty = CMYKdirty = true;
	  break;
	case LAB:
	  m_l = a;
	  m_la = b;
	  m_lb = c;
	  LABdirty = false;
	  RGBdirty = HSVdirty = CMYKdirty = true;
	  break;
	case CMYK:
	  m_c = a;
	  m_m = b;
	  m_y = c;
	  m_k = d;
	  CMYKdirty = false;
	  RGBdirty = HSVdirty = LABdirty = true;
	  break;
	default:
	  m_r = m_g = m_b = 0;
	  RGBdirty = false;
	  HSVdirty = LABdirty = CMYKdirty = true;
	}
}

KColor::~KColor() {}

void KColor::setRGB (int r, int g, int b)
{
  m_r = r;
  m_g = g;
  m_b = b;
  RGBdirty = false;
  HSVdirty = LABdirty = CMYKdirty = true;
}

void KColor::setHSV (int h, int s, int v)
{
  m_h = h;
  m_s = s;
  m_v = v;
  HSVdirty = false;
  RGBdirty = LABdirty = CMYKdirty = true;
}

void KColor::setLAB (int l, int a, int b)
{
  m_l = l;
  m_la = a;
  m_lb = b;
  LABdirty = false;
  RGBdirty = HSVdirty = CMYKdirty = true;
}

void KColor::setCMYK (int c, int m, int y, int k)
{
  m_c = c;
  m_m = m;
  m_y = y;
  m_k = k;
  CMYKdirty = false;
  RGBdirty = HSVdirty = LABdirty = true;
}

void KColor::rgb (int *r, int *g, int *b)
{
  if (RGBdirty)
	calcRGB();

  *r = m_r;
  *g = m_g;
  *b = m_b;
}

void KColor::hsv (int *h, int *s, int *v)
{
  if (HSVdirty)
  	calcHSV();

  *h = m_h;
  *s = m_s;
  *v = m_v;
}

void KColor::lab (int *l, int *a, int *b)
{
  if (LABdirty)
  	calcLAB();

  *l = m_l;
  *a = m_la;
  *b = m_lb;
}

void KColor::cmyk (int *c, int *m, int *y, int *k)
{
  if (CMYKdirty)
  	calcCMYK();

  *c = m_c;
  *m = m_m;
  *y = m_y;
  *k = m_k;
}

void KColor::calcRGB()
{
  if(!RGBdirty)
	return;

  if (!HSVdirty)
	HSVtoRGB(m_h, m_s, m_v, &m_r, &m_g, &m_b);
  else if (!LABdirty)
	LABtoRGB(m_l, m_la, m_lb, &m_r, &m_g, &m_b);
  else if (!CMYKdirty)
	CMYKtoRGB(m_c, m_m, m_y, m_k, &m_r, &m_g, &m_b);
  else // should never happen!
	  m_r = m_b = m_g = 0;
}

void KColor::calcHSV()
{
  if(!HSVdirty)
	return;

  if(!RGBdirty)
  	RGBtoHSV(m_r, m_g, m_b, &m_h, &m_s, &m_v);
  else if (!LABdirty)
	LABtoHSV(m_l, m_la, m_lb, &m_h, &m_s, &m_v);
  else if (!CMYKdirty)
	CMYKtoHSV(m_c, m_m, m_y, m_k, &m_h, &m_s, &m_v);
  else // should never happen!
	m_h = m_s = m_v = 0;
}

void KColor::calcLAB()
{
  if(!LABdirty)
	return;

  if(!RGBdirty)
  	RGBtoLAB(m_r, m_g, m_b, &m_l, &m_la, &m_lb);
  else if(!HSVdirty)
  	HSVtoLAB(m_h, m_s, m_v, &m_l, &m_la, &m_lb);
  else if(!CMYKdirty)
  	CMYKtoLAB(m_c, m_m, m_y, m_k, &m_l, &m_la, &m_lb);
  else // should never happen!
	m_l = m_la = m_lb = 0;
}

void KColor::calcCMYK()
{
  if(!CMYKdirty)
	return;
  
  if(!LABdirty)
	LABtoCMYK(m_l, m_la, m_lb, &m_c, &m_m, &m_y, &m_k);
  else if(!RGBdirty)
  	RGBtoCMYK(m_r, m_g, m_b, &m_c, &m_m, &m_y, &m_k);
  else if(!HSVdirty)
	HSVtoCMYK(m_h, m_s, m_v, &m_c, &m_m, &m_y, &m_k);
  else // should never happen!
	m_c = m_m = m_y = m_k = 0;
}

void KColor::RGBtoHSV(int r, int b, int g, int *h, int *s, int *v)
{
  unsigned int max, min = r;
  unsigned char maxValue = 0; // r=0, g=1, b=2
  
  // find maximum and minimum RGB values
  if (static_cast<unsigned int>(g) > max) { max = g; maxValue = 1; }
  if (static_cast<unsigned int>(b) > max) { max = b; maxValue = 2; }

  if (static_cast<unsigned int>(g) < min) min = g;
  if (static_cast<unsigned int>(b) < min ) min = b;

  int delta = max - min;
  *v = max; // value
  *s = max ? (510*delta+max)/(2*max) : 0; // saturation
  
  // calc hue
  if (*s == 0)
	  *h = -1; // undefined hue
  else 
	{
	  switch (maxValue)
		{
	    case 0: // red
		  if (g >= b)
			*h = (120*(g-b)+delta)/(2*delta);
		  else
		    *h = (120*(g-b+delta)+delta)/(2*delta) + 300;
		  break;
	    case 1:	// green
		  if (b > r)
		    *h = 120 + (120*(b-r)+delta)/(2*delta);
		  else
		    *h = 60 + (120*(b-r+delta)+delta)/(2*delta);
		  break;
	    case 2:	// blue
		  if (r > g)
		    *h = 240 + (120*(r-g)+delta)/(2*delta);
		  else
		    *h = 180 + (120*(r-g+delta)+delta)/(2*delta);
		  break;
		}
    }
}

void KColor::RGBtoLAB(int r, int b, int g, int *l, int *la, int *lb)
{
}

void KColor::RGBtoCMYK(int r, int b, int g, int *c, int *m, int *y, int *k)
{
}

void KColor::HSVtoRGB(int h, int s, int v, int *r, int *g, int *b)
{
  *r = *g = *b = v;

  if (s != 0 && h != -1) // chromatic
	{
	  if (h >= 360) // angle > 360
		h %= 360;
	  
	unsigned int f = h % 60;
	h /= 60;
	unsigned int p = static_cast<unsigned int>(2*v*(255-s)+255)/510;
	unsigned int q, t;
	
	if (h&1)
	  {
	    q = static_cast<unsigned int>(2*v*(15300-s*f)+15300)/30600;
	    switch(h)
		  {
		  case 1: *r=static_cast<int>(q); *g=static_cast<int>(v), *b=static_cast<int>(p); break;
		  case 3: *r=static_cast<int>(p); *g=static_cast<int>(q), *b=static_cast<int>(v); break;
		  case 5: *r=static_cast<int>(v); *g=static_cast<int>(p), *b=static_cast<int>(q); break;
		  }
	  }
	else
	  {
	    t = static_cast<unsigned int>(2*v*(15300-(s*(60-f)))+15300)/30600;
	    switch(h)
		  {
		  case 0: *r=static_cast<int>(v); *g=static_cast<int>(t), *b=static_cast<int>(p); break;
		  case 2: *r=static_cast<int>(p); *g=static_cast<int>(v), *b=static_cast<int>(t); break;
		  case 4: *r=static_cast<int>(t); *g=static_cast<int>(p), *b=static_cast<int>(v); break;
		  }
	  }
    }
}

void KColor::HSVtoLAB(int h, int s, int v, int *l, int *la, int *lb)
{
}

void KColor::HSVtoCMYK(int h, int s, int v, int *c, int *m, int *y, int*k)
{
}

void KColor::LABtoRGB(int l, int la, int lb, int *r, int *g, int *b)
{
}

void KColor::LABtoHSV(int l, int la, int lb, int *h, int *s, int *v)
{
}

void KColor::LABtoCMYK(int l, int la, int lb, int *c, int *m, int *y, int*k)
{
}

void KColor::CMYKtoRGB(int c, int m, int y, int k, int *r, int *g, int *b)
{
}

void KColor::CMYKtoHSV(int c, int m, int y, int k, int *h, int *s, int *v)
{
}

void KColor::CMYKtoLAB(int c, int m, int y, int k, int *l, int *la, int *lb)
{
}
