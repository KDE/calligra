/******************************************************************/
/* KWord - (c) by Reginald Stadlbauer and Torben Weis 1997-1998   */
/* Version: 0.0.1                                                 */
/* Author: Reginald Stadlbauer, Torben Weis                       */
/* E-Mail: reggie@kde.org, weis@kde.org                           */
/* Homepage: http://boch35.kfunigraz.ac.at/~rs                    */
/* needs c++ library Qt (http://www.troll.no)                     */
/* written for KDE (http://www.kde.org)                           */
/* needs mico (http://diamant.vsb.cs.uni-frankfurt.de/~mico/)     */
/* needs OpenParts and Kom (weis@kde.org)                         */
/* License: GNU GPL                                               */
/******************************************************************/
/* Module: Page (header)                                          */
/******************************************************************/

#ifndef kword_page_h
#define kword_page_h

#include "defs.h"
#include "fc.h"
#include "format.h"
#include "paraglayout.h"
#include "searchdia.h"

#include <qwidget.h>
#include <qpixmap.h>
#include <qtimer.h>

class KWordGUI;
class KWordDocument;
class QMouseEvent;
class QPaintEvent;
class QResizeEvent;
class QKeyEvent;
class QPopupMenu;
class KWFrameDia;

/******************************************************************/
/* Class: KWPage                                                  */
/******************************************************************/

class KWPage : public QWidget
{
    Q_OBJECT

public:
    KWPage( QWidget *parent, KWordDocument *_doc, KWordGUI *_gui );
    ~KWPage() { delete fc; }

    void setDocument( KWordDocument *_doc )
    { doc = _doc; }
    void init();

    void mousePressEvent( QMouseEvent* e );
    void mouseMoveEvent( QMouseEvent* e );
    void mouseReleaseEvent( QMouseEvent* e );
    void mouseDoubleClickEvent( QMouseEvent *e );
    void paintEvent( QPaintEvent* e );
    void keyPressEvent( QKeyEvent * e );
    void resizeEvent( QResizeEvent *e );

    /**
     * @return 0 if the cursor is visible with respect to the
     *         y-coordinate. Mention that the cursor may be never the less
     *         invisible because of its x-coordinate. 1 is returned if the cursor
     *         is under the visible area and -1 if the cursor is above.
     *
     * @see #isCursorXVisible
     */
    int isCursorYVisible( KWFormatContext &_fc );
    /**
     * @see #isCursorYVisible
     */
    int isCursorXVisible( KWFormatContext &_fc );

    void scrollToCursor( KWFormatContext &_fc );
    void scrollToParag( KWParag *_parag );
    void scrollToOffset( int _x, int _y, KWFormatContext &_fc );

    void setXOffset( int _x )
    { xOffset = _x; calcVisiblePages(); }
    void setYOffset( int _y )
    { yOffset = _y; calcVisiblePages(); }
    int getXOffset() { return xOffset; }
    int getYOffset() { return yOffset; }

    void scroll( int dx, int dy );

    void formatChanged( KWFormat &_format, bool _redraw = true );
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

    int getVertRulerPos();

    void insertPictureAsChar( QString _filename );

    void editCut();
    void editCopy();
    void editPaste( QString _string, const QString &_mime = "text/plain" );

    void recalcText();
    void recalcWholeText( bool _cursor = false, bool _fast = false );
    void footerHeaderDisappeared();
    void drawBorders( QPainter &_painter, KRect v_area );
    void drawFrameSelection( QPainter &_painter, KWFrame *_frame );
    void setRuler2Frame( unsigned int _frameset, unsigned int _frame );
    void setMouseMode( MouseMode _mm );
    int getPageOfRect( KRect _rect );
    void setHilitFrameSet( int _f )
    { hiliteFrameSet = _f; repaint( false ); }

    void forceFullUpdate();
    void setPixmapFilename( QString f )
    { pixmap_name = f; }

    void setPartEntry( KoDocumentEntry e ) { partEntry = e; }

    void clear() { buffer.fill( white ); drawBuffer(); }

    bool find( QString _expr, KWSearchDia::KWSearchEntry *_format, bool _first, bool _cs, bool _whole,
               bool _regexp, bool _wildcard, bool &_addlen, bool _select = true );
    bool findRev( QString _expr, KWSearchDia::KWSearchEntry *_format, bool _first, bool _cs, bool _whole,
                  bool _regexp, bool _wildcard, bool &_addlen, bool _select = true );
    void replace( QString _expr, KWSearchDia::KWSearchEntry *_format, bool _addlen );

    void removeSelection();
    void addLen() { currFindPos += currFindLen; }

    void setTableConfig( unsigned int rows, unsigned int cols )
    { trows = rows; tcols = cols; }

    void setLeftFrameBorder( KWParagLayout::Border _brd, bool _enable );
    void setRightFrameBorder( KWParagLayout::Border _brd, bool _enable );
    void setTopFrameBorder( KWParagLayout::Border _brd, bool _enable );
    void setBottomFrameBorder( KWParagLayout::Border _brd, bool _enable );
    void setFrameBackgroundColor( QBrush _color );

    KWGroupManager *getTable();
    KWGroupManager *getCurrentTable()
    { return curTable; }

    void insertVariable( VariableType type );
    void insertFootNote( KWFootNote *fn );

public slots:
    void newLeftIndent( int _left );
    void newFirstIndent( int _first );
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
    void mmKSpreadTable()
    { setMouseMode( MM_CREATE_KSPREAD_TABLE ); mmUncheckAll(); mm_menu->setItemChecked( mm_create_kspread_table, true ); }
    void mmFormula()
    { setMouseMode( MM_CREATE_FORMULA ); mmUncheckAll(); mm_menu->setItemChecked( mm_create_formula, true ); }
    void mmPart()
    { setMouseMode( MM_CREATE_PART ); mmUncheckAll(); mm_menu->setItemChecked( mm_create_part, true ); }
    void femProps();
    void tabListChanged( QList<KoTabulator> *_tablist );

protected slots:
    void frameDiaClosed();
    void startBlinkCursor();
    void blinkCursor();
    void stopBlinkCursor();

protected:
    unsigned int ptLeftBorder();
    unsigned int ptRightBorder();
    unsigned int ptTopBorder();
    unsigned int ptBottomBorder();
    unsigned int ptPaperWidth();
    unsigned int ptPaperHeight();
    unsigned int ptColumnWidth();
    unsigned int ptColumnSpacing();

    void enterEvent( QEvent * )
    { setFocus(); }
    void focusInEvent( QFocusEvent * );
    void focusOutEvent( QFocusEvent * );
    void keyReleaseEvent( QKeyEvent *e );

    /**
     * Looks at 'yOffset' and 'paperHeight' and calculates the first and
     * last visible pages. The values are stored in 'firstVisiblePage' and
     * 'lastVisiblePages'.
     *
     * @see #yOffset
     * @see #paperHeight
     * @see #firstVisiblePage
     * @see #lastVisiblePage
     */
    void calcVisiblePages();

    void drawBuffer();
    void drawBuffer( KRect _rect );
    void copyBuffer();
    void setupMenus();
    void mmUncheckAll();

    void selectText( int _pos, int _len, int _frameSetNum, KWTextFrameSet *_frameset, KWParag *_parag, bool _select = true );

    void setRulerFirstIndent( KoRuler *ruler, KWUnit _value );
    void setRulerLeftIndent( KoRuler *ruler, KWUnit _value );

    bool editModeChanged( QKeyEvent *e );
    void repaintTableHeaders( KWGroupManager *grpMgr );

    void startDrag();
    virtual void dragEnterEvent( QDragEnterEvent *e );
    virtual void dragMoveEvent( QDragMoveEvent *e );
    virtual void dragLeaveEvent( QDragLeaveEvent *e );
    virtual void dropEvent( QDropEvent *e );
    bool isInSelection( KWFormatContext *_fc );

    KWordDocument *doc;
    bool markerIsVisible;
    bool paint_directly, has_to_copy;

    /**
     * The xOffset in zoomed pixels.
     */
    unsigned int xOffset;
    /**
     * The yOffset in zoomed pixels.
     */
    unsigned int yOffset;

    /**
     * The first ( partial? ) visible page.
     *
     * @see #calcVisiblePages
     */
    unsigned int firstVisiblePage;
    /**
     * The last ( partial? ) visible page.
     *
     * @see #calcVisiblePages
     */
    unsigned int lastVisiblePage;

    KWFormatContext *fc;

    KWordGUI *gui;
    QPixmap buffer;
    KWFormat format;

    bool mousePressed;
    bool inKeyEvent;
    bool recalcAll;

    MouseMode mouseMode;
    QPopupMenu *mm_menu, *frame_edit_menu;
    int mm_edit, mm_edit_frame, mm_create_text, mm_create_pix, mm_create_clipart, mm_create_table,
        mm_create_formula, mm_create_part, mm_create_kspread_table;

    int oldMx, oldMy;
    bool deleteMovingRect;

    int hiliteFrameSet;

    KWFrameDia *frameDia;
    KRect insRect;

    bool redrawAllWhileScrolling, doRaster;
    QString pixmap_name;

    KoDocumentEntry partEntry;
    int editNum;

    KWParag *currFindParag;
    int currFindPos, currFindLen;
    int currFindFS;
    int selectedFrameSet, selectedFrame;
    unsigned int tcols, trows;

    bool recalcingText;
    bool mouseMoved;
    bool maybeDrag;

    EditMode editMode;

    KWGroupManager *curTable;

    QTimer blinkTimer;
    bool cursorIsVisible;

};

#endif
