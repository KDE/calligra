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
   the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
   Boston, MA 02111-1307, USA.
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

