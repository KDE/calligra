/* This file is part of the KDE project
   SPDX-FileCopyrightText: 1999 David Faure <faure@kde.org>
   SPDX-FileCopyrightText: 2004 Nicolas GOUTTE <goutte@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#pragma once

#include <ui_exportdialogui.h>

#include <QDialog>

class ExportDialogUI;
class QValidator;

namespace Calligra
{
namespace Sheets
{
class Map;
}
}

class CSVExportDialog : public QDialog
{
    Q_OBJECT
public:
    explicit CSVExportDialog(QWidget *parent);
    ~CSVExportDialog() override;

    [[nodiscard]] QChar getDelimiter() const;
    [[nodiscard]] QChar getTextQuote() const;
    [[nodiscard]] bool exportSheet(QString const &sheetName) const;
    [[nodiscard]] bool printAlwaysSheetDelimiter() const;
    [[nodiscard]] QString getSheetDelimiter() const;
    [[nodiscard]] bool exportSelectionOnly() const;

    void fillSheet(Calligra::Sheets::Map *map);
    [[nodiscard]] QString getEndOfLine() const;

private:
    void loadSettings();
    void saveSettings();
    Ui::ExportDialogUI m_dialog;

    QValidator *m_delimiterValidator;
    QString m_delimiter;
    QChar m_textquote;

private Q_SLOTS:
    void slotOk();
    void slotCancel();

    void returnPressed();
    void delimiterClicked(int id);
    void textChanged(const QString &);
    void textquoteSelected(const QString &mark);
    void selectionOnlyChanged(bool);
};
