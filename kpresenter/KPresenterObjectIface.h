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

#ifndef KPRESENTER_OBJ_IFACE_H
#define KPRESENTER_OBJ_IFACE_H

#include <dcopobject.h>
#include <dcopref.h>

#include <qstring.h>
#include <qcolor.h>

class KPObject;

class KPresenterObjectIface : virtual public DCOPObject
{
    K_DCOP
public:
    KPresenterObjectIface( KPObject *obj_ );

k_dcop:
    int getType();
    QRect getBoundingRect();
    bool isSelected();
    float getAngle();
    int getShadowDistance();
    int getShadowDirection();
    QColor getShadowColor();
    QSize getSize();
    QPoint getOrig();
    int getEffect();
    int getEffect2();
    int getPresNum();
    int getSubPresSteps();
    bool getDisappear();
    int getDisappearNum();
    int getEffect3();

    void setSelected( bool _selected );
    void rotate( float _angle );
    void setShadowDistance( int _distance );
    float getAngle() const;
    void setSticky( bool b );
    bool isSticky() const ;

private:
    KPObject *obj;

};

#endif
