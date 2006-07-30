/* This file is part of the KDE project
 * Copyright (C) 2005-2006 Thomas Zander <zander@kde.org>
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

#include <QWidget>

class KWDocument;
class KWCanvas;
class KWGui;
class KWPage;

class KoShape;
class KoZoomAction;

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

    // interface KoView
    /// overwritten method from superclass
    void updateReadWrite(bool readWrite);
    /// overwritten method from superclass
    QWidget *canvas() const;

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

private:
    void setupActions();
    /// Update the menu to have the choices the current viewmode provide.
    void changeZoomMenu();
    void updateZoomControls();
    KWPage *currentPage();

private slots:
    void viewZoom( KoZoomMode::Mode mode, int zoom );
    void updateZoom();
    void editFrameProperties();

private:
    KWGui *m_gui;
    KWCanvas *m_canvas;
    KWDocument *m_document;
    KoZoomHandler m_zoomHandler;
    KWPage *m_currentPage;

    KoZoomAction *m_actionViewZoom;
    KAction *m_actionFormatFrameSet;
};

#endif
