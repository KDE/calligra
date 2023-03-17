/* This file is part of the KDE project
   SPDX-FileCopyrightText: 2005-2007 Stefan Nikolaus <stefan.nikolaus@kdemail.net>
             2006 Fredrik Edemar <f_edemar@linux.se>
             2005-2006 Raphael Langerhorst <raphael.langerhorst@kdemail.net>
             2004 Tomas Mecir <mecirt@gmail.com>
             2003 Norbert Andres <nandres@web.de>
             2002 Philipp Mueller <philipp.mueller@gmx.de>
             2000 David Faure <faure@kde.org>
             2000 Werner Trobin <trobin@kde.org>
             2000-2006 Laurent Montel <montel@kde.org>
             1999, 2000 Torben Weis <weis@kde.org>
             1999 Stephan Kulow <coolo@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef CALLIGRA_SHEETS_FORMAT
#define CALLIGRA_SHEETS_FORMAT

#include "sheets_core_export.h"
#include "engine/FormatEnum.h"

namespace Calligra
{
namespace Sheets
{

/**
 * \ingroup Style
 * Collection of string formatting enumeration and functions.
 */
namespace Format
{

//helper functions for the formatting
CALLIGRA_SHEETS_CORE_EXPORT bool isDate(Type type);
CALLIGRA_SHEETS_CORE_EXPORT bool isTime(Type type);
CALLIGRA_SHEETS_CORE_EXPORT bool isFraction(Type type);
CALLIGRA_SHEETS_CORE_EXPORT bool isMoney(Type type);
CALLIGRA_SHEETS_CORE_EXPORT bool isNumber(Type type);

} // namespace Format
} // namespace Sheets
} // namespace Calligra

#endif // CALLIGRA_SHEETS_FORMAT
