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

#ifndef PAGE_H
#define PAGE_H


#include <qstring.h>
#include <qwidget.h>
#include <qcolor.h>
#include <qlist.h>
#include <qpicture.h>
#include <qvaluelist.h>

#include <qrect.h>
#include <qsize.h>
#include <qpixmap.h>
#include <qpoint.h>

#include <koQueryTrader.h>
#include <koprinter.h>

#include "global.h"
#include "ktextedit.h"
#include "kpbackground.h"

class KPresenterView;
class KPresenterDoc;
class QPainter;
class KTextEdit;
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
class KPrinter;
class KPPixmapObject;

/**********************************************************************/
/* class Page - There is ONE instance of this class for a given view  */
/* It manages the graphical representation of all the objects         */
/**********************************************************************/
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
    void setTextFont( const QFont & );
    void setTextColor( const QColor & );
    void setTextAlign( int );
    void setTextBold( bool b );
    void setTextItalic( bool b );
    void setTextUnderline( bool b );
    void setTextFamily( const QString &f );
    void setTextPointSize( int s );
    KTextEdit* kTxtObj();
    KPTextObject* kpTxtObj();

    // Start a screen presentation
    // All pages if curPgNum is -1, otherwise just curPgNum (1-based)
    void startScreenPresentation( float presFakt, int curPgNum = -1);
    void stopScreenPresentation();
    bool pNext( bool );
    bool pPrev( bool );

    unsigned int presPage() const { return currPresPage; }
    int presStep() const { return currPresStep; }
    float presFakt() const { return _presFakt; }
    int numPresSteps() const { return presStepList.count(); }
    int numPresPages() const { return slideList.count(); }

    bool canAssignEffect( QList<KPObject> &objs ) const;

    void keyPressEvent( QKeyEvent *e );

    void print( QPainter*, KPrinter*, float, float );

    void editSelectedTextArea();
    void setPartEntry( KoDocumentEntry& _e )
    { partEntry = _e; }
    void setToolEditMode( ToolEditMode _m, bool updateView = true );
    void setAutoForm( QString _autoform )
    { autoform = _autoform; }

    KTextEdit *haveASelectedTextObj();
    KPTextObject *haveASelectedKPTextObj();

    void drawPageInPix( QPixmap&, int );
    void drawPageInPix2( QPixmap&, int, int, float _zoom = 1.0 );

    void gotoPage( int pg );

public slots:
    void exitEditMode();

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
    const QList<KPBackGround> *backgroundList() const;
    const QList<KPObject> *objectList() const;
    unsigned int objNums();
    int diffx() const;
    int diffy() const;
    unsigned int currPgNum();
    unsigned int rastX() const;
    unsigned int rastY() const;
    QColor txtBackCol() const;
    bool spInfinitLoop() const;
    bool spManualSwitch() const;
    QRect getPageRect( unsigned int p, float fakt=1.0, bool decBorders = true );
    unsigned int pageNums();
    int getPageOfObj( int i, float fakt = 1.0 );
    float objSpeedFakt();
    float pageSpeedFakt();
    bool calcRatio( int &dx, int &dy, KPObject *kpobject, double ratio ) const;

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

    void scalePixmapToBeOrigIn( const QSize &origSize, const QSize &pgSize,
				const QSize &presSize, KPPixmapObject *obj );
    QSize getPixmapOrigSize( KPPixmapObject *&obj );
    void setTextBackground( KPTextObject *obj );

    // variables
    QPopupMenu *graphMenu, *picMenu, *txtMenu, *clipMenu, *presMenu;
    QPopupMenu *alignMenu1, *alignMenu2, *alignMenu3, *alignMenu4, *alignMenu5;
    QPopupMenu *pageMenu, *pieMenu, *rectMenu, *alignMenu6, *partMenu, *alignMenu7;
    QPopupMenu *picResizeMenu;
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
    int m_showOnlyPage; // 1-based (-1 = all)
    QValueList<int> presStepList, slideList;
    QValueList<int>::Iterator slideListIterator;
    int PM_DM, PM_SM;
    int firstX, firstY;
    int delPageId;
    bool drawRubber;
    QRect rubber, oldBoundingRect, oldRect;
    ToolEditMode toolEditMode;
    QRect insRect;
    KoDocumentEntry partEntry;
    // List of objects to draw, from the previous step in the presentation
    // (those that remain on screen between two steps)
    QList <KPObject> tmpObjs;
    QString autoform;
    bool inEffect, keepRatio;
    double ratio;
    QPixmap buffer;

private:
    QValueList<int> pages(const QString &range);
    bool pagesHelper(const QString &chunk, QValueList<int> &list);

public slots:
    void chPic();
    void chClip();

private slots:
    void toFontChanged( const QFont &font ) { emit fontChanged( font ); }
    void toColorChanged( const QColor &color ) { emit colorChanged( color ); }
    void toAlignChanged( int a ) { emit alignChanged( a ); }
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
    void duplicateCopy();
    void pageDelete();
    void pagePaste();
    void pageDefaultTemplate();
    void configPages();
    void presStructView();
    void slotGotoPage();
    void slotExitPres();
    void slotEditHF();
    void slotTextContents2Height();
    void slotTextObj2Contents();
    void picViewOrig640x480();
    void picViewOrig800x600();
    void picViewOrig1024x768();
    void picViewOrig1280x1024();
    void picViewOrig1600x1200();
    void picViewOrigFactor();

signals:

    // signals to notify of changes
    void fontChanged( const QFont & );
    void colorChanged( const QColor & );
    void alignChanged( int );
    void updateSideBarItem( int );
    void stopPres();

};
#endif //PAGE_H
