/* This file is part of the KDE project
   SPDX-FileCopyrightText: 2010 Marijn Kruisselbrink <mkruisselbrink@kde.org>
   SPDX-FileCopyrightText: 2006-2007 Stefan Nikolaus <stefan.nikolaus@kdemail.net>
   SPDX-FileCopyrightText: 2005 Raphael Langerhorst <raphael.langerhorst@kdemail.net>
   SPDX-FileCopyrightText: 2004-2005, 2021 Tomas Mecir <mecirt@gmail.com>
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
#include "CellBase.h"
#include "CellBaseStorage.h"
#include "calligra_sheets_limits.h"
#include "Formula.h"
#include "MapBase.h"
#include "SheetBase.h"
#include "Validity.h"
#include "Value.h"
#include "ValueConverter.h"
#include "ValueParser.h"


using namespace Calligra::Sheets;

class Q_DECL_HIDDEN CellBase::Private : public QSharedData
{
public:
    Private() : sheet(0), column(0), row(0) {}

    SheetBase*  sheet;
    unsigned int    column;
    unsigned int    row;
};


CellBase::CellBase()
        : d(0)
{
}

CellBase::CellBase(SheetBase* sheet, unsigned int col, unsigned int row)
        : d(new Private)
{
    Q_ASSERT(sheet != 0);
    Q_ASSERT_X(1 <= col && col <= KS_colMax, __FUNCTION__, QString("%1 out of bounds").arg(col).toLocal8Bit());
    Q_ASSERT_X(1 <= row && row <= KS_rowMax, __FUNCTION__, QString("%1 out of bounds").arg(row).toLocal8Bit());
    d->sheet = sheet;
    d->column = col;
    d->row = row;
}

CellBase::CellBase(SheetBase* sheet, const QPoint& pos)
        : d(new Private)
{
    Q_ASSERT(sheet != 0);
    Q_ASSERT_X(1 <= pos.x() && pos.x() <= KS_colMax, __FUNCTION__, QString("%1 out of bounds").arg(pos.x()).toLocal8Bit());
    Q_ASSERT_X(1 <= pos.y() && pos.y() <= KS_rowMax, __FUNCTION__, QString("%1 out of bounds").arg(pos.y()).toLocal8Bit());
    d->sheet = sheet;
    d->column = pos.x();
    d->row = pos.y();
}

CellBase::CellBase(const CellBase& other)
        : d(other.d)
{
}

CellBase::~CellBase()
{
}

bool CellBase::isNull() const
{
    return (!d);
}

// Return the sheet that this cell belongs to.
SheetBase* CellBase::sheet() const
{
    Q_ASSERT(!isNull());
    return d->sheet;
}

// Return the column number of this cell.
int CellBase::column() const
{
    // Make sure this isn't called for the null cell.  This assert
    // can save you (could have saved me!) the hassle of some very
    // obscure bugs.
    Q_ASSERT(!isNull());
    Q_ASSERT(1 <= d->column);   //&& d->column <= KS_colMax );
    return d->column;
}

// Return the row number of this cell.
int CellBase::row() const
{
    // Make sure this isn't called for the null cell.  This assert
    // can save you (could have saved me!) the hassle of some very
    // obscure bugs.
    Q_ASSERT(!isNull());
    Q_ASSERT(1 <= d->row);   //&& d->row <= KS_rowMax );
    return d->row;
}

QPoint CellBase::cellPosition() const
{
    Q_ASSERT(!isNull());
    return QPoint(column(), row());
}

// Return the name of this cell, i.e. the string that the user would
// use to reference it.  Example: A1, BZ16
QString CellBase::name() const
{
    return name(column(), row());
}

// Return the name of any cell given by (col, row).
// static
QString CellBase::name(int col, int row)
{
    return columnName(col) + QString::number(row);
}

// Return the name of this cell, including the sheet name.
// Example: sheet1!A5
QString CellBase::fullName() const
{
    return fullName(sheet(), column(), row());
}

// Return the full name of any cell given a sheet and (col, row).
// static
QString CellBase::fullName(const SheetBase *s, int col, int row)
{
    return s->sheetName() + '!' + name(col, row);
}

// Return the symbolic name of the column of this cell.  Examples: A, BB.
QString CellBase::columnName() const
{
    return columnName(column());
}



// Return the value of this cell.
const Value CellBase::value() const
{
    return sheet()->cellStorage()->value(d->column, d->row);
}


// Set the value of this cell.
void CellBase::setValue(const Value& value)
{
    sheet()->cellStorage()->setValue(d->column, d->row, value);
}

Formula CellBase::formula() const
{
    return sheet()->cellStorage()->formula(d->column, d->row);
}

void CellBase::setFormula(const Formula& formula)
{
    sheet()->cellStorage()->setFormula(column(), row(), formula);
    // Also set the existing value - this is needed for undo to receive the correct data
    // The error will be cleared as soon as dependency updates run
    if (!formula.isEmpty()) setValue(Value::errorDEPEND());
}

void CellBase::setCellValue(const Value &value)
{
    // remove an existing formula
    setFormula(Formula::empty());
    setValue(value);
    QString str = sheet()->map()->converter()->asString(value).asString();
    sheet()->cellStorage()->setUserInput(column(), row(), str);
}

QString CellBase::userInput() const
{
    const Formula formula = this->formula();
    if (!formula.expression().isEmpty())
        return formula.expression();
    return sheet()->cellStorage()->userInput(d->column, d->row);
}

void CellBase::setUserInput(const QString& string)
{
    if (!string.isEmpty() && (string[0] == '=')) {
        // set the formula
        Formula formula(sheet(), *this);
        formula.setExpression(string);
        setFormula(formula);
        // remove an existing user input (the non-formula one)
        sheet()->cellStorage()->setUserInput(column(), row(), QString());
    } else {
        // remove an existing formula
        setFormula(Formula::empty());
        // set the value
        sheet()->cellStorage()->setUserInput(column(), row(), string);
    }

}

Value CellBase::parsedUserInput(const QString& text)
{
    // Parses the text and return the appropriate value.
    Value value = sheet()->map()->parser()->parse(text);

    // convert first letter to uppercase ?
    if (sheet()->getFirstLetterUpper() && value.isString() && !text.isEmpty()) {
        QString str = value.asString();
        value = Value(str[0].toUpper() + str.right(str.length() - 1));
    }
    return value;
}

// parses the text
void CellBase::parseUserInput(const QString& text)
{
    // empty string?
    if (text.isEmpty()) {
        setCellValue(Value::empty());
        return;
    }

    // a formula?
    if (text[0] == '=') {
        Formula formula(sheet(), *this);
        formula.setExpression(text);
        setFormula(formula);

/*  This most likely isn't needed anymore ...
        // parse the formula and check for errors
        if (!formula.isValid()) {
            sheet()->showStatusMessage(i18n("Parsing of formula in cell %1 failed.", fullName()));
            setValue(Value::errorPARSE());
            return;
        }
*/
        return;
    }

    // keep the old formula and value in case that validation fails
    const Formula oldFormula = formula();
    const QString oldUserInput = userInput();
    const Value oldValue = value();

    // here, the new value is not a formula anymore; clear an existing one
    setFormula(Formula());

    Value value = parsedUserInput(text);

    // set the new value
    setUserInput(text);
    setValue(value);

    // validation
    if (!sheet()->map()->isLoading()) {
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

void CellBase::setRawUserInput(const QString& string)
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



bool CellBase::isEmpty() const
{
    // empty = no value or formula
    if (value() != Value())
        return false;
    if (formula() != Formula())
        return false;
    return true;
}

// Return true if this cell is a formula.
//
bool CellBase::isFormula() const
{
    return !formula().expression().isEmpty();
}

bool CellBase::isLocked() const
{
    return sheet()->cellStorage()->isLocked(d->column, d->row);
}

QRect CellBase::lockedCells() const
{
    return sheet()->cellStorage()->lockedCells(d->column, d->row);
}

QString CellBase::comment() const
{
    return sheet()->cellStorage()->comment(d->column, d->row);
}

void CellBase::setComment(const QString& comment)
{
    sheet()->cellStorage()->setComment(Region(cellPosition()), comment);
}

Validity CellBase::validity() const
{
    return sheet()->cellStorage()->validity(d->column, d->row);
}

void CellBase::setValidity(Validity validity)
{
    sheet()->cellStorage()->setValidity(Region(cellPosition()), validity);
}












// Return the symbolic name of any column.
//
// static
QString CellBase::columnName(unsigned int column)
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

CellBase& CellBase::operator=(const CellBase & other)
{
    d = other.d;
    return *this;
}

bool CellBase::operator<(const CellBase& other) const
{
    if (sheet() != other.sheet())
        return sheet() < other.sheet(); // pointers!
    if (row() < other.row())
        return true;
    return ((row() == other.row()) && (column() < other.column()));
}

bool CellBase::operator==(const CellBase& other) const
{
    return (row() == other.row() && column() == other.column() && sheet() == other.sheet());
}

bool CellBase::operator!() const
{
    return (!d);   // isNull()
}


