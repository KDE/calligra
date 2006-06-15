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

#include "KPrObject2DAdaptor.h"
#include "KPrObjectAdaptor.h"

#include "KPrObject.h"
#include <kdebug.h>
#include <kapplication.h>

KPrObject2DAdaptor::KPrObject2DAdaptor(KPr2DObject *obj_)
    : KPrObjectAdaptor(obj_)
{
    obj = obj_;
}

QColor KPrObject2DAdaptor::gradientColor1() const
{
    return obj->getGColor1();
}

QColor KPrObject2DAdaptor::gradientColor2() const
{
    return obj->getGColor2();
}

void KPrObject2DAdaptor::setGUnbalanced( bool b )
{
    obj->setGUnbalanced(b);
}

void KPrObject2DAdaptor::setGXFactor( int f )
{
    obj->setGXFactor(f);
}

void KPrObject2DAdaptor::setGYFactor( int f )
{
    obj->setGYFactor(f);
}

int KPrObject2DAdaptor::xGradientFactor() const
{
    return obj->getGXFactor();
}

int KPrObject2DAdaptor::yGradientFactor() const
{
    return obj->getGYFactor();
}

QString KPrObject2DAdaptor::gradientFillType() const
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

void KPrObject2DAdaptor::setGradientType( const QString & type)
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
        kDebug(33001)<<"Error KPrObject2DAdaptor::setGradientType\n";
}

void KPrObject2DAdaptor::setFillType( const QString & type)
{
    if(type=="BRUSH")
        obj->setFillType(FT_BRUSH);
    else if(type=="GRADIENT")
        obj->setFillType(FT_GRADIENT);
    else
        kDebug(33001)<<"Error KPrObject2DAdaptor::setFillType\n";
}

void KPrObject2DAdaptor::setGradientColor1( const QColor &col )
{
    obj->setGColor1( col );
}

void KPrObject2DAdaptor::setGradientColor2( const QColor &col )
{
    obj->setGColor2( col );
}
