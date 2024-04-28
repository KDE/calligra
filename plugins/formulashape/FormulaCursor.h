/* This file is part of the KDE project
   SPDX-FileCopyrightText: 2001 Andrea Rizzi <rizzi@kde.org>
                      Ulrich Kuettler <ulrich.kuettler@mailbox.tu-dresden.de>
                 2006 Martin Pfeiffer <hubipete@gmx.net>
                 2009 Jeremias Epperlein <jeeree@web.de>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef FORMULACURSOR_H
#define FORMULACURSOR_H

#include "FormulaData.h"
#include "koformula_export.h"
#include <QPair>
#include <QString>

class BasicElement;
class QString;
class QPainter;
class QPointF;

enum CursorDirection { MoveRight, MoveLeft, MoveUp, MoveDown, NoDirection };

/**
 * @short The cursor being moved through a formula
 *
 * The FormulaCursor is used to store a cursor position (or selection) in a formula as well
 * as to manipulate it. It therefore has a selection state, a starting position
 * and (if it is selecting) a selection end position, called mark.
 *
 */

class KOFORMULA_EXPORT FormulaCursor
{
public:
    FormulaCursor(BasicElement *element, bool selecting, int position, int mark);
    FormulaCursor(BasicElement *element, int position);
    FormulaCursor();
    FormulaCursor(const FormulaCursor &other);
    FormulaCursor &operator=(const FormulaCursor &other);

    /**
     * Draw the cursor to the given QPainter
     * @param painter The QPainter the cursor draws itself to
     */
    void paint(QPainter &painter) const;

    /// @return whether the cursor is at the first position
    bool isHome() const;

    /// @return whether the cursor is at the last position
    bool isEnd() const;

    /// @return The element the FormulaCursor is currently inside
    BasicElement *currentElement() const;

    /// @return The current position in m_currentElement
    int position() const;

    /// set the position of the cursor in the current element
    void setPosition(int position);

    /// set the element, in which the cursor is
    void setCurrentElement(BasicElement *element);

    /// @return The current direction the cursor is moving in
    CursorDirection direction() const;

    /**
     * Make the cursor selecting
     * @param selecting When true the cursor is selecting
     */
    void setSelecting(bool selecting);

    /// @return @c true when the cursor is selecting
    bool isSelecting() const;

    /// @return @c true when the cursor is selecting
    bool hasSelection() const;

    /// set the start position of the selection
    void setMark(int position);

    /// @return the selection starting position
    int mark() const;

    /// select the element completely
    void selectElement(BasicElement *element);

    /// return the end and beginning of the current selection where the first element is the smaller one
    QPair<int, int> selection() const;

    /// @return checks if the cursor is valid were it is
    bool isAccepted() const;

    /// Move the cursor in the specified @p direction
    void move(CursorDirection direction);

    void moveTo(const FormulaCursor &pos);

    void moveTo(BasicElement *element, int position);

    void moveTo(BasicElement *element);

    /// Put the cursor in @p element, as close as possible to the point where @p cursor is
    bool moveCloseTo(BasicElement *element, FormulaCursor &cursor);

    /// Move the cursor to the first position in the current element
    void moveHome();

    /// Move the cursor to the last position in the current element
    void moveEnd();

    /// @return the midpoint of the current cursorLine in global coordinates
    QPointF getCursorPosition();

    /// Set the cursor to the element at @p point
    void setCursorTo(const QPointF &point);

    /// @return true if the cursor is inside a token element
    bool insideToken() const;

    /// @return true if the cursor is inside a row or inferred row
    bool insideInferredRow() const;

    /// @return true if the cursor is inside a element with fixed number of children
    bool insideFixedElement() const;

    bool performMovement(FormulaCursor &oldcursor);

    FormulaCursor &operator+=(int step);

    int offset();

private:
    /// The element that is currently left to the cursor
    BasicElement *m_currentElement;

    /// The position of the cursor in the current element
    int m_position;

    /// The position where the current selection starts in the current element
    int m_mark;

    /// Indicates whether the cursor is currently selecting
    bool m_selecting;

    CursorDirection m_direction;
};

#endif // FORMULACURSOR_H
