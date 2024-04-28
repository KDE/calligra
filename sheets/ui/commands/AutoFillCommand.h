/* This file is part of the KDE project
   SPDX-FileCopyrightText: 2006-2007 Stefan Nikolaus <stefan.nikolaus@kdemail.net>
   SPDX-FileCopyrightText: 2003 Ariya Hidayat <ariya@kde.org>
   SPDX-FileCopyrightText: 2002 Norbert Andres <nandres@web.de>
   SPDX-FileCopyrightText: 2001-2002 Laurent Montel <montel@kde.org>
   SPDX-FileCopyrightText: 2001 David Faure <faure@kde.org>
   SPDX-FileCopyrightText: 2000 Werner Trobin <trobin@kde.org>
   SPDX-FileCopyrightText: 1998-1999 Torben Weis <weis@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef CALLIGRA_SHEETS_AUTOFILL_COMMAND
#define CALLIGRA_SHEETS_AUTOFILL_COMMAND

#include "AbstractRegionCommand.h"

namespace Calligra
{
namespace Sheets
{
class AutoFillSequence;
class Cell;

/**
 * \ingroup Commands
 * \brief Auto-filling of a cell range.
 */
class AutoFillCommand : public AbstractRegionCommand
{
public:
    /**
     * Constructor.
     */
    AutoFillCommand();

    /**
     * Destructor.
     */
    ~AutoFillCommand() override;

    void setSourceRange(const QRect &range);
    void setTargetRange(const QRect &range);

    /**
     * Executes the actual operation.
     */
    bool performCommands() override;

public:
    static QStringList *other;
    static QStringList *month;
    static QStringList *day;
    static QStringList *shortMonth;
    static QStringList *shortDay;

private:
    void fillSequence(const QList<Cell> &_srcList, const QList<Cell> &_destList, const AutoFillSequence &_seqList, bool down = true);

private:
    QRect m_sourceRange;
    QRect m_targetRange;
};

} // namespace Sheets
} // namespace Calligra

#endif // CALLIGRA_SHEETS_AUTOFILL_COMMAND
