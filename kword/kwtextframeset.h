/* This file is part of the KDE project
   Copyright (C) 1998, 1999, 2000 Reginald Stadlbauer <reggie@kde.org>

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

#ifndef kwtextframeset_h
#define kwtextframeset_h

#include "qrichtext_p.h"
#include "kwframe.h"
#include "kwtextparag.h"
class KWStyle;
class KWTextFrameSet;

/**
 * Class: KWTextFrameSet
 * Contains text (KWTextDocument) and frames to display
 * that text.
 */
class KWTextFrameSet : public KWFrameSet, public QTextFlow
{
    Q_OBJECT
public:
    // constructor
    KWTextFrameSet( KWDocument *_doc );
    // destructor
    ~KWTextFrameSet();

    virtual FrameType getFrameType() { return FT_TEXT; }

    KWTextDocument *textDocument() const { return text; }

    virtual KWFrameSetEdit * createFrameSetEdit( KWCanvas * canvas );

    /** reshuffle frames so text is always displayed from top-left down and then right. */
    virtual void updateFrames();

    virtual bool isPTYInFrame( unsigned int _frame, unsigned int _ypos );

    // Convert the @p in the contents coordinates (those visible to the user)
    // into a point in the internal qtextdoc coordinates.
    // If @p onlyY is true, the X coordinate isn't taken into account - but should be 0
    QPoint contentsToInternal( QPoint p, bool onlyY = false ) const;

    // Convert the @p in the internal qtextdoc coordinates
    // into a point in the contents coordinates (those visible to the user).
    QPoint internalToContents( QPoint p ) const;

    // Return the available height (sum of all frames' height)
    // Used to know if we need to create more pages.
    int availableHeight() const;

    // Views notify the KWTextFrameSet of which area of the text
    // they're looking at, so that formatMore() ensures it's always formatted
    // correctly.
    // @p bottom is usually contentsY()+visibleHeight()
    void updateViewArea( QWidget * w, int bottom );

    virtual void save( QDomElement &parentElem );
    virtual void load( QDomElement &attributes );

    // stupid updating of all styles.
    void updateAllStyles();

    /** returns a deep copy of self (and all it contains) */
    KWTextFrameSet *getCopy();

    virtual void drawContents( QPainter *p, const QRect & crect,
                               QColorGroup &cg, bool onlyChanged )
    {
        // Called by KWCanvas when no focus (->no cursor)
        drawContents( p, crect, cg, onlyChanged, false, 0L );
    }

    void drawContents( QPainter *p, const QRect & crect,
                       QColorGroup &gb, bool onlyChanged,
                       bool drawCursor, QTextCursor *cursor );

    void drawCursor( QPainter *p, QTextCursor *cursor, bool cursorVisible );

    void insert( QTextCursor * cursor, QTextFormat * currentFormat, const QString &text );
    void removeSelectedText( QTextCursor * cursor );
    void undo();
    void redo();
    void clearUndoRedoInfo();
    void pasteSubType( QTextCursor * cursor, QTextFormat * currentFormat, const QCString& );

    /** Set format changes on selection or current cursor */
    void setFormat( QTextCursor * cursor, QTextFormat * & currentFormat, QTextFormat *format, int flags);

    enum KeyboardActionPrivate { // keep in sync with QTextEdit
	ActionBackspace,
	ActionDelete,
	ActionReturn,
	ActionKill
    };
    void doKeyboardAction( QTextCursor * cursor, KeyboardActionPrivate action );

    // -- Paragraph settings --
    void setCounter( QTextCursor * cursor, const Counter & counter );
    void setAlign( QTextCursor * cursor, int align );
    void setLineSpacing( QTextCursor * cursor, KWUnit spacing );
    void setBorders( QTextCursor * cursor, Border leftBorder, Border rightBorder, Border bottomBorder, Border topBorder );
    void setMargin( QTextCursor * cursor, QStyleSheetItem::Margin m, KWUnit margin );
    void applyStyle( QTextCursor * cursor, const KWStyle * style, int selectionId = QTextDocument::Standard );
    void applyStyleChange( const QString & changedStyle );

    void setTabList( QTextCursor * cursor,const QList<KoTabulator> *tabList );

    virtual void layout();

    // reimplemented from QTextFlow
    virtual void setWidth( int w );
    virtual int adjustLMargin( int yp, int margin, int space );
    virtual int adjustRMargin( int yp, int margin, int space );
    virtual void adjustFlow( int  &yp, int w, int h, bool pages = TRUE );
    virtual void draw( QPainter *p, int cx, int cy, int cw, int ch );
    virtual void eraseAfter( QTextParag *parag, QPainter *p );

signals:
    void hideCursor();
    void showCursor();
    // Emitted when the formatting under the cursor may have changed.
    // The Edit object should re-read settings and update the UI.
    void updateUI();
    // Same thing, when the current format (of the edit object) was changed
    void showCurrentFormat();
    // Emitted when, well, what was changed should be repainted by the views
    void repaintChanged();
    // The views should make sure the cursor is visible
    void ensureCursorVisible();

public slots:
    void formatMore();
private slots:
    void doChangeInterval();

protected:
    void storeParagUndoRedoInfo( QTextCursor * cursor, int selectionId = QTextDocument::Standard );
    void readFormats( QTextCursor &c1, QTextCursor &c2, int oldLen, QTextString &text, bool fillStyles = false );
    void setLastFormattedParag( QTextParag *parag ) { m_lastFormatted = parag; }

private:
    /**
     * The undo-redo structure holds the _temporary_ information that _will_
     * be used to create an undo/redo command. For instance, when typing "a"
     * and then "b", we don't want a command for each letter. So we keep adding
     * info to this structure, and when the user does something else and we
     * call clear(), it's at that point that the command is created.
     */
    struct UndoRedoInfo { // borrowed from QTextEdit
        enum Type { Invalid, Insert, Delete, Return, RemoveSelected, Format,
                    Alignment, Counter, Margin, LineSpacing, Borders, Tabulator };
        UndoRedoInfo( KWTextFrameSet * fs );
        ~UndoRedoInfo();
        void clear();
        bool valid() const;

        QString name;
        QTextString text;
        int id;
        int index;
        int eid;
        int eindex;
        QTextFormat *format;
        int flags;
        Type type;
        QStyleSheetItem::Margin margin; // if type==Margin
        KWTextFrameSet *textfs;
        QTextCursor *cursor; // basically a "mark" of the view that started this undo/redo info
        // If the view changes, the next call to checkUndoRedoInfo will terminate the previous view's edition

        QValueList<KWParagLayout> oldParagLayouts;
        KWParagLayout newParagLayout;
        QValueList< QVector<QStyleSheetItem> > oldStyles;
    };
    void checkUndoRedoInfo( QTextCursor * cursor, UndoRedoInfo::Type t );

    KWTextDocument *text;
    UndoRedoInfo undoRedoInfo;                 // Currently built undo/redo info
    QTextParag *m_lastFormatted;               // Idle-time-formatting stuff
    QTimer *formatTimer, *changeIntervalTimer; // Same
    int interval;                              // Same
    int m_width;                               // Copy of private QTextFlow::width
    int m_availableHeight;                     // Sum of the height of all our frames
    QMap<QWidget *, int> m_mapViewAreas;       // Store the "needs" of each view
};

/**
 * Object that is created to edit a Text frame set (KWTextFrameSet).
 * It handles all the events for it.
 * In terms of doc/view design, this object is part of the _view_.
 * There can be several KWFrameSetEdit objects for the same frameset,
 * but there is only one KWFrameSetEdit object per view at a given moment.
 */
class KWTextFrameSetEdit : public QObject, public KWFrameSetEdit
{
    Q_OBJECT
public:
    KWTextFrameSetEdit( KWTextFrameSet * fs, KWCanvas * canvas );
    virtual ~KWTextFrameSetEdit();

    /**
     * Paint this frameset with a cursor
     */
    virtual void drawContents( QPainter *p, const QRect & crect,
                               QColorGroup &gb, bool onlyChanged )
    {
        textFrameSet()->drawContents( p, crect, gb, onlyChanged, true, cursor );
    }

    KWTextFrameSet * textFrameSet() const
    {
        return static_cast<KWTextFrameSet*>(frameSet());
    }
    KWTextDocument * textDocument() const
    {
        return textFrameSet()->textDocument();
    }
    QTextCursor * getCursor() const { return cursor; }

    // Events forwarded by the canvas (when being in "edit" mode)
    virtual void keyPressEvent( QKeyEvent * );
    virtual void mousePressEvent( QMouseEvent * );
    virtual void mouseMoveEvent( QMouseEvent * ); // only called if button is pressed
    virtual void mouseReleaseEvent( QMouseEvent * );
    virtual void mouseDoubleClickEvent( QMouseEvent * );
    virtual void dragEnterEvent( QDragEnterEvent * );
    virtual void dragMoveEvent( QDragMoveEvent * );
    virtual void dragLeaveEvent( QDragLeaveEvent * );
    virtual void dropEvent( QDropEvent * );
    virtual void focusInEvent();
    virtual void focusOutEvent();
    virtual void doAutoScroll( QPoint pos );
    virtual void cut();
    virtual void copy();
    virtual void paste();
    virtual void selectAll() { selectAll( true ); }

    void drawCursor( bool b );
    void insertPicture( const QString & file );

    void insertSpecialChar(QChar _c);

    void setBold(bool on);
    void setItalic(bool on);
    void setUnderline(bool on);
    void setTextColor(const QColor &color);
    void setPointSize( int s );
    void setFamily(const QString &font);
    void setFont(const QFont &font);
    void setTextSubScript(bool on);
    void setTextSuperScript(bool on);

    QColor textColor() const;
    QFont textFont() const;
    int textFontSize()const;
    QString textFontFamily()const;

    // -- Paragraph settings --
    void setCounter( const Counter & counter ) { textFrameSet()->setCounter( cursor, counter ); }
    void setAlign( int align ) { textFrameSet()->setAlign( cursor, align ); }
    void setLineSpacing( KWUnit spacing ) { textFrameSet()->setLineSpacing( cursor, spacing ); }
    void setBorders( Border leftBorder, Border rightBorder, Border bottomBorder, Border topBorder )
          { textFrameSet()->setBorders( cursor, leftBorder, rightBorder, bottomBorder, topBorder ); }
    void setMargin( QStyleSheetItem::Margin m, KWUnit margin )
          { textFrameSet()->setMargin( cursor, m, margin ); }
    void applyStyle( const KWStyle * style ) { textFrameSet()->applyStyle( cursor, style ); }

    const KWParagLayout & currentParagLayout() const { return m_paragLayout; }

    void setTabList( const QList<KoTabulator> *tabList ){textFrameSet()->setTabList(cursor,tabList); }

public slots:
    void updateUI();

protected:
    void placeCursor( const QPoint &pos /* in internal coordinates */, QTextCursor *c = 0 );
    void selectAll( bool select );

private slots:
    void blinkCursor();
    void startDrag();
    // This allows KWTextFrameSet to hide/show all the cursors before modifying anything
    void hideCursor() { drawCursor( false ); }
    void showCursor() { drawCursor( true ); }
    void ensureCursorVisible();
    void showCurrentFormat();
    void repaintChanged();

private:

    enum MoveDirectionPrivate { // keep in sync with QTextEdit
	MoveLeft,
	MoveRight,
	MoveUp,
	MoveDown,
	MoveHome,
	MoveEnd,
	MovePgUp,
	MovePgDown
    };

    void moveCursor( MoveDirectionPrivate direction, bool shift, bool control );
    void moveCursor( MoveDirectionPrivate direction, bool control );

private:
    QPoint dragStartPos;
    QPoint mousePos;
    KWParagLayout m_paragLayout;
    QTextCursor *cursor;
    QTextFormat *currentFormat;
    QTimer *blinkTimer, *dragStartTimer;
    bool cursorVisible;
    bool blinkCursorVisible;
    bool inDoubleClick;
    bool mightStartDrag;
};
#endif
