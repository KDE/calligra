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

#ifndef kpresenter_view_h
#define kpresenter_view_h

#include <qguardedptr.h>

#include <koView.h>
#include <kpresenter_doc.h>
#include <global.h>
#include <searchdia.h>

class DCOPObject;
class KPresenterView;
class BackDia;
class Page;
class KoRuler;
class QScrollBar;
class AFChoose;
class StyleDia;
class PgConfDia;
class RotateDia;
class ShadowDia;
class KPPresStructView;
class ConfPieDia;
class ConfRectDia;
class QToolButton;
class SideBar;

class KAction;
class KToggleAction;

class PageBase : public QWidget
{
public:
    PageBase( QWidget *parent, KPresenterView *v ) : QWidget( parent ), view( v ) {}
    void resizeEvent( QResizeEvent *e );

private:
    KPresenterView *view;

};


/*****************************************************************/
/* class KPresenterView						 */
/*****************************************************************/
class KPresenterView : public KoView
{
    friend class PageBase;
    Q_OBJECT

public:

    // ------ C++ ------
    // constructor - destructor
    KPresenterView( KPresenterDoc* _doc, QWidget *_parent = 0, const char *_name = 0 );
    ~KPresenterView();

    void initGui();
    virtual DCOPObject* dcopObject();

    // for dcop interface
    int getCurrentPresPage();
    int getCurrentPresStep();
    int getPresStepsOfPage();
    int getNumPresPages();
    bool gotoPresPage( int pg );
    float getCurrentFaktor();

    virtual void setupPrinter( KPrinter &printer );
    virtual void print( KPrinter &printer );

signals:
    void currentPageChanged( int );


public slots:
    // edit menu
    void editUndo();
    void editRedo();
    void editCut();
    void editCopy();
    void editPaste();
    void editDelete();
    void editSelectAll();
    void editCopyPage();
    void editDuplicatePage();
    void editDelPage();
    void editFind();
    void editHeaderFooter();

    // view menu
    void viewShowSideBar();

    // insert menu
    void insertPage();
    void insertPicture();
    void insertPicture(const QString &file);
    void insertClipart();

    // tools menu
    void toolsMouse();
    void toolsLine();
    void toolsRectangle();
    void toolsCircleOrEllipse();
    void toolsPie();
    void toolsText();
    void toolsAutoform();
    void toolsDiagramm();
    void toolsTable();
    void toolsFormula();
    void toolsObject();

    // extra menu
    void extraPenBrush();
    void extraConfigPie();
    void extraConfigRect();
    void extraRaise();
    void extraLower();
    void extraRotate();
    void extraShadow();
    //    void extraAlignObj();
    void extraBackground();
    void extraLayout();
    void extraConfigure();
    void extraLineBegin();
    void extraLineEnd();
    void extraWebPres();
    void extraCreateTemplate();
    void extraDefaultTemplate();
    void extraGroup();
    void extraUnGroup();

    void extraAlignObjLeft();
    void extraAlignObjCenterH();
    void extraAlignObjRight();
    void extraAlignObjTop();
    void extraAlignObjCenterV();
    void extraAlignObjBottom();

    void extraAlignObjs();

    // screen menu
    void screenConfigPages();
    void screenPresStructView();
    void screenAssignEffect();
    void screenStart();
    void screenViewPage();
    void screenStop();
    void screenPause();
    void screenFirst();
    void screenPrev();
    void screenNext();
    void screenLast();
    void screenSkip();
    void screenPenColor();
    void screenPenWidth( const QString &w );

    // text toolbar
    void sizeSelected();
    void fontSelected();
    void textBold();
    void textItalic();
    void textUnderline();
    void textColor();
    void textAlignLeft();
    void textAlignCenter();
    void textAlignRight();
    void mtextFont();
    void textEnumList();
    void textUnsortList();
    void textNormalText();
    void textDepthPlus();
    void textDepthMinus();
    void textSettings();
    void textContentsToHeight();
    void textObjectToContents();
    void textInsertPageNum();

    // color bar
    void penChosen( const QColor &c );
    void brushChosen( const QColor &c );

    void skipToPage( int _num );
    void nextPage();
    void prevPage();

    /**
     * Update a given item in the sidebar
     */
    void updateSideBarItem( int pagenr );


    void objectSelectedChanged();

    void extraChangeClip();
    void renamePageTitle();

public:
    // create GUI
    virtual void createGUI();

    // get - set offsets
    int getDiffX() const { return xOffset; }
    int getDiffY() const { return yOffset; }
    void setDiffX( int _x ) {xOffset = _x; }
    void setDiffY( int _y ) {yOffset = _y; }

    // get current pagenum, 1-based
    unsigned int getCurrPgNum();

    // return pointer to document
    class KPresenterDoc *kPresenterDoc() const {return m_pKPresenterDoc; }

    // repaint page
    void repaint( bool );
    void repaint( unsigned int, unsigned int, unsigned int, unsigned int, bool );
    void repaint( QRect, bool );

    // properties
    void changePicture( const QString & );
    void changeClipart( const QString & );

    Page* getPage() {return page; }

    void changeUndo( QString, bool );
    void changeRedo( QString, bool );

    void setRulerMouseShow( bool _show );
    void setRulerMousePos( int mx, int my );

    // set scrollbar ranges
    void setRanges();

    KoRuler *getHRuler() { return h_ruler; }
    KoRuler *getVRuler() { return v_ruler; }
    QScrollBar *getHScrollBar() { return horz; }
    QScrollBar *getVScrollBar() { return vert; }

    void makeRectVisible( QRect _rect );

    void restartPresStructView();

    PieType getPieType() { return pieType; }
    int getPieAngle() { return pieAngle; }
    int getPieLength() { return pieLength; }
    QPen getPen() { return pen; }
    QBrush getBrush() { return brush; }
    LineEnd getLineBegin() {return lineBegin; }
    LineEnd getLineEnd() {return lineEnd; }
    QColor getGColor1() {return gColor1; }
    QColor getGColor2() {return gColor2; }
    BCType getGType() {return gType; }
    FillType getFillType() {return fillType; }
    bool getGUnbalanced() { return gUnbalanced; }
    int getGXFactor() { return gXFactor; }
    int getGYFactor() { return gYFactor; }

    void setTool( ToolEditMode toolEditMode );

    int getRndX() { return rndX; }
    int getRndY() { return rndY; }

    QFont &currFont() { return tbFont; }
    QColor &currColor() { return tbColor; }

    void enableWebPres();

    /**
     * Overloaded from View
     */
    bool doubleClickActivation() const;
    /**
     * Overloaded from View
     */
    QWidget* canvas();
    /**
     * Overloaded from View
     */
    int canvasXOffset() const;
    /**
     * Overloaded from View
     */
    int canvasYOffset() const;

    /**
     * Rebuild the whole sidebar
     */
    void updateSideBar();

    void refreshPageButton();


    void openPopupMenuMenuPage( const QPoint & _point );
    void openPopupMenuTextObject( const QPoint & _point );
    void openPopupMenuPartObject( const QPoint & _point );
    void openPopupMenuRectangleObject( const QPoint & _point );
    void openPopupMenuGraphMenu(const QPoint & _point );
    void openPopupMenuPieObject( const QPoint & _point );
    void openPopupMenuClipObject(const QPoint & _point);
    void openPopupMenuSideBar(const QPoint & _point);
    
protected slots:
    // dialog slots
    void backOk( bool );
    void afChooseOk( const QString & );
    void slotAfchooseCanceled();
    void styleOk();
    void pgConfOk();
    void effectOk();
    void rotateOk();
    void shadowOk();
    void psvClosed();
    void confPieOk();
    void confRectOk();

    // scrolling
    void scrollH( int );
    void scrollV( int );

    // textobject
    void fontChanged( const QFont & );
    void colorChanged( const QColor & );
    void alignChanged( int );

    void extraLineBeginNormal();
    void extraLineBeginArrow();
    void extraLineBeginRect();
    void extraLineBeginCircle();
    void extraLineEndNormal();
    void extraLineEndArrow();
    void extraLineEndRect();
    void extraLineEndCircle();

    void stopPres() {continuePres = false; }
    void newPageLayout( KoPageLayout _layout );
    void openPageLayoutDia() { extraLayout(); }
    void unitChanged( QString );

    void search();

protected:

// ********* functions ***********

// resize event
    virtual void resizeEvent( QResizeEvent* );
    virtual void dragEnterEvent( QDragEnterEvent *e );
    virtual void dragMoveEvent( QDragMoveEvent *e );
    virtual void dragLeaveEvent( QDragLeaveEvent *e );
    virtual void dropEvent( QDropEvent *e );

    virtual void keyPressEvent( QKeyEvent* );
    virtual void wheelEvent( QWheelEvent *e );

    virtual void guiActivateEvent( KParts::GUIActivateEvent *ev );

// GUI
    void setupActions();
    void setupPopupMenus();
    void setupScrollbars();
    void setupRulers();

    void startScreenPres( int pgNum = -1 );
    void doAutomaticScreenPres();

    virtual void updateReadWrite( bool readwrite );

// ********** variables **********

    // document
    KPresenterDoc *m_pKPresenterDoc;

    // flags
    bool continuePres, exitPres;

    // right button popup menus
    QPopupMenu *rb_oalign, *rb_lbegin, *rb_lend;

    // scrollbars
    QScrollBar *vert, *horz;
    int xOffset, yOffset;
    int xOffsetSaved, yOffsetSaved; // saved when going fullscreen

    // dialogs
    BackDia *backDia;
    AFChoose *afChoose;
    StyleDia *styleDia;
    PgConfDia *pgConfDia;
    RotateDia *rotateDia;
    ShadowDia *shadowDia;
    KPPresStructView *presStructView;
    ConfPieDia *confPieDia;
    ConfRectDia *confRectDia;
    QGuardedPtr<SearchDialog> searchDialog;

    // default pen and brush
    QPen pen;
    QBrush brush;
    LineEnd lineBegin;
    LineEnd lineEnd;
    QColor gColor1, gColor2;
    BCType gType;
    FillType fillType;
    PieType pieType;
    bool gUnbalanced;
    int gXFactor, gYFactor;
    int pieLength, pieAngle;
    int rndX, rndY;
    bool sticky;

    // the page
    Page *page;
    KoRuler *h_ruler, *v_ruler;

    // text toolbar values
    QFont tbFont;
    int tbAlign;
    QColor tbColor;
    QStringList fontList;

    bool m_bRectSelection;
    QRect m_rctRectSelection;
    QString m_strNewPart;
    bool m_bShowGUI;
    bool presStarted;
    bool allowWebPres;
    int currPg; // 0-based

    QSize oldSize;

    int screensaver_pid;

    // actions
    KAction *actionEditUndo;
    KAction *actionEditRedo;
    KAction *actionEditCut;
    KAction *actionEditCopy;
    KAction *actionEditPaste;
    KAction *actionEditDelete;
    KAction *actionEditSelectAll;
    KAction *actionEditDuplicatePage;
    KAction *actionEditDelPage;
    KAction *actionEditFind;
    KAction *actionEditHeaderFooter;

    KToggleAction *actionViewShowSideBar;

    KAction *actionInsertPage;
    KAction *actionInsertPicture;
    KAction *actionInsertClipart;

    KAction *actionToolsMouse;
    KAction *actionToolsLine;
    KAction *actionToolsRectangle;
    KAction *actionToolsCircleOrEllipse;
    KAction *actionToolsPie;
    KAction *actionToolsText;
    KAction *actionToolsAutoform;
    KAction *actionToolsDiagramm;
    KAction *actionToolsTable;
    KAction *actionToolsFormula;
    KAction *actionToolsObject;

    KAction *actionTextFont;
    KAction *actionTextFontSize;
    KAction *actionTextFontFamily;
    KAction *actionTextColor;
    KAction *actionTextAlignLeft;
    KAction *actionTextAlignCenter;
    KAction *actionTextAlignRight;
    KAction *actionTextTypeEnumList;
    KAction *actionTextTypeUnsortList;
    KAction *actionTextTypeNormalText;
    KAction *actionTextDepthPlus;
    KAction *actionTextDepthMinus;
    KAction *actionTextSettings;
    KAction *actionTextExtentCont2Height;
    KAction *actionTextExtendObj2Cont;
    KAction *actionTextBold;
    KAction *actionTextItalic;
    KAction *actionTextUnderline;
    KAction *actionTextInsertPageNum;

    KAction *actionExtraPenBrush;
    KAction *actionExtraConfigPie;
    KAction *actionExtraConfigRect;
    KAction *actionExtraRaise;
    KAction *actionExtraLower;
    KAction *actionExtraRotate;
    KAction *actionExtraShadow;
    KAction *actionExtraAlignObjs;
    KAction *actionExtraAlignObjLeft;
    KAction *actionExtraAlignObjCenterH;
    KAction *actionExtraAlignObjRight;
    KAction *actionExtraAlignObjTop;
    KAction *actionExtraAlignObjCenterV;
    KAction *actionExtraAlignObjBottom;
    KAction *actionExtraBackground;
    KAction *actionExtraLayout;
    KAction *actionExtraConfigure;
    KAction *actionExtraWebPres;
    KAction *actionExtraCreateTemplate;
    KAction *actionExtraLineBegin;
    KAction *actionExtraLineEnd;
    KAction *actionExtraGroup;
    KAction *actionExtraUnGroup;

    KAction *actionScreenConfigPages;
    KAction *actionScreenPresStructView;
    KAction *actionScreenAssignEffect;
    KAction *actionScreenStart;
    KAction *actionScreenViewPage;
    KAction *actionScreenStop;
    KAction *actionScreenPause;
    KAction *actionScreenFirst;
    KAction *actionScreenPrev;
    KAction *actionScreenNext;
    KAction *actionScreenLast;
    KAction *actionScreenSkip;
    KAction *actionScreenPenColor;
    KAction *actionScreenPenWidth;

    KAction *actionColorBar;
    KAction *actionExtraDefaultTemplate;


    KAction *actionResizeTextObject;
    KAction *actionExtendObjectHeight;
    KAction *actionObjectProperties;
    KAction *actionChangeClipart;

    KAction *actionRenamePage;

    DCOPObject *dcop;

    QToolButton *pgNext, *pgPrev;
    SideBar *sidebar;
    QSplitter *splitter;
    PageBase *pageBase;

};

#endif
