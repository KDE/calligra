/* This file is part of the KDE project
   Copyright (C) 1998, 1999 Reginald Stadlbauer <reggie@kde.org>

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

#ifndef KPRESENTER_BGND_IFACE_H
#define KPRESENTER_BGND_IFACE_H

#include <dcopobject.h>
#include <dcopref.h>

#include <qstring.h>
#include <qcolor.h>

class KPBackGround;

class KPresenterBackgroundIface : virtual public DCOPObject
{
    K_DCOP
public:
    KPresenterBackgroundIface( KPBackGround *back_ );

k_dcop:
    int getBackType();
    int getBackView();
    QColor getBackColor1();
    QColor getBackColor2();
    int getBackColorType();
    QString getBackPixFilename();
    QString getBackClipFilename();
    int getPageEffect();
    bool getBackUnbalanced();
    int getBackXFactor();
    int getBackYFactor();
    QSize getSize();

private:
    KPBackGround *back;

};

#endif
