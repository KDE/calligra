/*
 *  Copyright (c) 2006,2007 Thorsten Zachmann <zachmann@kde.org>
 *  Copyright (c) 2009,2010 Cyrille Berger <cberger@cberger.net>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation;
 * either version 2, or (at your option) any later version of the License.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this library; see the file COPYING.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
 */

#ifndef _CANVAS_H
#define _CANVAS_H

#include <QWidget>
#include <QList>

#include <KoCanvasBase.h>

class View;
class RootSection;
class Section;

/// Widget that shows a KoPAPage
class Canvas : public QWidget, public KoCanvasBase
{
    Q_OBJECT
public:
    explicit Canvas(View * view, RootSection * doc, Section* currentSection);
    ~Canvas();

    /// Returns pointer to the KoPADocument
    RootSection* rootSection() const {
        return m_doc;
    }

    /// reimplemented method
    virtual void addCommand(KUndo2Command *command);
    /// reimplemented method
    virtual KoShapeManager * shapeManager() const;
    /// reimplemented method
    virtual void updateCanvas(const QRectF& rc);
    /// reimplemented method
    virtual void updateInputMethodInfo();

    KoToolProxy * toolProxy() const {
        return m_toolProxy;
    }
    KoViewConverter *viewConverter() const;
    QWidget* canvasWidget() {
        return this;
    }
    const QWidget* canvasWidget() const {
        return this;
    }
    KoUnit unit() const;
    const QPoint & documentOffset() const;
    QPoint documentOrigin() const;

    View* koPAView() const {
        return m_view;
    }

    virtual void gridSize(qreal *horizontal, qreal *vertical) const;
    virtual bool snapToGrid() const;
    virtual void setCursor(const QCursor &cursor);
public Q_SLOTS:
    /**
     * Update the origin of the document.
     */
    void updateOriginAndSize();

    void setDocumentOffset(const QPoint &offset);

protected:
    virtual void focusInEvent(QFocusEvent * event);

Q_SIGNALS:
    void documentRect(const QRectF&);
    void canvasReceivedFocus();

    /**
     * Emitted when the entire controller size changes
     * @param size the size in widget pixels.
     */
    void sizeChanged(const QSize & size);

    /// Emitted when updateCanvas has been called.
    void canvasUpdated();

protected:
    /// reimplemented method from superclass
    void paintEvent(QPaintEvent* event);
    /// reimplemented method from superclass
    void tabletEvent(QTabletEvent *event);
    /// reimplemented method from superclass
    void mousePressEvent(QMouseEvent *event);
    /// reimplemented method from superclass
    void mouseDoubleClickEvent(QMouseEvent *event);
    /// reimplemented method from superclass
    void mouseMoveEvent(QMouseEvent *event);
    /// reimplemented method from superclass
    void mouseReleaseEvent(QMouseEvent *event);
    /// reimplemented method from superclass
    void keyPressEvent(QKeyEvent *event);
    /// reimplemented method from superclass
    void keyReleaseEvent(QKeyEvent *event);
    /// reimplemented method from superclass
    void wheelEvent(QWheelEvent * event);
    /// reimplemented method from superclass
    void closeEvent(QCloseEvent * event);
    /// reimplemented method from superclass
    virtual QVariant inputMethodQuery(Qt::InputMethodQuery query) const;
    /// reimplemented method from superclass
    virtual void inputMethodEvent(QInputMethodEvent *event);

    /// reimplemented method from superclass
    virtual void resizeEvent(QResizeEvent * event);

private:
    void updateOffset();
    /**
     * Shows the default context menu
     * @param globalPos global position to show the menu at.
     * @param actionList action list to be inserted into the menu
     */
    void showContextMenu(const QPoint& globalPos, const QList<QAction*>& actionList);
    /// Sets the canvas background color to the given color
    void setBackgroundColor(const QColor &color);

    QPoint widgetToView(const QPoint& p) const;
    QRect widgetToView(const QRect& r) const;
    QPoint viewToWidget(const QPoint& p) const;
    QRect viewToWidget(const QRect& r) const;
private:
    QPoint m_origin;
    View * m_view;
    RootSection* m_doc;
    KoShapeManager * m_shapeManager;
    KoToolProxy * m_toolProxy;
    QPoint m_documentOffset;
    QPoint m_originalOffset;
    QRectF m_oldDocumentRect;
    QRect m_oldViewDocumentRect;

};

#endif /* KOPACANVAS_H */
