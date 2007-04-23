/* This file is part of the KDE project
 * Copyright (C) 2005-2007 Thomas Zander <zander@kde.org>
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

#include <KoView.h>
#include <KoViewConverter.h>
#include <KoZoomHandler.h>
#include <KoZoomMode.h>
#include <KoShapeReorderCommand.h>

#include <QWidget>
#include "kword_export.h"

class KWDocument;
class KWCanvas;
class KWGui;
class KWPage;

class KoShape;
class KoZoomAction;
class KToggleAction;

class QResizeEvent;

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
    KWView( const QString& viewMode, KWDocument *document, QWidget *parent );
    ~KWView();

    /**
     * return the KWDocument that owns this view.
     * @see KoView::document()
     */
    KWDocument *kwdocument() const { return m_document; }

    // interface KoView
    /// overwritten method from superclass
    void updateReadWrite(bool readWrite);
    /// overwritten method from superclass
    QWidget *canvas() const;

    /// returns true if this view has the snap-to-grid enabled.
    bool snapToGrid() const { return m_snapToGrid; }

    /**
     * Return the current canvas; much like canvas(), but this one does not downcast.
     */
    KWCanvas *kwcanvas() const;

    /// Return the zoom handler for this view.
    KoViewConverter *viewConverter() { return &m_zoomHandler; }
    void setZoom( int zoom ); ///< change the zoom value

protected:
    /// overwritten method from superclass
    void resizeEvent( QResizeEvent *e );

    /// overwritten method from superclass
    void showEvent(QShowEvent *event);

private:
    void setupActions();
    /// Update the menu to have the choices the current viewmode provide.
    void changeZoomMenu();
    void updateZoomControls();

private slots:
    void viewZoom( KoZoomMode::Mode mode, double zoom );
    void updateZoom();
    void editFrameProperties();

    void selectionChanged();

    // actions
    void print();
    void insertFrameBreak();
    void editDeleteFrame();
    void toggleHeader();
    void toggleFooter();
    void toggleSnapToGrid();
    /** Move the selected frame above maximum 1 frame that is in front of it. */
    void raiseFrame() { adjustZOrderOfSelectedFrames(KoShapeReorderCommand::RaiseShape); }
    /** Move the selected frame behind maximum 1 frame that is behind it */
    void lowerFrame() { adjustZOrderOfSelectedFrames(KoShapeReorderCommand::LowerShape); }
    /** Move the selected frame(s) to be in the front most position. */
    void bringToFront() { adjustZOrderOfSelectedFrames(KoShapeReorderCommand::BringToFront); }
    /** Move the selected frame(s) to be behind all other frames */
    void sendToBack() { adjustZOrderOfSelectedFrames(KoShapeReorderCommand::SendToBack); }
    void toggleViewFrameBorders(bool on);
    void formatPage();
    void inlineFrame();

private:
    /// helper method for the raiseFrame/lowerFrame/bringToFront/sendToBack methods
    void adjustZOrderOfSelectedFrames(KoShapeReorderCommand::MoveShapeType direction);

private:
    KWGui *m_gui;
    KWCanvas *m_canvas;
    KWDocument *m_document;
    KoZoomHandler m_zoomHandler;
    KWPage *m_currentPage;

    KoZoomAction *m_actionViewZoom;
    KAction *m_actionFormatFrameSet;
    KAction *m_actionInsertFrameBreak;
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
