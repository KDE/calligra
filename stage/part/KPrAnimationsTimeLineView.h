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

#ifndef KPRANIMATIONSTIMELINEVIEW_H
#define KPRANIMATIONSTIMELINEVIEW_H

#include <QWidget>
class QTableView;
class KPrAnimationsDataModel;
class KPrTimeLineView;
class QScrollArea;
class QModelIndex;
class TimeLineWidget;
class KPrTimeLineHeader;
class QColor;

/**
  Main Animations Time Line Widget it depends on
  KPrTimeLineHeader and KPrTimeLineView
  */

class KPrAnimationsTimeLineView : public QWidget
{
    Q_OBJECT
public:
    explicit KPrAnimationsTimeLineView(QWidget *parent = 0);
    void setModel(KPrAnimationsDataModel *model);
    void resizeEvent(QResizeEvent *event);
    KPrAnimationsDataModel *model();
    int widthOfColumn(int column) const;
    void setSelectedRow(int row);
    void setSelectedColumn(int column);
    void setCurrentIndex(const QModelIndex &index);
    int rowsHeigth() const;
    int totalWidth() const;
    int selectedRow() const {return m_selectedRow;}
    int selectedColumn() const {return m_selectedColumn;}
    void paintItemBorder(QPainter *painter, const QPalette &palette, const QRect &rect);
    QScrollArea *scrollArea() const;
    int numberOfSteps() const;
    void setNumberOfSteps(int steps);
    void incrementScale();
    void adjustScale();
    int stepsScale();
    qreal maxLineLength() const;
    void setMaxLineLength(qreal length);
    QColor colorforRow(int row);
    int rowCount() const;

signals:
    void clicked(const QModelIndex&);
public slots:
    void update();
    void updateColumnsWidth();

private:
    KPrTimeLineView *m_view;
    KPrTimeLineHeader *m_header;
    KPrAnimationsDataModel *m_model;
    int m_selectedRow;
    int m_selectedColumn;
    QScrollArea *m_scrollArea;
    int m_rowsHeigth;
    int m_stepsNumber;
    int m_scaleOversize;
    int m_nameWidth;
    int m_orderColumnWidth;
    int m_shapeNameColumnWidth;
    qreal m_maxLength;
};

#endif // KPRANIMATIONSTIMELINEVIEW_H
