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



#ifndef __CBC__KO__H
#define __CBC__KO__H

#include "blockcipher.h"

/*
 *   Initialize this class with a pointer to a valid, uninitialized BlockCipher
 *   and it will apply that cipher using CBC.  You may want to make the
 *   initial block a full block of random data.  Do not change the block size
 *   at any time!!  You must pad it yourself.  Also, you can only encrypt or
 *   decrypt.  You can't do both with a given instance.  After you call one,
 *   calls to the other will fail in this instance.
 */

class CipherBlockChain : public BlockCipher {
  public:
    CipherBlockChain(BlockCipher *cipher);
    ~CipherBlockChain();

    bool setKey(void *key, int bitlength);
 
    int getKeyLen();
 
    bool variableKeyLen();
 
    bool readyToGo();
 
    int encrypt(void *block, int len);
 
    int decrypt(void *block, int len);

  private: 
    BlockCipher *_cipher;
    void *_register;
    void *_next;
    int _len;
    int _reader, _writer;
    
};

#endif
