/* This file is part of the KDE libraries
    Copyright (C) 2000 Stephan Kulow <coolo@kde.org>
                  1999 - 2008 David Faure <faure@kde.org>
                  2001 Holger Freyther <freyther@kde.org>

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

#ifndef KIO_RENAMEDIALOG_H
#define KIO_RENAMEDIALOG_H

#include <kio/global.h>

namespace KIO
{

// KDE5: get rid of M_OVERWRITE_ITSELF, trigger it internally if src==dest
// KDE5: get rid of M_SINGLE. If not multi, then single ;)
// KDE5: use QFlags to get rid of all the casting!
/**
 * M_OVERWRITE: We have an existing dest, show details about it and offer to overwrite it.
 * M_OVERWRITE_ITSELF: Warn that the current operation would overwrite a file with itself,
 *                     which is not allowed.
 * M_SKIP: Offer a "Skip" button, to skip other files too. Requires M_MULTI.
 * M_SINGLE: Deprecated and unused, please ignore.
 * M_MULTI: Set if the current operation concerns multiple files, so it makes sense
 *  to offer buttons that apply the user's choice to all files/folders.
 * M_RESUME: Offer a "Resume" button (plus "Resume All" if M_MULTI)
 * M_NORENAME: Don't offer a "Rename" button
 * M_ISDIR: The dest is a directory, so label the "overwrite" button something like "merge" instead.
 */
enum RenameDialog_Mode { M_OVERWRITE = 1, M_OVERWRITE_ITSELF = 2, M_SKIP = 4, M_SINGLE = 8, M_MULTI = 16, M_RESUME = 32, M_NORENAME = 64, M_ISDIR = 128 };

/**
 * The result of open_RenameDialog().
 */
enum RenameDialog_Result {R_RESUME = 6, R_RESUME_ALL = 7, R_OVERWRITE = 4, R_OVERWRITE_ALL = 5, R_SKIP = 2, R_AUTO_SKIP = 3, R_RENAME = 1, R_AUTO_RENAME = 8, R_CANCEL = 0};

}

#endif
