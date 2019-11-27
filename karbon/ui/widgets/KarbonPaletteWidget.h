/* This file is part of the KDE project
 * Copyright (C) 2011 Jan Hambrecht <jaham@gmx.net>
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

#ifndef KARBONPALETTEWIDGET_H
#define KARBONPALETTEWIDGET_H

#include <KoColor.h>
#include <QWidget>

class KoColorSet;

class KarbonPaletteWidget : public QWidget
{
    Q_OBJECT
public:
    explicit KarbonPaletteWidget(QWidget *parent = 0);
    ~KarbonPaletteWidget() override;

    /// Set orientation of color bar
    void setOrientation(Qt::Orientation orientation);

    /// Returns the current orientation
    Qt::Orientation orientation() const;

    /// Returns maximal scrolling offset
    int maximalScrollOffset() const;
    /// Returns the current scrolling offset
    int currentScrollOffset() const;

    /// Sets the color palette to display
    void setPalette(KoColorSet *colorSet);

    /// Returns the current palette
    KoColorSet *palette() const;

public Q_SLOTS:
    void scrollForward();
    void scrollBackward();

Q_SIGNALS:
    /// Emitted whenever a color was clicked
    void colorSelected(const KoColor &color);

    /// Emitted whenever the scroll offset changed
    void scrollOffsetChanged();

protected:
    void paintEvent(QPaintEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;
    void wheelEvent(QWheelEvent *event) override;
    void keyPressEvent(QKeyEvent *event) override;
    bool event(QEvent *event) override;

private:
    /// Returns color index from given position
    int indexFromPosition(const QPoint &position);
    /// Returns patch size
    QSize patchSize() const;
    /// Apply scrolling
    void applyScrolling(int delta);

    Qt::Orientation m_orientation;
    int m_scrollOffset;
    KoColorSet *m_palette;
    int m_pressedIndex;
    bool m_hasDragged;
};

#endif // KARBONPALETTEWIDGET_H
