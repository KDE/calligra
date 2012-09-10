/* This file is part of the KDE project
 * Copyright (C) 2005-2007, 2009, 2010 Thomas Zander <zander@kde.org>
 * Copyright (C) 2010 Boudewijn Rempt <boud@kogmbh.com>
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

#include "words_export.h"
#include "KWPage.h"

#include <KoView.h>
#include <KoViewConverter.h>
#include <KoZoomHandler.h>
#include <KoFindMatch.h>
#include <KoTextAnchor.h>

#include <QWidget>

class KWDocument;
class KWCanvas;
class KWFrame;
class KWGui;

class KoPart;
class KoCanvasBase;
class KoZoomController;
class KoFindText;
class KoFindStyle;
class KoRdfSemanticItem;
class KoTextAnchor;
typedef QExplicitlySharedDataPointer<KoRdfSemanticItem> hKoRdfSemanticItem;

class KToggleAction;
/**
 * Words' view class. Following the broad model-view-controller idea this class
 * shows you one view on the document. There can be multiple views of the same document each
 * in with independent settings for viewMode and zoom etc.
 */
class WORDS_EXPORT KWView : public KoView
{
    Q_OBJECT

public:
    /**
     * Construct a new view on the words document.
     * The view will have a canvas as a member which does all the actual painting, the view will
     * be responsible for handling the actions.  The View is technically speaking the controller
     * class in the MVC design.
     * @param part a KoPart
     * @param document the document we show.
     * @param parent a parent widget we show ourselves in.
     */
    KWView(KoPart *part, KWDocument *document, QWidget *parent);
    virtual ~KWView();

    /**
     * return the KWDocument that owns this view.
     * @see KoView::document()
     */
    KWDocument *kwdocument() const {
        return m_document;
    }

    /// reimplemented from superclass
    void addImages(const QList<QImage> &imageList, const QPoint &insertAt);

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
    KoCanvasBase *canvasBase() const;

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

    /// displays the KWPageSettingsDialog that allows to change properties of the entire page
    void formatPage();

    /// turns the border display on/off
    void toggleViewFrameBorders(bool on);
    /// toggle the display of non-printing characters
    void setShowFormattingChars(bool on);
    /// toggle the display of table borders
    void setShowTableBorders(bool on);
    /// go to previous page
    void goToPreviousPage(Qt::KeyboardModifiers modifiers = Qt::NoModifier);
    /// go to next page
    void goToNextPage(Qt::KeyboardModifiers modifiers = Qt::NoModifier);

protected:
    /// reimplemented method from superclass
    virtual void showEvent(QShowEvent *event);

private:
    void setupActions();
    virtual KoPrintJob *createPrintJob();
    /// loops over the selected shapes and returns the frames that go with them.
    QList<KWFrame*> selectedFrames() const;
    KoShape *selectedShape() const;

private slots:
    /// create a template from document
    void createTemplate();
    /// displays the KWFrameDialog that allows to alter the frameset properties
    void editFrameProperties();
    /// called if another shape got selected
    void selectionChanged();
    /// insert a bookmark on current text cursor location or selection
    void addBookmark();
    /// go to previously bookmarked text cursor location or selection
    void selectBookmark();
    /// delete previously bookmarked text cursor location or selection (from the Select Bookmark dialog)
    void deleteBookmark(const QString &name);
    /// enable document headers
    void enableHeader();
    /// enable document footers
    void enableFooter();
    /// snap to grid
    void toggleSnapToGrid();
    /// displays libs/main/rdf/SemanticStylesheetsEditor to edit Rdf stylesheets
    void editSemanticStylesheets();
    /// called if the zoom changed
    void zoomChanged(KoZoomMode::Mode mode, qreal zoom);
    /// shows or hides the rulers
    void showRulers(bool visible);
    /// creates a copy of the current frame
    void createLinkedFrame();
    /// shows or hides the status bar
    void showStatusBar(bool);
    /// selects all frames
    void editSelectAllFrames();
    /// calls delete on the active tool
    void editDeleteSelection();
    /** decide if we enable or disable the action "delete_page" uppon m_document->page_count() */
    void updateStatusBarAction();
    /// show guides menu option uses this
    void setGuideVisibility(bool on);
    /// open the configure dialog.
    void configure();
    /// A semantic item was updated and should have it's text refreshed.
    void semanticObjectViewSiteUpdated(hKoRdfSemanticItem item, const QString &xmlid);
    /// A match was found when searching.
    void findMatchFound(KoFindMatch match);
    /// The document has finished loading. This is used to update the text that can be searched.
    void loadingCompleted();
    /// The KWPageSettingsDialog was closed.
    void pageSettingsDialogFinished();
    /// user wants to past data from the clipboard
    void pasteRequested();

private:
    KWGui *m_gui;
    KWCanvas *m_canvas;
    KWDocument *m_document;
    KoZoomHandler m_zoomHandler;
    KoZoomController *m_zoomController;
    KWPage m_currentPage;
    KoFindText *m_find;

    KAction *m_actionCreateTemplate;
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
    KAction *m_actionViewHeader;
    KAction *m_actionViewFooter;
    KToggleAction *m_actionViewSnapToGrid;

    bool m_snapToGrid;
    QString m_lastPageSettingsTab;

    QSizeF m_maxPageSize; // The maximum size of the pages we have encountered. This is used to
                         // make sure that we always show all pages correctly in page/pagewidth mode.
};

#endif
