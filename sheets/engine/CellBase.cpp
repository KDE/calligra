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

#include <QObject>

using namespace Calligra::Sheets;

class Q_DECL_HIDDEN CellBase::Private
{
public:
    Private(CellBase *cell);
    ~Private() {}
private:
    CellBase *m_cell;
};

CellBase::Private::Private (CellBase *cell)
    : m_cell(cell)
{

}




CellBase::CellBase() :
    d(new Private(this))
{
}

CellBase::~CellBase()
{
}



// Return the symbolic name of any column.
//
// static
QString CellBase::columnName(uint column)
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


