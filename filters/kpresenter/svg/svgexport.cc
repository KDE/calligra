/* This file is part of the KDE project
   Copyright (C) 2005 Laurent Montel <montel@kde.org>
   code based on svgexport.cc from Inge Wallin <inge@lysator.liu.se>
   
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

#include <kmessagebox.h>

#include <KoFilterChain.h>
#include <KoStore.h>
//#include <KoStoreDevice.h>
#include <kgenericfactory.h>

#include "KPrDocument.h"
#include "KPrView.h"
#include "KPrCanvas.h"

#include "svgexport.h"


typedef KGenericFactory<SvgExport> SvgExportFactory;
K_EXPORT_COMPONENT_FACTORY( libkpresentersvgexport, SvgExportFactory( "svgexport" ) )

SvgExport::SvgExport(KoFilter *, const char *, const QStringList&) 
    : KoFilter(parent)
{
}

SvgExport::~SvgExport()
{
}


KoFilter::ConversionStatus
SvgExport::convert(const QByteArray& from, const QByteArray& to)
{
    KoDocument * document = m_chain->inputDocument();

    if ( !document )
        return KoFilter::StupidError;

    if ( strcmp(document->className(), "KPrDocument") != 0)
    {
        kWarning() << "document isn't a KPrDocument but a "
                     << document->className() << endl;
        return KoFilter::NotImplemented;
    }

    // Check for proper conversion.
    if ( from != "application/x-kpresenter" || to != "image/svg+xml" )
    {
        kWarning() << "Invalid mimetypes " << to << " " << from << endl;
        return KoFilter::NotImplemented;
    }
    KPrDocument * kpresenterdoc = const_cast<KPrDocument *>(static_cast<const KPrDocument *>(document));

    if ( kpresenterdoc->mimeType() != "application/x-kpresenter" )
    {
        kWarning() << "Invalid document mimetype " << kpresenterdoc->mimeType() << endl;
        return KoFilter::NotImplemented;
    }
    KoPageLayout layoutPage= kpresenterdoc->pageLayout();
    int width =  int( layoutPage.ptWidth );
    int height = int( layoutPage.ptHeight );
    
    Q3Picture  picture;
    QPainter  painter(&picture);
    QRect     rect(QPoint(0, 0), QPoint(width, height));
    kpresenterdoc->paintContent(painter, rect, false);
    painter.end();
    
    if ( !picture.save( m_chain->outputFile(), "SVG" ) ) {
        KMessageBox::error( 0, i18n( "Failed to write file." ),
                            i18n( "SVG Export Error" ) );
    }
	
    return KoFilter::OK;
}


#include "svgexport.moc"


