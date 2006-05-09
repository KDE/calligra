/* This file is part of the KDE project
   Copyright (C) 2001 Enno Bartels <ebartels@nwn.de>

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

#ifndef APPLIXGRAPHICIMPORT_H
#define APPLIXGRAPHICIMPORT_H

#include <KoFilter.h>
//Added by qt3to4:
#include <Q3CString>


class APPLIXGRAPHICImport : public KoFilter {

    Q_OBJECT

public:
    APPLIXGRAPHICImport (QObject* parent, const QStringList&);
    virtual ~APPLIXGRAPHICImport() {}

    virtual KoFilter::ConversionStatus convert( const QByteArray& from, const QByteArray& to );
};




// Kleinstes Objekt
class applixGraphicsLine
{
  public:
   int offX;
   int offY;
   int reColor;
   int thickNess;
   int ptX[5];
   int ptY[5];
   QString str;

   applixGraphicsLine ();   // Constructor
};

class applixGraphicsRect : public applixGraphicsLine
{
  public:
   int bf[7];
   int lf[7];

   int sh[6];

   int pa[8];
   int vs[3];

   int xr[2]; //XYRAD

   applixGraphicsRect (); // Constructor
};

#endif // APPLIXGRAPHICIMPORT_H
