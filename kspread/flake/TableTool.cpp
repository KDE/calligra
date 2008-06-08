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
#include "SheetsEditor.h"

#include <QApplication>
#include <QGridLayout>
#include <QLabel>
#include <QPainter>
#include <QSpinBox>
#include <QToolBar>
#include <QPushButton>

#include <kcombobox.h>
#include <kdebug.h>
#include <klineedit.h>
#include <klocale.h>
#include <kicon.h>
#include <kaction.h>
#include <kfiledialog.h>
#include <kpagedialog.h>

#include <KoCanvasBase.h>
#include <KoPointerEvent.h>
#include <KoSelection.h>
#include <KoShapeManager.h>

#include "Cell.h"
#include "Doc.h"
#include "Global.h"
#include "Selection.h"
#include "SelectionStrategy.h"
#include "Sheet.h"
#include "Map.h"

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

    KComboBox* sheetComboBox;
};


TableTool::TableTool( KoCanvasBase* canvas )
    : KoInteractionTool( canvas )
    , d( new Private )
{
    setObjectName("TableTool");

    d->selection = new Selection(this);
    connect(d->selection, SIGNAL(changed(const Region&)), this, SLOT(changeSelection(const Region&)));
    d->tableShape = 0;

    KAction* importAction = new KAction(KIcon("document-import"), i18n("Import OpenDocument Spreadsheet File"), this);
    importAction->setIconText(i18n("Import"));
    addAction("import", importAction);
    connect(importAction, SIGNAL(triggered()), this, SLOT(importDocument()));

    KAction* exportAction = new KAction(KIcon("document-export"), i18n("Export OpenDocument Spreadsheet File"), this);
    exportAction->setIconText(i18n("Export"));
    addAction("export", exportAction);
    connect(exportAction, SIGNAL(triggered()), this, SLOT(exportDocument()));
}

TableTool::~TableTool()
{
    delete d->selection;
    delete d;
}

void TableTool::importDocument()
{
    QString file = KFileDialog::getOpenFileName(KUrl(), "application/vnd.oasis.opendocument.spreadsheet", 0, "Import");
    if (file.isEmpty())
        return;
    d->tableShape->doc()->setModified(false);
    if ( ! d->tableShape->doc()->importDocument(file))
        return;
    updateSheetsList();
    if (Sheet* sheet = d->tableShape->sheet()) {
        QRect area = sheet->usedArea();
        if (area.width() > d->tableShape->columns())
            d->tableShape->setColumns(area.width());
        if (area.height() > d->tableShape->rows())
            d->tableShape->setRows(area.height());
    }
}

void TableTool::exportDocument()
{
    QString file = KFileDialog::getSaveFileName(KUrl(), "application/vnd.oasis.opendocument.spreadsheet", 0, "Export");
    if (file.isEmpty())
        return;
    d->tableShape->doc()->exportDocument(file);
}

void TableTool::repaintDecorations()
{
    // TODO Stefan: restrict to the changed area
    m_canvas->updateCanvas(d->tableShape->boundingRect());
}

void TableTool::paint( QPainter& painter, const KoViewConverter& viewConverter )
{
    KoShape::applyConversion(painter, viewConverter);

    // get the transparent selection color
    QColor selectionColor(QApplication::palette().highlight().color());
    selectionColor.setAlpha(127);

    // Set the pen.
    const double unzoomedPixel = viewConverter.viewToDocumentX(1.0);
    const QPen pen(QApplication::palette().text().color(), unzoomedPixel * 2.0);
    painter.setPen(pen);

    // Clip out the
    painter.save();
    const QRect markerRange = QRect(d->selection->marker(), d->selection->marker());
    const QRectF markerRect = d->tableShape->sheet()->cellCoordinatesToDocument(markerRange);
    painter.setClipRegion(painter.clipRegion().subtracted(markerRect.translated(d->tableShape->position()).toRect()));

    const QRect lastRange = d->selection->lastRange();
    Region::ConstIterator end = d->selection->constEnd();
    for (Region::ConstIterator it = d->selection->constBegin(); it != end; ++it)
    {
        const QRectF rect = d->tableShape->sheet()->cellCoordinatesToDocument((*it)->rect());
        // draw the transparent selection background
        painter.fillRect(rect.translated(d->tableShape->position()), selectionColor);
        if ((*it)->rect() == lastRange)
            painter.restore(); // no clipping for the frame
        painter.drawRect(rect.translated(d->tableShape->position()));
    }

    const QRectF rect = d->tableShape->sheet()->cellCoordinatesToDocument(d->selection->lastRange());
    const QPointF handleOffset = QPointF(unzoomedPixel * 2.0, unzoomedPixel * 2.0);
    if (d->tableShape->sheet()->layoutDirection() == Qt::LeftToRight)
        painter.drawRect(QRectF(rect.bottomRight() + d->tableShape->position() - handleOffset,
                                rect.bottomRight() + d->tableShape->position() + handleOffset));
    else
        painter.drawRect(QRectF(rect.bottomLeft() + d->tableShape->position() - handleOffset,
                                rect.bottomLeft() + d->tableShape->position() + handleOffset));
}

void TableTool::mousePressEvent(KoPointerEvent* event)
{
    KoInteractionTool::mousePressEvent(event);
}

void TableTool::mouseMoveEvent(KoPointerEvent* event)
{
    // Indicators are not neccessary for protected sheets or if there's a strategy.
    if (d->tableShape->sheet()->isProtected() || m_currentStrategy)
        return KoInteractionTool::mouseMoveEvent(event);

    // Get info about where the event occurred.
    QPointF position = event->point - d->tableShape->position();
    if (d->tableShape->sheet()->layoutDirection() == Qt::RightToLeft)
        position.setX(d->tableShape->size().width() - position.x());

    // Diagonal cursor, if the selection handle was hit.
    if (d->selection->selectionHandleArea(canvas()->viewConverter()).contains(position))
    {
        if (d->tableShape->sheet()->layoutDirection() == Qt::RightToLeft)
            useCursor(Qt::SizeBDiagCursor);
        else
            useCursor(Qt::SizeFDiagCursor);
        return KoInteractionTool::mouseMoveEvent(event);
    }

    // Hand cursor, if the selected area was hit.
    Region::ConstIterator end = d->selection->constEnd();
    for (Region::ConstIterator it = d->selection->constBegin(); it != end; ++it)
    {
        const QRect range = (*it)->rect();
        if (d->tableShape->sheet()->cellCoordinatesToDocument(range).contains(position))
        {
            useCursor(Qt::PointingHandCursor);
            return KoInteractionTool::mouseMoveEvent(event);
        }
    }

    // Reset to normal cursor.
    useCursor(Qt::ArrowCursor);
    KoInteractionTool::mouseMoveEvent(event);
}

void TableTool::mouseReleaseEvent(KoPointerEvent* event)
{
    KoInteractionTool::mouseReleaseEvent(event);
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
    d->tableShape->update();
}

void TableTool::deactivate()
{
    d->tableShape = 0;
}

KoInteractionStrategy* TableTool::createStrategy(KoPointerEvent* event)
{
    // Get info about where the event occurred.
    QPointF position = event->point - d->tableShape->position();
    if (d->tableShape->sheet()->layoutDirection() == Qt::RightToLeft)
        position.setX(d->tableShape->size().width() - position.x());

    // Autofilling or merging, if the selection handle was hit.
    if (d->selection->selectionHandleArea(canvas()->viewConverter()).contains(position))
    {
        if (event->button() == Qt::LeftButton)
            kDebug() << "autofill"; // TODO Stefan: autofill
        else if (event->button() == Qt::MidButton)
            kDebug() << "merging"; // TODO Stefan: merging cells
    }

    // Pasting with the middle mouse button.
    if (event->button() == Qt::MidButton)
        kDebug() << "pasting"; // TODO Stefan: pasting

    // Context menu with the right mouse button.
    if (event->button() == Qt::RightButton)
        kDebug() << "popup"; // TODO Stefan: popup menu

    // Drag & drop, if the selected area was hit.
    Region::ConstIterator end = d->selection->constEnd();
    for (Region::ConstIterator it = d->selection->constBegin(); it != end; ++it)
    {
        const QRect range = (*it)->rect();
        if (d->tableShape->sheet()->cellCoordinatesToDocument(range).contains(position))
            kDebug() << "drag&drop"; // TODO Stefan: drag & drop
    }

    // TODO Stefan: reference selection
    return new SelectionStrategy(this, canvas(), d->selection, event->point, event->modifiers());
}

void TableTool::changeColumns( int num )
{
    d->tableShape->setColumns( num );
    d->tableShape->update();
}

void TableTool::changeRows( int num )
{
    d->tableShape->setRows( num );
    d->tableShape->update();
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
    if( ! d->tableShape )
        return;
    DataManipulator* manipulator = new DataManipulator();
    manipulator->setSheet(d->tableShape->sheet());
    manipulator->setValue(Value(d->userInput));
    manipulator->setParsing(true);
    manipulator->setExpandMatrix(false);
    manipulator->add(*d->selection);
    manipulator->execute();
}

void TableTool::updateSheetsList()
{
d->sheetComboBox->blockSignals(true);
    d->sheetComboBox->clear();
    Map *map = d->tableShape->doc()->map();
    foreach(Sheet* sheet, map->sheetList()) {
        if (sheet->isHidden())
            continue;
        d->sheetComboBox->addItem( sheet->sheetName() );
        //d->sheetComboBox->setCurrentIndex( d->sheetComboBox->count()-1 );
    }
d->sheetComboBox->blockSignals(false);
}

void TableTool::sheetActivated(const QString& sheetName)
{
    if( d->tableShape )
        d->tableShape->setSheet(sheetName);
}

void TableTool::sheetsBtnClicked()
{
    KPageDialog* dialog = new KPageDialog();
    dialog->setCaption(i18n("Sheets"));
    dialog->setButtons( KDialog::Ok );
    dialog->setFaceType(KPageDialog::Plain);
    SheetsEditor* editor = new SheetsEditor(d->tableShape);
    dialog->setMainWidget(editor);
    dialog->exec();
    updateSheetsList();
    delete dialog;
}

QWidget* TableTool::createOptionWidget()
{
    QWidget* optionWidget = new QWidget();
    QVBoxLayout* l = new QVBoxLayout( optionWidget );
    l->setMargin(0);
    optionWidget->setLayout(l);

    QGridLayout* layout = new QGridLayout( optionWidget );
    l->addLayout(layout);

    QLabel* label = 0;
    QSpinBox* spinBox = 0;

    QHBoxLayout* sheetlayout = new QHBoxLayout(optionWidget);
    sheetlayout->setMargin(0);
    sheetlayout->setSpacing(3);
    layout->addLayout(sheetlayout, 0, 1);
    d->sheetComboBox = new KComboBox(optionWidget);
    sheetlayout->addWidget(d->sheetComboBox, 1);
    Map *map = d->tableShape->doc()->map();
    foreach(Sheet* s, map->sheetList()) {
        d->sheetComboBox->addItem(s->sheetName());
        //d->sheetComboBox->setCurrentIndex( d->sheetComboBox->count()-1 );
    }
    connect(d->sheetComboBox, SIGNAL(activated(QString)), this, SLOT(sheetActivated(QString)));

    QPushButton *sheetbtn = new QPushButton(KIcon("table"), QString(), optionWidget);
    sheetbtn->setFixedHeight( d->sheetComboBox->sizeHint().height() );
    connect(sheetbtn, SIGNAL(clicked()), this, SLOT(sheetsBtnClicked()));
    sheetlayout->addWidget(sheetbtn);
    label = new QLabel(i18n("Sheet:"), optionWidget);
    label->setBuddy(d->sheetComboBox);
    label->setToolTip(i18n("Selected Sheet"));
    layout->addWidget(label, 0, 0);

    KLineEdit* lineEdit = new KLineEdit(optionWidget);
    layout->addWidget(lineEdit, 1, 1);
    connect(lineEdit, SIGNAL(editingFinished()), this, SLOT(applyUserInput()));
    connect(lineEdit, SIGNAL(textChanged(const QString&)), this, SLOT(changeUserInput(const QString&)));
    connect(this, SIGNAL(userInputChanged(const QString&)), lineEdit, SLOT(setText(const QString&)));
    changeSelection(Region()); // initialize the lineEdit with the cell content

    label = new QLabel(i18n("Content:"), optionWidget);
    label->setBuddy(lineEdit);
    label->setToolTip(i18n("Cell content"));
    layout->addWidget(label, 1, 0);

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

//layout->setColumnStretch( 1, 1 );
    layout->setRowStretch( 4, 1 );

    QToolBar* tb = new QToolBar(optionWidget);
    l->addWidget(tb);
    tb->setMovable(false);
    tb->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
    tb->addAction( action("import") );
    tb->addAction( action("export") );

    return optionWidget;
}

#include "TableTool.moc"
