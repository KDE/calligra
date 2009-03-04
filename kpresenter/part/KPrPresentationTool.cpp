/* This file is part of the KDE project
 * Copyright (C) 2007 Thorsten Zachmann <zachmann@kde.org>
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

#include <QtGui/QBoxLayout>
#include <QtGui/QVBoxLayout>
#include <QKeyEvent>
#include <QtGui/QStackedLayout>
#include <QtGui/QPainter>
#include <QtGui/QPaintEvent>
#include <QtGui/QPixmap>
#include <QtGui/QPicture>
#include <QCursor>
#include <QString>

#include <KoShape.h>
#include <KoShapeManager.h>
#include <KoPointerEvent.h>
#include <KoEventAction.h>
#include <KoPACanvas.h>

#include "KPrViewModePresentation.h"
#include "KPrPresentationStrategy.h"


KPrPresentationTool::KPrPresentationTool( KPrViewModePresentation & viewMode )
: KoTool( viewMode.canvas() )
, m_viewMode( viewMode )
{    
    // init
    m_drawMode = false;
    m_highlightMode = false;

    // tool box
    m_frame = new QFrame( m_viewMode.canvas() );
    
    QVBoxLayout *frameLayout = new QVBoxLayout();
    m_presentationToolWidget = new KPrPresentationToolWidget(m_viewMode.canvas());
    frameLayout->addWidget( m_presentationToolWidget, 0, Qt::AlignLeft | Qt::AlignBottom );
    m_frame->setLayout( frameLayout );
    
    m_frame->show();
    m_frame->setVisible(false);

    m_presentationToolWidget->raise();
    m_presentationToolWidget->installEventFilter(this);
    
    // Connections of button clicked to slots
    connect( m_presentationToolWidget->presentationToolUi().penButton, SIGNAL( clicked() ), this, SLOT( drawOnPresentation() ) );
    connect( m_presentationToolWidget->presentationToolUi().highLightButton, SIGNAL( clicked() ), this, SLOT( highLightPresentation() ) );

}

KPrPresentationTool::~KPrPresentationTool()
{
}

bool KPrPresentationTool::wantsAutoScroll()
{
    return false;
}
void KPrPresentationTool::paint( QPainter &painter, const KoViewConverter &converter )
{
}

void KPrPresentationTool::mousePressEvent( KoPointerEvent *event )
{
    if ( event->button() & Qt::LeftButton ) {
        event->accept();
        finishEventActions();
        KoShape * shapeClicked = m_canvas->shapeManager()->shapeAt( event->point );
        if (shapeClicked) {
            m_eventActions = shapeClicked->eventActions();
            if ( m_eventActions.size() ) {
                foreach ( KoEventAction * eventAction, m_eventActions ) {
                    eventAction->execute( this );
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
}

void KPrPresentationTool::mouseMoveEvent( KoPointerEvent *event )
{
}

void KPrPresentationTool::mouseReleaseEvent( KoPointerEvent *event )
{
}

void KPrPresentationTool::keyPressEvent( QKeyEvent *event )
{
    finishEventActions();
    event->accept();
    KPrPresentationStrategy * m_strategy = new KPrPresentationStrategy(this);
  
    switch ( event->key() )
    {
        case Qt::Key_Escape:        
	    m_strategy->handleEscape(); 
	    /*if(m_drawMode)
            drawOnPresentation();
          else if(m_highlightMode)
            highLightPresentation();
          else
            m_viewMode.activateSavedViewMode();*/
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
	case Qt::Key_P:
	    switchDrawMode();
	    break;
	case Qt::Key_H:
	    highLightPresentation();
	    break;
        default:
            event->ignore();
            break;
    }
}

void KPrPresentationTool::keyReleaseEvent( QKeyEvent *event )
{
}

void KPrPresentationTool::wheelEvent( KoPointerEvent * event )
{
}

void KPrPresentationTool::activate( bool temporary )
{
}

void KPrPresentationTool::deactivate()
{
    finishEventActions();
}

void KPrPresentationTool::finishEventActions()
{
    foreach ( KoEventAction * eventAction, m_eventActions ) {
        eventAction->finish( this );
    }
}

// SLOTS
void KPrPresentationTool::highLightPresentation()
{
    // destroy the drawMode if it's active
    if ( m_drawMode )
    {
        // We put buttons on the presentation before deleting the draw widget
	m_presentationToolWidget->setParent( m_viewMode.canvas() );
	
        m_drawMode = false;
	delete m_drawWidget;
        QApplication::restoreOverrideCursor();
    }
    // create the high light
    QSize size = m_viewMode.canvas()->size();
    QPixmap newPage( size );
    if(newPage.isNull())
      return;

    if ( m_highlightMode )
    {
	// We put buttons on the presentation before deleting the highlight widget
	m_presentationToolWidget->setParent( m_viewMode.canvas() );
	
	m_highlightMode = false;
	delete m_blackBackgroundwidget;
    } 
    else 
    {
	m_highlightMode = true;
	m_blackBackgroundwidget = new KPrPresentationHighlightWidget( m_viewMode.canvas() );
	m_presentationToolWidget->setParent( m_blackBackgroundwidget );
	m_blackBackgroundwidget->show();

	m_blackBackgroundwidget->installEventFilter(this);
    }
}

bool KPrPresentationTool::getDrawMode()
{
    return m_drawMode;
}

void KPrPresentationTool::switchDrawMode()
{
    // destroy the highlightMode if it's active
    if ( m_highlightMode ) {
    
	// We put buttons on the presentation before deleting the highlight widget
	m_presentationToolWidget->setParent( m_viewMode.canvas() );
	
	m_highlightMode = false;
	delete m_blackBackgroundwidget;
    }
    // create the drawMode
    if ( !m_drawMode )
    {
        m_drawMode = true;
        QString str("kpresenter");
        KIconLoader kicon(str);
        str.clear();
        str.append("pen.png");
        QPixmap pix(kicon.loadIcon(str, kicon.Small));
        float factor = 1.2;
        pix = pix.scaledToHeight(pix.height()*factor);
        pix = pix.scaledToWidth(pix.width()*factor);
        QCursor cur = QCursor(pix);
        QApplication::setOverrideCursor(cur);
	m_drawWidget = new KPrPresentationDrawWidget(m_viewMode.canvas());
	
	m_presentationToolWidget->setParent( m_drawWidget );
	
	m_drawWidget->show();
	
	m_drawWidget->installEventFilter(this);
    }
    // destroy the drawMode if it's active
    else
    {
	// We put buttons on the presentation before deleting the draw widget
	m_presentationToolWidget->setParent( m_viewMode.canvas() );
	
        m_drawMode = false;
	delete m_drawWidget;
        QApplication::restoreOverrideCursor();
    }
    m_presentationToolWidget->presentationToolUi().penButton->setDown(m_drawMode);
}

bool KPrPresentationTool::getHighlightMode()
{
    return m_highlightMode;
}

void KPrPresentationTool::switchHighlightMode()
{
    if(!m_highlightMode){
	m_highlightMode = true;
    }
    else
    {
	m_highlightMode = false;
    }
}

void KPrPresentationTool::drawOnPresentation()
{
    m_presentationToolWidget->presentationToolUi().penButton->setText("test...");
    switchDrawMode();
}

// get the acces on m_frame
QFrame *KPrPresentationTool::m_frameToolPresentation()
{
    return m_frame;
}
// get the acces on m_blackBackgroundwidget
QWidget *KPrPresentationTool::m_blackBackgroundPresentation()
{
    if ( m_blackBackgroundwidget )
        return m_blackBackgroundwidget;
    else return 0;
}

void KPrPresentationTool::setBlackBackgroundVisibility(bool b)
{
    m_highlightMode = b;
}

bool KPrPresentationTool::getBlackBackgroundVisibility()
{
    return m_highlightMode;
}

bool KPrPresentationTool::eventFilter(QObject *obj, QEvent *event)
{
    if (event->type() == QEvent::MouseMove)
    {
	QMouseEvent *mouseEvent = static_cast<QMouseEvent*>(event);
	QWidget *source = static_cast<QWidget*>(obj);
	QPoint pos = source->mapFrom(m_viewMode.canvas(),mouseEvent->pos()); 
	
	QRect geometrie = QRect(0,m_frame->height()-100,100,100);
	if(geometrie.contains(pos))
	{
	    m_presentationToolWidget->setVisible(true);
	}
	else
	{
	    m_presentationToolWidget->setVisible(false);
	}
    }
    return false;
}

KPrViewModePresentation & KPrPresentationTool::getViewMode()
{
    return m_viewMode;
}

#include "KPrPresentationTool.moc"
