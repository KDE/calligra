/****************************************************************************
** $Id$
**
** Definition of the QTextEdit class
**
** Created : 990101
**
** Copyright (C) 1992-2000 Trolltech AS.  All rights reserved.
**
** This file is part of the widgets module of the Qt GUI Toolkit.
**
** This file may be distributed under the terms of the Q Public License
** as defined by Trolltech AS of Norway and appearing in the file
** LICENSE.QPL included in the packaging of this file.
**
** This file may be distributed and/or modified under the terms of the
** GNU General Public License version 2 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.
**
** Licensees holding valid Qt Enterprise Edition or Qt Professional Edition
** licenses may use this file in accordance with the Qt Commercial License
** Agreement provided with the Software.
**
** This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
** WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
**
** See http://www.trolltech.com/pricing.html or email sales@trolltech.com for
**   information about Qt Commercial License Agreements.
** See http://www.trolltech.com/qpl/ for QPL licensing information.
** See http://www.trolltech.com/gpl/ for GPL licensing information.
**
** Contact info@trolltech.com if any conditions of this licensing are
** not clear to you.
**
**********************************************************************/

#ifndef QTEXTEDIT_H
#define QTEXTEDIT_H

#ifndef QT_H
#include "qlist.h"
#include "qpixmap.h"
#include "qcolor.h"
#include "qtextview.h"
#endif // QT_H

#ifndef QT_NO_TEXTEDIT

class Q_EXPORT QTextEdit : public QTextView
{
    Q_OBJECT
    Q_PROPERTY( int undoDepth READ undoDepth WRITE setUndoDepth )
    Q_PROPERTY( bool overWriteMode READ isOverwriteMode WRITE setOverwriteMode )

public:
    QTextEdit(QWidget *parent = 0, const char *name = 0 );
    ~QTextEdit();

    void getCursorPosition( int &parag, int &index ) const;
    bool isModified() const;
    bool italic() const;
    bool bold() const;
    bool underline() const;
    QString family() const;
    int pointSize() const;
    QColor color() const;
    QFont font() const;
    int alignment() const;
    bool isOverwriteMode() const;
    int undoDepth() const;
    virtual void insert( const QString &text, bool indent = FALSE, bool checkNewLine = TRUE, bool removeSelected = TRUE );
    virtual bool getFormat( int parag, int index, QFont &font, QColor &color );

public slots:
    virtual void setOverwriteMode( bool b );
    virtual void undo();
    virtual void redo();
    virtual void cut();
    virtual void paste();
    virtual void pasteSubType( const QCString &subtype );
    virtual void indent();
    virtual void setItalic( bool b );
    virtual void setBold( bool b );
    virtual void setUnderline( bool b );
    virtual void setFamily( const QString &f );
    virtual void setPointSize( int s );
    virtual void setColor( const QColor &c );
    virtual void setFont( const QFont &f );
    virtual void setAlignment( int a );
    virtual void setParagType( QStyleSheetItem::DisplayMode dm, QStyleSheetItem::ListStyle listStyle );
    virtual void setCursorPosition( int parag, int index );
    virtual void setSelection( int parag_from, int index_from, int parag_to, int index_to );
    virtual void setModified( bool m );
    virtual void resetFormat();
    virtual void setUndoDepth( int d );
    virtual void save( const QString &fn = QString::null );
    virtual void removeSelectedText();

signals:
    void undoAvailable( bool yes );
    void redoAvailable( bool yes );
    void currentFontChanged( const QFont &f );
    void currentColorChanged( const QColor &c );
    void currentAlignmentChanged( int a );
    void cursorPositionChanged( QTextCursor *c );
    void returnPressed();
    void modificationChanged( bool m );

protected:
    enum KeyboardAction { // keep in sync with QTextView
	ActionBackspace,
	ActionDelete,
	ActionReturn,
	ActionKill
    };

    enum MoveDirection { // keep in sync with QTextView
	MoveLeft,
	MoveRight,
	MoveUp,
	MoveDown,
	MoveHome,
	MoveEnd,
	MovePgUp,
	MovePgDown
    };

    void setFormat( QTextFormat *f, int flags );
    void ensureCursorVisible();
    void placeCursor( const QPoint &pos, QTextCursor *c = 0 );
    void moveCursor( MoveDirection direction, bool shift, bool control );
    void moveCursor( MoveDirection direction, bool control );
    void doKeyboardAction( KeyboardAction action );
    QTextCursor *textCursor() const;

private:
    bool isReadOnly() const { return FALSE; }
    void emitUndoAvailable( bool b ) { emit undoAvailable( b ); }
    void emitRedoAvailable( bool b ) { emit redoAvailable( b ); }
    void emitCurrentFontChanged( const QFont &f ) { emit currentFontChanged( f ); }
    void emitCurrentColorChanged( const QColor &c ) { emit currentColorChanged( c ); }
    void emitCurrentAlignmentChanged( int a ) { emit currentAlignmentChanged( a ); }
    void emitCursorPositionChanged( QTextCursor *c ) { emit cursorPositionChanged( c ); }
    void emitReturnPressed() { emit returnPressed(); }
    void emitModificationChanged( bool m ) { emit modificationChanged( m ); }

private:	// Disabled copy constructor and operator=
#if defined(Q_DISABLE_COPY)
    QTextEdit( const QTextEdit & );
    QTextEdit& operator=( const QTextEdit & );
#endif
};

inline void QTextEdit::getCursorPosition( int &parag, int &index ) const
{
    QTextView::getCursorPosition( parag, index );
}

inline bool QTextEdit::isModified() const
{
    return QTextView::isModified();
}

inline bool QTextEdit::italic() const
{
    return QTextView::italic();
}

inline bool QTextEdit::bold() const
{
    return QTextView::bold();
}

inline bool QTextEdit::underline() const
{
    return QTextView::underline();
}

inline QString QTextEdit::family() const
{
    return QTextView::family();
}

inline int QTextEdit::pointSize() const
{
    return QTextView::pointSize();
}

inline QColor QTextEdit::color() const
{
    return QTextView::color();
}

inline QFont QTextEdit::font() const
{
    return QTextView::font();
}

inline int QTextEdit::alignment() const
{
    return QTextView::alignment();
}

inline bool QTextEdit::isOverwriteMode() const
{
    return QTextView::isOverwriteMode();
}

inline int QTextEdit::undoDepth() const
{
    return QTextView::undoDepth();
}

inline void QTextEdit::insert( const QString &text, bool indent, bool checkNewLine, bool removeSelected )
{
    QTextView::insert( text, indent, checkNewLine, removeSelected );
}

inline void QTextEdit::setOverwriteMode( bool b )
{
    QTextView::setOverwriteMode( b );
}

inline void QTextEdit::undo()
{
    QTextView::undo();
}

inline void QTextEdit::redo()
{
    QTextView::redo();
}

inline void QTextEdit::cut()
{
    QTextView::cut();
}

inline void QTextEdit::paste()
{
    QTextView::paste();
}

inline void QTextEdit::pasteSubType( const QCString &subtype )
{
    QTextView::pasteSubType( subtype );
}

inline void QTextEdit::indent()
{
    QTextView::indent();
}

inline void QTextEdit::setItalic( bool b )
{
    QTextView::setItalic( b );
}

inline void QTextEdit::setBold( bool b )
{
    QTextView::setBold( b );
}

inline void QTextEdit::setUnderline( bool b )
{
    QTextView::setUnderline( b );
}

inline void QTextEdit::setFamily( const QString &f )
{
    QTextView::setFamily( f );
}

inline void QTextEdit::setPointSize( int s )
{
    QTextView::setPointSize( s );
}

inline void QTextEdit::setColor( const QColor &c )
{
    QTextView::setColor( c );
}

inline void QTextEdit::setFont( const QFont &f )
{
    QTextView::setFontInternal( f );
}

inline void QTextEdit::setAlignment( int a )
{
    QTextView::setAlignment( a );
}

inline void QTextEdit::setParagType( QStyleSheetItem::DisplayMode dm, QStyleSheetItem::ListStyle listStyle )
{
    QTextView::setParagType( dm, listStyle );
}

inline void QTextEdit::setCursorPosition( int parag, int index )
{
    QTextView::setCursorPosition( parag, index );
}

inline void QTextEdit::setSelection( int parag_from, int index_from, int parag_to, int index_to )
{
    QTextView::setSelection( parag_from, index_from, parag_to, index_to );
}

inline void QTextEdit::setModified( bool m )
{
    QTextView::setModified( m );
}

inline void QTextEdit::resetFormat()
{
    QTextView::resetFormat();
}

inline void QTextEdit::setUndoDepth( int d )
{
    QTextView::setUndoDepth( d );
}

inline void QTextEdit::save( const QString &fn )
{
    QTextView::save( fn );
}

inline void QTextEdit::setFormat( QTextFormat *f, int flags )
{
    QTextView::setFormat( f, flags );
}

inline void QTextEdit::ensureCursorVisible()
{
    QTextView::ensureCursorVisible();
}

inline void QTextEdit::placeCursor( const QPoint &pos, QTextCursor *c )
{
    QTextView::placeCursor( pos, c );
}

inline void QTextEdit::moveCursor( MoveDirection direction, bool shift, bool control )
{
    QTextView::moveCursor( (MoveDirectionPrivate)direction, shift, control );
}

inline void QTextEdit::moveCursor( MoveDirection direction, bool control )
{
    QTextView::moveCursor( (MoveDirectionPrivate)direction, control );
}

inline void QTextEdit::removeSelectedText()
{
    QTextView::removeSelectedText();
}

inline void QTextEdit::doKeyboardAction( KeyboardAction action )
{
    QTextView::doKeyboardAction( (KeyboardActionPrivate)action );
}

inline QTextCursor *QTextEdit::textCursor() const
{
    return cursor;
}

inline bool QTextEdit::getFormat( int parag, int index, QFont &font, QColor &color )
{
    return QTextView::getFormat( parag, index, font, color );
}




#endif // QT_NO_TEXTEDIT

#endif
