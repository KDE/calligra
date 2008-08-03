/* This file is part of the KDE project
   Copyright (C) 2004 Cedric Pasteur <cedric.pasteur@free.fr>

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
 * Boston, MA 02110-1301, USA.
*/

#ifdef HAVE_KNEWSTUFF

#ifndef KEXINEWSTUFF_H
#define KEXINEWSTUFF_H

#include "knewstuff/knewstuff.h"

class KexiNewStuff : public KNewStuff
{
public:
    KexiNewStuff(QWidget *parent);
    virtual ~KexiNewStuff();

    virtual bool install(const QString &fileName);
    virtual bool createUploadFile(const QString &fileName);

    //KNS::Engine*  customEngine() { return KNewStuff::engine(); }
};

#endif

#endif //HAVE_KNEWSTUFF
