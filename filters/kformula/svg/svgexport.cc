/* This file is part of the KDE project
   Copyright (C) 2005 Inge Wallin <inge@lysator.liu.se>
   Copyright (C) 2005 Fredrik Edemar <f_edemar@linux.se>

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

#include <q3picture.h>
#include <qpainter.h>
//Added by qt3to4:
#include <Q3CString>

#include <kapplication.h>
#include <kmessagebox.h>

#include <KoFilterChain.h>
#include <KoStore.h>
//#include <KoStoreDevice.h>
#include <kgenericfactory.h>

#include <kformulacontainer.h>
#include "kformuladocument.h"

#include "svgexport.h"


typedef KGenericFactory<SvgExport, KoFilter> SvgExportFactory;
K_EXPORT_COMPONENT_FACTORY( libkfosvgexport, SvgExportFactory( "svgexport" ) )

SvgExport::SvgExport(KoFilter *, const char *, const QStringList&) 
    : KoFilter()
{
}

SvgExport::~SvgExport()
{
}


KoFilter::ConversionStatus
SvgExport::convert(const QByteArray& from, const QByteArray& to)
{
    // Check for proper conversion.
    if ( from != "application/x-kformula" || to != "image/svg+xml" )
        return KoFilter::NotImplemented;

    // Read the contents of the KFormula file
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
    KFormula::DocumentWrapper* wrapper = new KFormula::DocumentWrapper( KGlobal::config(), 0 );
    KFormula::Document* kformulaDoc = new KFormula::Document;
    wrapper->document( kformulaDoc );
    KFormula::Container* formula = kformulaDoc->createFormula();

    if ( !kformulaDoc->loadXML( domIn ) ) {
        KMessageBox::error( 0, i18n( "Malformed XML data." ), 
			    i18n( "SVG Export Error" ) );
        return KoFilter::WrongFormat;
    }

    // Draw the actual bitmap.
    Q3Picture  picture;
    QPainter  painter(&picture);
    QRect     rect(QPoint(0, 0), QPoint(500, 400));
    formula->draw( painter, rect, false );
    painter.end();

    // Save the image.
    if ( !picture.save( m_chain->outputFile(), "SVG" ) ) {
        KMessageBox::error( 0, i18n( "Failed to write file." ), 
			    i18n( "SVG Export Error" ) );
    }

    delete formula;
    delete wrapper;
    return KoFilter::OK;
}


#include <svgexport.moc>
#include <kglobal.h>
