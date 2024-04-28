/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2005-2007 Thomas Zander <zander@kde.org>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#ifndef KWGUI_H
#define KWGUI_H

#include <QWidget>

class KWView;
class KWCanvas;
class KoCanvasController;
class KoRuler;

class QMouseEvent;

/**
 * This class is a widget that is the sole widget under a KWView instance
 * separating concerns.
 */
class KWGui : public QWidget
{
    Q_OBJECT

public:
    /**
     * Create new gui widget.
     * @param viewMode the KWViewMode::type() string to indicate the view mode the canvas
     *        will use to show the content
     * @param parent the parent view
     */
    KWGui(const QString &viewMode, KWView *parent);
    ~KWGui() override;

    /**
     * Return the canvas child.
     * @return the canvas child.
     */
    KWCanvas *canvas() const
    {
        return m_canvas;
    }
    /**
     * return the width of the canvas that is fully shown and not obscured behind scrollbars.
     * @return the amount of pixels visible
     */
    int visibleWidth() const;
    /**
     * return the height of the canvas that is fully shown and not obscured behind scrollbars.
     * @return the amount of pixels visible
     */
    int visibleHeight() const;

    /**
     * return the height of the canvas that is fully shown and not obscured behind scrollbars.
     * @return the amount of pixels visible
     */
    QSize viewportSize() const;

    /// return if the there currently is a horizontal scrolBar visible
    bool horizontalScrollBarVisible();

    /**
     * Request a repaint of the rulers.
     */
    void updateRulers() const;

    /// return the canvasController that wraps this view
    KoCanvasController *canvasController() const
    {
        return m_canvasController;
    }

protected:
    void mouseMoveEvent(QMouseEvent *e) override;

private Q_SLOTS:
    void pageSetupChanged();
    void updateMousePos(const QPoint &point);
    void shapeSelectionChanged();
    void setupUnitActions();

private:
    KWView *m_view;
    KWCanvas *m_canvas;
    KoRuler *m_horizontalRuler;
    KoRuler *m_verticalRuler;

    KoCanvasController *m_canvasController;
};

#endif
