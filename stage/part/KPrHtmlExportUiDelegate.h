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
