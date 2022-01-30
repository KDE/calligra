/* This file is part of the KDE project
   SPDX-FileCopyrightText: 2010 Marijn Kruisselbrink <mkruisselbrink@kde.org>
   SPDX-FileCopyrightText: 2007 Stefan Nikolaus <stefan.nikolaus@kdemail.net>
   SPDX-FileCopyrightText: 1998, 1999 Torben Weis <weis@kde.org>
   SPDX-FileCopyrightText: 1999-2007 The KSpread Team <calligra-devel@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

// Local
#include "Sheet.h"

// #include <QApplication>

// #include <kcodecs.h>

// #include <KoShape.h>
// #include <KoDocumentResourceManager.h>
// #include <KoUnit.h>
// #include <KoCanvasResourceIdentities.h>

// #include "CellStorage.h"
// #include "Cluster.h"
// #include "Damages.h"
// #include "DocBase.h"
#include "engine/Formula.h"
// #include "FormulaStorage.h"
// #include "HeaderFooter.h"
// #include "LoadingInfo.h"
// #include "Map.h"
// #include "NamedAreaManager.h"
// #include "PrintSettings.h"
// #include "RowColumnFormat.h"
// #include "RowFormatStorage.h"
// #include "ShapeApplicationData.h"
// #include "SheetPrint.h"
// #include "SheetModel.h"
// #include "StyleStorage.h"
// #include "Validity.h"
// #include "ValueConverter.h"
// #include "ValueStorage.h"
// #include "DataFilter.h"

namespace Calligra
{
namespace Sheets
{

static QString createObjectName(const QString &sheetName)
{
    QString objectName;
    for (int i = 0; i < sheetName.count(); ++i) {
        if (sheetName[i].isLetterOrNumber() || sheetName[i] == '_')
            objectName.append(sheetName[i]);
        else
            objectName.append('_');
    }
    return objectName;
}


class Q_DECL_HIDDEN Sheet::Private
{
public:
    Private(Sheet* sheet) : rows(sheet), canvasOffsetX(0), canvasOffsetY(0) {}

    // We store this here even though MapBase also has a copy, so that we don't need to recast from MapBase to Map in main app.
    Map* workbook;
    SheetModel *model;

    Qt::LayoutDirection layoutDirection;

    // true if sheet is hidden
    bool hide;

    bool showGrid;
    bool showFormula;
    bool showFormulaIndicator;
    bool showCommentIndicator;
    bool lcMode;
    bool showColumnNumber;
    bool hideZero;

    // clusters to hold objects
    
    // This is the same object as the cellStorage in our base class. Keeping a separate pointer  so we don't need to dynamic_cast when needed.
    // A better solution may be needed here, but for now this'll do.
    CellStorage* cellStorage;
    RowFormatStorage rows;
    ColumnCluster columns;
    QList<KoShape*> shapes;

    // hold the print object
    SheetPrint* print;

    // Indicates whether the sheet should paint the page breaks.
    // Doing so costs some time, so by default it should be turned off.
    bool showPageOutline;

    // Max range of canvas in x and y direction.
    //  Depends on KS_colMax/KS_rowMax and the width/height of all columns/rows
    QSizeF documentSize;

    QImage backgroundImage;
    Sheet::BackgroundImageProperties backgroundProperties;

    int canvasOffsetX;
    int canvasOffsetY;
};


Sheet::Sheet(Map* map, const QString &sheetName)
        : KoShapeUserData(map)
        , SheetBase(map, sheetName)
        , KoShapeBasedDocumentBase()
        , d(new Private(this))
{
    d->workbook = map;
    if (map->doc()) {
        resourceManager()->setUndoStack(map->doc()->undoStack());
        QVariant variant;
        variant.setValue<void*>(map->doc()->sheetAccessModel());
        resourceManager()->setResource(::Sheets::CanvasResource::AccessModel, variant);
    }
    d->model = new SheetModel(this);

    d->layoutDirection = QApplication::layoutDirection();

    // Set a valid object name, so that we can offer scripting.
    setObjectName(createObjectName(sheetName));

    d->cellStorage = new CellStorage(this);
    setCellStorage(d->cellStorage);
    d->columns.setAutoDelete(true);

    d->documentSize = QSizeF(KS_colMax * d->workbook->defaultColumnFormat()->width(),
                             KS_rowMax * d->workbook->defaultRowFormat()->height());

    d->hide = false;
    d->showGrid = true;
    d->showFormula = false;
    d->showFormulaIndicator = false;
    d->showCommentIndicator = true;
    d->showPageOutline = false;

    d->lcMode = false;
    d->showColumnNumber = false;
    d->hideZero = false;
    d->print = new SheetPrint(this);

    // document size changes always trigger a visible size change
    connect(this, &Sheet::documentSizeChanged, this, &Sheet::visibleSizeChanged);
    // CellStorage connections
    connect(d->cellStorage, &CellStorage::insertNamedArea,
            d->workbook->namedAreaManager(), &NamedAreaManager::insert);
    connect(d->cellStorage, &CellStorage::namedAreaRemoved,
            d->workbook->namedAreaManager(), QOverload<const QString &>::of(&NamedAreaManager::remove));
}

Sheet::Sheet(const Sheet &other)
        : KoShapeUserData(other.d->workbook)
        , KoShapeBasedDocumentBase()
        , ProtectableObject(other)
        , SheetBase(other)
        , d(new Private(this))
{
    d->workbook = other.d->workbook;
    d->model = new SheetModel(this);

    // Set a valid object name, so that we can offer scripting.
    setObjectName(createObjectName(sheetName()));

    d->layoutDirection = other.d->layoutDirection;
    d->hide = other.d->hide;
    d->showGrid = other.d->showGrid;
    d->showFormula = other.d->showFormula;
    d->showFormulaIndicator = other.d->showFormulaIndicator;
    d->showCommentIndicator = other.d->showCommentIndicator;
    d->lcMode = other.d->lcMode;
    d->showColumnNumber = other.d->showColumnNumber;
    d->hideZero = other.d->hideZero;

    d->cellStorage = new CellStorage(*other.d->cellStorage, this);
    setCellStorage(d->cellStorage);

    d->rows = other.d->rows;
    d->columns = other.d->columns;

    // flake
#if 0 // CALLIGRA_SHEETS_WIP_COPY_SHEET_(SHAPES)
    //FIXME This does not work as copySettings does not work. Also createDefaultShapeAndInit without the correct settings can not work
    //I think this should use ODF load/save for copying
    KoShape* shape;
    const QList<KoShape*> shapes = other.d->shapes;
    for (int i = 0; i < shapes.count(); ++i) {
        KoShapeFactoryBase *factory = KoShapeRegistry::instance()->value(shapes[i]->shapeId());
        if (factory) {
            shape = factory->createDefaultShapeAndInit(0);
            shape->copySettings(shapes[i]);
            addShape(shape);
        }
    }
#endif // CALLIGRA_SHEETS_WIP_COPY_SHEET_(SHAPES)

    d->print = new SheetPrint(this); // FIXME = new SheetPrint(*other.d->print);

    d->showPageOutline = other.d->showPageOutline;
    d->documentSize = other.d->documentSize;
}

Sheet::~Sheet()
{
    //Disable automatic recalculation of dependencies on this sheet to prevent crashes
    //in certain situations:
    //
    //For example, suppose a cell in SheetB depends upon a cell in SheetA.  If the cell in SheetB is emptied
    //after SheetA has already been deleted, the program would try to remove dependencies from the cell in SheetA
    //causing a crash.
    setAutoCalculationEnabled(false);

    // cellStorage will be deleted in the parent class destructor, and we must not do it here.
    d->cellStorage = nullptr;

    delete d->print;
    qDeleteAll(d->shapes);
    delete d;
}

QAbstractItemModel* Sheet::model() const
{
    return d->model;
}

Map* Sheet::fullMap() const
{
    return d->workbook;
}

DocBase* Sheet::doc() const
{
    return d->workbook->doc();
}

void Sheet::addShape(KoShape* shape)
{
    if (!shape)
        return;
    d->shapes.append(shape);
    shape->setApplicationData(new ShapeApplicationData());
    emit shapeAdded(this, shape);
}

void Sheet::removeShape(KoShape* shape)
{
    if (!shape)
        return;
    d->shapes.removeAll(shape);
    emit shapeRemoved(this, shape);
}

void Sheet::deleteShapes()
{
    qDeleteAll(d->shapes);
    d->shapes.clear();
}

KoDocumentResourceManager* Sheet::resourceManager() const
{
    return map()->resourceManager();
}

QList<KoShape*> Sheet::shapes() const
{
    return d->shapes;
}

Qt::LayoutDirection Sheet::layoutDirection() const
{
    return d->layoutDirection;
}

void Sheet::setLayoutDirection(Qt::LayoutDirection dir)
{
    d->layoutDirection = dir;
}

bool Sheet::isHidden() const
{
    return d->hide;
}

void Sheet::setHidden(bool hidden)
{
    d->hide = hidden;
}

bool Sheet::getShowGrid() const
{
    return d->showGrid;
}

void Sheet::setShowGrid(bool _showGrid)
{
    d->showGrid = _showGrid;
}

bool Sheet::getShowFormula() const
{
    return d->showFormula;
}

void Sheet::setShowFormula(bool _showFormula)
{
    d->showFormula = _showFormula;
}

bool Sheet::getShowFormulaIndicator() const
{
    return d->showFormulaIndicator;
}

void Sheet::setShowFormulaIndicator(bool _showFormulaIndicator)
{
    d->showFormulaIndicator = _showFormulaIndicator;
}

bool Sheet::getShowCommentIndicator() const
{
    return d->showCommentIndicator;
}

void Sheet::setShowCommentIndicator(bool _indic)
{
    d->showCommentIndicator = _indic;
}

bool Sheet::getLcMode() const
{
    return d->lcMode;
}

void Sheet::setLcMode(bool _lcMode)
{
    d->lcMode = _lcMode;
}

bool Sheet::getShowColumnNumber() const
{
    return d->showColumnNumber;
}

void Sheet::setShowColumnNumber(bool _showColumnNumber)
{
    d->showColumnNumber = _showColumnNumber;
}

bool Sheet::getHideZero() const
{
    return d->hideZero;
}

void Sheet::setHideZero(bool _hideZero)
{
    d->hideZero = _hideZero;
}

bool Sheet::isShowPageOutline() const
{
    return d->showPageOutline;
}

const ColumnFormat* Sheet::columnFormat(int _column) const
{
    const ColumnFormat *p = d->columns.lookup(_column);
    if (p != 0)
        return p;

    return map()->defaultColumnFormat();
}

// needed in loading code
ColumnFormat *Sheet::nextColumn(int col) const
{
    return d->columns.next(col);
}

CellStorage* Sheet::fullCellStorage() const
{
    return d->cellStorage;
}

const CommentStorage* Sheet::commentStorage() const
{
    return d->cellStorage->commentStorage();
}

const ConditionsStorage* Sheet::conditionsStorage() const
{
    return d->cellStorage->conditionsStorage();
}

const FusionStorage* Sheet::fusionStorage() const
{
    return d->cellStorage->fusionStorage();
}

const LinkStorage* Sheet::linkStorage() const
{
    return d->cellStorage->linkStorage();
}

const StyleStorage* Sheet::styleStorage() const
{
    return d->cellStorage->styleStorage();
}

SheetPrint* Sheet::print() const
{
    return d->print;
}

PrintSettings* Sheet::printSettings() const
{
    return d->print->settings();
}

void Sheet::setPrintSettings(const PrintSettings &settings)
{
    d->print->setSettings(settings);
    // Repaint, if page borders are shown and this is the active sheet.
    if (isShowPageOutline()) {
        // Just repaint everything visible; no need to invalidate the visual cache.
        map()->addDamage(new SheetDamage(this, SheetDamage::ContentChanged));
    }
}

HeaderFooter *Sheet::headerFooter() const
{
    return d->print->headerFooter();
}

QSizeF Sheet::documentSize() const
{
    return d->documentSize;
}

void Sheet::adjustDocumentWidth(double deltaWidth)
{
    d->documentSize.rwidth() += deltaWidth;
    emit documentSizeChanged(d->documentSize);
}

void Sheet::adjustDocumentHeight(double deltaHeight)
{
    d->documentSize.rheight() += deltaHeight;
    emit documentSizeChanged(d->documentSize);
}

void Sheet::adjustCellAnchoredShapesX(qreal minX, qreal maxX, qreal delta)
{
    foreach (KoShape* s, d->shapes) {
        if (dynamic_cast<ShapeApplicationData*>(s->applicationData())->isAnchoredToCell()) {
            if (s->position().x() >= minX && s->position().x() < maxX) {
                QPointF p = s->position();
                p.setX(qMax(minX, p.x() + delta));
                s->setPosition(p);
            }
        }
    }
}

void Sheet::adjustCellAnchoredShapesX(qreal delta, int firstCol, int lastCol)
{
    adjustCellAnchoredShapesX(columnPosition(firstCol), columnPosition(lastCol+1), delta);
}

void Sheet::adjustCellAnchoredShapesY(qreal minY, qreal maxY, qreal delta)
{
    foreach (KoShape* s, d->shapes) {
        if (dynamic_cast<ShapeApplicationData*>(s->applicationData())->isAnchoredToCell()) {
            if (s->position().y() >= minY && s->position().y() < maxY) {
                QPointF p = s->position();
                p.setY(qMax(minY, p.y() + delta));
                s->setPosition(p);
            }
        }
    }
}

void Sheet::adjustCellAnchoredShapesY(qreal delta, int firstRow, int lastRow)
{
    adjustCellAnchoredShapesY(rowPosition(firstRow), rowPosition(lastRow+1), delta);
}

int Sheet::leftColumn(qreal _xpos, qreal &_left) const
{
    _left = 0.0;
    int col = 1;
    double x = columnFormat(col)->visibleWidth();
    while (x < _xpos && col < KS_colMax) {
        _left += columnFormat(col)->visibleWidth();
        x += columnFormat(++col)->visibleWidth();
    }
    return col;
}

int Sheet::rightColumn(double _xpos) const
{
    int col = 1;
    double x = columnFormat(col)->visibleWidth();
    while (x <= _xpos && col < KS_colMax)
        x += columnFormat(++col)->visibleWidth();
    return col;
}

int Sheet::topRow(qreal _ypos, qreal & _top) const
{
    qreal top;
    int row = rowFormats()->rowForPosition(_ypos, &top);
    _top = top;
    return row;
}

int Sheet::bottomRow(double _ypos) const
{
    return rowFormats()->rowForPosition(_ypos+1e-9);
}

QRectF Sheet::cellCoordinatesToDocument(const QRect& cellRange) const
{
    // TODO Stefan: Rewrite to save some iterations over the columns/rows.
    QRectF rect;
    rect.setLeft(columnPosition(cellRange.left()));
    rect.setRight(columnPosition(cellRange.right()) + columnFormat(cellRange.right())->width());
    rect.setTop(rowPosition(cellRange.top()));
    rect.setBottom(rowPosition(cellRange.bottom()) + rowFormats()->rowHeight(cellRange.bottom()));
    return rect;
}

QRect Sheet::documentToCellCoordinates(const QRectF &area) const
{
    double width = 0.0;
    int left = 0;
    while (width <= area.left())
        width += columnFormat(++left)->visibleWidth();
    int right = left;
    while (width < area.right())
        width += columnFormat(++right)->visibleWidth();
    int top = rowFormats()->rowForPosition(area.top());
    int bottom = rowFormats()->rowForPosition(area.bottom());
    return QRect(left, top, right - left + 1, bottom - top + 1);
}

double Sheet::columnPosition(int _col) const
{
    const int max = qMin(_col, KS_colMax);
    double x = 0.0;
    for (int col = 1; col < max; ++col)
        x += columnFormat(col)->visibleWidth();
    return x;
}


double Sheet::rowPosition(int _row) const
{
    const int max = qMin(_row, KS_rowMax+1);
    return rowFormats()->totalVisibleRowHeight(1, max-1);
}

ColumnFormat* Sheet::firstCol() const
{
    return d->columns.first();
}

ColumnFormat* Sheet::nonDefaultColumnFormat(int _column, bool force_creation)
{
    Q_ASSERT(_column >= 1 && _column <= KS_colMax);
    ColumnFormat *p = d->columns.lookup(_column);
    if (p != 0 || !force_creation)
        return p;

    p = new ColumnFormat(*map()->defaultColumnFormat());
    p->setSheet(this);
    p->setColumn(_column);

    d->columns.insertElement(p, _column);

    return p;
}

void Sheet::insertShiftRight(const QRect& rect)
{
    foreach(Sheet* sheet, map()->sheetList()) {
        for (int i = rect.top(); i <= rect.bottom(); ++i) {
            sheet->changeNameCellRef(QPoint(rect.left(), i), false,
                                     Sheet::ColumnInsert, sheetName(),
                                     rect.right() - rect.left() + 1);
        }
    }
}

void Sheet::insertShiftDown(const QRect& rect)
{
    foreach(Sheet* sheet, map()->sheetList()) {
        for (int i = rect.left(); i <= rect.right(); ++i) {
            sheet->changeNameCellRef(QPoint(i, rect.top()), false,
                                     Sheet::RowInsert, sheetName(),
                                     rect.bottom() - rect.top() + 1);
        }
    }
}

void Sheet::removeShiftUp(const QRect& rect)
{
    foreach(Sheet* sheet, map()->sheetList()) {
        for (int i = rect.left(); i <= rect.right(); ++i) {
            sheet->changeNameCellRef(QPoint(i, rect.top()), false,
                                     Sheet::RowRemove, sheetName(),
                                     rect.bottom() - rect.top() + 1);
        }
    }
}

void Sheet::removeShiftLeft(const QRect& rect)
{
    foreach(Sheet* sheet, map()->sheetList()) {
        for (int i = rect.top(); i <= rect.bottom(); ++i) {
            sheet->changeNameCellRef(QPoint(rect.left(), i), false,
                                     Sheet::ColumnRemove, sheetName(),
                                     rect.right() - rect.left() + 1);
        }
    }
}

void Sheet::insertColumns(int col, int number)
{
    double deltaWidth = 0.0;
    for (int i = 0; i < number; i++) {
        deltaWidth -= columnFormat(KS_colMax)->width();
        d->columns.insertColumn(col);
        deltaWidth += columnFormat(col + i)->width();
    }
    // Adjust document width (plus widths of new columns; minus widths of removed columns).
    adjustDocumentWidth(deltaWidth);

    foreach(Sheet* sheet, map()->sheetList()) {
        sheet->changeNameCellRef(QPoint(col, 1), true,
                                 Sheet::ColumnInsert, sheetName(),
                                 number);
    }
    //update print settings
    d->print->insertColumn(col, number);
}

void Sheet::insertRows(int row, int number)
{
    d->rows.insertRows(row, number);

    foreach(Sheet* sheet, map()->sheetList()) {
        sheet->changeNameCellRef(QPoint(1, row), true,
                                 Sheet::RowInsert, sheetName(),
                                 number);
    }
    //update print settings
    d->print->insertRow(row, number);
}

void Sheet::removeColumns(int col, int number)
{
    double deltaWidth = 0.0;
    for (int i = 0; i < number; ++i) {
        deltaWidth -= columnFormat(col)->width();
        d->columns.removeColumn(col);
        deltaWidth += columnFormat(KS_colMax)->width();
    }
    // Adjust document width (plus widths of new columns; minus widths of removed columns).
    adjustDocumentWidth(deltaWidth);

    foreach(Sheet* sheet, map()->sheetList()) {
        sheet->changeNameCellRef(QPoint(col, 1), true,
                                 Sheet::ColumnRemove, sheetName(),
                                 number);
    }
    //update print settings
    d->print->removeColumn(col, number);
}

void Sheet::removeRows(int row, int number)
{
    d->rows.removeRows(row, number);

    foreach(Sheet* sheet, map()->sheetList()) {
        sheet->changeNameCellRef(QPoint(1, row), true,
                                 Sheet::RowRemove, sheetName(),
                                 number);
    }

    //update print settings
    d->print->removeRow(row, number);
}

QString Sheet::changeNameCellRefHelper(const QPoint& pos, bool fullRowOrColumn, ChangeRef ref,
                                       int nbCol, const QPoint& point, bool isColumnFixed,
                                       bool isRowFixed)
{
    QString newPoint;
    int col = point.x();
    int row = point.y();
    // update column
    if (isColumnFixed)
        newPoint.append('$');
    if (ref == ColumnInsert &&
            col + nbCol <= KS_colMax &&
            col >= pos.x() &&    // Column after the new one : +1
            (fullRowOrColumn || row == pos.y())) {  // All rows or just one
        newPoint += Cell::columnName(col + nbCol);
    } else if (ref == ColumnRemove &&
               col > pos.x() &&    // Column after the deleted one : -1
               (fullRowOrColumn || row == pos.y())) {  // All rows or just one
        newPoint += Cell::columnName(col - nbCol);
    } else
        newPoint += Cell::columnName(col);

    // Update row
    if (isRowFixed)
        newPoint.append('$');
    if (ref == RowInsert &&
            row + nbCol <= KS_rowMax &&
            row >= pos.y() &&   // Row after the new one : +1
            (fullRowOrColumn || col == pos.x())) {  // All columns or just one
        newPoint += QString::number(row + nbCol);
    } else if (ref == RowRemove &&
               row > pos.y() &&   // Row after the deleted one : -1
               (fullRowOrColumn || col == pos.x())) {  // All columns or just one
        newPoint += QString::number(row - nbCol);
    } else
        newPoint += QString::number(row);

    if (((ref == ColumnRemove
            && (col >= pos.x() && col < pos.x() + nbCol) // Column is the deleted one : error
            && (fullRowOrColumn || row == pos.y())) ||
            (ref == RowRemove
             && (row >= pos.y() && row < pos.y() + nbCol) // Row is the deleted one : error
             && (fullRowOrColumn || col == pos.x())) ||
            (ref == ColumnInsert
             && col + nbCol > KS_colMax
             && col >= pos.x()     // Column after the new one : +1
             && (fullRowOrColumn || row == pos.y())) ||
            (ref == RowInsert
             && row + nbCol > KS_rowMax
             && row >= pos.y() // Row after the new one : +1
             && (fullRowOrColumn || col == pos.x())))) {
        newPoint = '#' + i18n("Dependency") + '!';
    }
    return newPoint;
}

QString Sheet::changeNameCellRefHelper(const QPoint& pos, const QRect& rect, bool fullRowOrColumn, ChangeRef ref,
                                       int nbCol, const QPoint& point, bool isColumnFixed,
                                       bool isRowFixed)
{
    const bool isFirstColumn = pos.x() == rect.left();
    const bool isLastColumn = pos.x() == rect.right();
    const bool isFirstRow = pos.y() == rect.top();
    const bool isLastRow = pos.y() == rect.bottom();

    QString newPoint;
    int col = point.x();
    int row = point.y();
    // update column
    if (isColumnFixed)
        newPoint.append('$');
    if (ref == ColumnInsert &&
            col + nbCol <= KS_colMax &&
            col >= pos.x() &&    // Column after the new one : +1
            (fullRowOrColumn || row == pos.y())) {  // All rows or just one
        newPoint += Cell::columnName(col + nbCol);
    } else if (ref == ColumnRemove &&
               (col > pos.x() ||
                (col == pos.x() && isLastColumn)) &&    // Column after the deleted one : -1
               (fullRowOrColumn || row == pos.y())) {  // All rows or just one
        newPoint += Cell::columnName(col - nbCol);
    } else
        newPoint += Cell::columnName(col);

    // Update row
    if (isRowFixed)
        newPoint.append('$');
    if (ref == RowInsert &&
            row + nbCol <= KS_rowMax &&
            row >= pos.y() &&   // Row after the new one : +1
            (fullRowOrColumn || col == pos.x())) {  // All columns or just one
        newPoint += QString::number(row + nbCol);
    } else if (ref == RowRemove &&
               (row > pos.y() ||
                (row == pos.y() && isLastRow)) &&   // Row after the deleted one : -1
               (fullRowOrColumn || col == pos.x())) {  // All columns or just one
        newPoint += QString::number(row - nbCol);
    } else
        newPoint += QString::number(row);

    if (((ref == ColumnRemove
            && col == pos.x() // Column is the deleted one : error
            && (fullRowOrColumn || row == pos.y())
            && (isFirstColumn && isLastColumn)) ||
            (ref == RowRemove
             && row == pos.y() // Row is the deleted one : error
             && (fullRowOrColumn || col == pos.x())
             && (isFirstRow && isLastRow)) ||
            (ref == ColumnInsert
             && col + nbCol > KS_colMax
             && col >= pos.x()     // Column after the new one : +1
             && (fullRowOrColumn || row == pos.y())) ||
            (ref == RowInsert
             && row + nbCol > KS_rowMax
             && row >= pos.y() // Row after the new one : +1
             && (fullRowOrColumn || col == pos.x())))) {
        newPoint = '#' + i18n("Dependency") + '!';
    }
    return newPoint;
}

void Sheet::changeNameCellRef(const QPoint& pos, bool fullRowOrColumn, ChangeRef ref,
                              const QString& tabname, int nbCol)
{
    for (int c = 0; c < formulaStorage()->count(); ++c) {
        QString newText('=');
        const Tokens tokens = formulaStorage()->data(c).tokens();
        for (int t = 0; t < tokens.count(); ++t) {
            const Token token = tokens[t];
            switch (token.type()) {
            case Token::Cell:
            case Token::Range: {
                if (map()->namedAreaManager()->contains(token.text())) {
                    newText.append(token.text()); // simply keep the area name
                    break;
                }
                const Region region(token.text(), map());
                if (!region.isValid() || !region.isContiguous()) {
                    newText.append(token.text());
                    break;
                }
                if (!region.firstSheet() && tabname != sheetName()) {
                    // nothing to do here
                    newText.append(token.text());
                    break;
                }
                // actually only one element in here, but we need extended access to the element
                Region::ConstIterator end(region.constEnd());
                for (Region::ConstIterator it(region.constBegin()); it != end; ++it) {
                    Region::Element* element = (*it);
                    if (element->type() == Region::Element::Point) {
                        if (element->sheet())
                            newText.append(element->sheet()->sheetName() + '!');
                        QString newPoint = changeNameCellRefHelper(pos, fullRowOrColumn, ref,
                                           nbCol,
                                           element->rect().topLeft(),
                                           element->isColumnFixed(),
                                           element->isRowFixed());
                        newText.append(newPoint);
                    } else { // (element->type() == Region::Element::Range)
                        if (element->sheet())
                            newText.append(element->sheet()->sheetName() + '!');
                        QString newPoint;
                        newPoint = changeNameCellRefHelper(pos, element->rect(), fullRowOrColumn, ref,
                                                           nbCol, element->rect().topLeft(),
                                                           element->isColumnFixed(),
                                                           element->isRowFixed());
                        newText.append(newPoint + ':');
                        newPoint = changeNameCellRefHelper(pos, element->rect(), fullRowOrColumn, ref,
                                                           nbCol, element->rect().bottomRight(),
                                                           element->isColumnFixed(),
                                                           element->isRowFixed());
                        newText.append(newPoint);
                    }
                }
                break;
            }
            default: {
                newText.append(token.text());
                break;
            }
            }
        }

        Cell cell(this, formulaStorage()->col(c), formulaStorage()->row(c));
        Formula formula(this, cell);
        formula.setExpression(newText);
        cell.setFormula(formula);
    }
}

// helper function for Sheet::areaIsEmpty
bool Sheet::cellIsEmpty(const Cell& cell, TestType _type)
{
    if (!cell.isPartOfMerged()) {
        switch (_type) {
        case Text :
            if (!cell.userInput().isEmpty())
                return false;
            break;
        case Validity:
            if (!cell.validity().isEmpty())
                return false;
            break;
        case Comment:
            if (!cell.comment().isEmpty())
                return false;
            break;
        case ConditionalCellAttribute:
            if (cell.conditions().conditionList().count() > 0)
                return false;
            break;
        }
    }
    return true;
}

// TODO: convert into a manipulator, similar to the Dilation one
bool Sheet::areaIsEmpty(const Region& region, TestType _type)
{
    CellStorage *storage = cellStorage();
    Region::ConstIterator endOfList = region.constEnd();
    for (Region::ConstIterator it = region.constBegin(); it != endOfList; ++it) {
        QRect range = (*it)->rect();
        // Complete rows selected ?
        if ((*it)->isRow()) {
            for (int row = range.top(); row <= range.bottom(); ++row) {
                Cell cell = storage->firstInRow(row);
                while (!cell.isNull()) {
                    if (!cellIsEmpty(cell, _type))
                        return false;
                    cell = storage->nextInRow(cell.column(), row);
                }
            }
        }
        // Complete columns selected ?
        else if ((*it)->isColumn()) {
            for (int col = range.left(); col <= range.right(); ++col) {
                Cell cell = storage->firstInColumn(col);
                while (!cell.isNull()) {
                    if (!cellIsEmpty(cell, _type))
                        return false;
                    cell = storage->nextInColumn(col, cell.row());
                }
            }
        } else {
            Cell cell;
            int right  = range.right();
            int bottom = range.bottom();
            for (int x = range.left(); x <= right; ++x)
                for (int y = range.top(); y <= bottom; ++y) {
                    cell = Cell(this, x, y);
                    if (!cellIsEmpty(cell, _type))
                        return false;
                }
        }
    }
    return true;
}

bool Sheet::onValidationFailed(Validity::Action action, const CellBase *cell, const QString &message, const QString &title) const
{
    showStatusMessage(i18n("Validation for cell %1 failed", cell->fullName()));

    switch (action) {
        case Validity::Stop:
            KMessageBox::error((QWidget*)0, message, title);
            break;
        case Validity::Warning:
            if (KMessageBox::warningYesNo((QWidget*)0, message, title) == KMessageBox::Yes) {
                return true;
            }
            break;
        case Validity::Information:
            KMessageBox::information((QWidget*)0, message, title);
            return true;
            break;
    }
    return false;
}



QRect Sheet::usedArea(bool onlyContent) const
{
    int maxCols = d->cellStorage->columns(!onlyContent);
    int maxRows = d->cellStorage->rows(!onlyContent);

    if (!onlyContent) {
        maxRows = qMax(maxRows, d->rows.lastNonDefaultRow());

        const ColumnFormat* col = firstCol();
        while (col) {
            if (col->column() > maxCols)
                maxCols = col->column();

            col = col->next();
        }
    }

    // flake
    QRectF shapesBoundingRect;
    for (int i = 0; i < d->shapes.count(); ++i)
        shapesBoundingRect |= d->shapes[i]->boundingRect();
    const QRect shapesCellRange = documentToCellCoordinates(shapesBoundingRect);
    maxCols = qMax(maxCols, shapesCellRange.right());
    maxRows = qMax(maxRows, shapesCellRange.bottom());

    return QRect(1, 1, maxCols, maxRows);
}


void Sheet::setShowPageOutline(bool b)
{
    if (b == d->showPageOutline)
        return;

    d->showPageOutline = b;
    // Just repaint everything visible; no need to invalidate the visual cache.
    if (!map()->isLoading()) {
        map()->addDamage(new SheetDamage(this, SheetDamage::ContentChanged));
    }
}

QImage Sheet::backgroundImage() const
{
    return d->backgroundImage;
}

void Sheet::setBackgroundImage(const QImage& image)
{
    d->backgroundImage = image;
}

Sheet::BackgroundImageProperties Sheet::backgroundImageProperties() const
{
    return d->backgroundProperties;
}

void Sheet::setBackgroundImageProperties(const Sheet::BackgroundImageProperties& properties)
{
    d->backgroundProperties = properties;
}

void Sheet::insertColumnFormat(ColumnFormat *l)
{
    d->columns.insertElement(l, l->column());
    if (!map()->isLoading()) {
        map()->addDamage(new SheetDamage(this, SheetDamage::ColumnsChanged));
    }
}

void Sheet::insertRowFormat(RowFormat *l)
{
    const int row = l->row();
    d->rows.setRowHeight(row, row, l->height());
    d->rows.setHidden(row, row, l->isHidden());
    d->rows.setFiltered(row, row, l->isFiltered());
    d->rows.setPageBreak(row, row, l->hasPageBreak());
    if (!map()->isLoading()) {
        map()->addDamage(new SheetDamage(this, SheetDamage::RowsChanged));
    }
}

void Sheet::deleteColumnFormat(int column)
{
    d->columns.removeElement(column);
    if (!map()->isLoading()) {
        map()->addDamage(new SheetDamage(this, SheetDamage::ColumnsChanged));
    }
}

void Sheet::deleteRowFormat(int row)
{
    d->rows.setDefault(row, row);
    if (!map()->isLoading()) {
        map()->addDamage(new SheetDamage(this, SheetDamage::RowsChanged));
    }
}


RowFormatStorage* Sheet::rowFormats()
{
    return &d->rows;
}

const RowFormatStorage* Sheet::rowFormats() const
{
    return &d->rows;
}

void Sheet::showStatusMessage(const QString &message, int timeout) const
{
    emit statusMessage(message, timeout);
}

void Sheet::hideSheet(bool _hide)
{
    setHidden(_hide);
    if (_hide)
        map()->addDamage(new SheetDamage(this, SheetDamage::Hidden));
    else
        map()->addDamage(new SheetDamage(this, SheetDamage::Shown));
}

bool Sheet::setSheetName(const QString& name)
{
    if (isProtected())
        return false;

    QString old_name = sheetName();
    if (old_name == name)
        return true;

    if (!SheetBase::setSheetName(name)) return false;

    setObjectName(name);

    emit nameChanged(old_name, name);

    return true;
}


void Sheet::updateLocale()
{
    for (int c = 0; c < valueStorage()->count(); ++c) {
        Cell cell(this, valueStorage()->col(c), valueStorage()->row(c));
        QString text = cell.userInput();
        cell.parseUserInput(text);
    }
    // Affects the displayed value; rebuild the visual cache.
    const Region region(1, 1, KS_colMax, KS_rowMax, this);
    map()->addDamage(new CellDamage(this, region, CellDamage::Appearance));
}

void Sheet::applyDatabaseFilter(const Database &database)
{
    Sheet* const sheet = database.range().lastSheet();
    const QRect range = database.range().lastRange();
    const int start = database.orientation() == Qt::Vertical ? range.top() : range.left();
    const int end = database.orientation() == Qt::Vertical ? range.bottom() : range.right();
    for (int i = start + 1; i <= end; ++i) {
        const bool isFiltered = !database.filter().evaluate(database, i);
//         debugSheets <<"Filtering column/row" << i <<"?" << isFiltered;
        if (database.orientation() == Qt::Vertical) {
            sheet->rowFormats()->setFiltered(i, i, isFiltered);
        } else { // database.orientation() == Qt::Horizontal
            sheet->nonDefaultColumnFormat(i)->setFiltered(isFiltered);
        }
    }
    if (database.orientation() == Qt::Vertical)
        sheet->map()->addDamage(new SheetDamage(sheet, SheetDamage::RowsChanged));
    else // database.orientation() == Qt::Horizontal
        sheet->map()->addDamage(new SheetDamage(sheet, SheetDamage::ColumnsChanged));

    cellStorage()->setDatabase(database.range(), Database());
    cellStorage()->setDatabase(database.range(), database);
    map()->addDamage(new CellDamage(this, database.range(), CellDamage::Appearance));
}

int Sheet::canvasOffsetX() const
{
    return d->canvasOffsetX;
}

void Sheet::setCanvasOffsetX(int value)
{
    d->canvasOffsetX = value;
}

int Sheet::canvasOffsetY() const
{
    return d->canvasOffsetY;
}

void Sheet::setCanvasOffsetY(int value)
{
    qInfo()<<Q_FUNC_INFO<<sheetName()<<d->canvasOffsetY<<':'<<value;
    d->canvasOffsetY = value;
}

/**********************
 * Printout Functions *
 **********************/

#ifndef NDEBUG
void Sheet::printDebug()
{
    int iMaxColumn = d->cellStorage->columns();
    int iMaxRow = d->cellStorage->rows();

    debugSheets << "Cell | Content | Value  [UserInput]";
    Cell cell;
    for (int currentrow = 1 ; currentrow <= iMaxRow ; ++currentrow) {
        for (int currentcolumn = 1 ; currentcolumn <= iMaxColumn ; currentcolumn++) {
            cell = Cell(this, currentcolumn, currentrow);
            if (!cell.isEmpty()) {
                QString cellDescr = Cell::name(currentcolumn, currentrow).rightJustified(4) +
                //QString cellDescr = "Cell ";
                //cellDescr += QString::number(currentrow).rightJustified(3,'0') + ',';
                //cellDescr += QString::number(currentcolumn).rightJustified(3,'0') + ' ';
                    " | ";
                QString valueType;
                QTextStream stream(&valueType);
                stream << cell.value().type();
                cellDescr += valueType.rightJustified(7) +
                             " | " +
                             map()->converter()->asString(cell.value()).asString().rightJustified(5) +
                             QString("  [%1]").arg(cell.userInput());
                debugSheets << cellDescr;
            }
        }
    }
}
#endif

} // namespace Sheets
} // namespace Calligra
