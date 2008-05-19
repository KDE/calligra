/* This file is part of the KOffice project
   Copyright (C) 2002 Werner Trobin <trobin@kde.org>
   Copyright (C) 2002 David Faure <faure@kde.org>

   This program is free software; you can redistribute it and/or
   modify it under the terms of the GNU General Public
   License version 2 as published by the Free Software Foundation.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; see the file COPYING.  If not, write to
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
*/

#ifndef MSWORDODFIMPORT_H
#define MSWORDODFIMPORT_H

#include <KoFilter.h>
#include <KoXmlWriter.h>
//Added by qt3to4:
#include <QByteArray>

//class QDomDocument;
//class QDomElement;

class MSWordOdfImport : public KoFilter
{
    Q_OBJECT
public:
    MSWordOdfImport( QObject* parent, const QStringList& );
    virtual ~MSWordOdfImport();

    virtual KoFilter::ConversionStatus convert( const QByteArray& from, const QByteArray& to );

private:
    class Private;
    Private* d;

    bool createContent( KoXmlWriter* contentWriter, KoXmlWriter* bodyWriter );
};

#endif // MSWORDODFIMPORT_H
