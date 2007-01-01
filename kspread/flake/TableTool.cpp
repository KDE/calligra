/* This file is part of the KDE project
   Copyright 2006 Stefan Nikolaus <stefan.nikolaus@kdemail.net>

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License as published by the Free Software Foundation; either
   version 2 of the License.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this library; see the file COPYING.LIB.  If not, write to
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
   Boston, MA 02110-1301, USA.
*/

#include <QPainter>

#include <kdebug.h>

#include <KoCanvasBase.h>
#include <KoSelection.h>
#include <KoShapeManager.h>

#include "TableShape.h"

#include "TableTool.h"

using namespace KSpread;

class TableTool::Private
{
public:
    TableShape* tableShape;
};

TableTool::TableTool( KoCanvasBase* canvas )
    : KoTool( canvas )
    , d( new Private )
{
    d->tableShape = 0;
}

TableTool::~TableTool()
{
    delete d;
}

void TableTool::paint( QPainter& painter, KoViewConverter& viewConverter )
{
    Q_UNUSED( viewConverter );
    QBrush brush( Qt::red );
    painter.fillRect(10,10,10,10, brush);
}

void TableTool::mousePressEvent( KoPointerEvent* )
{
}

void TableTool::mouseMoveEvent( KoPointerEvent* )
{
}

void TableTool::mouseReleaseEvent( KoPointerEvent* )
{
}

void TableTool::activate( bool temporary )
{
    Q_UNUSED( temporary );
    kDebug() << k_funcinfo << endl;

    KoSelection* selection = m_canvas->shapeManager()->selection();
    foreach ( KoShape* shape, selection->selectedShapes() )
    {
        d->tableShape = dynamic_cast<TableShape*>( shape );
        if ( d->tableShape )
            break;
    }
    if ( !d->tableShape )
    {
        emit sigDone();
        return;
    }
    useCursor( Qt::ArrowCursor, true );
    d->tableShape->repaint();
}

void TableTool::deactivate()
{
    d->tableShape = 0;
}
