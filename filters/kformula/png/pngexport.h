/* This file is part of the KDE project
   Copyright (C) 2002 Ulrich Kuettler <ulrich.kuettler@mailbox.tu-dresden.de>

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

#ifndef PNGEXPORT_H
#define PNGEXPORT_H

class QCString;
class QStringList;

#include <koFilter.h>

class PNGExport : public KoFilter
{
    Q_OBJECT
public:
    PNGExport( KoFilter *parent, const char *name, const QStringList& );
    virtual ~PNGExport() {}

    virtual KoFilter::ConversionStatus convert( const QCString& from, const QCString& to );
};

#endif // PNGEXPORT_H
