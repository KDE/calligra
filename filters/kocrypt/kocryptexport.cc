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
#include <qapplication.h>
#include <kmessagebox.h>
#include <klocale.h>

#include "blowfish.h"
#include "cbc.h"
#include "sha1.h"

#include <stdlib.h>
#include <time.h>

#include <pwdprompt.h>


#define READ_ERROR_CHECK()  do {                                           \
      if (rc < 0) {                                                        \
        QApplication::setOverrideCursor(Qt::arrowCursor);                  \
        KMessageBox::error(NULL,                                           \
              i18n("An error occurred reading the unencrypted data."),     \
              i18n("Encrypted Document Export"));                          \
         QApplication::restoreOverrideCursor();                            \
         return false;                                                     \
      }                                                                    \
      } while(0)
 
 
#define WRITE_ERROR_CHECK(XX)  do {                                        \
      if (rc != XX) {                                                      \
         QApplication::setOverrideCursor(Qt::arrowCursor);                 \
         KMessageBox::error(NULL,                                          \
              i18n("Disk write error - out of space?"),                    \
              i18n("Encrypted Document Export"));                          \
         QApplication::restoreOverrideCursor();                            \
         return false;                                                     \
      }                                                                    \
      } while(0)


#define CRYPT_ERROR_CHECK()  do {                                          \
      if (rc != blocksize) {                                               \
         QApplication::setOverrideCursor(Qt::arrowCursor);                 \
         KMessageBox::error(NULL,                                          \
              i18n("There was an internal error while encrypting the file."), \
              i18n("Encrypted Document Export"));                          \
         QApplication::restoreOverrideCursor();                            \
         return false;                                                     \
      }                                                                    \
      } while(0)



#define SHA_ERROR()  do {                                                  \
         QApplication::setOverrideCursor(Qt::arrowCursor);                 \
         KMessageBox::error(NULL,                                          \
              i18n("There was an internal error securing the file."),      \
              i18n("Encrypted Document Export"));                          \
         QApplication::restoreOverrideCursor();                            \
         return false;                                                     \
      } while(0)




KoCryptExport::KoCryptExport(KoFilter *parent, const char *name) :
                             KoFilter(parent, name) {
}


bool KoCryptExport::filter(const QString  &filenameIn,
                           const QString  &filenameOut,
                           const QString  &from,
                           const QString  &to,
                           const QString  &            )
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

    PasswordPrompt *pp = new PasswordPrompt(false);
    connect(pp, SIGNAL(setPassword(QString)), this, SLOT(setPassword(QString)));
    int dlgrc = pp->exec();
    delete pp;
    if (dlgrc == QDialog::Rejected) return false;

    BlowFish cipher;
    CipherBlockChain cbc(&cipher);
    SHA1 sha1;

    char thekey[100];   // we store 56 bytes at most currently. Keep updated!

    strncpy(thekey, pass.latin1(), 56);
    thekey[56] = 0;

    if (!KeyUtil::blowfishIsUsable(thekey, strlen(thekey)*8)) {
        // FIXME: Report
    }

    if (KeyUtil::strengthCheck(thekey, strlen(thekey)*8) < 25) {
        // FIXME: Report, make "25" configurable
    }
    
    // this propagates to the cipher
    if (!cbc.setKey((void *)thekey, strlen(thekey)*8)) {
       QApplication::setOverrideCursor(Qt::arrowCursor);
       KMessageBox::error(NULL,
                  i18n("There was an internal error preparing the passphrase."),
                  i18n("Encrypted Document Export"));
       QApplication::restoreOverrideCursor();
       return false;
    }

    if (cbc.blockSize() > 0) blocksize = cbc.blockSize();

    outf.open(IO_WriteOnly);
    inf.open(IO_ReadOnly);

    // This is bad.  We don't have a buffer big enough for this anyways.
    if (blocksize > 2048 || !sha1.readyToGo()) {  // lets piggy back this error
       QApplication::setOverrideCursor(Qt::arrowCursor);
       KMessageBox::error(NULL,
                  i18n("There was an internal error preparing the cipher."),
                  i18n("Encrypted Document Export"));
       QApplication::restoreOverrideCursor();
       return false;
    }


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

    rc = outf.writeBlock(p, 5);
    WRITE_ERROR_CHECK(5);

    // write the data
    unsigned int previous_rand = rand() % 0x10000;

    while ((previous_rand % 5120) < (unsigned int)blocksize)
       previous_rand = rand() % 0x10000;

    //kdDebug() << "++++++++++++++++++ Output blocksize: " << blocksize << endl;
    //kdDebug() << "++++++++++++++++++ Output previous_rand: " 
    //          << previous_rand%5120 << endl;
    for (char *t = p+2; t-p < (int)(previous_rand % 5120)+2; t += sizeof(int)) {
       *((int *)t) = rand();
    }

    // NOTE: we _don't_ want to write previous_rand%5120 but previous_rand itself.  This
    // just makes the crypto that much stronger.
    p[0] = previous_rand & 0x00ff;
    p[1] = (previous_rand >> 8) & 0x00ff;

    // Write the size of the file out.
    unsigned int filelen = inf.size();

    //kdDebug() << "++++++++++++++++++ Output fsize: " << filelen << endl;
    p[(previous_rand % 5120)+2] = filelen         & 0x00ff;
    p[(previous_rand % 5120)+3] = (filelen >> 8)  & 0x00ff;
    p[(previous_rand % 5120)+4] = (filelen >> 16) & 0x00ff;
    p[(previous_rand % 5120)+5] = (filelen >> 24) & 0x00ff;

    // pad up to the nearest blocksize.
    bool done = false;
    int cursize = (previous_rand % 5120) + 6;
    int shortness = cursize % blocksize;
    //kdDebug() << "++++++++ Cursize is " << cursize << endl;

    if (shortness != 0) {
       char *tp = &(p[cursize]);
       rc = inf.readBlock(tp, blocksize - shortness);
       READ_ERROR_CHECK();

       if (sha1.process(tp, rc) != rc)
          SHA_ERROR();

       // if we ran out of data already (!?!?) append random data.
       cursize += rc;

       shortness = blocksize - (cursize % blocksize);
       if (shortness < blocksize && shortness > 0) {
          // Guess we have to append the hash now.
          const unsigned char *res = sha1.getHash();

          for (int h = 0; h < sha1.size()/8; h++)
             p[cursize++] = res[h];
       }

       shortness = blocksize - (cursize % blocksize);
       while (shortness < blocksize && shortness > 0) {
          p[cursize++] = (char) (rand()%0x100);
          shortness--;
          done = true;
       }
    }

    // kdDebug() << "++++++++ Cursize is " << cursize << endl;

    for (;;) {
       int readsz = 4096 - (4096 % blocksize);
       if (cursize % blocksize != 0) {
         QApplication::setOverrideCursor(Qt::arrowCursor);
         KMessageBox::error(NULL,
            i18n("Internal error writing file.  Please file a bug report."),
                                  i18n("Encrypted Document Export"));
         QApplication::restoreOverrideCursor();
         return false;
       }

       for (int i = 0; i < cursize/blocksize; i++) {
          rc = cbc.encrypt(&(p[i*blocksize]), blocksize);
          CRYPT_ERROR_CHECK();
          rc = outf.writeBlock(&(p[i*blocksize]), blocksize);
          WRITE_ERROR_CHECK(blocksize);
       }

       if (done) break;

       rc = inf.readBlock(p, readsz);
       READ_ERROR_CHECK();

       if (sha1.process(p, rc) != rc)
          SHA_ERROR();

       cursize = rc;

       if (rc != readsz) {
          done = true;
          const unsigned char *res = sha1.getHash();

          for (int h = 0; h < sha1.size()/8; h++)
             p[cursize++] = res[h];

          shortness = blocksize - (cursize % blocksize);
          while (shortness) {
             p[cursize++] = (char) (rand()%0x100);
             shortness--;
          }
       }
    }

    /*************************************************

    This is for debugging only.  It dumps out the hash.

    const unsigned char *res = sha1.getHash();
 
    if (res) {
       for (int i = 0; i < 20; i++) {
          printf("%.2X", *res++);
          if (i>0 && (i-1)%2 == 0) printf(" ");
       }
       printf("\n");
    }
    **************************************************/

    return true;
}


void KoCryptExport::setPassword(QString x) {
   pass = x;
}


#include "kocryptexport.moc"

