/* This file is part of the KDE project
   Copyright (C) 1998, 1999 Reginald Stadlbauer <reggie@kde.org>

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

#ifndef frame_h
#define frame_h

#include <qrect.h>
#include <qpoint.h>
#include <qsize.h>
#include <qarray.h>
#include <qlist.h>
#include <qcursor.h>
#include <qregion.h>
#include <qpainter.h>
#include <qpicture.h>

#include "paraglayout.h"
#include "defs.h"

#include <qtextstream.h>
#include <koStream.h>

#include <koMainWindow.h>

class KWFrame;
class KWordDocument;
class KWordChild;
class KWordFrame;
class KWGroupManager;
class KFormulaEdit;
class KWFormat;
class KWResizeHandle;
class KWPage;

enum FrameType { FT_BASE = 0, FT_TEXT = 1, FT_PICTURE = 2, FT_PART = 3, FT_FORMULA = 4 };
enum FrameInfo { FI_BODY = 0, FI_FIRST_HEADER = 1, FI_ODD_HEADER = 2, FI_EVEN_HEADER = 3,
                 FI_FIRST_FOOTER = 4, FI_ODD_FOOTER = 5, FI_EVEN_FOOTER = 6,
                 FI_FOOTNOTE = 7 };
enum RunAround { RA_NO = 0, RA_BOUNDINGRECT = 1, RA_SKIP = 2 };
enum FrameBehaviour { AutoExtendFrame=0 , AutoCreateNewFrame=1, Ignore=2 };
enum NewFrameBehaviour { Reconnect=0, NoFollowup=1, Copy=2 };
enum SheetSide { AnySide=0, OddSide=1, EvenSide=2};

const int minColWidth=18;

/******************************************************************/
/* Class: KWFrame                                                 */
/******************************************************************/

class KWFrame : public QRect
{
    friend class KWPage;

public:
    KWFrame();
    KWFrame(KWFrameSet *fs, const QPoint &topleft, const QPoint &bottomright );
    KWFrame(KWFrameSet *fs, const QPoint &topleft, const QSize &size );
    KWFrame(KWFrameSet *fs, int left, int top, int width, int height );
    KWFrame(KWFrameSet *fs, int left, int top, int width, int height, RunAround _ra, KWUnit _gap );
    KWFrame(KWFrameSet *fs, const QRect &_rect );
    virtual ~KWFrame();

    void setRunAround( RunAround _ra ) { runAround = _ra; }
    RunAround getRunAround() { return runAround; }

    void setSelected( bool _selected ) { selected = _selected; }
    bool isSelected() { return selected; }

    void addIntersect( QRect &_r );
    void clearIntersects()
    { intersections.clear(); emptyRegionDirty = TRUE; }

    int getLeftIndent( int _y, int _h );
    int getRightIndent( int _y, int _h );

    bool hasIntersections() { return !intersections.isEmpty(); }

    QCursor getMouseCursor( int mx, int my, bool table );

    KWUnit getRunAroundGap() { return runAroundGap; }
    void setRunAroundGap( KWUnit gap ) { runAroundGap = gap; }
    SheetSide getSheetSide() { return sheetSide; }
    void setSheetSide( SheetSide ss ) { sheetSide = ss; }
    FrameBehaviour getFrameBehaviour() { return frameBehaviour; }
    void setFrameBehaviour( FrameBehaviour fb ) { frameBehaviour = fb; }
    NewFrameBehaviour getNewFrameBehaviour() { return newFrameBehaviour; }
    void setNewFrameBehaviour( NewFrameBehaviour nfb ) { newFrameBehaviour = nfb; }
    FrameInfo getFrameInfo();
    FrameType getFrameType();
    KWFrameSet *getFrameSet() { return frameSet; }
    void setFrameSet( KWFrameSet *fs ) { frameSet = fs; }

    bool isMostRight() { return mostRight; }
    void setMostRight( bool _mr ) { mostRight = _mr; }

    void setPageNum( int i ) { pageNum = i; }
    int getPageNum() { return pageNum; }

    KWParagLayout::Border &getLeftBorder() { return brd_left; }
    KWParagLayout::Border &getRightBorder() { return brd_right; }
    KWParagLayout::Border &getTopBorder() { return brd_top; }
    KWParagLayout::Border &getBottomBorder() { return brd_bottom; }
    KWParagLayout::Border getLeftBorder2() { return brd_left; }
    KWParagLayout::Border getRightBorder2() { return brd_right; }
    KWParagLayout::Border getTopBorder2() { return brd_top; }
    KWParagLayout::Border getBottomBorder2() { return brd_bottom; }
    void setLeftBorder( KWParagLayout::Border _brd ) { brd_left = _brd; }
    void setRightBorder( KWParagLayout::Border _brd ) { brd_right = _brd; }
    void setTopBorder( KWParagLayout::Border _brd ) { brd_top = _brd; }
    void setBottomBorder( KWParagLayout::Border _brd ) { brd_bottom = _brd; }

    QRegion getEmptyRegion( bool useCached = TRUE );

    QBrush getBackgroundColor() { return backgroundColor; }
    void setBackgroundColor( QBrush _color ) { backgroundColor = _color; }

    unsigned int getNextFreeYPos( unsigned int _y, unsigned int _h );

    void setBLeft( KWUnit b ) { bleft = b; }
    void setBRight( KWUnit b ) { bright = b; }
    void setBTop( KWUnit b ) { btop = b; }
    void setBBottom( KWUnit b ) { bbottom = b; }

    KWUnit getBLeft() { return bleft; }
    KWUnit getBRight() { return bright; }
    KWUnit getBTop() { return btop; }
    KWUnit getBBottom() { return bbottom; }

    KWFrame *getCopy();

protected:
    SheetSide sheetSide;
    RunAround runAround;
    FrameBehaviour frameBehaviour;
    NewFrameBehaviour newFrameBehaviour;
    bool selected;
    KWUnit runAroundGap;
    bool mostRight;
    int pageNum;

    QList<QRect> intersections;
    QRegion emptyRegion;
    bool emptyRegionDirty;

    KWParagLayout::Border brd_left, brd_right, brd_top, brd_bottom;
    QBrush backgroundColor;

    KWUnit bleft, bright, btop, bbottom;
    QArray<KWResizeHandle*> handles;

private:
    KWFrame &operator=( const KWFrame &_frame );
    KWFrame ( const KWFrame &_frame );
    KWFrameSet *frameSet;

};

/******************************************************************/
/* Class: KWFrameSet                                              */
/******************************************************************/

class KWFrameSet
{
public:
    KWFrameSet( KWordDocument *_doc );
    virtual ~KWFrameSet()
    {; }

    virtual FrameType getFrameType() { return FT_BASE; }
    virtual FrameInfo getFrameInfo() { return frameInfo; }
    void setFrameInfo( FrameInfo fi ) { frameInfo = fi; }

    virtual void addFrame( KWFrame _frame );
    virtual void addFrame( KWFrame *_frame );
    virtual void delFrame( unsigned int _num );
    virtual void delFrame( KWFrame *frm, bool remove = TRUE );

    virtual int getFrame( int _x, int _y );
    virtual KWFrame *getFrame( unsigned int _num );
    virtual int getFrameFromPtr( KWFrame *frame );
    virtual unsigned int getNumFrames()
    { return frames.count(); }

    virtual bool isPTYInFrame( unsigned int /*_frame*/, unsigned int /*_ypos */ )
        { return true; }

    virtual void update()
    {; }

    virtual void clear()
    { frames.clear(); }

    virtual bool contains( unsigned int mx, unsigned int my );
    /**
     * Return 1, if a frame gets selected which was not selected before,
     * 2, if a frame gets selected which was already selected
     */
    virtual int selectFrame( unsigned int mx, unsigned int my, bool simulate = false );
    virtual void deSelectFrame( unsigned int mx, unsigned int my );
    virtual QCursor getMouseCursor( unsigned int mx, unsigned int my );

    virtual void save( QTextStream&out );

    int getNext( QRect _rect );
    int getPageOfFrame( int i ) { return frames.at( i )->getPageNum(); }

    KWordDocument* getDocument() {return doc;}

    void setCurrent( int i ) { current = i; }
    int getCurrent() { return current; }

    void setGroupManager( KWGroupManager *gm ) { grpMgr = gm; }
    KWGroupManager *getGroupManager() { return grpMgr; }

    void setIsRemoveableHeader( bool _h )
    { removeableHeader = _h; }
    bool isRemoveableHeader()
    { return removeableHeader; }

    bool hasSelectedFrame();

    bool isVisible() { return visible; }
    void setVisible( bool v ) { visible = v; }

    QString getName() const { return name; }
    void setName( const QString &_name ) { name = _name; }

protected:
    virtual void init()
    {; }

    // document
    KWordDocument *doc;

    // frames
    QList<KWFrame> frames;

    FrameInfo frameInfo;
    int current;
    KWGroupManager *grpMgr;
    bool removeableHeader, visible;
    QString name;

};

/******************************************************************/
/* Class: KWTextFrameSet                                          */
/******************************************************************/

class KWTextFrameSet : public KWFrameSet
{
public:
    KWTextFrameSet( KWordDocument *_doc )
        : KWFrameSet( _doc ), parags(0L), format(0L) {}
    ~KWTextFrameSet();

    virtual FrameType getFrameType()
    { return FT_TEXT; }

    virtual void update();

    /**
     * If another parag becomes the first one it uses this function
     * to tell the document about it.
     */
    void setFirstParag( KWParag *_parag );
    KWParag* getFirstParag();

    /**
     * WARNING: This methode is _NOT_ efficient! It iterates through all parags!
     */
    KWParag *getLastParag();

    virtual bool isPTYInFrame( unsigned int _frame, unsigned int _ypos );

    void deleteParag( KWParag *_parag );
    void joinParag( KWParag *_parag1, KWParag *_parag2 );
    void insertParag( KWParag *_parag, InsertPos _pos );
    void splitParag( KWParag *_parag, unsigned int _pos );

    virtual void save( QTextStream&out );
    virtual void load( KOMLParser&, QValueList<KOMLAttrib>& );

    void updateCounters();
    void updateAllStyles();
    KWTextFrameSet *getCopy();

    // this function is optimized for framesets in tables and doesn't work for other purposes
    void assign( KWTextFrameSet *fs );

protected:
    virtual void init();

    // pointer to the first parag of the list of parags
    KWParag *parags;
    KWFormat *format;
};

/******************************************************************/
/* Class: KWPictureFrameSet                                       */
/******************************************************************/

class KWPictureFrameSet : public KWFrameSet
{
public:
    KWPictureFrameSet( KWordDocument *_doc )
        : KWFrameSet( _doc )
    { image = 0L; }
    virtual ~KWPictureFrameSet();

    virtual FrameType getFrameType()
    { return FT_PICTURE; }

    virtual void setImage( KWImage *_image )
    { image = _image; }
    void setFileName( QString _filename );
    void setFileName( QString _filename, QSize _imgSize );
    void setSize( QSize _imgSize );

    virtual KWImage* getImage()
    { return image; }
    QString getFileName() { return filename; }

    virtual void save( QTextStream&out );
    virtual void load( KOMLParser&, QValueList<KOMLAttrib>& );

protected:
    KWImage *image;
    QString filename;

};

/******************************************************************/
/* Class: KWPartFrameSet                                          */
/******************************************************************/

class KWPartFrameSet : public KWFrameSet
{
public:
    KWPartFrameSet( KWordDocument *_doc, KWordChild *_child );
    virtual ~KWPartFrameSet();

    virtual FrameType getFrameType()
    { return FT_PART; }

    virtual QPicture *getPicture();

    virtual void activate( QWidget *_widget );
    virtual void deactivate();

    KWordChild *getChild() { return child; }

    virtual void update();

    void enableDrawing( bool f ) { _enableDrawing = f; }

    virtual void save( QTextStream&out );
    virtual void load( KOMLParser&, QValueList<KOMLAttrib>& );

protected:
    KWordFrame *frame;
    KWordChild *child;
    QPicture pic;

    bool _enableDrawing;

};

/******************************************************************/
/* Class: KWFormulaFrameSet                                       */
/******************************************************************/

class KWFormulaFrameSet : public KWFrameSet
{
public:
    KWFormulaFrameSet( KWordDocument *_doc );
    KWFormulaFrameSet( KWordDocument *_doc, QWidget *parent );
    virtual ~KWFormulaFrameSet();

    void create( QWidget *parent );

    virtual FrameType getFrameType()
    { return FT_FORMULA; }

    virtual QPicture *getPicture();

    virtual void activate( QWidget *_widget );
    virtual void deactivate();

    virtual void update();

    virtual void save( QTextStream&out );
    virtual void load( KOMLParser&, QValueList<KOMLAttrib>& );

    void insertChar( int c );
    void setFormat( const QFont &f, const QColor &c );

    KWFormat *getFormat();

protected:
    KFormulaEdit *formulaEdit;
    KWFormat *format;
    QPicture *pic;
    QString text;
    QFont font;
    QColor color;

};

/******************************************************************/
/* Class: KWGroupManager                                          */
/*                                                                */
/* This class implements tables by acting as the manager for a    */
/* the frame(set)s which share a common grpMgr attribute.         */
/*                                                                */
/* A table can be anchored, in which case its frame(set)s are     */
/* located relative to the Y position of the anchor.              */
/******************************************************************/

class KWGroupManager: public KWCharAnchor
{
public:
    struct Cell
    {
        KWFrameSet *frameSet;
        unsigned int row, col;
        unsigned int rows, cols;
        ~Cell() {if(frameSet) delete frameSet; }
    };

    KWGroupManager( KWordDocument *_doc );
    KWGroupManager( const KWGroupManager &original );
    virtual ~KWGroupManager();

    void addFrameSet( KWFrameSet *fs, unsigned int row, unsigned int col );
    KWFrameSet *getFrameSet( unsigned int row, unsigned int col );

    bool isTableHeader( KWFrameSet *fs );

    void init( unsigned int x, unsigned int y,
               unsigned int width, unsigned int height,
               KWTblCellSize wid, KWTblCellSize hei );
    void init();
    void recalcCols();
    void recalcRows();

    unsigned int getRows() { return rows; }
    unsigned int getCols() { return cols; }

    QRect getBoundingRect();

    unsigned int getNumCells() { return cells.count(); }
    Cell *getCell( int i ) { return cells.at( i ); }
    Cell *getCell( unsigned int row, unsigned int col );
    Cell *getCell( KWFrameSet * );

    bool hasSelectedFrame();

    void moveBy( int dx, int dy );
    void drawAllRects( QPainter &p, int xOffset, int yOffset );

    void deselectAll();

    void setName( QString _name ) { name = _name; }
    QString getName() { return name; }

    void selectUntil( KWFrameSet *fs, KWPage *page );
    bool isOneSelected( KWFrameSet *fs, unsigned int &row, unsigned int &col );
    bool isOneSelected( unsigned int &row, unsigned int &col );

    void insertRow( unsigned int _idx, bool _recalc = true, bool _removeable = false );
    void insertCol( unsigned int _idx );

    void deleteRow( unsigned int _idx, bool _recalc = true );
    void deleteCol( unsigned int _idx );

    void setShowHeaderOnAllPages( bool s )
    { showHeaderOnAllPages = s; }
    bool getShowHeaderOnAllPages()
    { return showHeaderOnAllPages; }

    void updateTempHeaders();
    bool hasTempHeaders() { return hasTmpHeaders; }

    void ungroup();

    bool isActive() { return active; }

    bool joinCells();
    bool splitCell();

    QString anchorType();
    QString anchorInstance();
    void viewFormatting( QPainter &painter, int zoom );

protected:
    QList<Cell> cells;
    unsigned int rows, cols;
    KWordDocument *doc;
    QString name;
    bool showHeaderOnAllPages, hasTmpHeaders;
    bool active;
};

bool isAHeader( FrameInfo fi );
bool isAFooter( FrameInfo fi );
bool isAWrongHeader( FrameInfo fi, KoHFType t );
bool isAWrongFooter( FrameInfo fi, KoHFType t );


#endif
