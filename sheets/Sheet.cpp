/* This file is part of the KDE project
   Copyright 2010 Marijn Kruisselbrink <mkruisselbrink@kde.org>
   Copyright 2007 Stefan Nikolaus <stefan.nikolaus@kdemail.net>
   Copyright 1998,1999 Torben Weis <weis@kde.org>
   Copyright 1999-2007 The KSpread Team <calligra-devel@kde.org>

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
#include "Sheet.h"

#include <QApplication>

#include <kcodecs.h>

#include <KoShape.h>
#include <KoDocumentResourceManager.h>
#include <KoUnit.h>
#include <KoCanvasResourceIdentities.h>

#include "CellStorage.h"
#include "Cluster.h"
#include "Damages.h"
#include "DependencyManager.h"
#include "DocBase.h"
#include "FormulaStorage.h"
#include "HeaderFooter.h"
#include "LoadingInfo.h"
#include "Map.h"
#include "NamedAreaManager.h"
#include "PrintSettings.h"
#include "RecalcManager.h"
#include "RowColumnFormat.h"
#include "RowFormatStorage.h"
#include "ShapeApplicationData.h"
#include "SheetPrint.h"
#include "SheetModel.h"
#include "StyleStorage.h"
#include "Validity.h"
#include "ValueConverter.h"
#include "ValueStorage.h"
#include "database/Filter.h"

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
    Private(Sheet* sheet) : rows(sheet) {}

    Map* workbook;
    SheetModel *model;

    QString name;

    Qt::LayoutDirection layoutDirection;

    // true if sheet is hidden
    bool hide;

    bool showGrid;
    bool showFormula;
    bool showFormulaIndicator;
    bool showCommentIndicator;
    bool autoCalc;
    bool lcMode;
    bool showColumnNumber;
    bool hideZero;
    bool firstLetterUpper;

    // clusters to hold objects
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
};


Sheet::Sheet(Map* map, const QString &sheetName)
        : KoShapeUserData(map)
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

    d->name = sheetName;

    // Set a valid object name, so that we can offer scripting.
    setObjectName(createObjectName(d->name));

    d->cellStorage = new CellStorage(this);
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
    d->firstLetterUpper = false;
    d->autoCalc = true;
    d->print = new SheetPrint(this);

    // document size changes always trigger a visible size change
    connect(this, SIGNAL(documentSizeChanged(QSizeF)), SIGNAL(visibleSizeChanged()));
    // CellStorage connections
    connect(d->cellStorage, SIGNAL(insertNamedArea(Region,QString)),
            d->workbook->namedAreaManager(), SLOT(insert(Region,QString)));
    connect(d->cellStorage, SIGNAL(namedAreaRemoved(QString)),
            d->workbook->namedAreaManager(), SLOT(remove(QString)));
}

Sheet::Sheet(const Sheet &other)
        : KoShapeUserData(other.d->workbook)
        , KoShapeBasedDocumentBase()
        , ProtectableObject(other)
        , d(new Private(this))
{
    d->workbook = other.d->workbook;
    d->model = new SheetModel(this);

    // create a unique name
    int i = 1;
    do
        d->name = other.d->name + QString("_%1").arg(i++);
    while (d->workbook->findSheet(d->name));

    // Set a valid object name, so that we can offer scripting.
    setObjectName(createObjectName(d->name));

    d->layoutDirection = other.d->layoutDirection;
    d->hide = other.d->hide;
    d->showGrid = other.d->showGrid;
    d->showFormula = other.d->showFormula;
    d->showFormulaIndicator = other.d->showFormulaIndicator;
    d->showCommentIndicator = other.d->showCommentIndicator;
    d->autoCalc = other.d->autoCalc;
    d->lcMode = other.d->lcMode;
    d->showColumnNumber = other.d->showColumnNumber;
    d->hideZero = other.d->hideZero;
    d->firstLetterUpper = other.d->firstLetterUpper;

    d->cellStorage = new CellStorage(*other.d->cellStorage, this);
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

    delete d->print;
    delete d->cellStorage;
    qDeleteAll(d->shapes);
    delete d;
}

QAbstractItemModel* Sheet::model() const
{
    return d->model;
}

QString Sheet::sheetName() const
{
    return d->name;
}

Map* Sheet::map() const
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

bool Sheet::isAutoCalculationEnabled() const
{
    return d->autoCalc;
}

void Sheet::setAutoCalculationEnabled(bool enable)
{
    //Avoid possible recalculation of dependencies if the auto calc setting hasn't changed
    if (d->autoCalc == enable)
        return;

    d->autoCalc = enable;
    //If enabling automatic calculation, make sure that the dependencies are up-to-date
    if (enable == true) {
        map()->dependencyManager()->addSheet(this);
        map()->recalcManager()->recalcSheet(this);
    } else {
        map()->dependencyManager()->removeSheet(this);
    }
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

bool Sheet::getFirstLetterUpper() const
{
    return d->firstLetterUpper;
}

void Sheet::setFirstLetterUpper(bool _firstUpper)
{
    d->firstLetterUpper = _firstUpper;
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

CellStorage* Sheet::cellStorage() const
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

const FormulaStorage* Sheet::formulaStorage() const
{
    return d->cellStorage->formulaStorage();
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

const ValidityStorage* Sheet::validityStorage() const
{
    return d->cellStorage->validityStorage();
}

const ValueStorage* Sheet::valueStorage() const
{
    return d->cellStorage->valueStorage();
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

void Sheet::changeCellTabName(QString const & old_name, QString const & new_name)
{
    for (int c = 0; c < formulaStorage()->count(); ++c) {
        if (formulaStorage()->data(c).expression().contains(old_name)) {
            int nb = formulaStorage()->data(c).expression().count(old_name + '!');
            QString tmp = old_name + '!';
            int len = tmp.length();
            tmp = formulaStorage()->data(c).expression();

            for (int i = 0; i < nb; ++i) {
                int pos = tmp.indexOf(old_name + '!');
                tmp.replace(pos, len, new_name + '!');
            }
            Cell cell(this, formulaStorage()->col(c), formulaStorage()->row(c));
            Formula formula(this, cell);
            formula.setExpression(tmp);
            cell.setFormula(formula);
            cell.makeFormula();
        }
    }
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
    Region::ConstIterator endOfList = region.constEnd();
    for (Region::ConstIterator it = region.constBegin(); it != endOfList; ++it) {
        QRect range = (*it)->rect();
        // Complete rows selected ?
        if ((*it)->isRow()) {
            for (int row = range.top(); row <= range.bottom(); ++row) {
                Cell cell = d->cellStorage->firstInRow(row);
                while (!cell.isNull()) {
                    if (!cellIsEmpty(cell, _type))
                        return false;
                    cell = d->cellStorage->nextInRow(cell.column(), row);
                }
            }
        }
        // Complete columns selected ?
        else if ((*it)->isColumn()) {
            for (int col = range.left(); col <= range.right(); ++col) {
                Cell cell = d->cellStorage->firstInColumn(col);
                while (!cell.isNull()) {
                    if (!cellIsEmpty(cell, _type))
                        return false;
                    cell = d->cellStorage->nextInColumn(col, cell.row());
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

QDomElement Sheet::saveXML(QDomDocument& dd)
{
    QDomElement sheet = dd.createElement("table");

    // backward compatibility
    QString sheetName;
    for (int i = 0; i < d->name.count(); ++i) {
        if (d->name[i].isLetterOrNumber() || d->name[i] == ' ' || d->name[i] == '.')
            sheetName.append(d->name[i]);
        else
            sheetName.append('_');
    }
    sheet.setAttribute("name", sheetName);

    //Laurent: for oasis format I think that we must use style:direction...
    sheet.setAttribute("layoutDirection", (layoutDirection() == Qt::RightToLeft) ? "rtl" : "ltr");
    sheet.setAttribute("columnnumber", QString::number((int)getShowColumnNumber()));
    sheet.setAttribute("borders", QString::number((int)isShowPageOutline()));
    sheet.setAttribute("hide", QString::number((int)isHidden()));
    sheet.setAttribute("hidezero", QString::number((int)getHideZero()));
    sheet.setAttribute("firstletterupper", QString::number((int)getFirstLetterUpper()));
    sheet.setAttribute("grid", QString::number((int)getShowGrid()));
    sheet.setAttribute("printGrid", QString::number((int)print()->settings()->printGrid()));
    sheet.setAttribute("printCommentIndicator", QString::number((int)print()->settings()->printCommentIndicator()));
    sheet.setAttribute("printFormulaIndicator", QString::number((int)print()->settings()->printFormulaIndicator()));
    sheet.setAttribute("showFormula", QString::number((int)getShowFormula()));
    sheet.setAttribute("showFormulaIndicator", QString::number((int)getShowFormulaIndicator()));
    sheet.setAttribute("showCommentIndicator", QString::number((int)getShowCommentIndicator()));
    sheet.setAttribute("lcmode", QString::number((int)getLcMode()));
    sheet.setAttribute("autoCalc", QString::number((int)isAutoCalculationEnabled()));
    sheet.setAttribute("borders1.2", "1");
    QByteArray pwd;
    password(pwd);
    if (!pwd.isNull()) {
        if (pwd.size() > 0) {
            QByteArray str = KCodecs::base64Encode(pwd);
            sheet.setAttribute("protected", QString(str.data()));
        } else
            sheet.setAttribute("protected", "");
    }

    // paper parameters
    QDomElement paper = dd.createElement("paper");
    paper.setAttribute("format", printSettings()->paperFormatString());
    paper.setAttribute("orientation", printSettings()->orientationString());
    sheet.appendChild(paper);

    QDomElement borders = dd.createElement("borders");
    KoPageLayout pageLayout = print()->settings()->pageLayout();
    borders.setAttribute("left", QString::number(pageLayout.leftMargin));
    borders.setAttribute("top", QString::number(pageLayout.topMargin));
    borders.setAttribute("right", QString::number(pageLayout.rightMargin));
    borders.setAttribute("bottom", QString::number(pageLayout.bottomMargin));
    paper.appendChild(borders);

    QDomElement head = dd.createElement("head");
    paper.appendChild(head);
    if (!print()->headerFooter()->headLeft().isEmpty()) {
        QDomElement left = dd.createElement("left");
        head.appendChild(left);
        left.appendChild(dd.createTextNode(print()->headerFooter()->headLeft()));
    }
    if (!print()->headerFooter()->headMid().isEmpty()) {
        QDomElement center = dd.createElement("center");
        head.appendChild(center);
        center.appendChild(dd.createTextNode(print()->headerFooter()->headMid()));
    }
    if (!print()->headerFooter()->headRight().isEmpty()) {
        QDomElement right = dd.createElement("right");
        head.appendChild(right);
        right.appendChild(dd.createTextNode(print()->headerFooter()->headRight()));
    }
    QDomElement foot = dd.createElement("foot");
    paper.appendChild(foot);
    if (!print()->headerFooter()->footLeft().isEmpty()) {
        QDomElement left = dd.createElement("left");
        foot.appendChild(left);
        left.appendChild(dd.createTextNode(print()->headerFooter()->footLeft()));
    }
    if (!print()->headerFooter()->footMid().isEmpty()) {
        QDomElement center = dd.createElement("center");
        foot.appendChild(center);
        center.appendChild(dd.createTextNode(print()->headerFooter()->footMid()));
    }
    if (!print()->headerFooter()->footRight().isEmpty()) {
        QDomElement right = dd.createElement("right");
        foot.appendChild(right);
        right.appendChild(dd.createTextNode(print()->headerFooter()->footRight()));
    }

    // print range
    QDomElement printrange = dd.createElement("printrange-rect");
    QRect _printRange = printSettings()->printRegion().lastRange();
    int left = _printRange.left();
    int right = _printRange.right();
    int top = _printRange.top();
    int bottom = _printRange.bottom();
    //If whole rows are selected, then we store zeros, as KS_colMax may change in future
    if (left == 1 && right == KS_colMax) {
        left = 0;
        right = 0;
    }
    //If whole columns are selected, then we store zeros, as KS_rowMax may change in future
    if (top == 1 && bottom == KS_rowMax) {
        top = 0;
        bottom = 0;
    }
    printrange.setAttribute("left-rect", QString::number(left));
    printrange.setAttribute("right-rect", QString::number(right));
    printrange.setAttribute("bottom-rect", QString::number(bottom));
    printrange.setAttribute("top-rect", QString::number(top));
    sheet.appendChild(printrange);

    // Print repeat columns
    QDomElement printRepeatColumns = dd.createElement("printrepeatcolumns");
    printRepeatColumns.setAttribute("left", QString::number(printSettings()->repeatedColumns().first));
    printRepeatColumns.setAttribute("right", QString::number(printSettings()->repeatedColumns().second));
    sheet.appendChild(printRepeatColumns);

    // Print repeat rows
    QDomElement printRepeatRows = dd.createElement("printrepeatrows");
    printRepeatRows.setAttribute("top", QString::number(printSettings()->repeatedRows().first));
    printRepeatRows.setAttribute("bottom", QString::number(printSettings()->repeatedRows().second));
    sheet.appendChild(printRepeatRows);

    //Save print zoom
    sheet.setAttribute("printZoom", QString::number(printSettings()->zoom()));

    //Save page limits
    const QSize pageLimits = printSettings()->pageLimits();
    sheet.setAttribute("printPageLimitX", QString::number(pageLimits.width()));
    sheet.setAttribute("printPageLimitY", QString::number(pageLimits.height()));

    // Save all cells.
    const QRect usedArea = this->usedArea();
    for (int row = 1; row <= usedArea.height(); ++row) {
        Cell cell = d->cellStorage->firstInRow(row);
        while (!cell.isNull()) {
            QDomElement e = cell.save(dd);
            if (!e.isNull())
                sheet.appendChild(e);
            cell = d->cellStorage->nextInRow(cell.column(), row);
        }
    }

    // Save all RowFormat objects.
    int styleIndex = styleStorage()->nextRowStyleIndex(0);
    int rowFormatRow = 0, lastRowFormatRow = rowFormats()->lastNonDefaultRow();
    while (styleIndex || rowFormatRow <= lastRowFormatRow) {
        int lastRow;
        bool isDefault = rowFormats()->isDefaultRow(rowFormatRow, &lastRow);
        if (isDefault && styleIndex <= lastRow) {
            RowFormat rowFormat(*map()->defaultRowFormat());
            rowFormat.setSheet(this);
            rowFormat.setRow(styleIndex);
            QDomElement e = rowFormat.save(dd);
            if (e.isNull())
                return QDomElement();
            sheet.appendChild(e);
            styleIndex = styleStorage()->nextRowStyleIndex(styleIndex);
        } else if (!isDefault) {
            RowFormat rowFormat(rowFormats(), rowFormatRow);
            QDomElement e = rowFormat.save(dd);
            if (e.isNull())
                return QDomElement();
            sheet.appendChild(e);
            if (styleIndex == rowFormatRow)
                styleIndex = styleStorage()->nextRowStyleIndex(styleIndex);
        }
        if (isDefault) rowFormatRow = qMin(lastRow+1, styleIndex == 0 ? KS_rowMax : styleIndex);
        else rowFormatRow++;
    }

    // Save all ColumnFormat objects.
    ColumnFormat* columnFormat = firstCol();
    styleIndex = styleStorage()->nextColumnStyleIndex(0);
    while (columnFormat || styleIndex) {
        if (columnFormat && (!styleIndex || columnFormat->column() <= styleIndex)) {
            QDomElement e = columnFormat->save(dd);
            if (e.isNull())
                return QDomElement();
            sheet.appendChild(e);
            if (columnFormat->column() == styleIndex)
                styleIndex = styleStorage()->nextColumnStyleIndex(styleIndex);
            columnFormat = columnFormat->next();
        } else if (styleIndex) {
            ColumnFormat columnFormat(*map()->defaultColumnFormat());
            columnFormat.setSheet(this);
            columnFormat.setColumn(styleIndex);
            QDomElement e = columnFormat.save(dd);
            if (e.isNull())
                return QDomElement();
            sheet.appendChild(e);
            styleIndex = styleStorage()->nextColumnStyleIndex(styleIndex);
        }
    }
#if 0 // CALLIGRA_SHEETS_KOPART_EMBEDDING
    foreach(EmbeddedObject* object, doc()->embeddedObjects()) {
        if (object->sheet() == this) {
            QDomElement e = object->save(dd);

            if (e.isNull())
                return QDomElement();
            sheet.appendChild(e);
        }
    }
#endif // CALLIGRA_SHEETS_KOPART_EMBEDDING
    return sheet;
}

bool Sheet::isLoading()
{
    return map()->isLoading();
}

void Sheet::checkContentDirection(QString const & name)
{
    /* set sheet's direction to RTL if sheet name is an RTL string */
    if ((name.isRightToLeft()))
        setLayoutDirection(Qt::RightToLeft);
    else
        setLayoutDirection(Qt::LeftToRight);
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

bool Sheet::loadXML(const KoXmlElement& sheet)
{
    bool ok = false;
    QString sname = sheetName();
    if (!map()->loadingInfo()->loadTemplate()) {
        sname = sheet.attribute("name");
        if (sname.isEmpty()) {
            doc()->setErrorMessage(i18n("Invalid document. Sheet name is empty."));
            return false;
        }
    }

    bool detectDirection = true;
    QString layoutDir = sheet.attribute("layoutDirection");
    if (!layoutDir.isEmpty()) {
        if (layoutDir == "rtl") {
            detectDirection = false;
            setLayoutDirection(Qt::RightToLeft);
        } else if (layoutDir == "ltr") {
            detectDirection = false;
            setLayoutDirection(Qt::LeftToRight);
        } else
            debugSheets << " Direction not implemented :" << layoutDir;
    }
    if (detectDirection)
        checkContentDirection(sname);

    /* older versions of KSpread allowed all sorts of characters that
    the parser won't actually understand.  Replace these with '_'
    Also, the initial character cannot be a space.
    */
    while (sname[0] == ' ') {
        sname.remove(0, 1);
    }
    for (int i = 0; i < sname.length(); i++) {
        if (!(sname[i].isLetterOrNumber() ||
                sname[i] == ' ' || sname[i] == '.' || sname[i] == '_')) {
            sname[i] = '_';
        }
    }

    // validate sheet name, if it differs from the current one
    if (sname != sheetName()) {
        /* make sure there are no name collisions with the altered name */
        QString testName = sname;
        QString baseName = sname;
        int nameSuffix = 0;

        /* so we don't panic over finding ourself in the following test*/
        sname.clear();
        while (map()->findSheet(testName) != 0) {
            nameSuffix++;
            testName = baseName + '_' + QString::number(nameSuffix);
        }
        sname = testName;

        debugSheets << "Sheet::loadXML: table name =" << sname;
        setObjectName(sname);
        setSheetName(sname, true);
    }

//     (dynamic_cast<SheetIface*>(dcopObject()))->sheetNameHasChanged();

    if (sheet.hasAttribute("grid")) {
        setShowGrid((int)sheet.attribute("grid").toInt(&ok));
        // we just ignore 'ok' - if it didn't work, go on
    }
    if (sheet.hasAttribute("printGrid")) {
        print()->settings()->setPrintGrid((bool)sheet.attribute("printGrid").toInt(&ok));
        // we just ignore 'ok' - if it didn't work, go on
    }
    if (sheet.hasAttribute("printCommentIndicator")) {
        print()->settings()->setPrintCommentIndicator((bool)sheet.attribute("printCommentIndicator").toInt(&ok));
        // we just ignore 'ok' - if it didn't work, go on
    }
    if (sheet.hasAttribute("printFormulaIndicator")) {
        print()->settings()->setPrintFormulaIndicator((bool)sheet.attribute("printFormulaIndicator").toInt(&ok));
        // we just ignore 'ok' - if it didn't work, go on
    }
    if (sheet.hasAttribute("hide")) {
        setHidden((bool)sheet.attribute("hide").toInt(&ok));
        // we just ignore 'ok' - if it didn't work, go on
    }
    if (sheet.hasAttribute("showFormula")) {
        setShowFormula((bool)sheet.attribute("showFormula").toInt(&ok));
        // we just ignore 'ok' - if it didn't work, go on
    }
    //Compatibility with KSpread 1.1.x
    if (sheet.hasAttribute("formular")) {
        setShowFormula((bool)sheet.attribute("formular").toInt(&ok));
        // we just ignore 'ok' - if it didn't work, go on
    }
    if (sheet.hasAttribute("showFormulaIndicator")) {
        setShowFormulaIndicator((bool)sheet.attribute("showFormulaIndicator").toInt(&ok));
        // we just ignore 'ok' - if it didn't work, go on
    }
    if (sheet.hasAttribute("showCommentIndicator")) {
        setShowCommentIndicator((bool)sheet.attribute("showCommentIndicator").toInt(&ok));
        // we just ignore 'ok' - if it didn't work, go on
    }
    if (sheet.hasAttribute("borders")) {
        setShowPageOutline((bool)sheet.attribute("borders").toInt(&ok));
        // we just ignore 'ok' - if it didn't work, go on
    }
    if (sheet.hasAttribute("lcmode")) {
        setLcMode((bool)sheet.attribute("lcmode").toInt(&ok));
        // we just ignore 'ok' - if it didn't work, go on
    }
    if (sheet.hasAttribute("autoCalc")) {
        setAutoCalculationEnabled((bool)sheet.attribute("autoCalc").toInt(&ok));
        // we just ignore 'ok' - if it didn't work, go on
    }
    if (sheet.hasAttribute("columnnumber")) {
        setShowColumnNumber((bool)sheet.attribute("columnnumber").toInt(&ok));
        // we just ignore 'ok' - if it didn't work, go on
    }
    if (sheet.hasAttribute("hidezero")) {
        setHideZero((bool)sheet.attribute("hidezero").toInt(&ok));
        // we just ignore 'ok' - if it didn't work, go on
    }
    if (sheet.hasAttribute("firstletterupper")) {
        setFirstLetterUpper((bool)sheet.attribute("firstletterupper").toInt(&ok));
        // we just ignore 'ok' - if it didn't work, go on
    }

    // Load the paper layout
    KoXmlElement paper = sheet.namedItem("paper").toElement();
    if (!paper.isNull()) {
        KoPageLayout pageLayout;
        pageLayout.format = KoPageFormat::formatFromString(paper.attribute("format"));
        pageLayout.orientation = (paper.attribute("orientation")  == "Portrait")
                                 ? KoPageFormat::Portrait : KoPageFormat::Landscape;

        // <borders>
        KoXmlElement borders = paper.namedItem("borders").toElement();
        if (!borders.isNull()) {
            pageLayout.leftMargin   = MM_TO_POINT(borders.attribute("left").toFloat());
            pageLayout.rightMargin  = MM_TO_POINT(borders.attribute("right").toFloat());
            pageLayout.topMargin    = MM_TO_POINT(borders.attribute("top").toFloat());
            pageLayout.bottomMargin = MM_TO_POINT(borders.attribute("bottom").toFloat());
        }
        print()->settings()->setPageLayout(pageLayout);

        QString hleft, hright, hcenter;
        QString fleft, fright, fcenter;
        // <head>
        KoXmlElement head = paper.namedItem("head").toElement();
        if (!head.isNull()) {
            KoXmlElement left = head.namedItem("left").toElement();
            if (!left.isNull())
                hleft = left.text();
            KoXmlElement center = head.namedItem("center").toElement();
            if (!center.isNull())
                hcenter = center.text();
            KoXmlElement right = head.namedItem("right").toElement();
            if (!right.isNull())
                hright = right.text();
        }
        // <foot>
        KoXmlElement foot = paper.namedItem("foot").toElement();
        if (!foot.isNull()) {
            KoXmlElement left = foot.namedItem("left").toElement();
            if (!left.isNull())
                fleft = left.text();
            KoXmlElement center = foot.namedItem("center").toElement();
            if (!center.isNull())
                fcenter = center.text();
            KoXmlElement right = foot.namedItem("right").toElement();
            if (!right.isNull())
                fright = right.text();
        }
        print()->headerFooter()->setHeadFootLine(hleft, hcenter, hright, fleft, fcenter, fright);
    }

    // load print range
    KoXmlElement printrange = sheet.namedItem("printrange-rect").toElement();
    if (!printrange.isNull()) {
        int left = printrange.attribute("left-rect").toInt();
        int right = printrange.attribute("right-rect").toInt();
        int bottom = printrange.attribute("bottom-rect").toInt();
        int top = printrange.attribute("top-rect").toInt();
        if (left == 0) { //whole row(s) selected
            left = 1;
            right = KS_colMax;
        }
        if (top == 0) { //whole column(s) selected
            top = 1;
            bottom = KS_rowMax;
        }
        const Region region(QRect(QPoint(left, top), QPoint(right, bottom)), this);
        printSettings()->setPrintRegion(region);
    }

    // load print zoom
    if (sheet.hasAttribute("printZoom")) {
        double zoom = sheet.attribute("printZoom").toDouble(&ok);
        if (ok) {
            printSettings()->setZoom(zoom);
        }
    }

    // load page limits
    if (sheet.hasAttribute("printPageLimitX")) {
        int pageLimit = sheet.attribute("printPageLimitX").toInt(&ok);
        if (ok) {
            printSettings()->setPageLimits(QSize(pageLimit, 0));
        }
    }

    // load page limits
    if (sheet.hasAttribute("printPageLimitY")) {
        int pageLimit = sheet.attribute("printPageLimitY").toInt(&ok);
        if (ok) {
            const int horizontalLimit = printSettings()->pageLimits().width();
            printSettings()->setPageLimits(QSize(horizontalLimit, pageLimit));
        }
    }

    // Load the cells
    KoXmlNode n = sheet.firstChild();
    while (!n.isNull()) {
        KoXmlElement e = n.toElement();
        if (!e.isNull()) {
            QString tagName = e.tagName();
            if (tagName == "cell")
                Cell(this, 1, 1).load(e, 0, 0); // col, row will get overridden in all cases
            else if (tagName == "row") {
                RowFormat *rl = new RowFormat();
                rl->setSheet(this);
                if (rl->load(e))
                    insertRowFormat(rl);
                delete rl;
            } else if (tagName == "column") {
                ColumnFormat *cl = new ColumnFormat();
                cl->setSheet(this);
                if (cl->load(e))
                    insertColumnFormat(cl);
                else
                    delete cl;
            }
#if 0 // CALLIGRA_SHEETS_KOPART_EMBEDDING
            else if (tagName == "object") {
                EmbeddedCalligraObject *ch = new EmbeddedCalligraObject(doc(), this);
                if (ch->load(e))
                    insertObject(ch);
                else {
                    ch->embeddedObject()->setDeleted(true);
                    delete ch;
                }
            } else if (tagName == "chart") {
                EmbeddedChart *ch = new EmbeddedChart(doc(), this);
                if (ch->load(e))
                    insertObject(ch);
                else {
                    ch->embeddedObject()->setDeleted(true);
                    delete ch;
                }
            }
#endif // CALLIGRA_SHEETS_KOPART_EMBEDDING
        }
        n = n.nextSibling();
    }

    // load print repeat columns
    KoXmlElement printrepeatcolumns = sheet.namedItem("printrepeatcolumns").toElement();
    if (!printrepeatcolumns.isNull()) {
        int left = printrepeatcolumns.attribute("left").toInt();
        int right = printrepeatcolumns.attribute("right").toInt();
        printSettings()->setRepeatedColumns(qMakePair(left, right));
    }

    // load print repeat rows
    KoXmlElement printrepeatrows = sheet.namedItem("printrepeatrows").toElement();
    if (!printrepeatrows.isNull()) {
        int top = printrepeatrows.attribute("top").toInt();
        int bottom = printrepeatrows.attribute("bottom").toInt();
        printSettings()->setRepeatedRows(qMakePair(top, bottom));
    }

    if (!sheet.hasAttribute("borders1.2")) {
        convertObscuringBorders();
    }

    loadXmlProtection(sheet);

    return true;
}


bool Sheet::loadChildren(KoStore* _store)
{
    Q_UNUSED(_store);
#if 0 // CALLIGRA_SHEETS_KOPART_EMBEDDING
    foreach(EmbeddedObject* object, doc()->embeddedObjects()) {
        if (object->sheet() == this && (object->getType() == OBJECT_CALLIGRA_PART || object->getType() == OBJECT_CHART)) {
            debugSheets << "Calligra::Sheets::Sheet::loadChildren";
            if (!dynamic_cast<EmbeddedCalligraObject*>(object)->embeddedObject()->loadDocument(_store))
                return false;
        }
    }
#endif // CALLIGRA_SHEETS_KOPART_EMBEDDING
    return true;
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

void Sheet::showStatusMessage(const QString &message, int timeout)
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

bool Sheet::setSheetName(const QString& name, bool init)
{
    Q_UNUSED(init);
    if (map()->findSheet(name))
        return false;

    if (isProtected())
        return false;

    if (d->name == name)
        return true;

    QString old_name = d->name;
    d->name = name;

    // FIXME: Why is the change of a sheet's name not supposed to be propagated here?
    // If it is not, we have to manually do so in the loading process, e.g. for the
    // SheetAccessModel in the document's data center map.
    //if (init)
    //    return true;

    foreach(Sheet* sheet, map()->sheetList()) {
        sheet->changeCellTabName(old_name, name);
    }

    map()->addDamage(new SheetDamage(this, SheetDamage::Name));

    setObjectName(name);
//     (dynamic_cast<SheetIface*>(dcopObject()))->sheetNameHasChanged();

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

void Sheet::convertObscuringBorders()
{
    // FIXME Stefan: Verify that this is not needed anymore.
#if 0
    /* a word of explanation here:
       beginning with KSpread 1.2 (actually, cvs of Mar 28, 2002), border information
       is stored differently.  Previously, for a cell obscuring a region, the entire
       region's border's data would be stored in the obscuring cell.  This caused
       some data loss in certain situations.  After that date, each cell stores
       its own border data, and prints it even if it is an obscured cell (as long
       as that border isn't across an obscuring border).
       Anyway, this function is used when loading a file that was stored with the
       old way of borders.  All new files have the sheet attribute "borders1.2" so
       if that isn't in the file, all the border data will be converted here.
       It's a bit of a hack but I can't think of a better way and it's not *that*
       bad of a hack.:-)
    */
    Cell c = d->cellStorage->firstCell();
    QPen topPen, bottomPen, leftPen, rightPen;
    for (; c; c = c->nextCell()) {
        if (c->extraXCells() > 0 || c->extraYCells() > 0) {
            const Style* style = this->style(c->column(), c->row());
            topPen = style->topBorderPen();
            leftPen = style->leftBorderPen();
            rightPen = style->rightBorderPen();
            bottomPen = style->bottomBorderPen();

            c->format()->setTopBorderStyle(Qt::NoPen);
            c->format()->setLeftBorderStyle(Qt::NoPen);
            c->format()->setRightBorderStyle(Qt::NoPen);
            c->format()->setBottomBorderStyle(Qt::NoPen);

            for (int x = c->column(); x < c->column() + c->extraXCells(); x++) {
                Cell(this, x, c->row())->setTopBorderPen(topPen);
                Cell(this, x, c->row() + c->extraYCells())->
                setBottomBorderPen(bottomPen);
            }
            for (int y = c->row(); y < c->row() + c->extraYCells(); y++) {
                Cell(this, c->column(), y)->setLeftBorderPen(leftPen);
                Cell(this, c->column() + c->extraXCells(), y)->
                setRightBorderPen(rightPen);
            }
        }
    }
#endif
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
