/* This file is part of the KDE project
   SPDX-FileCopyrightText: 2007 Sebastian Sauer <mail@dipe.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef CALLIGRA_SHEETS_TABLE_SHEETSEDITOR
#define CALLIGRA_SHEETS_TABLE_SHEETSEDITOR

#include <QWidget>

class QListWidgetItem;

namespace Calligra
{
namespace Sheets
{
class TableShape;
class Sheet;
class SheetBase;

class SheetsEditor : public QWidget
{
    Q_OBJECT
public:
    explicit SheetsEditor(TableShape *tableShape, QWidget *parent = nullptr);
    virtual ~SheetsEditor();

private Q_SLOTS:
    void sheetAdded(SheetBase *sheet);
    void sheetNameChanged(Sheet *sheet, const QString &old_name);

    void selectionChanged();
    void itemChanged(QListWidgetItem *item);

    void renameClicked();
    void addClicked();
    void removeClicked();

private:
    Q_DISABLE_COPY(SheetsEditor)

    class Private;
    Private *const d;
};

} // namespace Sheets
} // namespace Calligra

#endif // CALLIGRA_SHEETS_TABLE_TOOL
