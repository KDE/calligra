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

#include "KPrCubicBezierCurveObjectAdaptor.h"
#include "KPrBezierCurveObject.h"
#include "KPrUtils.h"
#include <kdebug.h>

#include <kapplication.h>

KPrCubicBezierCurveObjectAdaptor::KPrCubicBezierCurveObjectAdaptor( KPrCubicBezierCurveObject *_obj )
    : KPrObjectAdaptor(_obj)
{
    obj = _obj;
}

void KPrCubicBezierCurveObjectAdaptor::setLineBegin( const QString & type)
{
    obj->setLineBegin(lineEndBeginFromString( type ));
}

void KPrCubicBezierCurveObjectAdaptor::setLineEnd( const QString & type)
{
    obj->setLineEnd(lineEndBeginFromString( type ));
}

QString KPrCubicBezierCurveObjectAdaptor::lineBegin()const
{
    LineEnd type=obj->getLineBegin();
    return lineEndBeginName( type );
}

QString KPrCubicBezierCurveObjectAdaptor::lineEnd() const
{
    LineEnd type=obj->getLineEnd();
    return lineEndBeginName( type );
}

void KPrCubicBezierCurveObjectAdaptor::horizontalFlip()
{
    obj->flip( true );
}

void KPrCubicBezierCurveObjectAdaptor::verticalFlip()
{
    obj->flip( false );
}

#include <KPrCubicBezierCurveObjectAdaptor.moc>
