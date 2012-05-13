/* This file is part of the KDE project
   Copyright (C) 2012 Paul Mendez <paulestebanms@gmail.com>

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

#include "KPrAnimationsDocker.h"
#include "QListWidget"
#include "QVBoxLayout"
#include "KPrView.h"

KPrAnimationsDocker::KPrAnimationsDocker(QWidget* parent, Qt::WindowFlags flags)
: QDockWidget(parent, flags)
, m_view(0)
{
    setWindowTitle( i18n( "Shape Animations" ) );

    QWidget* base = new QWidget( this );
    m_layoutsView = new QListWidget( base );
    QVBoxLayout* layout = new QVBoxLayout;
    layout->addWidget( m_layoutsView );
    base->setLayout( layout );
    setWidget( base );
}

void KPrAnimationsDocker::setView(KPrView* view)
{
    Q_ASSERT( view );
    if (m_view) {
        // don't disconnect the m_view->proxyObject as the object is already deleted
        disconnect(m_layoutsView, 0, this, 0);
    }
    m_view = view;
    //connect( m_view->proxyObject, SIGNAL( activePageChanged() ),
    //         this, SLOT( slotActivePageChanged() ) );

    // remove the layouts from the last view
    //m_layoutsView->clear();


    /*connect( m_layoutsView, SIGNAL( itemPressed( QListWidgetItem * ) ),
             this, SLOT( slotItemPressed( QListWidgetItem * ) ) );
    connect( m_layoutsView, SIGNAL( currentItemChanged( QListWidgetItem *, QListWidgetItem * ) ),
             this, SLOT( slotCurrentItemChanged( QListWidgetItem *, QListWidgetItem * ) ) );*/
}
