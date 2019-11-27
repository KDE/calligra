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
class KPrShapeAnimations;
class KPrTimeLineView;
class QScrollArea;
class QModelIndex;
class KPrTimeLineHeader;
class QColor;
class KPrAnimationGroupProxyModel;


/**
 Animations Time Line Widget for groups of time related animations
 it depends on KPrTimeLineHeader and KPrTimeLineView
  */
class KPrAnimationsTimeLineView : public QWidget
{
    Q_OBJECT
public:
    explicit KPrAnimationsTimeLineView(QWidget *parent = 0);

    /**
     * @brief Set the filter model (model that returns animations belonging to the same group)
     *
     * @param model a KPrAnimationGroupProxyModel pointer
     */
    void setModel(KPrAnimationGroupProxyModel *model);
    void resizeEvent(QResizeEvent *event) override;

    /**
      * Return the filter model for animation groups
      *
      * @return a KPrAnimationGroupProxyModel pointer
      */
    KPrAnimationGroupProxyModel *model();

    /**
      * Return the main model for animations
      *
      * @return an KPrShapeAnimations pointer
      */
    KPrShapeAnimations *animationsModel();

    /**
      * Return the current index
      *
      * @return a QModelIndex holding current index
      */
    QModelIndex currentIndex();

    /**
     * @brief Set the current index
     *
     * @param index
     */
    void setCurrentIndex(const QModelIndex &index);

    int rowCount() const;
    QSize sizeHint() const override;

    /** Helper classes to get the column range that the view has to display*/
    int startColumn() const;
    int endColumn() const;

    /** Helper classes holding the custom animations table view and the header */
    friend class KPrTimeLineView;
    friend class KPrTimeLineHeader;

Q_SIGNALS:
    /// emitted if an item is clicked (return index of the item clicked)
    void clicked(const QModelIndex&);

    /// emitted if an item time range has changed (return the index of the item changed)
    void timeValuesChanged(const QModelIndex&);

    /// emitted if the layout has changed
    void layoutChanged();

    /// emitted if the context menu is called
    void customContextMenuRequested(const QPoint &pos);

public Q_SLOTS:
    /// updates all widget
    void update();

    /// recalculate column preferred width and update widget
    void updateColumnsWidth();

    void resetData();

    /// Change the scale if an item has exceded the scale limit
    /// or the items are too short for the current scale
    void adjustScale();

    /// helper slot to emit timeValuesChanged signal mapping index to the main model
    void notifyTimeValuesChanged(const QModelIndex& index);

    /// helper slot to emit customContextMenuRequested signal mapping pos to the parent widget
    void requestContextMenu(QPoint pos);

protected:
    /// return width of column
    int widthOfColumn(int column) const;

    /// Set selected row and update view
    void setSelectedRow(int row);

    /// Set selected column and update view
    void setSelectedColumn(int column);

    /// Return row height
    int rowsHeight() const;

    /// Calculate width necessary to display all columns
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
    void incrementScale(int step = 1);
    void changeStartLimit(const int row);
    int stepsScale();
    qreal maxLineLength() const;
    void setMaxLineLength(qreal length);

    /// Return color of the bar depending on animation type: entrance, exit, etc.
    QColor barColor(int row);

private:
    /// Calculate where the animation bar has to start depending on the type (on click, with previous, after previous)
    int calculateStartOffset(int row) const;

    KPrTimeLineView *m_view;
    KPrTimeLineHeader *m_header;
    KPrAnimationGroupProxyModel *m_model;
    KPrShapeAnimations *m_shapeModel;
    int m_selectedRow;
    int m_selectedColumn;
    QScrollArea *m_scrollArea;
    int m_rowsHeight;
    int m_stepsNumber;
    int m_scaleOversize;
    qreal m_maxLength;
};

#endif // KPRANIMATIONSTIMELINEVIEW_H
