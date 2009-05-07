/* This file is part of the KDE project
   Copyright (C) 2006-2009 Thorsten Zachmann <zachmann@kde.org>

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License as published by the Free Software Foundation; either
   version 2 of the License, or (at your option) any later version.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this library; see the file COPYING.LIB.  If not, write to
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
   Boston, MA 02110-1301, USA.
*/

#ifndef KOPAVIEW_H
#define KOPAVIEW_H

#include <QObject>

#include <KoView.h>
#include <KoZoomHandler.h>
#include "KoPageApp.h"
#include "kopageapp_export.h"

class KoCanvasController;
class KoFind;
class KoPACanvas;
class Document;
class KToggleAction;
class Section;
class KoPADocumentStructureDocker;
class KoShapeManager;
class KoZoomAction;
class KoZoomController;
class QTextDocument;

/// Creates a view with a KoPACanvas and rulers
class KOPAGEAPP_EXPORT KoPAView : public KoView
{
    Q_OBJECT
public:
    enum KoPAAction
    {
        ActionInsertPage = 1,
        ActionCopyPage   = 2,
        ActionDeletePage = 4,
        AllActions       = 0xFF
    };

    /**
     * Constructor
     * @param document the document of this view
     * @param parent the parent widget
     */
    explicit KoPAView( Document * document, QWidget * parent = 0 );
    virtual ~KoPAView();

    void updateReadWrite( bool readwrite );

    virtual KoViewConverter * viewConverter( KoPACanvas * canvas ) { Q_UNUSED( canvas ); return &m_zoomHandler; }

    KoZoomHandler* zoomHandler() { return &m_zoomHandler; }

    KoZoomController *zoomController() { return m_zoomController; }

    KoPACanvas * kopaCanvas() { return m_canvas; }
    KoPACanvas * kopaCanvas() const { return m_canvas; }

    Document * document() { return m_doc; }
    /// @return Page that is shown in the canvas
    Section* activeSection() const;

    /// Set page shown in the canvas to @p page
    void setActiveSection( Section * page );

    void navigatePage( KoPageApp::PageNavigation pageNavigation );

    /// @return the shape manager used for this view
    KoShapeManager* shapeManager() const;

    /**
     * Set the active page and updates the UI
     */
    void doUpdateActiveSection( Section* sectio );

public slots:
    /// Insert a new page after the current one
    void insertPage();

signals:
    /// Emitted every time the active page is changed
    void activePageChanged();

protected:
    /// creates the widgets (called from the constructor)
    void initGUI();
    /// creates the actions (called from the constructor)
    void initActions();

//    /// Returns the document structure docker
//     KoPADocumentStructureDocker* documentStructureDocker() const;

    /// Update page navigation actions
    void updatePageNavigationActions();

protected slots:
    void viewSnapToGrid(bool snap);
    void viewGuides(bool show);
    void slotZoomChanged( KoZoomMode::Mode mode, qreal zoom );

    void editPaste();
    void editDeleteSelection();
    void editSelectAll();
    void editDeselectAll();

    /// Called when the mouse position changes on the canvas
    virtual void updateMousePosition(const QPoint& position);

    /// Called when the selection changed
    virtual void selectionChanged();

    /// Copy Page
    void copyPage();

    /// Delete the current page
    void deletePage();

    /// Called when the clipboard changed
    virtual void clipboardDataChanged();

    /// Go to the previous page
    void goToPreviousPage();
    /// Go to the next page
    void goToNextPage();
    /// Go to the first page
    void goToFirstPage();
    /// Go to the last page
    void goToLastPage();

    /**
     * Re-initialize the document structure docker after active document in this
     * view has been changed
     */
    void reinitDocumentDocker();

protected:
    Document *m_doc;
    KoPACanvas *m_canvas;
    Section *m_activeSection;

private:

    KoPADocumentStructureDocker * m_documentStructureDocker;

    KoCanvasController * m_canvasController;
    KoZoomController * m_zoomController;
    KoZoomHandler m_zoomHandler;

    KAction *m_editPaste;
    KAction *m_deleteSelectionAction;

    KToggleAction *m_actionViewSnapToGrid;
    KToggleAction *m_actionViewShowGuides;

    KAction * m_actionInsertPage;
    KAction * m_actionCopyPage;
    KAction * m_actionDeletePage;

    KToggleAction* m_viewRulers;

    KoZoomAction *m_zoomAction;

    KoFind * m_find;
};

#endif /* KOPAVIEW_H */
