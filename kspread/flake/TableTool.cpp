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

// Local
#include "TableTool.h"

#include <QApplication>
#include <QGridLayout>
#include <QLabel>
#include <QLineEdit>
#include <QPainter>
#include <QSpinBox>

#include <kdebug.h>
#include <klocale.h>

#include <KoCanvasBase.h>
#include <KoPointerEvent.h>
#include <KoSelection.h>
#include <KoShapeManager.h>

#include "Cell.h"
#include "Doc.h"
#include "Global.h"
#include "Selection.h"
#include "Sheet.h"

#include "commands/DataManipulators.h"

#include "TableShape.h"

using namespace KSpread;

class TableTool::Private
{
public:
    // If the user is dragging around with the mouse then this tells us what he is doing.
    // The user may want to mark cells or he started in the lower right corner
    // of the marker which is something special. The values for the 2 above
    // methods are called 'Mark' and 'Merge' or 'AutoFill' depending
    // on the mouse button used. By default this variable holds
    // the value 'None'.
    enum { None, Mark, Merge, AutoFill, Resize } mouseAction : 3;

    QString userInput;
    Selection* selection;
    TableShape* tableShape;

public:
    QRectF cellCoordinatesToDocument(const QRect& cellRange) const;
};

// TODO Stefan: Copied from Canvas. Share it somewhere.
QRectF TableTool::Private::cellCoordinatesToDocument( const QRect& cellRange ) const
{
    register Sheet * const sheet = tableShape->sheet();
    if (!sheet)
        return QRectF();

    QRectF rect;
    rect.setLeft  ( sheet->columnPosition( cellRange.left() ) );
    rect.setRight ( sheet->columnPosition( cellRange.right() + 1 ) );
    rect.setTop   ( sheet->rowPosition( cellRange.top() ) );
    rect.setBottom( sheet->rowPosition( cellRange.bottom() + 1 ) );
    return rect;
}


TableTool::TableTool( KoCanvasBase* canvas )
    : KoTool( canvas )
    , d( new Private )
{
    d->selection = new Selection(this);
    connect(d->selection, SIGNAL(changed(const Region&)), this, SLOT(changeSelection(const Region&)));
    d->tableShape = 0;
}

TableTool::~TableTool()
{
    delete d->selection;
    delete d;
}

void TableTool::paint( QPainter& painter, const KoViewConverter& viewConverter )
{
    KoShape::applyConversion(painter, viewConverter);

    // get the transparent selection color
    QColor selectionColor(QApplication::palette().highlight().color());
    selectionColor.setAlpha(127);

    // draw the transparent selection background
    const QRectF rect = d->cellCoordinatesToDocument(d->selection->lastRange());
    painter.fillRect(rect.translated(d->tableShape->position()), selectionColor);
}

// TODO Stefan: Taken from DefaultTool. Share it somewhere.
void TableTool::mousePressEvent(KoPointerEvent* event)
{
    if (!d->tableShape->boundingRect().contains(event->point))
        return;
    const QPointF position = event->point - d->tableShape->position();
    Sheet* const sheet = d->tableShape->sheet();

    // In which cell did the user click ?
    double xpos;
    double ypos;
    int col = sheet->leftColumn(position.x(), xpos);
    int row = sheet->topRow(position.y(), ypos);
    // you cannot move marker when col > KS_colMax or row > KS_rowMax
    if (col > KS_colMax || row > KS_rowMax)
        return;

    // Extending an existing selection with the shift button ?
    if ((event->modifiers() & Qt::ShiftModifier) && sheet->doc()->isReadWrite() &&
         !d->selection->isColumnOrRowSelected())
    {
        d->selection->update(QPoint(col, row));
        return;
    }

    switch (event->button())
    {
        case Qt::LeftButton:
        {
            if (event->modifiers() & Qt::ControlModifier)
            {
                // Start a marking action
                d->mouseAction = Private::Mark;
                // extend the existing selection
                d->selection->extend(QPoint(col, row), sheet);
            }
            else
            {
                // Start a marking action
                d->mouseAction = Private::Mark;
                // reinitialize the selection
                d->selection->initialize(QPoint(col, row), sheet);
            }
            break;
        }
        case Qt::MidButton:
        {
        }
        case Qt::RightButton:
        {
            if (!d->selection->contains(QPoint(col, row)))
            {
                // No selection or the mouse press was outside of an existing selection?
                d->selection->initialize(QPoint(col, row), sheet);
            }
            break;
        }
        default:
            break;
    }
    d->tableShape->repaint();
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
        emit done();
        return;
    }
    d->selection->setActiveSheet(d->tableShape->sheet());
    d->selection->setOriginSheet(d->tableShape->sheet());
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

void TableTool::changeSelection(const Region& changedRegion)
{
    const Cell cell(d->tableShape->sheet(), d->selection->marker());
    emit userInputChanged(cell.userInput());
}

void TableTool::changeUserInput(const QString& content)
{
    d->userInput = content;
}

void TableTool::applyUserInput()
{
    DataManipulator* manipulator = new DataManipulator();
    manipulator->setSheet(d->tableShape->sheet());
    manipulator->setValue(Value(d->userInput));
    manipulator->setParsing(true);
    manipulator->setExpandMatrix(false);
    manipulator->add(*d->selection);
    manipulator->execute();
}

QWidget* TableTool::createOptionWidget()
{
    QWidget* optionWidget = new QWidget();
    QGridLayout* layout = new QGridLayout( optionWidget );

    QLabel* label = 0;
    QSpinBox* spinBox = 0;

    QLineEdit* lineEdit = new QLineEdit(optionWidget);
    layout->addWidget(lineEdit, 0, 1);
    connect(lineEdit, SIGNAL(editingFinished()), this, SLOT(applyUserInput()));
    connect(lineEdit, SIGNAL(textChanged(const QString&)), this, SLOT(changeUserInput(const QString&)));
    connect(this, SIGNAL(userInputChanged(const QString&)), lineEdit, SLOT(setText(const QString&)));
    changeSelection(Region()); // initialize the lineEdit with the cell content

    label = new QLabel(i18n("Content:"), optionWidget);
    label->setBuddy(lineEdit);
    label->setToolTip(i18n("Cell content"));
    layout->addWidget(label, 0, 0);

    spinBox = new QSpinBox( optionWidget );
    spinBox->setRange( 1, KS_colMax );
    spinBox->setValue( d->tableShape->columns() );
    layout->addWidget( spinBox, 2, 1 );
    connect( spinBox, SIGNAL( valueChanged(int) ), this, SLOT( changeColumns(int) ) );

    label = new QLabel( i18n( "Columns:" ), optionWidget );
    label->setBuddy( spinBox );
    label->setToolTip( i18n( "Number of columns" ) );
    layout->addWidget( label, 2, 0 );

    spinBox = new QSpinBox( optionWidget );
    spinBox->setRange( 1, KS_rowMax );
    spinBox->setValue( d->tableShape->rows() );
    layout->addWidget( spinBox, 3, 1 );
    connect( spinBox, SIGNAL( valueChanged(int) ), this, SLOT( changeRows(int) ) );

    label = new QLabel( i18n( "Rows:" ), optionWidget );
    label->setBuddy( spinBox );
    label->setToolTip( i18n( "Number of rows" ) );
    layout->addWidget( label, 3, 0 );

    layout->setColumnStretch( 1, 1 );
    layout->setRowStretch( 4, 1 );

    return optionWidget;
}

#include "TableTool.moc"
