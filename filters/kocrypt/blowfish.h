/* This file is part of the KDE project
   Copyright (C) 2001 George Staikos <staikos@kde.org>
 
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


#include "blockcipher.h"


class BlowFish : public BlockCipher {
   public:
     BlowFish();
     ~BlowFish();

     bool setKey(void *key, int bitlength);

     int getKeyLen();

     bool variableKeyLen();

     bool readyToGo();

     int encrypt(void *block, int len);

     int decrypt(void *block, int len);

   private:
     unsigned long _S[4][256];
     unsigned long _P[18];

     void *_key;
     int _keylen;  // in bits

     bool _init;
 
     bool init();
     unsigned long F(unsigned long x);
     void encipher(unsigned long *xl, unsigned long *xr);
     void decipher(unsigned long *xl, unsigned long *xr);

};

