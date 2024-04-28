/* This file is part of the KDE project

   SPDX-FileCopyrightText: 2017 Dag Andersen <danders@get2net.dk>
   SPDX-FileCopyrightText: 2010 Johannes Simon <johannes.simon@gmail.com>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef KCHART_LAYOUT_H
#define KCHART_LAYOUT_H

// Qt
#include <QList>
#include <QMap>
#include <QSizeF>

// Calligra
#include <KoInsets.h>
#include <KoShapeContainerModel.h>

// KoChart
#include "kochart_global.h"

namespace KoChart
{

class PlotArea;

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
 * A chart can consist of the following elements:
 *
 * Title: Positioned at the top in (D)
 * Sub-title: Positioned below Title in (D)
 * Footer: Position at the bottom in (E)
 * Legend: Positiond according to legend-position and alignmnent
 *         Note that Legend is the only component with this level of layout control from odf
 *
 * Plot area: Positioned in the center (I)
 * Axes: Attached to the plot area
 * Axis title: Positioned beside the axis it describes
 *
 * Each of these elements may be manually positioned and sized,
 * but by default the layouting does the job.
 */
class ChartLayout : public KoShapeContainerModel
{
public:
    ChartLayout();
    ~ChartLayout();

    /**
     * Add @a shape and register it with GenericItemType
     *
     * This is normally called from the resource manager.
     *
     * Note that this shape will not be layed out until it gets a proper item type
     * set with setItemType()
     *
     * Abstract in KoShapeContainerModel so needs to be here.
     *
     * @see setItemType()
     */
    void add(KoShape *shape) override;

    /**
     * Removes a shape from the layout.
     */
    void remove(KoShape *shape) override;

    /**
     * Turns clipping of a shape on or off.
     */
    void setClipped(const KoShape *shape, bool clipping) override;

    /**
     * @see setClipping
     */
    bool isClipped(const KoShape *shape) const override;

    /// reimplemented
    void setInheritsTransform(const KoShape *shape, bool inherit) override;
    /// reimplemented
    bool inheritsTransform(const KoShape *shape) const override;

    /**
     * Returns the number of shapes in this layout.
     */
    int count() const override;

    /**
     * Returns a list of shapes in this layout.
     */
    QList<KoShape *> shapes() const override;

    /**
     * Called whenever a property of the container (i.e. the ChartShape) is changed.
     */
    void containerChanged(KoShapeContainer *container, KoShape::ChangeType type) override;

    /**
     * Returns whether a shape is locked for user modifications.
     */
    bool isChildLocked(const KoShape *shape) const override;

    /**
     * Changes the item type of the shape to @p itemType
     * The @p itemType controls where the shape is layed out
     */
    void setItemType(const KoShape *shape, ItemType itemType);

    void proposeMove(KoShape *child, QPointF &move) override;

    /**
     * Called whenever a property of a shape in this layout has changed.
     *
     * All layout items effected by this change will be re-layouted.
     */
    void childChanged(KoShape *shape, KoShape::ChangeType type) override;

    /**
     * Does the layouting of all visible shapes
     *
     * Only does a relayout if one has been schedules previously through
     * scheduleRelayout().
     *
     * \see scheduleRelayout
     */
    void layout();
    /// Calculates the layout used by @a layout()
    void calculateLayout();

    /**
     * Schedules a relayout that is to be done when layout() is called.
     *
     * \see layout
     */
    void scheduleRelayout();
    /**
     * Sets the padding to @p padding that will be applied during layout
     */
    void setPadding(const KoInsets &padding);
    /// Returns the padding defined for this layout
    KoInsets padding() const;
    /// Set spacing in points to @p hSpacing, @p vSpacing to be used for this layout
    void setSpacing(qreal hSpacing, qreal vSpacing);
    /// Returns the horizontal and vertical spacing in points defined for this layout
    QPointF spacing() const;

    /// Enable/disable layouting (ex: used during odf loading)
    void setLayoutingEnabled(bool value);

private:
    /// Set the chart size to @p size and schedule relayout
    void setContainerRect(const QRectF &rect);

    qreal xOffset(const QRectF &left, const QRectF &right, bool center = false) const;
    qreal yOffset(const QRectF &top, const QRectF &bottom, bool center = false) const;

    void rotateAxisTitles(PlotArea *plotarea);

    void layoutAxisTitles(int pos, const QMultiMap<int, KoShape *> &map, const QRectF &rect, const QRectF plotarea) const;

#ifdef COMPILING_TESTS
public:
#endif
    QString dbg(const QList<KoShape *> &shapes) const;
    QString dbg(const KoShape *shape) const;
    static qreal relativePosition(qreal start1, qreal length1, qreal start2, qreal length2, qreal start, qreal length);
    static QPointF itemPosition(const KoShape *shape);
    static QSizeF itemSize(const KoShape *shape);
    static void setItemSize(KoShape *shape, const QSizeF &size);
    static QRectF itemRect(const KoShape *shape);
    static void setItemPosition(KoShape *shape, const QPointF &pos);
    /// Returns the plot area minus axis labels
    static QRectF diagramArea(const KoShape *shape);
    /// Returns the @p rect minus axis labels
    static QRectF diagramArea(const KoShape *shape, const QRectF &rect);

    static bool autoPosition(const KoShape *shape);
    static bool autoSize(const KoShape *shape);

private:
    bool m_doingLayout;
    bool m_relayoutScheduled;
    QRectF m_containerRect;
    KoInsets m_padding;
    QPointF m_spacing;
    bool m_layoutingEnabled;
    class LayoutData;
    QMap<KoShape *, LayoutData *> m_layoutItems;
    QMap<int, KoShape *> m_shapes;
};

} // namespace KoChart

#endif // KCHART_CHARTLAYOUT_H
