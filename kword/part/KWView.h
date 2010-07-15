/* This file is part of the KDE project
 * Copyright (C) 2005-2007, 2009, 2010 Thomas Zander <zander@kde.org>
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
 * Boston, MA 02110-1301, USA
 */

#ifndef KWVIEW_H
#define KWVIEW_H

#include "kword_export.h"
#include "KWPage.h"

#include <KoView.h>
#include <KoViewConverter.h>
#include <KoZoomHandler.h>
#include <KoShapeReorderCommand.h>

#include <QWidget>

class KWDocument;
class KWCanvas;
class KWFrame;
class KWGui;

class KoZoomController;
class KToggleAction;
class KoRdfSemanticItem;

/**
 * KWords view class. Following the broad model-view-controller idea this class
 * shows you one view on the document. There can be multiple views of the same document each
 * in with independent settings for viewMode and zoom etc.
 */
class KWORD_EXPORT KWView : public KoView
{
    Q_OBJECT

public:
    /**
     * Construct a new view on the kword document.
     * The view will have a canvas as a member which does all the actual painting, the view will
     * be responsible for handling the actions.  The View is technically speaking the controller
     * class in the MVC design.
     * @param viewMode the KWViewMode we should show initially.
     * @param document the document we show.
     * @param parent a parent widget we show ourselves in.
     */
    KWView(const QString &viewMode, KWDocument *document, QWidget *parent);
    virtual ~KWView();

    /**
     * return the KWDocument that owns this view.
     * @see KoView::document()
     */
    KWDocument *kwdocument() const {
        return m_document;
    }

    // interface KoView
    /// reimplemented method from superclass
    virtual void updateReadWrite(bool readWrite);
    /// reimplemented method from superclass
    virtual QWidget *canvas() const;

    /// returns true if this view has the snap-to-grid enabled.
    bool snapToGrid() const {
        return m_snapToGrid;
    }

    /**
     * Return the current canvas; much like canvas(), but this one does not downcast.
     */
    KWCanvas *kwcanvas() const;

    /// Return the view converter for this view.
    KoViewConverter *viewConverter() {
        return &m_zoomHandler;
    }

    /// show a popup on the view, adding to it a list of actions
    void popupContextMenu(const QPoint &globalPosition, const QList<QAction*> &actions);

    const KWPage currentPage() const;
    void setCurrentPage(const KWPage &page);

    /// go to page
    void goToPage(const KWPage &page);

    virtual KoZoomController *zoomController() const { return m_zoomController; }

public slots:
    void offsetInDocumentMoved(int yOffset);

protected:
    /// reimplemented method from superclass
    virtual void showEvent(QShowEvent *event);

private:
    void setupActions();
    virtual KoPrintJob *createPrintJob();

private slots:
    /// displays the KWFrameDialog that allows to alter the frameset properties
    void editFrameProperties();
    /// called if another shape got selected
    void selectionChanged();
    /// force the remainder of the text into the next page
    void insertFrameBreak();
    /// insert a bookmark on current text cursor location or selection
    void addBookmark();
    /// go to previously bookmarked text cursor location or selection
    void selectBookmark();
    /// delete previously bookmarked text cursor location or selection (from the Select Bookmark dialog)
    void deleteBookmark(const QString &name);
    /// delete the currently selected frame(s)
    void editDeleteFrame();
    /// enable/disable document headers
    void toggleHeader();
    /// enable/disable document footers
    void toggleFooter();
    /// snap to grid
    void toggleSnapToGrid();
    /** Move the selected frame above maximum 1 frame that is in front of it. */
    void raiseFrame() {
        adjustZOrderOfSelectedFrames(KoShapeReorderCommand::RaiseShape);
    }
    /** Move the selected frame behind maximum 1 frame that is behind it */
    void lowerFrame() {
        adjustZOrderOfSelectedFrames(KoShapeReorderCommand::LowerShape);
    }
    /** Move the selected frame(s) to be in the front most position. */
    void bringToFront() {
        adjustZOrderOfSelectedFrames(KoShapeReorderCommand::BringToFront);
    }
    /** Move the selected frame(s) to be behind all other frames */
    void sendToBack() {
        adjustZOrderOfSelectedFrames(KoShapeReorderCommand::SendToBack);
    }
    /// turns the border display on/off
    void toggleViewFrameBorders(bool on);
    /// displays the KWPageSettingsDialog that allows to change properties of the entire page
    void formatPage();
    /// displays libs/main/rdf/SemanticStylesheetsEditor to edit Rdf stylesheets
    void editSemanticStylesheets();
    /// convert current frame to an inline frame
    void inlineFrame();
    /// called if the zoom changed
    void zoomChanged(KoZoomMode::Mode mode, qreal zoom);
    /// displays the KWStatisticsDialog
    void showStatisticsDialog();
    /// shows or hides the rulers
    void showRulers(bool visible);
    /// creates a copy of the current frame
    void createLinkedFrame();
    /// shows or hides the status bar
    void showStatusBar(bool);
    /// delete the current page
    void deletePage();
    /// insert a new page
    void insertPage();
    /// toggle the display of non-printing characters
    void setShowFormattingChars(bool on);
    /// selects all frames
    void editSelectAllFrames();
    /// calls delete on the active tool
    void editDeleteSelection();
    /// create a KWOutlineShape for the selected frame(s).
    void createCustomOutline();
    /// Wrap the selected frames into a clipping shape container.
    void createFrameClipping();
    /// unwrap the selected frames into a clipping shape container.
    void removeFrameClipping();
    /** decide if we enable or disable the action "delete_page" uppon m_document->page_count() */
    void handleDeletePageAction();
    /// set the status of the show-statusbar action to reflect the current setting.
    void updateStatusBarAction();
    /// insert image
    void insertImage();
    /// show guides menu option uses this
    void setGuideVisibility(bool on);
    /// go to previous page
    void goToPreviousPage();
    /// go to next page
    void goToNextPage();
    /// A semantic item was updated and should have it's text refreshed.
    void semanticObjectViewSiteUpdated(KoRdfSemanticItem *item, const QString &xmlid);
    void createTextOnShape();

private:
    /// helper method for the raiseFrame/lowerFrame/bringToFront/sendToBack methods
    void adjustZOrderOfSelectedFrames(KoShapeReorderCommand::MoveShapeType direction);

    /// loops over the selected shapes and returns the frames that go with them.
    QList<KWFrame*> selectedFrames() const;

private:
    KWGui *m_gui;
    KWCanvas *m_canvas;
    KWDocument *m_document;
    KoZoomHandler m_zoomHandler;
    KoZoomController *m_zoomController;
    KWPage m_currentPage;

    KAction *m_actionFormatFrameSet;
    KAction *m_actionInsertFrameBreak;
    KAction *m_actionAddBookmark;
    KAction *m_actionFormatFont;
    KAction *m_actionEditDelFrame;
    KAction *m_actionRaiseFrame;
    KAction *m_actionLowerFrame;
    KAction *m_actionBringToFront;
    KAction *m_actionSendBackward;
    KToggleAction *m_actionFormatBold;
    KToggleAction *m_actionFormatItalic;
    KToggleAction *m_actionFormatUnderline;
    KToggleAction *m_actionFormatStrikeOut;
    KToggleAction *m_actionViewHeader;
    KToggleAction *m_actionViewFooter;
    KToggleAction *m_actionViewSnapToGrid;

    bool m_snapToGrid;
};

#endif
