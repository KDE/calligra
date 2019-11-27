/* This file is part of the KDE project
 * Copyright (C) 2012 Paul Mendez <paulestebanms@gmail.com>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (  at your option ) any later version.
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

#ifndef KPRTIMELINEVIEW_H
#define KPRTIMELINEVIEW_H

#include <QWidget>

class KPrAnimationsTimeLineView;
class QModelIndex;

/**
  This widget is part of Animations Time Line Widget
  */
class KPrTimeLineView: public QWidget
{
     Q_OBJECT
public:
    explicit KPrTimeLineView(QWidget *parent = 0);
    QSize sizeHint() const override;
    QSize minimumSizeHint() const override;
    bool eventFilter(QObject *target, QEvent *event) override;

Q_SIGNALS:
    /// emitted if an item is clicked (returns the index of the item clicked
    void clicked(const QModelIndex&);

    /// emitted if an item time range has changed (return the index of the item changed)
    void timeValuesChanged(const QModelIndex&);

private:
    void keyPressEvent(QKeyEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;
    bool event(QEvent *event) override;

    /// Return the row under mouse y pos
    int rowAt(int ypos);
    /// Returns the column under mouse x pos
    int columnAt(int xpos);

    /// returns rect for cell in row and column
    QRectF getRowRect(const int row, const int column);

    /// Helper method to paint view items
    void paintEvent(QPaintEvent * event) override;
    /// main paint method
    void paintRow(QPainter *painter, int  row, int y, const int RowHeight);
    /// paint background of items
    void paintItemBackground(QPainter *painter, const QRect &rect, bool selected);
    /// Paint cells of time bars
    void paintLine(QPainter *painter, int row, const QRect &rect, bool selected);
    /// pant text cells
    void paintTextRow(QPainter *painter, int x, int y, int row, int column, const int RowHeight);
    /// paint icon cells
    void paintIconRow(QPainter *painter, int x, int y, int row, int column, int iconSize, const int RowHeight);
    double modD(double x, double y);

    KPrAnimationsTimeLineView *m_mainView;
    bool m_resize;
    bool m_move;
    int m_resizedRow;
    int startDragPos;
    bool m_adjust;
};

#endif // KPRTIMELINEVIEW_H
