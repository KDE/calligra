/* This file is part of the KDE project
   Copyright (C) 2000 Robert JACOLIN <rjacolin@ifrance.com>

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

#ifndef __XSLTEXPORTDIA_H__
#define __XSLTEXPORTDIA_H__

#include <xsltdialog.h>
#include <QStringList>
//Added by qt3to4:
#include <QByteArray>
#include <kurl.h>

class KoStoreDevice;
class KConfig;

class XSLTExportDia : public XSLTDialog
{
    Q_OBJECT

    QString _fileOut;
    KoStoreDevice* _in;
    /** xslt file current */
    KUrl _currentFile;
    QByteArray _format;
    KConfig* _config;
    /** List of the most recent xslt file used. */
    QStringList _recentList;

    /** Lits use for common xslt files. */
    QStringList _dirsList;
    QStringList _filesList;
    QStringList _namesList;

public:
    XSLTExportDia( KoStoreDevice*, const QByteArray &format, QWidget* parent = 0, const char* name = 0, bool modal = FALSE, Qt::WFlags fl = 0 );
    ~XSLTExportDia();

    void setOutputFile(QString file) { _fileOut = file; }

public slots:
    virtual void cancelSlot();
    virtual void chooseSlot();
    virtual void chooseRecentSlot();
    virtual void chooseCommonSlot();
    virtual void okSlot();
};

#endif /* __XSLTEXPORTDIA_H__ */
