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

#ifndef kpgradientcollection_h
#define kpgradientcollection_h

#include <qlist.h>
#include <global.h>

class QPixmap;
class KPGradient;

/******************************************************************/
/* Class: KPGradientCollection                                    */
/******************************************************************/

class KPGradientCollection
{
public:
    KPGradientCollection()
    { gradientList.setAutoDelete( true ); }

    virtual ~KPGradientCollection()
    { gradientList.clear(); }

    virtual QPixmap* getGradient( QColor _color1, QColor _color2, BCType _bcType, QSize _size,
				  bool _unbalanced, int _xfactor, int _yfactor, bool addref = true );

    virtual void removeRef( QColor _color1, QColor _color2, BCType _bcType, QSize _size,
			    bool _unbalanced, int _xfactor, int _yfactor );

protected:
    virtual int inGradientList( QColor _color1, QColor _color2, BCType _bcType, QSize _size,
				bool _unbalanced, int _xfactor, int _yfactor );

    QList<KPGradient> gradientList;

};

#endif
