/*
 *  SPDX-FileCopyrightText: 2006, 2007 Thorsten Zachmann <zachmann@kde.org>
 *  SPDX-FileCopyrightText: 2009, 2010 Cyrille Berger <cberger@cberger.net>
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

#include <QList>
#include <QWidget>

#include <KoCanvasBase.h>

class View;
class RootSection;
class Section;

/// Widget that shows a KoPAPage
class Canvas : public QWidget, public KoCanvasBase
{
    Q_OBJECT
public:
    explicit Canvas(View *view, RootSection *doc, Section *currentSection);
    ~Canvas();

    /// Returns pointer to the KoPADocument
    RootSection *rootSection() const
    {
        return m_doc;
    }

    /// reimplemented method
    void addCommand(KUndo2Command *command) override;
    /// reimplemented method
    KoShapeManager *shapeManager() const override;
    /// reimplemented method
    void updateCanvas(const QRectF &rc) override;
    /// reimplemented method
    void updateInputMethodInfo() override;

    KoToolProxy *toolProxy() const override
    {
        return m_toolProxy;
    }
    KoViewConverter *viewConverter() const override;
    QWidget *canvasWidget() override
    {
        return this;
    }
    const QWidget *canvasWidget() const override
    {
        return this;
    }
    KoUnit unit() const override;
    const QPoint &documentOffset() const;
    QPoint documentOrigin() const override;

    View *koPAView() const
    {
        return m_view;
    }

    virtual void gridSize(qreal *horizontal, qreal *vertical) const override;
    virtual bool snapToGrid() const override;
    virtual void setCursor(const QCursor &cursor) override;
public Q_SLOTS:
    /**
     * Update the origin of the document.
     */
    void updateOriginAndSize();

    void setDocumentOffset(const QPoint &offset);

protected:
    void focusInEvent(QFocusEvent *event) override;

Q_SIGNALS:
    void documentRect(const QRectF &);
    void canvasReceivedFocus();

    /**
     * Emitted when the entire controller size changes
     * @param size the size in widget pixels.
     */
    void sizeChanged(const QSize &size);

    /// Emitted when updateCanvas has been called.
    void canvasUpdated();

protected:
    /// reimplemented method from superclass
    void paintEvent(QPaintEvent *event) override;
    /// reimplemented method from superclass
    void tabletEvent(QTabletEvent *event) override;
    /// reimplemented method from superclass
    void mousePressEvent(QMouseEvent *event) override;
    /// reimplemented method from superclass
    void mouseDoubleClickEvent(QMouseEvent *event) override;
    /// reimplemented method from superclass
    void mouseMoveEvent(QMouseEvent *event) override;
    /// reimplemented method from superclass
    void mouseReleaseEvent(QMouseEvent *event) override;
    /// reimplemented method from superclass
    void keyPressEvent(QKeyEvent *event) override;
    /// reimplemented method from superclass
    void keyReleaseEvent(QKeyEvent *event) override;
    /// reimplemented method from superclass
    void wheelEvent(QWheelEvent *event) override;
    /// reimplemented method from superclass
    void closeEvent(QCloseEvent *event) override;
    /// reimplemented method from superclass
    virtual QVariant inputMethodQuery(Qt::InputMethodQuery query) const override;
    /// reimplemented method from superclass
    virtual void inputMethodEvent(QInputMethodEvent *event) override;

    /// reimplemented method from superclass
    virtual void resizeEvent(QResizeEvent *event) override;

private:
    void updateOffset();
    /**
     * Shows the default context menu
     * @param globalPos global position to show the menu at.
     * @param actionList action list to be inserted into the menu
     */
    void showContextMenu(const QPoint &globalPos, const QList<QAction *> &actionList);
    /// Sets the canvas background color to the given color
    void setBackgroundColor(const QColor &color);

    QPoint widgetToView(const QPoint &p) const;
    QRect widgetToView(const QRect &r) const;
    QPoint viewToWidget(const QPoint &p) const;
    QRect viewToWidget(const QRect &r) const;

private:
    QPoint m_origin;
    View *m_view;
    RootSection *m_doc;
    KoShapeManager *m_shapeManager;
    KoToolProxy *m_toolProxy;
    QPoint m_documentOffset;
    QPoint m_originalOffset;
    QRectF m_oldDocumentRect;
    QRect m_oldViewDocumentRect;
};

#endif /* KOPACANVAS_H */
