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
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
*/

#include <qstring.h>
#include <qtextstream.h>
#include <qfile.h>
#include <qobject.h>

#include <kapplication.h>
#include <kdebug.h>
#include <kgenericfactory.h>
#include <kmessagebox.h>

#include <KoFilterChain.h>
#include <KoStore.h>
#include <KoStoreDevice.h>


#include "pngexport.h"
#include "pngexportdia.h"


typedef KGenericFactory<PNGExport, KoFilter> PNGExportFactory;
K_EXPORT_COMPONENT_FACTORY( libkfopngexport, PNGExportFactory( "kofficefilters" ) )


PNGExport::PNGExport( KoFilter */*parent*/, const char */*name*/, const QStringList& )
    : KoFilter()
{
}


KoFilter::ConversionStatus PNGExport::convert( const QCString& from, const QCString& to )
{
    if ( to != "image/png" || from != "application/x-kformula" )
        return KoFilter::NotImplemented;

    KoStoreDevice* in = m_chain->storageFile( "root", KoStore::Read );
    if(!in) {
        kapp->restoreOverrideCursor();
        KMessageBox::error( 0, i18n( "Failed to read data." ), i18n( "PNG Export Error" ) );
        return KoFilter::FileNotFound;
    }

    QDomDocument dom( "KFORMULA" );
    if ( !dom.setContent( in, false ) ) {
        kapp->restoreOverrideCursor();
        KMessageBox::error( 0, i18n( "Malformed XML data." ), i18n( "PNG Export Error" ) );
        return KoFilter::WrongFormat;
    }

    PNGExportDia* dialog = new PNGExportDia( dom, m_chain->outputFile() );
    dialog->exec();
    delete dialog;
    return KoFilter::OK;
}

#include "pngexport.moc"
