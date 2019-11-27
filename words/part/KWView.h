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
#include "./dockers/KWStatisticsWidget.h"
#include "./dockers/KWStatisticsDocker.h"

#include <KoView.h>
#include <KoViewConverter.h>
#include <KoZoomHandler.h>
#include <KoFindMatch.h>

#include <QWidget>

class KWDocument;
class KWCanvas;
class KWFrame;
class KWGui;

class KoPart;
class KoCanvasBase;
class KoZoomController;
class KoFindText;

class QPushButton;
#ifdef SHOULD_BUILD_RDF
class KoRdfBasicSemanticItem;
typedef QExplicitlySharedDataPointer<KoRdfBasicSemanticItem> hKoRdfBasicSemanticItem;
#endif

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
    static const qreal AnnotationAreaWidth;

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
    ~KWView() override;

    /**
     * return the KWDocument that owns this view.
     * @see KoView::document()
     */
    KWDocument *kwdocument() const {
        return m_document;
    }

    /// reimplemented from superclass
    void addImages(const QVector<QImage> &imageList, const QPoint &insertAt) override;

    // interface KoView
    /// reimplemented method from superclass
    void updateReadWrite(bool readWrite) override;
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

    KoZoomController *zoomController() const override { return m_zoomController; }

    int minPageNumber() const { return m_minPageNum; }
    int maxPageNumber() const { return m_maxPageNum; }

    void viewMouseMoveEvent(QMouseEvent *e);


Q_SIGNALS:
    void shownPagesChanged();

public Q_SLOTS:
    void offsetInDocumentMoved(int yOffset);

    /// displays the KWPageSettingsDialog that allows to change properties of the entire page
    void formatPage();

    /// turns the border display on/off
    void toggleViewFrameBorders(bool on);
    /// toggle the display of non-printing characters
    void setShowFormattingChars(bool on);
    /// toggle the display of field shadings
    void setShowInlineObjectVisualization(bool on);
    /// toggle the display of table borders
    void setShowTableBorders(bool on);
    /// toggle the display of section bounds
    void setShowSectionBounds(bool on);
    /// go to previous page
    void goToPreviousPage(Qt::KeyboardModifiers modifiers = Qt::NoModifier);
    /// go to next page
    void goToNextPage(Qt::KeyboardModifiers modifiers = Qt::NoModifier);
    /// Call when "Exit Fullscreen Mode" in status bar clicked.
    void exitFullscreenMode();

protected:
    /// reimplemented method from superclass
    void showEvent(QShowEvent *event) override;
    bool event(QEvent* event) override;

private:
    void setupActions();
    KoPrintJob *createPrintJob() override;
    /// loops over the selected shapes and returns the top level shapes.
    QList<KoShape *> selectedShapes() const;
    KoShape *selectedShape() const;

private Q_SLOTS:
    /// create a template from document
    void createTemplate();
    /// displays the KWFrameDialog that allows to alter the frameset properties
    void editFrameProperties();
    /// called if another shape got selected
    void selectionChanged();
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
    /// shows or hides the status bar
    void showStatusBar(bool);
    /// calls delete on the active tool
    void editDeleteSelection();
    /** decide if we enable or disable the action "delete_page" uppon m_document->page_count() */
    void updateStatusBarAction();
    /// show guides menu option uses this
    void setGuideVisibility(bool on);
    /// open the configure dialog.
    void configure();
#ifdef SHOULD_BUILD_RDF
    /// A semantic item was updated and should have it's text refreshed.
    void semanticObjectViewSiteUpdated(hKoRdfBasicSemanticItem item, const QString &xmlid);
#endif
    /// A match was found when searching.
    void findMatchFound(KoFindMatch match);
    /// This is used to update the text that can be searched.
    void refreshFindTexts();
    /// The KWPageSettingsDialog was closed.
    void pageSettingsDialogFinished();
    /// user wants to past data from the clipboard
    void pasteRequested();
    /// Call when the user want to show/hide the WordsCount in the statusbar
    void showWordCountInStatusBar(bool doShow);
    /// Show annotations ("notes" in the UI) on the canvas - this is the user view menu visibility change
    void showNotes(bool show);
    /// "hasAnnotations" has changed ("notes" in the UI) - will cause showNotes above to change too
    void hasNotes(bool has);
    /**
     * Set view into fullscreen mode, hide menu bar, status bar, tool bar, dockers
     * and set view into  full screen mode.
     */
    void setFullscreenMode(bool); /// Call after 4 seconds, user doesn't move cursor.
    void hideCursor();
    /// Hide status bar and scroll bars after seconds in fullscreen mode.
    void hideUI();

private:
    KWGui *m_gui;
    KWCanvas *m_canvas;
    KWDocument *m_document;
    KoZoomHandler m_zoomHandler;
    KoZoomController *m_zoomController;
    KWPage m_currentPage;
    KoFindText *m_find;

    QAction *m_actionCreateTemplate;
    QAction *m_actionFormatFrameSet;
    QAction *m_actionInsertFrameBreak;
    QAction *m_actionFormatFont;
    QAction *m_actionEditDelFrame;
    QAction *m_actionRaiseFrame;
    QAction *m_actionLowerFrame;
    QAction *m_actionBringToFront;
    QAction *m_actionSendBackward;
    KToggleAction *m_actionFormatBold;
    KToggleAction *m_actionFormatItalic;
    KToggleAction *m_actionFormatUnderline;
    KToggleAction *m_actionFormatStrikeOut;
    QAction *m_actionViewHeader;
    QAction *m_actionViewFooter;
    KToggleAction *m_actionViewSnapToGrid;

    bool m_snapToGrid;
    QString m_lastPageSettingsTab;

    QSizeF m_pageSize; // The max size of the pages we currently show. Prevents endless loop
    qreal m_textMinX; // The min x value where text can appear we currently show. Prevents endless loop
    qreal m_textMaxX; // The max x value where text can appear we currently show. Prevents endless loop
    int m_minPageNum;
    int m_maxPageNum;

    //Word count stuff for display in status bar
    void buildAssociatedWidget();
    KWStatisticsWidget *wordCount;

    bool m_isFullscreenMode;
    QTimer *m_hideCursorTimer;
    // The button will add to status bar in fullscreen mode to let user come
    // back to standard view.
    QPushButton *m_dfmExitButton;
};

#endif
