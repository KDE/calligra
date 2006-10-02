/* This file is part of the KDE project
   Copyright (C) 2006 Martin Pfeiffer <hubipete@gmx.net>

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

#include "KFormula13Import.h"
#include "KFormula13ContentHandler.h"
#include <KoFilterChain.h>
#include <KoStoreDevice.h>
#include <kgenericfactory.h>
#include <kmessagebox.h>
#include <QXmlSimpleReader>
#include <QXmlInputSource>
#include <QApplication>
#include <QFile>

typedef KGenericFactory<KFormula13Import> KFormula13ImportFactory;
K_EXPORT_COMPONENT_FACTORY( libkformula13import,
                            KFormula13ImportFactory( "kofficefilters" ) )


KFormula13Import::KFormula13Import( QObject* parent, const QStringList& )
                 : KoFilter(parent)
{
}

KFormula13Import::~KFormula13Import()
{
}

KoFilter::ConversionStatus KFormula13Import::convert( const QByteArray& from,
                                                      const QByteArray& to )
{
    if ( to != "application/mathml+xml" || from != "application/x-kformula" )
        return KoFilter::NotImplemented;

    KoStoreDevice* in = m_chain->storageFile( "root", KoStore::Read );
    if( !in )
    {
        QApplication::restoreOverrideCursor();
        KMessageBox::error( 0, i18n( "Failed to read data." ),
                            i18n( "KFormula Import Error" ) );
        return KoFilter::StorageCreationError;
    }

    KoStoreDevice* out = m_chain->storageFile( "root", KoStore::Write );
    if( !out )
    {
        kError(30506) << "KFormula13 Import unable to open output file! (Root)" << endl;
        KMessageBox::error( NULL, i18n("Unable to save main document."),
                            i18n("KFormula1.3 Import Filter"), 0 );
        return KoFilter::StorageCreationError;
    }

    KFormula13ContentHandler handler;
    QXmlInputSource source( in );
    QXmlSimpleReader reader;
    reader.setContentHandler( &handler );
    reader.parse( &source );
    
    return KoFilter::OK;
}

#include "KFormula13Import.moc"
