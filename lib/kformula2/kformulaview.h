/* This file is part of the KDE project
   Copyright (C) 2001 Andrea Rizzi <rizzi@kde.org>
	              Ulrich Kuettler <ulrich.kuettler@mailbox.tu-dresden.de>

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
   the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
   Boston, MA 02111-1307, USA.
*/

#ifndef __KFORMULAVIEW_H
#define __KFORMULAVIEW_H

#include <qevent.h>
#include <qobject.h>
#include <qrect.h>

#include "formuladefs.h"

class BasicElement;
class FormulaCursor;
class FormulaElement;
class KFormulaContainer;
class QColorGroup;


/**
 * The view that shows the formula. Its main purpose is to handle
 * the cursor. There are methods
 * to move the cursor around. To edit the formula use the document.
 *
 * The view is meant to be easy embeddable into a widget or
 * to be used alone if there is a bigger widget the formula
 * is to be drawn into.
 */
class KFormulaView : public QObject {
    Q_OBJECT

public:
    KFormulaView(KFormulaContainer*);
    ~KFormulaView();

    /**
     * @returns the point inside the formula view where the cursor is.
     */
    QPoint getCursorPoint() const;

    /**
     * @returns the area the cursor is currently on.
     */
    QRect getDirtyArea() const { return dirtyArea; }
    
    /**
     * Puts the widget in read only mode.
     */
    void setReadOnly(bool ro);

    void mousePressEvent(QMouseEvent* event);
    void mouseReleaseEvent(QMouseEvent* event);
    void mouseDoubleClickEvent(QMouseEvent* event);
    void mouseMoveEvent(QMouseEvent* event);
    void wheelEvent(QWheelEvent* event);

    //void paintEvent(QPaintEvent* event);
    void keyPressEvent(QKeyEvent* event);
    void focusInEvent(QFocusEvent* event);
    void focusOutEvent(QFocusEvent* event);

    void draw(QPainter& painter, const QRect& rect, const QColorGroup& cg);

    /**
     * The document we show.
     */
    KFormulaContainer* getDocument() { return document; }

signals:

    /**
     * Is emitted everytime the cursor might have changed.
     */
    void cursorChanged(bool visible, bool selecting);
    
public slots:
    
    void slotSelectAll();

    void slotMoveLeft(MoveFlag flag);
    void slotMoveRight(MoveFlag flag);
    void slotMoveUp(MoveFlag flag);
    void slotMoveDown(MoveFlag flag);
    void slotMoveHome(MoveFlag flag);
    void slotMoveEnd(MoveFlag flag);

    /**
     * Our cursor.
     */
    FormulaCursor* getCursor() { return cursor; }

    void setSmallCursor(bool small);
    
protected slots:

    /**
     * The cursor has been moved by the container.
     * We need to repaint if it was ours.
     */
    void slotCursorMoved(FormulaCursor* cursor);

    /**
     * A new formula has been loaded.
     */
    void slotFormulaLoaded(FormulaElement*);

    /**
     * There is an element that will disappear from the tree.
     * our cursor must not be inside it.
     */
    void slotElementWillVanish(BasicElement*);

protected:

    /**
     * Convert Qt style key state modifiers to local
     * movement flags.
     */
    MoveFlag movementFlag(int state);
    
private:

    /**
     * Tell everybody that our cursor has changed if so.
     */
    void emitCursorChanged();

    /**
     * If set the cursor will never be bigger that the formula.
     */
    bool smallCursor;
    
    /**
     * Whether you can see the cursor. This has to be kept
     * in sync with reality.
     */
    bool cursorVisible;

    /**
     * Whether the cursor changed since the last time
     * we emitted a cursorChanged signal.
     */
    bool cursorHasChanged;

    /**
     * The area that needs an update because the cursor moved.
     */
    QRect dirtyArea;
    
    /**
     * The formula we show.
     */
    KFormulaContainer* document;

    /**
     * Out cursor.
     */
    FormulaCursor* cursor;
};

#endif // __KFORMULAVIEW_H
