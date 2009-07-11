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

#ifndef FORMULACURSOR_H
#define FORMULACURSOR_H

#include "kformula_export.h"
#include <QString>
#include <QPair>
#include "FormulaData.h"

class BasicElement;
class QString;
class QPainter;
class QPointF;
class QRectF;

enum CursorDirection {
    MoveRight,
    MoveLeft,
    MoveUp,
    MoveDown,
    NoDirection
};

/**
 * @short The cursor being moved through a formula
 *
 * The FormulaTool instanciates FormulaCursor to move around in the formula and edit
 * it. Each element can implement special cursor behaviour for its children. 
 * The cursor has a current element, a position in this element and (if it it is selecting)
 * a start position of this selection, called mark. Every element has a number
 * of possible cursor positions, it can influence the cursor movement 
 * by implementing moveCursor, acceptCursor and setCursorTo. 
 * A cursor also implements insertion and deletion of elements according to
 * its current element, position and selection.
 */
class KOFORMULA_EXPORT FormulaCursor {
public:
    /**
     * The constructor - set the FormulaCursor right to the beginning
     * @param element The element the FormulaCursor is set to at the beginning
     */
    explicit FormulaCursor( BasicElement* element, FormulaData* data );

    /**
     * Draw the cursor to the given QPainter
     * @param painter The QPainter the cursor draws itsself to
     */
    void paint( QPainter &painter ) const;

    /**
     * Insert text content at the current cursor position
     * @param text The text to insert
     */
    void insertText( const QString& text );

    /**
     * Insert an element at the current cursor position
     * @param element The element to be inserted
     */
    void insertElement( BasicElement* element );

    void insertData( const QString& data );

    /**
     * Remove an element from the formula
     * @param elementBeforePosition Indicates removal of element before or after cursor
     */
    void remove( bool elementBeforePosition );

    /**
     * Move the cursor in the specified @p direction
     * @param direction Indicates the direction to move to
     */
    void move( CursorDirection direction );

    /// Move the cursor to @p element
    void moveTo( BasicElement* element, int position );

    /// Put the cursor in @p element, as close as possible to the point where @p cursor is
    bool moveCloseTo( BasicElement* element, FormulaCursor* cursor);
    
    /// Move the cursor to the first position in the current element
    void moveHome();

    /// Move the cursor to the last position in the current element
    void moveEnd();

    /// Set the cursor to the element at @p point
    void setCursorTo( const QPointF& point );
    
    /// @return the midpoint of the current cursorLine in global coordinates
    QPointF getCursorPosition();
    
    /// @return whether the cursor is at the first position
    bool isHome() const;

    /// @return whether the cursor is at the last position
    bool isEnd() const;

    /// @return The element the FormulaCursor is currently inside
    BasicElement* currentElement() const;

    /// @return The current position in m_currentElement
    int position() const;
    
    /// @return The FormulaData which is navigated by this cursor
    FormulaData* formulaData() const;
    
    /// set the position of the cursor in the current element
    void setPosition(int position);

    /// set the element, in which the cursor is 
    void setCurrentElement(BasicElement* element);

    /// set the FormulaData which is navigated by this cursor
    void setData(FormulaData* data);

    /// @return The current direction the cursor is moving in
    CursorDirection direction() const;

    /// @return the buffer with the last user input
    QString inputBuffer() const;

    /**
     * Make the cursor selecting
     * @param selecting When true the cursor is selecting
     */ 
    void setSelecting( bool selecting );
    
    /// set the start position of the selection
    void setSelectionStart(int position);
    
    /// @return @c true when the cursor is selecting
    bool isSelecting() const;
    
    /// @return @c true when the cursor is selecting and the selection is not empty
    bool hasSelection() const;
    
    /// @return the selection starting position
    int mark() const;
    
    /// select the element completely
    void selectElement(BasicElement* element);
    
    /// return the end and beginning of the current selection where the first element is the smaller one
    QPair<int,int> selection() const;
   
    /// @return checks if the cursor is valid were it is
    bool isAccepted() const;
    
private:
    /// @return true if the cursor is inside a token element
    bool insideToken() const;

    /// @return true if the cursor is inside a row or inferred row
    bool insideInferredRow() const;
    
    /// @return true if the cursor is inside a element with fixed number of children
    bool insideFixedElement() const;
    
    /// @return true if the cursor is inside an empty element
    bool insideEmptyElement() const;
    
    /// @return if the element next to the cursor is an empty element it is returned, otherwise 0
    BasicElement* nextToEmpty() const;
    
    
    /// @return if the element is next to an empty element, it is placed there
    bool moveToEmpty();
    
    QString tokenType(const QChar& character) const;

    bool performMovement(CursorDirection direction, FormulaCursor* oldcursor);
    
private:
    /// The element that is currently left to the cursor
    BasicElement* m_currentElement;

    /// The formulaData 
    FormulaData* m_data;
    
    /// The position of the cursor in the current element
    int m_position;
    
    /// The position where the current selection starts in the current element
    int m_mark;
    
    /// The direction the cursor is moving to
    CursorDirection m_direction;

    /// Buffer for the user input
    QString m_inputBuffer;

    /// Indicates whether the cursor is currently selecting
    bool m_selecting;
};

#endif // FORMULACURSOR_H
