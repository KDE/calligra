/* This file is part of the KDE project
 * Copyright (C) 2008 Jan Hambrecht <jaham@gmx.net>
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
 */

#ifndef KARBONZOOMCONTROLLER_H
#define KARBONZOOMCONTROLLER_H

#include <KoZoomMode.h>
#include <QtCore/QObject>

class KoCanvasController;
class KActionCollection;
class KoZoomAction;
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
     */
    KarbonZoomController(KoCanvasController *controller, KActionCollection *actionCollection);

    /// destructor
    ~KarbonZoomController();

    /// returns the zoomAction that is maintained by this controller
    KoZoomAction *zoomAction() const;

    /**
     * Alter the current zoom mode which updates the Gui.
     * @param mode the new mode that will be used to auto-calculate a new zoom-level if needed.
     */
    void setZoomMode(KoZoomMode::Mode mode);

signals:
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

public slots:
    /**
     * Set the zoom and the zoom mode for this zoom Controller.  Typically for use just after construction
     * to restore the persistent data.
     */
    void setZoom(KoZoomMode::Mode mode, qreal zoom);

    /**
     * Set the size of the current page in document coordinates which allows zoom modes that use the pageSize
     * to update.
     * @param pageSize the new page size in points
     */
    void setPageSize(const QSizeF &pageSize);

private slots:
    /// so we know when the canvasController changes size
    void setAvailableSize();

    /// when the canvas controller wants us to change zoom
    void requestZoomBy(const qreal factor);

    /// so we know when the page size changed
    void resourceChanged(int key, const QVariant & value);

private:
    class Private;
    Private * const d;
};

#endif // KARBONZOOMCONTROLLER_H
