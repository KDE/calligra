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
#include "SheetBase.h"

#include <QObject>
#include <QSharedData>

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


