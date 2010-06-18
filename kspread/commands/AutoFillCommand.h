/* This file is part of the KDE project
   Copyright 2006-2007 Stefan Nikolaus <stefan.nikolaus@kdemail.net>
   Copyright 2003 Ariya Hidayat <ariya@kde.org>
   Copyright 2002 Norbert Andres <nandres@web.de>
   Copyright 2001-2002 Laurent Montel <montel@kde.org>
   Copyright 2001 David Faure <faure@kde.org>
   Copyright 2000 Werner Trobin <trobin@kde.org>
   Copyright 1998-1999 Torben Weis <weis@kde.org>

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


#ifndef KSPREAD_AUTOFILL_COMMAND
#define KSPREAD_AUTOFILL_COMMAND

#include "DataManipulators.h"

#include <QList>
#include <QRect>
#include <QString>
#include <QStringList>

namespace KSpread
{
class AutoFillSequence;
class Cell;

/**
 * \ingroup Commands
 * \brief Auto-filling of a cell range.
 */
class AutoFillCommand : public AbstractDataManipulator
{
public:
    /**
     * Constructor.
     */
    AutoFillCommand();

    /**
     * Destructor.
     */
    virtual ~AutoFillCommand();

    void setSourceRange(const QRect& range);
    void setTargetRange(const QRect& range);

    /**
     * Executes the actual operation.
     */
    virtual bool mainProcessing();

public:
    static QStringList *other;
    static QStringList *month;
    static QStringList *day;
    static QStringList *shortMonth;
    static QStringList *shortDay;

private:
    void fillSequence(const QList<Cell>& _srcList,
                      const QList<Cell>& _destList,
                      const AutoFillSequence& _seqList,
                      bool down = true);
    // dummy
    virtual Value newValue(Element*, int, int, bool*, Format::Type*) {
        return Value();
    }

private:
    QRect m_sourceRange;
    QRect m_targetRange;
};

} // namespace KSpread

#endif // KSPREAD_AUTOFILL_COMMAND
