// -*- Mode: c++; c-basic-offset: 4; indent-tabs-mode: nil; tab-width: 4; -*-
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
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
*/

#include "KPrObjectIface.h"

#include "KPrObject.h"
#include <kdebug.h>
#include <kapplication.h>
#include <dcopclient.h>

KPresenterObjectIface::KPresenterObjectIface( KPObject *obj_ )
    : DCOPObject()
{
    obj = obj_;
}

int KPresenterObjectIface::getType() const
{
    return (int)obj->getType();
}

bool KPresenterObjectIface::isSelected() const
{
    return obj->isSelected();
}

float KPresenterObjectIface::angle() const
{
    return obj->getAngle();
}

int KPresenterObjectIface::shadowDistance() const
{
    return obj->getShadowDistance();
}

int KPresenterObjectIface::shadowDirection() const
{
    return (int)obj->getShadowDirection();
}

QColor KPresenterObjectIface::shadowColor() const
{
    return obj->getShadowColor();
}

int KPresenterObjectIface::effect() const
{
    return (int)obj->getEffect();
}

int KPresenterObjectIface::effect2() const
{
    return (int)obj->getEffect2();
}

int KPresenterObjectIface::appearStep() const
{
    return obj->getAppearStep();
}

int KPresenterObjectIface::subPresSteps() const
{
    return obj->getSubPresSteps();
}

bool KPresenterObjectIface::disappear() const
{
    return obj->getDisappear();
}

int KPresenterObjectIface::disappearStep() const
{
    return obj->getDisappearStep();
}

int KPresenterObjectIface::effect3() const
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

void KPresenterObjectIface::setObjectName( const QString &_objectName )
{
    obj->setObjectName(_objectName);
}

void KPresenterObjectIface::setAppearStep( int _appearStep )
{
    obj->setAppearStep(_appearStep);
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

void KPresenterObjectIface::setEffect(const QString & effect)
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

void KPresenterObjectIface::setEffect3(const QString & effect)
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

void KPresenterObjectIface::setProtected( bool b )
{
    obj->setProtect(b);
}

bool KPresenterObjectIface::isProtected() const
{
    return obj->isProtect();
}

void KPresenterObjectIface::setKeepRatio( bool b )
{
    obj->setKeepRatio(b);
}

bool KPresenterObjectIface::isKeepRatio() const
{
    return obj->isKeepRatio();
}

void KPresenterObjectIface::move( double x, double y )
{
    obj->setOrig( x, y );
}

void KPresenterObjectIface::resize( double width, double height )
{
    obj->setSize( width, height );
}
