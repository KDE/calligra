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

#include <qstring.h>
#include <qtextstream.h>
#include <qfile.h>
#include <qobject.h>

#include <kapplication.h>
#include <kdebug.h>
#include <kgenericfactory.h>
#include <kmessagebox.h>

#include <koFilterChain.h>
#include <koStore.h>
#include <koStoreDevice.h>


#include "pngexport.h"
#include "pngexportdia.h"


typedef KGenericFactory<PNGExport, KoFilter> PNGExportFactory;
K_EXPORT_COMPONENT_FACTORY( libkfopngexport, PNGExportFactory( "kformulapngfilter" ) );


PNGExport::PNGExport( KoFilter */*parent*/, const char */*name*/, const QStringList& )
    : KoFilter()
{
}


KoFilter::ConversionStatus PNGExport::convert( const QCString& from, const QCString& to )
{
    QString config;

    if ( to != "image/png" || from != "application/x-kformula" )
        return KoFilter::NotImplemented;

    KoStore* in = KoStore::createStore(m_chain->inputFile(), KoStore::Read);
    if(!in || !in->open("root")) {
        kapp->restoreOverrideCursor();
        KMessageBox::error( 0, i18n( "Failed to read data." ), i18n( "PNG export error" ) );
        delete in;
        return KoFilter::FileNotFound;
    }

    KoStoreDevice device( in );
    QDomDocument dom( "KFORMULA" );
    if ( !dom.setContent( &device, false ) ) {
        kapp->restoreOverrideCursor();
        KMessageBox::error( 0, i18n( "Malformed XML data." ), i18n( "PNG export error" ) );
        delete in;
        return KoFilter::WrongFormat;
    }
    /* input file Reading */
    //in.close();

    PNGExportDia* dialog = new PNGExportDia( dom, m_chain->outputFile() );
    dialog->exec();
    delete dialog;
    delete in;
    return KoFilter::OK;
}

#include "pngexport.moc"
