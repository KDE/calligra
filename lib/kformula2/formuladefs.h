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

#ifndef FORMULADEFS_H
#define FORMULADEFS_H


/**
 * The symbols that are supported by our artwork.
 */
enum SymbolType {
    LeftSquareBracket = '[',
    RightSquareBracket = ']',
    LeftCurlyBracket = '{',
    RightCurlyBracket = '}',
    LineBracket = '|',
    LeftCornerBracket = '<',
    RightCornerBracket = '>',
    LeftRoundBracket = '(',
    RightRoundBracket = ')',

    // symbols that have no ascii character
    Empty = 1000,
    Integral,
    Sum,
    Product,
    Arrow
};


/**
 * Flag for cursor movement functions.
 * Select means move selecting the text (usually Shift key)
 * Word means move by whole words  (usually Control key)
 */
enum MoveFlag { NormalMovement = 0, SelectMovement = 1, WordMovement = 2 };


/**
 * The token types the sequence parser knows.
 */
enum TokenType {
    TEXT, SYMBOL, NUMBER, ELEMENT, ERROR, END,
    PLUS='+', MINUS='-', MUL='*', DIV='/',
    ASSIGN='=', LESS='<', GREATER='>', SEPARATOR='\\',
    COMMA=',', COLON=':', SEMICOLON=';'
};


#endif // FORMULADEFS_H
