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

#include <config.h>

#include <cbc.h>
#include <blowfish.h>

#include <kocryptdefs.h>

#include <qtextstream.h>
#include <kocryptimport.h>
#include <kdebug.h>



KoCryptImport::KoCryptImport(KoFilter *parent, const char *name) :
                             KoFilter(parent, name) {
}


bool KoCryptImport::filter(const QString &fileIn, const QString &fileOut,
                           const QString& from, const QString& to,
                           const QString &) {
int ftype = -1;
int blocksize = 64;
QFile inf(fileIn);
QFile outf(fileOut);

    if (to == "application/x-kword" && from == "application/x-kword-crypt")
    {
       ftype = APPID_KWORD;
    } else
    if (to == "application/x-kspread" && from == "application/x-kspread-crypt")
    {
       ftype = APPID_KSPREAD;
    } else {
        return false;
    }

    // FIXME: obtain the password

    BlockCipher *cipher = new BlowFish;
    BlockCipher *cbc = new CipherBlockChain(cipher);
    char *thekey = "a test key";
 
    cbc->setKey((void *)thekey, 80);   // this propagates to the cipher
 
    if (cbc->blockSize() > 0) blocksize = cbc->blockSize();

    inf.open(IO_ReadOnly);
    outf.open(IO_WriteOnly);

    // This is bad.  We don't have a buffer big enough for this anyways.
    if (blocksize > 2048) return false;

    char p[8192];

    // check the header
    inf.readBlock(p, 9);
    if (p[0] != FID_FIRST || p[1] != FID_SECOND || p[2] != FID_THIRD ||
        p[3] != ftype) {
      return false;   // wrong file type!
    }

    /*
     *   File format version determination.  Here we should set flags to be
     *   able to read older file formats.
     */
    if (p[4] != FID_FVER) {
      return false;   // right now there is only one fileversion to understand!
    }

    /*
     *   Crypto algorithm determination - here we set flags to take old
     *   algorithms into account if we're more recent.
     */
    if (p[5] != 0 || p[6] != 0 || p[7] != 0 || p[8] != 0) {
      return false;   // we only know one crypto algorithm too.
    }

    /*
     *   Decrypt it.  don't forget to toss the extra data at the beginning and
     *   end of the file!
     */
 // FIXME: check io errors from reads and writes throughout the file
 //         ALSO crypto return codes!!
    inf.readBlock(p, blocksize);
    cbc->decrypt(p, blocksize);
    kdDebug() <<  "p[0] = " << p[0]
              << " p[1] = " << p[1]
              << " p[2] = " << p[2]
              << " p[3] = " << p[3]
              << " p[4] = " << p[4]
              << " p[5] = " << p[5]
              << " p[6] = " << p[6]
              << " p[7] = " << p[7] << endl;
    unsigned int previous_rand = ((unsigned char)p[0] + ((unsigned char)p[1] << 8)) % 5120;
    unsigned int fsize;

    // We skip the rest of this block since previous_rand%5120 has to be >=
    // blocksize.

    kdDebug() << "previous_rand = " << previous_rand
              << endl;

    previous_rand -= (blocksize-2);

    unsigned int remaining = 0;
    while (previous_rand > 0) {
      inf.readBlock(p, blocksize); 
      cbc->decrypt(p, blocksize);
      if (previous_rand >= (unsigned int)blocksize) {
         previous_rand -= blocksize;
         continue;
      } else {
         remaining = (blocksize - previous_rand);
         previous_rand = 0;
      }
    }

    // read in the file size
    fsize = 0;
    for (int i = 0; i < 4; i++) {
       if (remaining == 0) {
          inf.readBlock(p, blocksize);
          cbc->decrypt(p, blocksize);
          remaining = blocksize;
       }
       fsize += (unsigned char)p[blocksize-remaining] << i*8;
       remaining--;
    }

    kdDebug() << "fsize = " << fsize << endl;
    // Empty out this remaining block that we read in
    if (remaining > 0) {
      if (remaining > fsize) {
        outf.writeBlock(&(p[blocksize-remaining]), fsize);
        remaining -= fsize;
        fsize = 0;
      } else {
        outf.writeBlock(&(p[blocksize-remaining]), remaining);
        fsize -= remaining;
        remaining = 0;
      }
    }

kdDebug() << "First loop is done." << endl;

    // read in the rest of the file and decode
    while (fsize > 0) {
      inf.readBlock(p, blocksize);
      cbc->decrypt(p, blocksize);
      if (fsize >= (unsigned int)blocksize) {
         fsize -= blocksize;
         outf.writeBlock(p, blocksize);
         continue;
      } else {
         // Hash will eventually be verified here
         outf.writeBlock(p, fsize);
         fsize = 0;
      }
    }

    // FIXME: check the filesize and the hash to make sure it was successful

kdDebug() << "Second loop is done." << endl;

    return true;
}



#include "kocryptimport.moc"

