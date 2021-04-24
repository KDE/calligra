/* This file is part of the KDE project
   SPDX-FileCopyrightText: 2002, 2003 Robert JACOLIN <rjacolin@ifrance.com>

   SPDX-License-Identifier: LGPL-2.0-or-later
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
