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
#include "cdrdocument.h"
#include "cdrodgwriter.h"
// Calligra core
#include <KoOdf.h>
#include <KoStore.h>
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
        return KoFilter::FileNotFound;
    }

    // prepare output
    KoStore* outputStore = KoStore::createStore( m_chain->outputFile(), KoStore::Write,
                                                 KoOdf::mimeType(KoOdf::Graphics), KoStore::Zip );
    if( ! outputStore ) {
        return KoFilter::StorageCreationError;
    }

    CdrOdgWriter odgWriter( outputStore );

    // translate!
    CdrParser parser;
    CdrDocument* document = parser.parse( inputFile );
    if( ! document ) {
        return KoFilter::CreationError;
    }
    if (! odgWriter.write(document)) {
        return KoFilter::CreationError;
    }

    return KoFilter::OK;
}
