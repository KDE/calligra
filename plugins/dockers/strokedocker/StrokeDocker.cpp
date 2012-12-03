/* This file is part of the KDE project
   Made by Tomislav Lukman (tomislav.lukman@ck.tel.hr)
   Copyright (C) 2002 Tomislav Lukman <tomislav.lukman@ck.t-com.hr>
   Copyright (C) 2002-2003 Rob Buis <buis@kde.org>
   Copyright (C) 2005-2006 Tim Beaulen <tbscope@gmail.com>
   Copyright (C) 2005-2007 Thomas Zander <zander@kde.org>
   Copyright (C) 2005-2006 Inge Wallin <inge@lysator.liu.se>
   Copyright (C) 2005-2008 Jan Hambrecht <jaham@gmx.net>
   Copyright (C) 2006 C. Boemann <cbo@boemann.dk>
   Copyright (C) 2006 Peter Simonsson <psn@linux.se>
   Copyright (C) 2006 Laurent Montel <montel@kde.org>
   Copyright (C) 2007 Thorsten Zachmann <t.zachmann@zagge.de>
   Copyright (C) 2011 Jean-Nicolas Artaud <jeannicolasartaud@gmail.com>

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

#include "StrokeDocker.h"

#include <KoStrokeConfigWidget.h>

#include <klocale.h>

#include <KoToolManager.h>
#include <KoCanvasBase.h>
#include <KoCanvasController.h>
#include <KoCanvasResourceManager.h>
#include <KoDocumentResourceManager.h>
#include <KoDockFactoryBase.h>
#include <KoShapeManager.h>
#include <KoShapeStrokeCommand.h>
#include <KoShapeStrokeModel.h>
#include <KoSelection.h>
#include <KoShapeStroke.h>
#include <KoPathShape.h>
#include <KoMarker.h>
#include <KoPathShapeMarkerCommand.h>
#include <KoShapeController.h>
#include <KoMarkerCollection.h>

class StrokeDocker::Private
{
public:
    Private()
        : canvas(0)
        , mainWidget(0)
    {}

    KoCanvasBase *canvas;
    KoStrokeConfigWidget *mainWidget;
};


StrokeDocker::StrokeDocker()
    : d( new Private() )
{
    setWindowTitle( i18n( "Stroke Properties" ) );

    d->mainWidget = new KoStrokeConfigWidget( this );
    setWidget( d->mainWidget );

    connect(this, SIGNAL(dockLocationChanged(Qt::DockWidgetArea)),
            this, SLOT(locationChanged(Qt::DockWidgetArea)));
}

StrokeDocker::~StrokeDocker()
{
    delete d;
}


void StrokeDocker::setCanvas( KoCanvasBase *canvas )
{
    if (d->canvas) {
        d->canvas->disconnectCanvasObserver(this); // "Every connection you make emits a signal, so duplicate connections emit two signals"
    }

    if (canvas) {
        d->mainWidget->setCanvas(canvas);
    }

    d->canvas = canvas;
}

void StrokeDocker::unsetCanvas()
{
    d->canvas = 0;
}

void StrokeDocker::locationChanged(Qt::DockWidgetArea area)
{
    d->mainWidget->locationChanged(area);
}

#include <StrokeDocker.moc>
