/*
 * imageshare.h -- Part of Krita
 *
 * Copyright (c) 2004 Boudewijn Rempt (boud@valdyas.org)
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
 */
#ifndef IMAGESHARE_H
#define IMAGESHARE_H

#include <QVariant>

#include <kparts/plugin.h>
#include "kis_types.h"
#include "stash.h"

class KoUpdater;
class KoProgressUpdater;
class QUrl;

class KisView2;
class KisPainter;

class O2DeviantART;
class SubmitDlg;

class ImageShare : public KParts::Plugin
{
    Q_OBJECT
public:
    ImageShare(QObject *parent, const QVariantList &);
    virtual ~ImageShare();

private slots:

    void slotImageShare();
    void openBrowser(const QUrl &url);
    void closeBrowser();
    void showSubmit();
    void testCallCompleted(Stash::Call, bool result);
    void performUpload();
    void submissionsChanged();
    void submitCallCompleted(Stash::Call, bool result);
    void uploadProgress(int id, qint64 bytesSent, qint64 bytesTotal);
    void availableSpaceChanged();

private:

    KisView2 *m_view;
    O2DeviantART *m_deviantArt;
    Stash* m_stash;
    SubmitDlg* m_submitDlg;
    KoProgressUpdater* m_progressUpdater;
    QPointer<KoUpdater> m_progressSubtask;
};

#endif // IMAGESHARE_H
