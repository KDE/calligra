/* This file is part of the KDE project
   Copyright (C) 2000 Reginald Stadlbauer <reggie@kde.org>

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

#ifndef QTEXTEDIT_H
#define QTEXTEDIT_H

#include <qwidget.h>
#include <qstring.h>
#include <qlist.h>
#include <qrect.h>
#include <qfontmetrics.h>
#include <qintdict.h>
#include <qmap.h>
#include <qstringlist.h>
#include <qfont.h>
#include <qcolor.h>
#include <qsize.h>
#include <qvaluelist.h>
#include <qvaluestack.h>
#include <qdict.h>
#include <qtextstream.h>
#include <qpainter.h>
#include <limits.h>
#include <qptrdict.h>
#include <kdebug.h>

class QPainter;
class QPaintEvent;
class QKeyEvent;
class QResizeEvent;
class QMouseEvent;
class QTimer;
class QVBox;
class QListBox;
class QFont;
class QColor;
class KTextEditDocument;
class KTextEditCommand;
class KTextEdit;
class KTextEditString;
class KTextEditCommandHistory;
class KTextEditFormat;
class KTextEditCursor;
class KTextEditParag;
class KTextEditFormatter;
class KTextEditFormat;
class KTextEditFormatCollection;

// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//#define DEBUG_COLLECTION 1

class KTextEditCursor
{
public:
    KTextEditCursor( KTextEditDocument *d );

    KTextEditParag *parag() const;
    int index() const;
    void setParag( KTextEditParag *s );

    void gotoLeft();
    void gotoRight();
    void gotoUp();
    void gotoDown();
    void gotoLineEnd();
    void gotoLineStart();
    void gotoHome();
    void gotoEnd();
    void gotoPageUp( KTextEdit *view );
    void gotoPageDown( KTextEdit *view );
    void gotoWordLeft();
    void gotoWordRight();

    void insert( const QString &s, bool checkNewLine );
    void splitAndInsertEmtyParag( bool ind = TRUE, bool updateIds = TRUE );
    bool remove();
    void indent();

    bool atParagStart();
    bool atParagEnd();

    void setIndex( int i );

    void checkIndex();

private:
    KTextEditParag *string;
    KTextEditDocument *doc;
    int idx, tmpIndex;

};

// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
class KPresenterDoc;
class KPTextObject;
class KTextEditCommandHistory
{
public:
    KTextEditCommandHistory( int s, KPresenterDoc *doc, KPTextObject *txtobj ) :
        document(doc), kptextobject(txtobj), current( -1 ), steps( s ) { history.setAutoDelete( TRUE ); }

    void addCommand( KTextEditCommand *cmd );
    KTextEditCursor *undo( KTextEditCursor *c );
    KTextEditCursor *redo( KTextEditCursor *c );

private:
    QList<KTextEditCommand> history;
    KPresenterDoc *document;
    KPTextObject *kptextobject;
    int current, steps;

};

class KTextEditCommand
{
public:
    enum Commands { Invalid, Insert, Delete, Format };
    KTextEditCommand( KTextEditDocument *d ) : doc( d ), cursor( d ) {}
    virtual ~KTextEditCommand() {}
    virtual Commands type() const { return Invalid; };

    virtual KTextEditCursor *execute( KTextEditCursor *c ) = 0;
    virtual KTextEditCursor *unexecute( KTextEditCursor *c ) = 0;

protected:
    KTextEditDocument *doc;
    KTextEditCursor cursor;

};

// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

class KTextEditDeleteCommand : public KTextEditCommand
{
public:
    KTextEditDeleteCommand( KTextEditDocument *d, int i, int idx, const QString &str )
        : KTextEditCommand( d ), id( i ), index( idx ), text( str ) {}
    virtual Commands type() const { return Delete; };

    virtual KTextEditCursor *execute( KTextEditCursor *c );
    virtual KTextEditCursor *unexecute( KTextEditCursor *c );

protected:
    int id, index;
    QString text;

};

class KTextEditInsertCommand : public KTextEditDeleteCommand
{
public:
    KTextEditInsertCommand( KTextEditDocument *d, int i, int idx, const QString &str )
        : KTextEditDeleteCommand( d, i, idx, str ) {}
    Commands type() const { return Insert; };

    virtual KTextEditCursor *execute( KTextEditCursor *c ) { return KTextEditDeleteCommand::unexecute( c ); }
    virtual KTextEditCursor *unexecute( KTextEditCursor *c ) { return KTextEditDeleteCommand::execute( c ); }

};

class KTextEditFormatCommand : public KTextEditCommand
{
public:
    KTextEditFormatCommand( KTextEditDocument *d, int selId, KTextEditFormat *f, int flags );
    ~KTextEditFormatCommand();
    Commands type() const { return Format; }

    virtual KTextEditCursor *execute( KTextEditCursor *c );
    virtual KTextEditCursor *unexecute( KTextEditCursor *c );

protected:
    int selection;
    KTextEditFormat *format;
    int flags;

};

// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
class KTextEditDocument
{
public:
    enum SelectionIds {
        Standard = 0,
        Search,
        Temp // This selection must not be drawn, it's used e.g. by undo/redo to
        // remove multiple lines with removeSelectedText()
    };

    static const int numSelections;

    enum Bullet {
        FilledCircle,
        FilledSquare,
        OutlinedCircle,
        OutlinedSquare
    };

    KTextEditDocument( KPresenterDoc *doc, KPTextObject *txtobj );

    void clear();

    void setText( const QString &text );
    QString text( KTextEditParag *p = 0 ) const;

    int x() const;
    int y() const;
    int width() const;
    int height() const;
    void setWidth( int w );

    KTextEditParag *firstParag() const;
    KTextEditParag *lastParag() const;
    void setFirstParag( KTextEditParag *p );
    void setLastParag( KTextEditParag *p );

    void invalidate();

    void setFormatter( KTextEditFormatter *f );
    KTextEditFormatter *formatter() const;

    QColor selectionColor( int id ) const;
    bool invertSelectionText( int id ) const;
    bool hasSelection( int id ) const;
    void setSelectionStart( int id, KTextEditCursor *cursor );
    bool setSelectionEnd( int id, KTextEditCursor *cursor );
    bool removeSelection( int id );
    void selectionStart( int id, int &paragId, int &index );
    void selectionEnd( int id, int &paragId, int &index );
    void setFormat( int id, KTextEditFormat *f, int flags );
    KTextEditParag *selectionStart( int id );
    KTextEditParag *selectionEnd( int id );

    QString selectedText( int id ) const;
    void copySelectedText( int id );
    void removeSelectedText( int id, KTextEditCursor *cursor );
    void indentSelection( int id );

    KTextEditParag *paragAt( int i ) const;

    void addCommand( KTextEditCommand *cmd );
    KTextEditCursor *undo( KTextEditCursor *c = 0 );
    KTextEditCursor *redo( KTextEditCursor *c  = 0 );

    KTextEditFormatCollection *formatCollection() const;

    int listIndent( int depth ) const;
    Bullet bullet( int depth ) const;
    QColor bulletColor( int depth ) const;

    bool find( const QString &expr, bool cs, bool wo, bool forward, int *parag, int *index, KTextEditCursor *cursor );

    void setParagSpacing( int s );
    void setLineSpacing( int s );
    void setMargin( int m );
    int paragSpacing( KTextEditParag *p = 0 ) const;
    int lineSpacing() const;
    int margin() const { return marg; }

    bool inSelection( int selId, const QPoint &pos ) const;

    void draw( QPainter *p, const QColorGroup &cg );
    void zoom( float f );
    void unzoom();

    void enableDrawAllInOneColor( const QColor &c ) { allColor = c; allInOne = TRUE; }
    void disableDrawAllInOneColor() { allInOne = FALSE; }

    bool drawAllInOneColor() const { return allInOne; }
    QColor allInOneColor() const { return allColor; }

    void setAlignmentToAll( int a );
    void setFontToAll( const QFont &f );
    void setColorToAll( const QColor &c );
    void setBoldToAll( bool b );
    void setItalicToAll( bool b );
    void setUnderlineToAll( bool b );
    void setPointSizeToAll( int s );
    void setFamilyToAll( const QString &f );

    struct TextSettings
    {
        TextSettings() {
            bulletType[0] = KTextEditDocument::FilledCircle;
            bulletType[1] = KTextEditDocument::FilledSquare;
            bulletType[2] = KTextEditDocument::OutlinedCircle;
            bulletType[3] = KTextEditDocument::OutlinedSquare;
            bulletColor[0] = bulletColor[1] = bulletColor[2] = bulletColor[3] = Qt::black;
            lineSpacing = paragSpacing = margin = 0;
        }

        KTextEditDocument::Bullet bulletType[4];
        QColor bulletColor[4];
        int lineSpacing, paragSpacing, margin;
    };

    TextSettings textSettings() const { return txtSettings; }
    void setTextSettings( TextSettings s );

private:
    struct Selection {
        KTextEditParag *startParag, *endParag;
        int startIndex;
    };

    int cx, cy, cw;
    KTextEditParag *fParag, *lParag;
    QMap<int, QColor> selectionColors;
    QMap<int, Selection> selections;
    QMap<int, bool> selectionText;
    KTextEditCommandHistory *commandHistory;
    KTextEditFormatter *pFormatter;
    int ls, ps;
    QColor allColor;
    bool allInOne;
    int listMult;
    int oldListMult;
    int oldLineSpacing, oldParagSpacing;
    TextSettings txtSettings;
    int marg;
    KPresenterDoc *kpr_doc;

};

// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

class KTextEditString
{
public:
    struct Char {
    public:
        Char() : format( 0 ), lineStart( 0 ) {}
        ~Char() { format = 0; }
        QChar c;
        ushort x;
        KTextEditFormat *format;
        uint lineStart : 1;
    private:
        Char &operator=( const Char & ) {
            return *this;
        }

    };

    KTextEditString();

    QString toString() const;

    Char &at( int i ) const;
    int length() const;

    void insert( int index, const QString &s, KTextEditFormat *f );
    void truncate( int index );
    void remove( int index, int len );

    void setFormat( int index, KTextEditFormat *f, bool useCollection );

private:
    QArray<Char> data;
    QString cache;

};

// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

class KTextEditParag
{
public:
    struct LineStart {
        LineStart() : y( 0 ), baseLine( 0 ), h( 0 ) {}
        LineStart( ushort y_, ushort bl, ushort h_ ) : y( y_ ), baseLine( bl ), h( h_ ) {}
        ushort y, baseLine, h;
    };

    enum Type {
        Normal = 0,
        BulletList,
        EnumList
    };

    KTextEditParag( KTextEditDocument *d, KTextEditParag *pr, KTextEditParag *nx, bool updateIds = TRUE );
    virtual ~KTextEditParag() {}

    Type type() const;
    void setType( Type t );

    KTextEditString *string() const;
    KTextEditString::Char *at( int i ) const; // maybe remove later
    int length() const; // maybe remove later

    KTextEditDocument *document() const;

    QRect rect() const;

    KTextEditParag *prev() const;
    KTextEditParag *next() const;
    void setPrev( KTextEditParag *s );
    void setNext( KTextEditParag *s );

    void insert( int index, const QString &s );
    void append( const QString &s );
    void truncate( int index );
    void remove( int index, int len );
    void join( KTextEditParag *s );

    void invalidate( int chr );

    void move( int dy );
    void format( int start = -1, bool doMove = TRUE );

    bool isValid() const;
    bool hasChanged() const;
    void setChanged( bool b );

    int lineHeightOfChar( int i, int *bl = 0, int *y = 0 ) const;
    KTextEditString::Char *lineStartOfChar( int i, int *index = 0, int *line = 0 ) const;
    int lines() const;
    KTextEditString::Char *lineStartOfLine( int line, int *index = 0 ) const;
    int lineY( int l ) const;
    int lineBaseLine( int l ) const;
    int lineHeight( int l ) const;
    void lineInfo( int l, int &y, int &h, int &bl ) const;

    void setSelection( int id, int start, int end );
    void removeSelection( int id );
    int selectionStart( int id ) const;
    int selectionEnd( int id ) const;
    bool hasSelection( int id ) const;
    bool hasAnySelection() const;
    bool fullSelected( int id ) const;

    void setParagId( int i );
    int paragId() const;

    void indent( int *oldIndent = 0, int *newIndent = 0 );

    QMap<int, LineStart*> &lineStartList();

    void setFormat( int index, int len, KTextEditFormat *f, bool useCollection, int flags = -1 );

    int leftIndent() const;
    int listDepth() const;
    void setListDepth( int d );

    void setAlignment( int a );
    int alignment() const;

    virtual void paint( QPainter &painter, const QColorGroup &cg,
                        KTextEditCursor *cusror = 0, bool drawSelections = FALSE );

private:
    void drawParagBuffer( QPainter &painter, const QString &buffer, int startX,
                          int lastY, int baseLine, int bw, int h, bool drawSelections,
                          KTextEditFormat *lastFormat, int i, int *selectionStarts,
                          int *selectionEnds, const QColorGroup &cg  );

private:
    struct Selection {
        int start, end;
    };

    QMap<int, LineStart*> lineStarts;
    int invalid;
    QRect r;
    KTextEditParag *p, *n;
    KTextEditDocument *doc;
    bool changed;
    bool firstFormat;
    QMap<int, Selection> selections;
    int id;
    KTextEditString *str;
    Type typ;
    int left;
    int depth;
    int align;

};

// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

class KTextEditFormatter
{
public:
    KTextEditFormatter( KTextEditDocument *d );
    virtual ~KTextEditFormatter() {}
    virtual int format( KTextEditParag *parag, int start ) = 0;

protected:
    KTextEditDocument *doc;

};

// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

class KTextEditFormatterBreakWords : public KTextEditFormatter
{
public:
    KTextEditFormatterBreakWords( KTextEditDocument *d );
    int format( KTextEditParag *parag, int start );

};

// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

class KTextEditFormat
{
    friend class KTextEditFormatCollection;

public:
    enum Flags {
        Bold = 1,
        Italic = 2,
        Underline = 4,
        Family = 8,
        Size = 16,
        Color = 32,
        Font = Bold | Italic | Underline | Family | Size,
        Format = Font | Color
    };

    KTextEditFormat( const QFont &f, const QColor &c );
    KTextEditFormat( const KTextEditFormat &fm );
    QColor color() const;
    QFont font() const;
    int minLeftBearing() const;
    int minRightBearing() const;
    int width( const QChar &c ) const;
    int height() const;
    int ascent() const;
    int descent() const;

    void setBold( bool b );
    void setItalic( bool b );
    void setUnderline( bool b );
    void setFamily( const QString &f );
    void setPointSize( int s );
    void setFont( const QFont &f );
    void setColor( const QColor &c );

    bool operator==( const KTextEditFormat &f ) const;
    KTextEditFormatCollection *parent() const;
    QString key() const;

    static QString getKey( const QFont &f, const QColor &c );

    void addRef();
    void removeRef();

private:
    void update();
    void generateKey();
    const QFontMetrics *fontMetrics() const;
    KTextEditFormat() {}

private:
    QFont fn;
    QColor col;
    QFontMetrics *fm;
    int leftBearing, rightBearing;
    int widths[ 256 ];
    int hei, asc, dsc;
    KTextEditFormatCollection *collection;
    int ref;
    QString k;

};

// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

class KTextEditFormatCollection
{
public:
    KTextEditFormatCollection();
    ~KTextEditFormatCollection();


    void setDefaultFormat( KTextEditFormat *f );
    KTextEditFormat *defaultFormat() const;
    KTextEditFormat *format( KTextEditFormat *f );
    KTextEditFormat *format( KTextEditFormat *of, KTextEditFormat *nf, int flags );
    KTextEditFormat *format( const QFont &f, const QColor &c );
    void remove( KTextEditFormat *f );

    void debug();

    void zoom( float f );
    void unzoom();

private:
    KTextEditFormat *defFormat, *lastFormat, *cachedFormat;
    QDict<KTextEditFormat> cKey;
    QPtrDict<int> orig;
    KTextEditFormat *cres;
    QFont cfont;
    QColor ccol;
    QString kof, knf;
    int cflags;
    float zoomFakt;

};

class KTextEdit : public QWidget
{
    Q_OBJECT

public:
    enum ParagType {
        Normal = 0,
        BulletList,
        EnumList
    };

    KTextEdit( KPresenterDoc *doc, KPTextObject *txtobj, QWidget *parent, const QString &fn, bool tabify = FALSE );
    KTextEdit( KPresenterDoc *doc, KPTextObject *txtobj, QWidget *parent = 0, const char *name = 0 );
    virtual ~KTextEdit();

    void clear();

    KTextEditDocument *document() const;

    QString text() const;
    void setText( const QString &txt );

    void cursorPosition( int &parag, int &index );
    void selection( int &parag_from, int &index_from,
                    int &parag_to, int &index_to );
    virtual bool find( const QString &expr, bool cs, bool wo, bool forward = TRUE,
                       int *parag = 0, int *index = 0 );
    void insert( const QString &text, bool checkNewLine = FALSE );

    int paragraphs() const;
    int lines() const;
    int linesOfParagraph( int parag ) const;
    int lineOfChar( int parag, int chr );

    bool isReadOnly() const;
    bool isModified() const;

    bool italic() const;
    bool bold() const;
    bool underline() const;
    QString family() const;
    int pointSize() const;
    QColor color() const;
    QFont font() const;
    ParagType paragType() const;
    int alignment() const;
    int maxLines() const;

    void zoom( float f );
    void unzoom();

    KTextEditParag *currentParagraph() { return cursor->parag(); }
    void extendContents2Height();
    void updateCurrentFormat();

public slots:
    virtual void undo();
    virtual void redo();

    virtual void cut();
    virtual void copy();
    virtual void paste();

    virtual void indent();

    virtual void setItalic( bool b );
    virtual void setBold( bool b );
    virtual void setUnderline( bool b );
    virtual void setFamily( const QString &f );
    virtual void setPointSize( int s );
    virtual void setColor( const QColor &c );
    virtual void setFont( const QFont &f );

    virtual void setParagType( ParagType t );
    virtual void setAlignment( int );
    virtual void setListDepth( int diff );

    virtual void setCursorPosition( int parag, int index );
    virtual void setSelection( int parag_from, int index_from,
                               int parag_to, int index_to );

    virtual void setReadOnly( bool ro );
    virtual void setModified( bool m );
    virtual void selectAll( bool select );

    virtual void setMaxLines( int l );
    virtual void resetFormat();

    QSize neededSize() const { return QSize( doc->width(), doc->lastParag()->rect().bottom() + 1 ); }

signals:
    void currentFontChanged( const QFont &f );
    void currentColorChanged( const QColor &c );
    void currentAlignmentChanged( int );
    void currentParagTypeChanged( KTextEdit::ParagType );
    void textChanged();
    void exitEditMode();

protected:
    void setFormat( KTextEditFormat *f, int flags );
    void drawContents( QPainter *p, int cx, int cy, int cw, int ch );
    void keyPressEvent( QKeyEvent *e );
    void resizeEvent( QResizeEvent *e );
    void paintEvent( QPaintEvent *e );
    void mousePressEvent( QMouseEvent *e );
    void mouseMoveEvent( QMouseEvent *e );
    void mouseReleaseEvent( QMouseEvent *e );
    void mouseDoubleClickEvent( QMouseEvent *e );
    void dragEnterEvent( QDragEnterEvent *e );
    void dragMoveEvent( QDragMoveEvent *e );
    void dragLeaveEvent( QDragLeaveEvent *e );
    void dropEvent( QDropEvent *e );
    bool event( QEvent * e );
    bool eventFilter( QObject *o, QEvent *e );
    bool focusNextPrevChild( bool next );

private slots:
    void formatMore();
    void doResize();
    void doChangeInterval();
    void blinkCursor();
    void setModified();
    void startDrag();

private:
    enum MoveDirection {
        MoveLeft,
        MoveRight,
        MoveUp,
        MoveDown,
        MoveHome,
        MoveEnd,
        MovePgUp,
        MovePgDown
    };
    enum KeyboardAction {
        ActionBackspace,
        ActionDelete,
        ActionReturn
    };

    struct UndoRedoInfo {
        enum Type { Invalid, Insert, Delete, Backspace, Return, RemoveSelected };
        UndoRedoInfo( KTextEditDocument *d ) : type( Invalid ), doc( d )
        { text = QString::null; id = -1; index = -1; }
        void clear();
        inline bool valid() const { return !text.isEmpty() && id >= 0&& index >= 0; }

        QString text;
        int id;
        int index;
        Type type;
        KTextEditDocument *doc;
    };

private:
    QPixmap *bufferPixmap( const QSize &s );
    void init();
    void drawCursor( bool visible );
    void placeCursor( const QPoint &pos, KTextEditCursor *c = 0 );
    void moveCursor( int direction, bool shift, bool control );
    void moveCursor( int direction, bool control );
    void removeSelectedText();
    void doKeyboardAction( int action );
    void checkUndoRedoInfo( UndoRedoInfo::Type t );
    void repaintChanged();

private:
    KTextEditDocument *doc;
    KTextEditCursor *cursor;
    bool drawAll;
    bool mousePressed;
    QTimer *formatTimer, *changeIntervalTimer, *blinkTimer, *dragStartTimer;
    KTextEditParag *lastFormatted;
    int interval;
    UndoRedoInfo undoRedoInfo;
    KTextEditFormat *currentFormat;
    QPainter painter;
    QPixmap *doubleBuffer;
    int currentAlignment;
    ParagType currentParagType;
    bool inDoubleClick;
    QPoint oldMousePos, mousePos;
    QPixmap *buf_pixmap;
    bool cursorVisible, blinkCursorVisible;
    bool readOnly, modified, mightStartDrag;
    QPoint dragStartPos;
    int mLines;

};


// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

inline KTextEditParag *KTextEditCursor::parag() const
{
    return string;
}

inline int KTextEditCursor::index() const
{
    return idx;
}

inline void KTextEditCursor::setIndex( int i )
{
    tmpIndex = -1;
    idx = i;
}

inline void KTextEditCursor::setParag( KTextEditParag *s )
{
    idx = 0;
    string = s;
    tmpIndex = -1;
}

inline void KTextEditCursor::checkIndex()
{
    if ( idx >= string->length() )
        idx = string->length() - 1;
}

// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

inline int KTextEditDocument::x() const
{
    return cx;
}

inline int KTextEditDocument::y() const
{
    return cy + marg;
}

inline int KTextEditDocument::width() const
{
    return cw - 2 * marg;
}

inline int KTextEditDocument::height() const
{
    if ( !lParag )
        return 0;
    return lParag->rect().bottom() + 1;
}

inline KTextEditParag *KTextEditDocument::firstParag() const
{
    return fParag;
}

inline KTextEditParag *KTextEditDocument::lastParag() const
{
    return lParag;
}

inline void KTextEditDocument::setFirstParag( KTextEditParag *p )
{
    fParag = p;
}

inline void KTextEditDocument::setLastParag( KTextEditParag *p )
{
    lParag = p;
}

inline void KTextEditDocument::setWidth( int w )
{
    cw = w;
}

inline void KTextEditDocument::setFormatter( KTextEditFormatter *f )
{
    pFormatter = f;
}

inline KTextEditFormatter *KTextEditDocument::formatter() const
{
    return pFormatter;
}

inline QColor KTextEditDocument::selectionColor( int id ) const
{
    return selectionColors[ id ];
}

inline bool KTextEditDocument::invertSelectionText( int id ) const
{
    return selectionText[ id ];
}

inline bool KTextEditDocument::hasSelection( int id ) const
{
    return selections.find( id ) != selections.end();
}

inline void KTextEditDocument::setSelectionStart( int id, KTextEditCursor *cursor )
{
    Selection sel;
    sel.startParag = cursor->parag();
    sel.endParag = cursor->parag();
    sel.startParag->setSelection( id, cursor->index(), cursor->index() );
    sel.startIndex = cursor->index();
    selections[ id ] = sel;
}

inline KTextEditParag *KTextEditDocument::paragAt( int i ) const
{
    KTextEditParag *s = fParag;
    while ( s ) {
        if ( s->paragId() == i )
            return s;
        s = s->next();
    }
    return 0;
}

inline int KTextEditDocument::listIndent( int depth ) const
{
    // #######
    return ( depth + 1 ) * listMult;
}

inline KTextEditDocument::Bullet KTextEditDocument::bullet( int depth ) const
{
    if ( depth == 0 )
        return txtSettings.bulletType[ 0 ];
    else if ( depth == 1 )
        return txtSettings.bulletType[ 1 ];
    else if ( depth == 2 )
        return txtSettings.bulletType[ 2 ];
    else if ( depth == 3 )
        return txtSettings.bulletType[ 3 ];
    else
        return txtSettings.bulletType[ 0 ];
}

inline QColor KTextEditDocument::bulletColor( int depth ) const
{
    if ( depth == 0 )
        return txtSettings.bulletColor[ 0 ];
    else if ( depth == 1 )
        return txtSettings.bulletColor[ 1 ];
    else if ( depth == 2 )
        return txtSettings.bulletColor[ 2 ];
    else if ( depth == 3 )
        return txtSettings.bulletColor[ 3 ];
    else
        return txtSettings.bulletColor[ 0 ];
}

inline int KTextEditDocument::paragSpacing( KTextEditParag * ) const
{
    return ps;
}

inline int KTextEditDocument::lineSpacing() const
{
    return ls;
}

// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

inline KTextEditFormat::KTextEditFormat( const QFont &f, const QColor &c )
    : fn( f ), col( c ), fm( new QFontMetrics( f ) ), collection( 0L )
{
    leftBearing = fm->minLeftBearing();
    rightBearing = fm->minRightBearing();
    hei = fm->height();
    asc = fm->ascent();
    dsc = fm->descent();
    for ( int i = 0; i < 256; ++i )
        widths[ i ] = 0;
    generateKey();
    addRef();
}

inline KTextEditFormat::KTextEditFormat( const KTextEditFormat &f ) : collection( 0L )
{
    //qDebug("constructing a format from another one");
    fn = f.fn;
    col = f.col;
    fm = new QFontMetrics( fn );
    leftBearing = f.leftBearing;
    rightBearing = f.rightBearing;
    for ( int i = 0; i < 256; ++i )
        widths[ i ] = f.widths[ i ];
    hei = f.hei;
    asc = f.asc;
    dsc = f.dsc;
    generateKey();
    addRef();
}

inline void KTextEditFormat::update()
{
    *fm = QFontMetrics( fn );
    leftBearing = fm->minLeftBearing();
    rightBearing = fm->minRightBearing();
    hei = fm->height();
    asc = fm->ascent();
    dsc = fm->descent();
    for ( int i = 0; i < 256; ++i )
        widths[ i ] = 0;
    generateKey();
}

inline const QFontMetrics *KTextEditFormat::fontMetrics() const
{
    return fm;
}

inline QColor KTextEditFormat::color() const
{
    return col;
}

inline QFont KTextEditFormat::font() const
{
    return fn;
}

inline int KTextEditFormat::minLeftBearing() const
{
    return leftBearing;
}

inline int KTextEditFormat::minRightBearing() const
{
    return rightBearing;
}

inline int KTextEditFormat::width( const QChar &c ) const
{
    if ( c == '\t' )
        return 30;
    int w = 0;
    if ( c.unicode() < 256 )
        w = widths[ c.unicode() ];
    if ( w == 0 ) {
        w = fm->width( c );
        if ( c.unicode() < 256 )
            ( (KTextEditFormat*)this )->widths[ c.unicode() ] = w;
    }
    return w;
}

inline int KTextEditFormat::height() const
{
    return hei;
}

inline int KTextEditFormat::ascent() const
{
    return asc;
}

inline int KTextEditFormat::descent() const
{
    return dsc;
}

inline bool KTextEditFormat::operator==( const KTextEditFormat &f ) const
{
    return k == f.k;
}

inline KTextEditFormatCollection *KTextEditFormat::parent() const
{
    return collection;
}

inline void KTextEditFormat::addRef()
{
    ref++;
#ifdef DEBUG_COLLECTION
    qDebug( "add ref of '%s' to %d (%p)", k.latin1(), ref, this );
#endif
}

inline void KTextEditFormat::removeRef()
{
    ref--;
    if ( !collection )
        return;
#ifdef DEBUG_COLLECTION
    qDebug( "remove ref of '%s' to %d (%p)", k.latin1(), ref, this );
#endif
    if ( ref == 0 )
        collection->remove( this );
}

inline QString KTextEditFormat::key() const
{
    return k;
}

inline void KTextEditFormat::generateKey()
{
    QTextOStream ts( &k );
    ts << fn.pointSize()
       << fn.weight()
       << (int)fn.underline()
       << (int)fn.italic()
       << col.rgb()
       << fn.family();
}

inline QString KTextEditFormat::getKey( const QFont &fn, const QColor &col )
{
    QString k;
    QTextOStream ts( &k );
    ts << fn.pointSize()
       << fn.weight()
       << (int)fn.underline()
       << (int)fn.italic()
       << col.rgb()
       << fn.family();
    return k;
}

// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

inline KTextEditString::Char &KTextEditString::at( int i ) const
{
    return data[ i ];
}

inline QString KTextEditString::toString() const
{
    return cache;
}

inline int KTextEditString::length() const
{
    return data.size();
}

// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

inline KTextEditString::Char *KTextEditParag::at( int i ) const
{
    return &str->at( i );
}

inline int KTextEditParag::length() const
{
    return str->length();
}

inline bool KTextEditParag::isValid() const
{
    return invalid == -1;
}

inline bool KTextEditParag::hasChanged() const
{
    return changed;
}

inline void KTextEditParag::setChanged( bool b )
{
    changed = b;
}

inline void KTextEditParag::append( const QString &s )
{
    insert( str->length(), s );
}

inline QRect KTextEditParag::rect() const
{
    return r;
}

inline KTextEditParag *KTextEditParag::prev() const
{
    return p;
}

inline KTextEditParag *KTextEditParag::next() const
{
    return n;
}

inline void KTextEditParag::setSelection( int id, int start, int end )
{
    QMap<int, Selection>::ConstIterator it = selections.find( id );
    if ( it != selections.end() ) {
        if ( start == ( *it ).start && end == ( *it ).end )
            return;
    }

    Selection sel;
    sel.start = start;
    sel.end = end;
    selections[ id ] = sel;
    changed = TRUE;
}

inline void KTextEditParag::removeSelection( int id )
{
    selections.remove( id );
    changed = TRUE;
}

inline int KTextEditParag::selectionStart( int id ) const
{
    QMap<int, Selection>::ConstIterator it = selections.find( id );
    if ( it == selections.end() )
        return -1;
    return ( *it ).start;
}

inline int KTextEditParag::selectionEnd( int id ) const
{
    QMap<int, Selection>::ConstIterator it = selections.find( id );
    if ( it == selections.end() )
        return -1;
    return ( *it ).end;
}

inline bool KTextEditParag::hasSelection( int id ) const
{
    QMap<int, Selection>::ConstIterator it = selections.find( id );
    if ( it == selections.end() )
        return FALSE;
    return ( *it ).start != ( *it ).end || length() == 1;
}

inline bool KTextEditParag::hasAnySelection() const
{
    return !selections.isEmpty();
}

inline bool KTextEditParag::fullSelected( int id ) const
{
    QMap<int, Selection>::ConstIterator it = selections.find( id );
    if ( it == selections.end() )
        return FALSE;
    return ( *it ).start == 0 && ( *it ).end == str->length() - 1;
}

inline void KTextEditParag::setParagId( int i )
{
    id = i;
}

inline int KTextEditParag::paragId() const
{
    if ( id == -1 )
        kdDebug() << "invalid parag id!!!!!!!! (" << this << ")" << endl;
    return id;
}

inline QMap<int, KTextEditParag::LineStart*> &KTextEditParag::lineStartList()
{
    return lineStarts;
}

inline int KTextEditParag::lineY( int l ) const
{
    if ( l > (int)lineStarts.count() - 1 ) {
        qWarning( "KTextEditParag::lineY: line %d out of range!", l );
        return 0;
    }

    if ( !isValid() )
        ( (KTextEditParag*)this )->format();

    QMap<int, LineStart*>::ConstIterator it = lineStarts.begin();
    while ( l-- > 0 )
        ++it;
    return ( *it )->y;
}


inline int KTextEditParag::lineBaseLine( int l ) const
{
    if ( l > (int)lineStarts.count() - 1 ) {
        qWarning( "KTextEditParag::lineBaseLine: line %d out of range!", l );
        return 10;
    }

    if ( !isValid() )
        ( (KTextEditParag*)this )->format();

    QMap<int, LineStart*>::ConstIterator it = lineStarts.begin();
    while ( l-- > 0 )
        ++it;
    return ( *it )->baseLine;
}

inline int KTextEditParag::lineHeight( int l ) const
{
    if ( l > (int)lineStarts.count() - 1 ) {
        qWarning( "KTextEditParag::lineHeight: line %d out of range!", l );
        return 15;
    }

    if ( !isValid() )
        ( (KTextEditParag*)this )->format();

    QMap<int, LineStart*>::ConstIterator it = lineStarts.begin();
    while ( l-- > 0 )
        ++it;
    return ( *it )->h;
}

inline void KTextEditParag::lineInfo( int l, int &y, int &h, int &bl ) const
{
    if ( l > (int)lineStarts.count() - 1 ) {
        qWarning( "KTextEditParag::lineInfo: line %d out of range!", l );
        qDebug( "%d %d", lineStarts.count() - 1, l );
        y = 0;
        h = 15;
        bl = 10;
        return;
    }

    if ( !isValid() )
        ( (KTextEditParag*)this )->format();

    QMap<int, LineStart*>::ConstIterator it = lineStarts.begin();
    while ( l-- > 0 )
        ++it;
    y = ( *it )->y;
    h = ( *it )->h;
    bl = ( *it )->baseLine;
}

inline KTextEditString *KTextEditParag::string() const
{
    return str;
}

inline KTextEditDocument *KTextEditParag::document() const
{
    return doc;
}

inline KTextEditParag::Type KTextEditParag::type() const
{
    return typ;
}

inline void KTextEditParag::setType( Type t )
{
    if ( t != typ ) {
        invalidate( 0 );
        if ( p  && p->type() == typ )
            p->invalidate( 0 );
    }
    typ = t;
    if ( t == Normal )
        left = 0;
}

inline int KTextEditParag::leftIndent() const
{
    return left;
}

inline int KTextEditParag::listDepth() const
{
    return depth;
}

inline void KTextEditParag::setAlignment( int a )
{
    if ( a == align )
        return;
    align = a;
    invalidate( 0 );
}

inline int KTextEditParag::alignment() const
{
    return align;
}

// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

inline void KTextEditFormatCollection::setDefaultFormat( KTextEditFormat *f )
{
    defFormat = f;
}

inline KTextEditFormat *KTextEditFormatCollection::defaultFormat() const
{
    return defFormat;
}

inline KTextEditDocument *KTextEdit::document() const
{
    return doc;
}

#endif
