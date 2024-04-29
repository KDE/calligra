/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2008 Jan Hambrecht <jaham@gmx.net>
 * SPDX-FileCopyrightText: 2010 Boudewijn Rempt <boud@kogmbh.com>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#ifndef KARBONZOOMCONTROLLER_H
#define KARBONZOOMCONTROLLER_H

#include <KoZoomMode.h>
#include <QObject>

class KoCanvasController;
class KActionCollection;
class KoZoomAction;
class QPointF;
class QSizeF;

class KarbonZoomController : public QObject
{
    Q_OBJECT
public:
    /**
     * Constructor. Create one per canvasController. The zoomAction is created in the constructor and will
     * be available to the passed actionCollection for usage by XMLGui.
     * @param controller the canvasController
     * @param actionCollection the action collection where the KoZoomAction is added to
     * @param parent the parent QObject
     */
    KarbonZoomController(KoCanvasController *controller, KActionCollection *actionCollection, QObject *parent = nullptr);

    /// destructor
    ~KarbonZoomController();

    /// returns the zoomAction that is maintained by this controller
    KoZoomAction *zoomAction() const;

    /**
     * Alter the current zoom mode which updates the Gui.
     * @param mode the new mode that will be used to auto-calculate a new zoom-level if needed.
     */
    void setZoomMode(KoZoomMode::Mode mode);

Q_SIGNALS:
    /**
     * Signal is triggered when the user clicks the zoom to selection button.
     * Nothing else happens except that this signal is emitted.
     */
    void zoomedToSelection();

    /**
     * Signal is triggered when the user clicks the zoom to all button.
     * Nothing else happens except that this signal is emitted.
     */
    void zoomedToAll();

public Q_SLOTS:

    /**
     * Set the zoom and the zoom mode for this zoom Controller.
     * Typically for use just after construction to restore the
     * persistent data.
     *
     * @param mode new zoom mode for the canvas
     * @param zoom (for ZOOM_CONSTANT zoom mode only) new zoom value for
     *             the canvas
     * @param stillPoint (for ZOOM_CONSTANT zoom mode only) the point
     *                   which will not change its position in widget
     *                   during the zooming. It is measured in view
     *                   coordinate system *before* zoom.
     */
    void setZoom(KoZoomMode::Mode mode, qreal zoom, const QPointF &stillPoint);

    /**
     * Convenience function with @p center always set to the current
     * center point of the canvas
     */
    void setZoom(KoZoomMode::Mode mode, qreal zoom);

    /**
     * Set the size of the current page in document coordinates which allows zoom modes that use the pageSize
     * to update.
     * @param pageSize the new page size in points
     */
    void setPageSize(const QSizeF &pageSize);

private Q_SLOTS:
    /// so we know when the canvasController changes size
    void setAvailableSize();

    /// when the canvas controller wants us to change zoom
    void requestZoomRelative(const qreal factor, const QPointF &stillPoint);

    /// so we know when the page size changed
    void canvasResourceChanged(int key, const QVariant &value);

    /// zoom in relative to current zoom
    void zoomInRelative();

    /// zoom out relative to current zoom
    void zoomOutRelative();

private:
    void requestZoomBy(const qreal factor);

private:
    class Private;
    Private *const d;
};

#endif // KARBONZOOMCONTROLLER_H
