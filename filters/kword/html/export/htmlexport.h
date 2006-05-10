/* This file is part of the KDE project
   Copyright (C) 1998, 1999 Reginald Stadlbauer <reggie@kde.org>
   Copyright (C) 2001 Nicolas GOUTTE <goutte@kde.org>

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

#ifndef HTMLEXPORT_H
#define HTMLEXPORT_H

#include <QString>
#include <qtextstream.h>
#include <QFile>
#include <qobject.h>
//Added by qt3to4:
#include <Q3CString>

#include <KoFilter.h>
#include <KoStore.h>


class HTMLExport : public KoFilter {

    Q_OBJECT

public:
    HTMLExport(QObject* parent, const QStringList &);
    virtual ~HTMLExport() {}

    virtual KoFilter::ConversionStatus convert( const QByteArray& from, const QByteArray& to );
};

#endif // HTMLEXPORT_H
