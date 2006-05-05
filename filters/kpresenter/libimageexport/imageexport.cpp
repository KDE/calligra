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

#include "KPrDocument.h"
#include "KPrView.h"
#include "KPrCanvas.h"
#include "imageexport.h"

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
    if ( from != "application/x-kpresenter" || to != exportFormat() )
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
    width =  int( layoutPage.ptWidth );
    height = int( layoutPage.ptHeight );
    if (extraImageAttribute())
    {
        KPrView* view = static_cast<KPrView*>( kpresenterdoc->views().first());
        if ( view ) // no view if embedded document
        {
            KPrCanvas * canvas = view->getCanvas();
            canvas->drawPageInPix( pixmap, view->getCurrPgNum()-1, 0, true, width,height );
        }
        else //when it's embedded we use just it.
        {
            pixmap = QPixmap(width, height);
            QPainter  painter(&pixmap);
            kpresenterdoc->paintContent(painter, pixmap.rect(), false);
        }
        if( !saveImage( m_chain->outputFile()))
            return KoFilter::CreationError;
        return KoFilter::OK;
    }
    return KoFilter::UserCancelled;
}


#include "imageexport.moc"

