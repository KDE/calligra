/*
 * This file is part of the KDE project
 *
 * Copyright (C) 2013 Arjen Hiemstra <ahiemstra@heimr.nl>
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
 *
 */

#ifndef CALLIGRA_COMPONENTS_VIEWCONTROLLER_H
#define CALLIGRA_COMPONENTS_VIEWCONTROLLER_H

#include <QQuickItem>

namespace Calligra {
namespace Components {

class View;

/**
 * \brief Provides an object to control the document transformation within a View.
 *
 * Due to API restrictions it is not easily possible to inherit Flickable's behaviour
 * in custom QtQuick items. This object works around that by providing an item that
 * can be put inside a Flickable and will scroll a View object based on the Flickable's
 * scrolling.
 *
 * In addition, this object controls the zoom level of a view. It can clamps zoom to a
 * minimum and maximum value and can optionally use an image during zoom operations to
 * accelerate the zoom operation, since zooming an actual document can be an expensive
 * operation.
 *
 * \note This object will control its own width and height as well as the contentWidth
 * and contentHeight of #flickable. While it is possible to put this item outside of a
 * flickable this is not recommended.
 */

class ViewController : public QQuickItem
{
    Q_OBJECT
    /**
     * \property view
     * \brief The view to control.
     *
     * \default null
     * \get view() const
     * \set setView()
     * \notify viewChanged()
     */
    Q_PROPERTY(Calligra::Components::View* view READ view WRITE setView NOTIFY viewChanged)
    /**
     * \property flickable
     * \brief The flickable that is used to control scrolling.
     *
     * \default null
     * \get flickable() const
     * \set setFlickable()
     * \notify flickableChanged()
     */
    Q_PROPERTY(QQuickItem* flickable READ flickable WRITE setFlickable NOTIFY flickableChanged)
    /**
     * \property minimumZoom
     * \brief The minimum zoom level.
     *
     * \default 0.5
     * \get minimumZoom() const
     * \set setMinimumZoom()
     * \notify minimumZoomChanged()
     */
    Q_PROPERTY(float minimumZoom READ minimumZoom WRITE setMinimumZoom NOTIFY minimumZoomChanged)
    /**
     * \property minimumZoomFitsWidth
     * \brief Should the ViewController determine minimum zoom level automatically?
     *
     * When true, the minimum zoom level will be determined automatically  based on the width of
     * the item assigned to this controller's #flickable property.
     *
     * \default false
     * \get minimumZoomFitsWidth() const
     * \set setMinimumZoomFitsWidth()
     * \notify minimumZoomFitsWidthChanged()
     */
    Q_PROPERTY(bool minimumZoomFitsWidth READ minimumZoomFitsWidth WRITE setMinimumZoomFitsWidth NOTIFY minimumZoomFitsWidthChanged)
    /**
     * \property zoom
     * \brief The zoom amount of the view.
     *
     * \default 1.0
     * \get zoom() const
     * \set setZoom()
     * \notify zoomChanged()
     */
    Q_PROPERTY(float zoom READ zoom WRITE setZoom NOTIFY zoomChanged)
    /**
     * \property maximumZoom
     * \brief The maximum zoom amount.
     *
     * \default 2.0
     * \get maximumZoom() const
     * \set setMaximumZoom()
     * \notify maximumZoomChanged()
     */
    Q_PROPERTY(float maximumZoom READ maximumZoom WRITE setMaximumZoom NOTIFY maximumZoomChanged)
    /**
     * \property useZoomProxy
     * \brief Should the ViewController use a proxy image when zooming?
     *
     * When true, during zooming, apply the zoom to a proxy image that is scaled and shown for a
     * short duration before applying it to the view. This makes continuous zooming operations
     * far smoother.
     *
     * \default true
     * \get useZoomProxy() const
     * \set setUseZoomProxy()
     * \notify useZoomProxyChanged()
     */
    Q_PROPERTY(bool useZoomProxy READ useZoomProxy WRITE setUseZoomProxy NOTIFY useZoomProxyChanged)

public:
    /**
     * Constructor.
     *
     * \param parent The parent item.
     */
    explicit ViewController(QQuickItem* parent = 0);
    /**
     * Destructor.
     */
    ~ViewController() override;

    /**
     * Getter for property #view
     */
    View* view() const;
    /**
     * Setter for property #view
     */
    void setView(View* newView);
    /**
     * Getter for property #flickable
     */
    QQuickItem* flickable() const;
    /**
     * Setter for property #flickable
     */
    void setFlickable(QQuickItem* item);
    /**
     * Getter for property #minimumZoom
     */
    float minimumZoom() const;
    /**
     * Setter for property #minimumZoom
     */
    void setMinimumZoom(float newValue);
    /**
     * Getter for property #minimumZoomFitsWidth
     */
    bool minimumZoomFitsWidth() const;
    /**
     * Setter for property #minimumZoomFitsWidth
     */
    void setMinimumZoomFitsWidth(bool newValue);
    /**
     * Getter for property #zoom
     */
    float zoom() const;
    /**
     * Setter for property #zoom
     */
    void setZoom(float newZoom);
    /**
     * Getter for property #maximumZoom
     */
    float maximumZoom() const;
    /**
     * Setter for property #maximumZoom
     */
    void setMaximumZoom(float newValue);
    /**
     * Getter for property #useZoomProxy
     */
    bool useZoomProxy() const;
    /**
     * Setter for property #useZoomProxy
     */
    void setUseZoomProxy(bool proxy);

    /**
     * Reimlemented from superclass
     */
    bool event(QEvent* event) override;
public Q_SLOTS:
    /**
     * \brief Zoom by a specific amount around a centre point.
     *
     * \note The centre point should be relative to this item. The easiest way
     * to achieve this in QML is to use the following:
     *
     * \code
     * var newCenter = mapToItem(controller, center.x, center.y);
     * controller.zoomAroundPoint(amount, newCenter.x, newCenter.y);
     * \endcode
     *
     * \param amount The amount to zoom by, relative to the current zoom level.
     * \param x The horizontal coordinate of the centre point.
     * \param y The vertical coordinate of the centre point.
     */
    void zoomAroundPoint(float amount, float x, float y);
    /**
     * \brief Zoom the item such that it will fit a certain width.
     *
     * This will zoom the view to make sure that the document fits the specified
     * width, as long as the resulting zoom level is not less than #minimumZoom or
     * more than #maximumZoom.
     *
     * \param width The width to fit to.
     */
    void zoomToFitWidth(float width);

Q_SIGNALS:
    /**
     * Notify signal for property #view.
     */
    void viewChanged();
    /**
     * Notify signal for property #flickable.
     */
    void flickableChanged();
    /**
     * Notify signal for property #minimumZoom.
     */
    void minimumZoomChanged();
    /**
     * Notify signal for property #minimumZoomFitsWidth.
     */
    void minimumZoomFitsWidthChanged();
    /**
     * Notify signal for property #zoom.
     */
    void zoomChanged();
    /**
     * Notify signal for property #maximumZoom.
     */
    void maximumZoomChanged();
    /**
     * Notify signal for property #useZoomProxy.
     */
    void useZoomProxyChanged();

protected:
    QSGNode* updatePaintNode(QSGNode* node, QQuickItem::UpdatePaintNodeData* ) override;

private Q_SLOTS:
    void documentChanged();
    void contentPositionChanged();
    void documentSizeChanged();
    void documentStatusChanged();
    void documentOffsetChanged(const QPoint& offset);
    void zoomTimeout();
    void flickableWidthChanged();

private:
    class Private;
    Private* const d;
};

} // Namespace Components
} // Namepsace Calligra

#endif // CALLIGRA_COMPONENTS_VIEWCONTROLLER_H
