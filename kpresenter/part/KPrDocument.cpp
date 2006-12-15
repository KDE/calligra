/* This file is part of the KDE project
   Copyright (C) 2006 Thorsten Zachmann <zachmann@kde.org>

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

#include "KPrDocument.h"

#include <kcommand.h>

#include <KoShapeManager.h>

#include "KPrCanvas.h"
#include "KPrView.h"
#include "KPrPage.h"

KPrDocument::KPrDocument( QWidget* parentWidget, QObject* parent, bool singleViewMode )
: KoDocument( parentWidget, parent, singleViewMode )
, m_commandHistory( new KCommandHistory() )    
, m_page( new KPrPage() )    
{
}

KPrDocument::~KPrDocument()
{
}

void KPrDocument::paintContent( QPainter &painter, const QRect &rect, bool transparent,
                                double zoomX, double zoomY )
{
}

bool KPrDocument::loadXML( QIODevice *, const KoXmlDocument & doc )
{
    //Perhaps not necessary if we use filter import/export for old file format
    return true;
}

bool KPrDocument::loadOasis( const KoXmlDocument & doc, KoOasisStyles& oasisStyles,
                             const KoXmlDocument & settings, KoStore* store )
{
    return true;
}

bool KPrDocument::saveOasis( KoStore* store, KoXmlWriter* manifestWriter )
{
    return true;
}

void KPrDocument::addShape( KoShape * shape )
{
    m_page->addShape( shape );
    foreach( KoView *view, views() ) {
        KPrCanvas *canvas = static_cast<KPrView*>( view )->kprcanvas();
        canvas->shapeManager()->add( shape );
    }
}

void KPrDocument::removeShape( KoShape *shape )
{
    m_page->removeShape( shape );
}

void KPrDocument::addCommand( KCommand* command, bool execute )
{
    m_commandHistory->addCommand( command, execute );
}

KoView * KPrDocument::createViewInstance( QWidget *parent )
{
    return new KPrView( this, parent );
}

#include "KPrDocument.moc"
