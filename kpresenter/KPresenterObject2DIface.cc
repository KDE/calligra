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

#include "KPresenterObject2DIface.h"
#include "KPresenterObjectIface.h"

#include "kpobject.h"
#include <kdebug.h>
#include <kapplication.h>
#include <dcopclient.h>

KPresenterObject2DIface::KPresenterObject2DIface(KP2DObject *obj_)
    : KPresenterObjectIface(obj_)
{
    obj = obj_;
}

QColor KPresenterObject2DIface::gradientColor1() const
{
    return obj->getGColor1();
}

QColor KPresenterObject2DIface::gradientColor2() const
{
    return obj->getGColor2();
}

void KPresenterObject2DIface::setGUnbalanced( bool b )
{
    obj->setGUnbalanced(b);
}

void KPresenterObject2DIface::setGXFactor( int f )
{
    obj->setGXFactor(f);
}

void KPresenterObject2DIface::setGYFactor( int f )
{
    obj->setGYFactor(f);
}

int KPresenterObject2DIface::xGradientFactor() const
{
    return obj->getGXFactor();
}

int KPresenterObject2DIface::yGradientFactor() const
{
    return obj->getGYFactor();
}

QString KPresenterObject2DIface::gradientFillType() const
{
    switch(obj->getFillType())
    {
    case FT_BRUSH:
        return QString("BRUSH");
    case FT_GRADIENT:
        return QString("GRADIENT");
    }
    return QString::null;
}

void KPresenterObject2DIface::setGradientType( const QString & type)
{
    if(type=="PLAIN")
        obj->setGType(BCT_PLAIN);
    else if(type=="GHORZ")
        obj->setGType(BCT_GHORZ);
    else if(type=="GVERT")
        obj->setGType(BCT_GVERT);
    else if(type=="GDIAGONAL1")
        obj->setGType(BCT_GDIAGONAL1);
    else if(type=="GDIAGONAL2")
        obj->setGType(BCT_GDIAGONAL2);
    else if(type=="GCIRCLE")
        obj->setGType(BCT_GCIRCLE);
    else if(type=="GRECT")
        obj->setGType(BCT_GRECT);
    else if(type=="GPIPECROSS")
        obj->setGType(BCT_GPIPECROSS);
    else if(type=="GPYRAMID")
        obj->setGType(BCT_GPYRAMID);
    else
        kdDebug(33001)<<"Error KPresenterObject2DIface::setGradientType\n";
}

void KPresenterObject2DIface::setFillType( const QString & type)
{
    if(type=="BRUSH")
        obj->setFillType(FT_BRUSH);
    else if(type=="GRADIENT")
        obj->setFillType(FT_GRADIENT);
    else
        kdDebug(33001)<<"Error KPresenterObject2DIface::setFillType\n";
}

void KPresenterObject2DIface::setGradientColor1( const QColor &col )
{
    obj->setGColor1( col );
}

void KPresenterObject2DIface::setGradientColor2( const QColor &col )
{
    obj->setGColor2( col );
}
