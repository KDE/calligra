/******************************************************************/
/* KPresenter - (c) by Reginald Stadlbauer 1997-1998              */
/* Version: 0.1.0alpha                                            */
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

#include "ktextobject.h"
#include "graphobj.h"
#include "global.h"
#include "qwmf.h"

#include <dither.h>

class KPresenterView_impl;
class KPresenterDocument_impl;

#ifndef min
#define min(a,b) ((a)<(b)?(a):(b))
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
  void selectAllObj();
  void deSelectAllObj();
  void selectObj(int num);      
  void deSelectObj(int num);
  void setTextFont(QFont*);
  void setTextColor(QColor*);
  void setTextAlign(TxtParagraph::HorzAlign);
  KTextObject* kTxtObj() {return txtPtr;}

  static void _drawBackColor(QColor c1,QColor c2,BCType bct,QPainter* p,QSize s)
    {Page *pg = new Page(0,"",0); pg->drawBackColor(c1,c2,bct,p,s); delete pg;}

  void startScreenPresentation();
  void stopScreenPresentation();
  bool pNext(bool);
  bool pPrev(bool);

  unsigned int presPage() {return currPresPage;}
  void setPresFakt(float f) {_presFakt = f;}
  float presFakt() {return _presFakt;}

  bool canAssignEffect(int&,int&);

  void keyPressEvent(QKeyEvent *e);

public slots:

  // public slots
  void clipCut();
  void clipCopy();
  void clipPaste();
  void deleteObjs() {view->KPresenterDoc()->deleteObjs(); setCursor(arrowCursor);}
  void rotateObjs() {view->KPresenterDoc()->rotateObjs(); setCursor(arrowCursor);}
  void restoreBackColor(unsigned int);
  
protected:

  // functions for displaying
  void paintEvent(QPaintEvent*);    
  void paintBackground(QPainter*,QRect);
  void paintObjects(QPainter*,QRect);   
  void mousePressEvent(QMouseEvent *e); 
  void mouseReleaseEvent(QMouseEvent *e);
  void mouseMoveEvent(QMouseEvent *e);
  void mouseDoubleClickEvent(QMouseEvent *e);
  void resizeEvent(QResizeEvent *e);
  int getObjectAt(int x,int y); 
  struct PageObjects* getObject(int num);
  void resizeObjTop(int diff,PageObjects* obj);
  void resizeObjLeft(int diff,PageObjects* obj);
  void resizeObjBot(int diff,PageObjects* obj);
  void resizeObjRight(int diff,PageObjects* obj);

  // setup popupmenus
  void setupMenus();

  // get - set data
  QList<Background> *pageList() {return view->KPresenterDoc()->pageList();}
  QList<PageObjects> *objList() {return view->KPresenterDoc()->objList();}
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

  void drawBackColor(QColor,QColor,BCType,QPainter*,QSize);
  void _repaint(bool erase=true) {view->KPresenterDoc()->repaint(erase);}
  void _repaint(int _x,int _y,int _w,int _h,bool erase=true)
    {view->KPresenterDoc()->repaint(_x+diffx(),_y+diffy(),_w,_h,erase);}

  // variables
  QPopupMenu *graphMenu,*picMenu,*txtMenu,*clipMenu;
  Background *pagePtr;            
  PageObjects *objPtr;            
  KTextObject *txtPtr;                
  unsigned int oldx,oldy;                             
  bool mousePressed;              
  unsigned int modType;                    
  unsigned int oldMx,oldMy;                
  unsigned int resizeObjNum,editNum;
  bool drawBack;                  
  GraphObj *graphPtr;
  KPresenterView_impl *view;
  bool editMode;
  unsigned int currPresPage,currPresStep;
  float _presFakt;
  QList<int> presStepList;

private slots:

  // slots to react on changes
  void toFontChanged(QFont* font) { emit fontChanged(font); }
  void toColorChanged(QColor* color) { emit colorChanged(color); }
  void toAlignChanged(TxtParagraph::HorzAlign a) { emit alignChanged(a); } 
  void objProperties();
  void chPic();
  void chClip();
  void assignEffect() {view->screenAssignEffect();}

signals:

  // signals to notify of changes
  void fontChanged(QFont*);
  void colorChanged(QColor*);
  void alignChanged(TxtParagraph::HorzAlign);

};
#endif //PAGE_H



