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


// ATTENTION:  If you update the format or crypto algorithm, be sure to
//             maintain backwards compatibility.

#include <kocryptexport.h>
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

    if (cbc->blockSize > 0) blocksize = cbc->blockSize();

    // create the output file, open the input file.
    outf.open(IO_WriteOnly);
    inf.open(IO_ReadOnly);

    // write the header out to the output file.
    char p[5120];
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
    int randlen = rand() % 0x10000;

    for (char *t = p+2; t-p < randlen+2; t += sizeof(int)) {
       ((int *)t) = rand();
    }

    p[0] = randlen & 0x00ff;
    p[1] = randlen & 0xff00;

    unsigned int filelen = inf.size();

    p[randlen+2] = filelen & 0x000000ff;
    p[randlen+3] = filelen & 0x0000ff00;
    p[randlen+4] = filelen & 0x00ff0000;
    p[randlen+5] = filelen & 0xff000000;

    // FIXME: write out the encrypted data
    
    return true;
}


#include "kocryptexport.moc"

