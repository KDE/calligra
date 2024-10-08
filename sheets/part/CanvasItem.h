/* This file is part of the KDE project
   SPDX-FileCopyrightText: 2006-2007 Stefan Nikolaus <stefan.nikolaus@kdemail.net>
   SPDX-FileCopyrightText: 1999-2002, 2004 Laurent Montel <montel@kde.org>
   SPDX-FileCopyrightText: 2002-2005 Ariya Hidayat <ariya@kde.org>
   SPDX-FileCopyrightText: 1999-2001, 2003 David Faure <faure@kde.org>
   SPDX-FileCopyrightText: 2001-2003 Philipp Mueller <philipp.mueller@gmx.de>
   SPDX-FileCopyrightText: 2002-2003 Norbert Andres <nandres@web.de>
   SPDX-FileCopyrightText: 2000-2001 Werner Trobin <trobin@kde.org>
   SPDX-FileCopyrightText: 2002 Harri Porten <porten@kde.org>
   SPDX-FileCopyrightText: 2002 John Dailey <dailey@vt.edu>
   SPDX-FileCopyrightText: 1999-2000 Torben Weis <weis@kde.org>
   SPDX-FileCopyrightText: 2000 Wilco Greven <greven@kde.org>
   SPDX-FileCopyrightText: 1999 Boris Wedl <boris.wedl@kfunigraz.ac.at>
   SPDX-FileCopyrightText: 1999 Reginald Stadlbauer <reggie@kde.org>
   SPDX-FileCopyrightText: 2010 Boudewijn Rempt <boud@kogmbh.com>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef CALLIGRA_SHEETS_CANVAS_ITEM
#define CALLIGRA_SHEETS_CANVAS_ITEM

#include <QGraphicsWidget>

#include "CanvasBase.h"

class QEvent;
class QFocusEvent;
class QKeyEvent;
class QPainter;

namespace Calligra
{
namespace Sheets
{
class Cell;
class CanvasItem;
class ColumnHeader;
class Damage;
class Doc;
class Sheet;
class RowHeader;
class Selection;
class View;

/**
 * The scrollable area showing the cells.
 */
class CALLIGRA_SHEETS_PART_EXPORT CanvasItem : public QGraphicsWidget, public CanvasBase
{
    friend class ColumnHeaderItem;
    friend class RowHeaderItem;
    friend class View;
    friend class CellTool;

    Q_OBJECT

public:
    explicit CanvasItem(Doc *doc, QGraphicsItem *parent = nullptr);
    ~CanvasItem() override;

    QWidget *canvasWidget() override
    {
        return nullptr;
    }
    const QWidget *canvasWidget() const override
    {
        return nullptr;
    }

    /**
     * Return the widget that will be added to the scrollArea.
     */
    QGraphicsObject *canvasItem() override
    {
        return this;
    }

    /**
     * Return the widget that will be added to the scrollArea.
     */
    const QGraphicsObject *canvasItem() const override
    {
        return this;
    }

public Q_SLOTS:
    void setDocumentOffset(const QPoint &offset)
    {
        CanvasBase::setDocumentOffset(offset);
    }
    void setDocumentSize(const QSizeF &size)
    {
        CanvasBase::setDocumentSize(size);
    }

    void refreshSheetViews();
    void setActiveSheet(Sheet *sheet);

    void setObscuredRange(const QSize &size);

    /**
     * \ingroup Damages
     * Handles damages that need visual updates.
     */
    void handleDamages(const QList<Damage *> &damages);

    void updateAccessedCellRange(Sheet *sheet, const QPoint &location);

Q_SIGNALS:
    /* virtual */ void documentSizeChanged(const QSize &) override;
    void obscuredRangeChanged(const Calligra::Sheets::Sheet *sheet, const QSize &);

public:
    void mousePressEvent(QGraphicsSceneMouseEvent *_ev) override;
    void mouseReleaseEvent(QGraphicsSceneMouseEvent *_ev) override;

protected:
    void keyPressEvent(QKeyEvent *_ev) override
    {
        CanvasBase::keyPressed(_ev);
    }
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget = nullptr) override;
    void mouseMoveEvent(QGraphicsSceneMouseEvent *_ev) override;
    void mouseDoubleClickEvent(QGraphicsSceneMouseEvent *) override;
    void focusInEvent(QFocusEvent *_ev) override
    {
        CanvasBase::focusIn(_ev);
        QGraphicsWidget::focusInEvent(_ev);
    }
    void dragEnterEvent(QGraphicsSceneDragDropEvent *) override;
    void dragMoveEvent(QGraphicsSceneDragDropEvent *) override;
    void dragLeaveEvent(QGraphicsSceneDragDropEvent *) override;
    void dropEvent(QGraphicsSceneDragDropEvent *) override;
    /// reimplemented method from superclass
    QVariant inputMethodQuery(Qt::InputMethodQuery query) const override
    {
        return CanvasBase::inputMethodQuery(query);
    }
    /// reimplemented method from superclass
    void inputMethodEvent(QInputMethodEvent *event) override
    {
        CanvasBase::inputMethodEvent(event);
    }
    /// reimplemented method from superclass
    virtual void tabletEvent(QTabletEvent *e)
    {
        CanvasBase::tabletEvent(e);
    }

private:
    bool eventFilter(QObject *o, QEvent *e) override
    {
        return CanvasBase::eventFilter(o, e);
    }

public:
    Selection *selection() const override;
    Sheet *activeSheet() const override;
    KoZoomHandler *zoomHandler() const override;
    SheetView *sheetView(Sheet *sheet) const override;

    bool isViewLoading() const override
    {
        return false;
    }
    void enableAutoScroll() override
    {
    }
    void disableAutoScroll() override
    {
    }
    void setVertScrollBarPos(qreal) override
    {
    }
    void setHorizScrollBarPos(qreal) override
    {
    }

    void update() override
    {
        QGraphicsWidget::update();
    }
    void update(const QRectF &rect) override
    {
        QGraphicsWidget::update(rect);
    }
    Qt::LayoutDirection layoutDirection() const override
    {
        return QGraphicsWidget::layoutDirection();
    }
    QRectF rect() const override
    {
        return QGraphicsWidget::rect();
    }
    QSizeF size() const override
    {
        return QGraphicsWidget::size();
    }
    QPoint mapToGlobal(const QPointF &point) const override
    {
        return QGraphicsWidget::mapToScene(point).toPoint(); /* TODO */
    }
    void updateMicroFocus() override
    { /*QGraphicsWidget::updateMicroFocus();*/
    }

    ColumnHeader *columnHeader() const override;
    RowHeader *rowHeader() const override;
    void setCursor(const QCursor &cursor) override;

    void showContextMenu(const QPoint &globalPos) override
    {
        Q_UNUSED(globalPos);
    }

private:
    Q_DISABLE_COPY(CanvasItem)

    class Private;
    Private *const d;
};

} // namespace Sheets
} // namespace Calligra

#endif // CALLIGRA_SHEETS_CANVAS_ITEM
