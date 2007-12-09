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

#include "KarbonStylePreviewDocker.h"
#include "KarbonStylePreview.h"
#include "KarbonStyleButtonBox.h"
#include "Karbon.h"

#include <KoToolManager.h>
#include <KoCanvasBase.h>
#include <KoCanvasController.h>
#include <KoCanvasResourceProvider.h>
#include <KoShapeManager.h>
#include <KoSelection.h>

#include <klocale.h>

#include <QVBoxLayout>

KarbonStylePreviewDocker::KarbonStylePreviewDocker( QWidget * parent )
    : QDockWidget( parent ), m_canvas(0)
{
    setWindowTitle( i18n( "Styles" ) );

    QWidget *mainWidget = new QWidget( this );
    QVBoxLayout * layout = new QVBoxLayout( mainWidget );

    m_preview = new KarbonStylePreview( mainWidget );
    layout->addWidget( m_preview );

    m_buttons = new KarbonStyleButtonBox( mainWidget );
    layout->addWidget( m_buttons );

    layout->addStretch( 1 );
    layout->setMargin( 1 );
    layout->setSpacing( 1 );

    connect( m_preview, SIGNAL(fillSelected()), this, SLOT(fillSelected()) );
    connect( m_preview, SIGNAL(strokeSelected()), this, SLOT(strokeSelected()) );

    setWidget( mainWidget );

    m_canvas = KoToolManager::instance()->activeCanvasController()->canvas();
    if( m_canvas )
        connect( m_canvas->shapeManager()->selection(), SIGNAL(selectionChanged()),
                 this, SLOT(selectionChanged()));
}

KarbonStylePreviewDocker::~KarbonStylePreviewDocker()
{
}

void KarbonStylePreviewDocker::setCanvas( KoCanvasBase * canvas )
{
    m_canvas = canvas;
    if( ! m_canvas )
    {
        updateStyle( 0, QBrush( Qt::NoBrush ) );
        return;
    }

    connect( m_canvas->shapeManager()->selection(), SIGNAL(selectionChanged()),
                this, SLOT(selectionChanged()));

    KoShape * shape = m_canvas->shapeManager()->selection()->firstSelectedShape();
    if( shape )
        updateStyle( shape->border(), shape->background() );
    else
        updateStyle( 0, QBrush( Qt::NoBrush ) );
}

void KarbonStylePreviewDocker::selectionChanged()
{
    if( ! m_canvas )
        return;

    KoShape * shape = m_canvas->shapeManager()->selection()->firstSelectedShape();
    if( shape )
        updateStyle( shape->border(), shape->background() );
    else
        updateStyle( 0, QBrush( Qt::NoBrush ) );
}

bool KarbonStylePreviewDocker::strokeIsSelected() const
{
    return m_preview->strokeIsSelected();
}

void KarbonStylePreviewDocker::updateStyle( const KoShapeBorderModel * stroke, const QBrush & fill )
{
    m_preview->update( stroke, fill );
}

void KarbonStylePreviewDocker::fillSelected()
{
    if( ! m_canvas )
        return;

    m_canvas->resourceProvider()->setResource( Karbon::ActiveStyle, Karbon::Background );
    m_buttons->setFill();
}

void KarbonStylePreviewDocker::strokeSelected()
{
    if( ! m_canvas )
        return;

    m_canvas->resourceProvider()->setResource( Karbon::ActiveStyle, Karbon::Foreground );
    m_buttons->setStroke();
}

KarbonStylePreviewDockerFactory::KarbonStylePreviewDockerFactory()
{
}

QString KarbonStylePreviewDockerFactory::id() const
{
    return QString("Style Preview");
}

KoDockFactory::DockPosition KarbonStylePreviewDockerFactory::defaultDockPosition() const
{
    return DockLeft;
}

QDockWidget* KarbonStylePreviewDockerFactory::createDockWidget()
{
    KarbonStylePreviewDocker * widget = new KarbonStylePreviewDocker();
    widget->setObjectName(id());

    return widget;
}

#include "KarbonStylePreviewDocker.moc"
