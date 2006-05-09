/*
    Copyright (C) 2000, S.R.Haque <shaheedhaque@hotmail.com>.
    This file is part of the KDE project

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

DESCRIPTION
*/

#include <config.h>
#include <qdom.h>
#include <q3cstring.h>
#include <kdebug.h>
#include <kgenericfactory.h>
#include <KoFilterChain.h>
#include <KoStoreDevice.h>
#include <core/vdocument.h>

#include "wmfimport.h"
#include "wmfimportparser.h"

typedef KGenericFactory<WMFImport> WMFImportFactory;
K_EXPORT_COMPONENT_FACTORY( libwmfimport, WMFImportFactory( "kofficefilters" ) )


WMFImport::WMFImport( KoFilter *, const char *, const QStringList&) :
        KoFilter(parent)
{
}

WMFImport::~WMFImport()
{
}

KoFilter::ConversionStatus WMFImport::convert( const QByteArray& from, const QByteArray& to )
{
    if( to != "application/x-karbon" || from != "image/x-wmf" )
    return KoFilter::NotImplemented;

    WMFImportParser wmfParser;
    if( !wmfParser.load( m_chain->inputFile() ) ) {
        return KoFilter::WrongFormat;
    }

    // Do the conversion!
    VDocument document;
    if (!wmfParser.play( document )) {
        return KoFilter::WrongFormat;
    }

    KoStoreDevice* out = m_chain->storageFile( "root", KoStore::Write );
    if( !out ) {
        kError(3800) << "Unable to open output file!" << endl;
        return KoFilter::StorageCreationError;
    }
    QDomDocument outdoc = document.saveXML();
    Q3CString content = outdoc.toCString();
    // kDebug() << " content : " << content << endl;
    out->write( content , content.length() );

    return KoFilter::OK;
}


#include <wmfimport.moc>
