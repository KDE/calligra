/* This file is part of the KDE project
   Copyright (C) 2001 David Faure <faure@kde.org>

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

#ifndef kotextview_h
#define kotextview_h

#include <qobject.h>
#include <qpoint.h>
#include <qcolor.h>
#include <qfont.h>
#include <koRuler.h> // for KoTabulatorList
class KoTextObject;
class KoTextDocument;
class KoTextFormat;
class KoParagCounter;
class KCommand;
class QTimer;
#include "qrichtext_p.h"
using namespace Qt3;
class KoBorder;
class KoStyle;

/**
 * Object that is created to edit a KoTextObject.
 * It handles all the events for it (mouse, keyboard).
 * There can be several KoTextView instances for the same KoTextObject.
 */
class KoTextView : public QObject
{
    Q_OBJECT
public:
    /**
     * Constructor. A KoTextView is associated to a KoTextObject.
     * Don't forget to call updateUI(true,true) in your derived constructor
     */
    KoTextView( KoTextObject *textobj );
    virtual ~KoTextView();

    /** Call this before deleting */
    void terminate();

    KoTextObject * textObject() const { return m_textobj; }
    QTextCursor * cursor() const { return m_cursor; }
    KoTextDocument * textDocument() const;

    /** Return true if the view is allowed to modify the text object.
        This is the case by default */
    bool isReadWrite() const { return m_bReadWrite; }
    /** Call setReadWrite(false) to make the text view readonly */
    void setReadWrite( bool b ) { m_bReadWrite = b; }

    KoTextFormat * currentFormat() const { return m_currentFormat; }
    void setCurrentFormat( KoTextFormat *fmt ) { m_currentFormat = fmt; }

    void setBold(bool on);
    void setItalic(bool on);
    void setUnderline(bool on);
    void setStrikeOut(bool on);
    void setTextColor(const QColor &color);
    void setPointSize( int s );
    void setFamily(const QString &font);
    void setFont(const QFont &font, bool _subscript, bool _superscript, const QColor &col, int flags);
    void setTextSubScript(bool on);
    void setTextSuperScript(bool on);

    void setDefaultFormat();

    QColor textColor() const;
    QFont textFont() const;
    QString textFontFamily()const;


    // -- Paragraph settings --
    KCommand * setCounterCommand( const KoParagCounter & counter );
    KCommand * setAlignCommand( int align );
    KCommand * setPageBreakingCommand( int pageBreaking );
    KCommand * setLineSpacingCommand( double spacing );
    KCommand * setBordersCommand( const KoBorder& leftBorder, const KoBorder& rightBorder, const KoBorder& bottomBorder, const KoBorder& topBorder );
    KCommand * setMarginCommand( QStyleSheetItem::Margin m, double margin );
    KCommand * setTabListCommand( const KoTabulatorList & tabList );

    void applyStyle( const KoStyle * style );

    void dragStarted();
    void focusInEvent();
    void focusOutEvent();
    void handleKeyPressEvent( QKeyEvent * e );
    void handleMousePressEvent( QMouseEvent* e, const QPoint& iPoint );
    void handleMouseMoveEvent( QMouseEvent* e, const QPoint& iPoint );
    void handleMouseReleaseEvent();
    void handleMouseDoubleClickEvent( QMouseEvent* e, const QPoint& /* Currently unused */ );
    bool maybeStartDrag( QMouseEvent* e );

    QTextCursor selectWordUnderCursor();
public slots:
    /** Show the current settings (those for the paragraph and character under the cursor), in the GUI.
        Needs to be implemented in the application. @see showFormat */
    virtual void updateUI( bool updateFormat, bool force = false ) = 0;
    virtual void ensureCursorVisible() = 0;
    void showCurrentFormat();

    // This allows KoTextObject to hide/show all the cursors before modifying anything
    void hideCursor() { drawCursor( false ); }
    void showCursor() { drawCursor( true ); }

protected slots:
    /** Start a drag */
    virtual void startDrag() = 0;

signals:
    void copy();
    void cut();
    void paste();

protected:
    /** Show the settings of this format in the GUI. Needs to be implemented in the application. */
    virtual void showFormat( KoTextFormat *format ) = 0;

    /** Draws the cursor (or hides it if b is false).
     * The default implementation only changes an internal flag, make sure to reimplement
     * and to call the parent implementation (in all cases)
     */
    virtual void drawCursor( bool b );
    void placeCursor( const QPoint &pos /* in internal coordinates */ );

    /** Reimplement this to handle PageUp. Example implementation:
        textView->cursor()->gotoPageUp( scrollview->visibleHeight() ); */
    virtual void pgUpKeyPressed() = 0;
    /** Reimplement this to handle PageDown. Example implementation:
        textView->cursor()->gotoPageDown( scrollview->visibleHeight() ); */
    virtual void pgDownKeyPressed() = 0;
    /** Reimplement this to handle CTRL+PageUp. Default implementation calls pgUpKeyPressed */
    virtual void ctrlPgUpKeyPressed() { pgUpKeyPressed(); }
    /** Reimplement this to handle CTRL+PageDown. Default implementation calls pgDownKeyPressed */
    virtual void ctrlPgDownKeyPressed() { pgDownKeyPressed(); }

    void deleteWordBack();
    void deleteWordForward();

private slots:
    void blinkCursor();
    void setCursor( QTextCursor * _cursor ) { *m_cursor = *_cursor; }

private:
    enum CursorAction { // keep in sync with QTextEdit
        MoveBackward,
        MoveForward,
        MoveWordBackward,
        MoveWordForward,
        MoveUp,
        MoveDown,
        MoveLineStart,
        MoveLineEnd,
        MoveHome,
        MoveEnd,
        MovePgUp,
        MovePgDown,
        MoveParagUp, // libkotext-specific
        MoveParagDown, // libkotext-specific
        MoveViewportUp, // KWord-specific
        MoveViewportDown // KWord-specific
    };

    void moveCursor( CursorAction action, bool select );
    void moveCursor( CursorAction action );

private:
    KoTextObject *m_textobj;
    QTextCursor *m_cursor;
    KoTextFormat *m_currentFormat;
    QTimer *blinkTimer, *dragStartTimer;
    QPoint dragStartPos;
    bool m_cursorVisible;
    bool blinkCursorVisible;
    bool inDoubleClick;
    bool mightStartDrag;
    bool m_bReadWrite;
};

#endif
