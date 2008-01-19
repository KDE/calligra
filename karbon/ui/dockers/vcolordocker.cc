/* This file is part of the KDE project
   Made by Tomislav Lukman (tomislav.lukman@ck.tel.hr)
   Copyright (C) 2002,2005 Tomislav Lukman <tomislav.lukman@ck.t-com.hr>
   Copyright (C) 2002-2003 Rob Buis <buis@kde.org>
   Copyright (C) 2002 Lennart Kudling <kudling@kde.org>
   Copyright (C) 2005 Malcolm Hunter <malcolm.hunter@gmx.co.uk>
   Copyright (C) 2005-2006 Tim Beaulen <tbscope@gmail.com>
   Copyright (C) 2005-2007 Thomas Zander <zander@kde.org>
   Copyright (C) 2005-2007 Jan Hambrecht <jaham@gmx.net>
   Copyright (C) 2007 Thorsten Zachmann <t.zachmann@zagge.de>
   Copyright (C) 2006 Inge Wallin <inge@lysator.liu.se>
   Copyright (C) 2006 Peter Simonsson <psn@linux.se>
   Copyright (C) 2006 Laurent Montel <montel@kde.org>
   Copyright (C) 2006 Boudewijn Rempt <boud@valdyas.org>
   Copyright (C) 2006 Sven Langkamp <sven.langkamp@gmail.com>

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

#include "vcolordocker.h"
#include "Karbon.h"

#include <KoUniColorChooser.h>
#include <KoShapeManager.h>
#include <KoSelection.h>
#include <KoShapeBackgroundCommand.h>
#include <KoShapeBorderCommand.h>
#include <KoToolManager.h>
#include <KoCanvasBase.h>
#include <KoCanvasController.h>
#include <KoCanvasResourceProvider.h>
#include <KoLineBorder.h>
#include <KoColor.h>
#include <KoColorSpaceRegistry.h>
#include <KoShape.h>

#include <klocale.h>

#include <QtCore/QEvent>
#include <QtGui/QMouseEvent>
#include <QtGui/QDockWidget>
#include <QtGui/QColor>
#include <QtGui/QToolTip>


VColorDockerFactory::VColorDockerFactory()
{
}

QString VColorDockerFactory::id() const
{
    return QString("Color Chooser");
}

QDockWidget* VColorDockerFactory::createDockWidget()
{
    VColorDocker* widget = new VColorDocker();
    widget->setObjectName(id());

    return widget;
}

KoDockFactory::DockPosition VColorDockerFactory::defaultDockPosition() const
{
    return DockMinimized;
}

VColorDocker::VColorDocker()
: m_isStrokeDocker( false ), m_canvas(0)
{
    setWindowTitle( i18n( "Color Chooser" ) );

    m_colorChooser = new KoUniColorChooser( this, true );
    m_colorChooser->changeLayout( KoUniColorChooser::SimpleLayout );

    setWidget( m_colorChooser );
    //setMaximumHeight( 174 );
    setMinimumWidth( 194 );

    connect( m_colorChooser, SIGNAL( sigColorChanged( const KoColor &) ), this, SLOT( updateColor( const KoColor &) ) );
    connect(this, SIGNAL(colorChanged(const KoColor &)), m_colorChooser, SLOT(setColor(const KoColor &)));

    setCanvas( KoToolManager::instance()->activeCanvasController()->canvas() );
}

VColorDocker::~VColorDocker()
{
}

void VColorDocker::updateColor( const KoColor &c )
{
    if( ! m_canvas )
        return;

    KoSelection *selection = m_canvas->shapeManager()->selection();
    if( ! selection || ! selection->count() )
        return;

    QColor color;
    quint8 opacity;
    c.toQColor(&color, &opacity);
    color.setAlpha(opacity);

    KoCanvasResourceProvider * provider = m_canvas->resourceProvider();
    int activeStyle = provider->resource( Karbon::ActiveStyle ).toInt();

    // check which color to set foreground == border, background == fill
    if( activeStyle == Karbon::Foreground )
    {
        // get the border of the first selected shape and check if it is a line border
        KoLineBorder * oldBorder = dynamic_cast<KoLineBorder*>( selection->firstSelectedShape()->border() );
        KoLineBorder * newBorder = 0;
        if( oldBorder )
        {
            // preserve the properties of the old border if it is a line border
            newBorder = new KoLineBorder( *oldBorder );
            newBorder->setColor( color );
        }
        else
            newBorder = new KoLineBorder( 1.0, color );

        KoShapeBorderCommand * cmd = new KoShapeBorderCommand( selection->selectedShapes(), newBorder );
        m_canvas->addCommand( cmd );
        m_canvas->resourceProvider()->setForegroundColor( c );
    }
    else
    {
        KoShapeBackgroundCommand *cmd = new KoShapeBackgroundCommand( selection->selectedShapes(), QBrush( color ) );
        m_canvas->addCommand( cmd );
        m_canvas->resourceProvider()->setBackgroundColor( c );
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
    if( ! m_canvas )
        return;

    KoSelection *selection = m_canvas->shapeManager()->selection();
    if( ! selection || ! selection->count() )
        return;

    KoShape * shape = selection->firstSelectedShape();
    if( ! shape )
        return;

    KoCanvasResourceProvider * provider = m_canvas->resourceProvider();
    int activeStyle = provider->resource( Karbon::ActiveStyle ).toInt();

    QColor qColor( Qt::black );
    if( activeStyle == Karbon::Foreground )
    {
        KoLineBorder * border = dynamic_cast<KoLineBorder*>( shape->border() );
        if( border )
            qColor = border->color();
    }
    else
    {
        if( shape->background().style() == Qt::SolidPattern )
            qColor = shape->background().color();
    }
    KoColor c( qColor, qColor.alpha(), KoColorSpaceRegistry::instance()->rgb8() );
    m_colorChooser->setColor( c );
}

void VColorDocker::setCanvas(KoCanvasBase *canvas)
{
    m_canvas = canvas;
    if( ! m_canvas )
        return;

    connect( m_canvas->resourceProvider(), SIGNAL(resourceChanged(int, const QVariant&)),
            this, SLOT( resourceChanged(int, const QVariant&)));
    connect( m_canvas->shapeManager(), SIGNAL(selectionChanged()),
            this, SLOT(update()));

    update();
}

void VColorDocker::resourceChanged(int key, const QVariant &)
{
    if( key == Karbon::ActiveStyle )
        update();
}

#include "vcolordocker.moc"

