/*
 *  Copyright (c) 2005 Boudewijn Rempt <boud@valdyas.org>
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
 *  Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
 */

#ifndef KIS_RAW_IMPORT_H_
#define KIS_RAW_IMPORT_H_

#include <KoFilter.h>

#include "imageviewer.h"
#include "ui_wdgrawimport.h"

class WdgRawImport : public QWidget, public Ui::WdgRawImport
{
    Q_OBJECT

    public:
        WdgRawImport(QWidget *parent) : QWidget(parent) { setupUi(this); }
};

class KProcess;
class KDialog;
class WdgRawImport;
class KoColorProfile;
class Q3ProgressDialog;

class KisRawImport : public KoFilter {
    Q_OBJECT

public:
    KisRawImport(QObject* parent, const QStringList&);
    virtual ~KisRawImport();

public:
    virtual KoFilter::ConversionStatus convert(const QByteArray& from, const QByteArray& to);


private slots:

    void slotUpdatePreview();
    void slotFillCmbProfiles();
    void slotProcessDone();
    void slotReceivedStdout(KProcess *proc, char *buffer, int buflen);
    void slotReceivedStderr(KProcess *proc, char *buffer, int buflen);
    void incrementProgress();

private:

    QStringList createArgumentList(bool forPreview = false);
    QSize determineSize(quint32& startOfImageData);
    void getImageData(QStringList arguments);
    KoColorProfile * profile();
    KoID getColorSpace();
    
private:
    QByteArray * m_data;
    KDialog * m_dialog;
    WdgRawImport * m_page;
    KoColorProfile * m_monitorProfile;
    KProcess * m_process;
    Q3ProgressDialog* m_progress;
};

#endif // KIS_RAW_IMPORT_H_

