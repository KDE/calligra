/* This file is part of the KDE project
   Copyright (C) 2001 George Staikos <staikos@kde.org>
   Based heavily on SHA1 code from GPG 1.0.3 (C) 1998 FSF
 
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

#include "sha1.h"


#define K1 0x5a827999L
#define K2 0x6ed9eba1L
#define K3 0x8f1bbcdcL
#define K4 0xca62c1d6L
#define F1(x,y,z) ( z ^ ( x & ( y ^ z ) ) )
#define F2(x,y,z) ( x ^ y ^ z )
#define F3(x,y,z) ( ( x & y ) | ( z & ( x | y ) ) )
#define F4(x,y,z) ( x ^ y ^ z )

SHA1::SHA1() {
  _hashlen = 160;
  _init = false;
  reset();
}


int SHA1::reset() {
  _h0 = 0x67452301;
  _h1 = 0xefcdab89;
  _h2 = 0x98badcfe;
  _h3 = 0x10325476;
  _h4 = 0xc3d2e1f0;
  _nblocks = 0;
  _count = 0;

  _init = true;
  return 0;
}


SHA1::~SHA1() {
}


bool SHA1::readyToGo() {
  return _init;
}



