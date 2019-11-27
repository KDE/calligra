/* This file is part of the KDE project
   Copyright (C) 1999 David Faure <faure@kde.org>
   Copyright (C) 2004 Nicolas GOUTTE <goutte@kde.org>

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
