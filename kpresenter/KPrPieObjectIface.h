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

#ifndef KPPIE_OBJECT_IFACE_H
#define KPPIE_OBJECT_IFACE_H

#include <dcopobject.h>
#include <dcopref.h>
#include "KPrObject2DIface.h"
#include <qstring.h>

class KPrPieObject;

class KPrPieObjectIface : public KPrObject2DIface
{
    K_DCOP
public:
    KPrPieObjectIface( KPrPieObject *obj_ );

k_dcop:
    virtual void setPieAngle( int _p_angle );
    virtual void setPieLength( int _p_len );
    virtual int pieAngle() const;
    virtual int pieLength() const;
    void setPieType( const QString & type );
    virtual void setLineBegin( const QString &);
    virtual void setLineEnd(const QString & );
    void horizontalFlip();
    void verticalFlip();

private:
    KPrPieObject *obj;
};

#endif
