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

float KPresenterObjectIface::angle()
{
    return obj->getAngle();
}

int KPresenterObjectIface::shadowDistance()
{
    return obj->getShadowDistance();
}

int KPresenterObjectIface::shadowDirection()
{
    return (int)obj->getShadowDirection();
}

QColor KPresenterObjectIface::shadowColor()
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

int KPresenterObjectIface::effect()
{
    return (int)obj->getEffect();
}

int KPresenterObjectIface::effect2()
{
    return (int)obj->getEffect2();
}

int KPresenterObjectIface::presNum()
{
    return obj->getPresNum();
}

int KPresenterObjectIface::subPresSteps()
{
    return obj->getSubPresSteps();
}

bool KPresenterObjectIface::disappear()
{
    return obj->getDisappear();
}

int KPresenterObjectIface::disappearNum()
{
    return obj->getDisappearNum();
}

int KPresenterObjectIface::effect3()
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

void KPresenterObjectIface::setSticky( bool b )
{
    obj->setSticky(b);
}

bool KPresenterObjectIface::isSticky() const
{
    return obj->isSticky();
}

void KPresenterObjectIface::shadowColor( const QColor & _color )
{
    obj->setShadowColor(_color);
}

void KPresenterObjectIface::setAppearTimer( int _appearTimer )
{
    obj->setAppearTimer(_appearTimer);
}

void KPresenterObjectIface::setDisappearTimer( int _disappearTimer )
{
    obj->setDisappearTimer(_disappearTimer);
}

void KPresenterObjectIface::setAppearSoundEffect( bool b )
{
    obj->setAppearSoundEffect(b);
}

void KPresenterObjectIface::setDisappearSoundEffect( bool b )
{
    obj->setDisappearSoundEffect(b);
}
void KPresenterObjectIface::setAppearSoundEffectFileName( const QString & _a_fileName )
{
    obj->setAppearSoundEffectFileName(_a_fileName);
}
void KPresenterObjectIface::setDisappearSoundEffectFileName( const QString &_d_fileName )
{
    obj->setDisappearSoundEffectFileName(_d_fileName);
}

void KPresenterObjectIface::setPresNum( int _presNum )
{
    obj->setPresNum(_presNum);
}

void KPresenterObjectIface::setDisappear( bool b )
{
    obj->setDisappear(b);
}

int KPresenterObjectIface::appearTimer() const
{
    return obj->getAppearTimer();
}

int KPresenterObjectIface::disappearTimer() const
{
    return obj->getDisappearTimer();
}

bool KPresenterObjectIface::appearSoundEffect() const
{
    return obj->getAppearSoundEffect();
}

bool KPresenterObjectIface::disappearSoundEffect() const
{
    return obj->getDisappearSoundEffect();
}

QString KPresenterObjectIface::appearSoundEffectFileName() const
{
    return obj->getAppearSoundEffectFileName();
}

QString KPresenterObjectIface::disappearSoundEffectFileName() const
{
    return obj->getDisappearSoundEffectFileName();
}

QString KPresenterObjectIface::typeString() const
{
    return obj->getTypeString();
}
