/* This file is part of the KDE project
   SPDX-FileCopyrightText: 2009 Benjamin Port <port.benjamin@gmail.com>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "KPrHtmlExportUiDelegate.h"

KPrHtmlExportUiDelegate::KPrHtmlExportUiDelegate()
: JobUiDelegate()
{
}

KIO::RenameDialog_Result KPrHtmlExportUiDelegate::askFileRename ( KJob* job, const QString& caption, const QUrl& src,
                                                                  const QUrl& dest, KIO::RenameDialog_Options options, QString& newDest,
                                                                  KIO::filesize_t sizeSrc, KIO::filesize_t sizeDest, const QDateTime &ctimeSrc,
                                                                  const QDateTime &ctimeDest, const QDateTime &mtimeSrc, const QDateTime &mtimeDest)
{
    // Change options, remove rename possibility
    options = (KIO::RenameDialog_Options)(options | KIO::RenameDialog_NoRename);
    return KIO::JobUiDelegate::askFileRename(job, caption, src, dest, options, newDest, sizeSrc,
                                             sizeDest, ctimeSrc, ctimeDest, mtimeSrc, mtimeDest);
}
