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

#include <QString>
#include <QTextStream>
#include <QFile>
#include <QObject>
#include <QByteArray>

#include <kapplication.h>
#include <kdebug.h>
#include <kgenericfactory.h>
#include <kmessagebox.h>

#include <KoFilterChain.h>
#include <KoStore.h>
#include <KoStoreDevice.h>


#include "pngexport.h"
#include "pngexportdia.h"


typedef KGenericFactory<PNGExport> PNGExportFactory;
K_EXPORT_COMPONENT_FACTORY( libkfopngexport, PNGExportFactory( "kofficefilters" ) )


PNGExport::PNGExport( QObject* parent, const QStringList& )
    : KoFilter(parent)
{
}


KoFilter::ConversionStatus PNGExport::convert( const QByteArray& from, const QByteArray& to )
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
