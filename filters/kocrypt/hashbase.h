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


#ifndef __hashbase__ko__h
#define __hashbase__ko__h


class HashBase {
   public:
     HashBase();
     virtual ~HashBase();

     /*
      *  Return the size of the hash in bits.  -1 on error.
      */
     int hashSize();

     /*
      *  True if all settings are good and we are ready to hash.
      */
     virtual bool readyToGo() = 0;

     /*
      *  Process a block of data for the hash function.
      */
     virtual int process(void *block, int len) = 0;
     
     /*
      *  Return the digest.
      */
     
     /*
      *  Reset the digest so a new one can be calculated.
      */
     virtual int reset() = 0;
     
   protected:
     int _hashlen;
};


#endif
