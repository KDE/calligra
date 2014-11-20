/*
 *  Copyright (c) 2014 Boudewijn Rempt <boud@kde.org>
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
 */
#ifndef KIS_IMAGE_VIEW_H
#define KIS_IMAGE_VIEW_H

#include <KoView.h>
#include <KoColorSpace.h>
#include <KoColorProfile.h>

#include <kis_types.h>

class QCloseEvent;

class KoZoomManager;
class KoZoomController;
class KoCanvasController;

class KisZoomManager;
class KisCanvas2;
class KisView2;
class KisDoc2;
class KisCanvasResourceProvider;
class KisCoordinatesConverter;

#include <krita_export.h>

/**
 * @brief The KisImageView class shows a single document's image in a canvas.
 */
class KRITAUI_EXPORT KisImageView : public KoView
{
    Q_OBJECT
public:
    explicit KisImageView(KoPart *part, KisDoc2 * doc, KoMainWindow *parent);
    virtual ~KisImageView();

    // Temporary while teasing apart view and mainwindow
    void setParentView(KisView2 *view);
    KisView2 *parentView() const;

    // KoView implementation
    virtual void updateReadWrite(bool readwrite) {
        Q_UNUSED(readwrite);
    }

    // KoView implementation
    virtual KoZoomController *zoomController() const;

    /**
     * The zoommanager handles everything action-related to zooming
     */
    KisZoomManager *zoomManager() const;

    /**
     * The CanvasController decorates the canvas with scrollbars
     * and knows where to start painting on the canvas widget, i.e.,
     * the document offset.
     */
    KoCanvasController *canvasController() const;
    KisCanvasResourceProvider *resourceProvider() const;

    /**
     * @return the canvas object
     */
    KisCanvas2 *canvasBase() const;

    /// @return the image this view is displaying
    KisImageWSP image() const;


    KisCoordinatesConverter *viewConverter() const;

    void resetImageSizeAndScroll(bool changeCentering,
                                 const QPointF oldImageStillPoint = QPointF(),
                                 const QPointF newImageStillPoint = QPointF());

    void setCurrentNode(KisNodeSP node);
    KisNodeSP currentNode() const;
    KisLayerSP currentLayer() const;
    KisMaskSP currentMask() const;

    /// Convenience method to get at the active selection (the
    /// selection of the current layer, or, if that does not exist,
    /// the global selection.
    KisSelectionSP selection();

protected:

    // QWidget overrides
    virtual void dragEnterEvent(QDragEnterEvent * event);
    virtual void dropEvent(QDropEvent * event);
    virtual bool event( QEvent* event );
    virtual void closeEvent(QCloseEvent *event);

signals:
    // From KisImage
    void sigSizeChanged(const QPointF &oldStillPoint, const QPointF &newStillPoint);
    void sigProfileChanged(const KoColorProfile *  profile);
    void sigColorSpaceChanged(const KoColorSpace*  cs);

public slots:

private slots:
    void slotLoadingFinished();

private:

    bool queryClose();

    class Private;
    Private * const d;
};

#endif // KIS_IMAGE_VIEW_H
