/* This file is part of the KDE project
   SPDX-FileCopyrightText: 2005-2006 Stefan Nikolaus <stefan.nikolaus@kdemail.net>
             SPDX-FileCopyrightText: 2006 Fredrik Edemar <f_edemar@linux.se>
             SPDX-FileCopyrightText: 2005-2006 Raphael Langerhorst <raphael.langerhorst@kdemail.net>
             SPDX-FileCopyrightText: 2004 Tomas Mecir <mecirt@gmail.com>
             SPDX-FileCopyrightText: 2003 Norbert Andres <nandres@web.de>
             SPDX-FileCopyrightText: 2002 Philipp Mueller <philipp.mueller@gmx.de>
             SPDX-FileCopyrightText: 2000 David Faure <faure@kde.org>
             SPDX-FileCopyrightText: 2000 Werner Trobin <trobin@kde.org>
             SPDX-FileCopyrightText: 2000-2006 Laurent Montel <montel@kde.org>
             SPDX-FileCopyrightText: 1999, 2000 Torben Weis <weis@kde.org>
             SPDX-FileCopyrightText: 1999 Stephan Kulow <coolo@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef KSPREAD_GLOBAL_H
#define KSPREAD_GLOBAL_H

#include "SheetsDebug.h"

namespace Calligra
{
namespace Sheets
{

/**
 * This namespace collects enumerations related to
 * pasting operations.
 */
namespace Paste
{
/**
 * The pasted content
 */
enum Mode {
    Normal /** Everything */,
    Text /** Text only */,
    Format /** Format only */,
    NoBorder /** not the borders */,
    Comment /** Comment only */,
    Result /** Result only, no formula */,
    NormalAndTranspose /** */,
    TextAndTranspose /** */,
    FormatAndTranspose /** */,
    NoBorderAndTranspose /** */
};
/**
 * The current cell value treatment.
 */
enum Operation {
    OverWrite /** Overwrite */,
    Add /** Add */,
    Mul /** Multiply */,
    Sub /** Subtract */,
    Div /** Divide */
};
} // namespace Paste

// necessary due to QDock* enums (Werner)
enum MoveTo { Bottom, Left, Top, Right, BottomFirst, NoMovement };
enum MethodOfCalc { SumOfNumber, Min, Max, Average, Count, NoneCalc, CountA };

} // namespace Sheets
} // namespace Calligra

#endif
