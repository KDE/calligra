/* This file is part of the KDE project
   Copyright (C) 2005 Yolla Indria <yolla.indria@gmail.com>
   Copyright (C) 2010 KO GmbH <jos.van.den.oever@kogmbh.com>

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

#ifndef POWERPOINTIMPORT_H
#define POWERPOINTIMPORT_H

#include <KoFilter.h>

class PowerPointImport : public KoFilter
{
    Q_OBJECT
public:
    PowerPointImport(QObject *parent, const QStringList&) : KoFilter(parent) {}
    virtual ~PowerPointImport() {}
    virtual KoFilter::ConversionStatus convert(const QByteArray& from,
            const QByteArray& to);
};

#endif // POWERPOINTIMPORT_H
