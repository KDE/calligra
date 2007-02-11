/* This file is part of the KDE project
   Made by Tomislav Lukman (tomislav.lukman@ck.tel.hr)
   Copyright (C) 2002 - 2005, The Karbon Developers
   Copyright (C) 2006 Jan Hambecht <jaham@gmx.net>

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

#include <QDockWidget>
#include <QColor>
#include <QToolTip>
#include <qevent.h>
#include <QMouseEvent>

#include <klocale.h>
#include <KoUniColorChooser.h>
#include <KoShapeManager.h>
#include <KoSelection.h>
#include <KoShapeBackgroundCommand.h>
#include <KoShapeBorderCommand.h>
#include <KoToolManager.h>
#include <KoCanvasController.h>
#include <KoCanvasResourceProvider.h>
#include <KoLineBorder.h>
#include <KoColor.h>
#include <KoShape.h>

#include "Karbon.h"
#include "vcolordocker.h"

#include <kdebug.h>

VColorDockerFactory::VColorDockerFactory()
{
}

QString VColorDockerFactory::dockId() const
{
    return QString("Color Chooser");
}

Qt::DockWidgetArea VColorDockerFactory::defaultDockWidgetArea() const
{
    return Qt::RightDockWidgetArea;
}

QDockWidget* VColorDockerFactory::createDockWidget()
{
    VColorDocker* widget = new VColorDocker();
    widget->setObjectName(dockId());

    return widget;
}

VColorDocker::VColorDocker()
: m_isStrokeDocker( false )
{
    setWindowTitle( i18n( "Color Chooser" ) );

    m_colorChooser = new KoUniColorChooser( this, true );
    m_colorChooser->changeLayout( KoUniColorChooser::SimpleLayout );

    setWidget( m_colorChooser );
    //setMaximumHeight( 174 );
    setMinimumWidth( 194 );

    connect( m_colorChooser, SIGNAL( sigColorChanged( const KoColor &) ), this, SLOT( updateColor( const KoColor &) ) );
    connect(this, SIGNAL(colorChanged(const KoColor &)), m_colorChooser, SLOT(setColor(const KoColor &)));
}

VColorDocker::~VColorDocker()
{
}

void VColorDocker::updateColor( const KoColor &c )
{
    KoCanvasController* canvasController = KoToolManager::instance()->activeCanvasController();
    KoSelection *selection = canvasController->canvas()->shapeManager()->selection();
    if( ! selection || ! selection->count() )
        return;

    QColor color;
    quint8 opacity;
    c.toQColor(&color, &opacity);
    color.setAlpha(opacity);

    KoCanvasResourceProvider * provider = canvasController->canvas()->resourceProvider();
    int activeStyle = provider->resource( Karbon::ActiveStyle ).toInt();

    if( activeStyle == Karbon::Foreground )
    {
        KoLineBorder * oldBorder = dynamic_cast<KoLineBorder*>( selection->firstSelectedShape()->border() );
        KoLineBorder * border = new KoLineBorder( 1.0, color );
        if( oldBorder )
        {
            // use the properties of the old border if it is a line border
            *border = *oldBorder;
            border->setColor( color );
        }
        KoShapeBorderCommand * cmd = new KoShapeBorderCommand( selection->selectedShapes(), border );
        canvasController->canvas()->addCommand( cmd );
        canvasController->canvas()->resourceProvider()->setForegroundColor( c );
    }
    else
    {
        KoShapeBackgroundCommand *cmd = new KoShapeBackgroundCommand( selection->selectedShapes(), QBrush( color ) );
        canvasController->canvas()->addCommand( cmd );
        canvasController->canvas()->resourceProvider()->setBackgroundColor( c );
    }
}

void VColorDocker::setFillDocker()
{
    m_isStrokeDocker = false;
}

void VColorDocker::setStrokeDocker()
{
    m_isStrokeDocker = true;
}

void VColorDocker::update()
{
    KoCanvasController* canvasController = KoToolManager::instance()->activeCanvasController();
    KoSelection *selection = canvasController->canvas()->shapeManager()->selection();
    if( ! selection || ! selection->count() )
        return;

    KoShape * shape = selection->firstSelectedShape();
    KoCanvasResourceProvider * provider = canvasController->canvas()->resourceProvider();
    int activeStyle = provider->resource( Karbon::ActiveStyle ).toInt();

    if( activeStyle == Karbon::Foreground )
    {
        KoLineBorder * border = dynamic_cast<KoLineBorder*>( shape->border() );
        if( border )
        {
            KoColor c;
            c.fromQColor( border->color() );
            m_colorChooser->setColor( c );
        }
    }
    else
    {
        KoColor c;
        c.fromQColor( shape->background().color() );
        m_colorChooser->setColor( c );
    }
}

#include "vcolordocker.moc"

