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
class QGraphicsView;
class QGraphicsScene;
class KPrTimeLineHeader;
class TimeLineItem;
class QAbstractItemModel;
class QModelIndex;

/**
  This widget is part of Animations Time Line Widget
  */
class KPrTimeLineView: public QWidget
{
     Q_OBJECT
public:
    KPrTimeLineView(QWidget *parent = 0);
    virtual QSize sizeHint() const;
    virtual QSize minimumSizeHint() const;
    virtual bool eventFilter(QObject *target, QEvent *event);

signals:
    void clicked(const QModelIndex&);

    void timeValuesChanged(const QModelIndex&);

private:
    void keyPressEvent(QKeyEvent *event);
    void mousePressEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);
    bool event(QEvent *event);

    int rowAt(int ypos);
    int columnAt(int xpos);

    /// Helper method to paint view items
    void paintEvent(QPaintEvent * event);
    void paintRow(QPainter *painter, int  row, int y, const int RowHeight);
    void paintItemBackground(QPainter *painter, const QRect &rect, bool selected);
    void paintLine(QPainter *painter, int row, const QRect &rect, bool selected);
    void paintTextRow(QPainter *painter, int x, int y, int row, int column, const int RowHeight);
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
