/* This file is part of the KDE project
 * Copyright (C) 2007 Jan Hambrecht <jaham@gmx.net>
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

#include "SimpleTextTool.h"

#include <KoCanvasBase.h>
#include <KoSelection.h>
#include <KoShapeManager.h>
#include <KoPointerEvent.h>
#include <KoPathShape.h>

#include <KLocale>
#include <KIcon>
#include <QAction>
#include <QGridLayout>
#include <QToolButton>
#include <QCheckBox>

SimpleTextTool::SimpleTextTool(KoCanvasBase *canvas)
    : KoTool(canvas), m_currentShape(0), m_path(0), m_tmpPath(0)
{
    m_attachPath  = new QAction(KIcon("attach-path"), i18n("Attach Path"), this);
    m_attachPath->setEnabled( false );
    connect( m_attachPath, SIGNAL(triggered()), this, SLOT(attachPath()) );

    m_detachPath  = new QAction(KIcon("detach-path"), i18n("Detach Path"), this);
    m_detachPath->setEnabled( false );
    connect( m_detachPath, SIGNAL(triggered()), this, SLOT(detachPath()) );
}

SimpleTextTool::~SimpleTextTool()
{
}

void SimpleTextTool::paint( QPainter &painter, const KoViewConverter &converter)
{
}

void SimpleTextTool::mousePressEvent( KoPointerEvent *event )
{
    event->ignore();
}

void SimpleTextTool::mouseMoveEvent( KoPointerEvent *event )
{
    m_tmpPath = 0;

    QRectF roi( event->point, QSizeF(1,1) );
    QList<KoShape*> shapes = m_canvas->shapeManager()->shapesAt( roi );
    foreach( KoShape * shape, shapes )
    {
        KoPathShape * path = dynamic_cast<KoPathShape*>( shape );
        if( path )
        {
            m_tmpPath = path;
            break;
        }
    }
    if( m_tmpPath )
        useCursor( QCursor( Qt::PointingHandCursor ) );
    else
        useCursor( QCursor( Qt::ForbiddenCursor ) );
}

void SimpleTextTool::mouseReleaseEvent( KoPointerEvent *event )
{
    m_path = m_tmpPath;
    updateActions();
}

void SimpleTextTool::activate( bool )
{
    KoSelection *selection = m_canvas->shapeManager()->selection();
    foreach( KoShape *shape, selection->selectedShapes() ) 
    {
        m_currentShape = dynamic_cast<SimpleTextShape*>( shape );
        if(m_currentShape)
            break;
    }
    if( m_currentShape == 0 ) 
    {
        // none found
        emit done();
        return;
    }

    updateActions();
}

void SimpleTextTool::deactivate()
{
    m_currentShape = 0;
    m_path = 0;
}

void SimpleTextTool::updateActions()
{
    m_attachPath->setEnabled( m_path != 0 );
    m_detachPath->setEnabled( m_currentShape->isOnPath() );
}

void SimpleTextTool::attachPath()
{
    if( m_path )
        m_currentShape->putOnPath( m_path );
}

void SimpleTextTool::detachPath()
{
    if( m_currentShape->isOnPath() )
        m_currentShape->removeFromPath();
}

QWidget *SimpleTextTool::createOptionWidget()
{
    QWidget * widget = new QWidget();
    QGridLayout * layout = new QGridLayout(widget);
    QToolButton * attachButton = new QToolButton(widget);
    attachButton->setDefaultAction( m_attachPath );
    layout->addWidget( attachButton, 0, 0 );
    QToolButton * detachButton = new QToolButton(widget);
    detachButton->setDefaultAction( m_detachPath );
    layout->addWidget( detachButton, 0, 1 );
    layout->setSpacing(0);
    layout->setMargin(6);
    layout->setRowStretch(3, 1);
    layout->setColumnStretch(2, 1);

    return widget;
}

#include "SimpleTextTool.moc"
