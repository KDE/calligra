/* This file is part of the KDE project
   Copyright (C) 2005, 2006, 2010 Stefan Nikolaus <stefan.nikolaus@kdemail.net>
             (C) 2006 Fredrik Edemar <f_edemar@linux.se>
             (C) 2005-2006 Raphael Langerhorst <raphael.langerhorst@kdemail.net>
             (C) 2004 Tomas Mecir <mecirt@gmail.com>
             (C) 2003 Norbert Andres <nandres@web.de>
             (C) 2002 Philipp Mueller <philipp.mueller@gmx.de>
             (C) 2000 David Faure <faure@kde.org>
             (C) 2000 Werner Trobin <trobin@kde.org>
             (C) 2000-2006 Laurent Montel <montel@kde.org>
             (C) 1999, 2000 Torben Weis <weis@kde.org>
             (C) 1999 Stephan Kulow <coolo@kde.org>

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

#ifndef KSPREAD_LIMITS
#define KSPREAD_LIMITS

/* Definition of maximal supported rows - please check kspread_util (columnName) and kspread_cluster also */
#ifndef KS_rowMax
#define KS_rowMax 0xFFFF
#endif

/* Definition of maximal supported columns - please check kspread_util (columnName) and kspread_cluster also */
#ifndef KS_colMax
#define KS_colMax 0x7FFF
#endif

/* Definition of maximal supported columns/rows, which can be merged */
#ifndef KS_spanMax
#define KS_spanMax 0xFFF
#endif

#endif // KSPREAD_LIMITS
