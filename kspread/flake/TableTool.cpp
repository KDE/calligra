/* This file is part of the KDE project
   Copyright 2006 Stefan Nikolaus <stefan.nikolaus@kdemail.net>

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

#include <QGridLayout>
#include <QLabel>
#include <QPainter>
#include <QSpinBox>

#include <kdebug.h>
#include <klocale.h>

#include <KoCanvasBase.h>
#include <KoPointerEvent.h>
#include <KoSelection.h>
#include <KoShapeManager.h>

#include "Global.h"

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
    Q_UNUSED( painter );
    Q_UNUSED( viewConverter );
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

void TableTool::changeColumns( int num )
{
    d->tableShape->setColumns( num );
    d->tableShape->repaint();
}

void TableTool::changeRows( int num )
{
    d->tableShape->setRows( num );
    d->tableShape->repaint();
}

QWidget* TableTool::createOptionWidget()
{
    QWidget* optionWidget = new QWidget();
    QGridLayout* layout = new QGridLayout( optionWidget );

    QLabel* label = 0;
    QSpinBox* spinBox = 0;

    spinBox = new QSpinBox( optionWidget );
    spinBox->setRange( 1, KS_colMax );
    spinBox->setValue( d->tableShape->columns() );
    layout->addWidget( spinBox, 0, 1 );
    connect( spinBox, SIGNAL( valueChanged(int) ), this, SLOT( changeColumns(int) ) );

    label = new QLabel( i18n( "Columns:" ), optionWidget );
    label->setBuddy( spinBox );
    label->setToolTip( i18n( "Number of columns" ) );
    layout->addWidget( label, 0, 0 );

    spinBox = new QSpinBox( optionWidget );
    spinBox->setRange( 1, KS_rowMax );
    spinBox->setValue( d->tableShape->rows() );
    layout->addWidget( spinBox, 1, 1 );
    connect( spinBox, SIGNAL( valueChanged(int) ), this, SLOT( changeRows(int) ) );

    label = new QLabel( i18n( "Rows:" ), optionWidget );
    label->setBuddy( spinBox );
    label->setToolTip( i18n( "Number of rows" ) );
    layout->addWidget( label, 1, 0 );

    layout->setColumnStretch( 1, 1 );
    layout->setRowStretch( 2, 1 );

    return optionWidget;
}

#include "TableTool.moc"
