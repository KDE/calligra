/* This file is part of the KDE project
   Copyright (C) 2005 Inge Wallin <inge@lysator.liu.se>

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
   Boston, MA  02110-1301  USA.
*/

#include <qpicture.h>
#include <qpainter.h>

#include <kmessagebox.h>

#include <KoFilterChain.h>
#include <KoStore.h>
//#include <KoStoreDevice.h>
#include <kgenericfactory.h>

#include "kchart_part.h"

#include "svgexport.h"


typedef KGenericFactory<SvgExport, KoFilter> SvgExportFactory;
K_EXPORT_COMPONENT_FACTORY( libkchartsvgexport, SvgExportFactory( "svgexport" ) )

SvgExport::SvgExport(KoFilter *, const char *, const QStringList&) 
    : KoFilter()
{
}

SvgExport::~SvgExport()
{
}


KoFilter::ConversionStatus
SvgExport::convert(const QCString& from, const QCString& to)
{
    // Check for proper conversion.
    if ( from != "application/x-kchart" || to != "image/svg+xml" )
        return KoFilter::NotImplemented;

    // Read the contents of the KChart file
    KoStoreDevice* storeIn = m_chain->storageFile( "root", KoStore::Read );
    if ( !storeIn ) {
	KMessageBox::error( 0, i18n("Failed to read data." ), 
			    i18n( "SVG Export Error" ) );
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
			    i18n( "SVG Export Error" ) );
        return KoFilter::WrongFormat;
    }

    // Draw the actual bitmap.
    QPicture  picture;
    QPainter  painter(&picture);
    QRect     rect(QPoint(0, 0), QPoint(500, 400));
    kchartDoc.paintContent(painter, rect, false);
    painter.end();

    // Save the image.
    if ( !picture.save( m_chain->outputFile(), "SVG" ) ) {
        KMessageBox::error( 0, i18n( "Failed to write file." ), 
			    i18n( "SVG Export Error" ) );
    }

    return KoFilter::OK;
}


#include <svgexport.moc>
