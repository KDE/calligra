/* This file is part of the KDE project
   SPDX-FileCopyrightText: 2010 Marijn Kruisselbrink <mkruisselbrink@kde.org>
   SPDX-FileCopyrightText: 2006-2007 Stefan Nikolaus <stefan.nikolaus@kdemail.net>
   SPDX-FileCopyrightText: 2005 Raphael Langerhorst <raphael.langerhorst@kdemail.net>
   SPDX-FileCopyrightText: 2004-2005 Tomas Mecir <mecirt@gmail.com>
   SPDX-FileCopyrightText: 2004-2006 Inge Wallin <inge@lysator.liu.se>
   SPDX-FileCopyrightText: 1999-2002, 2004, 2005 Laurent Montel <montel@kde.org>
   SPDX-FileCopyrightText: 2002-2005 Ariya Hidayat <ariya@kde.org>
   SPDX-FileCopyrightText: 2001-2003 Philipp Mueller <philipp.mueller@gmx.de>
   SPDX-FileCopyrightText: 2002-2003 Norbert Andres <nandres@web.de>
   SPDX-FileCopyrightText: 2003 Reinhart Geiser <geiseri@kde.org>
   SPDX-FileCopyrightText: 2003-2005 Meni Livne <livne@kde.org>
   SPDX-FileCopyrightText: 2003 Peter Simonsson <psn@linux.se>
   SPDX-FileCopyrightText: 1999-2002 David Faure <faure@kde.org>
   SPDX-FileCopyrightText: 2000-2002 Werner Trobin <trobin@kde.org>
   SPDX-FileCopyrightText: 1999, 2002 Harri Porten <porten@kde.org>
   SPDX-FileCopyrightText: 2002 John Dailey <dailey@vt.edu>
   SPDX-FileCopyrightText: 1998-2000 Torben Weis <weis@kde.org>
   SPDX-FileCopyrightText: 2000 Bernd Wuebben <wuebben@kde.org>
   SPDX-FileCopyrightText: 2000 Simon Hausmann <hausmann@kde.org
   SPDX-FileCopyrightText: 1999 Stephan Kulow <coolo@kde.org>
   SPDX-FileCopyrightText: 1999 Michael Reiher <michael.reiher@gmx.de>
   SPDX-FileCopyrightText: 1999 Boris Wedl <boris.wedl@kfunigraz.ac.at>
   SPDX-FileCopyrightText: 1998-1999 Reginald Stadlbauer <reggie@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

// Local
#include "Cell.h"

#include "engine/CalculationSettings.h"
#include "engine/Formula.h"
#include "engine/NamedAreaManager.h"
#include "engine/ValueCalc.h"

#include "CellStorage.h"
#include "ColFormatStorage.h"
#include "Condition.h"
#include "Currency.h"
#include "Database.h"
#include "Map.h"
#include "RowFormatStorage.h"
#include "Sheet.h"
#include "StyleStorage.h"
#include "ValueFormatter.h"

using namespace Calligra::Sheets;

Cell::Cell()
{
    cs = nullptr;
}

Cell::Cell(Sheet *sheet, int col, int row)
    : CellBase(sheet, col, row)
{
    cs = sheet ? sheet->fullCellStorage() : nullptr;
}

Cell::Cell(Sheet *sheet, const QPoint &pos)
    : CellBase(sheet, pos)
{
    cs = sheet ? sheet->fullCellStorage() : nullptr;
}

Cell::Cell(const Cell &other)
    : CellBase(other)
{
    cs = other.isNull() ? nullptr : other.fullSheet()->fullCellStorage();
}

Cell::~Cell()
{
}

Cell &Cell::operator=(const Cell &other)
{
    CellBase::operator=(other);
    cs = other.isNull() ? nullptr : other.fullSheet()->fullCellStorage();
    return *this;
}

// Return the sheet that this cell belongs to.
Sheet *Cell::fullSheet() const
{
    Q_ASSERT(!isNull());
    return dynamic_cast<Sheet *>(sheet());
}

Localization *Cell::locale() const
{
    return sheet()->map()->calculationSettings()->locale();
}

// Return true if this is the default cell.
bool Cell::isDefault() const
{
    // check each stored attribute
    if (!value().isEmpty())
        return false;
    if (formula() != Formula::empty())
        return false;
    if (!link().isEmpty())
        return false;
    if (doesMergeCells() == true)
        return false;
    if (!style().isDefault())
        return false;
    if (!comment().isEmpty())
        return false;
    if (!conditions().isEmpty())
        return false;
    if (!validity().isEmpty())
        return false;
    return true;
}

// Return true if this is the default cell (apart from maybe a custom style).
bool Cell::hasDefaultContent() const
{
    // check each stored attribute
    if (value() != Value())
        return false;
    if (formula() != Formula::empty())
        return false;
    if (!link().isEmpty())
        return false;
    if (doesMergeCells() == true)
        return false;
    if (!comment().isEmpty())
        return false;
    if (!conditions().isEmpty())
        return false;
    if (!validity().isEmpty())
        return false;
    return true;
}

Conditions Cell::conditions() const
{
    return cs->conditions(column(), row());
}

void Cell::setConditions(const Conditions &conditions)
{
    cs->setConditions(Region(cellPosition()), conditions);
}

Database Cell::database() const
{
    return cs->database(column(), row());
}

Style Cell::style() const
{
    return cs->style(column(), row());
}

Style Cell::effectiveStyle() const
{
    Style style = cs->style(column(), row());
    // use conditional formatting attributes
    const Style conditionalStyle = conditions().testConditions(*this);
    if (!conditionalStyle.isEmpty()) {
        style.merge(conditionalStyle);
    }
    return style;
}

void Cell::setStyle(const Style &style)
{
    cs->setStyle(Region(cellPosition()), style);
    cs->styleStorage()->contains(cellPosition());
}

void Cell::setUserInput(const QString &string)
{
    QString old = userInput();

    CellBase::setUserInput(string);

    if (old != string)
        // remove any existing richtext
        setRichText(QSharedPointer<QTextDocument>());
}

// Overrides the parser to disable parsing if the style is set to string
Value Cell::parsedUserInput(const QString &text)
{
    if (style().formatType() == Format::Text)
        return Value(text);

    return CellBase::parsedUserInput(text);
}

// Return the out text, i.e. the text that is visible in the cells
// square when shown.  This could, for instance, be the calculated
// result of a formula.
//
QString Cell::displayText(const Style &s, Value *v, bool *showFormula) const
{
    if (isNull())
        return QString();

    QString string;
    const Style style = s.isEmpty() ? effectiveStyle() : s;
    // Display a formula if warranted.  If not, display the value instead;
    // this is the most common case.
    if (isFormula() && !(fullSheet()->isProtected() && style.hideFormula())
        && ((showFormula && *showFormula) || (!showFormula && fullSheet()->getShowFormula()))) {
        string = userInput();
        if (showFormula)
            *showFormula = true;
    } else if (!isEmpty()) {
        Value theValue = fullSheet()->fullMap()->formatter()->formatText(value(),
                                                                         style.formatType(),
                                                                         style.precision(),
                                                                         style.floatFormat(),
                                                                         style.prefix(),
                                                                         style.postfix(),
                                                                         style.currency().symbol(),
                                                                         style.customFormat(),
                                                                         style.thousandsSep());
        if (v)
            *v = theValue;
        string = theValue.asString();
        if (showFormula)
            *showFormula = false;
    }
    return string;
}

QSharedPointer<QTextDocument> Cell::richText() const
{
    return cs->richText(column(), row());
}

void Cell::setRichText(QSharedPointer<QTextDocument> text)
{
    cs->setRichText(column(), row(), text);
}

// FIXME: Continue commenting and cleaning here (ingwa)

// copy/paste

void Cell::copyFormat(const Cell &cell, Paste::Mode mode)
{
    Q_ASSERT(!isNull()); // trouble ahead...
    Q_ASSERT(!cell.isNull());

    Value value = this->value();
    value.setFormat(cell.value().format());
    setValue(value);
    if (!style().isDefault() || !cell.style().isDefault()) {
        Style style = cell.style();
        if (mode == Paste::NoBorder) {
            // We do not want to include borders.
            style.clearAttribute(Style::LeftPen);
            style.clearAttribute(Style::RightPen);
            style.clearAttribute(Style::TopPen);
            style.clearAttribute(Style::BottomPen);
        }
        setStyle(style);
    }
    if (!conditions().isEmpty() || !cell.conditions().isEmpty())
        setConditions(cell.conditions());
}

void Cell::copyContent(const Cell &cell, Paste::Mode mode, Paste::Operation op)
{
    Q_ASSERT(!isNull()); // trouble ahead...
    Q_ASSERT(!cell.isNull());

    // Do we want values instead of the regular operation?
    if (mode == Paste::Result) {
        setCellValue(cell.value());
        return;
    }

    Value val = cell.value();
    // Do we want to adjust the value somehow? Only applies if one of the values is numeric.
    if ((op != Paste::OverWrite) && (value().isNumber() || val.isNumber())) {
        Value cur = value();
        ValueCalc *calc = sheet()->map()->calc();
        if (op == Paste::Add)
            val = calc->add(cur, val);
        else if (op == Paste::Mul)
            val = calc->mul(cur, val);
        else if (op == Paste::Sub)
            val = calc->sub(cur, val);
        else if (op == Paste::Div)
            val = calc->div(cur, val);
        setCellValue(val);
        return;
    }

    if (cell.isFormula()) {
        // change all the references, e.g. from A1 to A3 if copying
        // from e.g. B2 to B4
        Formula formula(sheet(), *this);
        formula.setExpression(decodeFormula(cell.encodeFormula()));
        setFormula(formula);
    } else {
        // copy the user input - we don't parse anything, just call it
        sheet()->cellStorage()->setUserInput(column(), row(), cell.userInput());
        setValue(val);
    }
    if (!cell.richText().isNull())
        setRichText(cell.richText());
}

void Cell::copyAll(const Cell &cell, Paste::Mode mode, Paste::Operation op)
{
    Q_ASSERT(!isNull()); // trouble ahead...
    Q_ASSERT(!cell.isNull());

    bool wantContent = true;
    bool wantFormat = true;
    bool wantComment = true;
    bool wantValidity = true;
    if ((mode == Paste::Text) || (mode == Paste::Result)) {
        wantFormat = false;
        wantComment = false;
        wantValidity = false;
    }
    if (mode == Paste::Format) {
        wantContent = false;
        wantComment = false;
        wantValidity = false;
    }
    if (mode == Paste::Comment) {
        wantContent = false;
        wantFormat = false;
        wantValidity = false;
    }

    if (wantFormat)
        copyFormat(cell, mode);
    if (wantContent)
        copyContent(cell, mode, op);

    if (wantComment && (!comment().isEmpty() || !cell.comment().isEmpty()))
        setComment(cell.comment());
    if (wantValidity && (!validity().isEmpty() || !cell.validity().isEmpty()))
        setValidity(cell.validity());
}

// end of copy/paste

bool Cell::needsPrinting() const
{
    if (!userInput().trimmed().isEmpty())
        return true;
    if (!comment().trimmed().isEmpty())
        return true;

    const Style style = effectiveStyle();

    // Cell borders?
    if (style.hasAttribute(Style::TopPen) || style.hasAttribute(Style::LeftPen) || style.hasAttribute(Style::RightPen) || style.hasAttribute(Style::BottomPen)
        || style.hasAttribute(Style::FallDiagonalPen) || style.hasAttribute(Style::GoUpDiagonalPen))
        return true;

    // Background color or brush?
    if (style.hasAttribute(Style::BackgroundBrush)) {
        QBrush brush = style.backgroundBrush();

        // Only brushes that are visible (ie. they have a brush style
        // and are not white) need to be drawn
        if ((brush.style() != Qt::NoBrush) && (brush.color() != Qt::white || !brush.texture().isNull()))
            return true;
    }

    if (style.hasAttribute(Style::BackgroundColor)) {
        debugSheetsRender << "needsPrinting: Has background color";
        QColor backgroundColor = style.backgroundColor();

        // We don't need to print anything, if the background is white opaque or fully transparent.
        if (!(backgroundColor == Qt::white || backgroundColor.alpha() == 0))
            return true;
    }

    return false;
}

// ----------------------------------------------------------------
//                          Formula handling

int Cell::effectiveAlignX() const
{
    const Style style = effectiveStyle();
    int align = style.halign();
    if (align == Style::HAlignUndefined) {
        // numbers should be right-aligned by default, as well as BiDi text
        if ((style.formatType() == Format::Text) || value().isString())
            align = (displayText().isRightToLeft()) ? Style::Right : Style::Left;
        else {
            Value val = value();
            while (val.isArray())
                val = val.element(0, 0);
            if (val.isBoolean() || val.isNumber())
                align = Style::Right;
            else
                align = Style::Left;
        }
    }
    return align;
}

double Cell::width() const
{
    const int rightCol = column() + mergedXCells();
    return fullSheet()->columnFormats()->totalColWidth(column(), rightCol);
}

double Cell::height() const
{
    const int bottomRow = row() + mergedYCells();
    return fullSheet()->rowFormats()->totalRowHeight(row(), bottomRow);
}

QString Cell::link() const
{
    return cs->link(column(), row());
}

void Cell::setLink(const QString &link)
{
    cs->setLink(column(), row(), link);

    if (!link.isEmpty() && userInput().isEmpty())
        parseUserInput(link);
}

bool Cell::isDate() const
{
    const Format::Type t = style().formatType();
    return (Format::isDate(t) || ((t == Format::Generic) && (value().format() == Value::fmt_Date)));
}

bool Cell::isTime() const
{
    const Format::Type t = style().formatType();
    return (Format::isTime(t) || ((t == Format::Generic) && (value().format() == Value::fmt_Time)));
}

bool Cell::isText() const
{
    const Format::Type t = style().formatType();
    return t == Format::Text;
}

// Return true if this cell is part of a merged cell, but not the
// master cell.

bool Cell::isPartOfMerged() const
{
    return cs->isPartOfMerged(column(), row());
}

Cell Cell::masterCell() const
{
    return cs->masterCell(column(), row());
}

// Merge a number of cells, i.e. make this cell obscure a number of
// other cells.  If _x and _y == 0, or dissociate is set, then the merging is removed.
// Setting dissociate removes merged cells in the entire range.
void Cell::mergeCells(int _col, int _row, int _x, int _y, bool dissociate)
{
    cs->mergeCells(_col, _row, _x, _y, dissociate);
}

bool Cell::doesMergeCells() const
{
    return cs->doesMergeCells(column(), row());
}

int Cell::mergedXCells() const
{
    return cs->mergedXCells(column(), row());
}

int Cell::mergedYCells() const
{
    return cs->mergedYCells(column(), row());
}

bool Cell::compareData(const Cell &other) const
{
    if (value() != other.value())
        return false;
    if (formula() != other.formula())
        return false;
    if (link() != other.link())
        return false;
    if (mergedXCells() != other.mergedXCells())
        return false;
    if (mergedYCells() != other.mergedYCells())
        return false;
    if (style() != other.style())
        return false;
    if (comment() != other.comment())
        return false;
    if (conditions() != other.conditions())
        return false;
    if (validity() != other.validity())
        return false;
    return true;
}
