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

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef CALLIGRA_SHEETS_CANVAS
#define CALLIGRA_SHEETS_CANVAS

#include "CanvasBase.h"

#include <QWidget>

namespace Calligra
{
namespace Sheets
{
class Cell;
class Canvas;
class ColumnHeader;
class Doc;
class Sheet;
class RowHeader;
class Selection;
class View;

/**
 * The scrollable area showing the cells.
 */
class CALLIGRA_SHEETS_PART_EXPORT Canvas : public QWidget, public CanvasBase
{
    friend class ColumnHeader;
    friend class RowHeader;
    friend class View;
    friend class CellTool;

    Q_OBJECT

public:
    explicit Canvas(View *view);
    ~Canvas() override;

    View *view() const;

    /// reimplemented method from KoCanvasBase
    QWidget *canvasWidget() override
    {
        return this;
    }
    const QWidget *canvasWidget() const override
    {
        return this;
    }

    Sheet *activeSheet() const override;
    Calligra::Sheets::Selection *selection() const override;
    void setCursor(const QCursor &cursor) override;

public Q_SLOTS:
    void setDocumentOffset(const QPoint &offset)
    {
        CanvasBase::setDocumentOffset(offset);
    }
    void setDocumentSize(const QSizeF &size)
    {
        CanvasBase::setDocumentSize(size);
    }

Q_SIGNALS:
    /* virtual */ void documentSizeChanged(const QSize &) override;

protected:
    bool event(QEvent *e) override;
    void keyPressEvent(QKeyEvent *_ev) override
    {
        CanvasBase::keyPressed(_ev);
    }
    void paintEvent(QPaintEvent *_ev) override;
    void mousePressEvent(QMouseEvent *_ev) override;
    void mouseReleaseEvent(QMouseEvent *_ev) override;
    void mouseMoveEvent(QMouseEvent *_ev) override;
    void mouseDoubleClickEvent(QMouseEvent *) override;
    void focusInEvent(QFocusEvent *_ev) override
    {
        CanvasBase::focusIn(_ev);
        QWidget::focusInEvent(_ev);
    }
    void dragEnterEvent(QDragEnterEvent *) override;
    void dragMoveEvent(QDragMoveEvent *) override;
    void dragLeaveEvent(QDragLeaveEvent *) override;
    void dropEvent(QDropEvent *) override;
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
    void tabletEvent(QTabletEvent *e) override
    {
        CanvasBase::tabletEvent(e);
    }

public:
    void update() override
    {
        QWidget::update();
    }
    void update(const QRectF &rect) override
    {
        QWidget::update(rect.toRect());
    }
    Qt::LayoutDirection layoutDirection() const override
    {
        return QWidget::layoutDirection();
    }
    QRectF rect() const override
    {
        return QWidget::rect();
    }
    QSizeF size() const override
    {
        return QWidget::size();
    }
    QPoint mapToGlobal(const QPointF &point) const override
    {
        return QWidget::mapToGlobal(point.toPoint());
    }
    void updateMicroFocus() override
    {
        QWidget::updateMicroFocus();
    }

    /**
     * Validates the selected cell.
     */
    void validateSelection();

    KoZoomHandler *zoomHandler() const override;
    bool isViewLoading() const override;
    SheetView *sheetView(Sheet *sheet) const override;
    void enableAutoScroll() override;
    void disableAutoScroll() override;
    void showContextMenu(const QPoint &globalPos) override;
    ColumnHeader *columnHeader() const override;
    RowHeader *rowHeader() const override;

private:
    void setVertScrollBarPos(qreal pos) override;
    void setHorizScrollBarPos(qreal pos) override;

    bool eventFilter(QObject *o, QEvent *e) override
    {
        return CanvasBase::eventFilter(o, e);
    }

private:
    Q_DISABLE_COPY(Canvas)

    class Private;
    Private *const cd;
};

} // namespace Sheets
} // namespace Calligra

#endif // CALLIGRA_SHEETS_CANVAS
