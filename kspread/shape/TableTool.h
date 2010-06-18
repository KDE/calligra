/* This file is part of the KDE project
   Copyright 2006 Stefan Nikolaus <stefan.nikolaus@kdemail.net>

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

#ifndef KSPREAD_TABLE_TOOL
#define KSPREAD_TABLE_TOOL

#include "CellToolBase.h"

class KoInteractionStrategy;

namespace KSpread
{
class Region;

class TableTool : public CellToolBase
{
    Q_OBJECT

public:
    explicit TableTool(KoCanvasBase* canvas);
    ~TableTool();

    virtual void repaintDecorations();

    virtual Selection* selection();

public Q_SLOTS:
    virtual void activate(ToolActivation toolActivation, const QSet<KoShape*> &shapes);
    virtual void deactivate();

protected:
    virtual QPointF offset() const;
    virtual QSizeF size() const;
    virtual QPointF canvasOffset() const;
    virtual int maxCol() const;
    virtual int maxRow() const;
    virtual SheetView* sheetView(const Sheet* sheet) const;

private Q_SLOTS:
    void changeColumns(int num);
    void changeRows(int num);
    void sheetActivated(const QString& sheetName);
    void sheetsBtnClicked();
    void importDocument();
    void exportDocument();

private:
    void updateSheetsList();
    virtual QWidget* createOptionWidget();

private:
    Q_DISABLE_COPY(TableTool)

    class Private;
    Private * const d;
};

} // namespace KSpread

#endif // KSPREAD_TABLE_TOOL
