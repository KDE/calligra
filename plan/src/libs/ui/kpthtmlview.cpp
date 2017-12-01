/* This file is part of the KDE project
  Copyright (C) 2009, 2011, 2012 Dag Andersen <danders@get2net.dk>

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

#include "kpthtmlview.h"

#include <KoDocument.h>

#include <QVBoxLayout>
#include <QUrl>

#include <khtmlview.h>

#include "kptdebug.h"

namespace KPlato
{

//-----------------------------------
HtmlView::HtmlView(KoPart *part, KoDocument *doc, QWidget *parent)
    : ViewBase(part, doc, parent)
{
    m_htmlPart = new KHTMLPart( this );
    m_htmlPart->view()->setFrameStyle( QFrame::StyledPanel );
    m_htmlPart->view()->setFrameShadow( QFrame::Sunken );

    QVBoxLayout * l = new QVBoxLayout( this );
    l->setMargin( 0 );
    l->addWidget( m_htmlPart->view() );
    m_htmlPart->show();

    setupGui();

    KParts::BrowserExtension *ext = m_htmlPart->browserExtension();
    if ( ext ) {
        connect( ext, SIGNAL(openUrlRequest(QUrl,KParts::OpenUrlArguments,KParts::BrowserArguments)), SLOT(slotOpenUrlRequest(QUrl,KParts::OpenUrlArguments,KParts::BrowserArguments)) );
    }
}

void HtmlView::slotOpenUrlRequest(const QUrl &url, const KParts::OpenUrlArguments &/*arguments*/, const KParts::BrowserArguments &/*browserArguments*/)
{
    emit openUrlRequest( this, url );
}

bool HtmlView::openHtml( const QUrl &url )
{
    return m_htmlPart->openUrl( url );
}

void HtmlView::updateReadWrite( bool /*readwrite */)
{
}

void HtmlView::setGuiActive( bool activate )
{
    debugPlan<<activate;
}

void HtmlView::slotContextMenuRequested( const QModelIndex &/*index*/, const QPoint& /*pos */)
{
    //debugPlan<<index.row()<<","<<index.column()<<":"<<pos;
}

void HtmlView::slotEnableActions( bool on )
{
    updateActionsEnabled( on );
}

void HtmlView::updateActionsEnabled(  bool /*on */)
{
}

void HtmlView::setupGui()
{
    // Add the context menu actions for the view options
}

KoPrintJob *HtmlView::createPrintJob()
{
    return 0;//m_view->createPrintJob( this );
}


} // namespace KPlato
