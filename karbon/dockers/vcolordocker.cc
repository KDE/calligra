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
#include <KoCanvasBase.h>
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

QString VColorDockerFactory::id() const
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
    widget->setObjectName(id());

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

