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


#ifndef __sha1__ko__h
#define __sha1__ko__h


class SHA1 {
   public:
     SHA1();
     virtual ~SHA1();

     /*
      *  The number of bits in the hash generated.
      */
     virtual int size();

     /*
      *  True if all settings are good and we are ready to hash.
      */
     virtual bool readyToGo();

     /*
      *  Process a block of data for the hash function.
      */
     virtual int process(void *block, int len);
     
     /*
      *  Return the digest as a 20 byte array reference.
      *  Calling this makes readyToGo() == false.
      */
     virtual const unsigned char *getHash();

     /*
      *  Reset the digest so a new one can be calculated.
      */
     virtual int reset();
     
   protected:
     int _hashlen;
     bool _init;

     long _h0, _h1, _h2, _h3, _h4;
     long _nblocks;
     int _count;
     unsigned char _buf[64];
     void transform(void *data);
};


#endif
