/* This file is part of the Calligra project
 * Copyright (C) 2006 Sebastian Sauer <mail@dipe.org>
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
 * Boston, MA 02110-1301, USA.
 */

#ifndef SCRIPTING_TEXTCURSOR_H
#define SCRIPTING_TEXTCURSOR_H

#include <QObject>
#include <QTextCursor>

namespace Scripting
{

/**
* A cursor to provides a control structure for the successive traversal
* of content within a \a TextDocument .
*/
class TextCursor : public QObject
{
    Q_OBJECT
    Q_ENUMS(MoveOperation)
public:
    TextCursor(QObject* parent, const QTextCursor& cursor);
    virtual ~TextCursor();
    QTextCursor& cursor() {
        return m_cursor;
    }

    enum MoveOperation {
        Start = QTextCursor::Start, //!< Move to the start of the document.
        End = QTextCursor::End, //!< Move to the end of the document.

        StartOfLine = QTextCursor::StartOfLine, //!< Move to the start of the current line.
        EndOfLine = QTextCursor::EndOfLine, //!< Move to the end of the current line.

        StartOfBlock = QTextCursor::StartOfBlock, //!< Move to the start of the current block.
        EndOfBlock = QTextCursor::EndOfBlock, //!< Move to the end of the current block.
        PreviousBlock = QTextCursor::PreviousBlock, //!< Move to the start of the previous block.
        NextBlock = QTextCursor::NextBlock, //!< Move to the beginning of the next block.

        StartOfWord = QTextCursor::StartOfWord, //!< Move to the start of the current word.
        EndOfWord = QTextCursor::EndOfWord, //!< Move to the end of the current word.
        PreviousWord = QTextCursor::PreviousWord, //!< Move to the beginning of the previous word.
        NextWord = QTextCursor::NextWord, //!< Move to the next word.
        WordLeft = QTextCursor::WordLeft, //!< Move left one word.

        PreviousCharacter = QTextCursor::PreviousCharacter, //!< Move to the previous character.
        NextCharacter = QTextCursor::NextCharacter, //!< Move to the next character.
        Up = QTextCursor::Up, //!< Move up one line.
        Down = QTextCursor::Down, //!< Move down one line.
        Left = QTextCursor::Left, //!< Move left one character.
        Right = QTextCursor::Right //!< Move right one character.
    };

public Q_SLOTS:

    /** Return the position the cursor is on. */
    int position() const;
    /** Return the position of the anchor. */
    int anchor() const;
    /** Set the position the cursor is on. */
    void setPosition(int pos, bool moveAnchor = true);

    /** Moves the cursor to the defined position.
    *
    * The \p operation defines the \a MoveOperation that should be performed
    * while \p moveAnchor defines if the anchor should be moved too to the
    * new position.
    *
    * The operation could be one of the following;
    * \li Start = Move to the start of the document.
    * \li End = Move to the end of the document.
    * \li StartOfLine = Move to the start of the current line.
    * \li EndOfLine = Move to the end of the current line.
    * \li StartOfBlock = Move to the start of the current block.
    * \li EndOfBlock = Move to the end of the current block.
    * \li PreviousBlock = Move to the start of the previous block.
    * \li NextBlock = Move to the beginning of the next block.
    * \li StartOfWord = Move to the start of the current word.
    * \li EndOfWord = Move to the end of the current word.
    * \li PreviousWord = Move to the beginning of the previous word.
    * \li NextWord = Move to the next word.
    * \li WordLeft = Move left one word.
    * \li PreviousCharacter = Move to the previous character.
    * \li NextCharacter = Move to the next character.
    * \li Up = Move up one line.
    * \li Down = Move down one line.
    * \li Left = Move left one character.
    * \li Right = Move right one character.
    *
    * Python sample code;
    * \code
    * import Words
    * doc = Words.mainFrameSet().document()
    * cursor = doc.rootFrame().firstCursorPosition()
    * cursor.movePosition(cursor.NextBlock, true)
    * cursor.movePosition(cursor.End)
    * \endcode
    */
    bool movePosition(int operation, bool moveAnchor = true);

    /** Return true if the cursor is at the beginning of the document. */
    //bool atStart() const;
    /** Return true if the cursor is at the end of the document. */
    //bool atEnd() const;

    /** Return the block content as plain text. */
    QString text() const;

    /** Returns true if the cursor knows about a selection. */
    bool hasSelection() const;
    /** Return the position where the selection starts. */
    int selectionStart() const;
    /** Return the position where the selection ends. */
    int selectionEnd() const;
    /** Return the selected plain text. */
    QString selectedText() const;
    /** Return the selected HTML. */
    QString selectedHtml() const;

    /** Return the current \a TextFrame object if valid. */
    QObject* currentFrame();
    /** Return the current \a TextList object if valid. */
    QObject* currentList();
    /** Return the current \a TextTable object if valid. */
    QObject* currentTable();

    /** Insert the \p text plain-text at the current cursor position. */
    void insertText(const QString& text);
    /** Insert the \p html html-text at the current cursor position. */
    void insertHtml(const QString& html);
    /** Insert a block element at the current cursor position. */
    void insertBlock();
    /** Insert a default block element at the current cursor position. */
    void insertDefaultBlock();
    /** Insert and return a new \a TextFrame object at the current cursor position. */
    QObject* insertFrame();
    /** Insert and return a new \a TextList object at the current cursor position. */
    QObject* insertList();
    /** Insert and return a new \a TextTable object at the current cursor position. */
    QObject* insertTable(int rows, int columns);

    //void insertFragment ( const QTextDocumentFragment & fragment )
    //void insertImage ( const QTextImageFormat & format, QTextFrameFormat::Position alignment )

private:
    QTextCursor m_cursor;
};

}

#endif
