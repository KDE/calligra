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

#include "kpresenter_view.h"

#include <stdlib.h>

#include <qstring.h>
#include <qevent.h>
#include <qwidget.h>
#include <qcolor.h>
#include <qpainter.h>
#include <qlist.h>
#include <krect.h>
#include <qscrbar.h>
#include <ksize.h>
#include <qpaintd.h>
#include <qwmatrix.h>
#include <qpixmap.h>
#include <qapp.h>
#include <qpicture.h>
#include <kpoint.h>
#include <qpntarry.h>
#include <qpopmenu.h>
#include <qimage.h>
#include <qdatetm.h>
#include <qdropsite.h>

#include <koQueryTypes.h>

#include "ktextobject.h"
#include "global.h"
#include "qwmf.h"
#include "kpobject.h"
#include "kpbackground.h"
#include "kpclipartobject.h"
#include "kppixmapobject.h"

#include "movecmd.h"
#include "resizecmd.h"
#include "gotopage.h"

class KPresenterView;
class KPresenterDoc;

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
  Page(QWidget *parent=0,const char *name=0,KPresenterView *_view=0);
  ~Page();

  // public functions
  void draw(KRect,QPainter*);
  void selectAllObj();
  void deSelectAllObj();
  void selectObj(int num);
  void selectObj(KPObject*);
  void deSelectObj(int num);
  void deSelectObj(KPObject*);
  void setTextFont(QFont*);
  void setTextColor(QColor*);
  void setTextAlign(TxtParagraph::HorzAlign);
  KTextObject* kTxtObj()
    { return ((editNum != -1 && objectList()->at(editNum)->getType() == OT_TEXT) ?
	      dynamic_cast<KPTextObject*>(objectList()->at(editNum))->getKTextObject() : 0); }

  void startScreenPresentation(bool);
  void stopScreenPresentation();
  bool pNext(bool);
  bool pPrev(bool);

  unsigned int presPage() {return currPresPage;}
  void setPresFakt(float f) {_presFakt = f;}
  float presFakt() {return _presFakt;}

  bool canAssignEffect(int&,int&);

  void keyPressEvent(QKeyEvent *e);

  void print(QPainter*,QPrinter*,float,float);

  void editSelectedTextArea();
  void setPartEntry( KoDocumentEntry& _e )
    { partEntry = _e; }
  void setToolEditMode(ToolEditMode _m);
  void setAutoForm(QString _autoform)
    { autoform = _autoform; }

  KTextObject *haveASelectedTextObj();

  void drawPageInPix(QPixmap&,int);
  void drawPageInPix2(QPixmap&,int,int,float _zoom = 1.0);

public slots:

  // public slots
  void clipCut();
  void clipCopy();
  void clipPaste();
  void deleteObjs() {view->kPresenterDoc()->deleteObjs(); setToolEditMode(toolEditMode);}
  void rotateObjs() {view->extraRotate(); setToolEditMode(toolEditMode);}
  void shadowObjs() {view->extraShadow(); setToolEditMode(toolEditMode);}

protected:

  struct PicCache
  {
    QPicture pic;
    int num;
    int subPresStep;
  };

  // functions for displaying
  void paintEvent(QPaintEvent*);
  void paintBackground(QPainter*,KRect);
  void drawBackground(QPainter*,KRect);
  void drawObjects(QPainter*,KRect);
  void mousePressEvent(QMouseEvent *e);
  void mouseReleaseEvent(QMouseEvent *e);
  void mouseMoveEvent(QMouseEvent *e);
  void mouseDoubleClickEvent(QMouseEvent *e);
  void resizeEvent(QResizeEvent *e);
  int getObjectAt(int x,int y);
  void focusInEvent(QFocusEvent*) {}
  void focusOutEvent(QFocusEvent*) {}
  void enterEvent(QEvent *e)
    { view->setRulerMousePos(((QMouseEvent*)e)->x(),((QMouseEvent*)e)->y()); view->setRulerMouseShow(true); }
  void leaveEvent(QEvent *e)
    { view->setRulerMouseShow(false); }
  void dragEnterEvent(QDragEnterEvent *e);
  void dragLeaveEvent(QDragLeaveEvent *e);
  void dragMoveEvent(QDragMoveEvent *e);
  void dropEvent(QDropEvent *e);

  // setup popupmenus
  void setupMenus();

  // get - set data
  QList<KPBackGround> *backgroundList() {return view->kPresenterDoc()->backgroundList();}
  QList<KPObject> *objectList() {return view->kPresenterDoc()->objectList();}
  unsigned int objNums() {return view->kPresenterDoc()->objNums();}
  int diffx(int i = -1);
  int diffy(int i = -1);
  unsigned int currPgNum() {return view->getCurrPgNum();}
  unsigned int rastX() {return view->kPresenterDoc()->rastX();}
  unsigned int rastY() {return view->kPresenterDoc()->rastY();}
  QColor txtBackCol() {return view->kPresenterDoc()->txtBackCol();}
  bool spInfinitLoop() {return view->kPresenterDoc()->spInfinitLoop();}
  bool spManualSwitch() {return view->kPresenterDoc()->spManualSwitch();}
  KRect getPageSize(unsigned int p,float fakt=1.0,bool decBorders = true)
    {return view->kPresenterDoc()->getPageSize(p,diffx(),diffy(),fakt,decBorders);}
  unsigned int pageNums() {return view->kPresenterDoc()->getPageNums();}
  int getPageOfObj(int i,float fakt = 1.0) {return view->kPresenterDoc()->getPageOfObj(i,diffx(),diffy(),fakt);}
  float objSpeedFakt() {return ObjSpeed[static_cast<int>(view->kPresenterDoc()->getPresSpeed())];}
  float pageSpeedFakt() {return PageSpeed[static_cast<int>(view->kPresenterDoc()->getPresSpeed())];}

  void _repaint(bool erase=true) {view->kPresenterDoc()->repaint(false);}
  void _repaint(KRect r) {view->kPresenterDoc()->repaint(r);}
  void _repaint(KPObject *o) {view->kPresenterDoc()->repaint(o);}

  void drawPageInPainter(QPainter*,int,KRect);
  void changePages(QPixmap,QPixmap,PageEffect);
  void doObjEffects();
  void drawObject(KPObject*,QPixmap*,int,int,int,int,int,int);

  void insertText(KRect);
  void insertLineH(KRect,bool);
  void insertLineV(KRect,bool);
  void insertLineD1(KRect,bool);
  void insertLineD2(KRect,bool);
  void insertRect(KRect);
  void insertEllipse(KRect);
  void insertPie(KRect);
  void insertObject(KRect);
  void insertTable(KRect);
  void insertDiagramm(KRect);
  void insertFormula(KRect);
  void insertAutoform(KRect,bool);

  void selectNext();
  void selectPrev();

  // variables
  QPopupMenu *graphMenu,*picMenu,*txtMenu,*clipMenu,*presMenu;
  QPopupMenu *alignMenu1,*alignMenu2,*alignMenu3,*alignMenu4,*alignMenu5;
  QPopupMenu *pageMenu,*pieMenu,*rectMenu,*alignMenu6,*partMenu,*alignMenu7;
  bool mousePressed;
  ModifyType modType;
  unsigned int oldMx,oldMy;
  int resizeObjNum,editNum;
  bool fillBlack;
  KPresenterView *view;
  bool editMode,goingBack,drawMode;
  unsigned int currPresPage,currPresStep,subPresStep;
  unsigned int oldPresPage,oldPresStep,oldSubPresStep;
  float _presFakt;
  QList<int> presStepList;
  int PM_DM,PM_SM;
  int firstX,firstY;
  bool drawRubber;
  KRect rubber,oldBoundingRect;
  ToolEditMode toolEditMode;
  KRect insRect;
  KoDocumentEntry partEntry;
  QList <KPObject> tmpObjs;
  QString autoform;

public slots:
  void chPic();
  void chClip();

private slots:

  // slots to react on changes
  void toFontChanged(QFont* font) { emit fontChanged(font); }
  void toColorChanged(QColor* color) { emit colorChanged(color); }
  void toAlignChanged(TxtParagraph::HorzAlign a) { emit alignChanged(a); }
  void objProperties();
  void objConfigPie() {view->extraConfigPie();}
  void objConfigRect() {view->extraConfigRect();}
  void assignEffect() {view->screenAssignEffect();}
  void drawingMode()
  { presMenu->setItemChecked(PM_DM,true);presMenu->setItemChecked(PM_SM,false);drawMode = true;setCursor(arrowCursor); }
  void switchingMode()
  { presMenu->setItemChecked(PM_DM,false);presMenu->setItemChecked(PM_SM,true);drawMode = false;setCursor(blankCursor); }
  void alignObjLeft()
  { view->extraAlignObjLeftidl(); }
  void alignObjCenterH()
  { view->extraAlignObjCenterHidl(); }
  void alignObjRight()
  { view->extraAlignObjRightidl(); }
  void alignObjTop()
  { view->extraAlignObjTopidl(); }
  void alignObjCenterV()
  { view->extraAlignObjCenterVidl(); }
  void alignObjBottom()
  { view->extraAlignObjBottomidl(); }
  void pageLayout()
  { view->extraLayout(); }
  void pageBackground()
  { view->extraBackground(); }
  void pageInsert()
  { view->insertPage(); }
  void pageDelete()
  { view->editDelPage(); }
  void pagePaste()
  { view->editPaste(); }
  void configPages()
  { view->screenConfigPages(); }
  void presStructView()
  { view->screenPresStructView(); }
  void slotGotoPage();
  void slotExitPres()
  { view->screenStop(); }
  void slotEditHF()
  { view->editHeaderFooter(); }
  void slotTextContents2Height()
  { view->textContentsToHeight(); }
  
signals:

  // signals to notify of changes
  void fontChanged(QFont*);
  void colorChanged(QColor*);
  void alignChanged(TxtParagraph::HorzAlign);
  void stopPres();

};
#endif //PAGE_H



