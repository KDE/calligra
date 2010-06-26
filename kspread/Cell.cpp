/* This file is part of the KDE project
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

#include "CalculationSettings.h"
#include "CellStorage.h"
#include "Condition.h"
#include "Formula.h"
#include "GenValidationStyle.h"
#include "Global.h"
#include "Localization.h"
#include "LoadingInfo.h"
#include "Map.h"
#include "NamedAreaManager.h"
#include "OdfLoadingContext.h"
#include "OdfSavingContext.h"
#include "RowColumnFormat.h"
#include "ShapeApplicationData.h"
#include "Sheet.h"
#include "Style.h"
#include "StyleManager.h"
#include "Util.h"
#include "Value.h"
#include "Validity.h"
#include "ValueConverter.h"
#include "ValueFormatter.h"
#include "ValueParser.h"
#include "StyleStorage.h"

#include <KoShape.h>
#include <KoShapeLoadingContext.h>
#include <KoShapeRegistry.h>
#include <KoStyleStack.h>
#include <KoXmlNS.h>
#include <KoXmlReader.h>
#include <KoOdfStylesReader.h>
#include <KoXmlWriter.h>

#include <KoTextLoader.h>
#include <KoStyleManager.h>
#include <KoTextSharedLoadingData.h>
#include <KoTextDocument.h>
#include <KoTextWriter.h>
#include <KoEmbeddedDocumentSaver.h>
#include <KoParagraphStyle.h>

#include <kdebug.h>

#include <QTimer>
#include <QTextDocument>
#include <QTextCursor>

using namespace KSpread;

class Cell::Private : public QSharedData
{
public:
    Private() : sheet(0), column(0), row(0) {}

    Sheet*  sheet;
    uint    column  : 17; // KS_colMax
    uint    row     : 17; // KS_rowMax
};


Cell::Cell()
        : d(0)
{
}

Cell::Cell(const Sheet* sheet, int col, int row)
        : d(new Private)
{
    Q_ASSERT(sheet != 0);
    Q_ASSERT(1 <= col && col <= KS_colMax);
    Q_ASSERT(1 <= row && row <= KS_rowMax);
    d->sheet = const_cast<Sheet*>(sheet);
    d->column = col;
    d->row = row;
}

Cell::Cell(const Sheet* sheet, const QPoint& pos)
        : d(new Private)
{
    Q_ASSERT(sheet != 0);
    Q_ASSERT(1 <= pos.x() && pos.x() <= KS_colMax);
    Q_ASSERT(1 <= pos.y() && pos.y() <= KS_rowMax);
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
    if (value() != Value())
        return false;
    if (formula() != Formula())
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
    if (formula() != Formula())
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

    column--;

    for (unsigned limit = 26; column >= limit + offset; limit *= 26, digits++)
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
        setFormula(Formula());
        // set the value
        sheet()->cellStorage()->setUserInput(d->column, d->row, string);
    }

    if (old != string) {
        // remove any existing richtext
        setRichText(QSharedPointer<QTextDocument>());
    }
}


// Return the out text, i.e. the text that is visible in the cells
// square when shown.  This could, for instance, be the calculated
// result of a formula.
//
QString Cell::displayText() const
{
    if (isNull())
        return QString();

    QString string;
    const Style style = effectiveStyle();
    // Display a formula if warranted.  If not, display the value instead;
    // this is the most common case.
    if (isFormula() && sheet()->getShowFormula() && !(sheet()->isProtected() && style.hideFormula())) {
        string = userInput();
    } else if (!isEmpty()) {
        string = sheet()->map()->formatter()->formatText(value(), style.formatType(), style.precision(),
                 style.floatFormat(), style.prefix(),
                 style.postfix(), style.currency().symbol()).asString();
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
        kDebug(36004) << "needsPrinting: Has background color";
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
    kDebug() << result;
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
                kDebug(36003) << "Cell::decodeFormula: row or column out of range (col:" << col << " | row:" << row << ')';
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
//   kDebug(36002) ;

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
    double height = 0.0;
    for (int row = d->row; row <= bottomRow; ++row)
        height += sheet()->rowFormat(row)->height();
    return height;
}

// parses the text
void Cell::parseUserInput(const QString& text)
{
//   kDebug() ;

    // empty string?
    if (text.isEmpty()) {
        setValue(Value::empty());
        setUserInput(text);
        setFormula(Formula());
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
            kDebug() << "Validation failed";
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
    cell.setAttribute("row", row() - yOffset);
    cell.setAttribute("column", column() - xOffset);

    //
    // Save the formatting information
    //
    QDomElement formatElement(doc.createElement("format"));
    style().saveXML(doc, formatElement, sheet()->map()->styleManager());
    if (formatElement.hasChildNodes() || formatElement.attributes().length())   // don't save empty tags
        cell.appendChild(formatElement);

    if (doesMergeCells()) {
        if (mergedXCells())
            formatElement.setAttribute("colspan", mergedXCells());
        if (mergedYCells())
            formatElement.setAttribute("rowspan", mergedYCells());
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
            // Have to be saved in some CDATA section because of too many special charatcers.
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

void Cell::saveOdfAnnotation(KoXmlWriter &xmlwriter)
{
    const QString comment = this->comment();
    if (!comment.isEmpty()) {
        //<office:annotation draw:style-name="gr1" draw:text-style-name="P1" svg:width="2.899cm" svg:height="2.691cm" svg:x="2.858cm" svg:y="0.001cm" draw:caption-point-x="-2.858cm" draw:caption-point-y="-0.001cm">
        xmlwriter.startElement("office:annotation");
        const QStringList text = comment.split('\n', QString::SkipEmptyParts);
        for (QStringList::ConstIterator it = text.begin(); it != text.end(); ++it) {
            xmlwriter.startElement("text:p");
            xmlwriter.addTextNode(*it);
            xmlwriter.endElement();
        }
        xmlwriter.endElement();
    }
}

QString Cell::saveOdfCellStyle(KoGenStyle &currentCellStyle, KoGenStyles &mainStyles)
{
    const Conditions conditions = this->conditions();
    if (!conditions.isEmpty()) {
        // this has to be an automatic style
        currentCellStyle = KoGenStyle(KoGenStyle::TableCellAutoStyle, "table-cell");
        conditions.saveOdfConditions(currentCellStyle, sheet()->map()->converter());
    }
    return style().saveOdf(currentCellStyle, mainStyles, d->sheet->map()->styleManager());
}


bool Cell::saveOdf(KoXmlWriter& xmlwriter, KoGenStyles &mainStyles,
                   int row, int column, int &repeated,
                   OdfSavingContext& tableContext)
{
    // see: OpenDocument, 8.1.3 Table Cell
    if (!isPartOfMerged())
        xmlwriter.startElement("table:table-cell");
    else
        xmlwriter.startElement("table:covered-table-cell");
#if 0
    //add font style
    QFont font;
    Value const value(cell.value());
    if (!cell.isDefault()) {
        font = cell.format()->textFont(i, row);
        m_styles.addFont(font);

        if (cell.format()->hasProperty(Style::SComment))
            hasComment = true;
    }
#endif
    // NOTE save the value before the style as long as the Formatter does not work correctly
    if (link().isEmpty())
        saveOdfValue(xmlwriter);

    // Either there's no column and row default and the style's not the default style,
    // or the style is different to one of them. The row default takes precedence.
    if ((!tableContext.rowDefaultStyles.contains(row) &&
            !tableContext.columnDefaultStyles.contains(column) &&
            !(style().isDefault() && conditions().isEmpty())) ||
            (tableContext.rowDefaultStyles.contains(row) && tableContext.rowDefaultStyles[row] != style()) ||
            (tableContext.columnDefaultStyles.contains(column) && tableContext.columnDefaultStyles[column] != style())) {
        KoGenStyle currentCellStyle; // the type determined in saveOdfCellStyle
        saveOdfCellStyle(currentCellStyle, mainStyles);
        // skip 'table:style-name' attribute for the default style
        if (!currentCellStyle.isDefaultStyle()) {
            if (mainStyles.styles().contains(currentCellStyle))
                xmlwriter.addAttribute("table:style-name", mainStyles.styles().find(currentCellStyle).value());
        }
    }

    // group empty cells with the same style
    const QString comment = this->comment();
    if (isEmpty() && comment.isEmpty() && !isPartOfMerged() && !doesMergeCells() &&
            !tableContext.cellHasAnchoredShapes(sheet(), row, column)) {
        bool refCellIsDefault = isDefault();
        int j = column + 1;
        Cell nextCell = sheet()->cellStorage()->nextInRow(column, row);
        while (!nextCell.isNull()) {
            // if
            //   the next cell is not the adjacent one
            // or
            //   the next cell is not empty
            if (nextCell.column() != j || (!nextCell.isEmpty() || tableContext.cellHasAnchoredShapes(sheet(), row, column))) {
                if (refCellIsDefault) {
                    // if the origin cell was a default cell,
                    // we count the default cells
                    repeated = nextCell.column() - j + 1;
                }
                // otherwise we just stop here to process the adjacent
                // cell in the next iteration of the outer loop
                // (in Sheet::saveOdfCells)
                break;
            }

            if (nextCell.isPartOfMerged() || nextCell.doesMergeCells() ||
                    !nextCell.comment().isEmpty() || tableContext.cellHasAnchoredShapes(sheet(), row, column) ||
                    !(nextCell.style() == style() && nextCell.conditions() == conditions())) {
                break;
            }
            ++repeated;
            // get the next cell and set the index to the adjacent cell
            nextCell = sheet()->cellStorage()->nextInRow(j++, row);
        }
        kDebug(36003) << "Cell::saveOdf: empty cell in column" << column
        << "repeated" << repeated << "time(s)" << endl;

        if (repeated > 1)
            xmlwriter.addAttribute("table:number-columns-repeated", QString::number(repeated));
    }

    Validity validity = Cell(sheet(), column, row).validity();
    if (!validity.isEmpty()) {
        GenValidationStyle styleVal(&validity, sheet()->map()->converter());
        xmlwriter.addAttribute("table:validation-name", tableContext.valStyle.insert(styleVal));
    }
    if (isFormula()) {
        //kDebug(36003) <<"Formula found";
        QString formula = Odf::encodeFormula(userInput(), locale());
        xmlwriter.addAttribute("table:formula", formula);
    } else if (!link().isEmpty()) {
        //kDebug(36003)<<"Link found";
        xmlwriter.startElement("text:p");
        xmlwriter.startElement("text:a");
        const QString url = link();
        //Reference cell is started by '#'
        if (Util::localReferenceAnchor(url))
            xmlwriter.addAttribute("xlink:href", ('#' + url));
        else
            xmlwriter.addAttribute("xlink:href", url);
        xmlwriter.addTextNode(userInput());
        xmlwriter.endElement();
        xmlwriter.endElement();
    }

    if (doesMergeCells()) {
        int colSpan = mergedXCells() + 1;
        int rowSpan = mergedYCells() + 1;

        if (colSpan > 1)
            xmlwriter.addAttribute("table:number-columns-spanned", QString::number(colSpan));

        if (rowSpan > 1)
            xmlwriter.addAttribute("table:number-rows-spanned", QString::number(rowSpan));
    }

    if (!isEmpty() && link().isEmpty()) {
        QSharedPointer<QTextDocument> doc = richText();
        if (doc) {
            QTextCharFormat format = style().asCharFormat();
            sheet()->map()->textStyleManager()->defaultParagraphStyle()->characterStyle()->copyProperties(format);

            KoEmbeddedDocumentSaver saver;
            KoShapeSavingContext shapeContext(xmlwriter, mainStyles, saver);
            KoTextWriter writer(shapeContext);

            writer.write(doc.data(), 0);
        } else {
            xmlwriter.startElement("text:p");
            xmlwriter.addTextNode(displayText().toUtf8());
            xmlwriter.endElement();
        }
    }

    // flake
    // Save shapes that are anchored to this cell.
    // see: OpenDocument, 2.3.1 Text Documents
    // see: OpenDocument, 9.2 Drawing Shapes
    if (tableContext.cellHasAnchoredShapes(sheet(), row, column)) {
        const QList<KoShape*> shapes = tableContext.cellAnchoredShapes(sheet(), row, column);
        for (int i = 0; i < shapes.count(); ++i) {
            KoShape* const shape = shapes[i];
            const QPointF bottomRight = shape->boundingRect().bottomRight();
            double endX = 0.0;
            double endY = 0.0;
            const int col = sheet()->leftColumn(bottomRight.x(), endX);
            const int row = sheet()->topRow(bottomRight.y(), endY);
            shape->setAdditionalAttribute("table:end-cell-address", Cell(sheet(), col, row).name());
            shape->setAdditionalAttribute("table:end-x", QString::number(bottomRight.x() - endX));
            shape->setAdditionalAttribute("table:end-y", QString::number(bottomRight.y() - endY));
            shapes[i]->saveOdf(tableContext.shapeContext);
            shape->removeAdditionalAttribute("table:end-cell-address");
            shape->removeAdditionalAttribute("table:end-x");
            shape->removeAdditionalAttribute("table:end-y");
        }
    }

    saveOdfAnnotation(xmlwriter);

    xmlwriter.endElement();
    return true;
}

void Cell::saveOdfValue(KoXmlWriter &xmlWriter)
{
    switch (value().format()) {
    case Value::fmt_None: break;  //NOTHING HERE
    case Value::fmt_Boolean: {
        xmlWriter.addAttribute("office:value-type", "boolean");
        xmlWriter.addAttribute("office:boolean-value", (value().asBoolean() ?
                               "true" : "false"));
        break;
    }
    case Value::fmt_Number: {
        if (isDate()) {
            xmlWriter.addAttribute("office:value-type", "date");
            xmlWriter.addAttribute("office:date-value",
                                   value().asDate(sheet()->map()->calculationSettings()).toString(Qt::ISODate));
        } else if (isText()) {
            xmlWriter.addAttribute("office:value-type", "string");
            if (value().isInteger())
                xmlWriter.addAttribute("office:string-value", QString::number(value().asInteger()));
            else
                xmlWriter.addAttribute("office:string-value", QString::number(numToDouble(value().asFloat()), 'g', DBL_DIG));
        } else {
            xmlWriter.addAttribute("office:value-type", "float");
            if (value().isInteger())
                xmlWriter.addAttribute("office:value", QString::number(value().asInteger()));
            else
                xmlWriter.addAttribute("office:value", QString::number(numToDouble(value().asFloat()), 'g', DBL_DIG));
        }
        break;
    }
    case Value::fmt_Percent: {
        xmlWriter.addAttribute("office:value-type", "percentage");
        xmlWriter.addAttribute("office:value",
                               QString::number((double) numToDouble(value().asFloat())));
        break;
    }
    case Value::fmt_Money: {
        xmlWriter.addAttribute("office:value-type", "currency");
        const Style style = this->style();
        if (style.hasAttribute(Style::CurrencyFormat)) {
            Currency currency = style.currency();
            xmlWriter.addAttribute("office:currency", currency.code());
        }
        xmlWriter.addAttribute("office:value", QString::number((double) numToDouble(value().asFloat())));
        break;
    }
    case Value::fmt_DateTime: break;  //NOTHING HERE
    case Value::fmt_Date: {
        if (isTime()) {
            xmlWriter.addAttribute("office:value-type", "time");
            xmlWriter.addAttribute("office:time-value",
                                   value().asTime(sheet()->map()->calculationSettings()).toString("PThhHmmMssS"));
        } else {
            xmlWriter.addAttribute("office:value-type", "date");
            xmlWriter.addAttribute("office:date-value",
                                   value().asDate(sheet()->map()->calculationSettings()).toString(Qt::ISODate));
        }
        break;
    }
    case Value::fmt_Time: {
        xmlWriter.addAttribute("office:value-type", "time");
        xmlWriter.addAttribute("office:time-value",
                               value().asTime(sheet()->map()->calculationSettings()).toString("PThhHmmMssS"));
        break;
    }
    case Value::fmt_String: {
        xmlWriter.addAttribute("office:value-type", "string");
        xmlWriter.addAttribute("office:string-value", value().asString());
        break;
    }
    };
}

bool Cell::loadOdf(const KoXmlElement& element, OdfLoadingContext& tableContext)
{
    //Search and load each paragraph of text. Each paragraph is separated by a line break.
    loadOdfCellText(element, tableContext);

    //
    // formula
    //
    bool isFormula = false;
    if (element.hasAttributeNS(KoXmlNS::table, "formula")) {
        kDebug(36003) << "cell:" << name() << "formula :" << element.attributeNS(KoXmlNS::table, "formula", QString());
        isFormula = true;
        QString oasisFormula(element.attributeNS(KoXmlNS::table, "formula", QString()));
        // each spreadsheet application likes to safe formulas with a different namespace
        // prefix, so remove all of them
        QStringList prefixes = QStringList() << "oooc:" << "kspr:" << "of:" << "msoxl:";
        QString namespacePrefix;
        foreach(const QString &prefix, prefixes) {
            if (oasisFormula.startsWith(prefix)) {
                oasisFormula = oasisFormula.mid(prefix.length());
                namespacePrefix = prefix;
                break;
            }
        }
        oasisFormula = Odf::decodeFormula(oasisFormula, locale(), namespacePrefix);
        setUserInput(oasisFormula);
    } else if (!userInput().isEmpty() && userInput().at(0) == '=')  //prepend ' to the text to avoid = to be painted
        setUserInput(userInput().prepend('\''));

    //
    // validation
    //
    if (element.hasAttributeNS(KoXmlNS::table, "validation-name")) {
        const QString validationName = element.attributeNS(KoXmlNS::table, "validation-name", QString());
        kDebug(36003) << "cell:" << name() << "validation-name:" << validationName;
        Validity validity;
        validity.loadOdfValidation(this, validationName, tableContext);
        if (!validity.isEmpty())
            setValidity(validity);
    }

    //
    // value type
    //
    if (element.hasAttributeNS(KoXmlNS::office, "value-type")) {
        const QString valuetype = element.attributeNS(KoXmlNS::office, "value-type", QString());
        kDebug(36003) << "cell:" << name() << "value-type:" << valuetype;
        if (valuetype == "boolean") {
            const QString val = element.attributeNS(KoXmlNS::office, "boolean-value", QString()).toLower();
            if ((val == "true") || (val == "false"))
                setValue(Value(val == "true"));
        }

        // integer and floating-point value
        else if (valuetype == "float") {
            bool ok = false;
            Value value(element.attributeNS(KoXmlNS::office, "value", QString()).toDouble(&ok));
            if (ok) {
                value.setFormat(Value::fmt_Number);
                setValue(value);
#if 0
                Style style;
                style.setFormatType(Format::Number);
                setStyle(style);
#endif
            }
            // always set the userInput to the actual value read from the cell, and not whatever happens to be set as text, as the textual representation of a value may be less accurate than the value itself
            if (!isFormula)
                setUserInput(sheet()->map()->converter()->asString(value).asString());
        }

        // currency value
        else if (valuetype == "currency") {
            bool ok = false;
            Value value(element.attributeNS(KoXmlNS::office, "value", QString()).toDouble(&ok));
            if (ok) {
                value.setFormat(Value::fmt_Money);
                setValue(value);

                Currency currency;
                if (element.hasAttributeNS(KoXmlNS::office, "currency")) {
                    currency = Currency(element.attributeNS(KoXmlNS::office, "currency", QString()));
                }

                if( style().isEmpty() ) {
                    Style style;
                    style.setCurrency(currency);
                    setStyle(style);
                }
            }
        } else if (valuetype == "percentage") {
            bool ok = false;
            Value value(element.attributeNS(KoXmlNS::office, "value", QString()).toDouble(&ok));
            if (ok) {
                value.setFormat(Value::fmt_Percent);
                setValue(value);
                if (!isFormula && userInput().isEmpty())
                    setUserInput(sheet()->map()->converter()->asString(value).asString());
// FIXME Stefan: Should be handled by Value::Format. Verify and remove!
#if 0
                Style style;
                style.setFormatType(Format::Percentage);
                setStyle(style);
#endif
            }
        } else if (valuetype == "date") {
            QString value = element.attributeNS(KoXmlNS::office, "date-value", QString());

            // "1980-10-15" or "2001-01-01T19:27:41"
            int year = 0, month = 0, day = 0, hours = 0, minutes = 0, seconds = 0;
            bool hasTime = false;
            bool ok = false;

            int p1 = value.indexOf('-');
            if (p1 > 0) {
                year  = value.left(p1).toInt(&ok);
                if (ok) {
                    int p2 = value.indexOf('-', ++p1);
                    month = value.mid(p1, p2 - p1).toInt(&ok);
                    if (ok) {
                        // the date can optionally have a time attached
                        int p3 = value.indexOf('T', ++p2);
                        if (p3 > 0) {
                            hasTime = true;
                            day = value.mid(p2, p3 - p2).toInt(&ok);
                            if (ok) {
                                int p4 = value.indexOf(':', ++p3);
                                hours = value.mid(p3, p4 - p3).toInt(&ok);
                                if (ok) {
                                    int p5 = value.indexOf(':', ++p4);
                                    minutes = value.mid(p4, p5 - p4).toInt(&ok);
                                    if (ok)
                                        seconds = value.right(value.length() - p5 - 1).toInt(&ok);
                                }
                            }
                        } else {
                            day = value.right(value.length() - p2).toInt(&ok);
                        }
                    }
                }
            }

            if (ok) {
                if (hasTime)
                    setValue(Value(QDateTime(QDate(year, month, day), QTime(hours, minutes, seconds)), sheet()->map()->calculationSettings()));
                else
                    setValue(Value(QDate(year, month, day), sheet()->map()->calculationSettings()));
// FIXME Stefan: Should be handled by Value::Format. Verify and remove!
//Sebsauer: Fixed now. Value::Format handles it correct.
#if 0
                Style s;
                s.setFormatType(Format::ShortDate);
                setStyle(s);
#endif
                kDebug(36003) << "cell:" << name() << "Type: date, value:" << value << "Date:" << year << " -" << month << " -" << day;
            }
        } else if (valuetype == "time") {
            QString value = element.attributeNS(KoXmlNS::office, "time-value", QString());

            // "PT15H10M12S"
            int hours = 0, minutes = 0, seconds = 0;
            int l = value.length();
            QString num;
            bool ok = false;
            for (int i = 0; i < l; ++i) {
                if (value[i].isNumber()) {
                    num += value[i];
                    continue;
                } else if (value[i] == 'H')
                    hours   = num.toInt(&ok);
                else if (value[i] == 'M')
                    minutes = num.toInt(&ok);
                else if (value[i] == 'S')
                    seconds = num.toInt(&ok);
                else
                    continue;
                //kDebug(36003) << "Num:" << num;
                num.clear();
                if (!ok)
                    break;
            }

            if (ok) {
                // Value kval( timeToNum( hours, minutes, seconds ) );
                // cell.setValue( kval );
                setValue(Value(QTime(hours % 24, minutes, seconds), sheet()->map()->calculationSettings()));
// FIXME Stefan: Should be handled by Value::Format. Verify and remove!
#if 0
                Style style;
                style.setFormatType(Format::Time);
                setStyle(style);
#endif
                kDebug(36003) << "cell:" << name() << "Type: time:" << value << "Hours:" << hours << "," << minutes << "," << seconds;
            }
        } else if (valuetype == "string") {
            if (element.hasAttributeNS(KoXmlNS::office, "string-value")) {
                QString value = element.attributeNS(KoXmlNS::office, "string-value", QString());
                setValue(Value(value));
            } else {
                // use the paragraph(s) read in before
                setValue(Value(userInput()));
            }
// FIXME Stefan: Should be handled by Value::Format. Verify and remove!
#if 0
            Style style;
            style.setFormatType(Format::Text);
            setStyle(style);
#endif
        } else {
            kDebug(36003) << "cell:" << name() << "  Unknown type. Parsing user input.";
            // Set the value by parsing the user input.
            parseUserInput(userInput());
        }
    } else { // no value-type attribute
        kDebug(36003) << "cell:" << name() << "  No value type specified. Parsing user input.";
        // Set the value by parsing the user input.
        parseUserInput(userInput());
    }

    //
    // merged cells ?
    //
    int colSpan = 1;
    int rowSpan = 1;
    if (element.hasAttributeNS(KoXmlNS::table, "number-columns-spanned")) {
        bool ok = false;
        int span = element.attributeNS(KoXmlNS::table, "number-columns-spanned", QString()).toInt(&ok);
        if (ok) colSpan = span;
    }
    if (element.hasAttributeNS(KoXmlNS::table, "number-rows-spanned")) {
        bool ok = false;
        int span = element.attributeNS(KoXmlNS::table, "number-rows-spanned", QString()).toInt(&ok);
        if (ok) rowSpan = span;
    }
    if (colSpan > 1 || rowSpan > 1)
        mergeCells(d->column, d->row, colSpan - 1, rowSpan - 1);

    //
    // cell comment/annotation
    //
    KoXmlElement annotationElement = KoXml::namedItemNS(element, KoXmlNS::office, "annotation");
    if (!annotationElement.isNull()) {
        QString comment;
        KoXmlNode node = annotationElement.firstChild();
        while (!node.isNull()) {
            KoXmlElement commentElement = node.toElement();
            if (!commentElement.isNull())
                if (commentElement.localName() == "p" && commentElement.namespaceURI() == KoXmlNS::text) {
                    if (!comment.isEmpty()) comment.append('\n');
                    comment.append(commentElement.text());
                }

            node = node.nextSibling();
        }
        if (!comment.isEmpty())
            setComment(comment);
    }

    loadOdfObjects(element, tableContext.odfContext);

    return true;
}

// Similar to KoXml::namedItemNS except that children of span tags will be evaluated too.
KoXmlElement namedItemNSWithSpan(const KoXmlNode& node, const char* nsURI, const char* localName)
{
    KoXmlNode n = node.firstChild();
    for (; !n.isNull(); n = n.nextSibling()) {
        if (n.isElement()) {
            if (n.localName() == localName && n.namespaceURI() == nsURI) {
                return n.toElement();
            }
            if (n.localName() == "span" && n.namespaceURI() == nsURI) {
                KoXmlElement e = KoXml::namedItemNS(n, nsURI, localName); // not recursive
                if (!e.isNull()) {
                    return e;
                }
            }
        }
    }
    return KoXmlElement();
}

// recursively goes through all children of parent and returns true if there is any element
// in the draw: namespace in this subtree
static bool findDrawElements(const KoXmlElement& parent)
{
    KoXmlElement element;
    forEachElement(element , parent) {
        if (element.namespaceURI() == KoXmlNS::draw)
            return true;
        if (findDrawElements(element))
            return true;
    }
    return false;
}

void Cell::loadOdfCellText(const KoXmlElement& parent, OdfLoadingContext& tableContext)
{
    //Search and load each paragraph of text. Each paragraph is separated by a line break
    KoXmlElement textParagraphElement;
    QString cellText;

    bool multipleTextParagraphsFound = false;
    const bool hasRichText = KoTextLoader::containsRichText(parent);

    forEachElement(textParagraphElement , parent) {
        if (textParagraphElement.localName() == "p" &&
                textParagraphElement.namespaceURI() == KoXmlNS::text) {

            // our text, could contain formating for value or result of formul
            if (cellText.isEmpty()) {
                cellText = textParagraphElement.text();
            } else {
                cellText += '\n' + textParagraphElement.text();
                multipleTextParagraphsFound = true;
            }

            // the text:a link could be located within a text:span element
            KoXmlElement textA = namedItemNSWithSpan(textParagraphElement, KoXmlNS::text, "a");
            if (!textA.isNull()) {
                if (textA.hasAttributeNS(KoXmlNS::xlink, "href")) {
                    QString link = textA.attributeNS(KoXmlNS::xlink, "href", QString());
                    cellText = textA.text();
                    setUserInput(cellText);
                    // The value will be set later in loadOdf().
                    if ((!link.isEmpty()) && (link[0] == '#'))
                        link = link.remove(0, 1);
                    setLink(link);
                }
            }
        }
    }

    if (!cellText.isNull()) {
        setUserInput(cellText);
        // The value will be set later in loadOdf().

        if (hasRichText && !findDrawElements(parent)) {
            // for now we don't support richtext and embedded shapes in the same cell;
            // this is because they would currently be loaded twice, once by the KoTextLoader
            // and later properly by the cell itself

            QTextCharFormat format = style().asCharFormat();
            sheet()->map()->textStyleManager()->defaultParagraphStyle()->characterStyle()->copyProperties(format);

            KoTextLoader loader(*tableContext.shapeContext);
            QSharedPointer<QTextDocument> doc(new QTextDocument);
            KoTextDocument(doc.data()).setStyleManager(sheet()->map()->textStyleManager());

            QTextCursor cursor(doc.data());
            loader.loadBody(parent, cursor);

            setUserInput(doc->toPlainText());
            setRichText(doc);
        }
    }

    //Enable word wrapping if multiple lines of text have been found.
    if (multipleTextParagraphsFound) {
        Style style;
        style.setWrapText(true);
        setStyle(style);
    }
}

void Cell::loadOdfObjects(const KoXmlElement &parent, KoOdfLoadingContext& odfContext)
{
    // Register additional attributes, that identify shapes anchored in cells.
    // Their dimensions need adjustment after all rows are loaded,
    // because the position of the end cell is not always known yet.
    KoShapeLoadingContext::addAdditionalAttributeData(KoShapeLoadingContext::AdditionalAttributeData(
                KoXmlNS::table, "end-cell-address",
                "table:end-cell-address"));
    KoShapeLoadingContext::addAdditionalAttributeData(KoShapeLoadingContext::AdditionalAttributeData(
                KoXmlNS::table, "end-x",
                "table:end-x"));
    KoShapeLoadingContext::addAdditionalAttributeData(KoShapeLoadingContext::AdditionalAttributeData(
                KoXmlNS::table, "end-y",
                "table:end-y"));

    KoShapeLoadingContext shapeContext(odfContext, d->sheet->resourceManager());
    KoXmlElement element;
    forEachElement(element, parent) {
        if (element.namespaceURI() != KoXmlNS::draw)
            continue;

        KoShape* shape = KoShapeRegistry::instance()->createShapeFromOdf(element, shapeContext);
        if (!shape) {
            kDebug(36003) << "Unable to load shape.";
            continue;
        }

        d->sheet->addShape(shape);

        // All three attributes are necessary for cell anchored shapes.
        // Otherwise, they are anchored in the sheet.
        if (!shape->hasAdditionalAttribute("table:end-cell-address") ||
                !shape->hasAdditionalAttribute("table:end-x") ||
                !shape->hasAdditionalAttribute("table:end-y")) {
            kDebug(36003) << "Not all attributes found, that are necessary for cell anchoring.";
            continue;
        }

        Region endCell(Region::loadOdf(shape->additionalAttribute("table:end-cell-address")),
                       d->sheet->map(), d->sheet);
        if (!endCell.isValid() || !endCell.isSingular())
            continue;

        QString string = shape->additionalAttribute("table:end-x");
        if (string.isNull())
            continue;
        double endX = KoUnit::parseValue(string);

        string = shape->additionalAttribute("table:end-y");
        if (string.isNull())
            continue;
        double endY = KoUnit::parseValue(string);

        // The position is relative to the upper left sheet corner until now. Move it.
        QPointF position = shape->position();
        // Remember how far we're off from the top-left corner of this cell
        double offsetX = position.x();
        double offsetY = position.y();
        for (int col = 1; col < column(); ++col)
            position += QPointF(d->sheet->columnFormat(col)->width(), 0.0);
        for (int row = 1; row < this->row(); ++row)
            position += QPointF(0.0, d->sheet->rowFormat(row)->height());
        shape->setPosition(position);

        // The column dimensions are already the final ones, but not the row dimensions.
        // The default height is used for the not yet loaded rows.
        // TODO Stefan: Honor non-default row heights later!
        // subtract offset because the accumulated width and height we calculate below starts
        // at the top-left corner of this cell, but the shape can have an offset to that corner
        QSizeF size = QSizeF(endX - offsetX, endY - offsetY);
        for (int col = column(); col < endCell.firstRange().left(); ++col)
            size += QSizeF(d->sheet->columnFormat(col)->width(), 0.0);
        for (int row = this->row(); row < endCell.firstRange().top(); ++row)
            size += QSizeF(0.0, d->sheet->rowFormat(row)->height());
        shape->setSize(size);

        dynamic_cast<ShapeApplicationData*>(shape->applicationData())->setAnchoredToCell(true);
    }
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
        kDebug(36001) << "Cell::load: Value out of range Cell:row=" << d->row;
        return false;
    }
    if (d->column < 1 || d->column > KS_colMax) {
        kDebug(36001) << "Cell::load: Value out of range Cell:column=" << d->column;
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
                kDebug(36001) << "Value out of range Cell::colspan=" << i;
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
                kDebug(36001) << "Value out of range Cell::rowspan=" << i;
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

            bool clear = true;
            // boolean ?
            if (dataType == "Bool") {
                if (t == "false")
                    setValue(Value(false));
                else if (t == "true")
                    setValue(Value(true));
                else
                    clear = false;
            } else if (dataType == "Num") {
                bool ok = false;
                double dd = t.toDouble(&ok);
                if (ok)
                    setValue(Value(dd));
                else
                    clear = false;
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
                    else
                        clear = false;
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
                        setValue(Value(time, sheet()->map()->calculationSettings()));
                    else
                        clear = false;
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
        // extract the link and the correspoding text
        // This is a rather dirty hack, but enough for KSpread generated XML
        bool inside_tag = false;
        QString qml_text;
        QString tag;
        QString qml_link;

        for (int i = 1; i < t.length(); i++) {
            QChar ch = t[i];
            if (ch == '<') {
                if (!inside_tag) {
                    inside_tag = true;
                    tag.clear();
                }
            } else if (ch == '>') {
                if (inside_tag) {
                    inside_tag = false;
                    if (tag.startsWith("a href=\"", Qt::CaseSensitive))
                        if (tag.endsWith('"'))
                            qml_link = tag.mid(8, tag.length() - 9);
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
                    kWarning(36001) << "Couldn't parse '" << t << "' as number.";
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
                setValue(Value(QTime(hours, minutes, second), sheet()->map()->calculationSettings()));
                if (value().asTime(sheet()->map()->calculationSettings()).isValid())    // Should always be the case for new docs
                    setUserInput(locale()->formatTime(value().asTime(sheet()->map()->calculationSettings()), true));
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
    setValue(Value(QTime(hours, minutes, second), sheet()->map()->calculationSettings()));
    return value().asTime(sheet()->map()->calculationSettings());
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
