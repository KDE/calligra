/* This file is part of the KDE project
   Copyright 2010 Marijn Kruisselbrink <mkruisselbrink@kde.org>
   Copyright 2006-2007 Stefan Nikolaus <stefan.nikolaus@kdemail.net>
   Copyright 2005 Raphael Langerhorst <raphael.langerhorst@kdemail.net>
   Copyright 2004-2005 Tomas Mecir <mecirt@gmail.com>
   Copyright 2004-2006 Inge Wallin <inge@lysator.liu.se>
   Copyright 1999-2002,2004,2005 Laurent Montel <montel@kde.org>
   Copyright 2002-2005 Ariya Hidayat <ariya@kde.org>
   Copyright 2001-2003 Philipp Mueller <philipp.mueller@gmx.de>
   Copyright 2002-2003 Norbert Andres <nandres@web.de>
   Copyright 2003 Reinhart Geiser <geiseri@kde.org>
   Copyright 2003-2005 Meni Livne <livne@kde.org>
   Copyright 2003 Peter Simonsson <psn@linux.se>
   Copyright 1999-2002 David Faure <faure@kde.org>
   Copyright 2000-2002 Werner Trobin <trobin@kde.org>
   Copyright 1999,2002 Harri Porten <porten@kde.org>
   Copyright 2002 John Dailey <dailey@vt.edu>
   Copyright 1998-2000 Torben Weis <weis@kde.org>
   Copyright 2000 Bernd Wuebben <wuebben@kde.org>
   Copyright 2000 Simon Hausmann <hausmann@kde.org
   Copyright 1999 Stephan Kulow <coolo@kde.org>
   Copyright 1999 Michael Reiher <michael.reiher@gmx.de>
   Copyright 1999 Boris Wedl <boris.wedl@kfunigraz.ac.at>
   Copyright 1998-1999 Reginald Stadlbauer <reggie@kde.org>

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
#include "Cell.h"

#include <stdlib.h>
#include <ctype.h>
#include <float.h>
#include <math.h>

#include "SheetsDebug.h"
#include "CalculationSettings.h"
#include "CellStorage.h"
#include "Condition.h"
#include "Formula.h"
#include "Global.h"
#include "Localization.h"
#include "LoadingInfo.h"
#include "Map.h"
#include "NamedAreaManager.h"
#include "RowColumnFormat.h"
#include "RowFormatStorage.h"
#include "Sheet.h"
#include "Style.h"
#include "StyleManager.h"
#include "Util.h"
#include "Value.h"
#include "Validity.h"
#include "ValueFormatter.h"
#include "ValueParser.h"
#include "StyleStorage.h"

#include <KoXmlReader.h>

#include <QTimer>
#include <QTextDocument>
#include <QTextCursor>

using namespace Calligra::Sheets;

class Q_DECL_HIDDEN Cell::Private : public QSharedData
{
public:
    Private() : sheet(0), column(0), row(0) {}

    Sheet*  sheet;
    uint    column  : 17; // KS_colMax
    uint    row     : 21; // KS_rowMax
};


Cell::Cell()
        : d(0)
{
}

Cell::Cell(const Sheet* sheet, int col, int row)
        : d(new Private)
{
    Q_ASSERT(sheet != 0);
    Q_ASSERT_X(1 <= col && col <= KS_colMax, __FUNCTION__, QString("%1 out of bounds").arg(col).toLocal8Bit());
    Q_ASSERT_X(1 <= row && row <= KS_rowMax, __FUNCTION__, QString("%1 out of bounds").arg(row).toLocal8Bit());
    d->sheet = const_cast<Sheet*>(sheet);
    d->column = col;
    d->row = row;
}

Cell::Cell(const Sheet* sheet, const QPoint& pos)
        : d(new Private)
{
    Q_ASSERT(sheet != 0);
    Q_ASSERT_X(1 <= pos.x() && pos.x() <= KS_colMax, __FUNCTION__, QString("%1 out of bounds").arg(pos.x()).toLocal8Bit());
    Q_ASSERT_X(1 <= pos.y() && pos.y() <= KS_rowMax, __FUNCTION__, QString("%1 out of bounds").arg(pos.y()).toLocal8Bit());
    d->sheet = const_cast<Sheet*>(sheet);
    d->column = pos.x();
    d->row = pos.y();
}

Cell::Cell(const Cell& other)
        : d(other.d)
{
}

Cell::~Cell()
{
}

// Return the sheet that this cell belongs to.
Sheet* Cell::sheet() const
{
    Q_ASSERT(!isNull());
    return d->sheet;
}

KLocale* Cell::locale() const
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

bool Cell::isEmpty() const
{
    // empty = no value or formula
    if (value() != Value())
        return false;
    if (formula() != Formula())
        return false;
    return true;
}

bool Cell::isNull() const
{
    return (!d);
}

// Return true if this cell is a formula.
//
bool Cell::isFormula() const
{
    return !formula().expression().isEmpty();
}

// Return the column number of this cell.
//
int Cell::column() const
{
    // Make sure this isn't called for the null cell.  This assert
    // can save you (could have saved me!) the hassle of some very
    // obscure bugs.
    Q_ASSERT(!isNull());
    Q_ASSERT(1 <= d->column);   //&& d->column <= KS_colMax );
    return d->column;
}

// Return the row number of this cell.
int Cell::row() const
{
    // Make sure this isn't called for the null cell.  This assert
    // can save you (could have saved me!) the hassle of some very
    // obscure bugs.
    Q_ASSERT(!isNull());
    Q_ASSERT(1 <= d->row);   //&& d->row <= KS_rowMax );
    return d->row;
}

// Return the name of this cell, i.e. the string that the user would
// use to reference it.  Example: A1, BZ16
//
QString Cell::name() const
{
    return name(column(), row());
}

// Return the name of any cell given by (col, row).
//
// static
QString Cell::name(int col, int row)
{
    return columnName(col) + QString::number(row);
}

// Return the name of this cell, including the sheet name.
// Example: sheet1!A5
//
QString Cell::fullName() const
{
    return fullName(sheet(), column(), row());
}

// Return the full name of any cell given a sheet and (col, row).
//
// static
QString Cell::fullName(const Sheet* s, int col, int row)
{
    return s->sheetName() + '!' + name(col, row);
}

// Return the symbolic name of the column of this cell.  Examples: A, BB.
//
QString Cell::columnName() const
{
    return columnName(column());
}

// Return the symbolic name of any column.
//
// static
QString Cell::columnName(uint column)
{
    if (column < 1)     //|| column > KS_colMax)
        return QString("@@@");

    QString   str;
    unsigned  digits = 1;
    unsigned  offset = 0;

    --column;

    for (unsigned limit = 26; column >= limit + offset; limit *= 26, ++digits)
        offset += limit;

    for (unsigned col = column - offset; digits; --digits, col /= 26)
        str.prepend(QChar('A' + (col % 26)));

    return str;
}

QString Cell::comment() const
{
    return sheet()->cellStorage()->comment(d->column, d->row);
}

void Cell::setComment(const QString& comment)
{
    sheet()->cellStorage()->setComment(Region(cellPosition()), comment);
}

Conditions Cell::conditions() const
{
    return sheet()->cellStorage()->conditions(d->column, d->row);
}

void Cell::setConditions(const Conditions& conditions)
{
    sheet()->cellStorage()->setConditions(Region(cellPosition()), conditions);
}

Database Cell::database() const
{
    return sheet()->cellStorage()->database(d->column, d->row);
}

Formula Cell::formula() const
{
    return sheet()->cellStorage()->formula(d->column, d->row);
}

void Cell::setFormula(const Formula& formula)
{
    sheet()->cellStorage()->setFormula(column(), row(), formula);
}

Style Cell::style() const
{
    return sheet()->cellStorage()->style(d->column, d->row);
}

Style Cell::effectiveStyle() const
{
    Style style = sheet()->cellStorage()->style(d->column, d->row);
    // use conditional formatting attributes
    const Style conditionalStyle = conditions().testConditions(*this);
    if (!conditionalStyle.isEmpty()) {
        style.merge(conditionalStyle);
    }
    return style;
}

void Cell::setStyle(const Style& style)
{
    sheet()->cellStorage()->setStyle(Region(cellPosition()), style);
    sheet()->cellStorage()->styleStorage()->contains(cellPosition());
}

Validity Cell::validity() const
{
    return sheet()->cellStorage()->validity(d->column, d->row);
}

void Cell::setValidity(Validity validity)
{
    sheet()->cellStorage()->setValidity(Region(cellPosition()), validity);
}





// Return the user input of this cell.  This could, for instance, be a
// formula.
//
QString Cell::userInput() const
{
    const Formula formula = this->formula();
    if (!formula.expression().isEmpty())
        return formula.expression();
    return sheet()->cellStorage()->userInput(d->column, d->row);
}

void Cell::setUserInput(const QString& string)
{
    QString old = userInput();

    if (!string.isEmpty() && string[0] == '=') {
        // set the formula
        Formula formula(sheet(), *this);
        formula.setExpression(string);
        setFormula(formula);
        // remove an existing user input (the non-formula one)
        sheet()->cellStorage()->setUserInput(d->column, d->row, QString());
    } else {
        // remove an existing formula
        setFormula(Formula::empty());
        // set the value
        sheet()->cellStorage()->setUserInput(d->column, d->row, string);
    }

    if (old != string) {
        // remove any existing richtext
        setRichText(QSharedPointer<QTextDocument>());
    }
}

void Cell::setRawUserInput(const QString& string)
{
    if (!string.isEmpty() && string[0] == '=') {
        // set the formula
        Formula formula(sheet(), *this);
        formula.setExpression(string);
        setFormula(formula);
    } else {
        // set the value
        sheet()->cellStorage()->setUserInput(d->column, d->row, string);
    }
}


// Return the out text, i.e. the text that is visible in the cells
// square when shown.  This could, for instance, be the calculated
// result of a formula.
//
QString Cell::displayText(const Style& s, Value *v, bool *showFormula) const
{
    if (isNull())
        return QString();

    QString string;
    const Style style = s.isEmpty() ? effectiveStyle() : s;
    // Display a formula if warranted.  If not, display the value instead;
    // this is the most common case.
    if ( isFormula() && !(sheet()->isProtected() && style.hideFormula()) &&
         ( (showFormula && *showFormula) || (!showFormula && sheet()->getShowFormula()) ) )
    {
        string = userInput();
        if (showFormula)
            *showFormula = true;
    } else if (!isEmpty()) {
        Value theValue = sheet()->map()->formatter()->formatText(value(), style.formatType(), style.precision(),
                 style.floatFormat(), style.prefix(),
                 style.postfix(), style.currency().symbol(),
                 style.customFormat(), style.thousandsSep());
        if (v) *v = theValue;
        string = theValue.asString();
        if (showFormula)
            *showFormula = false;
    }
    return string;
}


// Return the value of this cell.
//
const Value Cell::value() const
{
    return sheet()->cellStorage()->value(d->column, d->row);
}


// Set the value of this cell.
//
void Cell::setValue(const Value& value)
{
    sheet()->cellStorage()->setValue(d->column, d->row, value);
}


QSharedPointer<QTextDocument> Cell::richText() const
{
    return sheet()->cellStorage()->richText(d->column, d->row);
}

void Cell::setRichText(QSharedPointer<QTextDocument> text)
{
    sheet()->cellStorage()->setRichText(d->column, d->row, text);
}

// FIXME: Continue commenting and cleaning here (ingwa)


void Cell::copyFormat(const Cell& cell)
{
    Q_ASSERT(!isNull());   // trouble ahead...
    Q_ASSERT(!cell.isNull());
    Value value = this->value();
    value.setFormat(cell.value().format());
    sheet()->cellStorage()->setValue(d->column, d->row, value);
    if (!style().isDefault() || !cell.style().isDefault())
        setStyle(cell.style());
    if (!conditions().isEmpty() || !cell.conditions().isEmpty())
        setConditions(cell.conditions());
}

void Cell::copyAll(const Cell& cell)
{
    Q_ASSERT(!isNull());   // trouble ahead...
    Q_ASSERT(!cell.isNull());
    copyFormat(cell);
    copyContent(cell);
    if (!comment().isEmpty() || !cell.comment().isEmpty())
        setComment(cell.comment());
    if (!validity().isEmpty() || !cell.validity().isEmpty())
        setValidity(cell.validity());
}

void Cell::copyContent(const Cell& cell)
{
    Q_ASSERT(!isNull());   // trouble ahead...
    Q_ASSERT(!cell.isNull());
    if (cell.isFormula()) {
        // change all the references, e.g. from A1 to A3 if copying
        // from e.g. B2 to B4
        Formula formula(sheet(), *this);
        formula.setExpression(decodeFormula(cell.encodeFormula()));
        setFormula(formula);
    } else {
        // copy the user input
        sheet()->cellStorage()->setUserInput(d->column, d->row, cell.userInput());
    }
    // copy the value in both cases
    sheet()->cellStorage()->setValue(d->column, d->row, cell.value());
}

bool Cell::needsPrinting() const
{
    if (!userInput().trimmed().isEmpty())
        return true;
    if (!comment().trimmed().isEmpty())
        return true;

    const Style style = effectiveStyle();

    // Cell borders?
    if (style.hasAttribute(Style::TopPen) ||
            style.hasAttribute(Style::LeftPen) ||
            style.hasAttribute(Style::RightPen) ||
            style.hasAttribute(Style::BottomPen) ||
            style.hasAttribute(Style::FallDiagonalPen) ||
            style.hasAttribute(Style::GoUpDiagonalPen))
        return true;

    // Background color or brush?
    if (style.hasAttribute(Style::BackgroundBrush)) {
        QBrush brush = style.backgroundBrush();

        // Only brushes that are visible (ie. they have a brush style
        // and are not white) need to be drawn
        if ((brush.style() != Qt::NoBrush) &&
                (brush.color() != Qt::white || !brush.texture().isNull()))
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


QString Cell::encodeFormula(bool fixedReferences) const
{
    if (!isFormula())
        return QString();

    QString result('=');
    const Tokens tokens = formula().tokens();
    for (int i = 0; i < tokens.count(); ++i) {
        const Token token = tokens[i];
        switch (token.type()) {
        case Token::Cell:
        case Token::Range: {
            if (sheet()->map()->namedAreaManager()->contains(token.text())) {
                result.append(token.text()); // simply keep the area name
                break;
            }
            const Region region(token.text(), sheet()->map());
            // Actually, a contiguous region, but the fixation is needed
            Region::ConstIterator end = region.constEnd();
            for (Region::ConstIterator it = region.constBegin(); it != end; ++it) {
                if (!(*it)->isValid())
                    continue;
                if ((*it)->type() == Region::Element::Point) {
                    if ((*it)->sheet())
                        result.append((*it)->sheet()->sheetName() + '!');
                    const QPoint pos = (*it)->rect().topLeft();
                    if ((*it)->isColumnFixed())
                        result.append(QString("$%1").arg(pos.x()));
                    else if (fixedReferences)
                        result.append(QChar(0xA7) + QString("%1").arg(pos.x()));
                    else
                        result.append(QString("#%1").arg(pos.x() - (int)d->column));
                    if ((*it)->isRowFixed())
                        result.append(QString("$%1#").arg(pos.y()));
                    else if (fixedReferences)
                        result.append(QChar(0xA7) + QString("%1#").arg(pos.y()));
                    else
                        result.append(QString("#%1#").arg(pos.y() - (int)d->row));
                } else { // ((*it)->type() == Region::Range)
                    if ((*it)->sheet())
                        result.append((*it)->sheet()->sheetName() + '!');
                    QPoint pos = (*it)->rect().topLeft();
                    if ((*it)->isLeftFixed())
                        result.append(QString("$%1").arg(pos.x()));
                    else if (fixedReferences)
                        result.append(QChar(0xA7) + QString("%1").arg(pos.x()));
                    else
                        result.append(QString("#%1").arg(pos.x() - (int)d->column));
                    if ((*it)->isTopFixed())
                        result.append(QString("$%1#").arg(pos.y()));
                    else if (fixedReferences)
                        result.append(QChar(0xA7) + QString("%1#").arg(pos.y()));
                    else
                        result.append(QString("#%1#").arg(pos.y() - (int)d->row));
                    result.append(':');
                    pos = (*it)->rect().bottomRight();
                    if ((*it)->isRightFixed())
                        result.append(QString("$%1").arg(pos.x()));
                    else if (fixedReferences)
                        result.append(QChar(0xA7) + QString("%1").arg(pos.x()));
                    else
                        result.append(QString("#%1").arg(pos.x() - (int)d->column));
                    if ((*it)->isBottomFixed())
                        result.append(QString("$%1#").arg(pos.y()));
                    else if (fixedReferences)
                        result.append(QChar(0xA7) + QString("%1#").arg(pos.y()));
                    else
                        result.append(QString("#%1#").arg(pos.y() - (int)d->row));
                }
            }
            break;
        }
        default: {
            result.append(token.text());
            break;
        }
        }
    }
    //debugSheets << result;
    return result;
}

QString Cell::decodeFormula(const QString &_text) const
{
    QString erg;
    unsigned int pos = 0;
    const unsigned int length = _text.length();

    if (_text.isEmpty())
        return QString();

    while (pos < length) {
        if (_text[pos] == '"') {
            erg += _text[pos++];
            while (pos < length && _text[pos] != '"') {
                erg += _text[pos++];
                // Allow escaped double quotes (\")
                if (pos < length && _text[pos] == '\\' && _text[pos+1] == '"') {
                    erg += _text[pos++];
                    erg += _text[pos++];
                }
            }
            if (pos < length)
                erg += _text[pos++];
        } else if (_text[pos] == '#' || _text[pos] == '$' || _text[pos] == QChar(0xA7)) {
            bool abs1 = false;
            bool abs2 = false;
            bool era1 = false; // if 1st is relative but encoded absolutely
            bool era2 = false;

            QChar _t = _text[pos++];
            if (_t == '$')
                abs1 = true;
            else if (_t == QChar(0xA7))
                era1 = true;

            int col = 0;
            unsigned int oldPos = pos;
            while (pos < length && (_text[pos].isDigit() || _text[pos] == '-')) ++pos;
            if (pos != oldPos)
                col = _text.mid(oldPos, pos - oldPos).toInt();
            if (!abs1 && !era1)
                col += d->column;
            // Skip '#' or '$'

            _t = _text[pos++];
            if (_t == '$')
                abs2 = true;
            else if (_t == QChar(0xA7))
                era2 = true;

            int row = 0;
            oldPos = pos;
            while (pos < length && (_text[pos].isDigit() || _text[pos] == '-')) ++pos;
            if (pos != oldPos)
                row = _text.mid(oldPos, pos - oldPos).toInt();
            if (!abs2 && !era2)
                row += d->row;
            // Skip '#' or '$'
            ++pos;
            if (row < 1 || col < 1 || row > KS_rowMax || col > KS_colMax) {
                debugSheetsODF << "Cell::decodeFormula: row or column out of range (col:" << col << " | row:" << row << ')';
                erg += Value::errorREF().errorMessage();
            } else {
                if (abs1)
                    erg += '$';
                erg += Cell::columnName(col); //Get column text

                if (abs2)
                    erg += '$';
                erg += QString::number(row);
            }
        } else
            erg += _text[pos++];
    }

    return erg;
}


// ----------------------------------------------------------------
//                          Formula handling


bool Cell::makeFormula()
{
//   debugSheetsFormula ;

    // sanity check
    if (!isFormula())
        return false;

    // parse the formula and check for errors
    if (!formula().isValid()) {
        sheet()->showStatusMessage(i18n("Parsing of formula in cell %1 failed.", fullName()));
        setValue(Value::errorPARSE());
        return false;
    }
    return true;
}

int Cell::effectiveAlignX() const
{
    const Style style = effectiveStyle();
    int align = style.halign();
    if (align == Style::HAlignUndefined) {
        //numbers should be right-aligned by default, as well as BiDi text
        if ((style.formatType() == Format::Text) || value().isString())
            align = (displayText().isRightToLeft()) ? Style::Right : Style::Left;
        else {
            Value val = value();
            while (val.isArray()) val = val.element(0, 0);
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
    const int rightCol = d->column + mergedXCells();
    double width = 0.0;
    for (int col = d->column; col <= rightCol; ++col)
        width += sheet()->columnFormat(col)->width();
    return width;
}

double Cell::height() const
{
    const int bottomRow = d->row + mergedYCells();
    return sheet()->rowFormats()->totalRowHeight(d->row, bottomRow);
}

// parses the text
void Cell::parseUserInput(const QString& text)
{
//   debugSheets ;

    // empty string?
    if (text.isEmpty()) {
        setValue(Value::empty());
        setUserInput(text);
        setFormula(Formula::empty());
        return;
    }

    // a formula?
    if (text[0] == '=') {
        Formula formula(sheet(), *this);
        formula.setExpression(text);
        setFormula(formula);

        // parse the formula and check for errors
        if (!formula.isValid()) {
            sheet()->showStatusMessage(i18n("Parsing of formula in cell %1 failed.", fullName()));
            setValue(Value::errorPARSE());
            return;
        }
        return;
    }

    // keep the old formula and value for the case, that validation fails
    const Formula oldFormula = formula();
    const QString oldUserInput = userInput();
    const Value oldValue = value();

    // here, the new value is not a formula anymore; clear an existing one
    setFormula(Formula());

    Value value;
    if (style().formatType() == Format::Text)
        value = Value(QString(text));
    else {
        // Parses the text and return the appropriate value.
        value = sheet()->map()->parser()->parse(text);

#if 0
        // Parsing as time acts like an autoformat: we even change the input text
        // [h]:mm:ss -> might get set by ValueParser
        if (isTime() && (formatType() != Format::Time7))
            setUserInput(locale()->formatTime(value().asDateTime(sheet()->map()->calculationSettings()).time(), true));
#endif

        // convert first letter to uppercase ?
        if (sheet()->getFirstLetterUpper() && value.isString() && !text.isEmpty()) {
            QString str = value.asString();
            value = Value(str[0].toUpper() + str.right(str.length() - 1));
        }
    }
    // set the new value
    setUserInput(text);
    setValue(value);

    // validation
    if (!sheet()->isLoading()) {
        Validity validity = this->validity();
        if (!validity.testValidity(this)) {
            debugSheetsODF << "Validation failed";
            //reapply old value if action == stop
            setFormula(oldFormula);
            setUserInput(oldUserInput);
            setValue(oldValue);
        }
    }
}

QString Cell::link() const
{
    return sheet()->cellStorage()->link(d->column, d->row);
}

void Cell::setLink(const QString& link)
{
    sheet()->cellStorage()->setLink(d->column, d->row, link);

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
    return sheet()->cellStorage()->isPartOfMerged(d->column, d->row);
}

Cell Cell::masterCell() const
{
    return sheet()->cellStorage()->masterCell(d->column, d->row);
}

// Merge a number of cells, i.e. make this cell obscure a number of
// other cells.  If _x and _y == 0, then the merging is removed.
void Cell::mergeCells(int _col, int _row, int _x, int _y)
{
    sheet()->cellStorage()->mergeCells(_col, _row, _x, _y);
}

bool Cell::doesMergeCells() const
{
    return sheet()->cellStorage()->doesMergeCells(d->column, d->row);
}

int Cell::mergedXCells() const
{
    return sheet()->cellStorage()->mergedXCells(d->column, d->row);
}

int Cell::mergedYCells() const
{
    return sheet()->cellStorage()->mergedYCells(d->column, d->row);
}

bool Cell::isLocked() const
{
    return sheet()->cellStorage()->isLocked(d->column, d->row);
}

QRect Cell::lockedCells() const
{
    return sheet()->cellStorage()->lockedCells(d->column, d->row);
}


// ================================================================
//                       Saving and loading


QDomElement Cell::save(QDomDocument& doc, int xOffset, int yOffset, bool era)
{
    // Save the position of this cell
    QDomElement cell = doc.createElement("cell");
    cell.setAttribute("row", QString::number(row() - yOffset));
    cell.setAttribute("column", QString::number(column() - xOffset));

    //
    // Save the formatting information
    //
    QDomElement formatElement(doc.createElement("format"));
    style().saveXML(doc, formatElement, sheet()->map()->styleManager());
    if (formatElement.hasChildNodes() || formatElement.attributes().length())   // don't save empty tags
        cell.appendChild(formatElement);

    if (doesMergeCells()) {
        if (mergedXCells())
            formatElement.setAttribute("colspan", QString::number(mergedXCells()));
        if (mergedYCells())
            formatElement.setAttribute("rowspan", QString::number(mergedYCells()));
    }

    Conditions conditions = this->conditions();
    if (!conditions.isEmpty()) {
        QDomElement conditionElement = conditions.saveConditions(doc, sheet()->map()->converter());
        if (!conditionElement.isNull())
            cell.appendChild(conditionElement);
    }

    Validity validity = this->validity();
    if (!validity.isEmpty()) {
        QDomElement validityElement = validity.saveXML(doc, sheet()->map()->converter());
        if (!validityElement.isNull())
            cell.appendChild(validityElement);
    }

    const QString comment = this->comment();
    if (!comment.isEmpty()) {
        QDomElement commentElement = doc.createElement("comment");
        commentElement.appendChild(doc.createCDATASection(comment));
        cell.appendChild(commentElement);
    }

    //
    // Save the text
    //
    if (!userInput().isEmpty()) {
        // Formulas need to be encoded to ensure that they
        // are position independent.
        if (isFormula()) {
            QDomElement txt = doc.createElement("text");
            // if we are cutting to the clipboard, relative references need to be encoded absolutely
            txt.appendChild(doc.createTextNode(encodeFormula(era)));
            cell.appendChild(txt);

            /* we still want to save the results of the formula */
            QDomElement formulaResult = doc.createElement("result");
            saveCellResult(doc, formulaResult, displayText());
            cell.appendChild(formulaResult);

        } else if (!link().isEmpty()) {
            // KSpread pre 1.4 saves link as rich text, marked with first char '
            // Have to be saved in some CDATA section because of too many special characters.
            QDomElement txt = doc.createElement("text");
            QString qml = "!<a href=\"" + link() + "\">" + userInput() + "</a>";
            txt.appendChild(doc.createCDATASection(qml));
            cell.appendChild(txt);
        } else {
            // Save the cell contents (in a locale-independent way)
            QDomElement txt = doc.createElement("text");
            saveCellResult(doc, txt, userInput());
            cell.appendChild(txt);
        }
    }
    if (cell.hasChildNodes() || cell.attributes().length() > 2)   // don't save empty tags
        // (the >2 is due to "row" and "column" attributes)
        return cell;
    else
        return QDomElement();
}

bool Cell::saveCellResult(QDomDocument& doc, QDomElement& result,
                          QString str)
{
    QString dataType = "Other"; // fallback

    if (value().isNumber()) {
        if (isDate()) {
            // serial number of date
            QDate dd = value().asDateTime(sheet()->map()->calculationSettings()).date();
            dataType = "Date";
            str = "%1/%2/%3";
            str = str.arg(dd.year()).arg(dd.month()).arg(dd.day());
        } else if (isTime()) {
            // serial number of time
            dataType = "Time";
            str = value().asDateTime(sheet()->map()->calculationSettings()).time().toString();
        } else {
            // real number
            dataType = "Num";
            if (value().isInteger())
                str = QString::number(value().asInteger());
            else
                str = QString::number(numToDouble(value().asFloat()), 'g', DBL_DIG);
        }
    }

    if (value().isBoolean()) {
        dataType = "Bool";
        str = value().asBoolean() ? "true" : "false";
    }

    if (value().isString()) {
        dataType = "Str";
        str = value().asString();
    }

    result.setAttribute("dataType", dataType);

    const QString displayText = this->displayText();
    if (!displayText.isEmpty())
        result.setAttribute("outStr", displayText);
    result.appendChild(doc.createTextNode(str));

    return true; /* really isn't much of a way for this function to fail */
}

bool Cell::load(const KoXmlElement & cell, int _xshift, int _yshift,
                Paste::Mode mode, Paste::Operation op, bool paste)
{
    bool ok;

    //
    // First of all determine in which row and column this
    // cell belongs.
    //
    d->row = cell.attribute("row").toInt(&ok) + _yshift;
    if (!ok) return false;
    d->column = cell.attribute("column").toInt(&ok) + _xshift;
    if (!ok) return false;

    // Validation
    if (d->row < 1 || d->row > KS_rowMax) {
        debugSheets << "Cell::load: Value out of range Cell:row=" << d->row;
        return false;
    }
    if (d->column < 1 || d->column > KS_colMax) {
        debugSheets << "Cell::load: Value out of range Cell:column=" << d->column;
        return false;
    }

    //
    // Load formatting information.
    //
    KoXmlElement formatElement = cell.namedItem("format").toElement();
    if (!formatElement.isNull() &&
            ((mode == Paste::Normal) || (mode == Paste::Format) || (mode == Paste::NoBorder))) {
        int mergedXCells = 0;
        int mergedYCells = 0;
        if (formatElement.hasAttribute("colspan")) {
            int i = formatElement.attribute("colspan").toInt(&ok);
            if (!ok) return false;
            // Validation
            if (i < 0 || i > KS_spanMax) {
                debugSheets << "Value out of range Cell::colspan=" << i;
                return false;
            }
            if (i)
                mergedXCells = i;
        }

        if (formatElement.hasAttribute("rowspan")) {
            int i = formatElement.attribute("rowspan").toInt(&ok);
            if (!ok) return false;
            // Validation
            if (i < 0 || i > KS_spanMax) {
                debugSheets << "Value out of range Cell::rowspan=" << i;
                return false;
            }
            if (i)
                mergedYCells = i;
        }

        if (mergedXCells != 0 || mergedYCells != 0)
            mergeCells(d->column, d->row, mergedXCells, mergedYCells);

        Style style;
        if (!style.loadXML(formatElement, mode))
            return false;
        setStyle(style);
    }

    //
    // Load the condition section of a cell.
    //
    KoXmlElement conditionsElement = cell.namedItem("condition").toElement();
    if (!conditionsElement.isNull()) {
        Conditions conditions;
        Map *const map = sheet()->map();
        ValueParser *const valueParser = map->parser();
        conditions.loadConditions(conditionsElement, valueParser);
        if (!conditions.isEmpty())
            setConditions(conditions);
    } else if (paste && (mode == Paste::Normal || mode == Paste::NoBorder)) {
        //clear the conditional formatting
        setConditions(Conditions());
    }

    KoXmlElement validityElement = cell.namedItem("validity").toElement();
    if (!validityElement.isNull()) {
        Validity validity;
        if (validity.loadXML(this, validityElement))
            setValidity(validity);
    } else if (paste && (mode == Paste::Normal || mode == Paste::NoBorder)) {
        // clear the validity
        setValidity(Validity());
    }

    //
    // Load the comment
    //
    KoXmlElement comment = cell.namedItem("comment").toElement();
    if (!comment.isNull() &&
            (mode == Paste::Normal || mode == Paste::Comment || mode == Paste::NoBorder)) {
        QString t = comment.text();
        //t = t.trimmed();
        setComment(t);
    }

    //
    // The real content of the cell is loaded here. It is stored in
    // the "text" tag, which contains either a text or a CDATA section.
    //
    // TODO: make this suck less. We set data twice, in loadCellData, and
    // also here. Not good.
    KoXmlElement text = cell.namedItem("text").toElement();

    if (!text.isNull() &&
            (mode == Paste::Normal || mode == Paste::Text || mode == Paste::NoBorder || mode == Paste::Result)) {

        /* older versions mistakenly put the datatype attribute on the cell instead
           of the text. Just move it over in case we're parsing an old document */
        QString dataType;
        if (cell.hasAttribute("dataType"))     // new docs
            dataType = cell.attribute("dataType");

        KoXmlElement result = cell.namedItem("result").toElement();
        QString txt = text.text();
        if ((mode == Paste::Result) && (txt[0] == '='))
            // paste text of the element, if we want to paste result
            // and the source cell contains a formula
            setUserInput(result.text());
        else
            //otherwise copy everything
            loadCellData(text, op, dataType);

        if (!result.isNull()) {
            QString dataType;
            QString t = result.text();

            if (result.hasAttribute("dataType"))
                dataType = result.attribute("dataType");

            // boolean ?
            if (dataType == "Bool") {
                if (t == "false")
                    setValue(Value(false));
                else if (t == "true")
                    setValue(Value(true));
            } else if (dataType == "Num") {
                bool ok = false;
                double dd = t.toDouble(&ok);
                if (ok)
                    setValue(Value(dd));
            } else if (dataType == "Date") {
                bool ok = false;
                double dd = t.toDouble(&ok);
                if (ok) {
                    Value value(dd);
                    value.setFormat(Value::fmt_Date);
                    setValue(value);
                } else {
                    int pos   = t.indexOf('/');
                    int year  = t.mid(0, pos).toInt();
                    int pos1  = t.indexOf('/', pos + 1);
                    int month = t.mid(pos + 1, ((pos1 - 1) - pos)).toInt();
                    int day   = t.right(t.length() - pos1 - 1).toInt();
                    QDate date(year, month, day);
                    if (date.isValid())
                        setValue(Value(date, sheet()->map()->calculationSettings()));
                }
            } else if (dataType == "Time") {
                bool ok = false;
                double dd = t.toDouble(&ok);
                if (ok) {
                    Value value(dd);
                    value.setFormat(Value::fmt_Time);
                    setValue(value);
                } else {
                    int hours   = -1;
                    int minutes = -1;
                    int second  = -1;
                    int pos, pos1;
                    pos   = t.indexOf(':');
                    hours = t.mid(0, pos).toInt();
                    pos1  = t.indexOf(':', pos + 1);
                    minutes = t.mid(pos + 1, ((pos1 - 1) - pos)).toInt();
                    second  = t.right(t.length() - pos1 - 1).toInt();
                    QTime time(hours, minutes, second);
                    if (time.isValid())
                        setValue(Value(time));
                }
            } else {
                setValue(Value(t));
            }
        }
    }

    return true;
}

bool Cell::loadCellData(const KoXmlElement & text, Paste::Operation op, const QString &_dataType)
{
    //TODO: use converter()->asString() to generate userInput()

    QString t = text.text();
    t = t.trimmed();

    // A formula like =A1+A2 ?
    if ((!t.isEmpty()) && (t[0] == '=')) {
        t = decodeFormula(t);
        parseUserInput(pasteOperation(t, userInput(), op));

        makeFormula();
    }
    // rich text ?
    else if ((!t.isEmpty()) && (t[0] == '!')) {
        // KSpread pre 1.4 stores hyperlink as rich text (first char is '!')
        // extract the link and the corresponding text
        // This is a rather dirty hack, but enough for Calligra Sheets generated XML
        bool inside_tag = false;
        QString qml_text;
        QString tag;
        QString qml_link;

        for (int i = 1; i < t.length(); ++i) {
            QChar ch = t[i];
            if (ch == '<') {
                if (!inside_tag) {
                    inside_tag = true;
                    tag.clear();
                }
            } else if (ch == '>') {
                if (inside_tag) {
                    inside_tag = false;
                    if (tag.startsWith(QLatin1String("a href=\""), Qt::CaseSensitive) &&
                        tag.endsWith(QLatin1Char('"'))) {
                        qml_link.remove(0, 8).chop(1);
                    }
                    tag.clear();
                }
            } else {
                if (!inside_tag)
                    qml_text += ch;
                else
                    tag += ch;
            }
        }

        if (!qml_link.isEmpty())
            setLink(qml_link);
        setUserInput(qml_text);
        setValue(Value(qml_text));
    } else {
        bool newStyleLoading = true;
        QString dataType = _dataType;

        if (dataType.isNull()) {
            if (text.hasAttribute("dataType")) {   // new docs
                dataType = text.attribute("dataType");
            } else { // old docs: do the ugly solution of parsing the text
                // ...except for date/time
                if (isDate() && (t.count('/') == 2))
                    dataType = "Date";
                else if (isTime() && (t.count(':') == 2))
                    dataType = "Time";
                else {
                    parseUserInput(pasteOperation(t, userInput(), op));
                    newStyleLoading = false;
                }
            }
        }

        if (newStyleLoading) {
            // boolean ?
            if (dataType == "Bool")
                setValue(Value(t.toLower() == "true"));

            // number ?
            else if (dataType == "Num") {
                bool ok = false;
                if (t.contains('.'))
                    setValue(Value(t.toDouble(&ok)));      // We save in non-localized format
                else
                    setValue(Value(t.toLongLong(&ok)));
                if (!ok) {
                    warnSheets << "Couldn't parse '" << t << "' as number.";
                }
                /* We will need to localize the text version of the number */
                KLocale* locale = sheet()->map()->calculationSettings()->locale();

                /* KLocale::formatNumber requires the precision we want to return.
                */
                int precision = t.length() - t.indexOf('.') - 1;

                if (style().formatType() == Format::Percentage) {
                    if (value().isInteger())
                        t = locale->formatNumber(value().asInteger() * 100);
                    else
                        t = locale->formatNumber(numToDouble(value().asFloat() * 100.0), precision);
                    setUserInput(pasteOperation(t, userInput(), op));
                    setUserInput(userInput() + '%');
                } else {
                    if (value().isInteger())
                        t = locale->formatLong(value().asInteger());
                    else
                        t = locale->formatNumber(numToDouble(value().asFloat()), precision);
                    setUserInput(pasteOperation(t, userInput(), op));
                }
            }

            // date ?
            else if (dataType == "Date") {
                int pos = t.indexOf('/');
                int year = t.mid(0, pos).toInt();
                int pos1 = t.indexOf('/', pos + 1);
                int month = t.mid(pos + 1, ((pos1 - 1) - pos)).toInt();
                int day = t.right(t.length() - pos1 - 1).toInt();
                setValue(Value(QDate(year, month, day), sheet()->map()->calculationSettings()));
                if (value().asDate(sheet()->map()->calculationSettings()).isValid())   // Should always be the case for new docs
                    setUserInput(locale()->formatDate(value().asDate(sheet()->map()->calculationSettings()), KLocale::ShortDate));
                else { // This happens with old docs, when format is set wrongly to date
                    parseUserInput(pasteOperation(t, userInput(), op));
                }
            }

            // time ?
            else if (dataType == "Time") {
                int hours = -1;
                int minutes = -1;
                int second = -1;
                int pos, pos1;
                pos = t.indexOf(':');
                hours = t.mid(0, pos).toInt();
                pos1 = t.indexOf(':', pos + 1);
                minutes = t.mid(pos + 1, ((pos1 - 1) - pos)).toInt();
                second = t.right(t.length() - pos1 - 1).toInt();
                setValue(Value(QTime(hours, minutes, second)));
                if (value().asTime().isValid())    // Should always be the case for new docs
                    setUserInput(locale()->formatTime(value().asTime(), true));
                else { // This happens with old docs, when format is set wrongly to time
                    parseUserInput(pasteOperation(t, userInput(), op));
                }
            }

            else {
                // Set the cell's text
                setUserInput(pasteOperation(t, userInput(), op));
                setValue(Value(userInput()));
            }
        }
    }

    if (!sheet()->isLoading())
        parseUserInput(userInput());

    return true;
}

QTime Cell::toTime(const KoXmlElement &element)
{
    //TODO: can't we use tryParseTime (after modification) instead?
    QString t = element.text();
    t = t.trimmed();
    int hours = -1;
    int minutes = -1;
    int second = -1;
    int pos, pos1;
    pos = t.indexOf(':');
    hours = t.mid(0, pos).toInt();
    pos1 = t.indexOf(':', pos + 1);
    minutes = t.mid(pos + 1, ((pos1 - 1) - pos)).toInt();
    second = t.right(t.length() - pos1 - 1).toInt();
    setValue(Value(QTime(hours, minutes, second)));
    return value().asTime();
}

QDate Cell::toDate(const KoXmlElement &element)
{
    QString t = element.text();
    int pos;
    int pos1;
    int year = -1;
    int month = -1;
    int day = -1;
    pos = t.indexOf('/');
    year = t.mid(0, pos).toInt();
    pos1 = t.indexOf('/', pos + 1);
    month = t.mid(pos + 1, ((pos1 - 1) - pos)).toInt();
    day = t.right(t.length() - pos1 - 1).toInt();
    setValue(Value(QDate(year, month, day), sheet()->map()->calculationSettings()));
    return value().asDate(sheet()->map()->calculationSettings());
}

QString Cell::pasteOperation(const QString &new_text, const QString &old_text, Paste::Operation op)
{
    if (op == Paste::OverWrite)
        return new_text;

    QString tmp_op;
    QString tmp;
    QString old;

    if (!new_text.isEmpty() && new_text[0] == '=') {
        tmp = new_text.right(new_text.length() - 1);
    } else {
        tmp = new_text;
    }

    if (old_text.isEmpty() &&
            (op == Paste::Add || op == Paste::Mul || op == Paste::Sub || op == Paste::Div)) {
        old = "=0";
    }

    if (!old_text.isEmpty() && old_text[0] == '=') {
        old = old_text.right(old_text.length() - 1);
    } else {
        old = old_text;
    }

    bool b1, b2;
    tmp.toDouble(&b1);
    old.toDouble(&b2);
    if (b1 && !b2 && old.length() == 0) {
        old = '0';
        b2 = true;
    }

    if (b1 && b2) {
        switch (op) {
        case  Paste::Add:
            tmp_op = QString::number(old.toDouble() + tmp.toDouble());
            break;
        case Paste::Mul :
            tmp_op = QString::number(old.toDouble() * tmp.toDouble());
            break;
        case Paste::Sub:
            tmp_op = QString::number(old.toDouble() - tmp.toDouble());
            break;
        case Paste::Div:
            tmp_op = QString::number(old.toDouble() / tmp.toDouble());
            break;
        default:
            Q_ASSERT(0);
        }

        return tmp_op;
    } else if ((new_text[0] == '=' && old_text[0] == '=') ||
               (b1 && old_text[0] == '=') || (new_text[0] == '=' && b2)) {
        switch (op) {
        case Paste::Add :
            tmp_op = "=(" + old + ")+" + '(' + tmp + ')';
            break;
        case Paste::Mul :
            tmp_op = "=(" + old + ")*" + '(' + tmp + ')';
            break;
        case Paste::Sub:
            tmp_op = "=(" + old + ")-" + '(' + tmp + ')';
            break;
        case Paste::Div:
            tmp_op = "=(" + old + ")/" + '(' + tmp + ')';
            break;
        default :
            Q_ASSERT(0);
        }

        tmp_op = decodeFormula(tmp_op);
        return tmp_op;
    }

    tmp = decodeFormula(new_text);
    return tmp;
}

Cell& Cell::operator=(const Cell & other)
{
    d = other.d;
    return *this;
}

bool Cell::operator<(const Cell& other) const
{
    if (sheet() != other.sheet())
        return sheet() < other.sheet(); // pointers!
    if (row() < other.row())
        return true;
    return ((row() == other.row()) && (column() < other.column()));
}

bool Cell::operator==(const Cell& other) const
{
    return (row() == other.row() && column() == other.column() && sheet() == other.sheet());
}

bool Cell::operator!() const
{
    return (!d);   // isNull()
}

bool Cell::compareData(const Cell& other) const
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

QPoint Cell::cellPosition() const
{
    Q_ASSERT(!isNull());
    return QPoint(column(), row());
}
