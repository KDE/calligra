// -*- Mode: c++; c-basic-offset: 4; indent-tabs-mode: nil; tab-width: 4; -*-
/* This file is part of the KDE project
   Copyright (C) 1998, 1999 Reginald Stadlbauer <reggie@kde.org>
   Copyright (C) 2002-2005 Thorsten Zachmann <zachmann@kde.org>

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
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
*/


#ifndef __KPRCANVAS__
#define __KPRCANVAS__

#include <QWidget>
#include <q3ptrlist.h>
#include <q3picture.h>
#include <q3valuelist.h>
#include <qpixmap.h>
#include <q3pointarray.h>
#include <q3valuevector.h>
#include <qtimer.h>
//Added by qt3to4:
#include <QKeyEvent>
#include <QDragEnterEvent>
#include <QDragMoveEvent>
#include <QDropEvent>
#include <QResizeEvent>
#include <QFocusEvent>
#include <QMouseEvent>
#include <QEvent>
#include <QPaintEvent>
#include <QWheelEvent>

#include <KoGuides.h>
#include <KoRuler.h>
#include <KoQueryTrader.h>
#include "KoPointArray.h"
#include "global.h"
#include <KoRect.h>
#include <KoTextFormat.h>
#include "presstep.h"

class KUrl;
class KoTextFormatInterface;
class KPrView;
class KPrDocument;
class QPainter;
class KTextEdit;
class KPrTextObject;
class KPrObject;
class KMenu;
class QResizeEvent;
class QPaintEvent;
class QDragEnterEvent;
class QDragMoveEvent;
class QDropEvent;
class QFocusEvent;
class QMouseEvent;
class QWheelEvent;
class QKeyEvent;
class KPrPixmapObject;
class KPrBackGround;
class QRect;
class QSize;
class QPoint;
class KPrSoundPlayer;
class KoTextObject;
class KoParagCounter;
class KPrPage;
class KPrinter;
class KPrTextView;
class KPrPartObject;
class KCommand;
class KPrEffectHandler;
class KPrPageEffects;
/**
 * Class KPCanvas - There is a single instance of this class for a given view.
 *
 * It manages the graphical representation of all the objects.
 */
class KPrCanvas : public QWidget
{
    Q_OBJECT

public:

    /// constructor
    KPrCanvas( QWidget *parent=0,const char *name=0,KPrView *_view=0 );
    /// destructor
    ~KPrCanvas();

    KPrView * getView()const { return m_view; }

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
    void selectObj( KPrObject* );
    void deSelectObj( KPrObject* );
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
    KPrTextObject* kpTxtObj() const;

    /**
     * Start a screen presentation
     * All pages if @p curPgNum is -1, otherwise just @p curPgNum (1-based)
     */
    void startScreenPresentation( double zoomX, double zoomY, int curPgNum = -1);
    void stopScreenPresentation();

    /**
     * Go to the next step in the presentation.
     * gotoNextPage: if is set to true go to beginning of the
     *               next page.
     */
    bool pNext( bool gotoNextPage = false );

    /**
     * Go to previous step of the presentation.
     * gotoPreviousPage: if is set to true go to beginning of the
     *                   previous page.
     */
    bool pPrev( bool gotoPreviousPage = false );

    /// returns the current page of the presentation 1 based
    unsigned int presPage() const { return m_step.m_pageNumber + 1; }
    /// returns the current step of the presentation
    int presStep() const { return m_step.m_step; }
    int numPresSteps() const { return m_pageEffectSteps.count(); }
    int numPresPages() const { return m_presentationSlides.count(); }

    bool canAssignEffect( Q3PtrList<KPrObject> &objs ) const;

    void print( QPainter*, KPrinter*, float, float );


    void setPartEntry( KoDocumentEntry& _e )
        { partEntry = _e; }
    void setToolEditMode( ToolEditMode _m, bool updateView = true );
    void setAutoForm( const QString &_autoform )
        { autoform = _autoform; }

    /**
    \brief Draw page into QPixmap.

    Draws page pgnum of the currently open presentation into a QPixmap
    using the specified zoom factor (or fixed width/height dimensions, resp.)

    Set forceWidth and/or forceHeight to override the zoom factor
    and obtain a pixmap of the specified width and/or height.
    By omitting one of them you make sure that the aspect ratio
    of your page is used for the resulting image.

    \sa exportPage
    */
    void drawPageInPix( QPixmap&, int pgnum, int zoom,
                        bool forceRealVariableValue = false,
                        int forceWidth  = 0,
                        int forceHeight = 0 );

    /**
     \brief Save page to bitmap file.

     Export a page of the currently open presentation to disk
     using a bitmap format like e.g. PNG.
     This method uses a QPixmap::save() call.

     \param nPage the internally used <b>0-based</b> page number
     \param nWidth the desired image width in px
     \param nHeight the desired image height in px
     \param fileURL the URL of the image file to be created: if this
       does not point to a local file a KTempFile is created by QPixmap::save()
       which is then copied over to the desired location.
     \param format the format of the image file (see QPixmap::save())
      \param quality the quality of the image (see QPixmap::save())

     example:
\verbatim
exportPage( 0, s, 800, 600, "/home/khz/page0.png", "PNG", 100 );
\endverbatim
     \returns True if the file was written successfully.

     \sa drawPageInPix, KPrViewIface::exportPage
     */
    bool exportPage( int nPage, int nWidth, int nHeight,
                     const KUrl& fileURL,
                     const char* format,
                     int quality = -1 );

    void gotoPage( int pg );

    /**
     * Go to first slide of presentation.
     * Used in presentation mode.
     */
    void presGotoFirstPage();

    KPrPage* activePage() const;

    bool oneObjectTextExist() const;
    bool oneObjectTextSelected() const;
    bool isOneObjectSelected() const;
    /** Returns the list of selected text objects */
    Q3PtrList<KPrTextObject> selectedTextObjs() const;
    /**
     * Returns the list of interfaces to use for changing the text format.
     * This can be either the currently edited text object,
     * or the list of text objects currently selected
     */
    Q3PtrList<KoTextFormatInterface> applicableTextInterfaces() const;
    /**
     * Returns the list of text objects, either selected or activated.
     */
    Q3PtrList<KPrTextObject> applicableTextObjects() const;

    void setMouseSelectedObject(bool b);

    bool haveASelectedPartObj() const;
    bool haveASelectedGroupObj() const;
    bool haveASelectedPixmapObj() const;

    KPrTextView *currentTextObjectView() const { return m_currentTextObjectView; }

    Q3PtrList<KPrObject> objectList() const;

    // get - set data
    const Q3PtrList<KPrObject> & getObjectList() const;

    void playSound( const QString &soundFileName );
    void stopSound();

    ///for KPrTextView
    void dragStarted() { mousePressed = false; }

    void setXimPosition( int x, int y, int w, int h, QFont *f );
    void createEditing( KPrTextObject *textObj );


    /**
     * @brief Add a line
     *
     * @param startPoint the start point of the line
     * @param endPoint the end point of the line
     */
    void insertLine( const KoPoint &startPoint, const KoPoint &endPoint );
    
    /**
     * @brief Add a rectangle
     *
     * @param rect the size and position of the rectangle
     */
    void insertRect( const KoRect &rect);

    /**
     * @brief Add a ellipse
     *
     * @param rect the size and position of the ellipse
     */
    void insertEllipse( const KoRect &rect );

    /**
     * @brief Add a pie/arc/chord
     *
     * @param rect the size and position of the pie/arc/chord
     */
    void insertPie( const KoRect &rect );

    /**
     * @brief Add a text object
     *
     * @param rect the size and position of the text object
     */
    KPrTextObject* insertTextObject( const KoRect &rect );

    /**
     * @brief Add a picture
     *
     * @param rect the size and position of the picture
     */
    void insertPicture( const KoRect &rect );
    void changePicture( const KUrl & url, QWidget *window );

    int numberOfObjectSelected() const;
    KPrObject *getSelectedObj() const;
    KPrPixmapObject *getSelectedImage() const;

    bool getEditMode() const { return editMode; }

    void ungroupObjects();
    void groupObjects();

    unsigned int objNums() const;

    void ensureVisible( int x, int y, int xmargin, int ymargin );

    /**
     * @brief get the rect for the objects
     *
     * @param all true if the rect for all objects shoud be returned
     *        false if only the rect for selected objects sould be returned
     *
     * @return rect of the objects
     */
    KoRect objectRect( bool all ) const;

    void scrollTopLeftPoint( const QPoint & pos );

    void textContentsToHeight();
    void textObjectToContents();

    void flipObject( bool _horizontal );

    void lowerObjs( bool backward );
    void raiseObjs( bool forward );

    /**
     * @brief Align objects
     *
     * This will align all selected non protected objects according to the 
     * align type at.
     *
     * @param at the align type
     */
    void alignObjects( AlignType at );

    bool canMoveOneObject()const;
    Q3PtrList<KPrTextObject> listOfTextObjs() const;

    QPixmap getPicturePixmap() const;
    void closeObject(bool _close);
    void layout();
    void alignVertical( VerticalAlignmentType _type );
    void savePicture();

    void dropImage( QMimeSource * data, bool resizeImageToOriginalSize = false , int posX = 10, int posX=10 );
    KoGuides & guideLines() { return m_gl; }

    /**
     * @brief Open the object popup
     *
     * @param object the selected object
     * @param point where the popup should be displayed
     */
    void objectPopup( KPrObject *object, const QPoint &point );

public slots:
    // use repaint = false when the KPrCanvas is destroyed to avaid a possible crash
    void exitEditMode( bool repaint = true );

    void clipCut();
    void clipCopy();
    void clipPaste();
    void deleteObjs();
    void copyObjs();
    void copyOasisObjs();

    void chPic();
    void picViewOriginalSize();
    void picViewOrig640x480();
    void picViewOrig800x600();
    void picViewOrig1024x768();
    void picViewOrig1280x1024();
    void picViewOrig1600x1200();
    void picViewOrigFactor();
    void setActivePage( KPrPage* active );

    /**
     * @brief Set the guide move state
     *
     * @param state true set guide move state
     *              false unset guide move state
     */
    void setPaintGuides( bool state );

signals:

    // signals to notify of changes
    void fontChanged( const QFont & );
    void colorChanged( const QColor & );
    void alignChanged( int );
    /**
     * Emmitted when presentation should be restarted
     * This is used in automatic screen and infinite loop mode.
     */
    void stopAutomaticPresentation();
    /**
     * Emmitted when presentation should be restarted
     * This is used in automatic screen and infinite loop mode.
     */
    void restartPresentation();
    void objectSelectedChanged();
    void objectSizeChanged();
    /// Emitted when the current frameset edit changes
    void currentObjectEditChanged();

    void selectionChanged( bool hasSelection );
    void sigMouseWheelEvent( QWheelEvent * );

protected:
    struct PicCache
    {
        Q3Picture pic;
        int num;
        int subPresStep;
    };

    // functions for displaying
    /**
     * This method is used for repainting the canvas.
     */
    virtual void paintEvent( QPaintEvent* );

    /**
     * Draws the background of page to painter.
     * Set edit to true if in edit mode, false in presentation mode.
     */
    void drawBackground( QPainter* painter, const QRect& rect, KPrPage * page, bool edit = false ) const;

    /**
     * Draw obj to painter.
     */
    void drawAllObjectsInPage( QPainter *painter, const Q3PtrList<KPrObject> & obj, int pageNum ) const;

    /**
     * Draw _objects shown at step to painter.
     * This method is used for presentation mode, printing.
     */
    void drawObjectsPres( QPainter *painter, const Q3PtrList<KPrObject> &_objects, PresStep step ) const;

    /**
     * Draw _objects to painter.
     * Only rect is painted.
     * This method is used for edit mode.
     */
    void drawObjectsEdit( QPainter *painter, const KoRect &rect, const Q3PtrList<KPrObject> &_objects,
                          SelectionMode selectionMode, int page ) const;

    /**
     * Draw _objects to painter.
     * contour if true only countor of selected objects is drawn
     * selectionMode selected objects use this selection mode
     * textView if set print editable textobject (used by drawObjectsEdit)
     * This method is used by drawObjectsPres and drawObjectsEdit.
     */
    void drawObjects( QPainter *painter, const Q3PtrList<KPrObject> &objects, SelectionMode selectionMode,
                      bool contour, KPrTextView * textView, int pageNum ) const;

    /**
     * Draw _objects of page to painter.
     * This method is used for edit mode.
     */
    void drawEditPage( QPainter *painter, const QRect &_rect,
                       KPrPage *page, SelectionMode selectionMode ) const;

    /**
     * Draw _objects of page shown at step to painter.
     * This method is used for presentation mode, printing.
     */
    void drawPresPage( QPainter *painter, const QRect &_rect, PresStep step ) const;

    /// draw grid
    void drawGrid(QPainter *painter, const QRect &rect2) const;

    /**
     * Finish the object effects.
     * This shown the last step of the effect. It stops the effect timer and
     * disconnect it and the effect handler deleted.
     */
    bool finishObjectEffects();

    bool finishPageEffect( bool cancel = false );

    void drawCurrentPageInPix( QPixmap& ) const;
    virtual void mousePressEvent( QMouseEvent *e );
    virtual void mouseReleaseEvent( QMouseEvent *e );
    virtual void mouseMoveEvent( QMouseEvent *e );
    virtual void mouseDoubleClickEvent( QMouseEvent *e );
    virtual void wheelEvent( QWheelEvent *e );
    virtual void resizeEvent( QResizeEvent *e );
    /**
     * Returns the object at position pos
     * @param pos the position in the document on which to look
     * @param withoutProtected if set to true only unprotecred objects are returned
     * @return the object at position pos
     */
    KPrObject *getObjectAt( const KoPoint &pos, bool withoutProtected = false );
    virtual void focusInEvent( QFocusEvent* ) {}
    virtual void focusOutEvent( QFocusEvent* ) {}
    virtual void enterEvent( QEvent *e );
    virtual void leaveEvent( QEvent *e );
    virtual void dragEnterEvent( QDragEnterEvent *e );
    virtual void dragMoveEvent( QDragMoveEvent *e );
    virtual void dropEvent( QDropEvent *e );
    virtual void keyPressEvent( QKeyEvent *e );
    virtual void keyReleaseEvent( QKeyEvent *e );
    virtual void imStartEvent( QIMEvent * e );
    virtual void imComposeEvent( QIMEvent * e );
    virtual void imEndEvent( QIMEvent * e );

    void eraseEmptySpace( QPainter * painter, const QRegion & emptySpaceRegion, const QBrush & brush ) const;

    /// setup popupmenus
    void setupMenus();

    unsigned int currPgNum() const;
    QColor txtBackCol() const;
    bool spInfiniteLoop() const;
    bool spManualSwitch() const;
    QRect getPageRect( bool decBorders = true ) const;
    unsigned int pageNums() const;

    void _repaint( bool erase=true );
    void _repaint( const QRect &r );
    void _repaint( KPrObject *o );

    /**
     * Draw page defined in step to painter.
     * This method is used for printing a presentation.
     */
    void printPage( QPainter*, PresStep step, KPrinter *printer, int rows, int cols );

    /**
     * This method animates the objects in the presentation.
     */
    void doObjEffects( bool isAllreadyPainted = false );

    /**
     * @brief Add a object
     *
     * @param rect the size and position of the object
     */
    KPrPartObject *insertObject( const KoRect &rect );

    /**
     * @brief Add a autoform object
     *
     * @param rect the size and position of the autoform object
     */
    void insertAutoform( const KoRect &rect );
    void insertFreehand( const KoPointArray &_pointArray );
    void insertPolyline( const KoPointArray &_pointArray );
    void insertCubicBezierCurve( const KoPointArray &_pointArray );
    void insertPolygon( const KoPointArray &_pointArray );
    void insertClosedLine( const KoPointArray &_pointArray );

    void selectNext();
    void selectPrev();

    void scalePixmapToBeOrigIn( const KoSize &currentSize, const KoSize &pgSize,
                                const QSize &presSize, KPrPixmapObject *obj );
    bool getPixmapOrigAndCurrentSize( KPrPixmapObject *&obj, KoSize *origSize, KoSize *currentSize );
    void setTextBackground( KPrTextObject *obj );
    virtual bool eventFilter( QObject *o, QEvent *e );
    virtual bool focusNextPrevChild( bool );

    void endDrawPolyline();

    /**
     * @brief Draw a cubic bezier curve.
     */ 
    void drawCubicBezierCurve( QPainter &p, KoPointArray &points );
    //void drawCubicBezierCurve( int _dx, int _dy );
    void endDrawCubicBezierCurve();

    /**
     * @brief Redraw the curve defined in m_pointArray
     *
     * @param p painter used for drawing
     */
    void redrawCubicBezierCurve( QPainter &p );


#ifndef NDEBUG
    void printRTDebug( int info );
#endif

    KoRect getAlignBoundingRect() const;

private:
    void picViewOrigHelper(int x, int y);

    /**
     * @brief Move object by key
     *
     * @param x The distance to move the objects in x direction
     * @param y The distance to move the objects in y direction
     */
    void moveObjectsByKey( int x, int y );

    /**
     * @brief Move object by mouse
     *
     * @param pos The position of the mouse
     * @param keepXorYunchanged if true keep x or y position unchanged
     */
    void moveObjectsByMouse( KoPoint &pos, bool keepXorYunchanged );

    //---- stuff needed for resizing ----
    /// resize the m_resizeObject
    void resizeObject( ModifyType _modType, const KoPoint & point, bool keepRatio, bool scaleAroundCenter );
    /// create KPrResizeCmd
    void finishResizeObject( const QString &name, bool layout = true );

    /**
     * @brief Display object above the other objects in editiong mode
     *
     * This is used to bring a single slected object to front, so it is easier 
     * to modify.
     *
     * @param object which should be displayed above the other objects
     */
    void raiseObject( KPrObject *object );
    /**
     * @brief Don't display an object above the others
     */
    void lowerObject();
    /**
     * @brief Get the list of objects in the order they should be displayed.
     *
     * This takes into acount the object set in raiseObject so that it is 
     * the last one in the list returned (the one that is displayed above all
     * the others).
     *
     * @return List of objects
     */
    const Q3PtrList<KPrObject> displayObjectList() const; 

    /**
     * This is used in automatic presentation mode.
     * If it is set to true and we are in automatic presentation
     * mode the timer of the page will be activated after the last
     * effect has been shown.
     */
    bool m_setPageTimer;

    /// Helper function to draw a polygon
    void drawPolygon( QPainter &p, const KoRect &rect );

    /// Helper function to draw a pie/arc/chord
    void drawPieObject( QPainter *p,  const KoRect &rect );

    QPoint limitOfPoint(const QPoint& _point) const;

    /**
     * Return true if object is a header/footer which is hidden.
     */
    bool objectIsAHeaderFooterHidden(KPrObject *obj) const;

    KPrTextObject* textUnderMouse( const QPoint & point );
    bool checkCurrentTextEdit( KPrTextObject * textObj );

    /**
     * @brief Snap @p pos to grid and guide lines.
     *
     * This method looks if the @p pos has to be snapped to a guide or grid or both. 
     * If so it returns the position it has snapped to.
     *
     * @param pos the point which should be snapped
     * @param repaintSnapping true if the guides should be repainted.
     * @param repaintSnapping false when the guides should not be repainted.
     *        You have to call repaint by yourself.
     *
     * @return the position of the snapped point
     */
    KoPoint snapPoint( KoPoint &pos, bool repaintSnapping = true );

    /**
     * @brief Find the next grid in distance
     *
     * Allways the top left corner is snapped to the grid.
     *
     * @param rect The rect which should be snapped to the grid.
     * @param diffx The range in x distance in which the grid has to be.
     * @param diffy The range in y distance in which the grid has to be.
     *
     * @return the distance to the guide or (  0, 0 ) if there is no guide to snap to.
     */
    KoPoint diffGrid( KoRect &rect, double diffx, double diffy );

    void recalcAutoGuides( );

    /**
     * @brief Calculate the points in distance to the top left corner
     *
     * @param pointArray the points in coordinate position
     */
    KoPointArray getObjectPoints( const KoPointArray &pointArray );


    /**
     * @brief Update m_insertRect 
     *
     * This will set the bottom right corner of the insert rect to
     * the given point. If the AltButton is pressed the insert rect 
     * is moved with its bottom right to the given point.
     *
     * @param point the bottom right point of the inserted object
     * @param The state of the mouse and modifier buttons
     *
     */
    void updateInsertRect( const KoPoint &point, Qt::ButtonState state );

    /**
     * @brief Popup context menu
     *
     * Popup context menu for current active or selected object,
     * otherwise popup context menu for page.
     */
    void popupContextMenu();

private slots:
    void toFontChanged( const QFont &font ) { emit fontChanged( font ); }
    void toColorChanged( const QColor &color ) { emit colorChanged( color ); }
    void toAlignChanged( int a ) { emit alignChanged( a ); }
    /**
     * Switch to drawing mode. In drawing mode it is possible to draw on the
     * current slide.
     * Only to be used in presentation mode.
     */
    void setDrawingMode();
    /**
     * Set switching mode. This ends drawing mode and goes back to
     * stepping between the pages.
     * If continueTimer is true the autoPres timer is continued
     * Only to be used in presentation mode.
     */
    void setSwitchingMode( bool continueTimer = true );
    void slotGotoPage();
    void slotExitPres();
    void terminateEditing( KPrTextObject * );

    /**
     * Do the next step of the object effect.
     * This restarts the effect tmer. If the effects are
     * completed the timer is disconnected and the effect handler
     * deleted.
     */
    void slotDoEffect();

    void slotDoPageEffect();

    /**
     * Scroll canvas when receiving this signal
     */
    void slotAutoScroll(const QPoint &scrollDist);

private:
    /// Distance for snapping to guide line with mouse 
    static const int MOUSE_SNAP_DISTANCE;
    /// Distance for snapping to guide line with key 
    static const int KEY_SNAP_DISTANCE;
    // variables
    /// Popup menu used in presentation mode.
    KMenu *m_presMenu;
    bool showingLastSlide;
    bool mousePressed;
    bool drawContour;
    ModifyType modType;

    /// text or part object when it is edited
    KPrObject * m_editObject;

    bool fillBlack;
    KPrView *m_view;
    bool editMode, goingBack;
    /**
     * True when we are in drawing mode.
     * False when we are in switching mode.
     */
    bool m_drawMode;
    /**
     * True if the a line should be drawn.
     * Used in drawing mode.
     */
    bool m_drawLineInDrawMode;

    /**
     * Save the lines drawed in drawMode.
     * This is used for paint events.
     * Used in drawing mode.
     */
    Q3ValueVector<Q3PointArray> m_drawModeLines;

    /**
     * Index into the QPointArray for the next point of the line in draw mode.
     * Used in drawing mode.
     */
    int m_drawModeLineIndex;

    bool mouseSelectedObject;
    /// information about current step of the presentation
    PresStep m_step;
    float _presFakt;
    int m_showOnlyPage; // 1-based (-1 = all)
    /// list of all effect steps occuring on the active page
    Q3ValueList<int> m_pageEffectSteps;
    /// List of the slides used in the presentation
    Q3ValueList<int> m_presentationSlides;
    /// Iterator over the slides of a presentation
    Q3ValueList<int>::Iterator m_presentationSlidesIterator;
    /// KPrEffectHandler for object effects
    KPrEffectHandler *m_effectHandler;

    KPrPageEffects *m_pageEffect;

    /// EffectTimer
    QTimer m_effectTimer;

    QTimer m_pageEffectTimer;

    /// menu identifier for draw mode
    int PM_DM;
    int delPageId;
    bool drawRubber;
    /// rect for drawing rubber
    KoRect m_rubber;

    //---- stuff needed for resizing ----
    /// object which gets resized
    KPrObject *m_resizeObject;
    /// size of the object at when resizing is started
    KoRect m_rectBeforeResize;
    /// ratio of the object ( width / height )
    double m_ratio;
    bool m_isResizing;

    //---- stuff needed for moving ----
    bool m_isMoving;
    KoPoint m_moveStartPoint;

    //---- stuff needed for rotating ----
    /// object which gets rotated
    KPrObject *m_rotateObject;
    /// center of the rotated object
    KoPoint m_rotateCenter;
    /// start point for rotation
    KoPoint m_rotateStart;
    /// angle of the object at start of rotate
    double m_angleBeforeRotate;

    ToolEditMode toolEditMode;
    /// The rect of the object during insert.
    KoRect m_insertRect;
    KoDocumentEntry partEntry;
    QString autoform;
    QPixmap buffer;

    KPrTextView *m_currentTextObjectView;

    KPrSoundPlayer *soundPlayer;

    KoPointArray m_pointArray, m_oldCubicBezierPointArray;
    QPoint m_dragStartPoint, m_dragEndPoint, m_dragSymmetricEndPoint;
    /// The start position of an insert for line objects
    KoPoint m_startPoint;
    /// The end position of an insert for line objects
    KoPoint m_endPoint;
    KoPoint m_symmetricEndPoint;
    KoPoint m_CubicBezierSecondPoint, m_CubicBezierThirdPoint;
    int m_indexPointArray;
    bool m_drawPolyline;
    bool m_drawCubicBezierCurve;
    bool m_drawLineWithCubicBezierCurve;
    bool m_zoomRubberDraw;

    KPrPage *m_activePage;
    int m_xOffset, m_yOffset;
    int m_xOffsetSaved, m_yOffsetSaved; // saved when going fullscreen

    /// Start position for move with mouse
    KoPoint m_moveStartPosMouse;
    /// This holds the distance it an object was snapped to a guide
    KoPoint m_moveSnapDiff;
    /// Set to true if snapping should be disabled (by pressing shift during move)
    bool m_disableSnapping;
    /// The last position of the mouse during moving
    KoPoint m_origMousePos;
    /// start position for move with key
    KoPoint m_moveStartPosKey; 

    KPrPage * m_activePageBeforePresentation;
    int m_zoomBeforePresentation;

    bool m_keyPressEvent;
    bool m_drawSymetricObject;

    /// guides
    KoGuides m_gl;
    /// true if we are in guide move state, false otherwise
    bool m_paintGuides;

    /// object which is selected and should be shown above all the other objects
    KPrObject * m_objectDisplayAbove;

    /// Previously spoken text object.
    KPrTextObject *m_prevSpokenTO;
};

#endif // __KPRCANVAS__
