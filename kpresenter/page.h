/******************************************************************/
/* KPresenter - (c) by Reginald Stadlbauer 1997-1998              */
/* Version: 0.1.0                                                 */
/* Author: Reginald Stadlbauer                                    */
/* E-Mail: reggie@kde.org                                         */
/* Homepage: http://boch35.kfunigraz.ac.at/~rs                    */
/* needs c++ library Qt (http://www.troll.no)                     */
/* needs mico (http://diamant.vsb.cs.uni-frankfurt.de/~mico/)     */
/* needs OpenParts and Kom (weis@kde.org)                         */
/* written for KDE (http://www.kde.org)                           */
/* License: GNU GPL                                               */
/******************************************************************/
/* Module: Page (header)                                          */
/******************************************************************/

#ifndef PAGE_H
#define PAGE_H


#include <qstring.h>
#include <qwidget.h>
#include <qcolor.h>
#include <qlist.h>
#include <qpixmap.h>
#include <qpicture.h>
#include <qvaluelist.h>

#include <qrect.h>
#include <qsize.h>
#include <qpoint.h>

#include <koQueryTypes.h>

#include "global.h"
#include "ktextobject.h"
#include "kpbackground.h"

class KPresenterView;
class KPresenterDoc;
class QPainter;
class KTextObject;
class KPTextObject;
class KPObject;
class QPopupMenu;
class QEvent;
class QResizeEvent;
class QPaintEvent;
class QDragEnterEvent;
class QDragLeaveEvent;
class QDragMoveEvent;
class QDropEvent;
class QFocusEvent;
class QMouseEvent;
class QKeyEvent;
class QPainter;

#ifndef min
#define min(a,b) ((a)<(b)?(a):(b))
#endif

#ifndef max
#define max(a,b) ((a)>(b)?(a):(b))
#endif

/******************************************************************/
/* class Page - Page                                              */
/******************************************************************/
class Page : public QWidget
{
    Q_OBJECT

public:

    // constructor - destructor
    Page( QWidget *parent=0, const char *name=0, KPresenterView *_view=0 );
    ~Page();

    // public functions
    void draw( QRect, QPainter* );
    void selectAllObj();
    void deSelectAllObj();
    void selectObj( int num );
    void selectObj( KPObject* );
    void deSelectObj( int num );
    void deSelectObj( KPObject* );
    void setTextFont( QFont* );
    void setTextColor( QColor* );
    void setTextAlign( TxtParagraph::HorzAlign );
    KTextObject* kTxtObj();
    KPTextObject* kpTxtObj();

    void startScreenPresentation( bool, int );
    void stopScreenPresentation();
    bool pNext( bool );
    bool pPrev( bool );

    unsigned int presPage() {return currPresPage; }
    void setPresFakt( float f ) {_presFakt = f; }
    float presFakt() {return _presFakt; }

    bool canAssignEffect( int&, int& );

    void keyPressEvent( QKeyEvent *e );

    void print( QPainter*, QPrinter*, float, float );

    void editSelectedTextArea();
    void setPartEntry( KoDocumentEntry& _e )
    { partEntry = _e; }
    void setToolEditMode( ToolEditMode _m );
    void setAutoForm( QString _autoform )
    { autoform = _autoform; }

    KTextObject *haveASelectedTextObj();
    KPTextObject *haveASelectedKPTextObj();

    void drawPageInPix( QPixmap&, int );
    void drawPageInPix2( QPixmap&, int, int, float _zoom = 1.0 );

public slots:

    // public slots
    void clipCut();
    void clipCopy();
    void clipPaste();
    void deleteObjs();
    void rotateObjs();
    void shadowObjs();

protected:

    struct PicCache
    {
        QPicture pic;
        int num;
        int subPresStep;
    };

    // functions for displaying
    void paintEvent( QPaintEvent* );
    void paintBackground( QPainter*, QRect );
    void drawBackground( QPainter*, QRect );
    void drawObjects( QPainter*, QRect );
    void mousePressEvent( QMouseEvent *e );
    void mouseReleaseEvent( QMouseEvent *e );
    void mouseMoveEvent( QMouseEvent *e );
    void mouseDoubleClickEvent( QMouseEvent *e );
    void resizeEvent( QResizeEvent *e );
    int getObjectAt( int x, int y );
    void focusInEvent( QFocusEvent* ) {}
    void focusOutEvent( QFocusEvent* ) {}
    void enterEvent( QEvent *e );
    void leaveEvent( QEvent *e );
    void dragEnterEvent( QDragEnterEvent *e );
    void dragLeaveEvent( QDragLeaveEvent *e );
    void dragMoveEvent( QDragMoveEvent *e );
    void dropEvent( QDropEvent *e );

    // setup popupmenus
    void setupMenus();

    // get - set data
    QList<KPBackGround> *backgroundList();
    QList<KPObject> *objectList();
    unsigned int objNums();
    int diffx( int i = -1 );
    int diffy( int i = -1 );
    unsigned int currPgNum();
    unsigned int rastX();
    unsigned int rastY();
    QColor txtBackCol();
    bool spInfinitLoop();
    bool spManualSwitch();
    QRect getPageSize( unsigned int p, float fakt=1.0, bool decBorders = true );
    unsigned int pageNums();
    int getPageOfObj( int i, float fakt = 1.0 );
    float objSpeedFakt();
    float pageSpeedFakt();
    bool calcRatio( int &dx, int &dy, KPObject *kpobject, double ratio );
    
    void _repaint( bool erase=true );
    void _repaint( QRect r );
    void _repaint( KPObject *o );

    void drawPageInPainter( QPainter*, int, QRect );
    void changePages( QPixmap, QPixmap, PageEffect );
    void doObjEffects();
    void drawObject( KPObject*, QPixmap*, int, int, int, int, int, int );

    void insertText( QRect );
    void insertLineH( QRect, bool );
    void insertLineV( QRect, bool );
    void insertLineD1( QRect, bool );
    void insertLineD2( QRect, bool );
    void insertRect( QRect );
    void insertEllipse( QRect );
    void insertPie( QRect );
    void insertObject( QRect );
    void insertTable( QRect );
    void insertDiagramm( QRect );
    void insertFormula( QRect );
    void insertAutoform( QRect, bool );

    void selectNext();
    void selectPrev();

    // variables
    QPopupMenu *graphMenu, *picMenu, *txtMenu, *clipMenu, *presMenu;
    QPopupMenu *alignMenu1, *alignMenu2, *alignMenu3, *alignMenu4, *alignMenu5;
    QPopupMenu *pageMenu, *pieMenu, *rectMenu, *alignMenu6, *partMenu, *alignMenu7;
    bool mousePressed;
    ModifyType modType;
    unsigned int oldMx, oldMy;
    int resizeObjNum, editNum;
    bool fillBlack;
    KPresenterView *view;
    bool editMode, goingBack, drawMode;
    unsigned int currPresPage, currPresStep, subPresStep;
    unsigned int oldPresPage, oldPresStep, oldSubPresStep;
    float _presFakt;
    QValueList<int> presStepList, slideList;
    QValueList<int>::Iterator slideListIterator;
    int PM_DM, PM_SM;
    int firstX, firstY;
    bool drawRubber;
    QRect rubber, oldBoundingRect, oldRect;
    ToolEditMode toolEditMode;
    QRect insRect;
    KoDocumentEntry partEntry;
    QList <KPObject> tmpObjs;
    QString autoform;
    bool inEffect, keepRatio;
    double ratio;
    
public slots:
    void chPic();
    void chClip();

private slots:

    // slots to react on changes
    void toFontChanged( QFont* font ) { emit fontChanged( font ); }
    void toColorChanged( QColor* color ) { emit colorChanged( color ); }
    void toAlignChanged( TxtParagraph::HorzAlign a ) { emit alignChanged( a ); }
    void objProperties();
    void objConfigPie();
    void objConfigRect();
    void assignEffect();
    void drawingMode();
    void switchingMode();
    void alignObjLeft();
    void alignObjCenterH();
    void alignObjRight();
    void alignObjTop();
    void alignObjCenterV();
    void alignObjBottom();
    void pageLayout();
    void pageBackground();
    void pageInsert();
    void pageCopy();
    void pageDelete();
    void pagePaste();
    void configPages();
    void presStructView();
    void slotGotoPage();
    void slotExitPres();
    void slotEditHF();
    void slotTextContents2Height();
    void slotTextObj2Contents();

signals:

    // signals to notify of changes
    void fontChanged( QFont* );
    void colorChanged( QColor* );
    void alignChanged( TxtParagraph::HorzAlign );
    void stopPres();

};
#endif //PAGE_H



