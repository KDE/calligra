/* This file is part of the KDE project
 * Copyright (C) 2008 Thorsten Zachmann <zachmann@kde.org>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or ( at your option ) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public License
 * along with this library; see the file COPYING.LIB.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
 */

#include "KPrEndOfSlideShowPage.h"

#include <klocale.h>
#include <KoShapeLayer.h>
#include <KoShapeFactoryBase.h>
#include <KoShapeRegistry.h>
#include <KoColorBackground.h>
#include <KoTextShapeData.h>
#include <QTextCursor>
#include <QTextDocument>

#include "KPrDocument.h"
#include "KPrMasterPage.h"

#include <kdebug.h>

KPrEndOfSlideShowPage::KPrEndOfSlideShowPage( const QRectF & screenRect, KPrDocument * document )
: KPrPage( new KPrMasterPage(), document )
{
    qreal ratio = screenRect.width() / screenRect.height();
    KoPageLayout pageLayout;
    pageLayout.height = 510;

    pageLayout.width = 510 * ratio;
    pageLayout.leftMargin = 0;
    pageLayout.rightMargin = 0;
    pageLayout.topMargin = 0;
    pageLayout.bottomMargin = 0;
    pageLayout.orientation = screenRect.width() > screenRect.height() ? KoPageFormat::Landscape : KoPageFormat::Portrait;
    pageLayout.bindingSide = 0;
    pageLayout.pageEdge = 0;
    pageLayout.format = KoPageFormat::IsoA3Size; 

    masterPage()->setPageLayout( pageLayout );
    masterPage()->setBackground( new KoColorBackground( Qt::black ) );

    KoShapeLayer* layer = new KoShapeLayer;
    addShape( layer );

    KoShapeFactoryBase *factory = KoShapeRegistry::instance()->value( "TextShapeID" );
    Q_ASSERT( factory );
    if ( factory ) {
        KoShape * textShape = factory->createDefaultShape();
        QTextDocument * document = qobject_cast<KoTextShapeData*>( textShape->userData() )->document();
        QTextCursor cursor( document );
        QTextCharFormat format;
        format.setForeground( QBrush( Qt::white ) );
        cursor.mergeCharFormat( format );
        cursor.insertText( i18n("End of presentation. Click to exit." ) );
        textShape->setPosition( QPointF( 10.0, 10.0 ) );
        textShape->setSize( QSizeF( pageLayout.width - 20.0, pageLayout.height - 20.0 ) );
        layer->addShape( textShape );
    }
}

KPrEndOfSlideShowPage::~KPrEndOfSlideShowPage()
{
    delete masterPage();
}

