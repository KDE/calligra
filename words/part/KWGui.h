/* This file is part of the KDE project
 * Copyright (C) 2005-2007 Thomas Zander <zander@kde.org>
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
 * Boston, MA 02110-1301, USA
 */

#ifndef KWGUI_H
#define KWGUI_H


#include <QWidget>

class KWView;
class KWCanvas;
class KoCanvasController;
class KoRuler;

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
    ~KWGui();

    /**
     * Return the canvas child.
     * @return the canvas child.
     */
    KWCanvas *canvas() const {
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
    KoCanvasController *canvasController() const {
        return m_canvasController;
    }

private slots:
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
