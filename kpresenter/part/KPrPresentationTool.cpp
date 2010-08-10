/* This file is part of the KDE project
 * Copyright (C) 2007-2009 Thorsten Zachmann <zachmann@kde.org>
 * Copyright (C) 2008 Jim Courtiau <jeremy.courtiau@gmail.com>
 * Copyright (C) 2009 Alexia Allanic <alexia_allanic@yahoo.fr>
 * Copyright (C) 2009 Jean-Nicolas Artaud <jeannicolasartaud@gmail.com>
 * Copyright (C) 2009 Jérémy Lugagne <jejewindsurf@hotmail.com>
 * Copyright (C) 2009 Johann Hingue <yoan1703@hotmail.fr>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
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

#include "KPrPresentationTool.h"

#include <QtGui/QWidget>
#include <QtGui/QVBoxLayout>
#include <QtGui/QPainter>
#include <QKeyEvent>

#include <KoShape.h>
#include <KoShapeManager.h>
#include <KoPointerEvent.h>
#include <KoEventAction.h>
#include <KoPACanvas.h>

#include "KPrViewModePresentation.h"
#include "KPrPresentationStrategy.h"
#include "KPrPresentationHighlightStrategy.h"
#include "KPrPresentationDrawStrategy.h"
#include "KPrPresentationBlackStrategy.h"
#include "ui/KPrPresentationToolWidget.h"
#include "KPrPresentationToolAdaptor.h"
#include "KPrViewModePresentation.h"


KPrPresentationTool::KPrPresentationTool( KPrViewModePresentation &viewMode )
: KoToolBase( viewMode.canvas() )
, m_viewMode( viewMode )
, m_strategy( new KPrPresentationStrategy( this ) )
, m_bus ( new KPrPresentationToolAdaptor( this ) )
{
    QDBusConnection::sessionBus().registerObject("/kpresenter/PresentationTools", this);

    // tool box
    m_frame = new QFrame( m_viewMode.canvas()->canvasWidget() );

    QVBoxLayout *frameLayout = new QVBoxLayout();

    m_presentationToolWidget = new KPrPresentationToolWidget(m_viewMode.canvas()->canvasWidget());
    frameLayout->addWidget( m_presentationToolWidget, 0, Qt::AlignLeft | Qt::AlignBottom );
    m_frame->setLayout( frameLayout );
    m_frame->show();

    m_presentationToolWidget->raise();
    m_presentationToolWidget->setVisible( false );
    m_presentationToolWidget->installEventFilter(this);

    // Connections of button clicked to slots
    connect( m_presentationToolWidget->presentationToolUi().penButton, SIGNAL( clicked() ), this, SLOT( drawOnPresentation() ) );
    connect( m_presentationToolWidget->presentationToolUi().highLightButton, SIGNAL( clicked() ), this, SLOT( highlightPresentation() ) );
    connect( m_presentationToolWidget->presentationToolUi().blackButton, SIGNAL( clicked() ), this, SLOT( blackPresentation() ) );

}

KPrPresentationTool::~KPrPresentationTool()
{
    delete m_strategy;
}

bool KPrPresentationTool::wantsAutoScroll() const
{
    return false;
}

void KPrPresentationTool::paint( QPainter &painter, const KoViewConverter &converter )
{
    Q_UNUSED(painter);
    Q_UNUSED(converter);
}

void KPrPresentationTool::mousePressEvent( KoPointerEvent *event )
{
    if ( event->button() & Qt::LeftButton ) {
        event->accept();
        finishEventActions();
        KoShape * shapeClicked = canvas()->shapeManager()->shapeAt( event->point );
        if (shapeClicked) {
            m_eventActions = shapeClicked->eventActions();
            if (!m_eventActions.isEmpty()) {
                foreach ( KoEventAction * eventAction, m_eventActions ) {
                    eventAction->start();
                }
                // don't do next step if a action was executed
                return;
            }
        }
        m_viewMode.navigate( KPrAnimationDirector::NextStep );
    }
    else if ( event->button() & Qt::RightButton ) {
        event->accept();
        finishEventActions();
        m_viewMode.navigate( KPrAnimationDirector::PreviousStep );
    }
}

void KPrPresentationTool::mouseDoubleClickEvent( KoPointerEvent *event )
{
    Q_UNUSED(event);
}

void KPrPresentationTool::mouseMoveEvent( KoPointerEvent *event )
{
    Q_UNUSED(event);
}

void KPrPresentationTool::mouseReleaseEvent( KoPointerEvent *event )
{
    Q_UNUSED(event);
}

void KPrPresentationTool::keyPressEvent( QKeyEvent *event )
{
    finishEventActions();
    // first try to handle the event in the strategy if it is done there no need to use the default action
    if ( ! m_strategy->keyPressEvent( event ) ) {
        switch ( event->key() )
        {
            case Qt::Key_Escape:
                m_viewMode.activateSavedViewMode();
                break;
            case Qt::Key_Home:
                m_viewMode.navigate( KPrAnimationDirector::FirstPage );
                break;
            case Qt::Key_Up:
            case Qt::Key_PageUp:
                m_viewMode.navigate( KPrAnimationDirector::PreviousPage );
                break;
            case Qt::Key_Backspace:
            case Qt::Key_Left:
                m_viewMode.navigate( KPrAnimationDirector::PreviousStep );
                break;
            case Qt::Key_Right:
            case Qt::Key_Space:
                m_viewMode.navigate( KPrAnimationDirector::NextStep );
                break;
            case Qt::Key_Down:
            case Qt::Key_PageDown:
                m_viewMode.navigate( KPrAnimationDirector::NextPage );
                break;
            case Qt::Key_End:
                m_viewMode.navigate( KPrAnimationDirector::LastPage );
                break;
            default:
                event->ignore();
                break;
        }
    }
}

void KPrPresentationTool::keyReleaseEvent( QKeyEvent *event )
{
    Q_UNUSED( event );
}

void KPrPresentationTool::wheelEvent( KoPointerEvent * event )
{
    Q_UNUSED( event );
}

void KPrPresentationTool::activate(ToolActivation toolActivation, const QSet<KoShape*> &shapes)
{
    Q_UNUSED(toolActivation);
    Q_UNUSED(shapes);
    m_frame->setGeometry( canvas()->canvasWidget()->geometry() );
    m_presentationToolWidget->setVisible( false );
    // redirect event to tool widget
    m_frame->installEventFilter( this );
    // activate tracking for show/hide tool buttons
    m_frame->setMouseTracking( true );
}

void KPrPresentationTool::deactivate()
{
    switchStrategy( new KPrPresentationStrategy( this ) );
    finishEventActions();
}

void KPrPresentationTool::finishEventActions()
{
    foreach ( KoEventAction * eventAction, m_eventActions ) {
        eventAction->finish();
    }
}

void KPrPresentationTool::switchStrategy( KPrPresentationStrategyBase * strategy )
{
    Q_ASSERT( strategy );
    Q_ASSERT( m_strategy != strategy );
    delete m_strategy;
    m_strategy = strategy;
}

// SLOTS
void KPrPresentationTool::highlightPresentation()
{
    KPrPresentationStrategyBase * strategy;
    if ( dynamic_cast<KPrPresentationHighlightStrategy *>( m_strategy ) ) {
        strategy = new KPrPresentationStrategy( this );
    }
    else {
        strategy = new KPrPresentationHighlightStrategy( this );
    }
    switchStrategy( strategy );
}

void KPrPresentationTool::drawOnPresentation()
{
    KPrPresentationStrategyBase * strategy;
    if ( dynamic_cast<KPrPresentationDrawStrategy*>( m_strategy ) ) {
        strategy = new KPrPresentationStrategy( this );
    }
    else {
        strategy = new KPrPresentationDrawStrategy( this );
    }
    switchStrategy( strategy );
}

void KPrPresentationTool::blackPresentation()
{
    KPrPresentationStrategyBase * strategy;
    if ( dynamic_cast<KPrPresentationBlackStrategy*>( m_strategy ) ) {
        strategy = new KPrPresentationStrategy( this );
    }
    else {
        strategy = new KPrPresentationBlackStrategy( this );
    }
    switchStrategy( strategy );
}

bool KPrPresentationTool::eventFilter( QObject *obj, QEvent * event )
{
    if ( event->type() == QEvent::MouseMove ) {
        QMouseEvent *mouseEvent = static_cast<QMouseEvent*>( event );
        QWidget *source = static_cast<QWidget*>( obj );
        QPoint pos = source->mapFrom( m_viewMode.canvas()->canvasWidget(), mouseEvent->pos() );

        QSize buttonSize = m_presentationToolWidget->size() + QSize( 20, 20 );
        QRect geometry = QRect( 0, m_frame->height() - buttonSize.height(), buttonSize.width(), buttonSize.height() );
        if ( geometry.contains( pos ) ) {
            m_presentationToolWidget->setVisible( true );
        }
        else {
            m_presentationToolWidget->setVisible( false );
        }
    }
    return false;
}

KPrPresentationStrategyBase *KPrPresentationTool::strategy()
{
    return m_strategy;
}

KPrViewModePresentation &KPrPresentationTool::viewModePresentation()
{
    return m_viewMode;
}


void KPrPresentationTool::normalPresentation()
{
   switchStrategy( new KPrPresentationStrategy( this ) );
}

#include "KPrPresentationTool.moc"
