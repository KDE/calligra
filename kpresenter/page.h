/******************************************************************/
/* KPresenter - (c) by Reginald Stadlbauer 1997-1998              */
/* Version: 0.0.1                                                 */
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
#include <qrect.h>
#include <qscrbar.h>
#include <qsize.h>
#include <qpaintd.h>
#include <qwmatrix.h>
#include <qpixmap.h>
#include <qapp.h>
#include <qpicture.h>
#include <qpoint.h>
#include <qpntarry.h>
#include <qpopmenu.h>
#include <qimage.h>
#include <qdatetm.h>

#include "ktextobject.h"
#include "global.h"
#include "qwmf.h"
#include "kpobject.h"
#include "kpbackground.h"
#include "kpclipartobject.h"
#include "kppixmapobject.h"

#include "movecmd.h"

class KPresenterView_impl;
class KPresenterDocument_impl;

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
  Page(QWidget *parent=0,const char *name=0,KPresenterView_impl *_view=0);
  ~Page(); 
  
  // public functions
  void draw(QRect,QPainter*);
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

public slots:

  // public slots
  void clipCut();
  void clipCopy();
  void clipPaste();
  void deleteObjs() {view->KPresenterDoc()->deleteObjs(); setCursor(arrowCursor);}
  void rotateObjs() {view->extraRotate(); setCursor(arrowCursor);}
  void shadowObjs() {view->extraShadow(); setCursor(arrowCursor);}
  
protected:
  
  struct PicCache
  {
    QPicture pic;
    int num;
    int subPresStep;
  };

  // functions for displaying
  void paintEvent(QPaintEvent*);    
  void paintBackground(QPainter*,QRect);
  void drawBackground(QPainter*,QRect);
  void drawObjects(QPainter*,QRect);
  void mousePressEvent(QMouseEvent *e); 
  void mouseReleaseEvent(QMouseEvent *e);
  void mouseMoveEvent(QMouseEvent *e);
  void mouseDoubleClickEvent(QMouseEvent *e);
  void resizeEvent(QResizeEvent *e);
  int getObjectAt(int x,int y); 
  void focusInEvent(QFocusEvent*) {}
  void focusOutEvent(QFocusEvent*) {}

  // setup popupmenus
  void setupMenus();

  // get - set data
  QList<KPBackGround> *backgroundList() {return view->KPresenterDoc()->backgroundList();}
  QList<KPObject> *objectList() {return view->KPresenterDoc()->objectList();}
  unsigned int objNums() {return view->KPresenterDoc()->objNums();}
  int diffx() {return view->getDiffX();}
  int diffy() {return view->getDiffY();}
  unsigned int currPgNum() {return view->getCurrPgNum();}
  unsigned int rastX() {return view->KPresenterDoc()->rastX();}
  unsigned int rastY() {return view->KPresenterDoc()->rastY();}
  QColor txtBackCol() {return view->KPresenterDoc()->txtBackCol();}
  QColor txtSelCol() {return view->KPresenterDoc()->txtSelCol();}
  bool spInfinitLoop() {return view->KPresenterDoc()->spInfinitLoop();}
  bool spManualSwitch() {return view->KPresenterDoc()->spManualSwitch();}
  QRect getPageSize(unsigned int p,float fakt=1.0) {return view->KPresenterDoc()->getPageSize(p,diffx(),diffy(),fakt);}
  unsigned int pageNums() {return view->KPresenterDoc()->getPageNums();}
  int getPageOfObj(int i,float fakt = 1.0) {return view->KPresenterDoc()->getPageOfObj(i,diffx(),diffy(),fakt);}

  void _repaint(bool erase=true) {view->KPresenterDoc()->repaint(false);}
  void _repaint(QRect r) {view->KPresenterDoc()->repaint(r);}
  void _repaint(KPObject *o) {view->KPresenterDoc()->repaint(o);}

  void drawPageInPix(QPixmap&,int);
  void drawPageInPainter(QPainter*,int,QRect);
  void changePages(QPixmap,QPixmap,PageEffect);
  void doObjEffects();
  void drawObject(KPObject*,QPixmap*,int,int,int,int,int,int);

  // variables
  QPopupMenu *graphMenu,*picMenu,*txtMenu,*clipMenu,*presMenu;
  QPopupMenu *alignMenu1,*alignMenu2,*alignMenu3,*alignMenu4;
  bool mousePressed;              
  ModifyType modType;                    
  unsigned int oldMx,oldMy;                
  int resizeObjNum,editNum;
  bool fillBlack;
  KPresenterView_impl *view;
  bool editMode,goingBack,drawMode;
  unsigned int currPresPage,currPresStep,subPresStep;
  float _presFakt;
  QList<int> presStepList;
  int PM_DM,PM_SM;
  int firstX,firstY;

private slots:

  // slots to react on changes
  void toFontChanged(QFont* font) { emit fontChanged(font); }
  void toColorChanged(QColor* color) { emit colorChanged(color); }
  void toAlignChanged(TxtParagraph::HorzAlign a) { emit alignChanged(a); } 
  void objProperties();
  void chPic();
  void chClip();
  void assignEffect() {view->screenAssignEffect();}
  void drawingMode()
    {presMenu->setItemChecked(PM_DM,true);presMenu->setItemChecked(PM_SM,false);drawMode = true;setCursor(arrowCursor);}
  void switchingMode()
    {presMenu->setItemChecked(PM_DM,false);presMenu->setItemChecked(PM_SM,true);drawMode = false;setCursor(blankCursor);}
  void alignObjLeft()
    {view->extraAlignObjLeftidl();}
  void alignObjCenterH()
    {view->extraAlignObjCenterHidl();}
  void alignObjRight()
    {view->extraAlignObjRightidl();}
  void alignObjTop()
    {view->extraAlignObjTopidl();}
  void alignObjCenterV()
    {view->extraAlignObjCenterVidl();}
  void alignObjBottom()
    {view->extraAlignObjBottomidl();}
 
signals:

  // signals to notify of changes
  void fontChanged(QFont*);
  void colorChanged(QColor*);
  void alignChanged(TxtParagraph::HorzAlign);
  void stopPres();

};
#endif //PAGE_H



