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

#ifndef kword_page_h
#define kword_page_h

#include "defs.h"
#include "fc.h"
#include "format.h"
#include "paraglayout.h"
#include "searchdia.h"

#include <qscrollview.h>
#include <qpixmap.h>
#include <qtimer.h>
#include <qstringlist.h>

#include <X11/Xlib.h>

class KWordGUI;
class KWordDocument;
class QMouseEvent;
class QPaintEvent;
class QResizeEvent;
class QKeyEvent;
class QPopupMenu;
class QRegion;
class KWFrameDia;
class KWPage;
class KWFrame;
class KWTableDia;

/******************************************************************/
/* Class: KWResizeHandle                                          */
/******************************************************************/

class KWResizeHandle : public QWidget
{
    Q_OBJECT

public:
    enum Direction {
        LeftUp = 0,
        Up,
        RightUp,
        Right,
        RightDown,
        Down,
        LeftDown,
        Left
    };

    KWResizeHandle( KWPage *p, Direction d, KWFrame *frm );
    void updateGeometry();
    KWPage *getPage();

protected:
    void mouseMoveEvent( QMouseEvent *e );
    void mousePressEvent( QMouseEvent *e );
    void mouseReleaseEvent( QMouseEvent *e );

    bool mousePressed;
    int oldX, oldY;
    KWPage *page;
    Direction direction;
    KWFrame *frame;

};

/******************************************************************/
/* Class: KWPage                                                  */
/******************************************************************/

class KWPage : public QScrollView
{
    friend class KWResizeHandle;
    friend class KWordDocument;
    Q_OBJECT

public:
    KWPage( QWidget *parent, KWordDocument *_doc, KWordGUI *_gui );
    ~KWPage();

    void setDocument( KWordDocument *_doc ) { doc = _doc; }
    KWordDocument* getDocument() { return doc; }
    void init();

    void viewportMousePressEvent( QMouseEvent* e );
    void viewportMouseMoveEvent( QMouseEvent* e );
    void viewportMouseReleaseEvent( QMouseEvent* e );
    void viewportMouseDoubleClickEvent( QMouseEvent *e );
    void viewportPaintEvent( QPaintEvent *e );
    void keyPressEvent( QKeyEvent * e );

    int isCursorYVisible( KWFormatContext &_fc );
    int isCursorXVisible( KWFormatContext &_fc );

    void scrollToCursor();
    void scrollToParag( KWParag *_parag );
    void scrollToOffset( int _x, int _y );

    void formatChanged( KWFormat &_format, bool _redraw = TRUE, int flags = KWFormat::All );
    void setFlow( KWParagLayout::Flow _flow );
    void setLeftIndent( KWUnit _left );
    void setFirstLineIndent( KWUnit _first );
    void setSpaceBeforeParag( KWUnit _before );
    void setSpaceAfterParag( KWUnit _after );
    void setLineSpacing( KWUnit _spacing );

    void setParagLeftBorder( KWParagLayout::Border _brd );
    void setParagRightBorder( KWParagLayout::Border _brd );
    void setParagTopBorder( KWParagLayout::Border _brd );
    void setParagBottomBorder( KWParagLayout::Border _brd );

    KWParagLayout::Flow getFlow()
    { return fc->getParag()->getParagLayout()->getFlow(); }
    KWUnit getLeftIndent()
    { return fc->getParag()->getParagLayout()->getLeftIndent(); }
    KWUnit getFirstLineIndent()
    { return fc->getParag()->getParagLayout()->getFirstLineLeftIndent(); }
    KWUnit getSpaceBeforeParag()
    { return fc->getParag()->getParagLayout()->getParagHeadOffset(); }
    KWUnit getSpaceAfterParag()
    { return fc->getParag()->getParagLayout()->getParagFootOffset(); }
    KWUnit getLineSpacing()
    { return fc->getParag()->getParagLayout()->getLineSpacing(); }
    KWParagLayout::Border getLeftBorder()
    { return fc->getParag()->getParagLayout()->getLeftBorder(); }
    KWParagLayout::Border getRightBorder()
    { return fc->getParag()->getParagLayout()->getRightBorder(); }
    KWParagLayout::Border getTopBorder()
    { return fc->getParag()->getParagLayout()->getTopBorder(); }
    KWParagLayout::Border getBottomBorder()
    { return fc->getParag()->getParagLayout()->getBottomBorder(); }

    KWParagLayout::Counter getCounter()
    { return fc->getParag()->getParagLayout()->getCounter(); }
    void setCounter( KWParagLayout::Counter _counter );

    void applyStyle( QString _style );

    KWParagLayout *getParagLayout() { return fc->getParag()->getParagLayout(); }
    KWFormatContext *getCursor() { return fc; }

    void setEnumList();
    void setBulletList();
    void setNormalText();

    void recalcCursor( bool _repaint = true, int _pos = -1, KWFormatContext *_fc = 0L );
    void recalcPage( KWParag *_p );

    int getVertRulerPos(int contentY=-1);
    int getHorzRulerPos(int contentX=-1);

    void insertPictureAsChar( QString _filename );

    void editCut();
    void editCopy();
    void editPaste( QString _string, const QString &_mime = "text/plain" );
    void deleteTable( KWGroupManager *g );

    void recalcText();
    void recalcWholeText( bool _cursor = false, bool _fast = false );
    void recalcWholeText( KWParag *start, unsigned int fs );
    void footerHeaderDisappeared();
    void drawBorders( QPainter &_painter, QRect v_area, bool drawBack = true, QRegion *region = 0 );
    void drawFrameBorder( QPainter &p, KWFrame *frame, int dx = 0, int dy = 0 );
    void setRuler2Frame( unsigned int _frameset, unsigned int _frame ); // depricated.
    void setRuler2Frame( KWFrame *);
    void setMouseMode( MouseMode _mm );
    int getPageOfRect( QRect _rect );
    void setHilitFrameSet( int _f )
    { hiliteFrameSet = _f; repaint( false ); }

    void forceFullUpdate();
    void setPixmapFilename( QString f )
    { pixmap_name = f; }

    void setPartEntry( KoDocumentEntry e ) { partEntry = e; }

    bool find( QString _expr, KWSearchDia::KWSearchEntry *_format, bool _first, bool _cs, bool _whole,
               bool _regexp, bool _wildcard, bool &_addlen, bool _select = true );
    bool findRev( QString _expr, KWSearchDia::KWSearchEntry *_format, bool _first, bool _cs, bool _whole,
                  bool _regexp, bool _wildcard, bool &_addlen, bool _select = true );
    void replace( QString _expr, KWSearchDia::KWSearchEntry *_format, bool _addlen );

    void removeSelection();
    void addLen() { currFindPos += currFindLen; }

    void setTableConfig( unsigned int rows, unsigned int cols,
                         KWTblCellSize wid, KWTblCellSize hei,
                         bool isFloating) {
        useAnchor = isFloating;
        trows = rows; tcols = cols;
        twid = wid; thei = hei;
    }

    void setLeftFrameBorder( KWParagLayout::Border _brd, bool _enable );
    void setRightFrameBorder( KWParagLayout::Border _brd, bool _enable );
    void setTopFrameBorder( KWParagLayout::Border _brd, bool _enable );
    void setBottomFrameBorder( KWParagLayout::Border _brd, bool _enable );
    void setFrameBorderColor(const QColor &_color);
    void setFrameBackgroundColor( QBrush _color );

    KWGroupManager *getTable();
    KWGroupManager *getCurrentTable()
    { return curTable; }

    void insertAnchor( KWCharAnchor *_anchor );
    void insertVariable( VariableType type );
    void insertFootNote( KWFootNote *fn );

    void clear();

    virtual void resizeContents( int w, int h );
    virtual void setContentsPos( int x, int y );
    void repaintScreen( bool erase );
    void repaintScreen( const QRect &r, bool erase );
    void repaintScreen( int currFS, bool erase );

    void selectAll();
    void insertFormulaChar( int c );
    bool formulaIsActive() const;
    void updateSelections();

public slots:
    void newLeftIndent( double _left );
    void newFirstIndent( double _first );
    void frameSizeChanged( KoPageLayout );
    void mmEdit()
    { setMouseMode( MM_EDIT ); mmUncheckAll(); mm_menu->setItemChecked( mm_edit, true ); }
    void mmEditFrame()
    { setMouseMode( MM_EDIT_FRAME ); mmUncheckAll(); mm_menu->setItemChecked( mm_edit_frame, true ); }
    void mmCreateText()
    { setMouseMode( MM_CREATE_TEXT ); mmUncheckAll(); mm_menu->setItemChecked( mm_create_text, true ); }
    void mmCreatePix()
    { setMouseMode( MM_CREATE_PIX ); mmUncheckAll(); mm_menu->setItemChecked( mm_create_pix, true ); }
    void mmClipart()
    { setMouseMode( MM_CREATE_CLIPART ); mmUncheckAll(); mm_menu->setItemChecked( mm_create_clipart, true ); }
    void mmTable()
    { setMouseMode( MM_CREATE_TABLE ); mmUncheckAll(); mm_menu->setItemChecked( mm_create_table, true ); }
    void mmKSpreadTable() {
        setMouseMode( MM_CREATE_KSPREAD_TABLE ); mmUncheckAll();
        mm_menu->setItemChecked( mm_create_kspread_table, true );
    }
    void mmFormula()
    { setMouseMode( MM_CREATE_FORMULA ); mmUncheckAll(); mm_menu->setItemChecked( mm_create_formula, true ); }
    void mmPart()
    { setMouseMode( MM_CREATE_PART ); mmUncheckAll(); mm_menu->setItemChecked( mm_create_part, true ); }
    void femProps();
    void tabListChanged( QList<KoTabulator> *_tablist );
    void editDeleteFrame();
    void editReconnectFrame();

    int tableRows() const {
        return trows;
    }
    int tableCols() const {
        return tcols;
    }
    KWTblCellSize tableWidthMode() const {
        return twid;
    }
    KWTblCellSize tableHeightMode() const {
        return thei;
    }

protected slots:
    void frameDiaClosed();
    /**
     * Show the cursor and toggle it at regular intervals.
     */
    void startBlinkCursor();

    /**
     * Toggle the cursor.
     */
    void blinkCursor();

    /**
     * Stop toggling the cursor.
     * @param visible Indicates how the cursor is left behind.
     */
    void stopBlinkCursor( bool visible=false );

    void contentsWillMove( int x, int y );
    void doAutoScroll();
    void formatMore();
    void noInput();

protected:
    unsigned int ptLeftBorder();
    unsigned int ptRightBorder();
    unsigned int ptTopBorder();
    unsigned int ptBottomBorder();
    unsigned int ptPaperWidth();
    unsigned int ptPaperHeight();
    unsigned int ptColumnWidth();
    unsigned int ptColumnSpacing();

    void doSelect(int mx, int my);

    /**
     * Show or hide the cursor depending on @param visible.
     */
    void showCursor( bool visible );

    bool focusNextPrevChild( bool next );
//    void enterEvent( QEvent * )
//    { setFocus(); }
    void focusInEvent( QFocusEvent * );
    void focusOutEvent( QFocusEvent * );
    void keyReleaseEvent( QKeyEvent *e );

    void calcVisiblePages();

    void setupMenus();
    void mmUncheckAll();

    void selectText( int _pos, int _len, int _frameSetNum, KWTextFrameSet *_frameset,
                     KWParag *_parag, bool _select = true );

    void setRulerFirstIndent( KoRuler *ruler, KWUnit _value );
    void setRulerLeftIndent( KoRuler *ruler, KWUnit _value );

    bool editModeChanged( QKeyEvent *e );
    void repaintTableHeaders( KWGroupManager *grpMgr );

    void startDrag();
    virtual void viewportDragEnterEvent( QDragEnterEvent *e );
    virtual void viewportDragMoveEvent( QDragMoveEvent *e );
    virtual void viewportDragLeaveEvent( QDragLeaveEvent *e );
    virtual void viewportDropEvent( QDropEvent *e );
    virtual void viewportResizeEvent( QResizeEvent *e );
    bool isInSelection( KWFormatContext *_fc );

    void startProcessKeyEvent();
    void stopProcessKeyEvent();
    bool kInsertTableRow();
    bool kContinueSelection( QKeyEvent *e );
    bool kHome( QKeyEvent *e, int oldPage, int oldFrame, KWParag *oldParag, KWTextFrameSet *frameSet );
    bool kEnd( QKeyEvent *e, int oldPage, int oldFrame, KWParag *oldParag, KWTextFrameSet *frameSet );
    bool kRight( QKeyEvent *e, int oldPage, int oldFrame, KWParag *oldParag, KWTextFrameSet *frameSet );
    bool kLeft( QKeyEvent *e, int oldPage, int oldFrame, KWParag *oldParag, KWTextFrameSet *frameSet );
    bool kUp( QKeyEvent *e, int oldPage, int oldFrame, KWParag *oldParag, KWTextFrameSet *frameSet );
    bool kDown( QKeyEvent *e, int oldPage, int oldFrame, KWParag *oldParag, KWTextFrameSet *frameSet );
    bool kReturn( QKeyEvent *e, int oldPage, int oldFrame, KWParag *oldParag, KWTextFrameSet *frameSet );
    bool kDelete( QKeyEvent *e, int oldPage, int oldFrame, KWParag *oldParag, KWTextFrameSet *frameSet );
    bool kBackspace( QKeyEvent *e, int oldPage, int oldFrame, KWParag *oldParag, KWTextFrameSet *frameSet );
    bool kTab( QKeyEvent *e, int oldPage, int oldFrame, KWParag *oldParag, KWTextFrameSet *frameSet );
    bool kDefault( QKeyEvent *e, int oldPage, int oldFrame, KWParag *oldParag, KWTextFrameSet *frameSet );

    void vmmEdit( int mx, int my );
    void vmmEditFrameResize( int mx, int my , bool top, bool bottom, bool left, bool right);
    void vmmEditFrameSizeAll( int mx, int my );
    void vmmCreate( int mx, int my );
    bool vmpEdit( int mx, int my );
    void vmpEditFrame( QMouseEvent *e, int mx, int my );
    void vmpCreate( int mx, int my );
    void vmpCreatePixmap( int mx, int my );
    void vmpMidButton();
    void vmpRightButton( QMouseEvent *e, int mx, int my );
    void vmrEdit();
    void vmrEditFrame( int mx, int my );
    void vmrCreateText();
    void vmrCreatePixmap();
    void vmrCreatePartAnSoOn();
    void vmrCreateFormula();
    void vmrCreateTable();
    void vmdEdit( int mx, int my );
    void vmdEditFrame( int mx, int my );

    void paintPicture( QPainter &painter, int i );
    void paintPart( QPainter &painter, int i );
    void paintFormula( QPainter &painter, int i );
    void paintText( QPainter &painter, KWFormatContext *paintfc, int i, QPaintEvent *e );
    void finishPainting( QPaintEvent *e, QPainter &painter );

    bool allowBreak1( KWFormatContext *paintfc, unsigned int i );
    void repaintKeyEvent1( KWTextFrameSet *frameSet, bool full, bool exitASAP = true );

    void startKeySelection();
    void continueKeySelection();

    void selectAllFrames( bool select );
    void selectFrame( int mx, int my, bool select );

    void cursorGotoNextTableCell();
    void cursorGotoPrevTableCell();

    KWordDocument *doc;

    unsigned int firstVisiblePage;
    unsigned int lastVisiblePage;

    KWFormatContext *fc, *formatFC;

    KWordGUI *gui;
    KWFormat format;

    bool mousePressed;
    bool inKeyEvent;
    bool recalcAll;

    MouseMode mouseMode;
    QPopupMenu *mm_menu, *frame_edit_menu;
    int mm_edit, mm_edit_frame, mm_create_text, mm_create_pix, mm_create_clipart, mm_create_table,
        mm_create_formula, mm_create_part, mm_create_kspread_table;
    int frEditProps, frEditDel, frEditReconnect;

    // Anchor support.
    bool useAnchor;
    KWCharAnchor *anchor;
    int oldMx, oldMy;
    bool deleteMovingRect;

    int hiliteFrameSet;

    KWFrameDia *frameDia;
    QRect insRect;

    bool doRaster;
    QString pixmap_name;

    KoDocumentEntry partEntry;
    int editNum;

    KWParag *currFindParag;
    int currFindPos, currFindLen;
    int currFindFS;
    int selectedFrameSet, selectedFrame;

    // Table creation support.
    KWTableDia *tableDia;
    unsigned int tcols, trows;
    KWTblCellSize twid, thei;

    bool recalcingText;
    bool mouseMoved;
    bool maybeDrag;

    EditMode editMode;

    KWGroupManager *curTable;

    QTimer blinkTimer, scrollTimer, formatTimer, inputTimer;
    bool cursorIsVisible;

    XKeyboardControl kbdc;
    XKeyboardState kbds;
    bool keyboardRepeat, continueSelection, redrawAllWhileScrolling;

    KWParag *cachedParag;
    QStringList cachedLines;
    KWFormatContext *oldFc;
    QPoint cachedContentsPos;
    bool _erase, _setErase, redrawOnlyCurrFrameset, _scrolled;
    int currFrameSet;
    QByteArray pasteLaterData;
    QString pasteLaterMimeType;
    QRect scrollClipRect;
    int scrollDummy;

};

#endif
