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

#ifndef __KFORMULAWIDGET_H
#define __KFORMULAWIDGET_H

#include <qdom.h>
#include <qpoint.h>
#include <qwidget.h>

//#include <kaccel.h>
//#include <kaction.h>

#include "formuladefs.h"


class BasicElement;
class FormulaCursor;
class FormulaElement;
class KFormulaContainer;


/**
 * The view. A widget that shows the formula. There are methods
 * to move the cursor around. To edit the formula use the document.
 */
class KFormulaWidget : public QWidget {
    friend class KFormulaContainer;
    Q_OBJECT

public:
    KFormulaWidget(KFormulaContainer*, QWidget* parent=0, const char* name=0, WFlags f=0);
    ~KFormulaWidget();


    /**
     * @returns the point inside the formula widget where the cursor is.
     */
    QPoint getCursorPoint() const;

    /**
     * Puts the widget in read only mode.
     */
    void setReadOnly(bool ro) { readOnly = ro; }

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

protected slots:

    /**
     * The cursor has been moved by the container.
     * We need to repaint if it was ours.
     */
    void slotCursorMoved(FormulaCursor* cursor);

    /**
     * The formula has changed and needs to be redrawn.
     */
    void slotFormulaChanged(int width, int height);

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

    virtual void mousePressEvent(QMouseEvent* event);
    virtual void mouseReleaseEvent(QMouseEvent* event);
    virtual void mouseDoubleClickEvent(QMouseEvent* event);
    virtual void mouseMoveEvent(QMouseEvent* event);
    virtual void wheelEvent(QWheelEvent* event);

    virtual void paintEvent(QPaintEvent* event);
    virtual void keyPressEvent(QKeyEvent* event);
    virtual void focusInEvent(QFocusEvent* event);
    virtual void focusOutEvent(QFocusEvent* event);

    /**
     * Convert Qt style key state modifiers to local
     * movement flags.
     */
    MoveFlag movementFlag(int state);

    void hideCursor();
    void showCursor();

    /**
     * The document we show.
     */
    KFormulaContainer* getDocument() { return document; }

    /**
     * Our cursor.
     */
    FormulaCursor* getCursor() { return cursor; }
    
private:

    /**
     * Tell everybody that our cursor has changed if so.
     */
    void emitCursorChanged();

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
     * Whether we are only allowed to read.
     */
    bool readOnly;

    /**
     * The formula we show.
     */
    KFormulaContainer* document;

    /**
     * Out cursor.
     */
    FormulaCursor* cursor;
};

#endif // __KFORMULAWIDGET_H
