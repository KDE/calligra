/* This file is part of the KDE project
 * Copyright 2020 Dag Andersen <dag.andersen@kdemail.net>
 * Copyright 2006 Stefan Nikolaus <stefan.nikolaus@kdemail.net>
 *  
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 * 
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 * 
 * You should have received a copy of the GNU Library General Public License
 * along with this library; see the file COPYING.LIB.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
 */

#ifndef CALLIGRA_SHEETS_TABLE_TOOL
#define CALLIGRA_SHEETS_TABLE_TOOL

#include "CellToolBase.h"

#include <QScrollBar>

namespace Calligra
{
namespace Sheets
{

class TableShape;

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
    enum ToolType { None,
                    Table,
                    ColumnHeader,
                    RowHeader,
                    HScrollBar,
                    VScrollBar,
                    SheetName,
                    ResizeAreaLeft,
                    ResizeAreaRight,
                    ResizeAreaTop,
                    ResizeAreaBottom,
                    ResizeAreaTopLeft,
                    ResizeAreaTopRight,
                    ResizeAreaBottomLeft,
                    ResizeAreaBottomRight
    };

    explicit TableTool(KoCanvasBase* canvas);
    ~TableTool();

    void repaintDecorations() override;
    void paint(QPainter &painter, const KoViewConverter &converter) override;
    Selection* selection() override;

    TableShape *shape() const;

public Q_SLOTS:
    void activate(ToolActivation toolActivation, const QSet<KoShape*> &shapes) override;
    void deactivate() override;

    void slotSelectionChanged(const Region&);

    void slotShapeChanged(KoShape *shape);

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
    void openSheetsEditor();
#if 0
    void exportDocument();
#endif
    void slotHorizontalScrollBarValueChanged(int value);
    void slotVerticalScrollBarValueChanged(int value);

    void update();

    void recalcSheet();
    void recalcMap();

    void sheetSelected();

    void slotSheetAdded(Sheet*);
    void slotSheetRemoved(int);
    void slotSheetModified(Sheet*);

private:
    QList<QPointer<QWidget> > createOptionWidgets() override;
    void activateSheet();

    QList<QAction*> popupMenuActionList() const override;
    void createActions();

    void saveCurrentSheetSelection();

    bool autoScroll(ToolType type, KoPointerEvent* event);

    void sheetSelectionCombo();

private:
    Q_DISABLE_COPY(TableTool)

    class Private;
    Private * const d;

    QPointF m_lastMousePoint;
};

} // namespace Sheets
} // namespace Calligra

#endif // CALLIGRA_SHEETS_TABLE_TOOL
