/* This file is part of the Calligra project
   Copyright (C) 2012 Matus Uzak <matus.uzak@gmail.com>

   This library is free software; you can redistribute it and/or
   modify it under the terms of the Library GNU General Public
   version 2 of the License, or (at your option) version 3 or,
   at the discretion of KDE e.V (which shall act as a proxy as in
   section 14 of the GPLv3), any later version..

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this library; see the file COPYING.LIB.  If not, write to
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
   Boston, MA 02110-1301, USA.
*/

#ifndef MSOLEPS_H
#define MSOLEPS_H

/**
 * The PropertyIdentifier data type represents the property identifier
 * of a property in a property set.
 *
 * 0x00000002 — 0x7FFFFFFF - Used to identify normal properties.
 */
enum PropertyType
{
    PIDSI_CODEPAGE = 0x00000001,
    PIDSI_TITLE,
    PIDSI_SUBJECT,
    PIDSI_AUTHOR,
    PIDSI_KEYWORDS,
    PIDSI_COMMENTS,
    PIDSI_TEMPLATE,
    PIDSI_LASTAUTHOR,
    PIDSI_REVNUMBER,
    PIDSI_EDITTIME,
    PIDSI_LASTPRINTED,
    PIDSI_CREATE_DTM,
    PIDSI_LASTSAVE_DTM,
    PIDSI_PAGECOUNT,
    PIDSI_WORDCOUNT,
    PIDSI_CHARCOUNT,
    PIDSI_APPNAME = 0x00000012,
    PIDSI_DOC_SECURITY = 0x00000013
};

#endif
