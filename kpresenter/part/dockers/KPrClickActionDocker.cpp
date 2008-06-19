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
#include "KPrShapeApplicationData.h"
#include "KPrEventActionData.h"

#include <kdebug.h>

KPrClickActionDocker::KPrClickActionDocker( QWidget* parent, Qt::WindowFlags flags )
: QDockWidget( parent, flags )
, m_view( 0 )
{
    setWindowTitle( i18n( "Click Actions" ) );

    QWidget* base = new QWidget( this );

    // setup widget layout
    QVBoxLayout* layout = new QVBoxLayout;
    m_cbNavigate = new QCheckBox(i18n("Navigate to:"));
    m_cbPlaySound = new QComboBox(); //i18n("Play:")
    //layout->addWidget(m_cbNavigate);
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

    m_cbNavigate->setEnabled(false);
    m_cbPlaySound->setEnabled(false);
    m_cbPlaySound->addItem(i18n("No sound"));
    m_cbPlaySound->addItem(i18n("Import..."));

    connect( m_cbPlaySound, SIGNAL( currentIndexChanged(int) ),
             this, SLOT( soundComboChanged() ) );

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

    if( ! shape) {
        m_cbNavigate->setEnabled(false);
        m_cbPlaySound->setEnabled(false);
        return;
    }

    //m_cbNavigate->setEnabled(true);
    m_cbPlaySound->setEnabled(true);

    m_cbPlaySound->blockSignals(true);
    m_cbNavigate->blockSignals(true);

    m_cbNavigate->setChecked(false);
    m_cbPlaySound->setCurrentIndex(0);

    if(KPrShapeApplicationData *appData
                        = dynamic_cast<KPrShapeApplicationData *>( shape->applicationData())) {
        if(appData->m_soundData)
        {
            int index = m_cbPlaySound->findText(appData->m_soundData->title());
            m_cbPlaySound->setCurrentIndex(index);
        }
    }
    m_cbPlaySound->blockSignals(false);
    m_cbNavigate->blockSignals(false);
}

void KPrClickActionDocker::soundComboChanged()
{
    KoSelection *selection = m_canvas->shapeManager()->selection();
    KoShape *shape = selection->firstSelectedShape();

    if( ! shape)
        return;

    KPrShapeApplicationData *appData
            = dynamic_cast<KPrShapeApplicationData *>( shape->applicationData());

    if(appData==0) {
        appData = new KPrShapeApplicationData;
        shape->setApplicationData(appData);
    }

    delete appData->m_soundData;
    appData->m_soundData = 0;

    if(m_cbPlaySound->currentIndex() > 1) { // a previous sound was chosen
        // copy it rather then just point to it - so the refcount is updated
        appData->m_soundData = new KPrSoundData(*m_soundCollection->findSound(
                                            m_cbPlaySound->currentText()));
    }

    m_cbPlaySound->blockSignals(true);

    if(m_cbPlaySound->currentIndex() == 1) {// "Import..." was chosen
        KUrl url = KFileDialog::getOpenUrl();
        if(!url.isEmpty()) {
            appData->m_soundData = new KPrSoundData(m_soundCollection, url.toLocalFile());
            QFile *file = new QFile(url.toLocalFile());
            file->open(QIODevice::ReadOnly);
            appData->m_soundData->loadFromFile(file); //also closes the file and deletes the class
        }
    }

    m_cbPlaySound->clear();
    m_cbPlaySound->addItem(i18n("No sound"));
    m_cbPlaySound->addItem(i18n("Import..."));
    m_cbPlaySound->addItems(m_soundCollection->titles());
    if(appData->m_soundData)
        m_cbPlaySound->setCurrentIndex(m_cbPlaySound->findText(appData->m_soundData->title()));
    else
        m_cbPlaySound->setCurrentIndex(0);

    m_cbPlaySound->blockSignals(false);
}

void KPrClickActionDocker::setCanvas( KoCanvasBase *canvas )
{
    m_canvas = canvas;

    connect( m_canvas->shapeManager(), SIGNAL( selectionChanged() ),
            this, SLOT( selectionChanged() ) );
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
