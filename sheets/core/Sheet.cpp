/* This file is part of the KDE project
   SPDX-FileCopyrightText: 2010 Marijn Kruisselbrink <mkruisselbrink@kde.org>
   SPDX-FileCopyrightText: 2007 Stefan Nikolaus <stefan.nikolaus@kdemail.net>
   SPDX-FileCopyrightText: 1998, 1999 Torben Weis <weis@kde.org>
   SPDX-FileCopyrightText: 1999-2007 The KSpread Team <calligra-devel@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

// Local
#include "Sheet.h"

#include <QApplication>

#include <KLocalizedString>
#include <KMessageBox>

#include <KoShape.h>

#include "engine/Damages.h"
#include "engine/Formula.h"
#include "engine/NamedAreaManager.h"
#include "engine/ValueConverter.h"
#include "engine/ValueStorage.h"

#include "CellStorage.h"
#include "ColFormatStorage.h"
#include "Condition.h"
#include "DataFilter.h"
#include "Database.h"
#include "HeaderFooter.h"
#include "Map.h"
#include "PrintSettings.h"
#include "RowFormatStorage.h"
#include "ShapeApplicationData.h"
#include "SheetModel.h"
#include "SheetPrint.h"

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
    Private(Sheet *sheet)
        : rows(sheet)
        , columns(sheet)
        , canvasOffsetX(0)
        , canvasOffsetY(0)
    {
    }

    // We store this here even though MapBase also has a copy, so that we don't need to recast from MapBase to Map in main app.
    Map *workbook;
    SheetModel *model;

    Qt::LayoutDirection layoutDirection;

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
    CellStorage *cellStorage;
    RowFormatStorage rows;
    ColFormatStorage columns;
    QList<KoShape *> shapes;

    HeaderFooter *headerFooter;
    PrintSettings *printSettings;
    SheetPrint *sheetPrint;

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

Sheet::Sheet(Map *map, const QString &sheetName)
    : KoShapeUserData(map)
    , SheetBase(map, sheetName)
    , KoShapeBasedDocumentBase()
    , d(new Private(this))
{
    d->workbook = map;

    d->layoutDirection = QApplication::layoutDirection();

    // Set a valid object name, so that we can offer scripting.
    setObjectName(createObjectName(sheetName));

    d->cellStorage = new CellStorage(this);
    setCellStorage(d->cellStorage);
    d->model = new SheetModel(this);

    d->documentSize = QSizeF(KS_colMax * d->workbook->defaultColumnFormat().width, KS_rowMax * d->workbook->defaultRowFormat().height);

    d->showGrid = true;
    d->showFormula = false;
    d->showFormulaIndicator = false;
    d->showCommentIndicator = true;
    d->showPageOutline = false;

    d->lcMode = false;
    d->showColumnNumber = false;
    d->hideZero = false;

    d->printSettings = new PrintSettings();
    d->sheetPrint = new SheetPrint(this);
    d->headerFooter = new HeaderFooter(this);

    // document size changes always trigger a visible size change
    connect(this, &Sheet::documentSizeChanged, this, &Sheet::visibleSizeChanged);
    // CellStorage connections
    connect(d->cellStorage, &CellStorage::insertNamedArea, d->workbook->namedAreaManager(), &NamedAreaManager::insert);
    connect(d->cellStorage, &CellStorage::namedAreaRemoved, d->workbook->namedAreaManager(), QOverload<const QString &>::of(&NamedAreaManager::remove));
}

Sheet::Sheet(const Sheet &other)
    : KoShapeUserData(other.d->workbook)
    , SheetBase(other)
    , KoShapeBasedDocumentBase()
    , ProtectableObject(other)
    , d(new Private(this))
{
    d->workbook = other.d->workbook;

    // Set a valid object name, so that we can offer scripting.
    setObjectName(createObjectName(sheetName()));

    d->layoutDirection = other.d->layoutDirection;
    d->showGrid = other.d->showGrid;
    d->showFormula = other.d->showFormula;
    d->showFormulaIndicator = other.d->showFormulaIndicator;
    d->showCommentIndicator = other.d->showCommentIndicator;
    d->lcMode = other.d->lcMode;
    d->showColumnNumber = other.d->showColumnNumber;
    d->hideZero = other.d->hideZero;

    d->cellStorage = new CellStorage(*other.d->cellStorage, this);
    d->headerFooter = new HeaderFooter(*other.d->headerFooter);
    setCellStorage(d->cellStorage);
    d->model = new SheetModel(this);

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

    d->printSettings = new PrintSettings(*(other.d->printSettings));
    d->sheetPrint = new SheetPrint(this);

    d->showPageOutline = other.d->showPageOutline;
    d->documentSize = other.d->documentSize;
}

Sheet::~Sheet()
{
    // Disable automatic recalculation of dependencies on this sheet to prevent crashes
    // in certain situations:
    //
    // For example, suppose a cell in SheetB depends upon a cell in SheetA.  If the cell in SheetB is emptied
    // after SheetA has already been deleted, the program would try to remove dependencies from the cell in SheetA
    // causing a crash.
    setAutoCalculationEnabled(false);

    // cellStorage will be deleted in the parent class destructor, and we must not do it here.
    d->cellStorage = nullptr;

    delete d->model;
    delete d->printSettings;
    delete d->sheetPrint;
    delete d->headerFooter;
    qDeleteAll(d->shapes);
    delete d;
}

Map *Sheet::fullMap() const
{
    return d->workbook;
}

DocBase *Sheet::doc() const
{
    return d->workbook->doc();
}

void Sheet::addShape(KoShape *shape)
{
    if (!shape)
        return;
    d->shapes.append(shape);
    shape->setApplicationData(new ShapeApplicationData());
    emit shapeAdded(this, shape);
}

void Sheet::removeShape(KoShape *shape)
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

QList<KoShape *> Sheet::shapes() const
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

CellStorage *Sheet::fullCellStorage() const
{
    return d->cellStorage;
}

const CommentStorage *Sheet::commentStorage() const
{
    return d->cellStorage->commentStorage();
}

const ConditionsStorage *Sheet::conditionsStorage() const
{
    return d->cellStorage->conditionsStorage();
}

const FusionStorage *Sheet::fusionStorage() const
{
    return d->cellStorage->fusionStorage();
}

const LinkStorage *Sheet::linkStorage() const
{
    return d->cellStorage->linkStorage();
}

const StyleStorage *Sheet::styleStorage() const
{
    return d->cellStorage->styleStorage();
}

PrintSettings *Sheet::printSettings() const
{
    return d->printSettings;
}

SheetPrint *Sheet::print() const
{
    return d->sheetPrint;
}

void Sheet::setPrintSettings(const PrintSettings &settings, bool forcePaint)
{
    *(d->printSettings) = settings;
    d->sheetPrint->setSettings(settings, forcePaint);
    // Repaint, if page borders are shown and this is the active sheet.
    if (isShowPageOutline() || forcePaint) {
        // Just repaint everything visible; no need to invalidate the visual cache.
        map()->addDamage(new SheetDamage(this, SheetDamage::ContentChanged));
    }
}

HeaderFooter *Sheet::headerFooter() const
{
    return d->headerFooter;
}

SheetModel *Sheet::model() const
{
    return d->model;
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

void Sheet::adjustCellAnchoredShapesX(double minX, double maxX, double delta)
{
    for (KoShape *s : d->shapes) {
        if (dynamic_cast<ShapeApplicationData *>(s->applicationData())->isAnchoredToCell()) {
            if (s->position().x() >= minX && s->position().x() < maxX) {
                QPointF p = s->position();
                p.setX(qMax(minX, p.x() + delta));
                s->setPosition(p);
            }
        }
    }
}

void Sheet::adjustCellAnchoredShapesX(double delta, int firstCol, int lastCol)
{
    adjustCellAnchoredShapesX(columnPosition(firstCol), columnPosition(lastCol + 1), delta);
}

void Sheet::adjustCellAnchoredShapesY(double minY, double maxY, double delta)
{
    for (KoShape *s : d->shapes) {
        if (dynamic_cast<ShapeApplicationData *>(s->applicationData())->isAnchoredToCell()) {
            if (s->position().y() >= minY && s->position().y() < maxY) {
                QPointF p = s->position();
                p.setY(qMax(minY, p.y() + delta));
                s->setPosition(p);
            }
        }
    }
}

void Sheet::adjustCellAnchoredShapesY(double delta, int firstRow, int lastRow)
{
    adjustCellAnchoredShapesY(rowPosition(firstRow), rowPosition(lastRow + 1), delta);
}

int Sheet::leftColumn(double _xpos, double &_left) const
{
    double left;
    int col = columnFormats()->colForPosition(_xpos, &left);
    _left = left;
    return col;
}

int Sheet::rightColumn(double _xpos) const
{
    return columnFormats()->colForPosition(_xpos + 1e-9);
}

int Sheet::topRow(double _ypos, double &_top) const
{
    double top;
    int row = rowFormats()->rowForPosition(_ypos, &top);
    _top = top;
    return row;
}

int Sheet::bottomRow(double _ypos) const
{
    return rowFormats()->rowForPosition(_ypos + 1e-9);
}

QRectF Sheet::cellCoordinatesToDocument(const QRect &cellRange) const
{
    // TODO Stefan: Rewrite to save some iterations over the columns/rows.
    QRectF rect;
    rect.setLeft(columnPosition(cellRange.left()));
    rect.setRight(columnPosition(cellRange.right()) + columnFormats()->colWidth(cellRange.right()));
    rect.setTop(rowPosition(cellRange.top()));
    rect.setBottom(rowPosition(cellRange.bottom()) + rowFormats()->rowHeight(cellRange.bottom()));
    return rect;
}

QRect Sheet::documentToCellCoordinates(const QRectF &area) const
{
    int left = columnFormats()->colForPosition(area.left());
    int right = columnFormats()->colForPosition(area.right());
    int top = rowFormats()->rowForPosition(area.top());
    int bottom = rowFormats()->rowForPosition(area.bottom());
    return QRect(left, top, right - left + 1, bottom - top + 1);
}

double Sheet::columnPosition(int _col) const
{
    const int max = qMin(_col, KS_colMax);
    return columnFormats()->totalVisibleColWidth(1, max - 1);
}

double Sheet::rowPosition(int _row) const
{
    const int max = qMin(_row, KS_rowMax + 1);
    return rowFormats()->totalVisibleRowHeight(1, max - 1);
}

void Sheet::insertShiftRight(const QRect &rect)
{
    changeNameCellRefs(rect, SheetBase::ColumnInsert);
}

void Sheet::insertShiftDown(const QRect &rect)
{
    changeNameCellRefs(rect, SheetBase::RowInsert);
}

void Sheet::removeShiftUp(const QRect &rect)
{
    changeNameCellRefs(rect, SheetBase::RowRemove);
}

void Sheet::removeShiftLeft(const QRect &rect)
{
    changeNameCellRefs(rect, SheetBase::ColumnRemove);
}

void Sheet::insertColumns(int col, int number)
{
    d->columns.insertCols(col, number);

    changeNameCellRefs(QRect(QPoint(col, 1), QPoint(col + number - 1, KS_colMax)), Sheet::ColumnInsert);

    emit columnsAdded(col, number);
}

void Sheet::insertRows(int row, int number)
{
    d->rows.insertRows(row, number);

    changeNameCellRefs(QRect(QPoint(1, row), QPoint(KS_rowMax, row + number - 1)), Sheet::RowInsert);

    emit rowsAdded(row, number);
}

void Sheet::removeColumns(int col, int number)
{
    d->columns.removeCols(col, number);

    changeNameCellRefs(QRect(QPoint(col, 1), QPoint(col + number - 1, KS_colMax)), Sheet::ColumnRemove);

    emit columnsRemoved(col, number);
}

void Sheet::removeRows(int row, int number)
{
    d->rows.removeRows(row, number);

    changeNameCellRefs(QRect(QPoint(1, row), QPoint(KS_rowMax, row + number - 1)), Sheet::RowRemove);

    emit rowsRemoved(row, number);
}

// helper function for Sheet::areaIsEmpty
bool Sheet::cellIsEmpty(const Cell &cell, TestType _type)
{
    if (!cell.isPartOfMerged()) {
        switch (_type) {
        case Text:
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
bool Sheet::areaIsEmpty(const Region &region, TestType _type)
{
    CellStorage *storage = fullCellStorage();
    Region::ConstIterator endOfList = region.constEnd();
    QRect used = usedArea();
    for (Region::ConstIterator it = region.constBegin(); it != endOfList; ++it) {
        QRect range = (*it)->rect();
        // Complete rows selected ?
        if ((*it)->isRow()) {
            int from = range.top();
            int to = range.bottom();
            if (from < used.top())
                from = used.top();
            if (to > used.bottom())
                to = used.bottom();
            for (int row = from; row <= to; ++row) {
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
            int from = range.left();
            int to = range.right();
            if (from < used.left())
                from = used.left();
            if (to > used.right())
                to = used.right();
            for (int col = from; col <= to; ++col) {
                Cell cell = storage->firstInColumn(col);
                while (!cell.isNull()) {
                    if (!cellIsEmpty(cell, _type))
                        return false;
                    cell = storage->nextInColumn(col, cell.row());
                }
            }
        } else {
            int right = range.right();
            int bottom = range.bottom();
            if (bottom > used.bottom())
                bottom = used.bottom();
            if (right > used.right())
                right = used.right();
            for (int x = range.left(); x <= right; ++x)
                for (int y = range.top(); y <= bottom; ++y) {
                    Cell cell(this, x, y);
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
        KMessageBox::error(nullptr, message, title);
        break;
    case Validity::Warning:
        if (KMessageBox::warningTwoActions(nullptr, message, title, KStandardGuiItem::ok(), KStandardGuiItem::cancel()) == KMessageBox::PrimaryAction) {
            return true;
        }
        break;
    case Validity::Information:
        KMessageBox::information(nullptr, message, title);
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
        maxCols = qMax(maxCols, d->columns.lastNonDefaultCol());
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

void Sheet::setBackgroundImage(const QImage &image)
{
    d->backgroundImage = image;
}

Sheet::BackgroundImageProperties Sheet::backgroundImageProperties() const
{
    return d->backgroundProperties;
}

void Sheet::setBackgroundImageProperties(const Sheet::BackgroundImageProperties &properties)
{
    d->backgroundProperties = properties;
}

void Sheet::setColumnFormat(int col, const ColFormat &f)
{
    d->columns.setColFormat(col, col, f);
    if (!map()->isLoading()) {
        map()->addDamage(new SheetDamage(this, SheetDamage::ColumnsChanged));
    }
}

void Sheet::setRowFormat(int row, const RowFormat &f)
{
    d->rows.setRowFormat(row, row, f);
    if (!map()->isLoading()) {
        map()->addDamage(new SheetDamage(this, SheetDamage::RowsChanged));
    }
}

void Sheet::clearColumnFormat(int column)
{
    d->columns.setDefault(column, column);
    if (!map()->isLoading()) {
        map()->addDamage(new SheetDamage(this, SheetDamage::ColumnsChanged));
    }
}

void Sheet::clearRowFormat(int row)
{
    d->rows.setDefault(row, row);
    if (!map()->isLoading()) {
        map()->addDamage(new SheetDamage(this, SheetDamage::RowsChanged));
    }
}

RowFormatStorage *Sheet::rowFormats()
{
    return &d->rows;
}

const RowFormatStorage *Sheet::rowFormats() const
{
    return &d->rows;
}

ColFormatStorage *Sheet::columnFormats()
{
    return &d->columns;
}

const ColFormatStorage *Sheet::columnFormats() const
{
    return &d->columns;
}

bool Sheet::rowIsHidden(int row) const
{
    return rowFormats()->isHidden(row);
}

void Sheet::showStatusMessage(const QString &message, int timeout) const
{
    emit statusMessage(message, timeout);
}

bool Sheet::setSheetName(const QString &name)
{
    if (isProtected())
        return false;

    QString old_name = sheetName();
    if (old_name == name)
        return true;

    if (!SheetBase::setSheetName(name))
        return false;

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
    Sheet *const sheet = dynamic_cast<Sheet *>(database.range().lastSheet());
    if (!sheet)
        return;
    const QRect range = database.range().lastRange();
    const int start = database.orientation() == Qt::Vertical ? range.top() : range.left();
    const int end = database.orientation() == Qt::Vertical ? range.bottom() : range.right();
    for (int i = start + 1; i <= end; ++i) {
        const bool isFiltered = !database.filter().evaluate(database, i);
        //         debugSheets <<"Filtering column/row" << i <<"?" << isFiltered;
        if (database.orientation() == Qt::Vertical) {
            sheet->rowFormats()->setFiltered(i, i, isFiltered);
        } else { // database.orientation() == Qt::Horizontal
            sheet->columnFormats()->setFiltered(i, i, isFiltered);
        }
    }
    if (database.orientation() == Qt::Vertical)
        sheet->map()->addDamage(new SheetDamage(sheet, SheetDamage::RowsChanged));
    else // database.orientation() == Qt::Horizontal
        sheet->map()->addDamage(new SheetDamage(sheet, SheetDamage::ColumnsChanged));

    fullCellStorage()->setDatabase(database.range(), Database());
    fullCellStorage()->setDatabase(database.range(), database);
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
    for (int currentrow = 1; currentrow <= iMaxRow; ++currentrow) {
        for (int currentcolumn = 1; currentcolumn <= iMaxColumn; currentcolumn++) {
            cell = Cell(this, currentcolumn, currentrow);
            if (!cell.isEmpty()) {
                QString cellDescr = Cell::name(currentcolumn, currentrow).rightJustified(4) +
                    // QString cellDescr = "Cell ";
                    // cellDescr += QString::number(currentrow).rightJustified(3,'0') + ',';
                    // cellDescr += QString::number(currentcolumn).rightJustified(3,'0') + ' ';
                    " | ";
                QString valueType;
                QTextStream stream(&valueType);
                stream << cell.value().type();
                cellDescr += valueType.rightJustified(7) + " | " + map()->converter()->asString(cell.value()).asString().rightJustified(5)
                    + QString("  [%1]").arg(cell.userInput());
                debugSheets << cellDescr;
            }
        }
    }
}
#endif

} // namespace Sheets
} // namespace Calligra
