/* This file is part of the KDE project
   Copyright 2006-2007 Stefan Nikolaus <stefan.nikolaus@kdemail.net>
   Copyright 1999-2002,2004 Laurent Montel <montel@kde.org>
   Copyright 2002-2005 Ariya Hidayat <ariya@kde.org>
   Copyright 1999-2001,2003 David Faure <faure@kde.org>
   Copyright 2001-2003 Philipp Mueller <philipp.mueller@gmx.de>
   Copyright 2002-2003 Norbert Andres <nandres@web.de>
   Copyright 2000-2001 Werner Trobin <trobin@kde.org>
   Copyright 2002 Harri Porten <porten@kde.org>
   Copyright 2002 John Dailey <dailey@vt.edu>
   Copyright 1999-2000 Torben Weis <weis@kde.org>
   Copyright 2000 Wilco Greven <greven@kde.org>
   Copyright 1999 Boris Wedl <boris.wedl@kfunigraz.ac.at>
   Copyright 1999 Reginald Stadlbauer <reggie@kde.org>

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

#ifndef CALLIGRA_SHEETS_CANVAS
#define CALLIGRA_SHEETS_CANVAS

#include <QList>
#include <QWidget>
#include <QVariant>

#include <KoCanvasBase.h>

#include "sheets_common_export.h"
#include "../Global.h"
#include "CanvasBase.h"

class QDragLeaveEvent;
class QDragMoveEvent;
class QDropEvent;
class QEvent;
class QFocusEvent;
class QKeyEvent;
class QMouseEvent;
class QPaintEvent;

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
class CALLIGRA_SHEETS_COMMON_EXPORT Canvas : public QWidget, public CanvasBase
{
    friend class ColumnHeader;
    friend class RowHeader;
    friend class View;
    friend class CellTool;

    Q_OBJECT

public:
    explicit Canvas(View* view);
    ~Canvas() override;

    View* view() const;

    /// reimplemented method from KoCanvasBase
    QWidget* canvasWidget() override {
        return this;
    }
    const QWidget* canvasWidget() const override {
        return this;
    }

    Sheet* activeSheet() const override;
    Calligra::Sheets::Selection* selection() const override;
    void setCursor(const QCursor &cursor) override;

public Q_SLOTS:
    void setDocumentOffset(const QPoint& offset) {
        CanvasBase::setDocumentOffset(offset);
    }
    void setDocumentSize(const QSizeF& size) {
        CanvasBase::setDocumentSize(size);
    }

Q_SIGNALS:
    /* virtual */ void documentSizeChanged(const QSize&) override;

protected:
    bool event(QEvent *e) override;
    void keyPressEvent(QKeyEvent* _ev) override {
        CanvasBase::keyPressed(_ev);
    }
    void paintEvent(QPaintEvent* _ev) override;
    void mousePressEvent(QMouseEvent* _ev) override;
    void mouseReleaseEvent(QMouseEvent* _ev) override;
    void mouseMoveEvent(QMouseEvent* _ev) override;
    void mouseDoubleClickEvent(QMouseEvent*) override;
    void focusInEvent(QFocusEvent* _ev) override {
        CanvasBase::focusIn(_ev);
        QWidget::focusInEvent(_ev);
    }
    void dragEnterEvent(QDragEnterEvent*) override;
    void dragMoveEvent(QDragMoveEvent*) override;
    void dragLeaveEvent(QDragLeaveEvent*) override;
    void dropEvent(QDropEvent*) override;
    /// reimplemented method from superclass
    QVariant inputMethodQuery(Qt::InputMethodQuery query) const override {
        return CanvasBase::inputMethodQuery(query);
    }
    /// reimplemented method from superclass
    void inputMethodEvent(QInputMethodEvent *event) override {
        CanvasBase::inputMethodEvent(event);
    }
    /// reimplemented method from superclass
    void tabletEvent(QTabletEvent *e) override {
        CanvasBase::tabletEvent(e);
    }

public:
    void update() override {
        QWidget::update();
    }
    void update(const QRectF& rect) override {
        QWidget::update(rect.toRect());
    }
    Qt::LayoutDirection layoutDirection() const override {
        return QWidget::layoutDirection();
    }
    QRectF rect() const override {
        return QWidget::rect();
    }
    QSizeF size() const override {
        return QWidget::size();
    }
    QPoint mapToGlobal(const QPointF& point) const override {
        return QWidget::mapToGlobal(point.toPoint());
    }
    void updateMicroFocus() override {
        QWidget::updateMicroFocus();
    }

    KoZoomHandler* zoomHandler() const override;
    bool isViewLoading() const override;
    SheetView* sheetView(const Sheet* sheet) const override;
    void enableAutoScroll() override;
    void disableAutoScroll() override;
    void showContextMenu(const QPoint& globalPos) override;
    ColumnHeader* columnHeader() const override;
    RowHeader* rowHeader() const override;
private:
    void setVertScrollBarPos(qreal pos) override;
    void setHorizScrollBarPos(qreal pos) override;

    bool eventFilter(QObject *o, QEvent *e) override {
        return CanvasBase::eventFilter(o, e);
    }

private:
    Q_DISABLE_COPY(Canvas)

    class Private;
    Private * const cd;
};

} // namespace Sheets
} // namespace Calligra

#endif // CALLIGRA_SHEETS_CANVAS
