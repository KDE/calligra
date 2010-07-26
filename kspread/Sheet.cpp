/* This file is part of the KDE project
   Copyright 2007 Stefan Nikolaus <stefan.nikolaus@kdemail.net>
   Copyright 1998,1999 Torben Weis <weis@kde.org>
   Copyright 1999-2007 The KSpread Team <koffice-devel@kde.org>

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
#include <QBuffer>
#include <QClipboard>
#include <QList>
#include <QMap>
#include <QMimeData>
#include <QStack>
#include <QTextStream>
#include <QImage>

#include <kdebug.h>
#include <kcodecs.h>
#include <kfind.h>
#include <kfinddialog.h>
#include <kreplace.h>
#include <kreplacedialog.h>
#include <kurl.h>

#include <KoDocumentInfo.h>
#include <KoOdfLoadingContext.h>
#include <KoOasisSettings.h>
#include <KoOdfStylesReader.h>

#include <KoShape.h>
#include <KoResourceManager.h>
#include <KoShapeLoadingContext.h>
#include <KoShapeManager.h>
#include <KoShapeRegistry.h>
#include <KoShapeSavingContext.h>
#include <KoStyleStack.h>
#include <KoUnit.h>
#include <KoXmlNS.h>
#include <KoXmlWriter.h>
#include <KoStore.h>
#include <KoText.h>
#include <KoStyleManager.h>
#include <KoTextSharedLoadingData.h>
#include <KoParagraphStyle.h>

#include "CellStorage.h"
#include "Cluster.h"
#include "Condition.h"
#include "Damages.h"
#include "DependencyManager.h"
#include "part/Doc.h" // FIXME detach from part
#include "FormulaStorage.h"
#include "Global.h"
#include "HeaderFooter.h"
#include "LoadingInfo.h"
#include "Localization.h"
#include "Map.h"
#include "NamedAreaManager.h"
#include "OdfLoadingContext.h"
#include "OdfSavingContext.h"
#include "PrintSettings.h"
#include "RecalcManager.h"
#include "RowColumnFormat.h"
#include "ShapeApplicationData.h"
#include "SheetPrint.h"
#include "RectStorage.h"
#include "SheetModel.h"
#include "Style.h"
#include "StyleManager.h"
#include "StyleStorage.h"
#include "Util.h"
#include "Validity.h"
#include "ValueConverter.h"
#include "ValueStorage.h"

// commands
#include "commands/CopyCommand.h"
#include "commands/DataManipulators.h"
#include "commands/DeleteCommand.h"
#include "commands/MergeCommand.h"
#include "commands/RowColumnManipulators.h"
#include "commands/Undo.h"

// D-Bus
#include "interfaces/SheetAdaptor.h"
#include <QtDBus/QtDBus>

namespace KSpread
{

static QString createObjectName(const QString& sheetName)
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


class Sheet::Private
{
public:
    Map* workbook;
    SheetModel *model;

    SheetAdaptor* dbus;

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
    RowCluster rows;
    ColumnCluster columns;
    QList<KoShape*> shapes;

    // hold the print object
    SheetPrint* print;

    // Indicates whether the sheet should paint the page breaks.
    // Doing so costs some time, so by default it should be turned off.
    bool showPageBorders;

    // Max range of canvas in x and y direction.
    //  Depends on KS_colMax/KS_rowMax and the width/height of all columns/rows
    QSizeF documentSize;

    QImage backgroundImage;
    Sheet::BackgroundImageProperties backgroundProperties;
};


Sheet::Sheet(Map* map, const QString& sheetName)
        : KoShapeUserData(map)
        , KoShapeControllerBase()
        , d(new Private)
{
    d->workbook = map;
    if (map->doc()) {
        resourceManager()->setUndoStack(map->doc()->undoStack());
        QVariant variant;
        variant.setValue<void*>(map->doc()->sheetAccessModel());
        resourceManager()->setResource(75751149, variant); // duplicated in kchart.
    }
    d->model = new SheetModel(this);

    d->layoutDirection = QApplication::layoutDirection();

    d->name = sheetName;

    // Set a valid object name, so that we can offer scripting.
    setObjectName(createObjectName(d->name));
    new SheetAdaptor(this);
    QString dbusPath('/' + d->workbook->objectName() + '/' + objectName());
    if (parent()) {
        dbusPath.prepend('/' + parent()->objectName());
    }
    QDBusConnection::sessionBus().registerObject(dbusPath, this);

    d->cellStorage = new CellStorage(this);
    d->rows.setAutoDelete(true);
    d->columns.setAutoDelete(true);

    d->documentSize = QSizeF(KS_colMax * d->workbook->defaultColumnFormat()->width(),
                             KS_rowMax * d->workbook->defaultRowFormat()->height());

    d->hide = false;
    d->showGrid = true;
    d->showFormula = false;
    d->showFormulaIndicator = false;
    d->showCommentIndicator = true;
    d->showPageBorders = false;

    d->lcMode = false;
    d->showColumnNumber = false;
    d->hideZero = false;
    d->firstLetterUpper = false;
    d->autoCalc = true;
    d->print = new SheetPrint(this);

    // document size changes always trigger a visible size change
    connect(this, SIGNAL(documentSizeChanged(const QSizeF&)), SIGNAL(visibleSizeChanged()));
    // CellStorage connections
    connect(d->cellStorage, SIGNAL(insertNamedArea(const Region&, const QString&)),
            d->workbook->namedAreaManager(), SLOT(insert(const Region&, const QString&)));
    connect(d->cellStorage, SIGNAL(namedAreaRemoved(const QString&)),
            d->workbook->namedAreaManager(), SLOT(remove(const QString&)));
}

Sheet::Sheet(const Sheet& other)
        : KoShapeUserData(other.d->workbook)
        , KoShapeControllerBase()
        , ProtectableObject(other)
        , d(new Private)
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
    new SheetAdaptor(this);
    QString dbusPath('/' + d->workbook->objectName() + '/' + objectName());
    if (parent()) {
        dbusPath.prepend('/' + parent()->objectName());
    }
    QDBusConnection::sessionBus().registerObject(dbusPath, this);

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
#if 0 // KSPREAD_WIP_COPY_SHEET_(SHAPES)
    //FIXME This does not work as copySettings does not work. Also createDefaultShapeAndInit without the correct settings can not work
    //I think this should use saveOdf and loadOdf for copying
    KoShape* shape;
    const QList<KoShape*> shapes = other.d->shapes;
    for (int i = 0; i < shapes.count(); ++i) {
        shape = KoShapeRegistry::instance()->value(shapes[i]->shapeId())->createDefaultShapeAndInit(0);
        shape->copySettings(shapes[i]);
        addShape(shape);
    }
#endif // KSPREAD_WIP_COPY_SHEET_(SHAPES)

    d->print = new SheetPrint(this); // FIXME = new SheetPrint(*other.d->print);

    d->showPageBorders = other.d->showPageBorders;
    d->documentSize = other.d->documentSize;
}

Sheet::~Sheet()
{
    //Disable automatic recalculation of dependancies on this sheet to prevent crashes
    //in certain situations:
    //
    //For example, suppose a cell in SheetB depends upon a cell in SheetA.  If the cell in SheetB is emptied
    //after SheetA has already been deleted, the program would try to remove dependancies from the cell in SheetA
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

Doc* Sheet::doc() const
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

KoResourceManager* Sheet::resourceManager() const
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
    //Avoid possible recalculation of dependancies if the auto calc setting hasn't changed
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

bool Sheet::isShowPageBorders() const
{
    return d->showPageBorders;
}

const ColumnFormat* Sheet::columnFormat(int _column) const
{
    const ColumnFormat *p = d->columns.lookup(_column);
    if (p != 0)
        return p;

    return map()->defaultColumnFormat();
}

const RowFormat* Sheet::rowFormat(int _row) const
{
    const RowFormat *p = d->rows.lookup(_row);
    if (p != 0)
        return p;

    return map()->defaultRowFormat();
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

StyleStorage* Sheet::styleStorage() const
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

void Sheet::setPrintSettings(const PrintSettings& settings)
{
    d->print->setSettings(settings);
    // Repaint, if page borders are shown and this is the active sheet.
    if (isShowPageBorders()) {
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

int Sheet::leftColumn(double _xpos, double &_left) const
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

int Sheet::topRow(double _ypos, double & _top) const
{
    _top = 0.0;
    int row = 1;
    double y = rowFormat(row)->visibleHeight();
    while (y < _ypos && row < KS_rowMax) {
        _top += rowFormat(row)->visibleHeight();
        y += rowFormat(++row)->visibleHeight();
    }
    return row;
}

int Sheet::bottomRow(double _ypos) const
{
    int row = 1;
    double y = rowFormat(row)->visibleHeight();
    while (y <= _ypos && row < KS_rowMax)
        y += rowFormat(++row)->visibleHeight();
    return row;
}

QRectF Sheet::cellCoordinatesToDocument(const QRect& cellRange) const
{
    // TODO Stefan: Rewrite to save some iterations over the columns/rows.
    QRectF rect;
    rect.setLeft(columnPosition(cellRange.left()));
    rect.setRight(columnPosition(cellRange.right()) + columnFormat(cellRange.right())->width());
    rect.setTop(rowPosition(cellRange.top()));
    rect.setBottom(rowPosition(cellRange.bottom()) + rowFormat(cellRange.bottom())->height());
    return rect;
}

QRect Sheet::documentToCellCoordinates(const QRectF& area) const
{
    double width = 0.0;
    int left = 0;
    while (width <= area.left())
        width += columnFormat(++left)->visibleWidth();
    int right = left;
    while (width < area.right())
        width += columnFormat(++right)->visibleWidth();
    int top = 0;
    double height = 0.0;
    while (height <= area.top())
        height += rowFormat(++top)->visibleHeight();
    int bottom = top;
    while (height < area.bottom())
        height += rowFormat(++bottom)->visibleHeight();
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
    const int max = qMin(_row, KS_rowMax);
    double y = 0.0;
    for (int row = 1; row < max; ++row)
        y += rowFormat(row)->visibleHeight();
    return y;
}


RowFormat* Sheet::firstRow() const
{
    return d->rows.first();
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

RowFormat* Sheet::nonDefaultRowFormat(int _row, bool force_creation)
{
    Q_ASSERT(_row >= 1 && _row <= KS_rowMax);
    RowFormat *p = d->rows.lookup(_row);
    if (p != 0 || !force_creation)
        return p;

    p = new RowFormat(*map()->defaultRowFormat());
    p->setSheet(this);
    p->setRow(_row);

    d->rows.insertElement(p, _row);

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
    double deltaHeight = 0.0;
    for (int i = 0; i < number; i++) {
        deltaHeight -= rowFormat(KS_rowMax)->height();
        d->rows.insertRow(row);
        deltaHeight += rowFormat(row)->height();
    }
    // Adjust document height (plus heights of new rows; minus heights of removed rows).
    adjustDocumentHeight(deltaHeight);

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
    double deltaHeight = 0.0;
    for (int i = 0; i < number; i++) {
        deltaHeight -= rowFormat(row)->height();
        d->rows.removeRow(row);
        deltaHeight += rowFormat(KS_rowMax)->height();
    }
    // Adjust document height (plus heights of new rows; minus heights of removed rows).
    adjustDocumentHeight(deltaHeight);

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
            && col == pos.x() // Column is the deleted one : error
            && (fullRowOrColumn || row == pos.y())) ||
            (ref == RowRemove
             && row == pos.y() // Row is the deleted one : error
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
                        newPoint = changeNameCellRefHelper(pos, fullRowOrColumn, ref,
                                                           nbCol, element->rect().topLeft(),
                                                           element->isColumnFixed(),
                                                           element->isRowFixed());
                        newText.append(newPoint + ':');
                        newPoint = changeNameCellRefHelper(pos, fullRowOrColumn, ref,
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
    sheet.setAttribute("columnnumber", (int)getShowColumnNumber());
    sheet.setAttribute("borders", (int)isShowPageBorders());
    sheet.setAttribute("hide", (int)isHidden());
    sheet.setAttribute("hidezero", (int)getHideZero());
    sheet.setAttribute("firstletterupper", (int)getFirstLetterUpper());
    sheet.setAttribute("grid", (int)getShowGrid());
    sheet.setAttribute("printGrid", (int)print()->settings()->printGrid());
    sheet.setAttribute("printCommentIndicator", (int)print()->settings()->printCommentIndicator());
    sheet.setAttribute("printFormulaIndicator", (int)print()->settings()->printFormulaIndicator());
    sheet.setAttribute("showFormula", (int)getShowFormula());
    sheet.setAttribute("showFormulaIndicator", (int)getShowFormulaIndicator());
    sheet.setAttribute("showCommentIndicator", (int)getShowCommentIndicator());
    sheet.setAttribute("lcmode", (int)getLcMode());
    sheet.setAttribute("autoCalc", (int)isAutoCalculationEnabled());
    sheet.setAttribute("borders1.2", 1);
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
    borders.setAttribute("left", pageLayout.leftMargin);
    borders.setAttribute("top", pageLayout.topMargin);
    borders.setAttribute("right", pageLayout.rightMargin);
    borders.setAttribute("bottom", pageLayout.bottomMargin);
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
    printrange.setAttribute("left-rect", left);
    printrange.setAttribute("right-rect", right);
    printrange.setAttribute("bottom-rect", bottom);
    printrange.setAttribute("top-rect", top);
    sheet.appendChild(printrange);

    // Print repeat columns
    QDomElement printRepeatColumns = dd.createElement("printrepeatcolumns");
    printRepeatColumns.setAttribute("left", printSettings()->repeatedColumns().first);
    printRepeatColumns.setAttribute("right", printSettings()->repeatedColumns().second);
    sheet.appendChild(printRepeatColumns);

    // Print repeat rows
    QDomElement printRepeatRows = dd.createElement("printrepeatrows");
    printRepeatRows.setAttribute("top", printSettings()->repeatedRows().first);
    printRepeatRows.setAttribute("bottom", printSettings()->repeatedRows().second);
    sheet.appendChild(printRepeatRows);

    //Save print zoom
    sheet.setAttribute("printZoom", printSettings()->zoom());

    //Save page limits
    const QSize pageLimits = printSettings()->pageLimits();
    sheet.setAttribute("printPageLimitX", pageLimits.width());
    sheet.setAttribute("printPageLimitY", pageLimits.height());

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
    RowFormat* rowFormat = firstRow();
    int styleIndex = styleStorage()->nextRowStyleIndex(0);
    while (rowFormat || styleIndex) {
        if (rowFormat && (!styleIndex || rowFormat->row() <= styleIndex)) {
            QDomElement e = rowFormat->save(dd);
            if (e.isNull())
                return QDomElement();
            sheet.appendChild(e);
            if (rowFormat->row() == styleIndex)
                styleIndex = styleStorage()->nextRowStyleIndex(styleIndex);
            rowFormat = rowFormat->next();
        } else if (styleIndex) {
            RowFormat rowFormat(*map()->defaultRowFormat());
            rowFormat.setSheet(this);
            rowFormat.setRow(styleIndex);
            QDomElement e = rowFormat.save(dd);
            if (e.isNull())
                return QDomElement();
            sheet.appendChild(e);
            styleIndex = styleStorage()->nextRowStyleIndex(styleIndex);
        }
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
#if 0 // KSPREAD_KOPART_EMBEDDING
    foreach(EmbeddedObject* object, doc()->embeddedObjects()) {
        if (object->sheet() == this) {
            QDomElement e = object->save(dd);

            if (e.isNull())
                return QDomElement();
            sheet.appendChild(e);
        }
    }
#endif // KSPREAD_KOPART_EMBEDDING
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

bool Sheet::loadSheetStyleFormat(KoXmlElement *style)
{
    QString hleft, hmiddle, hright;
    QString fleft, fmiddle, fright;
    KoXmlNode header = KoXml::namedItemNS(*style, KoXmlNS::style, "header");

    if (!header.isNull()) {
        kDebug(36003) << "Header exists";
        KoXmlNode part = KoXml::namedItemNS(header, KoXmlNS::style, "region-left");
        if (!part.isNull()) {
            hleft = getPart(part);
            kDebug(36003) << "Header left:" << hleft;
        } else
            kDebug(36003) << "Style:region:left doesn't exist!";
        part = KoXml::namedItemNS(header, KoXmlNS::style, "region-center");
        if (!part.isNull()) {
            hmiddle = getPart(part);
            kDebug(36003) << "Header middle:" << hmiddle;
        }
        part = KoXml::namedItemNS(header, KoXmlNS::style, "region-right");
        if (!part.isNull()) {
            hright = getPart(part);
            kDebug(36003) << "Header right:" << hright;
        }
        //If Header doesn't have region tag add it to Left
        hleft.append(getPart(header));
    }
    //TODO implement it under kspread
    KoXmlNode headerleft = KoXml::namedItemNS(*style, KoXmlNS::style, "header-left");
    if (!headerleft.isNull()) {
        KoXmlElement e = headerleft.toElement();
        if (e.hasAttributeNS(KoXmlNS::style, "display"))
            kDebug(36003) << "header.hasAttribute( style:display ) :" << e.hasAttributeNS(KoXmlNS::style, "display");
        else
            kDebug(36003) << "header left doesn't has attribute  style:display";
    }
    //TODO implement it under kspread
    KoXmlNode footerleft = KoXml::namedItemNS(*style, KoXmlNS::style, "footer-left");
    if (!footerleft.isNull()) {
        KoXmlElement e = footerleft.toElement();
        if (e.hasAttributeNS(KoXmlNS::style, "display"))
            kDebug(36003) << "footer.hasAttribute( style:display ) :" << e.hasAttributeNS(KoXmlNS::style, "display");
        else
            kDebug(36003) << "footer left doesn't has attribute  style:display";
    }

    KoXmlNode footer = KoXml::namedItemNS(*style, KoXmlNS::style, "footer");

    if (!footer.isNull()) {
        KoXmlNode part = KoXml::namedItemNS(footer, KoXmlNS::style, "region-left");
        if (!part.isNull()) {
            fleft = getPart(part);
            kDebug(36003) << "Footer left:" << fleft;
        }
        part = KoXml::namedItemNS(footer, KoXmlNS::style, "region-center");
        if (!part.isNull()) {
            fmiddle = getPart(part);
            kDebug(36003) << "Footer middle:" << fmiddle;
        }
        part = KoXml::namedItemNS(footer, KoXmlNS::style, "region-right");
        if (!part.isNull()) {
            fright = getPart(part);
            kDebug(36003) << "Footer right:" << fright;
        }
        //If Footer doesn't have region tag add it to Left
        fleft.append(getPart(footer));
    }

    print()->headerFooter()->setHeadFootLine(hleft, hmiddle, hright,
            fleft, fmiddle, fright);
    return true;
}

void Sheet::replaceMacro(QString & text, const QString & old, const QString & newS)
{
    int n = text.indexOf(old);
    if (n != -1)
        text = text.replace(n, old.length(), newS);
}

QString Sheet::getPart(const KoXmlNode & part)
{
    QString result;
    KoXmlElement e = KoXml::namedItemNS(part, KoXmlNS::text, "p");
    while (!e.isNull()) {
        QString text = e.text();

        KoXmlElement macro = KoXml::namedItemNS(e, KoXmlNS::text, "time");
        if (!macro.isNull())
            replaceMacro(text, macro.text(), "<time>");

        macro = KoXml::namedItemNS(e, KoXmlNS::text, "date");
        if (!macro.isNull())
            replaceMacro(text, macro.text(), "<date>");

        macro = KoXml::namedItemNS(e, KoXmlNS::text, "page-number");
        if (!macro.isNull())
            replaceMacro(text, macro.text(), "<page>");

        macro = KoXml::namedItemNS(e, KoXmlNS::text, "page-count");
        if (!macro.isNull())
            replaceMacro(text, macro.text(), "<pages>");

        macro = KoXml::namedItemNS(e, KoXmlNS::text, "sheet-name");
        if (!macro.isNull())
            replaceMacro(text, macro.text(), "<sheet>");

        macro = KoXml::namedItemNS(e, KoXmlNS::text, "title");
        if (!macro.isNull())
            replaceMacro(text, macro.text(), "<name>");

        macro = KoXml::namedItemNS(e, KoXmlNS::text, "file-name");
        if (!macro.isNull())
            replaceMacro(text, macro.text(), "<file>");

        //add support for multi line into kspread
        if (!result.isEmpty())
            result += '\n';
        result += text;
        e = e.nextSibling().toElement();
    }

    return result;
}

bool Sheet::loadOdf(const KoXmlElement& sheetElement,
                    OdfLoadingContext& tableContext,
                    const Styles& autoStyles,
                    const QHash<QString, Conditions>& conditionalStyles)
{
    KoOdfLoadingContext& odfContext = tableContext.odfContext;
    if (sheetElement.hasAttributeNS(KoXmlNS::table, "style-name")) {
        QString stylename = sheetElement.attributeNS(KoXmlNS::table, "style-name", QString());
        //kDebug(36003)<<" style of table :"<<stylename;
        const KoXmlElement *style = odfContext.stylesReader().findStyle(stylename, "table");
        Q_ASSERT(style);
        //kDebug(36003)<<" style :"<<style;
        if (style) {
            KoXmlElement properties(KoXml::namedItemNS(*style, KoXmlNS::style, "table-properties"));
            if (!properties.isNull()) {
                if (properties.hasAttributeNS(KoXmlNS::table, "display")) {
                    bool visible = (properties.attributeNS(KoXmlNS::table, "display", QString()) == "true" ? true : false);
                    setHidden(!visible);
                }
            }
            if (style->hasAttributeNS(KoXmlNS::style, "master-page-name")) {
                QString masterPageStyleName = style->attributeNS(KoXmlNS::style, "master-page-name", QString());
                //kDebug()<<"style->attribute( style:master-page-name ) :"<<masterPageStyleName;
                KoXmlElement *masterStyle = odfContext.stylesReader().masterPages()[masterPageStyleName];
                //kDebug()<<"stylesReader.styles()[masterPageStyleName] :"<<masterStyle;
                if (masterStyle) {
                    loadSheetStyleFormat(masterStyle);
                    if (masterStyle->hasAttributeNS(KoXmlNS::style, "page-layout-name")) {
                        QString masterPageLayoutStyleName = masterStyle->attributeNS(KoXmlNS::style, "page-layout-name", QString());
                        //kDebug(36003)<<"masterPageLayoutStyleName :"<<masterPageLayoutStyleName;
                        const KoXmlElement *masterLayoutStyle = odfContext.stylesReader().findStyle(masterPageLayoutStyleName);
                        if (masterLayoutStyle) {
                            //kDebug(36003)<<"masterLayoutStyle :"<<masterLayoutStyle;
                            KoStyleStack styleStack;
                            styleStack.setTypeProperties("page-layout");
                            styleStack.push(*masterLayoutStyle);
                            loadOdfMasterLayoutPage(styleStack);
                        }
                    }
                }
            }

            if (style->hasChildNodes() ) {
                KoXmlElement element;
                forEachElement(element, properties) {
                    if (element.nodeName() == "style:background-image") {
                        QString imagePath = element.attributeNS(KoXmlNS::xlink, "href");
                        KoStore* store = tableContext.odfContext.store();
                        if (store->hasFile(imagePath)) {
                            QByteArray data;
                            store->extractFile(imagePath, data);
                            QImage image = QImage::fromData(data);

                            if( image.isNull() ) {
                                continue;
                            }

                            setBackgroundImage(image);

                            BackgroundImageProperties bgProperties;
                            if( element.hasAttribute("draw:opacity") ) {
                                QString opacity = element.attribute("draw:opacity", "");
                                if( opacity.endsWith('%') ) {
                                    opacity = opacity.left(opacity.size() - 2);
                                }
                                bool ok;
                                float opacityFloat = opacity.toFloat( &ok );
                                if( ok ) {
                                    bgProperties.opacity = opacityFloat;
                                }
                            }
                            //TODO
                            //if( element.hasAttribute("style:filterName") ) {
                            //}
                            if( element.hasAttribute("style:position") ) {
                                const QString positionAttribute = element.attribute("style:position","");
                                const QStringList positionList = positionAttribute.split(" ", QString::SkipEmptyParts);
                                if( positionList.size() == 1) {
                                    const QString position = positionList.at(0);
                                    if( position == "left" ) {
                                        bgProperties.horizontalPosition = BackgroundImageProperties::Left;
                                    }
                                    if( position == "center" ) {
                                        //NOTE the standard is too vague to know what center alone means, we assume that it means both centered
                                        bgProperties.horizontalPosition = BackgroundImageProperties::HorizontalCenter;
                                        bgProperties.verticalPosition = BackgroundImageProperties::VerticalCenter;
                                    }
                                    if( position == "right" ) {
                                        bgProperties.horizontalPosition = BackgroundImageProperties::Right;
                                    }
                                    if( position == "top" ) {
                                        bgProperties.verticalPosition = BackgroundImageProperties::Top;
                                    }
                                    if( position == "bottom" ) {
                                        bgProperties.verticalPosition = BackgroundImageProperties::Bottom;
                                    }
                                }
                                else if (positionList.size() == 2) {
                                    const QString verticalPosition = positionList.at(0);
                                    const QString horizontalPosition = positionList.at(1);
                                    if( horizontalPosition == "left" ) {
                                        bgProperties.horizontalPosition = BackgroundImageProperties::Left;
                                    }
                                    if( horizontalPosition == "center" ) {
                                        bgProperties.horizontalPosition = BackgroundImageProperties::HorizontalCenter;
                                    }
                                    if( horizontalPosition == "right" ) {
                                        bgProperties.horizontalPosition = BackgroundImageProperties::Right;
                                    }
                                    if( verticalPosition == "top" ) {
                                        bgProperties.verticalPosition = BackgroundImageProperties::Top;
                                    }
                                    if( verticalPosition == "center" ) {
                                        bgProperties.verticalPosition = BackgroundImageProperties::VerticalCenter;
                                    }
                                    if( verticalPosition == "bottom" ) {
                                        bgProperties.verticalPosition = BackgroundImageProperties::Bottom;
                                    }
                                }
                            }
                            if( element.hasAttribute("style:repeat") ) {
                                const QString repeat = element.attribute("style:repeat");
                                if( repeat == "no-repeat" ) {
                                    bgProperties.repeat = BackgroundImageProperties::NoRepeat;
                                }
                                if( repeat == "repeat" ) {
                                    bgProperties.repeat = BackgroundImageProperties::Repeat;
                                }
                                if( repeat == "stretch" ) {
                                    bgProperties.repeat = BackgroundImageProperties::Stretch;
                                }
                            }
                            setBackgroundImageProperties(bgProperties);
                        }
                    }
                }

            }
        }
    }

    // Cell style regions
    QHash<QString, QRegion> cellStyleRegions;
    // Cell style regions (row defaults)
    QHash<QString, QRegion> rowStyleRegions;
    // Cell style regions (column defaults)
    QHash<QString, QRegion> columnStyleRegions;

    int rowIndex = 1;
    int indexCol = 1;
    int maxColumn = 1;
    KoXmlNode rowNode = sheetElement.firstChild();
    // Some spreadsheet programs may support more rows than
    // KSpread so limit the number of repeated rows.
    // FIXME POSSIBLE DATA LOSS!

    // First load all style information for rows, columns and cells
    while (!rowNode.isNull() && rowIndex <= KS_rowMax) {
        kDebug(36003) << " rowIndex :" << rowIndex << " indexCol :" << indexCol;
        KoXmlElement rowElement = rowNode.toElement();
        if (!rowElement.isNull()) {
            // slightly faster
            KoXml::load(rowElement);

            kDebug(36003) << " Sheet::loadOdf rowElement.tagName() :" << rowElement.localName();
            if (rowElement.namespaceURI() == KoXmlNS::table) {
                if (rowElement.localName() == "table-header-columns") {
                    KoXmlNode headerColumnNode = rowElement.firstChild();
                    while (!headerColumnNode.isNull()) {
                        // NOTE Handle header cols as ordinary ones
                        //      as long as they're not supported.
                        loadColumnFormat(headerColumnNode.toElement(), odfContext.stylesReader(),
                                         indexCol, columnStyleRegions);
                        headerColumnNode = headerColumnNode.nextSibling();
                    }
                } else if (rowElement.localName() == "table-column" && indexCol <= KS_colMax) {
                    kDebug(36003) << " table-column found : index column before" << indexCol;
                    loadColumnFormat(rowElement, odfContext.stylesReader(), indexCol, columnStyleRegions);
                    kDebug(36003) << " table-column found : index column after" << indexCol;
                    maxColumn = qMax(maxColumn, indexCol - 1);
                } else if (rowElement.localName() == "table-header-rows") {
                    KoXmlNode headerRowNode = rowElement.firstChild();
                    while (!headerRowNode.isNull()) {
                        // NOTE Handle header rows as ordinary ones
                        //      as long as they're not supported.
                        int columnMaximal = loadRowFormatStyles(headerRowNode.toElement(), rowIndex,
                                      tableContext, rowStyleRegions,
                                      cellStyleRegions);
                        // allow the row to define more columns then defined via table-column
                        maxColumn = qMax(maxColumn, columnMaximal);
                        headerRowNode = headerRowNode.nextSibling();
                    }
                } else if (rowElement.localName() == "table-row") {
                    kDebug(36003) << " table-row found :index row before" << rowIndex;
                    int columnMaximal = loadRowFormatStyles(rowElement, rowIndex, tableContext,
                                  rowStyleRegions, cellStyleRegions);
                    // allow the row to define more columns then defined via table-column
                    maxColumn = qMax(maxColumn, columnMaximal);
                    kDebug(36003) << " table-row found :index row after" << rowIndex;
                }
            }

            // don't need it anymore
            KoXml::unload(rowElement);
        }

        rowNode = rowNode.nextSibling();
        map()->increaseLoadedRowsCounter();
    }

    // insert the styles into the storage (column defaults)
    kDebug(36003) << "Inserting column default cell styles ...";
    loadOdfInsertStyles(autoStyles, columnStyleRegions, conditionalStyles,
                        QRect(1, 1, maxColumn, rowIndex - 1));
    // insert the styles into the storage (row defaults)
    kDebug(36003) << "Inserting row default cell styles ...";
    loadOdfInsertStyles(autoStyles, rowStyleRegions, conditionalStyles,
                        QRect(1, 1, maxColumn, rowIndex - 1));
    // insert the styles into the storage
    kDebug(36003) << "Inserting cell styles ...";
    loadOdfInsertStyles(autoStyles, cellStyleRegions, conditionalStyles,
                        QRect(1, 1, maxColumn, rowIndex - 1));

    rowIndex = 1;
    indexCol = 1;
    rowNode = sheetElement.firstChild();
    // And secondly also load the actual cell content. Loading cell content requires syle
    // information to be present, so we need this two phase approach
    while (!rowNode.isNull() && rowIndex <= KS_rowMax) {
        kDebug(36003) << " rowIndex :" << rowIndex << " indexCol :" << indexCol;
        KoXmlElement rowElement = rowNode.toElement();
        if (!rowElement.isNull()) {
            // slightly faster
            KoXml::load(rowElement);

            kDebug(36003) << " Sheet::loadOdf rowElement.tagName() :" << rowElement.localName();
            if (rowElement.namespaceURI() == KoXmlNS::table) {
                if (rowElement.localName() == "table-header-rows") {
                    KoXmlNode headerRowNode = rowElement.firstChild();
                    while (!headerRowNode.isNull()) {
                        // NOTE Handle header rows as ordinary ones
                        //      as long as they're not supported.
                        int columnMaximal = loadRowFormatContent(headerRowNode.toElement(), rowIndex,
                                      tableContext);
                        // allow the row to define more columns then defined via table-column
                        maxColumn = qMax(maxColumn, columnMaximal);
                        headerRowNode = headerRowNode.nextSibling();
                    }
                } else if (rowElement.localName() == "table-row") {
                    kDebug(36003) << " table-row found :index row before" << rowIndex;
                    int columnMaximal = loadRowFormatContent(rowElement, rowIndex, tableContext);
                    // allow the row to define more columns then defined via table-column
                    maxColumn = qMax(maxColumn, columnMaximal);
                    kDebug(36003) << " table-row found :index row after" << rowIndex;
                } // the evaluation of the elements is done in during data reading so this does not seam
                  // to have any use, therefore removed
                else if (rowElement.localName() == "shapes") {
                    // OpenDocument v1.1, 8.3.4 Shapes:
                    // The <table:shapes> element contains all graphic shapes
                    // with an anchor on the table this element is a child of.
                    KoShapeLoadingContext* shapeLoadingContext = tableContext.shapeContext;
                    KoXmlElement element;
                    forEachElement(element, rowElement) {
                        if (element.namespaceURI() != KoXmlNS::draw)
                            continue;
                        KoShape* shape = KoShapeRegistry::instance()->createShapeFromOdf(element, *shapeLoadingContext);
                        if (!shape)
                            continue;
                        addShape(shape);
                        dynamic_cast<ShapeApplicationData*>(shape->applicationData())->setAnchoredToCell(false);
                    }
                }
            }

            // don't need it anymore
            KoXml::unload(rowElement);
        }

        rowNode = rowNode.nextSibling();
        map()->increaseLoadedRowsCounter();
    }


    if (sheetElement.hasAttributeNS(KoXmlNS::table, "print-ranges")) {
        // e.g.: Sheet4.A1:Sheet4.E28
        QString range = sheetElement.attributeNS(KoXmlNS::table, "print-ranges", QString());
        Region region(Region::loadOdf(range));
        if (!region.firstSheet() || sheetName() == region.firstSheet()->sheetName())
            printSettings()->setPrintRegion(region);
    }

    if (sheetElement.attributeNS(KoXmlNS::table, "protected", QString()) == "true") {
        loadOdfProtection(sheetElement);
    }
    return true;
}

void Sheet::loadOdfMasterLayoutPage(KoStyleStack &styleStack)
{
    KoPageLayout pageLayout;

    if (styleStack.hasProperty(KoXmlNS::fo, "page-width")) {
        pageLayout.width = KoUnit::parseValue(styleStack.property(KoXmlNS::fo, "page-width"));
    }
    if (styleStack.hasProperty(KoXmlNS::fo, "page-height")) {
        pageLayout.height = KoUnit::parseValue(styleStack.property(KoXmlNS::fo, "page-height"));
    }
    if (styleStack.hasProperty(KoXmlNS::fo, "margin-top")) {
        pageLayout.topMargin = KoUnit::parseValue(styleStack.property(KoXmlNS::fo, "margin-top"));
    }
    if (styleStack.hasProperty(KoXmlNS::fo, "margin-bottom")) {
        pageLayout.bottomMargin = KoUnit::parseValue(styleStack.property(KoXmlNS::fo, "margin-bottom"));
    }
    if (styleStack.hasProperty(KoXmlNS::fo, "margin-left")) {
        pageLayout.leftMargin = KoUnit::parseValue(styleStack.property(KoXmlNS::fo, "margin-left"));
    }
    if (styleStack.hasProperty(KoXmlNS::fo, "margin-right")) {
        pageLayout.rightMargin = KoUnit::parseValue(styleStack.property(KoXmlNS::fo, "margin-right"));
    }
    if (styleStack.hasProperty(KoXmlNS::style, "writing-mode")) {
        kDebug(36003) << "styleStack.hasAttribute( style:writing-mode ) :" << styleStack.hasProperty(KoXmlNS::style, "writing-mode");
        const QString writingMode = styleStack.property(KoXmlNS::style, "writing-mode");
        if (writingMode == "lr-tb") {
            setLayoutDirection(Qt::LeftToRight);
        } else if (writingMode == "rl-tb") {
            setLayoutDirection(Qt::RightToLeft);
        } else {
            // Set the layout direction to the direction of the sheet name.
            checkContentDirection(sheetName());
        }
        //TODO
        //<value>lr-tb</value>
        //<value>rl-tb</value>
        //<value>tb-rl</value>
        //<value>tb-lr</value>
        //<value>lr</value>
        //<value>rl</value>
        //<value>tb</value>
        //<value>page</value>

    } else {
        // Set the layout direction to the direction of the sheet name.
        checkContentDirection(sheetName());
    }
    if (styleStack.hasProperty(KoXmlNS::style, "print-orientation")) {
        pageLayout.orientation = (styleStack.property(KoXmlNS::style, "print-orientation") == "landscape")
                                 ? KoPageFormat::Landscape : KoPageFormat::Portrait;
    }
    if (styleStack.hasProperty(KoXmlNS::style, "num-format")) {
        //not implemented into kspread
        //These attributes specify the numbering style to use.
        //If a numbering style is not specified, the numbering style is inherited from
        //the page style. See section 6.7.8 for information on these attributes
        kDebug(36003) << " num-format :" << styleStack.property(KoXmlNS::style, "num-format");

    }
    if (styleStack.hasProperty(KoXmlNS::fo, "background-color")) {
        //TODO
        kDebug(36003) << " fo:background-color :" << styleStack.property(KoXmlNS::fo, "background-color");
    }
    if (styleStack.hasProperty(KoXmlNS::style, "print")) {
        //todo parsing
        QString str = styleStack.property(KoXmlNS::style, "print");
        kDebug(36003) << " style:print :" << str;

        if (str.contains("headers")) {
            //TODO implement it into kspread
        }
        if (str.contains("grid")) {
            print()->settings()->setPrintGrid(true);
        }
        if (str.contains("annotations")) {
            //TODO it's not implemented
        }
        if (str.contains("objects")) {
            //TODO it's not implemented
        }
        if (str.contains("charts")) {
            //TODO it's not implemented
        }
        if (str.contains("drawings")) {
            //TODO it's not implemented
        }
        if (str.contains("formulas")) {
            d->showFormula = true;
        }
        if (str.contains("zero-values")) {
            //TODO it's not implemented
        }
    }
    if (styleStack.hasProperty(KoXmlNS::style, "table-centering")) {
        QString str = styleStack.property(KoXmlNS::style, "table-centering");
        //TODO not implemented into kspread
        kDebug(36003) << " styleStack.attribute( style:table-centering ) :" << str;
#if 0
        if (str == "horizontal") {
        } else if (str == "vertical") {
        } else if (str == "both") {
        } else if (str == "none") {
        } else
            kDebug(36003) << " table-centering unknown :" << str;
#endif
    }
    print()->settings()->setPageLayout(pageLayout);
}


bool Sheet::loadColumnFormat(const KoXmlElement& column,
                             const KoOdfStylesReader& stylesReader, int & indexCol,
                             QHash<QString, QRegion>& columnStyleRegions)
{
//   kDebug(36003)<<"bool Sheet::loadColumnFormat(const KoXmlElement& column, const KoOdfStylesReader& stylesReader, unsigned int & indexCol ) index Col :"<<indexCol;

    bool isNonDefaultColumn = false;

    int number = 1;
    if (column.hasAttributeNS(KoXmlNS::table, "number-columns-repeated")) {
        bool ok = true;
        int n = column.attributeNS(KoXmlNS::table, "number-columns-repeated", QString()).toInt(&ok);
        if (ok)
            // Some spreadsheet programs may support more rows than KSpread so
            // limit the number of repeated rows.
            // FIXME POSSIBLE DATA LOSS!
            number = qMin(n, KS_colMax - indexCol + 1);
        kDebug(36003) << "Repeated:" << number;
    }

    if (column.hasAttributeNS(KoXmlNS::table, "default-cell-style-name")) {
        const QString styleName = column.attributeNS(KoXmlNS::table, "default-cell-style-name", QString());
        if (!styleName.isEmpty()) {
            columnStyleRegions[styleName] += QRect(indexCol, 1, number, KS_rowMax);
        }
    }

    enum { Visible, Collapsed, Filtered } visibility = Visible;
    if (column.hasAttributeNS(KoXmlNS::table, "visibility")) {
        const QString string = column.attributeNS(KoXmlNS::table, "visibility", "visible");
        if (string == "collapse")
            visibility = Collapsed;
        else if (string == "filter")
            visibility = Filtered;
        isNonDefaultColumn = true;
    }

    KoStyleStack styleStack;
    if (column.hasAttributeNS(KoXmlNS::table, "style-name")) {
        QString str = column.attributeNS(KoXmlNS::table, "style-name", QString());
        const KoXmlElement *style = stylesReader.findStyle(str, "table-column");
        if (style) {
            styleStack.push(*style);
            isNonDefaultColumn = true;
        }
    }
    styleStack.setTypeProperties("table-column"); //style for column

    double width = -1.0;
    if (styleStack.hasProperty(KoXmlNS::style, "column-width")) {
        width = KoUnit::parseValue(styleStack.property(KoXmlNS::style, "column-width") , -1.0);
        kDebug(36003) << " style:column-width : width :" << width;
        isNonDefaultColumn = true;
    }

    bool insertPageBreak = false;
    if (styleStack.hasProperty(KoXmlNS::fo, "break-before")) {
        QString str = styleStack.property(KoXmlNS::fo, "break-before");
        if (str == "page") {
            insertPageBreak = true;
        } else
            kDebug(36003) << " str :" << str;
        isNonDefaultColumn = true;
    } else if (styleStack.hasProperty(KoXmlNS::fo, "break-after")) {
        // TODO
    }

    // If it's a default column, we can return here.
    // This saves the iteration, which can be caused by column cell default styles,
    // but which are not inserted here.
    if (!isNonDefaultColumn) {
        indexCol += number;
        return true;
    }

    for (int i = 0; i < number; ++i) {
        //kDebug(36003) << " insert new column: pos :" << indexCol << " width :" << width << " hidden ?" << visibility;

        const ColumnFormat* columnFormat;
        if (isNonDefaultColumn) {
            ColumnFormat* cf = nonDefaultColumnFormat(indexCol);
            columnFormat = cf;

            if (width != -1.0)   //safe
                cf->setWidth(width);
            if (insertPageBreak) {
                cf->setPageBreak(true);
            }
            if (visibility == Collapsed)
                cf->setHidden(true);
            else if (visibility == Filtered)
                cf->setFiltered(true);

            cf->setPageBreak(insertPageBreak);
        } else {
            columnFormat = this->columnFormat(indexCol);
        }
        ++indexCol;
    }
//     kDebug(36003)<<" after index column !!!!!!!!!!!!!!!!!! :"<<indexCol;
    return true;
}

void Sheet::loadOdfInsertStyles(const Styles& autoStyles,
                                const QHash<QString, QRegion>& styleRegions,
                                const QHash<QString, Conditions>& conditionalStyles,
                                const QRect& usedArea)
{
    const QList<QString> styleNames = styleRegions.keys();
    for (int i = 0; i < styleNames.count(); ++i) {
        if (!autoStyles.contains(styleNames[i]) && !map()->styleManager()->style(styleNames[i])) {
            kWarning(36003) << "\t" << styleNames[i] << " not used";
            continue;
        }
        const bool hasConditions = conditionalStyles.contains(styleNames[i]);
        const QRegion styleRegion = styleRegions[styleNames[i]] & QRegion(usedArea);
        foreach(const QRect& rect, styleRegion.rects()) {
            if (autoStyles.contains(styleNames[i])) {
                kDebug(36003) << "\tautomatic:" << styleNames[i] << " at" << rect;
                Style style;
                style.setDefault(); // "overwrite" existing style
                style.merge(autoStyles[styleNames[i]]);
                cellStorage()->setStyle(Region(rect), style);
            } else {
                const CustomStyle* namedStyle = map()->styleManager()->style(styleNames[i]);
                kDebug(36003) << "\tcustom:" << namedStyle->name() << " at" << rect;
                Style style;
                style.setDefault(); // "overwrite" existing style
                style.merge(*namedStyle);
                cellStorage()->setStyle(Region(rect), style);
            }
            if (hasConditions)
                cellStorage()->setConditions(Region(rect), conditionalStyles[styleNames[i]]);
        }
    }
}

int Sheet::loadRowFormatStyles(const KoXmlElement& row, int &rowIndex,
                          OdfLoadingContext& tableContext,
                          QHash<QString, QRegion>& rowStyleRegions,
                          QHash<QString, QRegion>& cellStyleRegions)
{
//    kDebug(36003)<<"Sheet::loadRowFormat( const KoXmlElement& row, int &rowIndex,const KoOdfStylesReader& stylesReader, bool isLast )***********";
    KoOdfLoadingContext& odfContext = tableContext.odfContext;
    bool isNonDefaultRow = false;

    KoStyleStack styleStack;
    if (row.hasAttributeNS(KoXmlNS::table, "style-name")) {
        QString str = row.attributeNS(KoXmlNS::table, "style-name", QString());
        const KoXmlElement *style = odfContext.stylesReader().findStyle(str, "table-row");
        if (style) {
            styleStack.push(*style);
            isNonDefaultRow = true;
        }
    }
    styleStack.setTypeProperties("table-row");

    int number = 1;
    if (row.hasAttributeNS(KoXmlNS::table, "number-rows-repeated")) {
        bool ok = true;
        int n = row.attributeNS(KoXmlNS::table, "number-rows-repeated", QString()).toInt(&ok);
        if (ok)
            // Some spreadsheet programs may support more rows than KSpread so
            // limit the number of repeated rows.
            // FIXME POSSIBLE DATA LOSS!
            number = qMin(n, KS_rowMax - rowIndex + 1);
    }

    if (row.hasAttributeNS(KoXmlNS::table, "default-cell-style-name")) {
        const QString styleName = row.attributeNS(KoXmlNS::table, "default-cell-style-name", QString());
        if (!styleName.isEmpty()) {
            rowStyleRegions[styleName] += QRect(1, rowIndex, KS_colMax, number);
        }
    }

    double height = -1.0;
    if (styleStack.hasProperty(KoXmlNS::style, "row-height")) {
        height = KoUnit::parseValue(styleStack.property(KoXmlNS::style, "row-height") , -1.0);
        //    kDebug(36003)<<" properties style:row-height : height :"<<height;
        isNonDefaultRow = true;
    }

    enum { Visible, Collapsed, Filtered } visibility = Visible;
    if (row.hasAttributeNS(KoXmlNS::table, "visibility")) {
        const QString string = row.attributeNS(KoXmlNS::table, "visibility", "visible");
        if (string == "collapse")
            visibility = Collapsed;
        else if (string == "filter")
            visibility = Filtered;
        isNonDefaultRow = true;
    }

    bool insertPageBreak = false;
    if (styleStack.hasProperty(KoXmlNS::fo, "break-before")) {
        QString str = styleStack.property(KoXmlNS::fo, "break-before");
        if (str == "page") {
            insertPageBreak = true;
        }
        //  else
        //      kDebug(36003)<<" str :"<<str;
        isNonDefaultRow = true;
    } else if (styleStack.hasProperty(KoXmlNS::fo, "break-after")) {
        // TODO
    }

//     kDebug(36003)<<" create non defaultrow format :"<<rowIndex<<" repeate :"<<number<<" height :"<<height;
    if (isNonDefaultRow) {
        for (int r = 0; r < number; ++r) {
            RowFormat* rowFormat = nonDefaultRowFormat(rowIndex + r);
            if (height != -1.0)
                rowFormat->setHeight(height);
            if (insertPageBreak) {
                rowFormat->setPageBreak(true);
            }
            if (visibility == Collapsed)
                rowFormat->setHidden(true);
            else if (visibility == Filtered)
                rowFormat->setFiltered(true);

            rowFormat->setPageBreak(insertPageBreak);
        }
    }

    int columnIndex = 1;
    int columnMaximal = 0;
    //const int endRow = qMin(rowIndex + number - 1, KS_rowMax);

    KoXmlElement cellElement;
    forEachElement(cellElement, row) {
        if (cellElement.namespaceURI() != KoXmlNS::table)
            continue;
        if (cellElement.localName() != "table-cell" && cellElement.localName() != "covered-table-cell")
            continue;

        bool ok = false;
        const int n = cellElement.attributeNS(KoXmlNS::table, "number-columns-repeated", QString()).toInt(&ok);
        // Some spreadsheet programs may support more columns than
        // KSpread so limit the number of repeated columns.
        // FIXME POSSIBLE DATA LOSS!
        const int numberColumns = ok ? qMin(n, KS_colMax - columnIndex + 1) : 1;
        columnMaximal = qMax(numberColumns, columnMaximal);

        // Styles are inserted at the end of the loading process, so check the XML directly here.
        const QString styleName = cellElement.attributeNS(KoXmlNS::table , "style-name", QString());
        if (!styleName.isEmpty())
            cellStyleRegions[styleName] += QRect(columnIndex, rowIndex, numberColumns, number);

        columnIndex += numberColumns;
    }
    rowIndex += number;
    return columnMaximal;
}

int Sheet::loadRowFormatContent(const KoXmlElement& row, int &rowIndex,
                          OdfLoadingContext& tableContext)
{
//    kDebug(36003)<<"Sheet::loadRowFormat( const KoXmlElement& row, int &rowIndex,const KoOdfStylesReader& stylesReader, bool isLast )***********";
    int number = 1;
    if (row.hasAttributeNS(KoXmlNS::table, "number-rows-repeated")) {
        bool ok = true;
        int n = row.attributeNS(KoXmlNS::table, "number-rows-repeated", QString()).toInt(&ok);
        if (ok)
            // Some spreadsheet programs may support more rows than KSpread so
            // limit the number of repeated rows.
            // FIXME POSSIBLE DATA LOSS!
            number = qMin(n, KS_rowMax - rowIndex + 1);
    }

    int columnIndex = 1;
    int columnMaximal = 0;
    const int endRow = qMin(rowIndex + number - 1, KS_rowMax);

    KoXmlElement cellElement;
    forEachElement(cellElement, row) {
        if (cellElement.namespaceURI() != KoXmlNS::table)
            continue;
        if (cellElement.localName() != "table-cell" && cellElement.localName() != "covered-table-cell")
            continue;

        Cell cell(this, columnIndex, rowIndex);
        cell.loadOdf(cellElement, tableContext);

        bool ok = false;
        const int n = cellElement.attributeNS(KoXmlNS::table, "number-columns-repeated", QString()).toInt(&ok);
        // Some spreadsheet programs may support more columns than
        // KSpread so limit the number of repeated columns.
        // FIXME POSSIBLE DATA LOSS!
        const int numberColumns = ok ? qMin(n, KS_colMax - columnIndex + 1) : 1;
        columnMaximal = qMax(numberColumns, columnMaximal);

        if (!cell.comment().isEmpty())
            cellStorage()->setComment(Region(columnIndex, rowIndex, numberColumns, number, this), cell.comment());
        if (!cell.conditions().isEmpty())
            cellStorage()->setConditions(Region(columnIndex, rowIndex, numberColumns, number, this), cell.conditions());
        if (!cell.validity().isEmpty())
            cellStorage()->setValidity(Region(columnIndex, rowIndex, numberColumns, number, this), cell.validity());

        if (!cell.hasDefaultContent()) {
            // Row-wise filling of PointStorages is faster than column-wise filling.
            QSharedPointer<QTextDocument> richText = cell.richText();
            for (int r = rowIndex; r <= endRow; ++r) {
                for (int c = 0; c < numberColumns; ++c) {
                    Cell target(this, columnIndex + c, r);
                    target.setFormula(cell.formula());
                    target.setUserInput(cell.userInput());
                    target.setRichText(richText);
                    target.setValue(cell.value());
                    if (cell.doesMergeCells()) {
                        target.mergeCells(columnIndex + c, r, cell.mergedXCells(), cell.mergedYCells());
                    }
                }
            }
        }
        columnIndex += numberColumns;
    }
    rowIndex += number;
    return columnMaximal;
}

QRect Sheet::usedArea(bool onlyContent) const
{
    int maxCols = d->cellStorage->columns(!onlyContent);
    int maxRows = d->cellStorage->rows(!onlyContent);

    if (!onlyContent) {
        const RowFormat * row = firstRow();
        while (row) {
            if (row->row() > maxRows)
                maxRows = row->row();

            row = row->next();
        }

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

bool Sheet::compareRows(int row1, int row2, int& maxCols, OdfSavingContext& tableContext) const
{
    if (*rowFormat(row1) != *rowFormat(row2)) {
//         kDebug(36003) <<"\t Formats of" << row1 <<" and" << row2 <<" are different";
        return false;
    }
    if (tableContext.rowHasCellAnchoredShapes(this, row1) != tableContext.rowHasCellAnchoredShapes(this, row2)) {
        return false;
    }
    Cell cell1 = cellStorage()->firstInRow(row1);
    Cell cell2 = cellStorage()->firstInRow(row2);
    if (cell1.isNull() != cell2.isNull())
        return false;
    while (!cell1.isNull()) {
        if (cell1.column() != cell2.column())
            return false;
        if (cell1.column() > maxCols)
            break;
        if (!cell1.compareData(cell2)) {
//             kDebug(36003) <<"\t Cell at column" << col <<" in row" << row2 <<" differs from the one in row" << row1;
            return false;
        }
        cell1 = cellStorage()->nextInRow(cell1.column(), cell1.row());
        cell2 = cellStorage()->nextInRow(cell2.column(), cell2.row());
        if (cell1.isNull() != cell2.isNull())
            return false;
    }
    return true;
}

void Sheet::saveOdfHeaderFooter(KoXmlWriter &xmlWriter) const
{
    QString headerLeft = print()->headerFooter()->headLeft();
    QString headerCenter = print()->headerFooter()->headMid();
    QString headerRight = print()->headerFooter()->headRight();

    QString footerLeft = print()->headerFooter()->footLeft();
    QString footerCenter = print()->headerFooter()->footMid();
    QString footerRight = print()->headerFooter()->footRight();

    xmlWriter.startElement("style:header");
    if ((!headerLeft.isEmpty())
            || (!headerCenter.isEmpty())
            || (!headerRight.isEmpty())) {
        xmlWriter.startElement("style:region-left");
        xmlWriter.startElement("text:p");
        convertPart(headerLeft, xmlWriter);
        xmlWriter.endElement();
        xmlWriter.endElement();

        xmlWriter.startElement("style:region-center");
        xmlWriter.startElement("text:p");
        convertPart(headerCenter, xmlWriter);
        xmlWriter.endElement();
        xmlWriter.endElement();

        xmlWriter.startElement("style:region-right");
        xmlWriter.startElement("text:p");
        convertPart(headerRight, xmlWriter);
        xmlWriter.endElement();
        xmlWriter.endElement();
    } else {
        xmlWriter.startElement("text:p");

        xmlWriter.startElement("text:sheet-name");
        xmlWriter.addTextNode("???");
        xmlWriter.endElement();

        xmlWriter.endElement();
    }
    xmlWriter.endElement();


    xmlWriter.startElement("style:footer");
    if ((!footerLeft.isEmpty())
            || (!footerCenter.isEmpty())
            || (!footerRight.isEmpty())) {
        xmlWriter.startElement("style:region-left");
        xmlWriter.startElement("text:p");
        convertPart(footerLeft, xmlWriter);
        xmlWriter.endElement();
        xmlWriter.endElement(); //style:region-left

        xmlWriter.startElement("style:region-center");
        xmlWriter.startElement("text:p");
        convertPart(footerCenter, xmlWriter);
        xmlWriter.endElement();
        xmlWriter.endElement();

        xmlWriter.startElement("style:region-right");
        xmlWriter.startElement("text:p");
        convertPart(footerRight, xmlWriter);
        xmlWriter.endElement();
        xmlWriter.endElement();
    } else {
        xmlWriter.startElement("text:p");

        xmlWriter.startElement("text:sheet-name");
        xmlWriter.addTextNode("Page ");   // ???
        xmlWriter.endElement();

        xmlWriter.startElement("text:page-number");
        xmlWriter.addTextNode("1");   // ???
        xmlWriter.endElement();

        xmlWriter.endElement();
    }
    xmlWriter.endElement();


}

void Sheet::addText(const QString & text, KoXmlWriter & writer) const
{
    if (!text.isEmpty())
        writer.addTextNode(text);
}

void Sheet::convertPart(const QString & part, KoXmlWriter & xmlWriter) const
{
    QString text;
    QString var;

    bool inVar = false;
    uint i = 0;
    uint l = part.length();
    while (i < l) {
        if (inVar || part[i] == '<') {
            inVar = true;
            var += part[i];
            if (part[i] == '>') {
                inVar = false;
                if (var == "<page>") {
                    addText(text, xmlWriter);
                    xmlWriter.startElement("text:page-number");
                    xmlWriter.addTextNode("1");
                    xmlWriter.endElement();
                } else if (var == "<pages>") {
                    addText(text, xmlWriter);
                    xmlWriter.startElement("text:page-count");
                    xmlWriter.addTextNode("99");   //TODO I think that it can be different from 99
                    xmlWriter.endElement();
                } else if (var == "<date>") {
                    addText(text, xmlWriter);
                    //text:p><text:date style:data-style-name="N2" text:date-value="2005-10-02">02/10/2005</text:date>, <text:time>10:20:12</text:time></text:p> "add style" => create new style
#if 0 //FIXME
                    KoXmlElement t = dd.createElement("text:date");
                    t.setAttribute("text:date-value", "0-00-00");
                    // todo: "style:data-style-name", "N2"
                    t.appendChild(dd.createTextNode(QDate::currentDate().toString()));
                    parent.appendChild(t);
#endif
                } else if (var == "<time>") {
                    addText(text, xmlWriter);

                    xmlWriter.startElement("text:time");
                    xmlWriter.addTextNode(QTime::currentTime().toString());
                    xmlWriter.endElement();
                } else if (var == "<file>") { // filepath + name
                    addText(text, xmlWriter);
                    xmlWriter.startElement("text:file-name");
                    xmlWriter.addAttribute("text:display", "full");
                    xmlWriter.addTextNode("???");
                    xmlWriter.endElement();
                } else if (var == "<name>") { // filename
                    addText(text, xmlWriter);

                    xmlWriter.startElement("text:title");
                    xmlWriter.addTextNode("???");
                    xmlWriter.endElement();
                } else if (var == "<author>") {
                    Doc* sdoc = doc();
                    KoDocumentInfo* docInfo = sdoc->documentInfo();

                    text += docInfo->authorInfo("creator");
                    addText(text, xmlWriter);
                } else if (var == "<email>") {
                    Doc* sdoc = doc();
                    KoDocumentInfo* docInfo = sdoc->documentInfo();

                    text += docInfo->authorInfo("email");
                    addText(text, xmlWriter);

                } else if (var == "<org>") {
                    Doc* sdoc = doc();
                    KoDocumentInfo* docInfo    = sdoc->documentInfo();

                    text += docInfo->authorInfo("company");
                    addText(text, xmlWriter);

                } else if (var == "<sheet>") {
                    addText(text, xmlWriter);

                    xmlWriter.startElement("text:sheet-name");
                    xmlWriter.addTextNode("???");
                    xmlWriter.endElement();
                } else {
                    // no known variable:
                    text += var;
                    addText(text, xmlWriter);
                }

                text = "";
                var  = "";
            }
        } else {
            text += part[i];
        }
        ++i;
    }
    if (!text.isEmpty() || !var.isEmpty()) {
        //we don't have var at the end =>store it
        addText(text + var, xmlWriter);
    }
    kDebug(36003) << " text end :" << text << " var :" << var;
}

void Sheet::saveOdfBackgroundImage(KoXmlWriter& xmlWriter) const
{
    const BackgroundImageProperties& properties = backgroundImageProperties();
    xmlWriter.startElement("style:backgroundImage");

    //xmlWriter.addAttribute("xlink:href", fileName);
    xmlWriter.addAttribute("xlink:type", "simple");
    xmlWriter.addAttribute("xlink:show", "embed");
    xmlWriter.addAttribute("xlink:actuate", "onLoad");

    QString opacity = QString("%1%").arg(properties.opacity);
    xmlWriter.addAttribute("draw:opacity", opacity);

    QString position;
    if(properties.horizontalPosition == BackgroundImageProperties::Left) {
        position += "left";
    }
    else if(properties.horizontalPosition == BackgroundImageProperties::HorizontalCenter) {
        position += "center";
    }
    else if(properties.horizontalPosition == BackgroundImageProperties::Right) {
        position += "right";
    }

    position += " ";

    if(properties.verticalPosition == BackgroundImageProperties::Top) {
        position += "top";
    }
    else if(properties.verticalPosition == BackgroundImageProperties::VerticalCenter) {
        position += "center";
    }
    else if(properties.verticalPosition == BackgroundImageProperties::Bottom) {
        position += "right";
    }
    xmlWriter.addAttribute("style:position", position);

    QString repeat;
    if(properties.repeat == BackgroundImageProperties::NoRepeat) {
        repeat = "no-repeat";
    }
    else if(properties.repeat == BackgroundImageProperties::Repeat) {
        repeat = "repeat";
    }
    else if(properties.repeat == BackgroundImageProperties::Stretch) {
        repeat = "stretch";
    }
    xmlWriter.addAttribute("style:repeat", repeat);

    xmlWriter.endElement();
}


void Sheet::loadOdfSettings(const KoOasisSettings::NamedMap &settings)
{
    // Find the entry in the map that applies to this sheet (by name)
    KoOasisSettings::Items items = settings.entry(sheetName());
    if (items.isNull())
        return;
    setHideZero(!items.parseConfigItemBool("ShowZeroValues"));
    setShowGrid(items.parseConfigItemBool("ShowGrid"));
    setFirstLetterUpper(items.parseConfigItemBool("FirstLetterUpper"));

    int cursorX = qMin(KS_colMax, qMax(1, items.parseConfigItemInt("CursorPositionX") + 1));
    int cursorY = qMin(KS_rowMax, qMax(1, items.parseConfigItemInt("CursorPositionY") + 1));
    map()->loadingInfo()->setCursorPosition(this, QPoint(cursorX, cursorY));

    double offsetX = items.parseConfigItemDouble("xOffset");
    double offsetY = items.parseConfigItemDouble("yOffset");
    map()->loadingInfo()->setScrollingOffset(this, QPointF(offsetX, offsetY));

    setShowFormulaIndicator(items.parseConfigItemBool("ShowFormulaIndicator"));
    setShowCommentIndicator(items.parseConfigItemBool("ShowCommentIndicator"));
    setShowPageBorders(items.parseConfigItemBool("ShowPageBorders"));
    setLcMode(items.parseConfigItemBool("lcmode"));
    setAutoCalculationEnabled(items.parseConfigItemBool("autoCalc"));
    setShowColumnNumber(items.parseConfigItemBool("ShowColumnNumber"));
}

void Sheet::saveOdfSettings(KoXmlWriter &settingsWriter) const
{
    //not into each page into oo spec
    settingsWriter.addConfigItem("ShowZeroValues", !getHideZero());
    settingsWriter.addConfigItem("ShowGrid", getShowGrid());
    //not define into oo spec
    settingsWriter.addConfigItem("FirstLetterUpper", getFirstLetterUpper());
    settingsWriter.addConfigItem("ShowFormulaIndicator", getShowFormulaIndicator());
    settingsWriter.addConfigItem("ShowCommentIndicator", getShowCommentIndicator());
    settingsWriter.addConfigItem("ShowPageBorders", isShowPageBorders());
    settingsWriter.addConfigItem("lcmode", getLcMode());
    settingsWriter.addConfigItem("autoCalc", isAutoCalculationEnabled());
    settingsWriter.addConfigItem("ShowColumnNumber", getShowColumnNumber());
}

bool Sheet::saveOdf(OdfSavingContext& tableContext)
{
    KoXmlWriter & xmlWriter = tableContext.shapeContext.xmlWriter();
    KoGenStyles & mainStyles = tableContext.shapeContext.mainStyles();
    xmlWriter.startElement("table:table");
    xmlWriter.addAttribute("table:name", sheetName());
    xmlWriter.addAttribute("table:style-name", saveOdfSheetStyleName(mainStyles));
    QByteArray pwd;
    password(pwd);
    if (!pwd.isNull()) {
        xmlWriter.addAttribute("table:protected", "true");
        QByteArray str = KCodecs::base64Encode(pwd);
        // FIXME Stefan: see OpenDocument spec, ch. 17.3 Encryption
        xmlWriter.addAttribute("table:protection-key", QString(str));
    }
    QRect _printRange = printSettings()->printRegion().lastRange();
    if (_printRange != (QRect(QPoint(1, 1), QPoint(KS_colMax, KS_rowMax)))) {
        const Region region(_printRange, this);
        if (region.isValid()) {
            kDebug(36003) << region;
            xmlWriter.addAttribute("table:print-ranges", region.saveOdf());
        }
    }

    // flake
    // Create a dict of cell anchored shapes with the cell as key.
    foreach(KoShape* shape, d->shapes) {
        if (dynamic_cast<ShapeApplicationData*>(shape->applicationData())->isAnchoredToCell()) {
            double dummy;
            const QPointF position = shape->position();
            const int col = leftColumn(position.x(), dummy);
            const int row = topRow(position.y(), dummy);
            tableContext.insertCellAnchoredShape(this, row, col, shape);
        }
    }

    const QRect usedArea = this->usedArea();
    saveOdfColRowCell(xmlWriter, mainStyles, usedArea.width(), usedArea.height(), tableContext);

    // flake
    // Save the remaining shapes, those that are anchored in the page.
    if (!d->shapes.isEmpty()) {
        xmlWriter.startElement("table:shapes");
        foreach(KoShape* shape, d->shapes) {
            if (dynamic_cast<ShapeApplicationData*>(shape->applicationData())->isAnchoredToCell())
                continue;
            shape->saveOdf(tableContext.shapeContext);
        }
        xmlWriter.endElement();
    }

    xmlWriter.endElement();
    return true;
}

QString Sheet::saveOdfSheetStyleName(KoGenStyles &mainStyles)
{
    KoGenStyle pageStyle(KoGenStyle::TableAutoStyle, "table"/*FIXME I don't know if name is sheet*/);

    KoGenStyle pageMaster(KoGenStyle::MasterPageStyle);
    const QString pageLayoutName = printSettings()->saveOdfPageLayout(mainStyles,
                                   getShowFormula(),
                                   !getHideZero());
    pageMaster.addAttribute("style:page-layout-name", pageLayoutName);

    QBuffer buffer;
    buffer.open(QIODevice::WriteOnly);
    KoXmlWriter elementWriter(&buffer);    // TODO pass indentation level
    saveOdfHeaderFooter(elementWriter);

    QString elementContents = QString::fromUtf8(buffer.buffer(), buffer.buffer().size());
    pageMaster.addChildElement("headerfooter", elementContents);
    pageStyle.addAttribute("style:master-page-name", mainStyles.insert(pageMaster, "Standard"));

    pageStyle.addProperty("table:display", !isHidden());

    if( !backgroundImage().isNull() ) {
        QBuffer bgBuffer;
        bgBuffer.open(QIODevice::WriteOnly);
        KoXmlWriter bgWriter(&bgBuffer); //TODO pass identation level
        saveOdfBackgroundImage(bgWriter);

        const QString bgContent = QString::fromUtf8(bgBuffer.buffer(), bgBuffer.size());
        pageMaster.addChildElement("backgroundImage", bgContent);
    }

    return mainStyles.insert(pageStyle, "ta");
}


void Sheet::saveOdfColRowCell(KoXmlWriter& xmlWriter, KoGenStyles &mainStyles,
                              int maxCols, int maxRows, OdfSavingContext& tableContext)
{
    kDebug(36003) << "Sheet::saveOdfColRowCell:" << d->name;

    // calculate the column/row default cell styles
    int maxMaxRows = maxRows; // includes the max row a column default style occupies
    // also extends the maximum column/row to include column/row styles
    styleStorage()->saveOdfCreateDefaultStyles(maxCols, maxMaxRows, tableContext);
    if (tableContext.rowDefaultStyles.count() != 0)
        maxRows = qMax(maxRows, (--tableContext.rowDefaultStyles.constEnd()).key());
    // OpenDocument needs at least one cell per sheet.
    maxCols = qMin(KS_colMax, qMax(1, maxCols));
    maxRows = qMin(KS_rowMax, qMax(1, maxRows));
    maxMaxRows = maxMaxRows;
    kDebug(36003) << "\t Sheet dimension:" << maxCols << " x" << maxRows;

    // saving the columns
    //
    int i = 1;
    while (i <= maxCols) {
        const ColumnFormat* column = columnFormat(i);
//         kDebug(36003) << "Sheet::saveOdfColRowCell: first col loop:"
//                       << "i:" << i
//                       << "column:" << (column ? column->column() : 0)
//                       << "default:" << (column ? column->isDefault() : false);

        //style default layout for column
        const Style style = tableContext.columnDefaultStyles.value(i);

        int j = i;
        int count = 1;

        while (j <= maxCols) {
            const ColumnFormat* nextColumn = d->columns.next(j);
            const int nextColumnIndex = nextColumn ? nextColumn->column() : 0;
            const QMap<int, Style>::iterator nextColumnDefaultStyle = tableContext.columnDefaultStyles.upperBound(j);
            const int nextStyleColumnIndex = nextColumnDefaultStyle == tableContext.columnDefaultStyles.end()
                                             ? 0 : nextColumnDefaultStyle.key();
            // j becomes the index of the adjacent column
            ++j;

//           kDebug(36003) <<"Sheet::saveOdfColRowCell: second col loop:"
//                         << "j:" << j
//                         << "next column:" << (nextColumn ? nextColumn->column() : 0)
//                         << "next styled column:" << nextStyleColumnIndex;

            // no next or not the adjacent column?
            if ((!nextColumn && !nextStyleColumnIndex) ||
                    (nextColumnIndex != j && nextStyleColumnIndex != j)) {
                // if the origin column was a default column,
                if (column->isDefault() && style.isDefault()) {
                    // we count the default columns
                    if (!nextColumn && !nextStyleColumnIndex)
                        count = maxCols - i + 1;
                    else if (nextColumn && (!nextStyleColumnIndex || nextColumn->column() <= nextStyleColumnIndex))
                        count = nextColumn->column() - i;
                    else
                        count = nextStyleColumnIndex - i;
                }
                // otherwise we just stop here to process the adjacent
                // column in the next iteration of the outer loop
                break;
            }

            // stop, if the next column differs from the current one
            if ((nextColumn && (*column != *nextColumn)) || (!nextColumn && !column->isDefault()))
                break;
            if (style != tableContext.columnDefaultStyles.value(j))
                break;
            ++count;
        }

        xmlWriter.startElement("table:table-column");
        if (!column->isDefault()) {
            KoGenStyle currentColumnStyle(KoGenStyle::TableColumnAutoStyle, "table-column");
            currentColumnStyle.addPropertyPt("style:column-width", column->width());
            if (column->hasPageBreak()) {
                currentColumnStyle.addProperty("fo:break-before", "page");
            }
            xmlWriter.addAttribute("table:style-name", mainStyles.insert(currentColumnStyle, "co"));
        }
        if (!column->isDefault() || !style.isDefault()) {
            if (!style.isDefault()) {
                KoGenStyle currentDefaultCellStyle; // the type is determined in saveOdfStyle
                const QString name = style.saveOdf(currentDefaultCellStyle, mainStyles,
                                                   map()->styleManager());
                xmlWriter.addAttribute("table:default-cell-style-name", name);
            }

            if (column->isHidden())
                xmlWriter.addAttribute("table:visibility", "collapse");
            else if (column->isFiltered())
                xmlWriter.addAttribute("table:visibility", "filter");
        }
        if (count > 1)
            xmlWriter.addAttribute("table:number-columns-repeated", count);
        xmlWriter.endElement();

        kDebug(36003) << "Sheet::saveOdfColRowCell: column" << i
        << "repeated" << count - 1 << "time(s)";

        i += count;
    }

    // saving the rows and the cells
    // we have to loop through all rows of the used area
    for (i = 1; i <= maxRows; ++i) {
        const RowFormat* row = rowFormat(i);

        // default cell style for row
        const Style style = tableContext.rowDefaultStyles.value(i);

        xmlWriter.startElement("table:table-row");

        if (!row->isDefault()) {
            KoGenStyle currentRowStyle(KoGenStyle::TableRowAutoStyle, "table-row");
            currentRowStyle.addPropertyPt("style:row-height", row->height());
            if (row->hasPageBreak()) {
                currentRowStyle.addProperty("fo:break-before", "page");
            }
            xmlWriter.addAttribute("table:style-name", mainStyles.insert(currentRowStyle, "ro"));
        }

        int repeated = 1;
        // empty row?
        if (!d->cellStorage->firstInRow(i) && !tableContext.rowHasCellAnchoredShapes(this, i)) { // row is empty
//             kDebug(36003) <<"Sheet::saveOdfColRowCell: first row loop:"
//                           << " i: " << i
//                           << " row: " << row->row();
            int j = i + 1;

            // search for
            //   next non-empty row
            // or
            //   next row with different Format
            while (j <= maxRows && !d->cellStorage->firstInRow(j) && !tableContext.rowHasCellAnchoredShapes(this, j)) {
                const RowFormat* nextRow = rowFormat(j);
//               kDebug(36003) <<"Sheet::saveOdfColRowCell: second row loop:"
//                         << " j: " << j
//                         << " row: " << nextRow->row();

                // if the reference row has the default row format
                if (row->isDefault() && style.isDefault()) {
                    // if the next is not default, stop here
                    if (!nextRow->isDefault() || !tableContext.rowDefaultStyles.value(j).isDefault())
                        break;
                    // otherwise, jump to the next
                    ++j;
                    continue;
                }

                // stop, if the next row differs from the current one
                if ((nextRow && *row != *nextRow) || (!nextRow && !row->isDefault()))
                    break;
                if (style != tableContext.rowDefaultStyles.value(j))
                    break;
                // otherwise, process the next
                ++j;
            }
            repeated = j - i;

            if (repeated > 1)
                xmlWriter.addAttribute("table:number-rows-repeated", repeated);
            if (!style.isDefault()) {
                KoGenStyle currentDefaultCellStyle; // the type is determined in saveOdfCellStyle
                const QString name = style.saveOdf(currentDefaultCellStyle, mainStyles,
                                                   map()->styleManager());
                xmlWriter.addAttribute("table:default-cell-style-name", name);
            }
            if (row->isHidden())   // never true for the default row
                xmlWriter.addAttribute("table:visibility", "collapse");
            else if (row->isFiltered()) // never true for the default row
                xmlWriter.addAttribute("table:visibility", "filter");

            // NOTE Stefan: Even if paragraph 8.1 states, that rows may be empty, the
            //              RelaxNG schema does not allow that.
            xmlWriter.startElement("table:table-cell");
            // Fill the row with empty cells, if there's a row default cell style.
            if (!style.isDefault())
                xmlWriter.addAttribute("table:number-columns-repeated", QString::number(maxCols));
            // Fill the row with empty cells up to the last column with a default cell style.
            else if (!tableContext.columnDefaultStyles.isEmpty()) {
                const int col = (--tableContext.columnDefaultStyles.constEnd()).key();
                xmlWriter.addAttribute("table:number-columns-repeated", QString::number(col));
            }
            xmlWriter.endElement();

            kDebug(36003) << "Sheet::saveOdfColRowCell: empty row" << i
            << "repeated" << repeated << "time(s)";

            // copy the index for the next row to process
            i = j - 1; /*it's already incremented in the for loop*/
        } else { // row is not empty
            if (!style.isDefault()) {
                KoGenStyle currentDefaultCellStyle; // the type is determined in saveOdfCellStyle
                const QString name = style.saveOdf(currentDefaultCellStyle, mainStyles,
                                                   map()->styleManager());
                xmlWriter.addAttribute("table:default-cell-style-name", name);
            }
            if (row->isHidden())   // never true for the default row
                xmlWriter.addAttribute("table:visibility", "collapse");
            else if (row->isFiltered()) // never true for the default row
                xmlWriter.addAttribute("table:visibility", "filter");

            int j = i + 1;
            while (j <= maxRows && compareRows(i, j, maxCols, tableContext)) {
                j++;
                repeated++;
            }
            if (repeated > 1) {
                kDebug(36003) << "Sheet::saveOdfColRowCell: NON-empty row" << i
                << "repeated" << repeated << "times";

                xmlWriter.addAttribute("table:number-rows-repeated", repeated);
            }

            saveOdfCells(xmlWriter, mainStyles, i, maxCols, tableContext);

            // copy the index for the next row to process
            i = j - 1; /*it's already incremented in the for loop*/
        }
        xmlWriter.endElement();
    }

    // Fill in rows with empty cells, if there's a column default cell style.
    if (!tableContext.columnDefaultStyles.isEmpty()) {
        if (maxMaxRows > maxRows) {
            xmlWriter.startElement("table:table-row");
            if (maxMaxRows > maxRows + 1)
                xmlWriter.addAttribute("table:number-rows-repeated", maxMaxRows - maxRows);
            xmlWriter.startElement("table:table-cell");
            const int col = qMin(maxCols, (--tableContext.columnDefaultStyles.constEnd()).key());
            xmlWriter.addAttribute("table:number-columns-repeated", QString::number(col));
            xmlWriter.endElement();
            xmlWriter.endElement();
        }
    }
}

void Sheet::saveOdfCells(KoXmlWriter& xmlWriter, KoGenStyles &mainStyles, int row, int maxCols,
                         OdfSavingContext& tableContext)
{
    int i = 1;
    Cell cell(this, i, row);
    Cell nextCell = d->cellStorage->nextInRow(i, row);
    // handle situations where the row contains shapes and nothing else
    if (cell.isDefault() && nextCell.isNull()) {
        int nextShape = tableContext.nextAnchoredShape(this, row, i);
        if (nextShape)
            nextCell = Cell(this, nextShape, row);
    }
    // while
    //   the current cell is not a default one
    // or
    //   we have a further cell in this row
    while (!cell.isDefault() || tableContext.cellHasAnchoredShapes(this, cell.row(), cell.column()) || !nextCell.isNull()) {
//         kDebug(36003) <<"Sheet::saveOdfCells:"
//                       << " i: " << i
//                       << " column: " << cell.column() << endl;

        int repeated = 1;
        int column = i;
        cell.saveOdf(xmlWriter, mainStyles, row, column, repeated, tableContext);
        i += repeated;
        // stop if we reached the end column
        if (i > maxCols || nextCell.isNull())
            break;

        cell = Cell(this, i, row);
        // if we have a shape anchored to an empty cell, ensure that the cell gets also processed
        int nextShape = tableContext.nextAnchoredShape(this, row, column);
        if (nextShape && ((nextShape < i) || cell.isDefault())) {
            cell = Cell(this, nextShape, row);
            i = nextShape;
        }

        nextCell = d->cellStorage->nextInRow(i, row);
    }

    // Fill the row with empty cells, if there's a row default cell style.
    if (tableContext.rowDefaultStyles.contains(row)) {
        if (maxCols >= i) {
            xmlWriter.startElement("table:table-cell");
            if (maxCols > i)
                xmlWriter.addAttribute("table:number-columns-repeated", QString::number(maxCols - i + 1));
            xmlWriter.endElement();
        }
    }
    // Fill the row with empty cells up to the last column with a default cell style.
    else if (!tableContext.columnDefaultStyles.isEmpty()) {
        const int col = (--tableContext.columnDefaultStyles.constEnd()).key();
        if (col >= i) {
            xmlWriter.startElement("table:table-cell");
            if (col > i)
                xmlWriter.addAttribute("table:number-columns-repeated", QString::number(col - i + 1));
            xmlWriter.endElement();
        }
    }
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
            kDebug() << " Direction not implemented :" << layoutDir;
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

        kDebug(36001) << "Sheet::loadXML: table name =" << sname;
        setObjectName(sname.toUtf8());
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
        setShowPageBorders((bool)sheet.attribute("borders").toInt(&ok));
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
                else
                    delete rl;
            } else if (tagName == "column") {
                ColumnFormat *cl = new ColumnFormat();
                cl->setSheet(this);
                if (cl->load(e))
                    insertColumnFormat(cl);
                else
                    delete cl;
            }
#if 0 // KSPREAD_KOPART_EMBEDDING
            else if (tagName == "object") {
                EmbeddedKOfficeObject *ch = new EmbeddedKOfficeObject(doc(), this);
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
#endif // KSPREAD_KOPART_EMBEDDING
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
#if 0 // KSPREAD_KOPART_EMBEDDING
    foreach(EmbeddedObject* object, doc()->embeddedObjects()) {
        if (object->sheet() == this && (object->getType() == OBJECT_KOFFICE_PART || object->getType() == OBJECT_CHART)) {
            kDebug() << "KSpreadSheet::loadChildren";
            if (!dynamic_cast<EmbeddedKOfficeObject*>(object)->embeddedObject()->loadDocument(_store))
                return false;
        }
    }
#endif // KSPREAD_KOPART_EMBEDDING
    return true;
}


void Sheet::setShowPageBorders(bool b)
{
    if (b == d->showPageBorders)
        return;

    d->showPageBorders = b;
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
    d->rows.insertElement(l, l->row());
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
    d->rows.removeElement(row);
    if (!map()->isLoading()) {
        map()->addDamage(new SheetDamage(this, SheetDamage::RowsChanged));
    }
}

void Sheet::showStatusMessage(const QString &message, int timeout)
{
    emit statusMessage(message, timeout);
}

bool Sheet::saveChildren(KoStore* _store, const QString &_path)
{
    Q_UNUSED(_store);
    Q_UNUSED(_path);
#if 0 // KSPREAD_KOPART_EMBEDDING
    int i = 0;
    foreach(EmbeddedObject* object, doc()->embeddedObjects()) {
        if (object->sheet() == this && (object->getType() == OBJECT_KOFFICE_PART || object->getType() == OBJECT_CHART)) {
            QString path = QString("%1/%2").arg(_path).arg(i++);
            if (!dynamic_cast<EmbeddedKOfficeObject*>(object)->embeddedObject()->document()->saveToStore(_store, path))
                return false;
        }
    }
#endif // KSPREAD_KOPART_EMBEDDING
    return true;
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

    setObjectName(name.toUtf8());
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

/**********************
 * Printout Functions *
 **********************/

#ifndef NDEBUG
void Sheet::printDebug()
{
    int iMaxColumn = d->cellStorage->columns();
    int iMaxRow = d->cellStorage->rows();

    kDebug(36001) << "Cell | Content | Value  [UserInput]";
    Cell cell;
    for (int currentrow = 1 ; currentrow <= iMaxRow ; ++currentrow) {
        for (int currentcolumn = 1 ; currentcolumn <= iMaxColumn ; currentcolumn++) {
            cell = Cell(this, currentcolumn, currentrow);
            if (!cell.isEmpty()) {
                QString cellDescr = Cell::name(currentcolumn, currentrow).rightJustified(4);
                //QString cellDescr = "Cell ";
                //cellDescr += QString::number(currentrow).rightJustified(3,'0') + ',';
                //cellDescr += QString::number(currentcolumn).rightJustified(3,'0') + ' ';
                cellDescr += " | ";
                QString valueType;
                QTextStream stream(&valueType);
                stream << cell.value().type();
                cellDescr += valueType.rightJustified(7);
                cellDescr += " | ";
                cellDescr += map()->converter()->asString(cell.value()).asString().rightJustified(5);
                cellDescr += QString("  [%1]").arg(cell.userInput());
                kDebug(36001) << cellDescr;
            }
        }
    }
}
#endif

} // namespace KSpread

#include "Sheet.moc"
