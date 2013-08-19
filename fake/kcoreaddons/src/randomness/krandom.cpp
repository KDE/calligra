/* This file is part of the KDE libraries
    Copyright (c) 1999 Matthias Kalle Dalheimer <kalle@kde.org>
    Copyright (c) 2000 Charles Samuels <charles@kde.org>
    Copyright (c) 2005 Joseph Wenninger <kde@jowenn.at>

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
    the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
    Boston, MA 02110-1301, USA.
        */

#include "krandom.h"

#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <time.h>
#include <sys/time.h>
#include <fcntl.h>

#include <QtCore/QFile>

int KRandom::random()
{
   static bool init = false;
   if (!init)
   {
      unsigned int seed;
      init = true;
      QFile urandom(QString::fromLatin1("/dev/urandom"));
      bool opened = urandom.open(QIODevice::ReadOnly);
      if (!opened || urandom.read((char *)&seed, sizeof(seed)) != sizeof(seed))
      {
            // No /dev/urandom... try something else.
            srand(getpid());
            seed = rand()+time(0);
      }
      srand(seed);
   }
   return rand();
}

QString KRandom::randomString(int length)
{
   if (length <=0 ) return QString();

   QString str; str.resize( length );
   int i = 0;
   while (length--)
   {
      int r=random() % 62;
      r+=48;
      if (r>57) r+=7;
      if (r>90) r+=6;
      str[i++] =  char(r);
      // so what if I work backwards?
   }
   return str;
}
