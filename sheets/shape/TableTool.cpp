/* This file is part of the KDE project
   SPDX-FileCopyrightText: 2006 Stefan Nikolaus <stefan.nikolaus@kdemail.net>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

// Local
#include "TableTool.h"
#include "ScreenConversions.h"
#include "SheetsEditor.h"
#include "TableShape.h"
#include "ToolHeaders.h"

#include <QAction>
#include <QApplication>
#include <QFileDialog>
#include <QGridLayout>
#include <QLabel>
#include <QMimeDatabase>
#include <QPainter>
#include <QPushButton>
#include <QVBoxLayout>

#include <KComboBox>
#include <KLocalizedString>
#include <kpagedialog.h>

#include <KoCanvasBase.h>
#include <KoCanvasControllerWidget.h>
#include <KoIcon.h>
#include <KoPointerEvent.h>
#include <KoViewConverter.h>

#include "core/Map.h"
#include "core/Sheet.h"
#include "ui/SheetView.h"

#define TextShapeId "TextShapeID"

using namespace Calligra::Sheets;

ScrollBar::ScrollBar(Qt::Orientation o, QWidget *parent)
    : QScrollBar(o, parent)
{
    setMaximum(100);
    setPageStep(5);
    setSingleStep(1);
}
bool ScrollBar::event(QEvent *event)
{
    if (event->type() == QEvent::ShortcutOverride) {
        event->accept();
    }
    return QScrollBar::event(event);
}

class TableTool::Private
{
public:
    TableTool *q;
    Selection *selection;
    TableShape *tableShape;
    //     TableToolEditorShape *editorShape;
    KComboBox *sheetComboBox;
    QWidget *optionWidget;

    void setTools();
    void paintTools(QPainter &painter, const KoViewConverter &viewConverter);
    qreal xOffset, yOffset;
    QMarginsF margins;
    QRectF toolRect;
    QRectF shapeRect;
    QRectF columnRect;
    QRectF rowRect;
    QRectF verticalScrollRect;
    QRectF horizontalScrollRect;

    ToolType toolType;
    ToolType focus;

    Tool::ColumnHeader columnHeader;
    Tool::RowHeader rowHeader;
    ScrollBar *horizontalScrollBar;
    ScrollBar *verticalScrollBar;

    KoCanvasControllerWidget *controller;
};

void TableTool::Private::setTools()
{
    toolType = None;
    margins.setLeft(18.);
    margins.setRight(12.);
    margins.setTop(14.);
    margins.setBottom(12.);

    shapeRect = tableShape->boundingRect();
    toolRect = shapeRect + margins;

    const Sheet *sheet = tableShape->sheet();
    const QRect used = sheet->usedArea();
    const QRect selected = q->selection()->boundingRect();
    int maxColumn = qMax(used.width(), selected.right());
    horizontalScrollRect = QRectF(shapeRect.left(), shapeRect.bottom(), shapeRect.width(), margins.bottom());
    horizontalScrollBar->setFocusPolicy(Qt::StrongFocus);
    horizontalScrollBar->setInvertedControls(false);
    horizontalScrollBar->setMaximum(std::max(tableShape->size().width() * 10, sheet->columnPosition(maxColumn)));
    horizontalScrollBar->setPageStep(tableShape->size().width());
    horizontalScrollBar->setSingleStep(1);
    horizontalScrollBar->setValue(tableShape->topLeftOffset().x());
    horizontalScrollBar->show();
    connect(horizontalScrollBar, &QAbstractSlider::valueChanged, q, &TableTool::slotHorizontalScrollBarValueChanged);

    int maxRow = qMax(used.height(), selected.bottom());
    verticalScrollRect = QRectF(shapeRect.right(), shapeRect.top(), margins.right(), shapeRect.height());
    verticalScrollBar->setFocusPolicy(Qt::StrongFocus);
    verticalScrollBar->setMaximum(std::max(tableShape->size().height() * 10, sheet->rowPosition(maxRow)));
    verticalScrollBar->setPageStep(tableShape->size().height());
    verticalScrollBar->setSingleStep(1);
    verticalScrollBar->setValue(tableShape->topLeftOffset().y());
    verticalScrollBar->show();
    connect(verticalScrollBar, &QAbstractSlider::valueChanged, q, &TableTool::slotVerticalScrollBarValueChanged);

    columnRect = QRectF(shapeRect.left(), toolRect.top(), shapeRect.width(), margins.top());
    columnHeader.setGeometry(columnRect);
    columnHeader.setCanvas(controller->canvas());
    columnHeader.setScrollBars(horizontalScrollBar, verticalScrollBar);
    columnHeader.setSheetHeight(shapeRect.height());

    rowRect = QRectF(toolRect.left(), shapeRect.top(), margins.left(), shapeRect.height());
    rowHeader.setGeometry(rowRect);
    rowHeader.setCanvas(controller->canvas());
    rowHeader.setScrollBars(horizontalScrollBar, verticalScrollBar);
    rowHeader.setSheetWidth(shapeRect.width());
}

void TableTool::Private::paintTools(QPainter &painter, const KoViewConverter &viewConverter)
{
    if (shapeRect != tableShape->boundingRect()) {
        // The shape has been moved under our feet.
        // This can happen if anchor properties are changed.
        setTools();
    }
    QRectF tr = toolRect.translated(-shapeRect.topLeft());
    painter.fillRect(tr, qApp->palette().window());

    painter.save();
    QRectF clipRect(QPointF(), tableShape->size());
    painter.setClipRect(clipRect);
    tableShape->paintCells(painter);
    painter.restore();

    Sheet *sheet = tableShape->sheet();
    QRectF paintRect;

    painter.save();
    columnHeader.setSheet(sheet);
    paintRect = QRectF(QPointF(tableShape->topLeftOffset().x(), 0.0), columnRect.size());
    painter.translate(-paintRect.left(), -paintRect.height());
    painter.setClipRect(paintRect);
    columnHeader.paint(&painter, paintRect);
    painter.restore();

    painter.save();
    rowHeader.setSheet(sheet);
    paintRect = QRectF(QPointF(0.0, tableShape->topLeftOffset().y()), rowRect.size()); // rowRect.translated(-rowRect.width(), -rowRect.top());
    painter.translate(-paintRect.width(), -paintRect.top());
    painter.setClipRect(paintRect);
    rowHeader.paint(&painter, paintRect);
    painter.restore();

    const QPointF documentOrigin = q->canvas()->documentOrigin();
    const QPoint canvasScroll(controller->canvasOffsetX(), controller->canvasOffsetY());

    QRect rect = viewConverter.documentToView(horizontalScrollRect).translated(documentOrigin).toRect();
    rect.translate(canvasScroll);
    horizontalScrollBar->setGeometry(rect);

    rect = viewConverter.documentToView(verticalScrollRect).translated(documentOrigin).toRect();
    rect.translate(canvasScroll);
    verticalScrollBar->setGeometry(rect);

    painter.setPen(QPen(Qt::lightGray, 0));
    painter.drawRect(tr);
}

TableTool::TableTool(KoCanvasBase *canvas)
    : CellToolBase(canvas)
    , d(new Private)
{
    setObjectName(QLatin1String("TableTool"));
    d->q = this;
    d->selection = new Selection(canvas);
    d->controller = nullptr;
    for (QWidget *w = canvas->canvasWidget(); w; w = w->parentWidget()) {
        d->controller = qobject_cast<KoCanvasControllerWidget *>(w);
        if (d->controller) {
            break;
        }
    }
    d->horizontalScrollBar = new ScrollBar(Qt::Horizontal, d->controller);
    d->horizontalScrollBar->hide();
    d->verticalScrollBar = new ScrollBar(Qt::Vertical, d->controller);
    d->verticalScrollBar->hide();

    d->columnHeader.setSelection(d->selection);
    d->rowHeader.setSelection(d->selection);
    d->tableShape = nullptr;
    d->optionWidget = nullptr;

    QAction *importAction = new QAction(koIcon("document-import"), i18n("Import OpenDocument Spreadsheet File"), this);
    importAction->setIconText(i18n("Import"));
    addAction("import", importAction);
    connect(importAction, &QAction::triggered, this, &TableTool::importDocument);

    QAction *exportAction = new QAction(koIcon("document-export"), i18n("Export OpenDocument Spreadsheet File"), this);
    exportAction->setIconText(i18n("Export"));
    addAction("export", exportAction);
    connect(exportAction, &QAction::triggered, this, &TableTool::exportDocument);
}

TableTool::~TableTool()
{
    delete d->selection;
    delete d;
}

TableTool::ToolType TableTool::mouseOn(KoPointerEvent *event)
{
    if (!d->toolRect.contains(event->point)) {
        return None;
    }
    if (d->tableShape->hitTest(event->point)) {
        return Shape;
    }
    if (d->columnRect.contains(event->point)) {
        return ColumnHeader;
    }
    if (d->rowRect.contains(event->point)) {
        return RowHeader;
    }
    if (d->horizontalScrollRect.contains(event->point)) {
        return HScrollBar;
    }
    if (d->verticalScrollRect.contains(event->point)) {
        return VScrollBar;
    }
    return None;
}

void TableTool::mousePressEvent(KoPointerEvent *event)
{
    d->horizontalScrollBar->clearFocus();
    d->verticalScrollBar->clearFocus();
    switch (mouseOn(event)) {
    case Shape: {
        d->toolType = Shape;
        d->focus = Shape;
        CellToolBase::mousePressEvent(event);
        break;
    }
    case ColumnHeader: {
        d->toolType = ColumnHeader;
        d->focus = ColumnHeader;
        selection()->emitCloseEditor(true);
        KoPointerEvent e(event, event->point - d->columnRect.topLeft() + d->tableShape->topLeftOffset());
        d->columnHeader.mousePress(&e);
        if (event->button() == Qt::RightButton) {
            setPopupActionList(popupMenuActionList());
            event->accept();
        }
        break;
    }
    case RowHeader: {
        d->toolType = RowHeader;
        d->focus = RowHeader;
        selection()->emitCloseEditor(true);
        KoPointerEvent e(event, event->point - d->rowRect.topLeft() + d->tableShape->topLeftOffset());
        d->rowHeader.mousePress(&e);
        if (event->button() == Qt::RightButton) {
            setPopupActionList(popupMenuActionList());
            event->accept();
        }
        break;
    }
    case HScrollBar: {
        d->toolType = HScrollBar;
        d->focus = HScrollBar;
        selection()->emitCloseEditor(true);
        d->horizontalScrollBar->setFocus();
        QPoint pos = ScreenConversions::scaleFromPtToPx(event->point - d->horizontalScrollRect.topLeft());
        QMouseEvent e(QEvent::MouseButtonPress, pos, event->globalPos(), event->globalPos(), event->button(), event->buttons(), event->modifiers());
        qApp->sendEvent(d->horizontalScrollBar, &e);
        canvas()->updateCanvas(d->toolRect);
        break;
    }
    case VScrollBar: {
        d->toolType = VScrollBar;
        d->focus = VScrollBar;
        selection()->emitCloseEditor(true);
        d->verticalScrollBar->setFocus();
        QPoint pos = ScreenConversions::scaleFromPtToPx(event->point - d->verticalScrollRect.topLeft());
        QMouseEvent e(QEvent::MouseButtonPress, pos, event->globalPos(), event->globalPos(), event->button(), event->buttons(), event->modifiers());
        qApp->sendEvent(d->verticalScrollBar, &e);
        canvas()->updateCanvas(d->toolRect);
        break;
    }
    default: {
        d->toolType = None;
        d->focus = None;
        event->ignore();
        break;
    }
    }
}

void TableTool::mouseReleaseEvent(KoPointerEvent *event)
{
    switch (d->toolType) {
    case Shape: {
        d->toolType = None;
        CellToolBase::mouseReleaseEvent(event);
        break;
    }
    case ColumnHeader: {
        d->toolType = None;
        KoPointerEvent e(event, event->point - d->columnRect.topLeft() + d->tableShape->topLeftOffset());
        d->columnHeader.mouseRelease(&e);
        canvas()->updateCanvas(d->toolRect);
        break;
    }
    case RowHeader: {
        d->toolType = None;
        KoPointerEvent e(event, event->point - d->rowRect.topLeft() + d->tableShape->topLeftOffset());
        d->rowHeader.mouseRelease(&e);
        canvas()->updateCanvas(d->toolRect);
        break;
    }
    case HScrollBar: {
        d->toolType = None;
        QPoint pos = ScreenConversions::scaleFromPtToPx(event->point - d->horizontalScrollRect.topLeft());
        QMouseEvent e(QEvent::MouseButtonRelease, pos, event->globalPos(), event->globalPos(), event->button(), event->buttons(), event->modifiers());
        qApp->sendEvent(d->horizontalScrollBar, &e);
        canvas()->updateCanvas(d->toolRect);
        break;
    }
    case VScrollBar: {
        d->toolType = None;
        QPoint pos = ScreenConversions::scaleFromPtToPx(event->point - d->verticalScrollRect.topLeft());
        QMouseEvent e(QEvent::MouseButtonRelease, pos, event->globalPos(), event->globalPos(), event->button(), event->buttons(), event->modifiers());
        qApp->sendEvent(d->verticalScrollBar, &e);
        canvas()->updateCanvas(d->toolRect);
        break;
    }
    default: {
        d->toolType = None;
        if (!d->toolRect.contains(event->point)) {
            event->ignore();
        }
        break;
    }
    }
}

void TableTool::mouseMoveEvent(KoPointerEvent *event)
{
    ToolType type = d->toolType == None ? mouseOn(event) : d->toolType;
    switch (type) {
    case Shape:
        CellToolBase::mouseMoveEvent(event);
        break;
    case ColumnHeader: {
        KoPointerEvent e(event, event->point - d->columnRect.topLeft() + d->tableShape->topLeftOffset());
        d->columnHeader.mouseMove(&e);
        break;
    }
    case RowHeader: {
        KoPointerEvent e(event, event->point - d->rowRect.topLeft() + d->tableShape->topLeftOffset());
        d->rowHeader.mouseMove(&e);
        break;
    }
    case HScrollBar: {
        QPoint pos = ScreenConversions::scaleFromPtToPx(event->point - d->horizontalScrollRect.topLeft());
        QMouseEvent e(QEvent::MouseMove, pos, event->globalPos(), event->globalPos(), event->button(), event->buttons(), event->modifiers());
        qApp->sendEvent(d->horizontalScrollBar, &e);
        canvas()->updateCanvas(d->toolRect);
        break;
    }
    case VScrollBar: {
        QPoint pos = ScreenConversions::scaleFromPtToPx(event->point - d->verticalScrollRect.topLeft());
        QMouseEvent e(QEvent::MouseMove, pos, event->globalPos(), event->globalPos(), event->button(), event->buttons(), event->modifiers());
        qApp->sendEvent(d->verticalScrollBar, &e);
        canvas()->updateCanvas(d->toolRect);
        break;
    }
    default:
        if (!d->toolRect.contains(event->point)) {
            event->ignore();
        }
        break;
    }
}

void TableTool::mouseDoubleClickEvent(KoPointerEvent *event)
{
    switch (mouseOn(event)) {
    case Shape: {
        d->toolType = Shape;
        d->focus = Shape;
        CellToolBase::mouseDoubleClickEvent(event);
        break;
    }
    case ColumnHeader: {
        d->toolType = ColumnHeader;
        d->focus = ColumnHeader;
        selection()->emitCloseEditor(true);
        KoPointerEvent e(event, event->point - d->columnRect.topLeft());
        d->columnHeader.mouseDoubleClick(&e);
        break;
    }
    case RowHeader: {
        d->toolType = RowHeader;
        d->focus = RowHeader;
        selection()->emitCloseEditor(true);
        KoPointerEvent e(event, event->point - d->rowRect.topLeft());
        d->rowHeader.mouseDoubleClick(&e);
        break;
    }
    case HScrollBar: {
        d->toolType = HScrollBar;
        d->focus = HScrollBar;
        selection()->emitCloseEditor(true);
        d->horizontalScrollBar->setFocus();
        QPoint pos = ScreenConversions::scaleFromPtToPx(event->point - d->horizontalScrollRect.topLeft());
        QMouseEvent e(QEvent::MouseButtonDblClick, pos, event->globalPos(), event->globalPos(), event->button(), event->buttons(), event->modifiers());
        qApp->sendEvent(d->horizontalScrollBar, &e);
        canvas()->updateCanvas(d->toolRect);
        break;
    }
    case VScrollBar: {
        d->toolType = VScrollBar;
        d->focus = VScrollBar;
        selection()->emitCloseEditor(true);
        d->verticalScrollBar->setFocus();
        QPoint pos = ScreenConversions::scaleFromPtToPx(event->point - d->verticalScrollRect.topLeft());
        QMouseEvent e(QEvent::MouseButtonDblClick, pos, event->globalPos(), event->globalPos(), event->button(), event->buttons(), event->modifiers());
        qApp->sendEvent(d->verticalScrollBar, &e);
        canvas()->updateCanvas(d->toolRect);
        break;
    }
    default: {
        d->toolType = None;
        d->focus = None;
        if (!d->toolRect.contains(event->point)) {
            event->ignore();
        }
        break;
    }
    }
}

void TableTool::keyPressEvent(QKeyEvent *event)
{
    switch (d->focus) {
    case Shape:
        if (event->key() == Qt::Key_Menu) {
            setPopupActionList(popupMenuActionList());
        }
        CellToolBase::keyPressEvent(event);
        event->accept();
        break;
    case ColumnHeader:
    case RowHeader:
        if (event->key() == Qt::Key_Menu) {
            setPopupActionList(popupMenuActionList());
        }
        event->accept();
        break;
    case HScrollBar:
    case VScrollBar:
        event->accept();
        break;
    default:
        break;
    }
}

KoInteractionStrategy *TableTool::createStrategy(KoPointerEvent *event)
{
    if (d->tableShape->hitTest(event->point)) {
        return CellToolBase::createStrategy(event);
    }
    return nullptr;
}

void TableTool::importDocument()
{
    const QString filterString = QMimeDatabase().mimeTypeForName("application/vnd.oasis.opendocument.spreadsheet").filterString();
    // TODO: i18n for title
    QString file = QFileDialog::getOpenFileName(0, "Import", QString(), filterString);
    if (file.isEmpty())
        return;
#if 0 // FIXME Stefan: Port!
    d->tableShape->doc()->setModified(false);
    if (! d->tableShape->doc()->importDocument(file))
        return;
#endif
    updateSheetsList();
    //     if (Sheet* sheet = d->tableShape->sheet()) {
    //         QRect area = sheet->usedArea();
    //         if (area.width() > d->tableShape->columns())
    //             d->tableShape->setColumns(area.width());
    //         if (area.height() > d->tableShape->rows())
    //             d->tableShape->setRows(area.height());
    //     }
}

void TableTool::exportDocument()
{
    const QString filterString = QMimeDatabase().mimeTypeForName("application/vnd.oasis.opendocument.spreadsheet").filterString();
    // TODO: i18n for title
    QString file = QFileDialog::getSaveFileName(0, "Export", QString(), filterString);
    if (file.isEmpty())
        return;
#if 0 // FIXME Stefan: Port!
    d->tableShape->doc()->exportDocument(file);
#endif
}

void TableTool::repaintDecorations()
{
    if (!d->tableShape)
        return;
    // TODO Stefan: restrict to the changed area
    canvas()->updateCanvas(d->tableShape->boundingRect());
}

Selection *TableTool::selection()
{
    return d->selection;
}

void TableTool::slotSelectionChanged(const Region &)
{
    scrollToCell(selection()->cursor());
    canvas()->updateCanvas(d->toolRect);
}

void TableTool::activate(ToolActivation toolActivation, const QSet<KoShape *> &shapes)
{
    for (KoShape *shape : shapes) {
        d->tableShape = dynamic_cast<TableShape *>(shape);
        if (d->tableShape)
            break;
    }
    if (!d->tableShape) {
        warnSheets << "No table shape found in selection.";
        Q_EMIT done();
        return;
    }
    d->tableShape->setPaintingDisabled(true);
    d->setTools();

    d->selection->setActiveSheet(d->tableShape->sheet());
    d->selection->setOriginSheet(d->tableShape->sheet());
    useCursor(Qt::ArrowCursor);

    CellToolBase::activate(toolActivation, shapes);
    connect(d->selection, &Selection::changed, this, &TableTool::slotSelectionChanged);

    activateSheet();
}

void TableTool::deactivate()
{
    d->horizontalScrollBar->hide();
    d->verticalScrollBar->hide();

    // undoing createShape crash if we where editing a cell and try to apply user input
    deleteEditor(false /*do not save*/);
    CellToolBase::deactivate();

    disconnect(d->tableShape->sheet(), &Sheet::documentSizeChanged, this, &TableTool::update);
    d->tableShape->setPaintingDisabled(false);
    d->tableShape->update();
    d->tableShape = nullptr;
    canvas()->updateCanvas(d->toolRect);
}

void TableTool::paint(QPainter &painter, const KoViewConverter &viewConverter)
{
    QTransform painterMatrix = painter.worldTransform();
    painter.setWorldTransform(d->tableShape->absoluteTransformation(&viewConverter) * painterMatrix);
    KoShape::applyConversion(painter, viewConverter);

    d->tableShape->sheetView()->setViewConverter(&viewConverter);
    // paint decoration
    d->paintTools(painter, viewConverter);

    painter.setClipRect(QRectF(QPointF(), size()));
    painter.translate(-d->tableShape->topLeftOffset());
    const QRectF paintRect = QRectF(d->tableShape->topLeftOffset(), size());
    paintReferenceSelection(painter, paintRect);
    paintSelection(painter, paintRect);
}

QPointF TableTool::offset() const
{
    QPointF point = d->tableShape->absolutePosition(KoFlake::TopLeftCorner);
    point -= d->tableShape->topLeftOffset();
    return point;
}

QSizeF TableTool::size() const
{
    return d->tableShape->size();
}

double TableTool::canvasOffsetX() const
{
    double x = canvas()->viewConverter()->viewToDocumentX(-canvas()->canvasController()->scrollBarValue().x());
    x += canvas()->documentOrigin().x();
    return x;
}

double TableTool::canvasOffsetY() const
{
    double y = canvas()->viewConverter()->viewToDocumentX(-canvas()->canvasController()->scrollBarValue().y());
    y += canvas()->documentOrigin().y();
    return y;
}

// not used anywhere
QPointF TableTool::canvasOffset() const
{
    return QPointF();
}

int TableTool::maxCol() const
{
    return KS_colMax;
}

int TableTool::maxRow() const
{
    return KS_rowMax;
}

SheetView *TableTool::sheetView(Sheet *sheet) const
{
    Q_UNUSED(sheet);
    return d->tableShape->sheetView();
}

void TableTool::updateSheetsList()
{
    d->sheetComboBox->blockSignals(true);
    d->sheetComboBox->clear();
    Map *map = d->tableShape->map();
    for (SheetBase *sheet : map->sheetList()) {
        if (sheet->isHidden())
            continue;
        d->sheetComboBox->addItem(sheet->sheetName());
        // d->sheetComboBox->setCurrentIndex( d->sheetComboBox->count()-1 );
    }
    d->sheetComboBox->blockSignals(false);
}

void TableTool::update()
{
    canvas()->updateCanvas(d->toolRect);
}

void TableTool::activateSheet()
{
    Sheet *sheet = d->tableShape->sheet();
    if (sheet) {
        connect(sheet, &Sheet::documentSizeChanged, this, &TableTool::update);
    }
    update();
}

void TableTool::sheetActivated(const QString &sheetName)
{
    Sheet *sheet = d->tableShape->sheet();
    if (sheet) {
        disconnect(sheet, &Sheet::documentSizeChanged, this, &TableTool::update);
    }
    d->tableShape->setSheet(sheetName);
    activateSheet();
}

void TableTool::sheetsBtnClicked()
{
    QPointer<KPageDialog> dialog = new KPageDialog();
    dialog->setWindowTitle(i18n("Sheets"));
    dialog->setStandardButtons(QDialogButtonBox::Ok);
    dialog->setFaceType(KPageDialog::Plain);
    SheetsEditor *editor = new SheetsEditor(d->tableShape);
    dialog->layout()->addWidget(editor);
    dialog->exec();
    updateSheetsList();
    delete dialog;
}

QList<QPointer<QWidget>> TableTool::createOptionWidgets()
{
    if (!d->optionWidget) {
        d->optionWidget = new QWidget();
        d->optionWidget->setObjectName(QLatin1String("TableTool/Table Options"));

        QVBoxLayout *l = new QVBoxLayout(d->optionWidget);
        l->setMargin(0);
        d->optionWidget->setLayout(l);

        QGridLayout *layout = new QGridLayout();
        l->addLayout(layout);

        QLabel *label = nullptr;

        QHBoxLayout *sheetlayout = new QHBoxLayout();
        sheetlayout->setMargin(0);
        sheetlayout->setSpacing(3);
        layout->addLayout(sheetlayout, 0, 1);
        d->sheetComboBox = new KComboBox(d->optionWidget);
        sheetlayout->addWidget(d->sheetComboBox, 1);
        Map *map = d->tableShape->map();
        for (SheetBase *s : map->sheetList()) {
            d->sheetComboBox->addItem(s->sheetName());
            // d->sheetComboBox->setCurrentIndex( d->sheetComboBox->count()-1 );
        }
        connect(d->sheetComboBox, &KComboBox::textActivated, this, &TableTool::sheetActivated);

        QPushButton *sheetbtn = new QPushButton(koIcon("table"), QString(), d->optionWidget);
        sheetbtn->setFixedHeight(d->sheetComboBox->sizeHint().height());
        connect(sheetbtn, &QAbstractButton::clicked, this, &TableTool::sheetsBtnClicked);
        sheetlayout->addWidget(sheetbtn);
        label = new QLabel(i18n("Sheet:"), d->optionWidget);
        label->setBuddy(d->sheetComboBox);
        label->setToolTip(i18n("Selected Sheet"));
        layout->addWidget(label, 0, 0);

        // layout->setColumnStretch( 1, 1 );
        layout->setRowStretch(6, 1);

        // TODO implement (or remove)
        //         QToolBar* tb = new QToolBar(d->optionWidget);
        //         l->addWidget(tb);
        //         tb->setMovable(false);
        //         tb->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
        //         tb->addAction(action("import"));
        //         tb->addAction(action("export"));

        d->optionWidget->setWindowTitle(i18n("View Options"));
    }
    QList<QPointer<QWidget>> ow = CellToolBase::createOptionWidgets();
    ow.append(d->optionWidget);
    return ow;
}

void TableTool::slotHorizontalScrollBarValueChanged(int value)
{
    QPointF pos = d->tableShape->topLeftOffset();
    pos.rx() = value;
    d->tableShape->setTopLeftOffset(pos);
    canvas()->updateCanvas(d->toolRect);
}

void TableTool::slotVerticalScrollBarValueChanged(int value)
{
    QPointF pos = d->tableShape->topLeftOffset();
    pos.ry() = value;
    d->tableShape->setTopLeftOffset(pos);
    canvas()->updateCanvas(d->toolRect);
}

void TableTool::scrollToCell(const QPoint &location)
{
    Sheet *sheet = d->tableShape->sheet();
    const QRectF bounds(d->tableShape->topLeftOffset(), d->tableShape->size());
    const Cell cell = Cell(sheet, location).masterCell();

    const qreal left = sheet->columnPosition(cell.cellPosition().x());
    const qreal right = left + cell.width();
    if (left < bounds.left()) {
        d->horizontalScrollBar->setValue(left);
    } else if (right > bounds.right()) {
        d->horizontalScrollBar->setMaximum(right - bounds.width() + 1);
        d->horizontalScrollBar->setValue(right - bounds.width() + 1);
    }

    const qreal top = sheet->rowPosition(cell.cellPosition().y());
    const qreal bottom = top + cell.height();
    if (top < bounds.top()) {
        d->verticalScrollBar->setValue(top);
    } else if (bottom > bounds.bottom()) {
        d->verticalScrollBar->setMaximum(bottom - bounds.height() + 1);
        d->verticalScrollBar->setValue(bottom - bounds.height() + 1);
    }
}
