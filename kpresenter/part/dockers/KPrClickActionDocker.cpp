/* This file is part of the KDE project
   Copyright (C) 2008 Casper Boemann <cbr@boemann.dk>
   Copyright (C) 2007 Thorsten Zachmann <zachmann@kde.org>

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
   Boston, MA 02110-1301, USA.
*/

#include "KPrClickActionDocker.h"

#include <QVBoxLayout>
#include <QCheckBox>
#include <QComboBox>
#include <QLabel>
#include <QEvent>
#include <klocale.h>

#include <KFileDialog>

#include <KoPACanvas.h>
#include <KPrDocument.h>
#include <KoCanvasBase.h>
#include <KoSelection.h>
#include <KoShapeManager.h>
#include <KoEventAction.h>
#include <KoEventActionFactory.h>
#include <KoEventActionRegistry.h>
#include <KoEventActionWidget.h>
#include "KPrSoundData.h"
#include "KPrSoundCollection.h"
#include "KPrView.h"
#include "KPrPage.h"
#include "KPrEventActionData.h"

#include <kdebug.h>

KPrClickActionDocker::KPrClickActionDocker( QWidget* parent, Qt::WindowFlags flags )
: QDockWidget( parent, flags )
, m_view( 0 )
, m_soundCollection( 0 )
{
    setWindowTitle( i18n( "Click Actions" ) );

    QWidget* base = new QWidget( this );

    // setup widget layout
    QVBoxLayout* layout = new QVBoxLayout;
    m_cbPlaySound = new QComboBox();
    //layout->addWidget(m_cbPlaySound);

    QList<KoEventActionFactory *> factories = KoEventActionRegistry::instance()->presentationEventActions();
    foreach ( KoEventActionFactory * factory, factories ) {
        KoEventActionWidget * optionWidget = factory->createOptionWidget();
        layout->addWidget( optionWidget );
        m_eventActionWidgets.insert( factory->id(), optionWidget );
        connect( optionWidget, SIGNAL( addCommand( QUndoCommand * ) ),
                 this, SLOT( addCommand( QUndoCommand * ) ) );
    }

    base->setLayout( layout );
    setWidget( base );
}

void KPrClickActionDocker::selectionChanged()
{
    if( ! m_canvas )
        return;
    KoSelection *selection = m_canvas->shapeManager()->selection();
    KoShape *shape = selection->firstSelectedShape();

    if ( shape ) {
        QList<KoEventAction *> eventActions = shape->eventActions();
        QMap<QString, KoEventAction*> eventActionMap;
        foreach ( KoEventAction * eventAction, eventActions ) {
            eventActionMap.insert( eventAction->id(), eventAction );
        }

        QMap<QString, KoEventActionWidget *>::const_iterator it( m_eventActionWidgets.begin() );

        for ( ; it != m_eventActionWidgets.end(); ++it )  {
            // if it is not in the map a default value 0 pointer will be returned
            KPrEventActionData data( shape, eventActionMap.value( it.key() ), m_soundCollection );
            it.value()->setData( &data );
        }
    }
    else {
        foreach ( KoEventActionWidget * widget, m_eventActionWidgets ) {
            KPrEventActionData data( 0, 0, m_soundCollection );
            widget->setData( &data );
        }
    }
}

void KPrClickActionDocker::setCanvas( KoCanvasBase *canvas )
{
    m_canvas = canvas;

    connect( m_canvas->shapeManager(), SIGNAL( selectionChanged() ),
             this, SLOT( selectionChanged() ) );

    selectionChanged();
}

void KPrClickActionDocker::setView( KPrView *view )
{
    m_view = view;
    m_soundCollection = dynamic_cast<KPrSoundCollection *>( m_view->kopaDocument()->dataCenterMap()["SoundCollection"] );
}

void KPrClickActionDocker::addCommand( QUndoCommand * command )
{
    if ( m_view ) {
        m_view->kopaCanvas()->addCommand( command );
    }
}

#include "KPrClickActionDocker.moc"
