/* This file is part of the Calligra project, made within the KDE community.

   Copyright 2012 Friedrich W. H. Kossebau <kossebau@kde.org>

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

#include "cdrimportfilter.h"

// filter
#include "cdrparser.h"
// Karbon
#include <KarbonPart.h>
#include <KarbonDocument.h>
// Calligra core
#include <KoFilterChain.h>
// Qt
#include <QtCore/QFile>


CdrImportFilter::CdrImportFilter( QObject* parent, const QVariantList& )
  : KoFilter(parent)
{
}

CdrImportFilter::~CdrImportFilter()
{
}

KoFilter::ConversionStatus
CdrImportFilter::convert( const QByteArray& from, const QByteArray& to )
{
    if ((from != "application/vnd.corel-draw") ||
        (to   != "application/vnd.oasis.opendocument.graphics")) {
        return KoFilter::NotImplemented;
    }

    // prepare input
    QFile inputFile( m_chain->inputFile() );
    if( ! inputFile.open(QIODevice::ReadOnly) )
    {
        inputFile.close();
        return KoFilter::FileNotFound;
    }

    // prepare output
    KarbonPart* part = dynamic_cast<KarbonPart*>( m_chain->outputDocument() );
    if (! part)
        return KoFilter::CreationError;

    // translate!
    CdrParser parser;
    if (! parser.parse(&part->document(), inputFile)) {
        inputFile.close();
        return KoFilter::CreationError;
    }

    return KoFilter::OK;
}
