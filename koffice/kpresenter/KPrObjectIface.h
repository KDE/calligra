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

#ifndef KPRESENTER_OBJ_IFACE_H
#define KPRESENTER_OBJ_IFACE_H

#include <dcopobject.h>
#include <dcopref.h>

#include <qstring.h>
#include <qcolor.h>

class KPrObject;

class KPrObjectIface : public DCOPObject
{
    K_DCOP
public:
    KPrObjectIface( KPrObject *obj_ );

k_dcop:
    int getType() const;

    /// @since 1.4
    void move( double x, double y );
    /// @since 1.4
    void resize( double width, double height );

    bool isSelected() const;
    float angle() const;
    int shadowDistance() const;
    int shadowDirection() const;
    QColor shadowColor() const;
    int effect() const;
    int effect2() const;
    int appearStep() const;
    int subPresSteps() const;
    bool disappear() const;
    int disappearStep() const;
    int effect3() const;

    void setEffect(const QString & effect);
    void setEffect3(const QString & effect);

    void setSelected( bool _selected );
    void rotate( float _angle );
    void setShadowDistance( int _distance );

    void shadowColor( const QColor & _color );

    void setAppearTimer( int _appearTimer );
    void setDisappearTimer( int _disappearTimer );

    void setAppearSoundEffect( bool b );
    void setDisappearSoundEffect( bool b );
    void setAppearSoundEffectFileName( const QString & _a_fileName );
    void setDisappearSoundEffectFileName( const QString &_d_fileName );

    void setObjectName( const QString &_objectName );

    void setAppearStep( int _appearStep );

    void setDisappear( bool b );

    int appearTimer() const;
    int disappearTimer() const;
    bool appearSoundEffect() const;
    bool disappearSoundEffect() const;
    QString appearSoundEffectFileName() const;
    QString disappearSoundEffectFileName() const;
    QString typeString() const;

    void setProtected( bool b );
    bool isProtected() const;

    void setKeepRatio( bool b );
    bool isKeepRatio() const;

private:
    KPrObject *obj;
};

#endif
