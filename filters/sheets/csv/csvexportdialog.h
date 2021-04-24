/* This file is part of the KDE project
   SPDX-FileCopyrightText: 1999 David Faure <faure@kde.org>
   SPDX-FileCopyrightText: 2004 Nicolas GOUTTE <goutte@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef CSVEXPORTDIALOG_H
#define CSVEXPORTDIALOG_H

#include <KoDialog.h>
#include <ui_exportdialogui.h>

class ExportDialogUI;
class QValidator;

namespace Calligra
{
namespace Sheets
{
class Map;
}
}

class ExportDialogUI : public QWidget, public Ui::ExportDialogUI
{
public:
    explicit ExportDialogUI(QWidget *parent) : QWidget(parent) {
        setupUi(this);
    }
};


class CSVExportDialog : public KoDialog
{
    Q_OBJECT
public:
    explicit CSVExportDialog(QWidget *parent);
    ~CSVExportDialog() override;

    QChar   getDelimiter() const;
    QChar   getTextQuote() const;
    bool    exportSheet(QString const & sheetName) const;
    bool    printAlwaysSheetDelimiter() const;
    QString getSheetDelimiter() const;
    bool    exportSelectionOnly() const;

    void  fillSheet(Calligra::Sheets::Map * map);
    QString getEndOfLine(void) const;
    QTextCodec* getCodec(void) const;

private:
    void loadSettings();
    void saveSettings();
    ExportDialogUI * m_dialog;

    QValidator* m_delimiterValidator;
    QString m_delimiter;
    QChar   m_textquote;

private Q_SLOTS:
    void slotOk();
    void slotCancel();

    void returnPressed();
    void delimiterClicked(int id);
    void textChanged(const QString &);
    void textquoteSelected(const QString & mark);
    void selectionOnlyChanged(bool);
};

#endif
