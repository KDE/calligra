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

#include "KPresenterObjectIface.h"

#include "kpobject.h"

#include <kapplication.h>
#include <dcopclient.h>

KPresenterObjectIface::KPresenterObjectIface( KPObject *obj_ )
    : DCOPObject()
{
   obj = obj_;
}

int KPresenterObjectIface::getType()
{
    return (int)obj->getType();
}

QRect KPresenterObjectIface::getBoundingRect()
{
    //FIXME
    return QRect(); //obj->getBoundingRect( );
}

bool KPresenterObjectIface::isSelected()
{
    return obj->isSelected();
}

float KPresenterObjectIface::getAngle()
{
    return obj->getAngle();
}

int KPresenterObjectIface::getShadowDistance()
{
    return obj->getShadowDistance();
}

int KPresenterObjectIface::getShadowDirection()
{
    return (int)obj->getShadowDirection();
}

QColor KPresenterObjectIface::getShadowColor()
{
    return obj->getShadowColor();
}

QSize KPresenterObjectIface::getSize()
{
    return QSize();//return obj->getSize();
}

QPoint KPresenterObjectIface::getOrig()
{
    return QPoint();
    //return obj->getOrig();
}

int KPresenterObjectIface::getEffect()
{
    return (int)obj->getEffect();
}

int KPresenterObjectIface::getEffect2()
{
    return (int)obj->getEffect2();
}

int KPresenterObjectIface::getPresNum()
{
    return obj->getPresNum();
}

int KPresenterObjectIface::getSubPresSteps()
{
    return obj->getSubPresSteps();
}

bool KPresenterObjectIface::getDisappear()
{
    return obj->getDisappear();
}

int KPresenterObjectIface::getDisappearNum()
{
    return obj->getDisappearNum();
}

int KPresenterObjectIface::getEffect3()
{
    return (int)obj->getEffect3();
}


void KPresenterObjectIface::setSelected( bool _selected )
{
    obj->setSelected(_selected);
}

void KPresenterObjectIface::rotate( float _angle )
{
    obj->rotate(_angle);
}

void KPresenterObjectIface::setShadowDistance( int _distance )
{
    obj->setShadowDistance(_distance);
}

float KPresenterObjectIface::getAngle() const
{
    return obj->getAngle();
}

void KPresenterObjectIface::setSticky( bool b )
{
    obj->setSticky(b);
}

bool KPresenterObjectIface::isSticky() const
{
    return obj->isSticky();
}

