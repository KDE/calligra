// -*- Mode: c++; c-basic-offset: 4; indent-tabs-mode: nil; tab-width: 4; -*-
/* This file is part of the KDE project
   Copyright (C) 2002 Laurent MONTEL <lmontel@mandrakesoft.com>

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

#include "KPrPixmapObjectIface.h"
#include "KPrPixmapObject.h"
#include "KPrObject.h"

#include <kapplication.h>
#include <dcopclient.h>

KPrPixmapObjectIface::KPrPixmapObjectIface( KPrPixmapObject *_obj )
    : KPrObject2DIface(_obj)
{
    obj = _obj;
}

QString KPrPixmapObjectIface::fileName() const
{
    return obj->getFileName();
}

bool KPrPixmapObjectIface::swapRGB() const
{
    return obj->getPictureSwapRGB();
}

bool KPrPixmapObjectIface::grayscale() const
{
    return obj->getPictureGrayscal();
}

int KPrPixmapObjectIface::depth() const
{
    return obj->getPictureDepth();
}

int KPrPixmapObjectIface::pictureMirrorType() const
{
    return obj->getPictureMirrorType();
}

int KPrPixmapObjectIface::brightness() const
{
    return obj->getPictureBright();
}

void KPrPixmapObjectIface::setDepth(int depth )
{
    obj->setPictureDepth(depth);
}

void KPrPixmapObjectIface::setBrightness(int bright )
{
    obj->setPictureBright(bright);
}

void KPrPixmapObjectIface::setSwapRGB(bool swapRGB )
{
    obj->setPictureSwapRGB(swapRGB);
}

void KPrPixmapObjectIface::setGrayscale( bool grayscal )
{
    obj->setPictureGrayscal(grayscal);
}

void KPrPixmapObjectIface::loadImage( const QString & fileName )
{
    obj->loadPicture(fileName);
}

void KPrPixmapObjectIface::setPictureMirrorType(const QString & _mirror)
{
    if ( _mirror.lower()=="normal")
        obj->setPictureMirrorType( PM_NORMAL);
    else if ( _mirror.lower()=="horizontal")
        obj->setPictureMirrorType( PM_HORIZONTAL );
    else if ( _mirror.lower()=="vertical")
        obj->setPictureMirrorType( PM_VERTICAL );
    else if ( _mirror.lower()=="horizontalandvertical")
        obj->setPictureMirrorType( PM_HORIZONTALANDVERTICAL );
    else
        kdDebug(33001)<<" Error in setPictureMirrorType :"<<_mirror<<endl;
}
