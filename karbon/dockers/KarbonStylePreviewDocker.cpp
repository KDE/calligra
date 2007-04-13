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
#include "vstrokefillpreview.h"
#include "vtypebuttonbox.h"
#include "Karbon.h"

#include <KoToolManager.h>
#include <KoCanvasBase.h>
#include <KoCanvasController.h>
#include <KoCanvasResourceProvider.h>

#include <klocale.h>

#include <QVBoxLayout>

KarbonStylePreviewDocker::KarbonStylePreviewDocker( QWidget * parent )
    : QDockWidget( parent )
{
    setWindowTitle( i18n( "Style Preview" ) );

    QWidget *mainWidget = new QWidget( this );
    QVBoxLayout * layout = new QVBoxLayout( mainWidget );

    m_preview = new VStrokeFillPreview( mainWidget );
    layout->addWidget( m_preview );

    m_buttons = new VTypeButtonBox( mainWidget );
    layout->addWidget( m_buttons );

    layout->addStretch( 1 );
    layout->setMargin( 1 );
    layout->setSpacing( 1 );

    connect( m_preview, SIGNAL(fillSelected()), this, SLOT(fillSelected()) );
    connect( m_preview, SIGNAL(strokeSelected()), this, SLOT(strokeSelected()) );

    setWidget( mainWidget );
}

KarbonStylePreviewDocker::~KarbonStylePreviewDocker()
{
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
    KoCanvasController* canvasController = KoToolManager::instance()->activeCanvasController();
    canvasController->canvas()->resourceProvider()->setResource( Karbon::ActiveStyle, Karbon::Background );
    m_buttons->setFill();
}

void KarbonStylePreviewDocker::strokeSelected()
{
    KoCanvasController* canvasController = KoToolManager::instance()->activeCanvasController();
    canvasController->canvas()->resourceProvider()->setResource( Karbon::ActiveStyle, Karbon::Foreground );
    m_buttons->setStroke();
}

KarbonStylePreviewDockerFactory::KarbonStylePreviewDockerFactory()
{
}

QString KarbonStylePreviewDockerFactory::id() const
{
    return QString("Style Preview");
}

Qt::DockWidgetArea KarbonStylePreviewDockerFactory::defaultDockWidgetArea() const
{
    return Qt::LeftDockWidgetArea;
}

QDockWidget* KarbonStylePreviewDockerFactory::createDockWidget()
{
    KarbonStylePreviewDocker * widget = new KarbonStylePreviewDocker();
    widget->setObjectName(id());

    return widget;
}

#include "KarbonStylePreviewDocker.moc"
