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

#ifndef KSPREAD_CANVAS
#define KSPREAD_CANVAS

#include <QList>
#include <QWidget>

#include <KoCanvasBase.h>

#include "kspread_export.h"

#include "Global.h"
#include "CanvasBase.h"

class QDragLeaveEvent;
class QDragMoveEvent;
class QDropEvent;
class QEvent;
class QFocusEvent;
class QKeyEvent;
class QMouseEvent;
class QPainter;
class QPaintEvent;
class QPen;
class QResizeEvent;
class QScrollBar;

namespace KSpread
{
class Cell;
class CellEditor;
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
class KSPREAD_EXPORT Canvas : public QWidget, public CanvasBase
{
    friend class ColumnHeader;
    friend class RowHeader;
    friend class View;
    friend class CellTool;

    Q_OBJECT

public:
    explicit Canvas(View* view);
    ~Canvas();

    View* view() const;

    /// reimplemented method from KoCanvasBase
    virtual QWidget* canvasWidget() {
        return this;
    }
    virtual const QWidget* canvasWidget() const {
        return this;
    }

    virtual Sheet* activeSheet() const;
    virtual KSpread::Selection* selection() const;
    virtual void setCursor(const QCursor &cursor);

public Q_SLOTS:
    void setDocumentOffset(const QPoint& offset) {
        CanvasBase::setDocumentOffset(offset);
    }
    void setDocumentSize(const QSizeF& size) {
        CanvasBase::setDocumentSize(size);
    }

Q_SIGNALS:
    /* virtual */ void documentSizeChanged(const QSize&);

protected:
    virtual bool event(QEvent *e);
    virtual void keyPressEvent(QKeyEvent* _ev) {
        CanvasBase::keyPressed(_ev);
    }
    virtual void paintEvent(QPaintEvent* _ev);
    virtual void mousePressEvent(QMouseEvent* _ev);
    virtual void mouseReleaseEvent(QMouseEvent* _ev);
    virtual void mouseMoveEvent(QMouseEvent* _ev);
    virtual void mouseDoubleClickEvent(QMouseEvent*);
    virtual void focusInEvent(QFocusEvent* _ev) {
        CanvasBase::focusIn(_ev);
        QWidget::focusInEvent(_ev);
    }
    virtual void dragEnterEvent(QDragEnterEvent*);
    virtual void dragMoveEvent(QDragMoveEvent*);
    virtual void dragLeaveEvent(QDragLeaveEvent*);
    virtual void dropEvent(QDropEvent*);
    /// reimplemented method from superclass
    virtual QVariant inputMethodQuery(Qt::InputMethodQuery query) const {
        return CanvasBase::inputMethodQuery(query);
    }
    /// reimplemented method from superclass
    virtual void inputMethodEvent(QInputMethodEvent *event) {
        CanvasBase::inputMethodEvent(event);
    }
    /// reimplemented method from superclass
    virtual void tabletEvent(QTabletEvent *e) {
        CanvasBase::tabletEvent(e);
    }

public:
    virtual void update() {
        QWidget::update();
    }
    virtual void update(const QRectF& rect) {
        QWidget::update(rect.toRect());
    }
    virtual Qt::LayoutDirection layoutDirection() const {
        return QWidget::layoutDirection();
    }
    virtual QRectF rect() const {
        return QWidget::rect();
    }
    virtual QSizeF size() const {
        return QWidget::size();
    }
    virtual QPoint mapToGlobal(const QPointF& point) const {
        return QWidget::mapToGlobal(point.toPoint());
    }
    virtual void updateMicroFocus() {
        QWidget::updateMicroFocus();
    }

    virtual KoZoomHandler* zoomHandler() const;
    virtual bool isViewLoading() const;
    virtual SheetView* sheetView(const Sheet* sheet) const;
    virtual void enableAutoScroll();
    virtual void disableAutoScroll();
    virtual void showContextMenu(const QPoint& globalPos);
    virtual ColumnHeader* columnHeader() const;
    virtual RowHeader* rowHeader() const;
private:
    virtual void setVertScrollBarPos(qreal pos);
    virtual void setHorizScrollBarPos(qreal pos);

    virtual bool eventFilter(QObject *o, QEvent *e) {
        return CanvasBase::eventFilter(o, e);
    }

private:
    Q_DISABLE_COPY(Canvas)

    class Private;
    Private * const cd;
};

} // namespace KSpread

#endif // KSPREAD_CANVAS
