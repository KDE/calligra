/**
 * This file comes from Qt's internal korichtext.h file, with s/Q/Ko/ applied
 * everywhere to avoid symbol conflicts.
 * Please try to minimize the changes done to this file, and to change the #include'd
 * files instead, to make updates to this file easier.
 */

/****************************************************************************
**
** Definition of internal rich text classes
**
** Created : 990124
**
** Copyright (C) 1999-2000 Trolltech AS.  All rights reserved.
**
** This file is part of the kernel module of the Qt GUI Toolkit.
**
** This file may be distributed under the terms of the Q Public License
** as defined by Trolltech AS of Norway and appearing in the file
** LICENSE.QPL included in the packaging of this file.
**
** This file may be distributed and/or modified under the terms of the
** GNU General Public License version 2 as publish by the Free Software
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

#ifndef QRICHTEXT_P_H
#define QRICHTEXT_P_H

//
//  W A R N I N G
//  -------------
//
// This file is not part of the Qt API.  It exists for the convenience
// of a number of Qt sources files.  This header file may change from
// version to version without notice, or even be removed.
//
// We mean it.
//
//

#ifndef QT_H
#include "qstring.h"
#include "qptrlist.h"
#include "qrect.h"
#include "qfontmetrics.h"
#include "qintdict.h"
#include "qmap.h"
#include "qstringlist.h"
#include "qfont.h"
#include "qcolor.h"
#include "qsize.h"
#include "qvaluelist.h"
#include "qvaluestack.h"
#include "qobject.h"
#include "qdict.h"
#include "qtextstream.h"
#include "qpixmap.h"
#include "qstylesheet.h"
#include "qptrvector.h"
#include "qpainter.h"
#include "qlayout.h"
#include "qobject.h"
#include <limits.h>
#include "qcomplextext_p.h"
#include "qapplication.h"
#endif // QT_H

// ### TODO: make it depend on Qt version
#define INDIC

class KoTextParag;
class KoTextString;
class KoTextCursor;
class KoTextCustomItem;
class KoTextFlow;
class KoTextDocument;
//class KoTextPreProcessor;
class KoTextFormatterBase;
class KoTextIndent;
class KoTextFormat;
class KoTextFormatCollection;
struct KoBidiContext;

//// kotext additions (needed by the #included headers)
class KCommand;
class QDomElement;
class KoZoomHandler;
class KoTextFormatter;
class KoParagVisitor;
class KoTextDocCommand;
#include <qmemarray.h>
#include "koparaglayout.h"
#include "korichtext.h"
////

// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

class KoTextStringChar
{
    friend class KoTextString;

public:
    // this is never called, initialize variables in KoTextString::insert()!!!
    KoTextStringChar() : lineStart( 0 ), type( Regular ), startOfRun( 0 ) {d.format=0; }
    ~KoTextStringChar();

    QChar c;
#ifdef INDIC

    // this is the same struct as in qtextengine_p.h. Don't change!
    uchar softBreak      :1;     // Potential linebreak point
    uchar whiteSpace     :1;     // A unicode whitespace character, except NBSP, ZWNBSP
    uchar charStop       :1;     // Valid cursor position (for left/right arrow)
    uchar wordStop       :1;     // Valid cursor position (for ctrl + left/right arrow) (TODO: use)
    //uchar nobreak        :1;

#endif
    enum Type { Regular, Custom };
    uint lineStart : 1;
#ifndef INDIC
    uint rightToLeft : 1;
    //uint hasCursor : 1;
    //uint canBreak : 1;
    Type type : 2;
#else
    Type type : 1;
#endif
    uint startOfRun : 1;
#ifdef INDIC
    uint rightToLeft : 1;
#endif

    // --- added for WYSIWYG ---
    Q_INT8 pixelxadj; // adjustment to apply to lu2pixel(x)
    short int pixelwidth; // width in pixels
    short int width; // width in LU

    int x;
    int height() const;
    int ascent() const;
    int descent() const;
    bool isCustom() const { return type == Custom; }
    KoTextFormat *format() const;
    KoTextCustomItem *customItem() const;
    void setFormat( KoTextFormat *f );
    void setCustomItem( KoTextCustomItem *i );
    void loseCustomItem();
#ifndef INDIC
    KoTextStringChar *clone() const;
#endif
    struct CustomData
    {
	KoTextFormat *format;
	KoTextCustomItem *custom;
    };

    union {
	KoTextFormat* format;
	CustomData* custom;
    } d;

private:
    KoTextStringChar &operator=( const KoTextStringChar & ) {
	//abort();
	return *this;
    }
    KoTextStringChar( const KoTextStringChar & ); // copy-constructor, forbidden
    friend class KoComplexText;
    friend class KoTextParag;
};

#if defined(Q_TEMPLATEDLL)
// MOC_SKIP_BEGIN
template class QMemArray<KoTextStringChar>;
// MOC_SKIP_END
#endif

class KoTextString
{
public:

    KoTextString();
    KoTextString( const KoTextString &s );
    virtual ~KoTextString();

    QString toString() const;
    static QString toString( const QMemArray<KoTextStringChar> &data );
    QString toReverseString() const;

    KoTextStringChar &at( int i ) const;
    int length() const;

#ifndef INDIC
    //int width( int idx ) const;
#else
    //int width( int idx ) const; // moved to KoTextFormat
#endif

    void insert( int index, const QString &s, KoTextFormat *f );
    void insert( int index, KoTextStringChar *c );
    void truncate( int index );
    void remove( int index, int len );
    void clear();

    void setFormat( int index, KoTextFormat *f, bool useCollection );

    void setBidi( bool b ) { bidi = b; }
    bool isBidi() const;
    bool isRightToLeft() const;
    QChar::Direction direction() const;
    void setDirection( QChar::Direction d ) { dir = d; bidiDirty = TRUE; }

    /** Set dirty flag for background spell-checking */
    void setNeedsSpellCheck( bool b ) { bNeedsSpellCheck = b; }
    bool needsSpellCheck() const { return bNeedsSpellCheck; }

    QMemArray<KoTextStringChar> subString( int start = 0, int len = 0xFFFFFF ) const;
    QString mid( int start = 0, int len = 0xFFFFFF ) const; // kotext addition
    QMemArray<KoTextStringChar> rawData() const { return data.copy(); }

    void operator=( const QString &s ) { clear(); insert( 0, s, 0 ); }
    void operator+=( const QString &s );
    void prepend( const QString &s ) { insert( 0, s, 0 ); }

#ifdef INDIC
    // return next and previous valid cursor positions.
    bool validCursorPosition( int idx );
    int nextCursorPosition( int idx );
    int previousCursorPosition( int idx );

#endif
private:
    void checkBidi() const;

    QMemArray<KoTextStringChar> data;
    uint bidiDirty : 1;
    uint bidi : 1; // true when the paragraph has right to left characters
    uint rightToLeft : 1;
    uint dir : 5;
    uint bNeedsSpellCheck : 1;
};

inline bool KoTextString::isBidi() const
{
    if ( bidiDirty )
	checkBidi();
    return bidi;
}

inline bool KoTextString::isRightToLeft() const
{
    if ( bidiDirty )
	checkBidi();
    return rightToLeft;
}

inline QChar::Direction KoTextString::direction() const
{
    return (QChar::Direction) dir;
}

// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

#if defined(Q_TEMPLATEDLL)
// MOC_SKIP_BEGIN
template class QValueStack<int>;
template class QValueStack<KoTextParag*>;
template class QValueStack<bool>;
// MOC_SKIP_END
#endif

class KoTextCursor
{
public:
    KoTextCursor( KoTextDocument *d );
    KoTextCursor();
    KoTextCursor( const KoTextCursor &c );
    KoTextCursor &operator=( const KoTextCursor &c );
    virtual ~KoTextCursor() {}

    bool operator==( const KoTextCursor &c ) const;
    bool operator!=( const KoTextCursor &c ) const { return !(*this == c); }

    KoTextDocument *document() const { return doc; }
    void setDocument( KoTextDocument *d );

    KoTextParag *parag() const;
    int index() const;
    void setParag( KoTextParag *s, bool restore = TRUE );

    void gotoLeft();
    void gotoRight();
    void gotoNextLetter();
    void gotoPreviousLetter();
    void gotoUp();
    void gotoDown();
    void gotoLineEnd();
    void gotoLineStart();
    void gotoHome();
    void gotoEnd();
    void gotoPageUp( int visibleHeight );
    void gotoPageDown( int visibleHeight );
    void gotoNextWord();
    void gotoPreviousWord();
    void gotoWordLeft();
    void gotoWordRight();

    void insert( const QString &s, bool checkNewLine, QMemArray<KoTextStringChar> *formatting = 0 );
    void splitAndInsertEmptyParag( bool ind = TRUE, bool updateIds = TRUE );
    bool remove();
#ifdef INDIC
    bool removePreviousChar();
#endif
    void killLine();
    void indent();

    bool atParagStart() const;
    bool atParagEnd() const;

    void setIndex( int i, bool restore = TRUE );

    void checkIndex();

    int offsetX() const { return ox; }
    int offsetY() const { return oy; }

    KoTextParag *topParag() const { return parags.isEmpty() ? string : parags.first(); }
    int totalOffsetX() const;
    int totalOffsetY() const;

    bool place( const QPoint &pos, KoTextParag *s, bool link = false, int *customItemIndex = 0 );
    void restoreState();

    int x() const;
    int y() const;
#ifndef INDIC

#else
    void fixCursorPosition();
#endif
    int nestedDepth() const { return (int)indices.count(); } //### size_t/int cast

private:
    enum Operation { EnterBegin, EnterEnd, Next, Prev, Up, Down };

    void push();
    void pop();
    void processNesting( Operation op );
    void invalidateNested();
    void gotoIntoNested( const QPoint &globalPos );

    KoTextParag *string;
    KoTextDocument *doc;
    int idx, tmpIndex;
    int ox, oy;
    QValueStack<int> indices;
    QValueStack<KoTextParag*> parags;
    QValueStack<int> xOffsets;
    QValueStack<int> yOffsets;
    QValueStack<bool> nestedStack;
    bool nested;

};

// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

class KoTextDocCommand
{
public:
    enum Commands { Invalid, Insert, Delete, Format, Alignment, ParagType };

    KoTextDocCommand( KoTextDocument *d ) : doc( d ), cursor( d ) {}
    virtual ~KoTextDocCommand() {}
    virtual Commands type() const { return Invalid; };

    virtual KoTextCursor *execute( KoTextCursor *c ) = 0;
    virtual KoTextCursor *unexecute( KoTextCursor *c ) = 0;

protected:
    KoTextDocument *doc;
    KoTextCursor cursor;

};

#if defined(Q_TEMPLATEDLL)
// MOC_SKIP_BEGIN
template class QPtrList<KoTextDocCommand>;
// MOC_SKIP_END
#endif

class KoTextDocCommandHistory
{
public:
    KoTextDocCommandHistory( int s ) : current( -1 ), steps( s ) { history.setAutoDelete( TRUE ); }
    virtual ~KoTextDocCommandHistory() { clear(); }

    void clear() { history.clear(); current = -1; }

    void addCommand( KoTextDocCommand *cmd );
    KoTextCursor *undo( KoTextCursor *c );
    KoTextCursor *redo( KoTextCursor *c );

    bool isUndoAvailable();
    bool isRedoAvailable();

    void setUndoDepth( int d ) { steps = d; }
    int undoDepth() const { return steps; }

    int historySize() const { return history.count(); }
    int currentPosition() const { return current; }

private:
    QPtrList<KoTextDocCommand> history;
    int current, steps;

};

// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

class KoTextCustomItem
{
public:
    KoTextCustomItem( KoTextDocument *p );
    virtual ~KoTextCustomItem();
    virtual void draw(QPainter* p, int x, int y, int cx, int cy, int cw, int ch, const QColorGroup& cg, bool selected ) /* = 0*/;

    // Called after the item's paragraph has been formatted
    virtual void finalize() {}

    void move( int x, int y ) { xpos = x; ypos = y; }
    int x() const { return xpos; }
    int y() const { return ypos; }

    // Called when the format of the character is being changed, see KoTextStringChar::setFormat
    virtual void setFormat( KoTextFormat * ) { }

    //virtual void setPainter( QPainter*, bool adjust );

    enum Placement { PlaceInline = 0, PlaceLeft, PlaceRight };
    virtual Placement placement() const { return PlaceInline; }
    bool placeInline() { return placement() == PlaceInline; }

    virtual bool ownLine() const { return FALSE; }
    // Called for "ownline" items
    virtual void resize( int nwidth ) { width = nwidth; }
    virtual void invalidate() {};

    virtual bool isNested() const { return FALSE; }
    virtual int minimumWidth() const { return 0; }
    virtual int widthHint() const { return 0; }
    virtual int ascent() const { return height; }

    virtual QString richText() const { return QString::null; }

    int width;
    int height;

    QRect geometry() const { return QRect( xpos, ypos, width, height ); }

    virtual bool enter( KoTextCursor *, KoTextDocument *&doc, KoTextParag *&parag, int &idx, int &ox, int &oy, bool atEnd = FALSE );
    virtual bool enterAt( KoTextCursor *, KoTextDocument *&doc, KoTextParag *&parag, int &idx, int &ox, int &oy, const QPoint & );
    virtual bool next( KoTextCursor *, KoTextDocument *&doc, KoTextParag *&parag, int &idx, int &ox, int &oy );
    virtual bool prev( KoTextCursor *, KoTextDocument *&doc, KoTextParag *&parag, int &idx, int &ox, int &oy );
    virtual bool down( KoTextCursor *, KoTextDocument *&doc, KoTextParag *&parag, int &idx, int &ox, int &oy );
    virtual bool up( KoTextCursor *, KoTextDocument *&doc, KoTextParag *&parag, int &idx, int &ox, int &oy );

    void setParagraph( KoTextParag * p ) { parag = p; }
    KoTextParag *paragraph() const { return parag; }

    virtual void pageBreak( int /*y*/, KoTextFlow* /*flow*/ ) {}

    KoTextDocument *parent;

#include "kotextcustomitem.h"

protected:
    int xpos;
    int ypos;
private:
    KoTextParag *parag;
};

#if defined(Q_TEMPLATEDLL)
// MOC_SKIP_BEGIN
template class QMap<QString, QString>;
// MOC_SKIP_END
#endif

#if 0
class KoTextImage : public KoTextCustomItem
{
public:
    KoTextImage( KoTextDocument *p, const QMap<QString, QString> &attr, const QString& context,
		QMimeSourceFactory &factory );
    virtual ~KoTextImage();

    Placement placement() const { return place; }
    //void setPainter( QPainter*, bool );
    int widthHint() const { return width; }
    int minimumWidth() const { return width; }

    QString richText() const;

    void draw( QPainter* p, int x, int y, int cx, int cy, int cw, int ch, const QColorGroup& cg, bool selected );

private:
    QRegion* reg;
    QPixmap pm;
    Placement place;
    int tmpwidth, tmpheight;
    QMap<QString, QString> attributes;
    QString imgId;

};
#endif

class KoTextHorizontalLine : public KoTextCustomItem
{
public:
    KoTextHorizontalLine( KoTextDocument *p, const QMap<QString, QString> &attr, const QString& context,
			 QMimeSourceFactory &factory );
    virtual ~KoTextHorizontalLine();

    //void setPainter( QPainter*, bool );
    void draw(QPainter* p, int x, int y, int cx, int cy, int cw, int ch, const QColorGroup& cg, bool selected );
    QString richText() const;

    bool ownLine() const { return TRUE; }

private:
    int tmpheight;
    QColor color;

};

#if defined(Q_TEMPLATEDLL)
// MOC_SKIP_BEGIN
template class QPtrList<KoTextCustomItem>;
// MOC_SKIP_END
#endif

class KoTextFlow
{
    friend class KoTextDocument;
    friend class KoTextTableCell;

public:
    KoTextFlow();
    virtual ~KoTextFlow();

    virtual void setWidth( int width );
    int width() const { return w; }

    //virtual void setPageSize( int ps );
    //int pageSize() const { return pagesize; }

    /**
     * Called by the formatter to find out the left and right margin for a paragraph at ( yp, yp+h )
     * @param leftMargin returns the left margin
     * @param rightMargin returns the right margin (from the page width)
     * @param pageWidth returns the page width at that point
     * This method merges QRichText's adjustLMargin and adjustRMargin for efficiency reasons
     */
    virtual void adjustMargins( int yp, int h, int reqMinWidth, int& leftMargin, int& rightMargin, int& pageWidth, KoTextParag* parag );

    virtual void registerFloatingItem( KoTextCustomItem* item );
    virtual void unregisterFloatingItem( KoTextCustomItem* item );
    //virtual QRect boundingRect() const;

    /// kotext addition. Allows the textformatter to stop when it goes too far.
    virtual int availableHeight() const;
    virtual void drawFloatingItems(QPainter* p, int cx, int cy, int cw, int ch, const QColorGroup& cg, bool selected );

    virtual int adjustFlow( int y, int w, int h ); // adjusts y according to the defined pagesize. Returns the shift.

    virtual bool isEmpty();

    void clear();

private:
    int w;
    //int pagesize;

    QPtrList<KoTextCustomItem> leftItems;
    QPtrList<KoTextCustomItem> rightItems;

};

#ifdef QTEXTTABLE_AVAILABLE
class KoTextTable;

class KoTextTableCell : public QLayoutItem
{
    friend class KoTextTable;

public:
    KoTextTableCell( KoTextTable* table,
		    int row, int column,
		    const QMap<QString, QString> &attr,
		    const QStyleSheetItem* style,
		    const KoTextFormat& fmt, const QString& context,
		    QMimeSourceFactory &factory, QStyleSheet *sheet, const QString& doc );
    KoTextTableCell( KoTextTable* table, int row, int column );
    virtual ~KoTextTableCell();

    QSize sizeHint() const ;
    QSize minimumSize() const ;
    QSize maximumSize() const ;
    QSizePolicy::ExpandData expanding() const;
    bool isEmpty() const;
    void setGeometry( const QRect& ) ;
    QRect geometry() const;

    bool hasHeightForWidth() const;
    int heightForWidth( int ) const;

    void setPainter( QPainter*, bool );

    int row() const { return row_; }
    int column() const { return col_; }
    int rowspan() const { return rowspan_; }
    int colspan() const { return colspan_; }
    int stretch() const { return stretch_; }

    KoTextDocument* richText()  const { return richtext; }
    KoTextTable* table() const { return parent; }

    void draw( int x, int y, int cx, int cy, int cw, int ch, const QColorGroup& cg, bool selected );

    QBrush *backGround() const { return background; }
    virtual void invalidate();

    int verticalAlignmentOffset() const;
    int horizontalAlignmentOffset() const;

private:
    QPainter* painter() const;
    QRect geom;
    KoTextTable* parent;
    KoTextDocument* richtext;
    int row_;
    int col_;
    int rowspan_;
    int colspan_;
    int stretch_;
    int maxw;
    int minw;
    bool hasFixedWidth;
    QBrush *background;
    int cached_width;
    int cached_sizehint;
    QMap<QString, QString> attributes;
    int align;
};

#if defined(Q_TEMPLATEDLL)
// MOC_SKIP_BEGIN
template class QPtrList<KoTextTableCell>;
template class QMap<KoTextCursor*, int>;
// MOC_SKIP_END
#endif

class KoTextTable: public KoTextCustomItem
{
    friend class KoTextTableCell;

public:
    KoTextTable( KoTextDocument *p, const QMap<QString, QString> &attr );
    virtual ~KoTextTable();

    void setPainter( QPainter *p, bool adjust );
    void pageBreak( int  y, KoTextFlow* flow );
    void draw( QPainter* p, int x, int y, int cx, int cy, int cw, int ch,
	       const QColorGroup& cg, bool selected );

    bool noErase() const { return TRUE; }
    bool ownLine() const { return TRUE; }
    Placement placement() const { return place; }
    bool isNested() const { return TRUE; }
    void resize( int nwidth );
    virtual void invalidate();
    /// ## QString anchorAt( QPainter* p, int x, int y );

    virtual bool enter( KoTextCursor *c, KoTextDocument *&doc, KoTextParag *&parag, int &idx, int &ox, int &oy, bool atEnd = FALSE );
    virtual bool enterAt( KoTextCursor *c, KoTextDocument *&doc, KoTextParag *&parag, int &idx, int &ox, int &oy, const QPoint &pos );
    virtual bool next( KoTextCursor *c, KoTextDocument *&doc, KoTextParag *&parag, int &idx, int &ox, int &oy );
    virtual bool prev( KoTextCursor *c, KoTextDocument *&doc, KoTextParag *&parag, int &idx, int &ox, int &oy );
    virtual bool down( KoTextCursor *c, KoTextDocument *&doc, KoTextParag *&parag, int &idx, int &ox, int &oy );
    virtual bool up( KoTextCursor *c, KoTextDocument *&doc, KoTextParag *&parag, int &idx, int &ox, int &oy );

    QString richText() const;

    int minimumWidth() const { return layout ? layout->minimumSize().width() : 0; }
    int widthHint() const { return ( layout ? layout->sizeHint().width() : 0 ) + 2 * outerborder; }

    QPtrList<KoTextTableCell> tableCells() const { return cells; }

    QRect geometry() const { return layout ? layout->geometry() : QRect(); }
    bool isStretching() const { return stretch; }

private:
    void format( int &w );
    void addCell( KoTextTableCell* cell );

private:
    QGridLayout* layout;
    QPtrList<KoTextTableCell> cells;
    QPainter* painter;
    int cachewidth;
    int fixwidth;
    int cellpadding;
    int cellspacing;
    int border;
    int outerborder;
    int stretch;
    int innerborder;
    int us_cp, us_ib, us_b, us_ob, us_cs;
    QMap<QString, QString> attributes;
    QMap<KoTextCursor*, int> currCell;
    Placement place;
    void adjustCells( int y , int shift );
    int pageBreakFor;
};
#endif // QTEXTTABLE_AVAILABLE

// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

class KoTextTableCell;

struct KoTextDocumentSelection
{
    KoTextCursor startCursor, endCursor;
    bool swapped;
};

// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++


class KoTextDocDeleteCommand : public KoTextDocCommand
{
public:
    KoTextDocDeleteCommand( KoTextDocument *d, int i, int idx, const QMemArray<KoTextStringChar> &str );
    //KoTextDocDeleteCommand( KoTextParag *p, int idx, const QMemArray<KoTextStringChar> &str );
    virtual ~KoTextDocDeleteCommand();

    Commands type() const { return Delete; }
    KoTextCursor *execute( KoTextCursor *c );
    KoTextCursor *unexecute( KoTextCursor *c );

protected:
    int id, index;
    KoTextParag *parag;
    QMemArray<KoTextStringChar> text;

};

#if 0
class KoTextDocInsertCommand : public KoTextDocDeleteCommand
{
public:
    KoTextDocInsertCommand( KoTextDocument *d, int i, int idx, const QMemArray<KoTextStringChar> &str )
	: KoTextDocDeleteCommand( d, i, idx, str ) {}
    KoTextDocInsertCommand( KoTextParag *p, int idx, const QMemArray<KoTextStringChar> &str )
	: KoTextDocDeleteCommand( p, idx, str ) {}
    virtual ~KoTextDocInsertCommand() {}

    Commands type() const { return Insert; }
    KoTextCursor *execute( KoTextCursor *c ) { return KoTextDocDeleteCommand::unexecute( c ); }
    KoTextCursor *unexecute( KoTextCursor *c ) { return KoTextDocDeleteCommand::execute( c ); }

};
#endif

class KoTextDocFormatCommand : public KoTextDocCommand
{
public:
    KoTextDocFormatCommand( KoTextDocument *d, int sid, int sidx, int eid, int eidx, const QMemArray<KoTextStringChar> &old, const KoTextFormat *f, int fl );
    virtual ~KoTextDocFormatCommand();

    Commands type() const { return Format; }
    KoTextCursor *execute( KoTextCursor *c );
    KoTextCursor *unexecute( KoTextCursor *c );

protected:
    int startId, startIndex, endId, endIndex;
    KoTextFormat *format;
    QMemArray<KoTextStringChar> oldFormats;
    int flags;

};

class KoTextAlignmentCommand : public KoTextDocCommand
{
public:
    KoTextAlignmentCommand( KoTextDocument *d, int fParag, int lParag, int na, const QMemArray<int> &oa );
    virtual ~KoTextAlignmentCommand() {}

    Commands type() const { return Alignment; }
    KoTextCursor *execute( KoTextCursor *c );
    KoTextCursor *unexecute( KoTextCursor *c );

private:
    int firstParag, lastParag;
    int newAlign;
    QMemArray<int> oldAligns;

};

// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

struct KoTextParagSelection
{
    int start, end;
};

struct KoTextParagLineStart
{
    KoTextParagLineStart() : y( 0 ), baseLine( 0 ), h( 0 ), lineSpacing( 0 ), hyphenated( false )
#ifndef QT_NO_COMPLEXTEXT
	, bidicontext( 0 )
#endif
    {  }
    KoTextParagLineStart( ushort y_, ushort bl, ushort h_ ) : y( y_ ), baseLine( bl ), h( h_ ),
                                                              lineSpacing( 0 ), hyphenated( false ),
	w( 0 )
#ifndef QT_NO_COMPLEXTEXT
	, bidicontext( 0 )
#endif
    {  }
#ifndef QT_NO_COMPLEXTEXT
    KoTextParagLineStart( KoBidiContext *c, KoBidiStatus s ) : y(0), baseLine(0), h(0),
                                                               lineSpacing( 0 ), hyphenated( false ),
	status( s ), bidicontext( c ) { if ( bidicontext ) bidicontext->ref(); }
#endif

    virtual ~KoTextParagLineStart()
    {
#ifndef QT_NO_COMPLEXTEXT
	if ( bidicontext && bidicontext->deref() )
	    delete bidicontext;
#endif
    }

#ifndef QT_NO_COMPLEXTEXT
    void setContext( KoBidiContext *c ) {
	if ( c == bidicontext )
	    return;
	if ( bidicontext && bidicontext->deref() )
	    delete bidicontext;
	bidicontext = c;
	if ( bidicontext )
	    bidicontext->ref();
    }
    KoBidiContext *context() const { return bidicontext; }
#endif

public:
    ushort y, baseLine, h;
    short lineSpacing;
    bool hyphenated;
#ifndef QT_NO_COMPLEXTEXT
    KoBidiStatus status;
#endif
    int w;

private:
#ifndef QT_NO_COMPLEXTEXT
    KoBidiContext *bidicontext;
#endif
};

#if defined(Q_TEMPLATEDLL)
// MOC_SKIP_BEGIN
template class QMap<int, KoTextParagSelection>;
template class QMap<int, KoTextParagLineStart*>;
// MOC_SKIP_END
#endif

/*class KoTextParagData
{
public:
    KoTextParagData() {}
    virtual ~KoTextParagData() {}
    virtual void join( KoTextParagData * ) {}
};*/

class KoTextParag
{
    friend class KoTextDocument;
    friend class KoTextCursor;

public:
    KoTextParag( KoTextDocument *d, KoTextParag *pr = 0, KoTextParag *nx = 0, bool updateIds = TRUE );
    virtual ~KoTextParag();

    KoTextString *string() const;
    KoTextStringChar *at( int i ) const; // maybe remove later
    int leftGap() const;
    int length() const; // maybe remove later

    void setFormat( KoTextFormat *fm );
    KoTextFormat *paragFormat() const;

    KoTextDocument *document() const;

    QRect rect() const;
    void setRect( const QRect& rect ) { r = rect; }
    void setHeight( int h ) { r.setHeight( h ); }
    void setWidth( int w ) { r.setWidth( w ); }
    void show();
    void hide();
    bool isVisible() const { return visible; }

    //bool isLastInFrame() const { return lastInFrame; }
    //void setLastInFrame( bool b ) { lastInFrame = b; }

    KoTextParag *prev() const;
    KoTextParag *next() const;
    void setPrev( KoTextParag *s );
    void setNext( KoTextParag *s );

    void insert( int index, const QString &s );
    void append( const QString &s, bool reallyAtEnd = FALSE );
    void truncate( int index );
    void remove( int index, int len );

    void invalidate( int chr );

    void move( int &dy );
    void format( int start = -1, bool doMove = TRUE );

    bool isValid() const;
    bool hasChanged() const;
    void setChanged( bool b, bool recursive = FALSE );
    short int lineChanged(); // first line that has been changed.
    void setLineChanged( short int line );

    int lineHeightOfChar( int i, int *bl = 0, int *y = 0 ) const;
    KoTextStringChar *lineStartOfChar( int i, int *index = 0, int *line = 0 ) const;
    int lines() const;
    KoTextStringChar *lineStartOfLine( int line, int *index = 0 ) const;
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

    //void setEndState( int s );
    //int endState() const;

    void setParagId( int i );
    int paragId() const;

    //bool firstPreProcess() const;
    //void setFirstPreProcess( bool b );

    void indent( int *oldIndent = 0, int *newIndent = 0 );

    //void setExtraData( KoTextParagData *data );
    //KoTextParagData *extraData() const;

    QMap<int, KoTextParagLineStart*> &lineStartList();

    void setFormat( int index, int len, const KoTextFormat *f, bool useCollection = TRUE, int flags = -1 );

    void setAlignment( uint a );
    void setAlignmentDirect( uint a ) { align = a; }
    uint alignment() const;

    virtual void paint( QPainter &painter, const QColorGroup &cg, KoTextCursor *cursor, bool drawSelections,
                       int clipx, int clipy, int clipw, int cliph ); // kotextparag.cc


    int topMargin() const;
    int bottomMargin() const;
    int leftMargin() const;
    int firstLineMargin() const;
    int rightMargin() const;
    int lineSpacing( int line ) const;

    int numberOfSubParagraph() const;
    void registerFloatingItem( KoTextCustomItem *i );
    void unregisterFloatingItem( KoTextCustomItem *i );

    void setFullWidth( bool b ) { fullWidth = b; }
    bool isFullWidth() const { return fullWidth; }

#ifdef QTEXTTABLE_AVAILABLE
    KoTextTableCell *tableCell() const { return tc; }
    void setTableCell( KoTextTableCell *c ) { tc = c; }
#endif

    //void addCustomItem();
    //void removeCustomItem();
    int customItems() const;

    QBrush *background() const;

    void setDocumentRect( const QRect &r );
    int documentWidth() const;
    //int documentVisibleWidth() const;
    int documentX() const;
    int documentY() const;
    KoTextFormatCollection *formatCollection() const;
    //void setFormatter( KoTextFormatterBase *f );
    KoTextFormatterBase *formatter() const;
    //int minimumWidth() const;
    int widthUsed() const;

    int nextTabDefault( int i, int x );
    int nextTab( int i, int x ); // kotextparag.cc
    int *tabArray() const;
    void setTabArray( int *a );
    void setTabStops( int tw );

    //void setPainter( QPainter *p, bool adjust  );
    //QPainter *painter() const { return pntr; }

    void setNewLinesAllowed( bool b );
    bool isNewLinesAllowed() const;

    QString richText() const;

    //void addCommand( KoTextDocCommand *cmd );
    //KoTextCursor *undo( KoTextCursor *c = 0 );
    //KoTextCursor *redo( KoTextCursor *c  = 0 );
    //KoTextDocCommandHistory *commands() const { return commandHistory; }

    virtual void join( KoTextParag *s );
    virtual void copyParagData( KoTextParag *parag );

    void setBreakable( bool b ) { breakable = b; }
    bool isBreakable() const { return breakable; }

    //void setBackgroundColor( const QColor &c );
    //QColor *backgroundColor() const { return bgcol; }
    //void clearBackgroundColor();

    //bool isLineBreak() const { return isBr; }

    void setMovedDown( bool b ) { movedDown = b; }
    bool wasMovedDown() const { return movedDown; }

    void setDirection( QChar::Direction d );
    QChar::Direction direction() const;

    // For KoTextFormatter only
    void insertLineStart( int index, KoTextParagLineStart *ls );

protected:
    void drawLabel( QPainter* p, int x, int y, int w, int h, int base, const QColorGroup& cg );
    void drawCursorDefault( QPainter &painter, KoTextCursor *cursor, int curx, int cury, int curh, const QColorGroup &cg );
    void drawCursor( QPainter &painter, KoTextCursor *cursor, int curx, int cury, int curh, const QColorGroup &cg );

#include "kotextparag.h"

private:
    QMap<int, KoTextParagSelection> &selections() const;
    QPtrVector<QStyleSheetItem> &styleSheetItemsVec() const;
    QPtrList<KoTextCustomItem> &floatingItems() const;

    QMap<int, KoTextParagLineStart*> lineStarts;
    int invalid;
    QRect r;
    KoTextParag *p, *n;
    KoTextDocument *doc;
    uint changed : 1;
    //uint firstFormat : 1; /// unused
    //uint firstPProcess : 1;
    //uint needPreProcess : 1;
    uint fullWidth : 1;
    uint newLinesAllowed : 1;
    //uint lastInFrame : 1;
    uint visible : 1;
    uint breakable : 1;
    //uint isBr : 1;
    uint movedDown : 1;
    uint align : 4;
    short int m_lineChanged;
    int id;
    int m_wused;
    KoTextString *str;
    QMap<int, KoTextParagSelection> *mSelections;
    QPtrList<KoTextCustomItem> *mFloatingItems;
    //int tm, bm, lm, rm, flm; // margins
    KoTextFormat *defFormat; // is this really used?
    //int numCustomItems;
    int *tArray;
    //KoTextParagData *eData;
    //QColor *bgcol;
    //QPainter *pntr;

    // Those things are used by QRT for the case of a paragraph without document
    // We don't use this currently, and it's not worth making EVERY parag bigger
    // just for a special case that's rarely used. Better have lightweight KoTextDocument
    // replacement (with common base class), if we ever want efficient single-parag docs...
    //int tabStopWidth;
    //QRect docRect;
    //KoTextFormatterBase *pFormatter;
    //KoTextDocCommandHistory *commandHistory;
};

// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

class KoTextFormatterBase
{
public:
    KoTextFormatterBase();
    virtual ~KoTextFormatterBase() {}
    virtual bool format( KoTextDocument *doc, KoTextParag *parag, int start, const QMap<int, KoTextParagLineStart*> &oldLineStarts, int& y, int& widthUsed ) = 0;
    virtual int formatVertically( KoTextDocument* doc, KoTextParag* parag );

    // Called after formatting a paragraph
    virtual void postFormat( KoTextParag* parag ) = 0;

    bool isWrapEnabled( KoTextParag *p ) const { if ( !wrapEnabled ) return FALSE; if ( p && !p->isBreakable() ) return FALSE; return TRUE;}
    int wrapAtColumn() const { return wrapColumn;}
    virtual void setWrapEnabled( bool b ) { wrapEnabled = b; }
    virtual void setWrapAtColumn( int c ) { wrapColumn = c; }
    virtual void setAllowBreakInWords( bool b ) { biw = b; }
    bool allowBreakInWords() const { return biw; }

    // This setting is passed to KoTextParag::fixParagWidth by postFormat()
    void setViewFormattingChars( bool b ) { m_bViewFormattingChars = b; }
    bool viewFormattingChars() const { return m_bViewFormattingChars; }

    /*virtual*/ bool isBreakable( KoTextString *string, int pos ) const;
    /*virtual*/ bool isStretchable( KoTextString *string, int pos ) const;

protected:
    //virtual KoTextParagLineStart *formatLine( KoTextParag *parag, KoTextString *string, KoTextParagLineStart *line, KoTextStringChar *start,
    //					       KoTextStringChar *last, int align = AlignAuto, int space = 0 );
    //KoTextStringChar

#ifndef QT_NO_COMPLEXTEXT
    virtual KoTextParagLineStart *bidiReorderLine( KoTextParag *parag, KoTextString *string, KoTextParagLineStart *line, KoTextStringChar *start,
						    KoTextStringChar *last, int align, int space );
#endif

private:
    int wrapColumn;
    bool wrapEnabled;
    bool m_bViewFormattingChars;
    bool biw;
    bool unused; // for future extensions

#ifdef HAVE_THAI_BREAKS
    static QCString *thaiCache;
    static KoTextString *cachedString;
    static ThBreakIterator *thaiIt;
#endif
};

// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

// Not used
class KoTextIndent
{
public:
    KoTextIndent();
    virtual ~KoTextIndent() {}

    virtual void indent( KoTextDocument *doc, KoTextParag *parag, int *oldIndent = 0, int *newIndent = 0 ) = 0;

};

// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

#if 0
class KoTextPreProcessor
{
public:
    enum Ids {
	Standard = 0
    };

    KoTextPreProcessor() {}
    virtual ~KoTextPreProcessor() {}

    virtual void process( KoTextDocument *doc, KoTextParag *, int, bool = TRUE ) = 0;
    virtual KoTextFormat *format( int id ) = 0;

};
#endif

// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

inline int KoTextString::length() const
{
    return data.size();
}

inline void KoTextString::operator+=( const QString &s )
{
    insert( length(), s, 0 );
}

inline int KoTextParag::length() const
{
    return str->length();
}

inline QRect KoTextParag::rect() const
{
    return r;
}

inline KoTextParag *KoTextCursor::parag() const
{
    return string;
}

inline int KoTextCursor::index() const
{
    return idx;
}

inline void KoTextCursor::setParag( KoTextParag *s, bool restore )
{
    if ( restore )
	restoreState();
    idx = 0;
    string = s;
    tmpIndex = -1;
}

inline void KoTextCursor::checkIndex()
{
    if ( idx >= string->length() )
	idx = string->length() - 1;
}

// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

inline KoTextStringChar &KoTextString::at( int i ) const
{
    return data[ i ];
}

inline QString KoTextString::toString() const
{
    return toString( data );
}

// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

inline KoTextStringChar *KoTextParag::at( int i ) const
{
    return &str->at( i );
}

inline bool KoTextParag::isValid() const
{
    return invalid == -1;
}

inline bool KoTextParag::hasChanged() const
{
    return changed;
}

inline short int KoTextParag::lineChanged()
{
    return m_lineChanged;
}

/*inline void KoTextParag::setBackgroundColor( const QColor & c )
{
    delete bgcol;
    bgcol = new QColor( c );
    setChanged( TRUE );
}

inline void KoTextParag::clearBackgroundColor()
{
    delete bgcol; bgcol = 0; setChanged( TRUE );
}
*/

inline void KoTextParag::append( const QString &s, bool reallyAtEnd )
{
    if ( reallyAtEnd )
	insert( str->length(), s );
    else
	insert( QMAX( str->length() - 1, 0 ), s );
}

inline KoTextParag *KoTextParag::prev() const
{
    return p;
}

inline KoTextParag *KoTextParag::next() const
{
    return n;
}

inline bool KoTextParag::hasAnySelection() const
{
    return mSelections ? !selections().isEmpty() : FALSE;
}

/*inline void KoTextParag::setEndState( int s )
{
    if ( s == state )
	return;
    state = s;
}

inline int KoTextParag::endState() const
{
    return state;
}*/

inline void KoTextParag::setParagId( int i )
{
    id = i;
}

inline int KoTextParag::paragId() const
{
    //if ( id == -1 )
    //	kdWarning() << "invalid parag id!!!!!!!! (" << (void*)this << ")" << endl;
    return id;
}

/*inline bool KoTextParag::firstPreProcess() const
{
    return firstPProcess;
}

inline void KoTextParag::setFirstPreProcess( bool b )
{
    firstPProcess = b;
}*/

inline QMap<int, KoTextParagLineStart*> &KoTextParag::lineStartList()
{
    return lineStarts;
}

inline KoTextString *KoTextParag::string() const
{
    return str;
}

inline KoTextDocument *KoTextParag::document() const
{
    return doc;
}

inline void KoTextParag::setAlignment( uint a )
{
    if ( a == align )
	return;
    align = a;
    invalidate( 0 );
}

/*inline void KoTextParag::setListStyle( QStyleSheetItem::ListStyle ls )
{
    lstyle = ls;
    invalidate( 0 );
}

inline QStyleSheetItem::ListStyle KoTextParag::listStyle() const
{
    return lstyle;
}*/

inline KoTextFormat *KoTextParag::paragFormat() const
{
    return defFormat;
}

inline void KoTextParag::registerFloatingItem( KoTextCustomItem *i )
{
    floatingItems().append( i );
}

inline void KoTextParag::unregisterFloatingItem( KoTextCustomItem *i )
{
    floatingItems().removeRef( i );
}

/*inline void KoTextParag::addCustomItem()
{
    numCustomItems++;
}

inline void KoTextParag::removeCustomItem()
{
    numCustomItems--;
}*/

inline int KoTextParag::customItems() const
{
    return mFloatingItems ? mFloatingItems->count() : 0;
    // was numCustomItems, but no need for a separate count
}

inline QBrush *KoTextParag::background() const
{
#ifdef QTEXTTABLE_AVAILABLE
    return tc ? tc->backGround() : 0;
#endif
    return 0;
}


//inline void KoTextParag::setDocumentRect( const QRect &r )
//{
//    docRect = r;
//}

/*inline void KoTextParag::setExtraData( KoTextParagData *data )
{
    eData = data;
}

inline KoTextParagData *KoTextParag::extraData() const
{
    return eData;
}*/

inline void KoTextParag::setNewLinesAllowed( bool b )
{
    newLinesAllowed = b;
}

inline bool KoTextParag::isNewLinesAllowed() const
{
    return newLinesAllowed;
}

// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

inline KoTextFormat *KoTextStringChar::format() const
{
    return (type == Regular) ? d.format : d.custom->format;
}

inline KoTextCustomItem *KoTextStringChar::customItem() const
{
    return isCustom() ? d.custom->custom : 0;
}

#endif
