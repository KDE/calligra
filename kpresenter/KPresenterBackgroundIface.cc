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

#include "KPresenterBackgroundIface.h"

#include "kpbackground.h"

#include <kapp.h>
#include <dcopclient.h>

KPresenterBackgroundIface::KPresenterBackgroundIface( KPBackGround *back_ )
    : DCOPObject()
{
   back = back_;
}

int KPresenterBackgroundIface::getBackType()
{
    return (int)back->getBackType();
}

int KPresenterBackgroundIface::getBackView()
{
    return (int)back->getBackView();
}

QColor KPresenterBackgroundIface::getBackColor1()
{
    return back->getBackColor1();
}

QColor KPresenterBackgroundIface::getBackColor2()
{
    return back->getBackColor2();
}

int KPresenterBackgroundIface::getBackColorType()
{
    return (int)back->getBackColorType();
}

QString KPresenterBackgroundIface::getBackPixFilename()
{
    return back->getBackPixKey().filename();
}

QString KPresenterBackgroundIface::getBackClipFilename()
{
    return back->getBackClipFilename();
}

int KPresenterBackgroundIface::getPageEffect()
{
    return (int)back->getPageEffect();
}

bool KPresenterBackgroundIface::getBackUnbalanced()
{
    return back->getBackUnbalanced();
}

int KPresenterBackgroundIface::getBackXFactor()
{
    return back->getBackXFactor();
}

int KPresenterBackgroundIface::getBackYFactor()
{
    return back->getBackYFactor();
}

QSize KPresenterBackgroundIface::getSize()
{
    return back->getSize();
}
