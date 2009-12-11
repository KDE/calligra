/* This file is part of the KDE project
   Copyright 2007 Sebastian Sauer <mail@dipe.org>

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
   Boston, MA 02110-1301, USA.
*/

#ifndef KSPREAD_TABLE_SHEETSEDITOR
#define KSPREAD_TABLE_SHEETSEDITOR

#include <QWidget>
#include <QListWidgetItem>

namespace KSpread
{
class TableShape;
class Sheet;

class SheetsEditor : public QWidget
{
    Q_OBJECT
public:
    explicit SheetsEditor(TableShape* tableShape, QWidget* parent = 0);
    virtual ~SheetsEditor();

private Q_SLOTS:
    void sheetAdded(Sheet* sheet);
    void sheetNameChanged(Sheet* sheet, const QString& old_name);

    void selectionChanged();
    void itemChanged(QListWidgetItem* item);

    void renameClicked();
    void addClicked();
    void removeClicked();

private:
    Q_DISABLE_COPY(SheetsEditor)

    class Private;
    Private * const d;
};

} // namespace KSpread

#endif // KSPREAD_TABLE_TOOL
