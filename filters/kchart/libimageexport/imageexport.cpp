/* This file is part of the KDE project
   Copyright (C) 2005 Laurent Montel <montel@kde.org>

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

#include <qpixmap.h>
#include <qpainter.h>
//Added by qt3to4:
#include <Q3CString>

#include <kmessagebox.h>

#include <KoFilterChain.h>
#include <KoStore.h>
#include <kgenericfactory.h>
#include <KoDocument.h>

#include "imageexport.h"
#include "kchart_part.h"

ImageExport::ImageExport(KoFilter *, const char *, const QStringList&)
    : KoFilter()
{
}

ImageExport::~ImageExport()
{
}


KoFilter::ConversionStatus
ImageExport::convert(const QByteArray& from, const QByteArray& to)
{
    // Check for proper conversion.
    if ( from != "application/x-kchart" || to != exportFormat() )
        return KoFilter::NotImplemented;

    // Read the contents of the KChart file
    KoStoreDevice* storeIn = m_chain->storageFile( "root", KoStore::Read );
    if ( !storeIn ) {
	KMessageBox::error( 0, i18n("Failed to read data." ),
			    i18n( "Export Error" ) );
	return KoFilter::FileNotFound;
    }

    // Get the XML tree.
    QDomDocument  domIn;
    domIn.setContent( storeIn );
    QDomElement   docNode = domIn.documentElement();

    // Read the document from the XML tree.
    KChart::KChartPart  kchartDoc;
    if ( !kchartDoc.loadXML(0, domIn) ) {
        KMessageBox::error( 0, i18n( "Malformed XML data." ),
			    i18n( "Export Error" ) );
        return KoFilter::WrongFormat;
    }
    width = 500;
    height = 400;
    extraImageAttribute();
    pixmap = QPixmap(width, height);
    QPainter  painter(&pixmap);
    kchartDoc.paintContent(painter, pixmap.rect(), false);
    saveImage( m_chain->outputFile());
    return KoFilter::OK; 
}


#include "imageexport.moc"

