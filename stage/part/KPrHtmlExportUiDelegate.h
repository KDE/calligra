/* This file is part of the KDE project
   SPDX-FileCopyrightText: 2009 Benjamin Port <port.benjamin@gmail.com>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef KPRHTMLEXPORTUIDELEGATE_H
#define KPRHTMLEXPORTUIDELEGATE_H

#include <kio/jobuidelegate.h>
#include <kio/renamedialog.h>

class KPrHtmlExportUiDelegate : public KIO::JobUiDelegate
{
    Q_OBJECT
public:
    KPrHtmlExportUiDelegate();

    KIO::RenameDialog_Result askFileRename(KJob * job, const QString & caption, const QUrl& src, const QUrl & dest,
                                              KIO::RenameDialog_Options options, QString& newDest,
                                              KIO::filesize_t sizeSrc = (KIO::filesize_t) -1,
                                              KIO::filesize_t sizeDest = (KIO::filesize_t) -1,
                                              const QDateTime &ctimeSrc = QDateTime(), const QDateTime &ctimeDest = QDateTime(),
                                              const QDateTime &mtimeSrc = QDateTime(), const QDateTime &mtimeDest = QDateTime()) override;
};

#endif /* KPRHTMLEXPORTUIDELEGATE_H */
