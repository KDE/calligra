/* This file is part of the KDE project
   Copyright (C) 2001 Andrea Rizzi <rizzi@kde.org>
                      Ulrich Kuettler <ulrich.kuettler@mailbox.tu-dresden.de>
                 2006 Martin Pfeiffer <hubipete@gmx.net>

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

#ifndef KFORMULACANVAS_H
#define KFORMULACANVAS_H

#include <QPixmap>
#include <QWidget>
#include <KoCanvasBase.h>

class KFormulaPartView;
class KFormulaPartDocument;
class KoShapeManager;
class KoToolProxy;

/**
 * @short The canvas widget displaying the FormulaShape
 *
 * The KFormulaCanvas is a class derived from QWidget to display a @ref FormulaShape
 * and to direct all user inputs to the shape. KFormulaCanvas renders the formula
 * in its paintEvent method.
 */
class KFormulaCanvas : public KoCanvasBase, public QWidget {
public:
    /// The constructor taking arguments for QWidget
    KFormulaCanvas( KFormulaPartView* view, KFormulaPartDocument* document, QWidget* parent = 0 );

    /// The destructor
    ~KFormulaCanvas();

    /// reimplemented method from superclass
    void gridSize( qreal* horizontal, qreal* vertical ) const;

    /// reimplemented method from superclass
    bool snapToGrid() const;

    /// reimplemented method from superclass
    void addCommand( QUndoCommand* command );

    /// reimplemented method from superclass
    KoShapeManager* shapeManager() const;

    /// reimplemented method from superclass
    KoToolProxy* toolProxy() const;

    /// reimplemented method from superclass
    void updateCanvas( const QRectF& rc );

    /// reimplemented method from superclass
    const KoViewConverter* viewConverter() const;

    /// reimplemented method from superclass
    QWidget* canvasWidget();
    /// reimplemented method from superclass
    virtual const QWidget* canvasWidget() const;

    /// reimplemented method from superclass
    KoUnit unit() const;

    /// reimplemented method from superclass
    virtual void updateInputMethodInfo();

    void setCursor(const QCursor &cursor);

protected:
    void mousePressEvent( QMouseEvent* event );
    void mouseReleaseEvent( QMouseEvent* event );
    void mouseDoubleClickEvent( QMouseEvent* event );
    void mouseMoveEvent( QMouseEvent* event );
    void paintEvent( QPaintEvent* event );
    void keyPressEvent( QKeyEvent* event );

private:
    /// The buffer for painting - always updated when the formula changes
    QPixmap m_paintBuffer;

    /// True when a formula repaint is needed
    bool m_dirtyBuffer;

    /// The KFormulaPartView we belong to
    KFormulaPartView* m_view;

    /// The proxy used to forward events
    KoToolProxy* m_toolProxy;

    KoShapeManager* m_shapeManager;
};

#endif // KFORMULACANVAS_H
