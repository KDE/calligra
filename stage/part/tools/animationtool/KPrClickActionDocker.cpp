/* This file is part of the KDE project
   Copyright (C) 2008 C. Boemann <cbo@boemann.dk>
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

#include <klocalizedstring.h>

#include <KoPACanvas.h>
#include <KPrDocument.h>
#include <KoCanvasBase.h>
#include <KoSelection.h>
#include <KoShapeController.h>
#include <KoShapeManager.h>
#include <KoEventAction.h>
#include <KoEventActionFactoryBase.h>
#include <KoEventActionRegistry.h>

#include <KPrEventActionWidget.h>
#include "KPrSoundData.h"
#include <KPresenter.h>
#include "KPrSoundCollection.h"
#include "KPrView.h"
#include "KPrPage.h"
#include "KPrEventActionData.h"
#include "StageDebug.h"

KPrClickActionDocker::KPrClickActionDocker( QWidget* parent, Qt::WindowFlags flags )
: QWidget( parent, flags )
, m_view( 0 )
, m_soundCollection( 0 )
, m_canvas(0)
{
    setObjectName( "KPrClickActionDocker" );
    // setup widget layout
    QVBoxLayout* layout = new QVBoxLayout;

    QList<KoEventActionFactoryBase *> factories = KoEventActionRegistry::instance()->presentationEventActions();
    foreach ( KoEventActionFactoryBase * factory, factories ) {
        QWidget * optionWidget = factory->createOptionWidget();
        layout->addWidget( optionWidget );
        m_eventActionWidgets.insert( factory->id(), optionWidget );
        connect( optionWidget, SIGNAL(addCommand(KUndo2Command*)),
                 this, SLOT(addCommand(KUndo2Command*)) );
    }
    // The following widget activates a special feature in the
    // ToolOptionsDocker that makes the components of the widget align
    // to the top if there is extra space.
    QWidget *specialSpacer = new QWidget(this);
    specialSpacer->setObjectName("SpecialSpacer");
    layout->addWidget(specialSpacer);

    setLayout( layout );
}

void KPrClickActionDocker::selectionChanged()
{
    if( ! m_canvas )
        return;
    KoSelection *selection = m_canvas->shapeManager()->selection();
    KoShape *shape = selection->firstSelectedShape();

    if ( shape ) {
        QSet<KoEventAction *> eventActions = shape->eventActions();
        QMap<QString, KoEventAction*> eventActionMap;
        foreach ( KoEventAction * eventAction, eventActions ) {
            eventActionMap.insert( eventAction->id(), eventAction );
        }

        QMap<QString, QWidget *>::const_iterator it(m_eventActionWidgets.constBegin());

        for (; it != m_eventActionWidgets.constEnd(); ++it)  {
            KPrEventActionWidget *actionWidget = dynamic_cast<KPrEventActionWidget*>(it.value());
            if (actionWidget) {
                // if it is not in the map a default value 0 pointer will be returned
                KPrEventActionData data(shape, eventActionMap.value(it.key()), m_soundCollection);
                actionWidget->setData(&data);
            }
        }
    }
    else {
        foreach (QWidget *widget, m_eventActionWidgets) {
            KPrEventActionWidget *actionWidget = dynamic_cast<KPrEventActionWidget*>(widget);
            if (actionWidget) {
                KPrEventActionData data(0, 0, m_soundCollection);
                actionWidget->setData( &data );
            }
        }
    }
}

void KPrClickActionDocker::setCanvas( KoCanvasBase *canvas )
{
    if (m_canvas) {
        m_canvas->disconnectCanvasObserver(this);
    }

    m_canvas = canvas;

    if (m_canvas) {
        connect(m_canvas->shapeManager(), SIGNAL(selectionChanged()),
                this, SLOT(selectionChanged()));
    }

    selectionChanged();
}

void KPrClickActionDocker::setView(KoPAViewBase  *view )
{
    m_view = view;
    if (m_view->kopaDocument()->resourceManager()->hasResource(KPresenter::SoundCollection)) {
        QVariant variant = m_view->kopaDocument()->resourceManager()->resource(KPresenter::SoundCollection);
        m_soundCollection = variant.value<KPrSoundCollection*>();
    }

    setCanvas(view->kopaCanvas());
}

void KPrClickActionDocker::addCommand( KUndo2Command * command )
{
    if ( m_view ) {
        m_view->kopaCanvas()->addCommand( command );
    }
}
