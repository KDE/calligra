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

/******************************************************************/
/* class Page - Page                                              */
/******************************************************************/
class Page : public QWidget
{
  Q_OBJECT

public:

  // structure for page-configuration for screenpresentations
  struct spPageConfiguration                   
  {
    int time;
  };

  // constructor - destructor
  Page(QWidget *parent=0,const char *name=0,KPresenterView_impl *_view=0);
  ~Page(); 
  
  // public functions
  void selectAllObj();
  void deSelectAllObj();
  void setTextFont(QFont*);
  void setTextColor(QColor*);
  void setTextAlign(TxtParagraph::HorzAlign);
  KTextObject* kTxtObj() {return txtPtr;}

public slots:

  // public slots
  void clipCut();
  void clipCopy();
  void clipPaste();
  void deleteObjs() {view->KPresenterDoc()->deleteObjs(); setCursor(arrowCursor);}
  void rotateObjs() {view->KPresenterDoc()->rotateObjs(); setCursor(arrowCursor);}
  
protected:

  // functions for displaying
  void paintEvent(QPaintEvent*);    
  void paintBackground(QPainter*,QRect);
  void paintObjects(QPainter*,QRect);   
  void mousePressEvent(QMouseEvent *e); 
  void mouseReleaseEvent(QMouseEvent *e);
  void mouseMoveEvent(QMouseEvent *e);
  void mouseDoubleClickEvent(QMouseEvent *e);
  int getObjectAt(int x,int y); 
  struct PageObjects* getObject(int num);
  void selectObj(int num);      
  void deSelectObj(int num);
  void resizeObjTop(int diff,PageObjects* obj);
  void resizeObjLeft(int diff,PageObjects* obj);
  void resizeObjBot(int diff,PageObjects* obj);
  void resizeObjRight(int diff,PageObjects* obj);
  
  // setup popupmenus
  void setupMenus();

  // get - set datas
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
  QList<SpPageConfiguration> *spPageConfig() {return view->KPresenterDoc()->spPageConfig();}
  QRect getPageSize(unsigned int p) {return view->KPresenterDoc()->getPageSize(p,diffx(),diffy());}

  void drawBackColor(QColor,QColor,BCType,QRect,QPainter*,QRect);

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
  SpPageConfiguration *spPCPtr;
  KPresenterView_impl *view;

private slots:

  // slots to react on changes
  void toFontChanged(QFont* font) { emit fontChanged(font); }
  void toColorChanged(QColor* color) { emit colorChanged(color); }
  void toAlignChanged(TxtParagraph::HorzAlign a) { emit alignChanged(a); } 
  void objProperties();
  void chPic();
  void chClip();

signals:

  // signals to notify of changes
  void fontChanged(QFont*);
  void colorChanged(QColor*);
  void alignChanged(TxtParagraph::HorzAlign);

};
#endif //PAGE_H



