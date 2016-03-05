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
   Boston, MA 02110-1301, USA.
*/

#include "XFigImportFilter.h"

// filter
#include "XFigParser.h"
#include "XFigDocument.h"
#include "XFigOdgWriter.h"
// Calligra core
#include <KoOdf.h>
#include <KoStore.h>
#include <KoFilterChain.h>
// KF5
#include <kpluginfactory.h>
// Qt
#include <QFile>


K_PLUGIN_FACTORY_WITH_JSON(XFigImportFactory, "calligra_filter_xfig2odg.json",
                           registerPlugin<XFigImportFilter>();)


XFigImportFilter::XFigImportFilter( QObject* parent, const QVariantList& )
  : KoFilter(parent)
{
}

XFigImportFilter::~XFigImportFilter()
{
}

KoFilter::ConversionStatus
XFigImportFilter::convert( const QByteArray& from, const QByteArray& to )
{
    if ((from != "image/x-xfig") ||
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

    XFigOdgWriter odgWriter( outputStore );

    // translate!
    XFigDocument* document = XFigParser::parse( &inputFile );
    if( ! document ) {
        return KoFilter::CreationError;
    }

    const bool isWritten = odgWriter.write(document);

    delete document;

    return isWritten ? KoFilter::OK : KoFilter::CreationError;
}

#include "XFigImportFilter.moc"
