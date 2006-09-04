/* This file is part of the KDE project
   Copyright (C)  2006 Peter Simonsson <peter.simonsson@gmail.com>

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

#ifndef KIVIOVIEW_H
#define KIVIOVIEW_H

#include <KoView.h>
#include <KoShapeControllerBase.h>
#include <KoZoomMode.h>

class KAction;

class KoShapeManager;
class KoCanvasController;
class KoZoomAction;
class KoZoomHandler;
class KoRuler;

class KivioCanvas;
class KivioDocument;
class KivioAbstractPage;

class KivioView : public KoView, public KoShapeControllerBase
{
  Q_OBJECT

  public:
    KivioView(KivioDocument* document, QWidget* parent);
    ~KivioView();

    /// Returns the canvas widget.
    KivioCanvas* canvasWidget() const;
    virtual QWidget* canvas() const;

    /// Returns the document
    KivioDocument* document() const;

    /// Returns the zoom handler
    KoZoomHandler* zoomHandler() const;

    virtual void updateReadWrite(bool readwrite);

    /// The page currently shown on the canvas
    KivioAbstractPage* activePage() const;

    /// The shape manager that handles drawing of the shapes on the canvas
    KoShapeManager* shapeManager() const;

    /// Adds @p shape to the document and updates all views
    virtual void addShape(KoShape* shape);
    /// Removes @p shape from the document and updates all views
    virtual void removeShape(KoShape* shape);

  public Q_SLOTS:
    /// Change the page that will be shown on the canvas
    void setActivePage(KivioAbstractPage* page);

    /// Set the new zoom and update the canvas
    void setZoom(int zoom);

    /// Called when the mouse position changes on the canvas
    void updateMousePosition(QPoint position);

  protected Q_SLOTS:
    /// Called by the zoom action to set the zoom
    void viewZoom(KoZoomMode::Mode mode, int zoom);
    /// Called by the zoom in action... zoom +25%
    void viewZoomIn();
    /// Called by the zoom out action... zoom -25%
    void viewZoomOut();
    /// Called when the zoom mode needs a recalculation of the zoom
    void recalculateZoom();

  protected:
    /// Creates and initializes the GUI.
    void initGUI();
    /// Initializes all the actions
    void initActions();

    /// Reimplemented to recalc the zoom when in fit to page or width mode
    virtual void resizeEvent(QResizeEvent* event);

  private:
    KivioDocument* m_document;
    KivioCanvas* m_canvas;
    KoCanvasController* m_canvasController;
    KoZoomHandler* m_zoomHandler;
    KoRuler* m_horizontalRuler;
    KoRuler* m_verticalRuler;

    KAction* m_viewZoomIn;
    KAction* m_viewZoomOut;
    KoZoomAction* m_viewZoomAction;

    KivioAbstractPage* m_activePage;
};

#endif
