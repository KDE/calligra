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

#ifndef KIVIOCANVAS_H
#define KIVIOCANVAS_H

#include "KivioDocument.h"
#include "KivioView.h"

#include <QWidget>

#include <KoCanvasBase.h>
#include <KoUnit.h>

class QPaintEvent;

class KoZoomHandler;
class KoTool;
class KoShapeManager;

class KivioView;

class KivioCanvas : public QWidget, public KoCanvasBase
{
  Q_OBJECT

  public:
    KivioCanvas(KivioView* parent);
    ~KivioCanvas();

    /**
     * retrieve the grid size setting.
     * The grid spacing will be provided in pt.
     * @param horizontal a pointer to a double that will be filled with the horizontal grid-spacing
     * @param vertical a pointer to a double that will be filled with the vertical grid-spacing
     */
    virtual void gridSize(double *horizontal, double *vertical) const;

    /**
     * return if snap to grid is enabled.
     * @return if snap to grid is enabled.
     */
    virtual bool snapToGrid() const;

    /**
     * Adds a command to the history. Call this for each @p command you create.
     * Unless you set @p execute to false, this will also execute the command.
     * This means, most of the application's code will look like
     *    MyCommand * cmd = new MyCommand( parameters );
     *    canvas.addCommand( cmd );
     *
     * Note that the command history takes ownership of the command, it will delete
     * it when the undo limit is reached, or when deleting the command history itself.
     * @param command the command to add
     * @param execute if true, the commands execute method will be called
     */
    virtual void addCommand(KCommand *command, bool execute = true);

    /**
     * return the current shapeManager
     * @return the current shapeManager
     */
    virtual KoShapeManager *shapeManager() const;

    /**
     * Tell the canvas repaint the specified rectangle. The coordinates
     * are document coordinates, not view coordinates.
     */
    virtual void updateCanvas(const QRectF& rc);

    /**
     * Return the curently active tool, or 0 if non active.
     * @return the curently active tool, or 0 if non active.
     */
    KoTool* tool() { return m_tool; }

    /**
     * Set the new ative tool.
     * @param tool the new tool to be used on the canvas.
     */
    void setTool(KoTool* tool) { m_tool = tool; }

    /**
     * Return the viewConverter for this view.
     * @return the viewConverter for this view.
     */
    virtual KoViewConverter *viewConverter();

    /**
     * Return the widget that will be added to the scrollArea.
     */
    virtual QWidget* canvasWidget();

    KoUnit::Unit unit() { return m_view->document()->unit(); }

  public slots:
    /// Recalculates the size of the canvas (needed when zooming or changing pagelayout)
    void updateSize();

  protected:
    virtual void paintEvent(QPaintEvent* event);

    virtual void mouseMoveEvent(QMouseEvent* event);
    virtual void mousePressEvent(QMouseEvent* event);
    virtual void mouseReleaseEvent(QMouseEvent* event);

    virtual void keyPressEvent(QKeyEvent* event);
    virtual void keyReleaseEvent(QKeyEvent* event);

  private:
    KivioView* m_view;

    KoTool *m_tool;

    KoZoomHandler* m_zoomHandler;

    KoShapeManager* m_shapeManager;
};

#endif
