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

////////////////////////////////////////////////////////////////////////////
// ATTENTION:  If you update the format or crypto algorithm, be sure to
//             maintain backwards compatibility.
////////////////////////////////////////////////////////////////////////////

#include <config.h>

#include <kocryptexport.h>
#include <kocryptdefs.h>
#include <kdebug.h>
#include <qdom.h>

#include "blowfish.h"
#include "cbc.h"

#include <stdlib.h>
#include <time.h>

KoCryptExport::KoCryptExport(KoFilter *parent, const char *name) :
                             KoFilter(parent, name) {
}


bool KoCryptExport::filter(const QString  &filenameIn,
                           const QString  &filenameOut,
                           const QString  &from,
                           const QString  &to,
                           const QString  &         )
{
int ftype = -1;
QFile inf(filenameIn);
QFile outf(filenameOut);
int blocksize = 64;  // bytes
int rc;

    // FIXME: we should use a cryptographically strong random function here.
    srand(time(NULL));

    if (to == "application/x-kword-crypt" && from == "application/x-kword")
    {
       ftype = APPID_KWORD;
    } else
    if (to == "application/x-kspread-crypt" && from == "application/x-kspread")
    {
       ftype = APPID_KSPREAD;
    } else {
        return false;
    }

    // FIXME: obtain the password and ensure that we can use it.

    BlockCipher *cipher = new BlowFish;
    BlockCipher *cbc = new CipherBlockChain(cipher);
    char *thekey = "a test key";

    cbc->setKey((void *)thekey, 80);   // this propagates to the cipher

    if (cbc->blockSize() > 0) blocksize = cbc->blockSize();

    // FIXME: check for error codes here!!!
    // create the output file, open the input file.
    outf.open(IO_WriteOnly);
    inf.open(IO_ReadOnly);

    // This is bad.  We don't have a buffer big enough for this anyways.
    if (blocksize > 2048) return false;

    // write the header out to the output file.
    char p[8192];
    p[0] = FID_FIRST;
    p[1] = FID_SECOND;
    p[2] = FID_THIRD;
    p[3] = ftype;

    outf.writeBlock(p, 4);

    // write out the file format version number, and select the
    // original 16 rounds blowfish with CBC.  (0x00000000)
    p[0] = FID_FVER;
    p[1] = 0;
    p[2] = 0;
    p[3] = 0;
    p[4] = 0;

    outf.writeBlock(p, 5);

    // write the data
    unsigned int previous_rand = rand() % 0x10000;

    while ((previous_rand % 5120) < (unsigned int)blocksize)
       previous_rand = rand() % 0x10000;

kdDebug() << "++++++++++++++++++ Output previous_rand: " << previous_rand%5120 << endl;
    for (char *t = p+2; t-p < (int)(previous_rand % 5120)+2; t += sizeof(int)) {
       *((int *)t) = rand();
    }

    // NOTE: we _don't_ want to write previous_rand%5120 but previous_rand itself.  This
    // just makes the crypto that much stronger.
    p[0] = previous_rand & 0x00ff;
    p[1] = (previous_rand >> 8) & 0x00ff;

    // Write the size of the file out.
    unsigned int filelen = inf.size();

kdDebug() << "++++++++++++++++++ Output fsize: " << filelen << endl;
    p[(previous_rand % 5120)+2] = filelen         & 0x00ff;
    p[(previous_rand % 5120)+3] = (filelen >> 8)  & 0x00ff;
    p[(previous_rand % 5120)+4] = (filelen >> 16) & 0x00ff;
    p[(previous_rand % 5120)+5] = (filelen >> 24) & 0x00ff;

    // pad up to the nearest blocksize.
    bool done = false;
    int cursize = (previous_rand % 5120) + 6;
    int shortness = cursize % blocksize;

    if (shortness != 0) {
       char *tp = &(p[cursize]);
       rc = inf.readBlock(tp, blocksize - shortness);

       if (rc < 0) return false;

       // if we ran out of data already (!?!?) append random data.
       cursize += rc;
       shortness = blocksize - (cursize % blocksize);
       while (shortness > 0) {
          p[cursize++] = (char) (rand()%0x100);
          shortness--;
          done = true;
       }
    }

    for (;;) {
       // assert: cursize % blocksize == 0;

       for (int i = 0; i < cursize/blocksize; i++) {
          rc = cbc->encrypt(&(p[i*blocksize]), blocksize);
          if (rc != blocksize) return false;     // encryption error
          rc = outf.writeBlock(&(p[i*blocksize]), blocksize);
          if (rc != blocksize) return false;     // write error
       }

       if (done) break;

       rc = inf.readBlock(p, 4096);
       if (rc < 0) return false; // read error

       cursize = rc;

       if (rc != 4096) {
          done = true;
          shortness = blocksize - (cursize % blocksize);
          while (shortness) {
             p[cursize++] = (char) (rand()%0x100);
             shortness--;
          }
       }
    }

    return true;
}


#include "kocryptexport.moc"

