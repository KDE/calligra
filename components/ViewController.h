/*
 * This file is part of the KDE project
 * Copyright (C) 2013 Arjen Hiemstra <ahiemstra@heimr.nl>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
 */

#ifndef CALLIGRA_COMPONENTS_VIEWCONTROLLER_H
#define CALLIGRA_COMPONENTS_VIEWCONTROLLER_H

#include <QtQuick/QQuickPaintedItem>

namespace Calligra {
namespace Components {

class View;

/**
 * \brief Provides an object to control the document transformation within a View.
 *
 */

class ViewController : public QQuickPaintedItem
{
    Q_OBJECT
    /**
     * \property view
     * \brief The view to control.
     *
     * \get view() const
     * \set setView()
     * \notify viewChanged()
     */
    Q_PROPERTY(Calligra::Components::View* view READ view WRITE setView NOTIFY viewChanged)
    /**
     * \property flickable
     * \brief The flickable that is used to control scrolling.
     *
     * \get flickable() const
     * \set setFlickable()
     * \notify flickableChanged()
     */
    Q_PROPERTY(QQuickItem* flickable READ flickable WRITE setFlickable NOTIFY flickableChanged)
    /**
     * \property minimumZoom
     * \brief The minimum zoom level.
     *
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
     * \get minimumZoomFitsWidth() const
     * \set setMinimumZoomFitsWidth()
     * \notify minimumZoomFitsWidthChanged()
     */
    Q_PROPERTY(bool minimumZoomFitsWidth READ minimumZoomFitsWidth WRITE setMinimumZoomFitsWidth NOTIFY minimumZoomFitsWidthChanged)
    /**
     * \property zoom
     * \brief The zoom amount of the view.
     *
     * \get zoom() const
     * \set setZoom()
     * \notify zoomChanged()
     */
    Q_PROPERTY(float zoom READ zoom WRITE setZoom NOTIFY zoomChanged)
    /**
     * \property maximumZoom
     * \brief The maximum zoom amount.
     *
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
     * \get useZoomProxy() const
     * \set setUseZoomProxy()
     * \notify useZoomProxyChanged()
     */
    Q_PROPERTY(bool useZoomProxy READ useZoomProxy WRITE setUseZoomProxy NOTIFY useZoomProxyChanged)

public:
    ViewController(QQuickItem* parent = 0);
    virtual ~ViewController();

    /**
     * Inherited from QQuickPaintedItem.
     */
    virtual void paint(QPainter* painter) Q_DECL_OVERRIDE;
    /**
     * Getter for property #view
     */
    View* view() const;
    /**
     * Getter for property #flickable
     */
    QQuickItem* flickable() const;
    /**
     * Getter for property #minimumZoom
     */
    float minimumZoom() const;
    /**
     * Getter for property #minimumZoomFitsWidth
     */
    bool minimumZoomFitsWidth() const;
    /**
     * Getter for property #zoom
     */
    float zoom() const;
    /**
     * Getter for property #maximumZoom
     */
    float maximumZoom() const;
    /**
     * Getter for property #useZoomProxy
     */
    bool useZoomProxy() const;

public Q_SLOTS:
    /**
     * Setter for property #view
     */
    void setView(View* newView);
    /**
     * Setter for property #flickable
     */
    void setFlickable(QQuickItem* item);
    /**
     * Setter for property #minimumZoom
     */
    void setMinimumZoom(float newValue);
    /**
     * Setter for property #minimumZoomFitsWidth
     */
    void setMinimumZoomFitsWidth(bool newValue);
    /**
     * Setter for property #zoom
     */
    void setZoom(float newZoom);
    /**
     * Setter for property #maximumZoom
     */
    void setMaximumZoom(float newValue);
    /**
     * Setter for property #useZoomProxy
     */
    void setUseZoomProxy(bool proxy);

    
    void zoomAroundPoint(float amount, const QPointF& point);
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

private Q_SLOTS:
    void documentChanged();
    void contentPositionChanged();
    void documentSizeChanged();

private:
    class Private;
    Private* const d;
};

} // Namespace Components
} // Namepsace Calligra

#endif // CALLIGRA_COMPONENTS_VIEWCONTROLLER_H
