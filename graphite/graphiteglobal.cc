/* This file is part of the KDE project
   Copyright (C) 2000 Werner Trobin <wtrobin@mandrakesoft.com>

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

// This file has to be included, or kimageeffect will fail to compile!?!
#include <qimage.h>
#include <graphiteglobal.h>


const bool operator==(const Gradient &lhs, const Gradient &rhs) {

    return lhs.ca==rhs.ca && lhs.cb==rhs.cb && lhs.type==rhs.type &&
           lhs.xfactor==rhs.xfactor && lhs.yfactor==rhs.yfactor;
}

const bool operator!=(const Gradient &lhs, const Gradient &rhs) {
    return !operator==(lhs, rhs);
}

Gradient &Gradient::operator=(const Gradient &rhs) {

    ca=rhs.ca;
    cb=rhs.cb;
    type=rhs.type;
    xfactor=rhs.xfactor;
    yfactor=rhs.yfactor;
    return *this;
}

GraphiteGlobal *GraphiteGlobal::m_self=0;

GraphiteGlobal *GraphiteGlobal::self() {

    if(m_self==0L)
	m_self=new GraphiteGlobal();
    return m_self;	
}
