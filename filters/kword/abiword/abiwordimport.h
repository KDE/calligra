/*
   This file is part of the KDE project
   Copyright (C) 2001 Nicolas GOUTTE <goutte@kde.org>

   This program is free software; you can redistribute it and/or
   modify it under the terms of the GNU General Public License
   as published by the Free Software Foundation; either version 2
   of the License, or (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
*/

/*
   This file is based on the old file:
    koffice/filters/kword/ascii/asciiimport.h

   The old file was copyrighted by
    Copyright (C) 1998, 1999 Reginald Stadlbauer <reggie@kde.org>
    Copyright (C) 2000 Michael Johnson <mikej@xnet.com>

   The old file was licensed under the terms of the GNU Library General Public
   License version 2.
*/

#ifndef ABIWORDIMPORT_H
#define ABIWORDIMPORT_H

#include <QString>
#include <QFile>
#include <QObject>
//Added by qt3to4:
#include <QByteArray>

#include <KoFilter.h>
#include <KoStore.h>

class ABIWORDImport : public KoFilter {

    Q_OBJECT

public:
    ABIWORDImport(QObject* parent, const QStringList &);
    ABIWORDImport();
    virtual ~ABIWORDImport() {}

    virtual KoFilter::ConversionStatus convert( const QByteArray& from, const QByteArray& to );
};
#endif // ABIWORDIMPORT_H
