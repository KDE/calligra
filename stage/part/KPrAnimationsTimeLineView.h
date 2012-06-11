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

#include "stage_export.h"

/**
  Main Animations Time Line Widget it depends on
  KPrTimeLineHeader and KPrTimeLineView
  */

class STAGE_EXPORT  KPrAnimationsTimeLineView : public QWidget
{
    Q_OBJECT
public:
    explicit KPrAnimationsTimeLineView(QWidget *parent = 0);
    void setModel(KPrAnimationsDataModel *model);
    void resizeEvent(QResizeEvent *event);
    KPrAnimationsDataModel *model();
    QModelIndex currentIndex();
    void setCurrentIndex(const QModelIndex &index);
    int rowCount() const;
    friend class KPrTimeLineView;
    friend class KPrTimeLineHeader;

signals:
    void clicked(const QModelIndex&);

public slots:
    //updates all widget
    void update();

    //recalculate column preferred width and update widget
    void updateColumnsWidth();

protected:
    /// return width of column
    int widthOfColumn(int column) const;

    /// Set selected row and update view
    void setSelectedRow(int row);

    /// Set selected column and update view
    void setSelectedColumn(int column);

    /// Return row heigth
    int rowsHeigth() const;

    /// Calculate width necesary to display all columns
    int totalWidth() const;

    /// Returns selected row and column
    int selectedRow() const {return m_selectedRow;}
    int selectedColumn() const {return m_selectedColumn;}

    /// Helper method to paint border for items on the view
    void paintItemBorder(QPainter *painter, const QPalette &palette, const QRect &rect);

    /// Scroll area that contains the view
    QScrollArea *scrollArea() const;


    /// Helper methods to manage the time scales in view and header
    int numberOfSteps() const;
    void setNumberOfSteps(int steps);
    void incrementScale();
    void adjustScale();
    int stepsScale();
    qreal maxLineLength() const;
    void setMaxLineLength(qreal length);

    /// Return color of the bar depending on animation type: entrance, exit, etc.
    QColor colorforRow(int row);

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
