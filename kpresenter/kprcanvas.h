// -*- Mode: c++; c-basic-offset: 4; indent-tabs-mode: nil; tab-width: 4; -*-
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


#ifndef __KPRCANVAS__
#define __KPRCANVAS__

#include <qwidget.h>
#include <qptrlist.h>
#include <qpicture.h>
#include <qvaluelist.h>
#include <qpixmap.h>

#include <koRuler.h>
#include <koQueryTrader.h>
#include "koPointArray.h"
#include "global.h"
#include <koRect.h>
#include <kotextformat.h>

class KURL;
class KoTextFormatInterface;
class KPresenterView;
class KPresenterDoc;
class QPainter;
class KTextEdit;
class KPTextObject;
class KPObject;
class QPopupMenu;
class QResizeEvent;
class QPaintEvent;
class QDragEnterEvent;
class QDragMoveEvent;
class QDropEvent;
class QFocusEvent;
class QMouseEvent;
class QWheelEvent;
class QKeyEvent;
class KPPixmapObject;
class KPBackGround;
class QRect;
class QSize;
class QPoint;
class KPresenterSoundPlayer;
class KoTextObject;
class KoParagCounter;
class KPrPage;
class KPrinter;
class KPTextView;
class KPPartObject;
class KCommand;
/**
 * Class KPCanvas - There is a single instance of this class for a given view.
 *
 * It manages the graphical representation of all the objects.
 */
class KPrCanvas : public QWidget
{
    Q_OBJECT

public:

    // constructor - destructor
    KPrCanvas( QWidget *parent=0,const char *name=0,KPresenterView *_view=0 );
    ~KPrCanvas();

    KPresenterView * getView()const { return m_view; }

    // The equivalent of contentsX() and contentsY() for a QScrollView
    // The Canvas is a widget, not a scrollview (because we handle the scrollbars ourselves,
    // to have the "goto page" buttons at the bottom of the scrollbars etc.)
    int diffx() const { return m_xOffset; }
    int diffy() const { return m_yOffset; }
    void setDiffX( int _diffX ) { m_xOffset = _diffX; }
    void setDiffY( int _diffY ) { m_yOffset = _diffY; }
    void scrollX( int x );
    void scrollY( int y );

    // public functions
    void selectAllObj();
    void deSelectAllObj();
    void selectObj( KPObject* );
    void deSelectObj( KPObject* );
    void setTextFormat(const KoTextFormat &format, int flags);

    void setTextColor( const QColor & );
    void setTextBackgroundColor( const QColor & );
    void setTextAlign( int );
    void setTextDepthPlus();
    void setTextDepthMinus();
    void setNewRightIndent(double _rightIndent);
    void setNewLeftIndent(double _leftIndent);
    void setNewFirstIndent(double _firstIndent);
    //void setTextCounter(KoParagCounter counter);
    void setTabList( const KoTabulatorList & tabList );
    void setTextBold( bool b );
    void setTextItalic( bool b );
    void setTextUnderline( bool b );
    void setTextStrikeOut( bool b );
    void setTextFamily( const QString &f );
    void setTextPointSize( int s );
    void setTextSubScript( bool b );
    void setTextSuperScript( bool b );
    void setTextDefaultFormat( );
    void setIncreaseFontSize();
    void setDecreaseFontSize();
    KPTextObject* kpTxtObj() const;

    // Start a screen presentation
    // All pages if curPgNum is -1, otherwise just curPgNum (1-based)
    void startScreenPresentation( float presFakt, int curPgNum = -1);
    void stopScreenPresentation();
    bool pNext( bool );
    bool pPrev( bool );
    void setNextPageTimer( bool _nextPageTimer ) { nextPageTimer = _nextPageTimer; }

    unsigned int presPage() const { return currPresPage; } // 1 based
    int presStep() const { return currPresStep; }
    float presFakt() const { return _presFakt; }
    int numPresSteps() const { return presStepList.count(); }
    int numPresPages() const { return slideList.count(); }

    bool canAssignEffect( QPtrList<KPObject> &objs ) const;

    void print( QPainter*, KPrinter*, float, float );


    void setPartEntry( KoDocumentEntry& _e )
        { partEntry = _e; }
    void setToolEditMode( ToolEditMode _m, bool updateView = true );
    void setAutoForm( const QString &_autoform )
        { autoform = _autoform; }

    void drawPageInPix( QPixmap&, int pgnum, int zoom, bool forceRealVariableValue = false );

    void gotoPage( int pg );

    KPrPage* activePage() const;

    bool oneObjectTextExist() const;
    bool oneObjectTextSelected() const;
    bool isOneObjectSelected() const;
    /** Returns the list of selected text objects */
    QPtrList<KPTextObject> selectedTextObjs() const;
    /**
     * Returns the list of interfaces to use for changing the text format.
     * This can be either the currently edited text object,
     * or the list of text objects currently selected
     */
    QPtrList<KoTextFormatInterface> applicableTextInterfaces() const;
    /**
     * Returns the list of text objects, either selected or activated.
     */
    QPtrList<KPTextObject> applicableTextObjects() const;

    void setMouseSelectedObject(bool b);

    bool haveASelectedPartObj() const;
    bool haveASelectedGroupObj() const;
    bool haveASelectedPixmapObj() const;

    KPTextView *currentTextObjectView() const { return m_currentTextObjectView; }

    QPtrList<KPObject> objectList() const;

    // get - set data
    const QPtrList<KPObject> & getObjectList() const;

    void stopSound();

    //for KPTextView
    void dragStarted() { mousePressed = false; }

    void setXimPosition( int x, int y, int w, int h, QFont *f );
    void createEditing( KPTextObject *textObj );


    void insertLineH( const QRect&, bool );
    void insertLineV( const QRect&, bool );
    void insertLineD1( const QRect&, bool );
    void insertLineD2( const QRect&, bool );
    void insertRect( const QRect& );
    void insertEllipse( const QRect& );
    void insertPie( const QRect& );
    KPTextObject* insertTextObject( const QRect& );
    void insertPicture( const QRect &_r, const QPoint & tl = QPoint());
    void changePicture( const KURL & url );

    int numberOfObjectSelected() const;
    KPObject *getSelectedObj() const;
    KPPixmapObject *getSelectedImage() const;

    int getPenBrushFlags() const;
    bool getEditMode() const { return editMode; }

    void ungroupObjects();
    void groupObjects();

    unsigned int objNums() const;
    void removeHelpLine();
    void changeHelpLinePosition( double newPos);

    int tmpHorizHelpLine() const { return  m_tmpHorizHelpline;}
    int tmpVertHelpLine() const { return m_tmpVertHelpline;}

    void tmpDrawMoveHelpLine( const QPoint & newPos, bool _horizontal );

    void setTmpHelpLinePosX( double pos) { tmpHelpLinePosX = pos ;}
    void setTmpHelpLinePosY( double pos) { tmpHelpLinePosY = pos;}



    void changeHelpPointPosition( KoPoint newPos);
    void removeHelpPoint();

    int tmpHelpPoint() const { return  m_tmpHelpPoint;}

    void setTmpHelpPointPos( KoPoint pos) { tmpHelpPointPos = pos ;}

    void tmpDrawMoveHelpPoint( const QPoint & newPos );

    void ensureVisible( int x, int y, int xmargin, int ymargin );

    KoRect objectSelectedBoundingRect() const;

    void scrollTopLeftPoint( const QPoint & pos );

    void textContentsToHeight();
    void textObjectToContents();

    void flipObject( bool _horizontal );

    KCommand *setKeepRatioObj( bool p );
    KCommand *setProtectSizeObj(bool protect);

    KoRect zoomAllObject();


    //return pos of kptextobj
    int textObjectNum( KPTextObject * obj ) const;
    KPTextObject * textObjectByPos( int pos ) const;

    void alignObjLeft();
    void alignObjCenterH();
    void alignObjRight();
    void alignObjTop();
    void alignObjCenterV();
    void alignObjBottom();

    bool canMoveOneObject()const;
    QPtrList<KPTextObject> listOfTextObjs() const;

    bool getSticky( bool sticky ) const;

    bool getProtect( bool prot ) const;
    bool getKeepRatio( bool _ratio ) const;
    //return true if we are selected several object and there are not
    //the same "keep ratio" properties"
    bool differentKeepRatio( bool p )const;
    bool differentProtect( bool p )const;

    bool getProtectContent(bool prot) const;
    QPen getPen( const QPen & _pen )const;
    QBrush getBrush( const QBrush & _brush )const;
    LineEnd getLineBegin( LineEnd  _end )const;
    LineEnd getLineEnd( LineEnd  _end )const;
    FillType getFillType( FillType _fillType ) const;
    QColor getGColor1( const QColor & _col )const;
    QColor getGColor2( const QColor & _col )const;
    BCType getGType( BCType _gt )const;
    bool getGUnbalanced( bool _g )const;
    int getGXFactor( int _g )const;
    int getGYFactor( int _g )const;
    int getRndY( int _ry )const;
    int getRndX( int _rx )const;
    int getPieAngle( int pieAngle )const;
    int getPieLength( int pieLength )const;
    PieType getPieType( PieType pieType )const;
    bool getCheckConcavePolygon( bool check ) const;
    int getCornersValue( int corners ) const;
    int getSharpnessValue( int sharpness ) const;
    PictureMirrorType getPictureMirrorType( PictureMirrorType type ) const;
    int getPictureDepth( int depth ) const;
    bool getPictureSwapRGB( bool swapRGB ) const;
    bool getPictureGrayscal( bool grayscal ) const;
    int getPictureBright( int bright ) const;
    ImageEffect getImageEffect(ImageEffect eff) const;
    QPixmap getPicturePixmap() const;
    KCommand *setProtectContent( bool b );
    void closeObject(bool _close);
    void layout();
    void alignVertical( VerticalAlignmentType _type );
    void savePicture();

    void dropImage( QMimeSource * data, bool resizeImageToOriginalSize = false , int posX = 10, int posX=10 );

public slots:
    void exitEditMode();

    void clipCut();
    void clipCopy();
    void clipPaste();
    void deleteObjs();
    void copyObjs();

    void rotateObjs();
    void shadowObjs();

    void chPic();
    void picViewOriginalSize();
    void picViewOrig640x480();
    void picViewOrig800x600();
    void picViewOrig1024x768();
    void picViewOrig1280x1024();
    void picViewOrig1600x1200();
    void picViewOrigFactor();
    void setActivePage( KPrPage* active );

signals:

    // signals to notify of changes
    void fontChanged( const QFont & );
    void colorChanged( const QColor & );
    void alignChanged( int );
    void updateSideBarItem( int );
    void stopPres();
    void objectSelectedChanged();
    void objectSizeChanged();
    // Emitted when the current frameset edit changes
    void currentObjectEditChanged();

    void selectionChanged( bool hasSelection );
    void sigMouseWheelEvent( QWheelEvent * );

protected:

    struct PicCache
    {
        QPicture pic;
        int num;
        int subPresStep;
    };

    // functions for displaying
    virtual void paintEvent( QPaintEvent* );
    void drawBackground( QPainter*, const QRect& ) const;

    //draw all object : object in current page + object in sticky page
    void drawObjects( QPainter*, const QRect&, bool drawCursor,
                      SelectionMode selectionMode, bool doSpecificEffects ) const;

    void drawObjectsInPage(QPainter *painter, const KoRect& rect2, bool drawCursor,
                           SelectionMode selectionMode, bool doSpecificEffects,
                           const QPtrList<KPObject> & obj) const;

    void drawAllObjectsInPage( QPainter *painter, const QPtrList<KPObject> & obj ) const;

    // draw all helpline
    void drawHelplines(QPainter *painter, const QRect &rect2) const;

    void drawHelpPoints( QPainter *painter, const QRect &rect2) const;

    // draw grid
    void drawGrid(QPainter *painter, const QRect &rect2) const;


    QRect getOldBoundingRect( const KPObject *obj );

    void drawCurrentPageInPix( QPixmap& ) const;
    virtual void mousePressEvent( QMouseEvent *e );
    virtual void mouseReleaseEvent( QMouseEvent *e );
    virtual void mouseMoveEvent( QMouseEvent *e );
    virtual void mouseDoubleClickEvent( QMouseEvent *e );
    virtual void wheelEvent( QWheelEvent *e );
    virtual void resizeEvent( QResizeEvent *e );
    KPObject *getObjectAt( const KoPoint&pos );
    virtual void focusInEvent( QFocusEvent* ) {}
    virtual void focusOutEvent( QFocusEvent* ) {}
    virtual void enterEvent( QEvent *e );
    virtual void leaveEvent( QEvent *e );
    virtual void dragEnterEvent( QDragEnterEvent *e );
    virtual void dragMoveEvent( QDragMoveEvent *e );
    virtual void dropEvent( QDropEvent *e );
    virtual void keyPressEvent( QKeyEvent *e );
    virtual void keyReleaseEvent( QKeyEvent *e );

    void eraseEmptySpace( QPainter * painter, const QRegion & emptySpaceRegion, const QBrush & brush ) const;

    // setup popupmenus
    void setupMenus();

    unsigned int currPgNum() const;
    QColor txtBackCol() const;
    bool spInfiniteLoop() const;
    bool spManualSwitch() const;
    QRect getPageRect( bool decBorders = true ) const;
    unsigned int pageNums() const;
    float objSpeedFakt() const;
    float pageSpeedFakt() const;
    void calcRatio( double &dx, double &dy, ModifyType _modType, double ratio ) const;

    void _repaint( bool erase=true );
    void _repaint( const QRect &r );
    void _repaint( KPObject *o );

    void printPage( QPainter*, int pageNum );
    void doObjEffects();
    void drawObject( KPObject*, QPixmap*, int, int, int, int, int, int );

    KPPartObject *insertObject( const QRect& );
    void insertAutoform( const QRect&, bool );
    void insertFreehand( const KoPointArray &_pointArray );
    void insertPolyline( const KoPointArray &_pointArray );
    void insertCubicBezierCurve( const KoPointArray &_pointArray );
    void insertPolygon( const KoPointArray &_pointArray );
    void insertClosedLine( const KoPointArray &_pointArray );

    void selectNext();
    void selectPrev();

    void scalePixmapToBeOrigIn( const KoSize &currentSize, const KoSize &pgSize,
                                const QSize &presSize, KPPixmapObject *obj );
    bool getPixmapOrigAndCurrentSize( KPPixmapObject *&obj, KoSize *origSize, KoSize *currentSize );
    void setTextBackground( KPTextObject *obj );
    virtual bool eventFilter( QObject *o, QEvent *e );
    virtual bool focusNextPrevChild( bool );

    void endDrawPolyline();

    void drawCubicBezierCurve( int _dx, int _dy );
    void endDrawCubicBezierCurve();


#ifndef NDEBUG
    void printRTDebug( int info );
#endif

    KPrPage *stickyPage() const;

    void moveHelpLine(const QPoint & pos);
    void tmpMoveHelpLine( const QPoint & newPos);

    void moveHelpPoint( const QPoint & newPos );
    void scrollCanvas(const KoRect& oldPos);

    KoRect getAlignBoundingRect() const;

private:
    QValueList<int> pages(const QString &range) const;
    bool pagesHelper(const QString &chunk, QValueList<int> &list) const;
    void picViewOrigHelper(int x, int y);

    void moveObject( int x, int y, bool key );
    void resizeObject( ModifyType _modType, int _dx, int _dy );

    void raiseObject( KPObject *_kpobject );
    void lowerObject();
    int selectedObjectPosition;

    bool nextPageTimer;

    void playSound( const QString &soundFileName );

    void drawPolygon( const KoPoint &startPoint, const KoPoint &endPoint );

    void drawPieObject(QPainter *p,  const QRect & );
    void limitSizeOfObject();

    QPoint limitOfPoint(const QPoint& _point) const;

    void calcBoundingRect();
    bool objectIsAHeaderFooterHidden(KPObject *obj) const;

    KPTextObject* textUnderMouse( const QPoint & point );
    bool checkCurrentTextEdit( KPTextObject * textObj );

    void rectSymetricalObjet();
    QPoint applyGrid( const QPoint &pos,bool offset);
    KoPoint applyGrid( const KoPoint &pos);
    double applyGridX( double x );
    double applyGridY( double y );
    int applyGridOnPosX( int pos ) const;
    int applyGridOnPosY( int pos ) const;

private slots:
    void toFontChanged( const QFont &font ) { emit fontChanged( font ); }
    void toColorChanged( const QColor &color ) { emit colorChanged( color ); }
    void toAlignChanged( int a ) { emit alignChanged( a ); }
    void drawingMode();
    void switchingMode();
    void slotGotoPage();
    void slotExitPres();
    void terminateEditing( KPTextObject * );

private:
    // variables
    QPopupMenu *presMenu;
    bool showingLastSlide;
    bool mousePressed;
    bool drawContour;
    double startAngle;
    ModifyType modType;
    unsigned int oldMx, oldMy;

    KPObject *resizeObjNum, *editNum, *rotateNum;

    bool fillBlack;
    KPresenterView *m_view;
    bool editMode, goingBack, drawMode;
    bool drawLineInDrawMode;
    bool mouseSelectedObject;
    unsigned int currPresPage, currPresStep, subPresStep;
    unsigned int oldPresPage, oldPresStep, oldSubPresStep;
    float _presFakt;
    int m_showOnlyPage; // 1-based (-1 = all)
    QValueList<int> presStepList, slideList;
    QValueList<int>::Iterator slideListIterator;
    int PM_DM, PM_SM;
    int firstX, firstY;
    double axisX, axisY;
    int delPageId;
    bool drawRubber;
    QRect rubber, oldBoundingRect;
    KoRect resizeRect;
    ToolEditMode toolEditMode;
    QRect insRect;
    KoDocumentEntry partEntry;
    // List of objects to draw, from the previous step in the presentation
    // (those that remain on screen between two steps)
    QPtrList <KPObject> tmpObjs;
    QString autoform;
    bool inEffect, keepRatio;
    double ratio;
    QPixmap buffer;

    KPTextView *m_currentTextObjectView;

    KPresenterSoundPlayer *soundPlayer;

    KoPointArray m_pointArray, m_oldCubicBezierPointArray;
    QPoint m_dragStartPoint, m_dragEndPoint, m_dragSymmetricEndPoint;
    KoPoint m_CubicBezierSecondPoint, m_CubicBezierThirdPoint;
    unsigned int m_indexPointArray;
    bool m_drawPolyline;
    bool m_drawCubicBezierCurve;
    bool m_drawLineWithCubicBezierCurve;
    bool m_zoomRubberDraw;

    KPrPage *m_activePage;
    int m_xOffset, m_yOffset;
    int m_xOffsetSaved, m_yOffsetSaved; // saved when going fullscreen

    KoRect m_boundingRect; // when moving object(s)
    KoPoint m_hotSpot; // when moving frame(s)

    KoPoint moveStartPosMouse; // start position for move with mouse
    KoPoint moveStartPosKey; // start position for move with key
    int m_tmpHelpPoint;
    KoPoint tmpHelpPointPos;

    KPrPage * m_activePageBeforePresentation;
    int m_zoomBeforePresentation;
    int m_tmpHorizHelpline;
    int m_tmpVertHelpline;
    double tmpHelpLinePosX;
    double tmpHelpLinePosY;

    bool m_keyPressEvent;
    bool m_drawSymetricObject;

    KoRect m_origBRect;  // Start rect for move
    QPoint m_origPos;  // Start point for move
};

#endif // __KPRCANVAS__
