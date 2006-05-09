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

#ifndef MSWORDIMPORT_H
#define MSWORDIMPORT_H

#include <KoFilter.h>
//Added by qt3to4:
#include <Q3CString>

class QDomDocument;
class QDomElement;

class MSWordImport : public KoFilter
{
    Q_OBJECT
public:
    MSWordImport( QObject* parent, const QStringList& );
    virtual ~MSWordImport();

    virtual KoFilter::ConversionStatus convert( const QByteArray& from, const QByteArray& to );

private:
    void prepareDocument( QDomDocument& mainDocument, QDomElement& framesetsElem );
};

#endif // MSWORDIMPORT_H
