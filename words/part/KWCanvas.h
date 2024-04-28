/* This file is part of the KDE project
   SPDX-FileCopyrightText: 1998, 1999 Reginald Stadlbauer <reggie@kde.org>
   SPDX-FileCopyrightText: 2002-2006 David Faure <faure@kde.org>
   SPDX-FileCopyrightText: 2005-2006 Thomas Zander <zander@kde.org>
   SPDX-FileCopyrightText: 2010 Boudewijn Rempt <boud@kogmbh.com>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef KWCANVAS_H
#define KWCANVAS_H

#include "KWDocument.h"
#include "words_export.h"

#include "KWCanvasBase.h"

#include "KWViewMode.h"

#include <QWidget>

class QRect;

class KWGui;
class KWView;

/**
 * This class is responsible for the rendering of the frames to
 * the screen as well as the interaction with the user via mouse
 * and keyboard. There is one per view.
 */
class WORDS_TEST_EXPORT KWCanvas : public QWidget, public KWCanvasBase
{
    Q_OBJECT

public:
    /**
     * Constructor
     * Creates a new canvas widget that can display pages and frames.
     * @param viewMode the initial KWViewMode this canvas should use
     * @param document as this is one view in the MVC design; the document holds all content
     * @param view the parent KWView object
     * @param parent the parent widget.
     */
    KWCanvas(const QString &viewMode, KWDocument *document, KWView *view, KWGui *parent);
    ~KWCanvas() override;

    /// ask the widget to set the size this canvas takes to display all content
    void updateSize();

    // KoCanvasBase interface methods.
    /// reimplemented method from superclass
    bool snapToGrid() const override;

    /// reimplemented method from superclass
    QPointF viewToDocument(const QPointF &viewPoint) const override;

    /// reimplemented method from superclass
    QWidget *canvasWidget() override
    {
        return this;
    }
    /// reimplemented method from superclass
    const QWidget *canvasWidget() const override
    {
        return this;
    }

    KWView *view()
    {
        return m_view;
    }

    void setCursor(const QCursor &cursor) override;

public Q_SLOTS:
    /**
     * sets the document offset in the scrollArea
     * @param offset the offset, in pixels.
     */
    void setDocumentOffset(const QPoint &offset);

Q_SIGNALS:
    /**
     * emitted when the contentsSize changes.
     * @see KWViewMode::contentsSize
     * @param size the content area size, in pixels.
     */
    void documentSize(const QSizeF &size);

protected: // QWidget
    /// reimplemented method from superclass
    bool event(QEvent *) override;
    /// reimplemented method from superclass
    void keyPressEvent(QKeyEvent *e) override;
    /// reimplemented method from superclass
    void contextMenuEvent(QContextMenuEvent *e) override;
    /// reimplemented method from superclass
    void mouseMoveEvent(QMouseEvent *e) override;
    /// reimplemented method from superclass
    void mousePressEvent(QMouseEvent *e) override;
    /// reimplemented method from superclass
    void mouseReleaseEvent(QMouseEvent *e) override;
    /// reimplemented method from superclass
    void mouseDoubleClickEvent(QMouseEvent *e) override;
    /// reimplemented method from superclass
    void keyReleaseEvent(QKeyEvent *e) override;
    /// reimplemented method from superclass
    void paintEvent(QPaintEvent *ev) override;
    /// reimplemented method from superclass
    void tabletEvent(QTabletEvent *e) override;
    /// reimplemented method from superclass
    void wheelEvent(QWheelEvent *e) override;
    /// reimplemented method from superclass
    QVariant inputMethodQuery(Qt::InputMethodQuery query) const override;
    /// reimplemented method from superclass
    void inputMethodEvent(QInputMethodEvent *event) override;
    /// reimplemented method from superclass
    void updateInputMethodInfo() override;
    /// reimplemented method from superclass
    void updateCanvasInternal(const QRectF &clip) override
    {
        update(clip.toRect());
    }

private Q_SLOTS:
    /// Called whenever there was a page added/removed or simply resized.
    void pageSetupChanged();

private:
    KWView *m_view;
};

#endif
