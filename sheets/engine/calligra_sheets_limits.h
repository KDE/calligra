/* This file is part of the KDE project
   SPDX-FileCopyrightText: 2005, 2006, 2010 Stefan Nikolaus <stefan.nikolaus@kdemail.net>
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

#ifndef CALLIGRA_SHEETS_LIMITS
#define CALLIGRA_SHEETS_LIMITS

/* Definition of maximal supported rows - please check kspread_util (columnName) and kspread_cluster also */
#ifndef KS_rowMax
#define KS_rowMax 0x100000
#endif

/* Definition of maximal supported columns - please check kspread_util (columnName) and kspread_cluster also */
#ifndef KS_colMax
#define KS_colMax 0x7FFF
#endif

/* Definition of maximal supported columns/rows, which can be merged */
#ifndef KS_spanMax
#define KS_spanMax 0xFFF
#endif

#endif // CALLIGRA_SHEETS_LIMITS
