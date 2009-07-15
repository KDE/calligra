/* This file is part of the KDE project
  Copyright (C) 2009 Dag Andersen kplato@kde.org>

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

#include <QMenu>
#include <QList>
#include <QObject>
#include <QVBoxLayout>


#include <kaction.h>
#include <kicon.h>
#include <kglobal.h>
#include <klocale.h>
#include <kactioncollection.h>
#include <kxmlguifactory.h>
#include <khtmlview.h>

#include <kabc/addressee.h>
#include <kabc/vcardconverter.h>

#include <kdebug.h>

namespace KPlato
{


//-----------------------------------
HtmlView::HtmlView( KoDocument *part, QWidget *parent )
    : ViewBase( part, parent )
{
    QVBoxLayout * l = new QVBoxLayout( this );
    l->setMargin( 0 );
    l->addWidget( m_htmlPart.view() );
    m_htmlPart.show();

    setupGui();
}

bool HtmlView::openHtml( const KUrl &url )
{
    return m_htmlPart.openUrl( url );
}

void HtmlView::updateReadWrite( bool readwrite )
{
}

void HtmlView::setGuiActive( bool activate )
{
    kDebug()<<activate;
}

void HtmlView::slotContextMenuRequested( QModelIndex index, const QPoint& pos )
{
    //kDebug()<<index.row()<<","<<index.column()<<":"<<pos;
}

void HtmlView::slotEnableActions( bool on )
{
    updateActionsEnabled( on );
}

void HtmlView::updateActionsEnabled(  bool on )
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

#include "kpthtmlview.moc"
