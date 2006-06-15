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

#include "KPrPixmapObjectAdaptor.h"
#include "KPrPixmapObject.h"
#include "KPrObject.h"

#include <kapplication.h>

KPrPixmapObjectAdaptor::KPrPixmapObjectAdaptor( KPrPixmapObject *_obj )
    : KPrObject2DAdaptor(_obj)
{
    obj = _obj;
}

QString KPrPixmapObjectAdaptor::fileName() const
{
    return obj->getFileName();
}

bool KPrPixmapObjectAdaptor::swapRGB() const
{
    return obj->getPictureSwapRGB();
}

bool KPrPixmapObjectAdaptor::grayscale() const
{
    return obj->getPictureGrayscal();
}

int KPrPixmapObjectAdaptor::depth() const
{
    return obj->getPictureDepth();
}

int KPrPixmapObjectAdaptor::pictureMirrorType() const
{
    return obj->getPictureMirrorType();
}

int KPrPixmapObjectAdaptor::brightness() const
{
    return obj->getPictureBright();
}

void KPrPixmapObjectAdaptor::setDepth(int depth )
{
    obj->setPictureDepth(depth);
}

void KPrPixmapObjectAdaptor::setBrightness(int bright )
{
    obj->setPictureBright(bright);
}

void KPrPixmapObjectAdaptor::setSwapRGB(bool swapRGB )
{
    obj->setPictureSwapRGB(swapRGB);
}

void KPrPixmapObjectAdaptor::setGrayscale( bool grayscal )
{
    obj->setPictureGrayscal(grayscal);
}

void KPrPixmapObjectAdaptor::loadImage( const QString & fileName )
{
    obj->loadPicture(fileName);
}

void KPrPixmapObjectAdaptor::setPictureMirrorType(const QString & _mirror)
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
        kDebug(33001)<<" Error in setPictureMirrorType :"<<_mirror<<endl;
}
