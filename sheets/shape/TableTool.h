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

#ifndef CALLIGRA_SHEETS_TABLE_TOOL
#define CALLIGRA_SHEETS_TABLE_TOOL

#include "CellToolBase.h"

#include <QScrollBar>

namespace Calligra
{
namespace Sheets
{

class ScrollBar : public QScrollBar
{
    Q_OBJECT
public:
    ScrollBar(Qt::Orientation o, QWidget *parent = nullptr);

protected:
    bool event(QEvent *event) override;
};


class TableTool : public CellToolBase
{
    Q_OBJECT

public:
    enum ToolType { None, Shape, ColumnHeader, RowHeader, HScrollBar, VScrollBar };

    explicit TableTool(KoCanvasBase* canvas);
    ~TableTool();

    void repaintDecorations() override;
    void paint(QPainter &painter, const KoViewConverter &converter) override;
    Selection* selection() override;

public Q_SLOTS:
    void activate(ToolActivation toolActivation, const QSet<KoShape*> &shapes) override;
    void deactivate() override;

    void slotSelectionChanged(const Region&);

protected:
    void mousePressEvent(KoPointerEvent* event) override;
    void mouseReleaseEvent(KoPointerEvent* event) override;
    void mouseMoveEvent(KoPointerEvent* event) override;
    void mouseDoubleClickEvent(KoPointerEvent* event) override;
    void keyPressEvent(QKeyEvent* event) override;

    KoInteractionStrategy* createStrategy(KoPointerEvent* event) override;

    void scrollToCell(const QPoint &location) override;

    QPointF offset() const override;
    QSizeF size() const override;
    QPointF canvasOffset() const override;
    int maxCol() const override;
    int maxRow() const override;
    SheetView* sheetView(const Sheet* sheet) const override;

    double canvasOffsetX() const override;
    double canvasOffsetY() const override;

    ToolType mouseOn(KoPointerEvent* event);

private Q_SLOTS:
    void sheetActivated(const QString& sheetName);
    void sheetsBtnClicked();
    void importDocument();
    void exportDocument();

    void slotHorizontalScrollBarValueChanged(int value);
    void slotVerticalScrollBarValueChanged(int value);

    void update();

private:
    void updateSheetsList();
    QList<QPointer<QWidget> > createOptionWidgets() override;
    void activateSheet();

private:
    Q_DISABLE_COPY(TableTool)

    class Private;
    Private * const d;
};

} // namespace Sheets
} // namespace Calligra

#endif // CALLIGRA_SHEETS_TABLE_TOOL
