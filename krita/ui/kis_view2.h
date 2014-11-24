/*
 *  Copyright (c) 2006 Boudewijn Rempt <boud@valdyas.org>
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
 *  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
 */

#ifndef KIS_VIEW_2
#define KIS_VIEW_2


#include <QDockWidget>
#include <QQueue>
#include <KoView.h>
#include <KoProgressUpdater.h>
#include <krita_export.h>
#include <kis_types.h>
#include "kis_floating_message.h"

class KisAction;
class QDragEnterEvent;
class QDropEvent;
class QPoint;

class KisPaintOpPreset;

class KoCanvasController;

class KisCanvas2;
class KisCanvasResourceProvider;
class KisDoc2;
class KisFilterManager;
class KisGridManager;
class KisImage;
class KisImageManager;
class KisNodeManager;
class KisPaintingAssistantsDecoration;
class KisPaintopBox;
class KisPerspectiveGridManager;
class KisSelectionManager;
class KisStatusBar;
class KisUndoAdapter;
class KisZoomManager;
class KisPaintopBox;
class KisCanvasController;
class KisFlipbook;
class KisActionManager;

/**
 * Krita view class
 *
 * Following the broad model-view-controller idea this class shows you one view on the document.
 * There can be multiple views of the same document each in with independent settings for viewMode and zoom etc.
 */
class KRITAUI_EXPORT KisView2 : public KoView
{

    Q_OBJECT

public:
    /**
     * Construct a new view on the krita document.
     * @param document   the document we show.
     * @param parent   a parent widget we show ourselves in.
     */
    KisView2(KoPart *part, KisDoc2 *document, QWidget *parent);
    virtual ~KisView2();

public:

    // QWidget overrides
    virtual void dragEnterEvent(QDragEnterEvent * event);
    virtual void dropEvent(QDropEvent * event);
    virtual bool event(QEvent* event);

    // KoView implementation
    virtual void updateReadWrite(bool readwrite) {
        Q_UNUSED(readwrite);
    }

    // KoView implementation
    virtual KoZoomController *zoomController() const;

public:  // Krita specific interfaces

    /// Return the image this view is displaying
    KisImageWSP image() const;

    /// The resource provider contains all per-view settings, such as
    /// current color, current paint op etc.
    KisCanvasResourceProvider * resourceProvider();

    /// Return the canvasbase class
    KisCanvas2 * canvasBase() const;

    /// Return the actual widget that is displaying the current image
    QWidget* canvas() const;

    /// Return the wrapper class around the statusbar
    KisStatusBar * statusBar() const;

    KisPaintopBox* paintOpBox() const;

    /// create a new progress updater
    KoProgressUpdater* createProgressUpdater(KoProgressUpdater::Mode mode = KoProgressUpdater::Threaded);

    /// The selection manager handles everything action related to
    /// selections.
    KisSelectionManager * selectionManager();

    /// The CanvasController decorates the canvas with scrollbars
    /// and knows where to start painting on the canvas widget, i.e.,
    /// the document offset.
    KoCanvasController * canvasController();
    KisCanvasController *canvasControllerWidget();

    /// The node manager handles everything about nodes
    KisNodeManager * nodeManager();

    KisActionManager* actionManager();
    
    /**
     * Convenience method to get at the active node, which may be
     * a layer or a mask or a selection
     */
    KisNodeSP activeNode();

    /// Convenience method to get at the active layer
    KisLayerSP activeLayer();

    /// Convenience method to get at the active paint device
    KisPaintDeviceSP activeDevice();

    /// The zoommanager handles everything action-related to zooming
    KisZoomManager * zoomManager();

    /// The filtermanager handles everything action-related to filters
    KisFilterManager * filterManager();

    /// The image manager handles everything action-related to the
    /// current image
    KisImageManager * imageManager();

    /// Convenience method to get at the active selection (the
    /// selection of the current layer, or, if that does not exist,
    /// the global selection.
    KisSelectionSP selection();

    /// Checks if the current global or local selection is editable
    bool selectionEditable();

    /// The undo adapter is used to add commands to the undo stack
    KisUndoAdapter * undoAdapter();

    /// Go to all managers and enable or disable all actions and other
    /// gui elements
    void updateGUI();

    KisDoc2* document() const;

public:

    virtual KoPrintJob * createPrintJob();

    KisGridManager * gridManager();
    KisPerspectiveGridManager* perspectiveGridManager();
    KisPaintingAssistantsDecoration* paintingAssistantsDecoration();

    /// disable and enable toolbar controls. used for disabling them during painting.
    void enableControls();
    void disableControls();


    /// shows a floating message in the top right corner of the canvas
    void showFloatingMessage(const QString message, const QIcon& icon, int timeout = 4500,
                             KisFloatingMessage::Priority priority = KisFloatingMessage::Medium,
                             int alignment = Qt::AlignCenter | Qt::TextWordWrap);

    /// The QMainWindow associated with this view. This is most likely going to be shell(), but
    /// when running as Gemini or Sketch, this will be set to the applications' own QMainWindow.
    /// This can be checked by qobject_casting to KoMainWindow to check the difference.
    QMainWindow* qtMainWindow();
    /// The mainWindow function will return the shell() value, unless this function is called
    /// with a non-null value. To make it return shell() again, simply pass null to this function.
    void setQtMainWindow(QMainWindow* newMainWindow);

public slots:

    void slotLoadingFinished();
    void slotSavingFinished();
    void showJustTheCanvas(bool toggled);
    void setShowFloatingMessage(bool show);

signals:

    void sigLoadingFinished();
    void sigSavingFinished();
    void floatingMessageRequested(QString message, QString iconName);

private slots:

    void slotPreferences();
    void slotBlacklistCleanup();
    void slotImageSizeChanged(const QPointF &oldStillPoint, const QPointF &newStillPoint);
    void slotImageResolutionChanged();
    void slotNodeChanged();
    void slotCreateTemplate();
    void slotDocumentSaved();
    void slotSaveIncremental();
    void slotSaveIncrementalBackup();
    void showStatusBar(bool toggled);
    void showHideScrollbars();
    void toggleTabletLogger();
    void openResourcesDirectory();
    void updateIcons();
    void guiUpdateTimeout();

private:
    void createActions();
    void createManagers();
    void loadPlugins();

    void resetImageSizeAndScroll(bool changeCentering,
                                 const QPointF oldImageStillPoint = QPointF(),
                                 const QPointF newImageStillPoint = QPointF());
private:
    class KisView2Private;
    KisView2Private * const m_d;
};

#endif
