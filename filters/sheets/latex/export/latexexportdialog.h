/* This file is part of the KDE project
   Copyright (C) 2002, 2003 Robert JACOLIN <rjacolin@ifrance.com>

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

#ifndef __LATEXEXPORTDIALOG_H__
#define __LATEXEXPORTDIALOG_H__

#include <ui_latexexportwidget.h>

#include <KoDialog.h>
// Qt
#include <QStringList>

class KoStore;
class KConfig;

class LatexExportDialog : public KoDialog
{
    Q_OBJECT

public:
    explicit LatexExportDialog(KoStore*, QWidget* parent = 0);
    ~LatexExportDialog() override;

    void setOutputFile(const QString& file) {
        m_fileOut = file;
    }

public Q_SLOTS:
    void reject() override;
    void accept() override;
    virtual void addLanguage();
    virtual void removeLanguage();

private:
    Ui::LatexExportWidget m_ui;

    QString m_fileOut;
    KoStore* m_inputStore;
    KConfig* m_config;

};

#endif /* __LATEXEXPORTDIALOG_H__ */
