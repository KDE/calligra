/* This file is part of the KDE project

   Copyright 2007 Johannes Simon <johannes.simon@gmail.com>

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

// Local
#include "Surface.h"
#include "PlotArea.h"

// Qt
#include <QPointF>
#include <QBrush>
#include <QPen>
#include <QDebug>

// KOffice
#include <KoXmlReader.h>
#include <KoXmlWriter.h>
#include <KoXmlNS.h>
#include <KoOdfStylesReader.h>

using namespace KChart;

class Surface::Private
{
public:
    Private();
    ~Private();

    PlotArea *plotArea;

    QPointF position;
    int width;
    QBrush brush;
    QPen framePen;
};

Surface::Private::Private()
{
}

Surface::Private::~Private()
{
}


Surface::Surface( PlotArea *parent )
    : d( new Private )
{
    d->plotArea = parent;
}

Surface::~Surface()
{
}


QPointF Surface::position() const
{
    return d->position;
}

void Surface::setPosition( const QPointF &position )
{
    d->position = position;
}

int Surface::width() const
{
    return d->width;
}

void Surface::setWidth( int width )
{
    d->width = width;
}

QBrush Surface::brush() const
{
    return d->brush;
}

void Surface::setBrush( const QBrush &brush )
{
    d->brush = brush;
}

QPen Surface::framePen() const
{
    return d->framePen;
}

void Surface::setFramePen( const QPen &pen )
{
    d->framePen = pen;
}

bool Surface::loadOdf( const KoXmlElement &surfaceElement, const KoOdfStylesReader &stylesReader )
{
    qDebug() << "Surface::loadOdf()";
    return true;
}
