/* This file is part of the KDE project
   Copyright (C) 2009 Benjamin Port <port.benjamin@gmail.com>

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
 * Boston, MA 02110-1301, USA.
*/

#include "KPrHtmlExportUiDelegate.h"

KPrHtmlExportUiDelegate::KPrHtmlExportUiDelegate()
: JobUiDelegate()
{
}

KIO::RenameDialog_Result KPrHtmlExportUiDelegate::askFileRename ( KJob* job, const QString& caption, const QString& src,
                                                                  const QString& dest, KIO::RenameDialog_Mode mode, QString& newDest,
                                                                  KIO::filesize_t sizeSrc, KIO::filesize_t sizeDest, time_t ctimeSrc,
                                                                  time_t ctimeDest, time_t mtimeSrc, time_t mtimeDest)
{
    // Change mode, remove rename possibility
    mode = (KIO::RenameDialog_Mode)(mode | KIO::M_NORENAME);
    return KIO::JobUiDelegate::askFileRename(job, caption, src, dest, mode, newDest, sizeSrc,
                                             sizeDest, ctimeSrc, ctimeDest, mtimeSrc, mtimeDest);
}
