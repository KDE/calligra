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

const unsigned int minFrameWidth=18;
const unsigned int minFrameHeight=20;
const unsigned int tableCellSpacing=3;

/* Runaround types */
enum RunAround { RA_NO = 0, RA_BOUNDINGRECT = 1, RA_SKIP = 2 };

/* what should happen when the frame is full */
enum FrameBehaviour { AutoExtendFrame=0 , AutoCreateNewFrame=1, Ignore=2 };

/* types of behaviours for creating a followup frame on new page */
enum NewFrameBehaviour { Reconnect=0, NoFollowup=1, Copy=2 };

/* this frame will only be copied to: */
enum SheetSide { AnySide=0, OddSide=1, EvenSide=2};

/* The different types of framesets */
enum FrameType { FT_BASE = 0, FT_TEXT = 1, FT_PICTURE = 2, FT_PART = 3, FT_FORMULA = 4 };

/* The different types of textFramesets */
enum FrameInfo { FI_BODY = 0, FI_FIRST_HEADER = 1, FI_ODD_HEADER = 2, FI_EVEN_HEADER = 3,
                 FI_FIRST_FOOTER = 4, FI_ODD_FOOTER = 5, FI_EVEN_FOOTER = 6,
                 FI_FOOTNOTE = 7 };

/******************************************************************
 * Class: KWFrame
 * This class represents a single frame.
 * A frame will belong to a frameset which states its contents.
 * A frame will NOT have contents the frameset will store that. 
 * A frame is really just a square that is used to place the content
 * of a frameset.
 ******************************************************************/

class KWFrame : public QRect
{
    friend class KWPage;

public:

    /* Constructors */
    KWFrame();
    KWFrame(KWFrameSet *fs, const QPoint &topleft, const QPoint &bottomright );
    KWFrame(KWFrameSet *fs, const QPoint &topleft, const QSize &size );
    KWFrame(KWFrameSet *fs, int left, int top, int width, int height );
    KWFrame(KWFrameSet *fs, int left, int top, int width, int height, RunAround _ra, KWUnit _gap );
    KWFrame(KWFrameSet *fs, const QRect &_rect );
    /* destructor */
    virtual ~KWFrame();

    /* a frame can be selected by the user clicking on it. The frame 
       remembers if it is selected */
    void setSelected( bool _selected ) { selected = _selected; }
    bool isSelected() { return selected; }

    /* Run around stuff */
    void addIntersect( QRect &_r );
    bool hasIntersections() { return !intersections.isEmpty(); }
    void clearIntersects() { intersections.clear(); emptyRegionDirty = TRUE; }

    QRegion getEmptyRegion( bool useCached = TRUE );

    unsigned int getNextFreeYPos( unsigned int _y, unsigned int _h );

    QCursor getMouseCursor( int mx, int my, bool table );

    /* Calculate the left indent we have to make for a free spot to render in */
    int getLeftIndent( int y, int h );
    /* Calculate the left indent we have to make for a free spot to render in */
    int getRightIndent( int y, int h );

    KWUnit getRunAroundGap() { return runAroundGap; }
    void setRunAroundGap( KWUnit gap ) { runAroundGap = gap; }

    void setRunAround( RunAround _ra ) { runAround = _ra; }
    RunAround getRunAround() { return runAround; }

    FrameBehaviour getFrameBehaviour() { return frameBehaviour; }
    void setFrameBehaviour( FrameBehaviour fb ) { frameBehaviour = fb; }

    FrameInfo getFrameInfo();
    FrameType getFrameType();

    /* frame duplication properties */
    SheetSide getSheetSide() { return sheetSide; }
    void setSheetSide( SheetSide ss ) { sheetSide = ss; }

    NewFrameBehaviour getNewFrameBehaviour() { return newFrameBehaviour; }
    void setNewFrameBehaviour( NewFrameBehaviour nfb ) { newFrameBehaviour = nfb; }

    /* Data stucture methods */
    KWFrameSet *getFrameSet() { return frameSet; }
    void setFrameSet( KWFrameSet *fs ) { frameSet = fs; }

    bool isMostRight() { return mostRight; }
    void setMostRight( bool _mr ) { mostRight = _mr; }

    void setPageNum( int i ) { pageNum = i; }
    int getPageNum() { return pageNum; }

    /* All borders can be custum drawn with their own colors etc. */
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

    /* Resize handles (in kword_page.h) are the dots that are drawn on selected 
       frames, this creates and deletes then */
    void createResizeHandles();
    void createResizeHandlesForPage(KWPage *page);
    void removeResizeHandlesForPage(KWPage *page);
    void removeResizeHandles();
    void updateResizeHandles();

    QBrush getBackgroundColor() { return backgroundColor; }
    void setBackgroundColor( QBrush _color ) { backgroundColor = _color; }

    /* border size */
    void setBLeft( KWUnit b ) { bleft = b; }
    void setBRight( KWUnit b ) { bright = b; }
    void setBTop( KWUnit b ) { btop = b; }
    void setBBottom( KWUnit b ) { bbottom = b; }

    KWUnit getBLeft() { return bleft; }
    KWUnit getBRight() { return bright; }
    KWUnit getBTop() { return btop; }
    KWUnit getBBottom() { return bbottom; }

    /* returns a copy of self */
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

private:
    QList<KWResizeHandle> handles;
    KWFrame &operator=( const KWFrame &_frame );
    KWFrame ( const KWFrame &_frame );
    KWFrameSet *frameSet;
};

/******************************************************************
 * Class: KWFrameSet
 * Base type, a frameset holds content as well as frames to show that 
 * content.
 * The different types of content are implemented in the different
 * types of frameSet implementations (see below)
 ******************************************************************/

class KWFrameSet
{
public:


    // constructor
    KWFrameSet( KWordDocument *_doc );
    // destructor
    virtual ~KWFrameSet() {; }

    virtual FrameType getFrameType() { return FT_BASE; }
    virtual FrameInfo getFrameInfo() { return frameInfo; }
    void setFrameInfo( FrameInfo fi ) { frameInfo = fi; }

    // frame management
    virtual void addFrame( KWFrame _frame );
    virtual void addFrame( KWFrame *_frame );
    virtual void delFrame( unsigned int _num );
    virtual void delFrame( KWFrame *frm, bool remove = TRUE );

    // retrieve frame from x and y coords (absolute coords)
    virtual int getFrame( int _x, int _y );
    virtual KWFrame *getFrame( unsigned int _num );
    virtual int getFrameFromPtr( KWFrame *frame );
    virtual unsigned int getNumFrames() { return frames.count(); }

    virtual bool isPTYInFrame( unsigned int /*_frame*/, unsigned int /*_ypos */ )
        { return true; }

    // reshuffle frames to text is always displayed from top-left down and then right.
    virtual void update() {; }

/*
    // remove all frames
    virtual void clear()
    { frames.clear(); } */

    // returns true if we have a frame occupying that position
    virtual bool contains( unsigned int mx, unsigned int my );

    /**
     * Return 1, if a frame gets selected which was not selected before,
     * 2, if a frame gets selected which was already selected
     * Also select the frame if simulate==false.
     */
    virtual int selectFrame( unsigned int mx, unsigned int my, bool simulate = false );
    virtual void deSelectFrame( unsigned int mx, unsigned int my );
    virtual QCursor getMouseCursor( unsigned int mx, unsigned int my );

    /* create XML to describe yourself */
    virtual void save( QTextStream&out );

    int getNext( QRect _rect );
    // returns page number of the numbered frame 
    int getPageOfFrame( int i ) { return frames.at( i )->getPageNum(); }

    KWordDocument* getDocument() {return doc;}

    // only used for headers and footers...
    void setCurrent( int i ) { current = i; }
    int getCurrent() { return current; }

    // make this frameset part of a groupmanager
    void setGroupManager( KWGroupManager *gm ) { grpMgr = gm; }
    KWGroupManager *getGroupManager() { return grpMgr; }

    // table headers can created by the groupmanager, we store the fact that
    // it is here.
    void setIsRemoveableHeader( bool _h ) { removeableHeader = _h; }
    bool isRemoveableHeader() { return removeableHeader; }

    // returns if one of our frames has been selected.
    bool hasSelectedFrame();

    // returns the visibility of the frameset.
    bool isVisible() { return visible; }
    void setVisible( bool v ) { visible = v; }

    QString getName() const { return name; }
    void setName( const QString &_name ) { name = _name; }

protected:
    virtual void init() {; }

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


/******************************************************************
 * Class: KWTextFrameSet
 * Contains text in the form of paragraphs and frames to display 
 * that text
 ******************************************************************/

class KWTextFrameSet : public KWFrameSet
{
public:

    // constructor
    KWTextFrameSet( KWordDocument *_doc )
        : KWFrameSet( _doc ), parags(0L), format(0L) {}
    // descructor
    ~KWTextFrameSet();

    virtual FrameType getFrameType() { return FT_TEXT; }

    // reshuffle frames to text is always displayed from top-left down and then right.
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

    // text management
    void deleteParag( KWParag *_parag );
    void joinParag( KWParag *_parag1, KWParag *_parag2 );
    void insertParag( KWParag *_parag, InsertPos _pos );
    void splitParag( KWParag *_parag, unsigned int _pos );

    virtual void save( QTextStream&out );
    virtual void load( KOMLParser&, QValueList<KOMLAttrib>& );

    void updateCounters();
    // stupid updating of all styles.
    void updateAllStyles();

    // returns a deep copy of self (and all it contains)
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

    virtual FrameType getFrameType() { return FT_PICTURE; }

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

    virtual FrameType getFrameType() { return FT_PART; }

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

    virtual FrameType getFrameType() { return FT_FORMULA; }

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
/* This class implements tables by acting as the manager for      */
/* the frame(set)s which make up the table cells (hence share a   */
/* common grpMgr attribute.                                       */
/*                                                                */
/* A table can be anchored, in which case its frame(set)s are     */
/* located relative to the Y position of the anchor.              */
/* We have a cell structure which contains one frameset, because  */
/* of the nature of the table this frameset will always hold      */
/* excectly one frame. Therefor the term Cell, frameSet and frame */
/* can be used to describe the same thing; one table-cell         */
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

    // constructors
    KWGroupManager( KWordDocument *_doc );
    KWGroupManager( const KWGroupManager &original );
    // destructor
    virtual ~KWGroupManager();

    // frameset management
    void addFrameSet( KWFrameSet *fs, unsigned int row, unsigned int col );
    KWFrameSet *getFrameSet( unsigned int row, unsigned int col );

    // first row and auto-added rows are the table headers.
    bool isTableHeader( KWFrameSet *fs );

    // put all the frames in the right spots.
    void init( unsigned int x, unsigned int y,
               unsigned int width, unsigned int height,
               KWTblCellSize wid, KWTblCellSize hei );
    // announce all frames to the document
    void init();
    // resize and position all cells
    void recalcCols();
    void recalcRows();

    // returns the number of rows
    unsigned int getRows() { return rows; }
    // returns the number of columns
    unsigned int getCols() { return cols; }

    // returns a QRect which outlines the whole of the table.
    QRect getBoundingRect();

    // returns the number of cells the table contains, this includes 
    // temporary headers.
    unsigned int getNumCells() { return cells.count(); }

    // returns a specific table cell.
    Cell *getCell( int i ) { return cells.at( i ); }
    Cell *getCell( unsigned int row, unsigned int col );
    Cell *getCell( KWFrameSet * );

    // returns the fact if one cell (==frame) has been selected
    bool hasSelectedFrame();

    // deselect all frames
    void deselectAll();

    // move the whole of the table, this is mainly for anchored frames.
    void moveBy( int dx, int dy );
    void drawAllRects( QPainter &p, int xOffset, int yOffset );

    // sets the name of the table, this _must_ be a unique name!
    void setName( QString _name ) { name = _name; }
    QString getName() { return name; }

    // select all frames from the first selected to the argument frameset.
    void selectUntil( KWFrameSet *fs);
    bool getFirstSelected(unsigned int &row, unsigned int &col );
    bool isOneSelected( unsigned int &row, unsigned int &col );

    // insert a row of new cells
    void insertRow( unsigned int _idx, bool _recalc = true, bool _removeable = false );
    // insert a column of new cells
    void insertCol( unsigned int _idx );

    // remove all the cells in a certain row
    void deleteRow( unsigned int _idx, bool _recalc = true );
    // remove all the cells in a certain col
    void deleteCol( unsigned int _idx );

    // the boolean actually works, but is not saved (to xml) yet :(
    void setShowHeaderOnAllPages( bool s ) { showHeaderOnAllPages = s; }
    bool getShowHeaderOnAllPages() { return showHeaderOnAllPages; }

    // redraw temp headers. 
    void updateTempHeaders();
    bool hasTempHeaders() { return hasTmpHeaders; }

    // release the constrains of the table and allow all frames to be 
    // edited apart from each other. (ps. there is no way back..)
    void ungroup();

    bool isActive() { return active; }

    // merge cells to one cell.
    bool joinCells();
    // split selected cell into a number of cells
    bool splitCell(unsigned int intoRows, unsigned int intoCols);

    // display formatting information
    void viewFormatting( QPainter &painter, int zoom );
    // do a number of complex tests to test the validity of the table. Missing/duplicate cells 
    // and wrong values will be detected (and corrected)
    void validate();

    // Prerender the whole table to resize the table cells so all text will fitt.
    void preRender();

    QString anchorType();
    QString anchorInstance();

protected:
    QList<Cell> cells;
    unsigned int rows, cols;
    KWordDocument *doc;
    QString name;
    bool showHeaderOnAllPages, hasTmpHeaders;
    bool active;
    bool isRendered;
};

bool isAHeader( FrameInfo fi );
bool isAFooter( FrameInfo fi );
bool isAWrongHeader( FrameInfo fi, KoHFType t );
bool isAWrongFooter( FrameInfo fi, KoHFType t );


#endif
