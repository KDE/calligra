// -*- Mode: c++; c-basic-offset: 4; indent-tabs-mode: nil; tab-width: 4; -*-
/* This file is part of the KDE project
   Copyright (C) 1998, 1999 Reginald Stadlbauer <reggie@kde.org>
   Copyright (C) 2005 Thorsten Zachmann <zachmann@kde.org>

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

#include "KPrObjectIface.h"

#include "KPrObject.h"
#include <kdebug.h>
#include <kapplication.h>
#include <dcopclient.h>

KPrObjectIface::KPrObjectIface( KPrObject *obj_ )
    : DCOPObject()
{
    obj = obj_;
}

int KPrObjectIface::getType() const
{
    return (int)obj->getType();
}

bool KPrObjectIface::isSelected() const
{
    return obj->isSelected();
}

float KPrObjectIface::angle() const
{
    return obj->getAngle();
}

int KPrObjectIface::shadowDistance() const
{
    return obj->getShadowDistance();
}

int KPrObjectIface::shadowDirection() const
{
    return (int)obj->getShadowDirection();
}

QColor KPrObjectIface::shadowColor() const
{
    return obj->getShadowColor();
}

int KPrObjectIface::effect() const
{
    return (int)obj->getEffect();
}

int KPrObjectIface::effect2() const
{
    return (int)obj->getEffect2();
}

int KPrObjectIface::appearStep() const
{
    return obj->getAppearStep();
}

int KPrObjectIface::subPresSteps() const
{
    return obj->getSubPresSteps();
}

bool KPrObjectIface::disappear() const
{
    return obj->getDisappear();
}

int KPrObjectIface::disappearStep() const
{
    return obj->getDisappearStep();
}

int KPrObjectIface::effect3() const
{
    return (int)obj->getEffect3();
}

void KPrObjectIface::setSelected( bool _selected )
{
    obj->setSelected(_selected);
}

void KPrObjectIface::rotate( float _angle )
{
    obj->rotate(_angle);
}

void KPrObjectIface::setShadowDistance( int _distance )
{
    obj->setShadowDistance(_distance);
}

void KPrObjectIface::shadowColor( const QColor & _color )
{
    obj->setShadowColor(_color);
}

void KPrObjectIface::setAppearTimer( int _appearTimer )
{
    obj->setAppearTimer(_appearTimer);
}

void KPrObjectIface::setDisappearTimer( int _disappearTimer )
{
    obj->setDisappearTimer(_disappearTimer);
}

void KPrObjectIface::setAppearSoundEffect( bool b )
{
    obj->setAppearSoundEffect(b);
}

void KPrObjectIface::setDisappearSoundEffect( bool b )
{
    obj->setDisappearSoundEffect(b);
}
void KPrObjectIface::setAppearSoundEffectFileName( const QString & _a_fileName )
{
    obj->setAppearSoundEffectFileName(_a_fileName);
}
void KPrObjectIface::setDisappearSoundEffectFileName( const QString &_d_fileName )
{
    obj->setDisappearSoundEffectFileName(_d_fileName);
}

void KPrObjectIface::setObjectName( const QString &_objectName )
{
    obj->setObjectName(_objectName);
}

void KPrObjectIface::setAppearStep( int _appearStep )
{
    obj->setAppearStep(_appearStep);
}

void KPrObjectIface::setDisappear( bool b )
{
    obj->setDisappear(b);
}

int KPrObjectIface::appearTimer() const
{
    return obj->getAppearTimer();
}

int KPrObjectIface::disappearTimer() const
{
    return obj->getDisappearTimer();
}

bool KPrObjectIface::appearSoundEffect() const
{
    return obj->getAppearSoundEffect();
}

bool KPrObjectIface::disappearSoundEffect() const
{
    return obj->getDisappearSoundEffect();
}

QString KPrObjectIface::appearSoundEffectFileName() const
{
    return obj->getAppearSoundEffectFileName();
}

QString KPrObjectIface::disappearSoundEffectFileName() const
{
    return obj->getDisappearSoundEffectFileName();
}

QString KPrObjectIface::typeString() const
{
    return obj->getTypeString();
}

void KPrObjectIface::setEffect(const QString & effect)
{
    if(effect=="NONE")
        obj->setEffect(EF_NONE);
    else if(effect=="COME_RIGHT")
        obj->setEffect(EF_COME_RIGHT);
    else if(effect=="COME_LEFT")
        obj->setEffect(EF_COME_LEFT);
    else if(effect=="COME_TOP")
        obj->setEffect(EF_COME_TOP);
    else if(effect=="COME_BOTTOM")
        obj->setEffect(EF_COME_BOTTOM);
    else if(effect=="COME_RIGHT_TOP")
        obj->setEffect(EF_COME_RIGHT_TOP);
    else if(effect=="COME_RIGHT_BOTTOM")
        obj->setEffect(EF_COME_RIGHT_BOTTOM);
    else if(effect=="COME_LEFT_TOP")
        obj->setEffect(EF_COME_LEFT_TOP);
    else if(effect=="COME_LEFT_BOTTOM")
        obj->setEffect(EF_COME_LEFT_BOTTOM);
    else if(effect=="WIPE_LEFT")
        obj->setEffect(EF_WIPE_LEFT);
    else if(effect=="WIPE_RIGHT")
        obj->setEffect(EF_WIPE_RIGHT);
    else if(effect=="WIPE_TOP")
        obj->setEffect(EF_WIPE_TOP);
    else if(effect=="WIPE_BOTTOM")
        obj->setEffect(EF_WIPE_BOTTOM);
    else
        kdDebug(33001)<<"Error : setEffect()\n";
}

void KPrObjectIface::setEffect3(const QString & effect)
{
    if(effect=="NONE")
        obj->setEffect3(EF3_NONE);
    else if(effect=="GO_RIGHT")
        obj->setEffect3(EF3_GO_RIGHT);
    else if(effect=="GO_LEFT")
        obj->setEffect3(EF3_GO_LEFT);
    else if(effect=="GO_TOP")
        obj->setEffect3(EF3_GO_TOP);
    else if(effect=="GO_BOTTOM")
        obj->setEffect3(EF3_GO_BOTTOM);
    else if(effect=="GO_RIGHT_TOP")
        obj->setEffect3(EF3_GO_RIGHT_TOP);
    else if(effect=="GO_RIGHT_BOTTOM")
        obj->setEffect3(EF3_GO_RIGHT_BOTTOM);
    else if(effect=="GO_LEFT_TOP")
        obj->setEffect3(EF3_GO_LEFT_TOP);
    else if(effect=="GO_LEFT_BOTTOM")
        obj->setEffect3(EF3_GO_LEFT_BOTTOM);
    else if(effect=="WIPE_LEFT")
        obj->setEffect3(EF3_WIPE_LEFT);
    else if(effect=="WIPE_RIGHT")
        obj->setEffect3(EF3_WIPE_RIGHT);
    else if(effect=="WIPE_TOP")
        obj->setEffect3(EF3_WIPE_TOP);
    else if(effect=="WIPE_BOTTOM")
        obj->setEffect3(EF3_WIPE_BOTTOM);
}

void KPrObjectIface::setProtected( bool b )
{
    obj->setProtect(b);
}

bool KPrObjectIface::isProtected() const
{
    return obj->isProtect();
}

void KPrObjectIface::setKeepRatio( bool b )
{
    obj->setKeepRatio(b);
}

bool KPrObjectIface::isKeepRatio() const
{
    return obj->isKeepRatio();
}

void KPrObjectIface::move( double x, double y )
{
    obj->setOrig( x, y );
}

void KPrObjectIface::resize( double width, double height )
{
    obj->setSize( width, height );
}
