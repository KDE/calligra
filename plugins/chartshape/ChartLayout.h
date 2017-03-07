/* This file is part of the KDE project

   Copyright 2017 Dag Andersen <danders@get2net.dk>
   Copyright 2010 Johannes Simon <johannes.simon@gmail.com>

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License as published by the Free Software Foundation; either
   version 2 of the License, or (at your option) any later version.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this library; see the file COPYING.LIB.  If not, write to
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
*/

#ifndef KCHART_LAYOUT_H
#define KCHART_LAYOUT_H

// Qt
#include <QList>
#include <QMap>
#include <QSizeF>

// Calligra
#include <KoShapeContainerModel.h>

// KoChart
#include "kochart_global.h"



namespace KoChart {

/**
 * A generic chart-style layout with 10 possible positions:
 *
 *  ----------------------
 * | A |       D      | F |
 * |---|------------------|
 * |   |              |   |
 * | B |       I      | G |      (J)
 * |   |              |   |
 * |---|------------------|
 * | C |       E      | H |
 *  ----------------------
 *
 * A - TopStartPosition
 * B - StartPosition
 * C - BottomStartPosition
 * D - TopPosition
 * E - BottomPosition
 * F - TopEndPosition
 * G - EndPosition
 * H - BottomEndPosition
 * I - CenterPosition
 * J - FloatingPosition
 *
 * Layout elements with the same position that are in one of the positions B, D, E or G
 * will be placed more towards the center based on item type.
 * 
 * Note that layouting is heavily dependent on that the CenterPosition is
 * occopied by a PlotArea shape. If this is not the case the result is unpredictable.
 * 
 * This should not be a problem, a chart without a PlotArea in the middle is hardly a chart.
 */
class ChartLayout : public KoShapeContainerModel
{
public:
    ChartLayout();
    ~ChartLayout();

    /**
     * Adds a floating shape to the layout.
     */
    void add(KoShape *shape);

    /**
     * Adds a shape to the layout.
     *
     * @param pos    position in the layout
     * @param weight priority of this shape in regard to its placement when
     *               other shapes are in the same Position.
     *               A shape with a higher weight will be placed more towards
     *               the center (i.e., it "sinks" due to its higher weight)
     */
    void add(KoShape *shape, Position pos, int weight = 0);

    /**
     * Removes a shape from the layout.
     */
    void remove(KoShape *shape);

    /**
     * Turns clipping of a shape on or off.
     */
    void setClipped(const KoShape *shape, bool clipping);

    /**
     * @see setClipping
     */
    bool isClipped(const KoShape *shape) const;

    /// reimplemented
    virtual void setInheritsTransform(const KoShape *shape, bool inherit);
    /// reimplemented
    virtual bool inheritsTransform(const KoShape *shape) const;

    /**
     * Returns the number of shapes in this layout.
     */
    int count() const;

    /**
     * Returns a list of shapes in this layout.
     */
    QList<KoShape*> shapes() const;

    /**
     * Called whenever a property of the container (i.e. the ChartShape) is changed.
     */
    void containerChanged(KoShapeContainer *container, KoShape::ChangeType type);

    /**
     * Returns whether a shape is locked for user modifications.
     */
    bool isChildLocked(const KoShape *shape) const;

    /**
     * Changes the layout position of a shape that is already contained
     * in this layout.
     */
    void setPosition(const KoShape *shape, Position pos, int weight = 0);

    /**
     * Called whenever a property of a shape in this layout has changed.
     *
     * All layout items effected by this change will be re-layouted.
     */
    void childChanged(KoShape *shape, KoShape::ChangeType type);

    /**
     * Does the layouting of shapes that have changed its size or position or
     * that were effected by one of these changes.
     *
     * Only does a relayout if one has been schedules previously through
     * scheduleRelayout().
     *
     * \see scheduleRelayout
     */
    void layout();

    /**
     * Schedules a relayout that is to be done when layout() is called.
     *
     * \see layout
     */
    void scheduleRelayout();
    /**
     * Sets the horizontal and vertical margin that will be applied during layout
     */
    void setMargins (qreal hMargin, qreal vMargin);
    /// Returns the margins defined for this layout
    QPointF margins() const;
    /// Set spacing in points to @p hSpacing, @p vSpacing to be used for this layout
    void setSpacing(qreal hSpacing, qreal vSpacing);
    /// Returns the horizontal and vertical spacing in points defined for this layout
    /// Default: uses the margins
    QPointF spacing() const;

    /**
     * Calculates the layouting of the shapes with @p shape visibility @p show,
     * in accordance with the @p shapes default positioning set
     * when the shapes where added to this layout.
     * 
     * This will normally be used when @p shape changes visibility, but can also
     * be used to trigger relayout due to other changes in shape (resize).
     * 
     * Note that shape->isVisible() together with @p show is used to determine
     * if the shape changes visibility, hence you should call this method
     * before you call shape->setVisible()
     *
     * Returns the new position and size of shapes that must be moved and/or resized
     */
    QMap<KoShape*, QRectF> calculateLayout(const KoShape *shape, bool show) const;

    /// Enable/disable automatic layout
    void setAutoLayoutEnabled(bool on);

private:
    /// Set the chart size to @p size and schedule relayout
    void setContainerSize(const QSizeF &size);

    /**
     * Lays out all items in TopPosition, and returns the y value of
     * the bottom-most item's bottom.
     */
    qreal layoutTop(const QMap<int, KoShape*>& shapes, KoShape *p);

    /**
     * Lays out all items in BottomPosition, and returns the y value of
     * the top-most item's top.
     */
    qreal layoutBottom(const QMap<int, KoShape*>& shapes, KoShape *p);

    /**
     * Lays out all items in StartPosition, and returns the x value of
     * the right-most item's right.
     */
    qreal layoutStart(const QMap<int, KoShape*>& shapes, KoShape *p);

    /**
     * Lays out all items in EndPosition, and returns the x value of
     * the left-most item's left.
     */
    qreal layoutEnd(const QMap<int, KoShape*>& shapes, KoShape *p);

    void layoutTopStart(KoShape *shape, KoShape *p);
    void layoutBottomStart(KoShape *shape, KoShape *p);
    void layoutTopEnd(KoShape *shape, KoShape *p);
    void layoutBottomEnd(KoShape *shape, KoShape *p);

    QMap<KoShape*, QRectF> calculateLayoutTop(KoShape *shape, KoShape *center, bool hide) const;
    QMap<KoShape*, QRectF> calculateLayoutBottom(KoShape *shape, KoShape *center, bool hide) const;
    QMap<KoShape*, QRectF> calculateLayoutStart(KoShape *shape, KoShape *center, bool hide) const;
    QMap<KoShape*, QRectF> calculateLayoutEnd(KoShape *shape, KoShape *center, bool hide) const;
    QMap<KoShape*, QRectF> calculateLayoutTopStart(KoShape *shape, KoShape *center, bool hide) const;
    QMap<KoShape*, QRectF> calculateLayoutBottomStart(KoShape *shape, KoShape *center, bool hide) const;
    QMap<KoShape*, QRectF> calculateLayoutTopEnd(KoShape *shape, KoShape *center, bool hide) const;
    QMap<KoShape*, QRectF> calculateLayoutBottomEnd(KoShape *shape, KoShape *center, bool hide) const;

    QRectF layoutArea(Position area, const KoShape *plotArea) const;
    Position itemArea(const KoShape *item, const KoShape *plotArea) const;
    /// Returns the position where the @p shape shall be inserted.
    /// @p defaultValue is used if position cannot/shall not be calculated (eg shape layout position is FloatingPosition)
    /// @p itemstart and @p itemend the shapes start and end
    /// @p areastart and @p areacend the start and end of the area in which the item shall be positioned
    qreal itemDefaultPosition(const KoShape *shape, qreal defaultValue, qreal itemstart, qreal itemend, qreal areastart, qreal areaend) const;

    bool isShapeToBeMoved(const KoShape *shape, Position area, const KoShape *plotArea) const;

#ifdef COMPILING_TESTS
public:
#endif
    QString dbg(const KoShape *shape) const;
    static qreal relativePosition(qreal start1, qreal length1, qreal start2, qreal length2, qreal start, qreal length);
    static QPointF itemPosition(const KoShape *shape);
    static QSizeF  itemSize(const KoShape *shape);
    static QRectF  itemRect(const KoShape *shape);
    static void    setItemPosition(KoShape *shape, const QPointF& pos);
    /// Returns the plot area minus axis labels
    static QRectF diagramArea(const KoShape *shape);
    /// Returns the @p rect minus axis labels
    static QRectF diagramArea(const KoShape *shape, const QRectF &rect);

private:
    bool m_doingLayout;
    bool m_relayoutScheduled;
    QSizeF m_containerSize;
    qreal m_hMargin;
    qreal m_vMargin;
    QPointF m_spacing;
    bool m_autoLayoutEnabled;
    class LayoutData;
    QMap<KoShape*, LayoutData*> m_layoutItems;
};

} // namespace KoChart

#endif // KCHART_CHARTLAYOUT_H
