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
/* Module: Page                                                   */
/******************************************************************/

#include <qprinter.h>
#include <qprogdlg.h>
#include <qprogbar.h>
#include "page.h"
#include "page.moc"

/******************************************************************/
/* class Page - Page                                              */
/******************************************************************/

/*====================== constructor =============================*/
Page::Page(QWidget *parent=0,const char *name=0,KPresenterView_impl *_view=0)
  : QWidget(parent,name)
{
  if (parent)
    {
      mousePressed = false;
      modType = MT_NONE;
      resizeObjNum = -1;
      editNum = -1;
      setupMenus();
      setBackgroundColor(white);
      view = _view;
      setMouseTracking(true);
      show();
      editMode = true;
      currPresPage = 1;
      currPresStep = 0;
      subPresStep = 0;
      _presFakt = 1.0;
      goingBack = false;
      drawMode = false;
      fillBlack = true;
      drawRubber = false;
    }
  else 
    {
      view = 0;
      hide();
    }
}

/*======================== destructor ============================*/
Page::~Page()
{
}

/*============================ draw contents ====================*/
void Page::draw(QRect _rect,QPainter *p)
{
  p->save();
  editMode = false;
  fillBlack = false;
  _presFakt = 1.0;
  currPresStep = 1000;
  subPresStep = 1000;
  currPresPage = currPgNum();

  drawPageInPainter(p,view->getDiffY(),_rect);

  currPresPage = 1;
  currPresStep = 0;
  subPresStep = 0;
  _presFakt = 1.0;
  fillBlack = true;
  editMode = true;
  p->restore();
}

/*======================== paint event ===========================*/
void Page::paintEvent(QPaintEvent* paintEvent)
{
  QPainter painter;
  QPixmap pix(width(),height());

  painter.begin(&pix);

  if (editMode || !editMode && !fillBlack)
    painter.fillRect(paintEvent->rect().x(),paintEvent->rect().y(),
		     paintEvent->rect().width(),paintEvent->rect().height(),white);
  else
    painter.fillRect(paintEvent->rect().x(),paintEvent->rect().y(),
		     paintEvent->rect().width(),paintEvent->rect().height(),black);

  painter.setClipping(true);
  painter.setClipRect(paintEvent->rect());
  
  drawBackground(&painter,paintEvent->rect());
  drawObjects(&painter,paintEvent->rect());

  painter.end();

  bitBlt(this,paintEvent->rect().x(),paintEvent->rect().y(),&pix,
	 paintEvent->rect().x(),paintEvent->rect().y(),paintEvent->rect().width(),paintEvent->rect().height());
}

/*======================= draw background ========================*/
void Page::drawBackground(QPainter *painter,QRect rect)
{
  KPBackGround *kpbackground = 0;
  
  for (int i = 0;i < static_cast<int>(backgroundList()->count());i++)
    {
      kpbackground = backgroundList()->at(i);
      if ((rect.intersects(QRect(getPageSize(i,_presFakt))) && editMode) ||
 	  (!editMode && static_cast<int>(currPresPage) == i + 1))
 	kpbackground->draw(painter,QPoint(getPageSize(i,_presFakt).x(),getPageSize(i,_presFakt).y()),editMode);
    }
}

/*========================= draw objects =========================*/
void Page::drawObjects(QPainter *painter,QRect rect)
{
  KPObject *kpobject = 0;
  
  for (int i = 0;i < static_cast<int>(objectList()->count());i++)
    {
      kpobject = objectList()->at(i);
      
      if ((rect.intersects(kpobject->getBoundingRect(diffx(),diffy())) && editMode) ||
	   (!editMode && getPageOfObj(i,_presFakt) == static_cast<int>(currPresPage) &&
	    kpobject->getPresNum() <= static_cast<int>(currPresStep)))
	{
	  if (!editMode && static_cast<int>(currPresStep) == kpobject->getPresNum() && !goingBack)
	    {
	      kpobject->setSubPresStep(subPresStep);
	      kpobject->doSpecificEffects(true);
	    }

	  kpobject->draw(painter,diffx(),diffy());
	  kpobject->setSubPresStep(0);
	  kpobject->doSpecificEffects(false);
	}
    }
}

/*==================== handle mouse pressed ======================*/
void Page::mousePressEvent(QMouseEvent *e)
{
  KPObject *kpobject = 0;

  oldMx = e->x();
  oldMy = e->y();

  resizeObjNum = -1;

  if (editNum != -1)
    {
      kpobject = objectList()->at(editNum);
      editNum = -1;
      if (kpobject->getType() == OT_TEXT)
	{
	  KPTextObject * kptextobject = dynamic_cast<KPTextObject*>(kpobject);
	  kpobject->deactivate();
	  kptextobject->getKTextObject()->clearFocus();
	  disconnect(kptextobject->getKTextObject(),SIGNAL(fontChanged(QFont*)),this,SLOT(toFontChanged(QFont*)));
	  disconnect(kptextobject->getKTextObject(),SIGNAL(colorChanged(QColor*)),this,SLOT(toColorChanged(QColor*)));
	  disconnect(kptextobject->getKTextObject(),SIGNAL(horzAlignChanged(TxtParagraph::HorzAlign)),
		     this,SLOT(toAlignChanged(TxtParagraph::HorzAlign)));
	  kptextobject->getKTextObject()->setShowCursor(false);
	  setFocusProxy(0);
	  setFocusPolicy(QWidget::NoFocus);
	}
    }

  if (editMode)
    {
      if (e->button() == LeftButton)
	{
	  mousePressed = true;
	  bool overObject = false;
	  bool deSelAll = true;
	  KPObject *kpobject = 0;

	  firstX = e->x();
	  firstY = e->y();

	  for (int i = static_cast<int>(objectList()->count()) - 1;i >= 0 ;i--)
	    {
	      kpobject = objectList()->at(i);
	      if (kpobject->contains(QPoint(e->x(),e->y()),diffx(),diffy()))
		{
		  overObject = true;
		  if (kpobject->isSelected() && modType == MT_MOVE) deSelAll = false;
		  if (kpobject->isSelected() && modType != MT_MOVE && modType != MT_NONE)
		    resizeObjNum = i;
		  break;
		}
	    }

	  if (deSelAll && !(e->state() & ShiftButton) && !(e->state() & ControlButton))
 	    deSelectAllObj();

	  if (overObject)
	    {
	      selectObj(kpobject);
	      modType = MT_NONE;
	    }
	  else
	    {
	      modType = MT_NONE;
	      if (!(e->state() & ShiftButton) && !(e->state() & ControlButton))
		deSelectAllObj();
	      drawRubber = true;
	      rubber = QRect(e->x(),e->y(),0,0);
	    }

	}
      if (e->button() == RightButton)
	{
	  int num = getObjectAt(e->x(),e->y());
	  if (num != -1)
	    {
	      kpobject = objectList()->at(num);
	      if (kpobject->getType() == OT_PICTURE)
		{
		  mousePressed = false;
		  deSelectAllObj();
		  selectObj(kpobject);
		  QPoint pnt = QCursor::pos();
		  picMenu->popup(pnt);
		  modType = MT_NONE;
		}
	      else if (kpobject->getType() == OT_CLIPART)
		{
		  mousePressed = false;
		  deSelectAllObj();
		  selectObj(kpobject);
		  QPoint pnt = QCursor::pos();
		  clipMenu->popup(pnt);
		  modType = MT_NONE;
		}
	      else if (kpobject->getType() == OT_TEXT)
		{
		  if (!(e->state() & ShiftButton) && !(e->state() & ControlButton))
		    deSelectAllObj();
		  selectObj(kpobject);
		  QPoint pnt = QCursor::pos();
		  txtMenu->popup(pnt);
		  mousePressed = false;
		  modType = MT_NONE;
		}
	      else
		{
		  if (!(e->state() & ShiftButton) && !(e->state() & ControlButton))
		    deSelectAllObj();
		  selectObj(kpobject);
		  QPoint pnt = QCursor::pos();
		  graphMenu->popup(pnt);
		  mousePressed = false;
		  modType = MT_NONE;
		}
	    }
	}
    }
  else
    {
      oldMx = e->x();
      oldMy = e->y();
      if (e->button() == LeftButton)
	{	
	  if (presMenu->isVisible())
	    {
	      presMenu->hide();
	      setCursor(blankCursor);
	    }
	  else
	    {
	      if (drawMode)
		{}
	      else 
		view->screenNext();
	    }
	}
      else if (e->button() == MidButton)
	view->screenPrev();
      else if (e->button() == RightButton)
	{
	  setCursor(arrowCursor);
	  QPoint pnt = QCursor::pos();
	  presMenu->popup(pnt);
	} 
    }
  
  mouseMoveEvent(e);
}

/*=================== handle mouse released ======================*/
void Page::mouseReleaseEvent(QMouseEvent *e)
{
  int mx = e->x();
  int my = e->y();
  mx = (mx / rastX()) * rastX();
  my = (my / rastY()) * rastY();
  firstX = (firstX / rastX()) * rastX();
  firstY = (firstY / rastY()) * rastY();
  QList<KPObject> _objects;
  _objects.setAutoDelete(false);
  KPObject *kpobject = 0;

  switch (modType)
    {
    case MT_NONE:
      {
	if (drawRubber)
	  {
	    QPainter p;
	    p.begin(this);
	    p.setRasterOp(NotROP);
	    p.setPen(QPen(black,0,DotLine));
	    p.drawRect(rubber);
	    p.end();
	    drawRubber = false;

	    rubber = rubber.normalize();
	    KPObject *kpobject = 0;
	    for (int i = static_cast<int>(objectList()->count()) - 1;i >= 0;i--)
	      {
		kpobject = objectList()->at(i);
		if (rubber.intersects(kpobject->getBoundingRect(diffx(),diffy())))
		  selectObj(kpobject);
	      }
	  }
      } break;
    case MT_MOVE:
      {
	if (firstX != e->x() || firstY != e->y())
	  {
	    
	    for (int i = static_cast<int>(objectList()->count()) - 1;i >= 0;i--)
	      {
		kpobject = objectList()->at(i);
		if (kpobject->isSelected()) 
		  _objects.append(kpobject);
	      }
	    MoveByCmd *moveByCmd = new MoveByCmd(i18n("Move object(s)"),QPoint(mx - firstX,my - firstY),
						 _objects,view->KPresenterDoc());
	    view->KPresenterDoc()->commands()->addCommand(moveByCmd);
	  }
      } break;
    case MT_RESIZE_UP:
      {
	if (firstX != e->x() || firstY != e->y())
	  {
	    kpobject = objectList()->at(resizeObjNum);
	    ResizeCmd *resizeCmd = new ResizeCmd(i18n("Resize object up"),QPoint(0,my - firstY),QSize(0,firstY - my),
						 kpobject,view->KPresenterDoc());
	    view->KPresenterDoc()->commands()->addCommand(resizeCmd);
	  }
      } break;
    case MT_RESIZE_DN:
      {
	if (firstX != e->x() || firstY != e->y())
	  {
	    kpobject = objectList()->at(resizeObjNum);
	    ResizeCmd *resizeCmd = new ResizeCmd(i18n("Resize object down"),QPoint(0,0),QSize(0,my - firstY),
						 kpobject,view->KPresenterDoc());
	    view->KPresenterDoc()->commands()->addCommand(resizeCmd);
	  }
      } break;
    case MT_RESIZE_LF:
      {
	if (firstX != e->x() || firstY != e->y())
	  {
	    kpobject = objectList()->at(resizeObjNum);
	    ResizeCmd *resizeCmd = new ResizeCmd(i18n("Resize object left"),QPoint(mx - firstX,0),QSize(firstX - mx,0),
						 kpobject,view->KPresenterDoc());
	    view->KPresenterDoc()->commands()->addCommand(resizeCmd);
	  }
      } break;
    case MT_RESIZE_RT:
      {
	if (firstX != e->x() || firstY != e->y())
	  {
	    kpobject = objectList()->at(resizeObjNum);
	    ResizeCmd *resizeCmd = new ResizeCmd(i18n("Resize object right"),QPoint(0,0),QSize(mx - firstX,0),
						 kpobject,view->KPresenterDoc());
	    view->KPresenterDoc()->commands()->addCommand(resizeCmd);
	  }
      } break;
    case MT_RESIZE_LU:
      {
	if (firstX != e->x() || firstY != e->y())
	  {
	    kpobject = objectList()->at(resizeObjNum);
	    ResizeCmd *resizeCmd = new ResizeCmd(i18n("Resize object left up"),QPoint(mx - firstX,my - firstY),
						 QSize(firstX - mx,firstY - my),kpobject,view->KPresenterDoc());
	    view->KPresenterDoc()->commands()->addCommand(resizeCmd);
	  }
      } break;
    case MT_RESIZE_LD:
      {
	if (firstX != e->x() || firstY != e->y())
	  {
	    kpobject = objectList()->at(resizeObjNum);
	    ResizeCmd *resizeCmd = new ResizeCmd(i18n("Resize object left down"),QPoint(mx - firstX,0),
						 QSize(firstX - mx,my - firstY),kpobject,view->KPresenterDoc());
	    view->KPresenterDoc()->commands()->addCommand(resizeCmd);
	  }
      } break;
    case MT_RESIZE_RU:
      {
	if (firstX != e->x() || firstY != e->y())
	  {
	    kpobject = objectList()->at(resizeObjNum);
	    ResizeCmd *resizeCmd = new ResizeCmd(i18n("Resize object right up"),QPoint(0,my - firstY),
						 QSize(mx - firstX,firstY - my),kpobject,view->KPresenterDoc());
	    view->KPresenterDoc()->commands()->addCommand(resizeCmd);
	  }
      } break;
    case MT_RESIZE_RD:
      {
	if (firstX != e->x() || firstY != e->y())
	  {
	    kpobject = objectList()->at(resizeObjNum);
	    ResizeCmd *resizeCmd = new ResizeCmd(i18n("Resize object right down"),QPoint(0,0),QSize(mx - firstX,my - firstY),
						 kpobject,view->KPresenterDoc());
	    view->KPresenterDoc()->commands()->addCommand(resizeCmd);
	  }
      } break;
    }

  mousePressed = false;
  modType = MT_NONE;
  resizeObjNum = -1;
  mouseMoveEvent(e);
}

/*==================== handle mouse moved ========================*/
void Page::mouseMoveEvent(QMouseEvent *e)
{
  if (editMode)
    {
      KPObject *kpobject;
      
      if (!mousePressed || (!drawRubber && modType == MT_NONE))
	{
	  setCursor(arrowCursor);
	  for (int i = static_cast<int>(objectList()->count()) - 1;i >= 0;i--)
	    {
	      kpobject = objectList()->at(i);
	      if (kpobject->contains(QPoint(e->x(),e->y()),diffx(),diffy()))
		{
		  if (kpobject->isSelected())
		    setCursor(kpobject->getCursor(QPoint(e->x(),e->y()),diffx(),diffy(),modType));
		  return;
		}
	    }
	}
      else
	{
	  int mx = e->x();
	  int my = e->y();
	  mx = (mx / rastX()) * rastX();
	  my = (my / rastY()) * rastY();
	  oldMx = (oldMx / rastX()) * rastX();
	  oldMy = (oldMy / rastY()) * rastY();

	  if (modType == MT_NONE)
	    {
	      if (drawRubber)
		{
		  QPainter p;
		  p.begin(this);
		  p.setRasterOp(NotROP);
		  p.setPen(QPen(black,0,DotLine));
		  p.drawRect(rubber);
		  rubber.setRight(e->x());
		  rubber.setBottom(e->y());
		  p.drawRect(rubber);
		  p.end();
		}
	    }
	  else if (modType == MT_MOVE)
	    {
	      QList<KPObject> _objects;
	      _objects.setAutoDelete(false);
	      for (int i = static_cast<int>(objectList()->count()) - 1;i >= 0;i--)
		{
		  kpobject = objectList()->at(i);
		  if (kpobject->isSelected()) 
		    _objects.append(kpobject);
		}
	      MoveByCmd *moveByCmd = new MoveByCmd(i18n("Move object(s)"),QPoint(mx - oldMx,my - oldMy),
						   _objects,view->KPresenterDoc());
	      moveByCmd->execute();
	      delete moveByCmd;
	    }
	  else if (modType != MT_NONE && resizeObjNum != -1)
	    {
	      QRect oldRect;
	      kpobject = objectList()->at(resizeObjNum);
	      oldRect = kpobject->getBoundingRect(0,0);

	      switch (modType)
		{
		case MT_RESIZE_LU:
		  {
		    ResizeCmd *resizeCmd = new ResizeCmd(i18n("Resize object left up"),QPoint(mx - oldMx,my - oldMy),
							 QSize(oldMx - mx,oldMy - my),kpobject,view->KPresenterDoc());
		    resizeCmd->execute();
		    delete resizeCmd;
		  } break;
		case MT_RESIZE_LF:
		  {
		    ResizeCmd *resizeCmd = new ResizeCmd(i18n("Resize object left"),QPoint(mx - oldMx,0),QSize(oldMx - mx,0),
							 kpobject,view->KPresenterDoc());
		    resizeCmd->execute();
		    delete resizeCmd;
		  } break;
		case MT_RESIZE_LD:
		  {
		    ResizeCmd *resizeCmd = new ResizeCmd(i18n("Resize object left down"),QPoint(mx - oldMx,0),
							 QSize(oldMx - mx,my - oldMy),kpobject,view->KPresenterDoc());
		    resizeCmd->execute();
		    delete resizeCmd;
		  } break;
		case MT_RESIZE_RU:
		  {
		    ResizeCmd *resizeCmd = new ResizeCmd(i18n("Resize object right up"),QPoint(0,my - oldMy),
							 QSize(mx - oldMx,oldMy - my),kpobject,view->KPresenterDoc());
		    resizeCmd->execute();
		    delete resizeCmd;
		  } break;
		case MT_RESIZE_RT:
		  {
		    ResizeCmd *resizeCmd = new ResizeCmd(i18n("Resize object right"),QPoint(0,0),QSize(mx - oldMx,0),
							 kpobject,view->KPresenterDoc());
		    resizeCmd->execute();
		    delete resizeCmd;
		  } break;
		case MT_RESIZE_RD:
		  {
		    ResizeCmd *resizeCmd = new ResizeCmd(i18n("Resize object right down"),QPoint(0,0),QSize(mx - oldMx,my - oldMy),
							 kpobject,view->KPresenterDoc());
		    resizeCmd->execute();
		    delete resizeCmd;
		  } break;
		case MT_RESIZE_UP:
		  {
		    ResizeCmd *resizeCmd = new ResizeCmd(i18n("Resize object up"),QPoint(0,my - oldMy),QSize(0,oldMy - my),
							 kpobject,view->KPresenterDoc());
		    resizeCmd->execute();
		    delete resizeCmd;
		  } break;
		case MT_RESIZE_DN:
		  {
		    ResizeCmd *resizeCmd = new ResizeCmd(i18n("Resize object down"),QPoint(0,0),QSize(0,my - oldMy),
							 kpobject,view->KPresenterDoc());
		    resizeCmd->execute();
		    delete resizeCmd;
		  } break;
		default: break;
		}
	    }

	  oldMx = e->x();
	  oldMy = e->y();
	}
    }
  else if (!editMode && drawMode)
    {
      QPainter p;
      p.begin(this);
      p.setPen(view->KPresenterDoc()->presPen());
      p.drawLine(oldMx,oldMy,e->x(),e->y());
      oldMx = e->x();
      oldMy = e->y();
      p.end();
    }
  if (!editMode && !drawMode && !presMenu->isVisible() && fillBlack)
    setCursor(blankCursor);
}

/*==================== mouse double click ========================*/
void Page::mouseDoubleClickEvent(QMouseEvent *e)
{
  deSelectAllObj();
  KPObject *kpobject = 0;

  for (int i = static_cast<int>(objectList()->count()) - 1;i >= 0;i--)
    {
      kpobject = objectList()->at(i);
      if (kpobject->contains(QPoint(e->x(),e->y()),diffx(),diffy()))
	{
	  if (kpobject->getType() == OT_TEXT)
	    {
	      KPTextObject *kptextobject = dynamic_cast<KPTextObject*>(kpobject);
	      
	      kpobject->activate(this,diffx(),diffy());
	      kptextobject->getKTextObject()->setBackgroundColor(txtBackCol());
	      setFocusProxy(kptextobject->getKTextObject());
	      setFocusPolicy(QWidget::StrongFocus);
	      kptextobject->getKTextObject()->setFocus();
	      kptextobject->getKTextObject()->setShowCursor(true);
	      connect(kptextobject->getKTextObject(),SIGNAL(fontChanged(QFont*)),this,SLOT(toFontChanged(QFont*)));
	      connect(kptextobject->getKTextObject(),SIGNAL(colorChanged(QColor*)),this,SLOT(toColorChanged(QColor*)));
	      connect(kptextobject->getKTextObject(),SIGNAL(horzAlignChanged(TxtParagraph::HorzAlign)),
		      this,SLOT(toAlignChanged(TxtParagraph::HorzAlign)));      
	      editNum = i;
	    }
	}
    }
}

/*====================== key press event =========================*/
void Page::keyPressEvent(QKeyEvent *e)
{
  if (!editMode)
    {
      switch (e->key())
	{
	case Key_Space: case Key_Right: case Key_Down:
	  view->screenNext(); break;
	case Key_Backspace: case Key_Left: case Key_Up:
	  view->screenPrev(); break;
	case Key_Escape: case Key_Q: case Key_X:
	  view->screenStop(); break;
	}
    }
}

/*========================= resize Event =========================*/
void Page::resizeEvent(QResizeEvent *e)
{
  if (editMode) QWidget::resizeEvent(e);
  else QWidget::resizeEvent(new QResizeEvent(QSize(QApplication::desktop()->width(),QApplication::desktop()->height()),
					     e->oldSize()));
}

/*========================== get object ==========================*/
int Page::getObjectAt(int x,int y)
{
  KPObject *kpobject = 0;

  for (int i = static_cast<int>(objectList()->count()) - 1;i >= 0 ;i--)
    {
      kpobject = objectList()->at(i);
      if (kpobject->contains(QPoint(x,y),diffx(),diffy()))
	return i;
    }
  
  return -1;
}

/*======================= select object ==========================*/
void Page::selectObj(int num)
{
  if (num < static_cast<int>(objectList()->count()))
    selectObj(objectList()->at(num));
}

/*======================= deselect object ========================*/
void Page::deSelectObj(int num)
{
  if (num < static_cast<int>(objectList()->count()))
    deSelectObj(objectList()->at(num));
}

/*======================= select object ==========================*/
void Page::selectObj(KPObject *kpobject)
{
  kpobject->setSelected(true);
  _repaint(kpobject);
}

/*======================= deselect object ========================*/
void Page::deSelectObj(KPObject *kpobject)
{
  kpobject->setSelected(false);
  _repaint(kpobject);
}

/*====================== select all objects ======================*/
void Page::selectAllObj()
{
  for (int i = 0;i <= static_cast<int>(objectList()->count());i++)
    selectObj(i);
}


/*==================== deselect all objects ======================*/
void Page::deSelectAllObj()
{
  KPObject *kpobject;
  
  for (int i = 0;i < static_cast<int>(objectList()->count());i++)
    {
      kpobject = objectList()->at(i);
      if (kpobject->isSelected()) deSelectObj(kpobject);
    }
  
}

/*======================== setup menus ===========================*/
void Page::setupMenus()
{
  QString pixdir;;
  QPixmap pixmap;

  // create right button object align menu
  pixdir = KApplication::kde_datadir();
  alignMenu1 = new QPopupMenu();
  CHECK_PTR(alignMenu1);
  pixmap.load(pixdir + "/kpresenter/toolbar/aoleft.xpm");
  alignMenu1->insertItem(pixmap,this,SLOT(alignObjLeft()));
  alignMenu1->insertSeparator();
  pixmap.load(pixdir + "/kpresenter/toolbar/aocenterh.xpm");
  alignMenu1->insertItem(pixmap,this,SLOT(alignObjCenterH()));
  alignMenu1->insertSeparator();
  pixmap.load(pixdir + "/kpresenter/toolbar/aoright.xpm");
  alignMenu1->insertItem(pixmap,this,SLOT(alignObjRight()));
  alignMenu1->insertSeparator();
  pixmap.load(pixdir + "/kpresenter/toolbar/aotop.xpm");
  alignMenu1->insertItem(pixmap,this,SLOT(alignObjTop()));
  alignMenu1->insertSeparator();
  pixmap.load(pixdir + "/kpresenter/toolbar/aocenterv.xpm");
  alignMenu1->insertItem(pixmap,this,SLOT(alignObjCenterV()));
  alignMenu1->insertSeparator();
  pixmap.load(pixdir + "/kpresenter/toolbar/aobottom.xpm");
  alignMenu1->insertItem(pixmap,this,SLOT(alignObjBottom()));
  alignMenu1->setMouseTracking(true);
  alignMenu1->setCheckable(false);

  pixdir = KApplication::kde_datadir();
  alignMenu2 = new QPopupMenu();
  CHECK_PTR(alignMenu2);
  pixmap.load(pixdir + "/kpresenter/toolbar/aoleft.xpm");
  alignMenu2->insertItem(pixmap,this,SLOT(alignObjLeft()));
  alignMenu2->insertSeparator();
  pixmap.load(pixdir + "/kpresenter/toolbar/aocenterh.xpm");
  alignMenu2->insertItem(pixmap,this,SLOT(alignObjCenterH()));
  alignMenu2->insertSeparator();
  pixmap.load(pixdir + "/kpresenter/toolbar/aoright.xpm");
  alignMenu2->insertItem(pixmap,this,SLOT(alignObjRight()));
  alignMenu2->insertSeparator();
  pixmap.load(pixdir + "/kpresenter/toolbar/aotop.xpm");
  alignMenu2->insertItem(pixmap,this,SLOT(alignObjTop()));
  alignMenu2->insertSeparator();
  pixmap.load(pixdir + "/kpresenter/toolbar/aocenterv.xpm");
  alignMenu2->insertItem(pixmap,this,SLOT(alignObjCenterV()));
  alignMenu2->insertSeparator();
  pixmap.load(pixdir + "/kpresenter/toolbar/aobottom.xpm");
  alignMenu2->insertItem(pixmap,this,SLOT(alignObjBottom()));
  alignMenu2->setMouseTracking(true);
  alignMenu2->setCheckable(false);

  pixdir = KApplication::kde_datadir();
  alignMenu3 = new QPopupMenu();
  CHECK_PTR(alignMenu3);
  pixmap.load(pixdir + "/kpresenter/toolbar/aoleft.xpm");
  alignMenu3->insertItem(pixmap,this,SLOT(alignObjLeft()));
  alignMenu3->insertSeparator();
  pixmap.load(pixdir + "/kpresenter/toolbar/aocenterh.xpm");
  alignMenu3->insertItem(pixmap,this,SLOT(alignObjCenterH()));
  alignMenu3->insertSeparator();
  pixmap.load(pixdir + "/kpresenter/toolbar/aoright.xpm");
  alignMenu3->insertItem(pixmap,this,SLOT(alignObjRight()));
  alignMenu3->insertSeparator();
  pixmap.load(pixdir + "/kpresenter/toolbar/aotop.xpm");
  alignMenu3->insertItem(pixmap,this,SLOT(alignObjTop()));
  alignMenu3->insertSeparator();
  pixmap.load(pixdir + "/kpresenter/toolbar/aocenterv.xpm");
  alignMenu3->insertItem(pixmap,this,SLOT(alignObjCenterV()));
  alignMenu3->insertSeparator();
  pixmap.load(pixdir + "/kpresenter/toolbar/aobottom.xpm");
  alignMenu3->insertItem(pixmap,this,SLOT(alignObjBottom()));
  alignMenu3->setMouseTracking(true);
  alignMenu3->setCheckable(false);

  pixdir = KApplication::kde_datadir();
  alignMenu4 = new QPopupMenu();
  CHECK_PTR(alignMenu4);
  pixmap.load(pixdir + "/kpresenter/toolbar/aoleft.xpm");
  alignMenu4->insertItem(pixmap,this,SLOT(alignObjLeft()));
  alignMenu4->insertSeparator();
  pixmap.load(pixdir + "/kpresenter/toolbar/aocenterh.xpm");
  alignMenu4->insertItem(pixmap,this,SLOT(alignObjCenterH()));
  alignMenu4->insertSeparator();
  pixmap.load(pixdir + "/kpresenter/toolbar/aoright.xpm");
  alignMenu4->insertItem(pixmap,this,SLOT(alignObjRight()));
  alignMenu4->insertSeparator();
  pixmap.load(pixdir + "/kpresenter/toolbar/aotop.xpm");
  alignMenu4->insertItem(pixmap,this,SLOT(alignObjTop()));
  alignMenu4->insertSeparator();
  pixmap.load(pixdir + "/kpresenter/toolbar/aocenterv.xpm");
  alignMenu4->insertItem(pixmap,this,SLOT(alignObjCenterV()));
  alignMenu4->insertSeparator();
  pixmap.load(pixdir + "/kpresenter/toolbar/aobottom.xpm");
  alignMenu4->insertItem(pixmap,this,SLOT(alignObjBottom()));
  alignMenu4->setMouseTracking(true);
  alignMenu4->setCheckable(false);

  pixdir = KApplication::kde_toolbardir();

  // create right button graph menu 
  graphMenu = new QPopupMenu();
  CHECK_PTR(graphMenu);
  pixmap.load(pixdir+"/editcut.xpm");
  graphMenu->insertItem(pixmap,i18n("&Cut"),this,SLOT(clipCut()));
  pixmap.load(pixdir+"/editcopy.xpm");
  graphMenu->insertItem(pixmap,i18n("C&opy"),this,SLOT(clipCopy()));
//   pixmap.load(pixdir+"/editpaste.xpm");
//   graphMenu->insertItem(pixmap,i18n("&Paste"),this,SLOT(clipPaste()));
  pixdir = KApplication::kde_datadir();
  pixmap.load(pixdir+"/kpresenter/toolbar/delete.xpm");
  graphMenu->insertItem(pixmap,i18n("&Delete"),this,SLOT(deleteObjs()));
  graphMenu->insertSeparator();
  pixmap.load(pixdir+"/kpresenter/toolbar/rotate.xpm");
  graphMenu->insertItem(pixmap,i18n("&Rotate..."),this,SLOT(rotateObjs()));
  pixmap.load(pixdir+"/kpresenter/toolbar/shadow.xpm");
  graphMenu->insertItem(pixmap,i18n("&Shadow..."),this,SLOT(shadowObjs()));
  graphMenu->insertSeparator();
  pixmap.load(pixdir+"/kpresenter/toolbar/style.xpm");
  graphMenu->insertItem(pixmap,i18n("&Properties..."),this,SLOT(objProperties()));
  graphMenu->insertSeparator();
  pixmap.load(pixdir+"/kpresenter/toolbar/effect.xpm");
  graphMenu->insertItem(pixmap,i18n("&Assign effect..."),this,SLOT(assignEffect()));
  graphMenu->insertSeparator();
  pixmap.load(pixdir+"/kpresenter/toolbar/alignobjs.xpm");
  graphMenu->insertItem(pixmap,i18n("&Align objects"),alignMenu1);
  graphMenu->setMouseTracking(true);

  // create right button picture menu
  pixdir = KApplication::kde_toolbardir();
  picMenu = new QPopupMenu();
  CHECK_PTR(picMenu);
  pixmap.load(pixdir+"/editcut.xpm");
  picMenu->insertItem(pixmap,i18n("&Cut"),this,SLOT(clipCut()));
  pixmap.load(pixdir+"/editcopy.xpm");
  picMenu->insertItem(pixmap,i18n("C&opy"),this,SLOT(clipCopy()));
//   pixmap.load(pixdir+"/editpaste.xpm");
//   picMenu->insertItem(pixmap,i18n("&Paste"),this,SLOT(clipPaste()));
  pixdir = KApplication::kde_datadir();
  pixmap.load(pixdir+"/kpresenter/toolbar/delete.xpm");
  picMenu->insertItem(pixmap,i18n("&Delete"),this,SLOT(deleteObjs()));
  picMenu->insertSeparator();
  pixmap.load(pixdir+"/kpresenter/toolbar/rotate.xpm");
  picMenu->insertItem(pixmap,i18n("&Rotate..."),this,SLOT(rotateObjs()));
  pixmap.load(pixdir+"/kpresenter/toolbar/shadow.xpm");
  picMenu->insertItem(pixmap,i18n("&Shadow..."),this,SLOT(shadowObjs()));
  picMenu->insertSeparator();
  pixmap.load(pixdir+"/kpresenter/toolbar/picture.xpm");
  picMenu->insertItem(pixmap,i18n("&Change Picture..."),this,SLOT(chPic()));
  picMenu->insertSeparator();
  pixmap.load(pixdir+"/kpresenter/toolbar/effect.xpm");
  picMenu->insertItem(pixmap,i18n("&Assign effect..."),this,SLOT(assignEffect()));
  picMenu->insertSeparator();
  pixmap.load(pixdir+"/kpresenter/toolbar/alignobjs.xpm");
  picMenu->insertItem(pixmap,i18n("&Align objects"),alignMenu2);
  picMenu->setMouseTracking(true);

  // create right button clipart menu 
  pixdir = KApplication::kde_toolbardir();
  clipMenu = new QPopupMenu();
  CHECK_PTR(clipMenu);
  pixmap.load(pixdir+"/editcut.xpm");
  clipMenu->insertItem(pixmap,i18n("&Cut"),this,SLOT(clipCut()));
  pixmap.load(pixdir+"/editcopy.xpm");
  clipMenu->insertItem(pixmap,i18n("C&opy"),this,SLOT(clipCopy()));
//   pixmap.load(pixdir+"/editpaste.xpm");
//   clipMenu->insertItem(pixmap,i18n("&Paste"),this,SLOT(clipPaste()));
  pixdir = KApplication::kde_datadir();
  pixmap.load(pixdir+"/kpresenter/toolbar/delete.xpm");
  clipMenu->insertItem(pixmap,i18n("&Delete"),this,SLOT(deleteObjs()));
  clipMenu->insertSeparator();
  pixmap.load(pixdir+"/kpresenter/toolbar/rotate.xpm");
  clipMenu->insertItem(pixmap,i18n("&Rotate..."),this,SLOT(rotateObjs()));
  pixmap.load(pixdir+"/kpresenter/toolbar/shadow.xpm");
  clipMenu->insertItem(pixmap,i18n("&Shadow..."),this,SLOT(shadowObjs()));
  clipMenu->insertSeparator();
  pixmap.load(pixdir+"/kpresenter/toolbar/clipart.xpm");
  clipMenu->insertItem(pixmap,i18n("&Change Clipart..."),this,SLOT(chClip()));
  clipMenu->insertSeparator();
  pixmap.load(pixdir+"/kpresenter/toolbar/effect.xpm");
  clipMenu->insertItem(pixmap,i18n("&Assign effect..."),this,SLOT(assignEffect()));
  clipMenu->insertSeparator();
  pixmap.load(pixdir+"/kpresenter/toolbar/alignobjs.xpm");
  clipMenu->insertItem(pixmap,i18n("&Align objects"),alignMenu3);
  clipMenu->setMouseTracking(true);

  // create right button text menu 
  pixdir = KApplication::kde_toolbardir();
  txtMenu = new QPopupMenu();
  CHECK_PTR(txtMenu);
  pixmap.load(pixdir+"/editcut.xpm");
  txtMenu->insertItem(pixmap,i18n("&Cut"),this,SLOT(clipCut()));
  pixmap.load(pixdir+"/editcopy.xpm");
  txtMenu->insertItem(pixmap,i18n("C&opy"),this,SLOT(clipCopy()));
//   pixmap.load(pixdir+"/editpaste.xpm");
//   txtMenu->insertItem(pixmap,i18n("&Paste"),this,SLOT(clipPaste()));
  pixdir = KApplication::kde_datadir();
  pixmap.load(pixdir+"/kpresenter/toolbar/delete.xpm");
  txtMenu->insertItem(pixmap,i18n("&Delete"),this,SLOT(deleteObjs()));
  txtMenu->insertSeparator();
  pixmap.load(pixdir+"/kpresenter/toolbar/rotate.xpm");
  txtMenu->insertItem(pixmap,i18n("&Rotate..."),this,SLOT(rotateObjs()));
  pixmap.load(pixdir+"/kpresenter/toolbar/shadow.xpm");
  txtMenu->insertItem(pixmap,i18n("&Shadow..."),this,SLOT(shadowObjs()));
  txtMenu->insertSeparator();
  pixmap.load(pixdir+"/kpresenter/toolbar/effect.xpm");
  txtMenu->insertItem(pixmap,i18n("&Assign effect..."),this,SLOT(assignEffect()));
  txtMenu->insertSeparator();
  pixmap.load(pixdir+"/kpresenter/toolbar/alignobjs.xpm");
  txtMenu->insertItem(pixmap,i18n("&Align objects"),alignMenu4);
  txtMenu->setMouseTracking(true);

  // create right button presentation menu 
  presMenu = new QPopupMenu();
  CHECK_PTR(presMenu);
  presMenu->setCheckable(true);
  PM_SM = presMenu->insertItem(i18n("&Switching mode"),this,SLOT(switchingMode()));
  PM_DM = presMenu->insertItem(i18n("&Drawing mode"),this,SLOT(drawingMode()));
  presMenu->setItemChecked(PM_SM,true);
  presMenu->setItemChecked(PM_DM,false);
  presMenu->setMouseTracking(true);
}

/*======================== clipboard cut =========================*/
void Page::clipCut()
{
  if (editNum != -1 && objectList()->at(editNum)->getType() == OT_TEXT)
    dynamic_cast<KPTextObject*>(objectList()->at(editNum))->getKTextObject()->cutRegion();
  view->editCut();
}

/*======================== clipboard copy ========================*/
void Page::clipCopy()
{
  if (editNum != -1 && objectList()->at(editNum)->getType() == OT_TEXT)
    dynamic_cast<KPTextObject*>(objectList()->at(editNum))->getKTextObject()->copyRegion();
  view->editCopy();
}

/*====================== clipboard paste =========================*/
void Page::clipPaste()
{
  if (editNum != -1 && objectList()->at(editNum)->getType() == OT_TEXT)
    dynamic_cast<KPTextObject*>(objectList()->at(editNum))->getKTextObject()->paste();
  view->editPaste();
}

/*======================= object properties  =====================*/
void Page::objProperties()
{
  view->extraPenBrush();
}

/*======================= change picture  ========================*/
void Page::chPic()
{
  KPObject *kpobject = 0;

  for (unsigned int i = 0;i < objectList()->count();i++)
    {
      kpobject = objectList()->at(i);
      if (kpobject->isSelected() && kpobject->getType() == OT_PICTURE)
	{
	  view->changePicture(i,dynamic_cast<KPPixmapObject*>(kpobject)->getFileName());
	  break;
	}
    }
}

/*======================= change clipart  ========================*/
void Page::chClip()
{
  KPObject *kpobject = 0;

  for (unsigned int i = 0;i < objectList()->count();i++)
    {
      kpobject = objectList()->at(i);
      if (kpobject->isSelected() && kpobject->getType() == OT_CLIPART)
	{
	  view->changeClipart(i,dynamic_cast<KPClipartObject*>(kpobject)->getFileName());
	  break;
	}
    }
}

/*======================= set text font ==========================*/
void Page::setTextFont(QFont *font)
{
  if (editNum != -1 && objectList()->at(editNum)->getType() == OT_TEXT)
    {
      dynamic_cast<KPTextObject*>(objectList()->at(editNum))->getKTextObject()->setFocus();
      dynamic_cast<KPTextObject*>(objectList()->at(editNum))->getKTextObject()->setFont(*font);
    }
}

/*======================= set text color =========================*/
void Page::setTextColor(QColor *color)
{
  if (editNum != -1 && objectList()->at(editNum)->getType() == OT_TEXT)
    {
      dynamic_cast<KPTextObject*>(objectList()->at(editNum))->getKTextObject()->setFocus();
      dynamic_cast<KPTextObject*>(objectList()->at(editNum))->getKTextObject()->setColor(*color);
    }
}

/*===================== set text alignment =======================*/
void Page::setTextAlign(TxtParagraph::HorzAlign align)
{
  if (editNum != -1 && objectList()->at(editNum)->getType() == OT_TEXT)
    dynamic_cast<KPTextObject*>(objectList()->at(editNum))->getKTextObject()->setHorzAlign(align);
}

/*====================== start screenpresentation ================*/
void Page::startScreenPresentation(bool zoom)
{
  KPObject *kpobject = 0;

  if (editNum != -1)
    {
      kpobject = objectList()->at(editNum);
      editNum = -1;
      if (kpobject->getType() == OT_TEXT)
	{
	  KPTextObject * kptextobject = dynamic_cast<KPTextObject*>(kpobject);
	  kpobject->deactivate();
	  kptextobject->getKTextObject()->clearFocus();
	  disconnect(kptextobject->getKTextObject(),SIGNAL(fontChanged(QFont*)),this,SLOT(toFontChanged(QFont*)));
	  disconnect(kptextobject->getKTextObject(),SIGNAL(colorChanged(QColor*)),this,SLOT(toColorChanged(QColor*)));
	  disconnect(kptextobject->getKTextObject(),SIGNAL(horzAlignChanged(TxtParagraph::HorzAlign)),
		     this,SLOT(toAlignChanged(TxtParagraph::HorzAlign)));
	  kptextobject->getKTextObject()->setShowCursor(false);
	  setFocusProxy(0);
	  setFocusPolicy(QWidget::NoFocus);
	}
    }

  int i;

  if (zoom)
    {
      float _presFaktW = static_cast<float>(width()) / static_cast<float>(getPageSize(0).width()) > 0.0 ? 
	static_cast<float>(width()) / static_cast<float>(getPageSize(0).width()) : 1.0;
      float _presFaktH = static_cast<float>(height()) / static_cast<float>(getPageSize(0).height()) > 0.0 ? 
	static_cast<float>(height()) / static_cast<float>(getPageSize(0).height()) : 1.0;
      _presFakt = min(_presFaktW,_presFaktH);
    }
  else _presFakt = 1.0;

  KPBackGround *kpbackground = 0;
  
  for (i = 0;i < static_cast<int>(backgroundList()->count());i++)
    {
      kpbackground = backgroundList()->at(i);
      kpbackground->setSize(getPageSize(i,_presFakt).width(),getPageSize(i,_presFakt).height());
      kpbackground->restore();
    }
  
  for (i = 0;i < static_cast<int>(objectList()->count());i++)
    {
      kpobject = objectList()->at(i);
      kpobject->zoom(_presFakt);
      kpobject->drawSelection(false);
    }

  currPresPage = 1;
  editMode = false;
  drawMode = false;
  presStepList = view->KPresenterDoc()->reorderPage(1,diffx(),diffy(),_presFakt);
  currPresStep = (int)(presStepList.first());
  subPresStep = 0;
  repaint(true);
  setFocusPolicy(QWidget::StrongFocus);
  setFocus();
  setCursor(blankCursor);
}

/*====================== stop screenpresentation =================*/
void Page::stopScreenPresentation()
{
  KPObject *kpobject = 0;
  KPBackGround *kpbackground = 0;
  int i;

  for (i = 0;i < static_cast<int>(objectList()->count());i++)
    {
      kpobject = objectList()->at(i);
      kpobject->zoomOrig();
      kpobject->drawSelection(true);
    }

  _presFakt = 1.0;

  for (i = 0;i < static_cast<int>(backgroundList()->count());i++)
    {
      kpbackground = backgroundList()->at(i);
      kpbackground->setSize(getPageSize(i).width(),getPageSize(i).height());
      kpbackground->restore();
    }

  goingBack = false;
  currPresPage = 1;
  editMode = true;
  repaint(true);
  setCursor(arrowCursor);
}

/*========================== next ================================*/
bool Page::pNext(bool)
{
  bool addSubPres = false;
  bool clearSubPres = false;

  goingBack = false;
  KPObject *kpobject = 0;
  
  if ((int*)(currPresStep) < presStepList.last())
    {
      for (int i = 0;i < static_cast<int>(objectList()->count());i++)
	{
	  kpobject = objectList()->at(i);
	  if (getPageOfObj(i,_presFakt) == static_cast<int>(currPresPage) 
	      && kpobject->getPresNum() == static_cast<int>(currPresStep)
	    && kpobject->getType() == OT_TEXT && kpobject->getEffect2() != EF2_NONE)
	    {
	      if (static_cast<int>(subPresStep) < kpobject->getSubPresSteps())
		addSubPres = true;
	      else
		clearSubPres = true;
	    }
	}
      
      if (addSubPres)
	{
	  subPresStep++;
	  doObjEffects();
	  return false;
	}
      else if (clearSubPres)
	subPresStep = 0;
      
      presStepList.find((int*)(currPresStep));
      currPresStep = (int)(presStepList.next());
    
      if (currPresStep == 0)
	{
	  QPainter p;
	  p.begin(this);
	  drawBackground(&p,QRect(0,0,kapp->desktop()->width(),kapp->desktop()->height()));
	  p.end();  
	}

      doObjEffects();
    }
  else
    {
      if (currPresPage+1 > pageNums())
	{
	  for (int i = 0;i < static_cast<int>(objectList()->count());i++)
	    {
	      kpobject = objectList()->at(i);
	      if (getPageOfObj(i,_presFakt) == static_cast<int>(currPresPage) 
		  && kpobject->getPresNum() == static_cast<int>(currPresStep)
		  && kpobject->getType() == OT_TEXT && kpobject->getEffect2() != EF2_NONE)
		{
		  if (static_cast<int>(subPresStep) < kpobject->getSubPresSteps())
		    {
		      if (static_cast<int>(subPresStep) < kpobject->getSubPresSteps())
			addSubPres = true;
		    }
		}
	    }

	  if (addSubPres)
	    {
	      subPresStep++;
	      doObjEffects();
	      return false;
	    }

	  emit stopPres();

	  presStepList = view->KPresenterDoc()->reorderPage(currPresPage,diffx(),diffy(),_presFakt);
	  currPresStep = (int)(presStepList.last());
	  doObjEffects();
	  return false;
	}

      for (int i = 0;i < static_cast<int>(objectList()->count());i++)
	{
	  kpobject = objectList()->at(i);
	  if (getPageOfObj(i,_presFakt) == static_cast<int>(currPresPage) 
	      && kpobject->getPresNum() == static_cast<int>(currPresStep)
	      && kpobject->getType() == OT_TEXT && kpobject->getEffect2() != EF2_NONE)
	    {
	      if (static_cast<int>(subPresStep) < kpobject->getSubPresSteps())
		addSubPres = true;
	      else
		clearSubPres = true;
	    }
	}

      if (addSubPres)
	{
	  subPresStep++;
	  doObjEffects();
	  return false;
	}
      else if (clearSubPres)
	subPresStep = 0;

      QPixmap _pix1(QApplication::desktop()->width(),QApplication::desktop()->height());
      drawPageInPix(_pix1,view->getDiffY());
      
      currPresPage++;
      presStepList = view->KPresenterDoc()->reorderPage(currPresPage,diffx(),diffy(),_presFakt);
      currPresStep = (int)(presStepList.first());
      
      QPixmap _pix2(QApplication::desktop()->width(),QApplication::desktop()->height());
      drawPageInPix(_pix2,view->getDiffY() + view->KPresenterDoc()->getPageSize(0,0,0,_presFakt).height()+10);
      
      changePages(_pix1,_pix2,backgroundList()->at(currPresPage - 2)->getPageEffect());
      
      return true;
    }
  return false;
}

/*====================== previous ================================*/
bool Page::pPrev(bool manual)
{
  goingBack = true;
  subPresStep = 0;

  if ((int*)(currPresStep) > presStepList.first())
    {
      presStepList.find((int*)(currPresStep));
      currPresStep = (int)(presStepList.prev());
      repaint(false);
      return false;
    }
  else
    {
      if (currPresPage-1 <= 0)
	{
	  presStepList = view->KPresenterDoc()->reorderPage(currPresPage,diffx(),diffy(),_presFakt);
	  currPresStep = (int)(presStepList.first());
	  repaint(false);
	  return false;
	}
      currPresPage--;
      presStepList = view->KPresenterDoc()->reorderPage(currPresPage,diffx(),diffy(),_presFakt);
      currPresStep = (int)(presStepList.last());
      return true;
    }

  return false;
}

/*======================== can we assign an effect ? =============*/
bool Page::canAssignEffect(int &pgNum,int &objNum)
{
  bool ret = false; 
  pgNum = -1; objNum = -1;
  KPObject *kpobject;

  for (int i = 0;i < static_cast<int>(objectList()->count());i++)
    {
      kpobject = objectList()->at(i);
      if (kpobject->isSelected())
	{
	  if (ret) return false;
	  ret = true;
	  objNum = i;
	  pgNum = getPageOfObj(objNum);
	}
    }
  return ret;
}
	    
/*==================== draw a page in a pixmap ===================*/ 
void Page::drawPageInPix(QPixmap &_pix,int __diffy)
{
  int _yOffset = view->getDiffY();
  view->setDiffY(__diffy);

  QPainter p;
  p.begin(&_pix);

  drawBackground(&p,_pix.rect());
  drawObjects(&p,_pix.rect());
  
  p.end();

  view->setDiffY(_yOffset);
}

/*==================== draw a page in a pixmap ===================*/ 
void Page::drawPageInPainter(QPainter* painter,int __diffy,QRect _rect)
{
  int _yOffset = view->getDiffY();
  view->setDiffY(__diffy);

  drawBackground(painter,_rect);
  drawObjects(painter,_rect);
  
  view->setDiffY(_yOffset);
}

/*=========================== change pages =======================*/
void Page::changePages(QPixmap _pix1,QPixmap _pix2,PageEffect _effect)
{
  QTime _time;
  int _step = 0,_steps,_h,_w,_x,_y;

  switch (_effect)
    {
    case PEF_NONE:
      {
	bitBlt(this,0,0,&_pix2,0,0,_pix2.width(),_pix2.height());
      } break;
    case PEF_CLOSE_HORZ:
      {
	_steps = static_cast<int>(50000.0 / static_cast<float>(kapp->desktop()->height()));
	_time.start();

	for (;;)
	  {
	    kapp->processEvents();
	    if (_time.elapsed() >= 1)
	      {
		_step++;
		_h = (_pix2.height()/(2 * _steps)) * _step;
		_h = _h > _pix2.height() / 2 ? _pix2.height() / 2 : _h;

		bitBlt(this,0,0,&_pix2,0,_pix2.height() / 2 - _h,width(),_h);
		bitBlt(this,0,height() - _h,&_pix2,0,_pix2.height() / 2,width(),_h);

		_time.restart();
	      }
	    if ((_pix2.height()/(2 * _steps)) * _step >= _pix2.height() / 2) break;
	  }
      } break;
    case PEF_CLOSE_VERT:
      {
	_steps = static_cast<int>(50000.0 / static_cast<float>(kapp->desktop()->width()));
	_time.start();

	for (;;)
	  {
	    kapp->processEvents();
	    if (_time.elapsed() >= 1)
	      {
		_step++;
		_w = (_pix2.width()/(2 * _steps)) * _step;
		_w = _w > _pix2.width() / 2 ? _pix2.width() / 2 : _w;

		bitBlt(this,0,0,&_pix2,_pix2.width() / 2 - _w,0,_w,height());
		bitBlt(this,width() - _w,0,&_pix2,_pix2.width() / 2,0,_w,height());

		_time.restart();
	      }
	    if ((_pix2.width()/(2 * _steps)) * _step >= _pix2.width() / 2) break;
	  }
      } break;
    case PEF_CLOSE_ALL:
      {
	_steps = static_cast<int>(50000.0 / static_cast<float>(kapp->desktop()->width()));
	_time.start();

	for (;;)
	  {
	    kapp->processEvents();
	    if (_time.elapsed() >= 1)
	      {
		_step++;
		_w = (_pix2.width()/(2 * _steps)) * _step;
		_w = _w > _pix2.width() / 2 ? _pix2.width() / 2 : _w;

		_h = (_pix2.height()/(2 * _steps)) * _step;
		_h = _h > _pix2.height() / 2 ? _pix2.height() / 2 : _h;

		bitBlt(this,0,0,&_pix2,0,0,_w,_h);
		bitBlt(this,width() - _w,0,&_pix2,width() - _w,0,_w,_h);
		bitBlt(this,0,height() - _h,&_pix2,0,height() - _h,_w,_h);
		bitBlt(this,width() - _w,height() - _h,&_pix2,width() - _w,height() - _h,_w,_h);

		_time.restart();
	      }
	    if ((_pix2.width()/(2 * _steps)) * _step >= _pix2.width() / 2
		&& (_pix2.height()/(2 * _steps)) * _step >= _pix2.height() / 2) break;
	  }
      } break;
    case PEF_OPEN_HORZ:
      {
	_steps = static_cast<int>(50000.0 / static_cast<float>(kapp->desktop()->height()));
	_time.start();

	for (;;)
	  {
	    kapp->processEvents();
	    if (_time.elapsed() >= 1)
	      {
		_step++;
		_h = (_pix2.height() / _steps) * _step;
		_h = _h > _pix2.height() ? _pix2.height() : _h;

		_y = _pix2.height() / 2;

		bitBlt(this,0,_y - _h / 2,&_pix2,0,_y - _h / 2,width(),_h);
	
		_time.restart();
	      }
	    if ((_pix2.height() / _steps) * _step >= _pix2.height()) break;
	  }
      } break;
    case PEF_OPEN_VERT:
      {
	_steps = static_cast<int>(50000.0 / static_cast<float>(kapp->desktop()->height()));
	_time.start();

	for (;;)
	  {
	    kapp->processEvents();
	    if (_time.elapsed() >= 1)
	      {
		_step++;
		_w = (_pix2.width() / _steps) * _step;
		_w = _w > _pix2.width() ? _pix2.width() : _w;

		_x = _pix2.width() / 2;

		bitBlt(this,_x - _w / 2,0,&_pix2,_x - _w / 2,0,_w,height());
	
		_time.restart();
	      }
	    if ((_pix2.width() / _steps) * _step >= _pix2.width()) break;
	  }
      } break;
    case PEF_OPEN_ALL:
      {
	_steps = static_cast<int>(50000.0 / static_cast<float>(kapp->desktop()->height()));
	_time.start();

	for (;;)
	  {
	    kapp->processEvents();
	    if (_time.elapsed() >= 1)
	      {
		_step++;
		_w = (_pix2.width() / _steps) * _step;
		_w = _w > _pix2.width() ? _pix2.width() : _w;

		_x = _pix2.width() / 2;

		_h = (_pix2.height() / _steps) * _step;
		_h = _h > _pix2.height() ? _pix2.height() : _h;

		_y = _pix2.height() / 2;

		bitBlt(this,_x - _w / 2,_y - _h / 2,&_pix2,_x - _w / 2,_y - _h / 2,_w,_h);
	
		_time.restart();
	      }
	    if ((_pix2.width() / _steps) * _step >= _pix2.width()) break;
	  }
      } break;
    case PEF_INTERLOCKING_HORZ_1:
      {
	_steps = static_cast<int>(50000.0 / static_cast<float>(kapp->desktop()->width()));
	_time.start();

	for (;;)
	  {
	    kapp->processEvents();
	    if (_time.elapsed() >= 1)
	      {
		_step++;
		_w = (_pix2.width() / _steps) * _step;
		_w = _w > _pix2.width() ? _pix2.width() : _w;

		bitBlt(this,0,0,&_pix2,0,0,_w,height() / 4);
		bitBlt(this,0,height() / 2,&_pix2,0,height() / 2,_w,height() / 4);
		bitBlt(this,width() - _w,height() / 4,&_pix2,width() - _w,height() / 4,_w,height() / 4);
		bitBlt(this,width() - _w,height() / 2 + height() / 4,&_pix2,width() - _w,
		       height() / 2 + height() / 4,_w,height() / 4);

		_time.restart();
	      }
	    if ((_pix2.width() / _steps) * _step >= _pix2.width()) break;
	  }
      } break;
    case PEF_INTERLOCKING_HORZ_2:
      {
	_steps = static_cast<int>(50000.0 / static_cast<float>(kapp->desktop()->width()));
	_time.start();

	for (;;)
	  {
	    kapp->processEvents();
	    if (_time.elapsed() >= 1)
	      {
		_step++;
		_w = (_pix2.width() / _steps) * _step;
		_w = _w > _pix2.width() ? _pix2.width() : _w;

		bitBlt(this,0,height() / 4,&_pix2,0,height() / 4,_w,height() / 4);
		bitBlt(this,0,height() / 2 + height() / 4,&_pix2,0,height() / 2 + height() / 4,_w,height() / 4);
		bitBlt(this,width() - _w,0,&_pix2,width() - _w,0,_w,height() / 4);
		bitBlt(this,width() - _w,height() / 2,&_pix2,width() - _w,height() / 2,_w,height() / 4);

		_time.restart();
	      }
	    if ((_pix2.width() / _steps) * _step >= _pix2.width()) break;
	  }
      } break;
    case PEF_INTERLOCKING_VERT_1:
      {
	_steps = static_cast<int>(50000.0 / static_cast<float>(kapp->desktop()->height()));
	_time.start();

	for (;;)
	  {
	    kapp->processEvents();
	    if (_time.elapsed() >= 1)
	      {
		_step++;
		_h = (_pix2.height() / _steps) * _step;
		_h = _h > _pix2.height() ? _pix2.height() : _h;

		bitBlt(this,0,0,&_pix2,0,0,width() / 4,_h);
		bitBlt(this,width() / 2,0,&_pix2,width() / 2,0,width() / 4,_h);
		bitBlt(this,width() / 4,height() - _h,&_pix2,width() / 4,height() - _h,width() / 4,_h);
		bitBlt(this,width() / 2 + width() / 4,height() - _h,&_pix2,width() / 2 + width() / 4,height() - _h,
		       width() / 4,_h);

		_time.restart();
	      }
	    if ((_pix2.height() / _steps) * _step >= _pix2.height()) break;
	  }
      } break;
    case PEF_INTERLOCKING_VERT_2:
      {
	_steps = static_cast<int>(50000.0 / static_cast<int>(kapp->desktop()->height()));
	_time.start();

	for (;;)
	  {
	    kapp->processEvents();
	    if (_time.elapsed() >= 1)
	      {
		_step++;
		_h = (_pix2.height() / _steps) * _step;
		_h = _h > _pix2.height() ? _pix2.height() : _h;

		bitBlt(this,width() / 4,0,&_pix2,width() / 4,0,width() / 4,_h);
		bitBlt(this,width() / 2 + width() / 4,0,&_pix2,width() / 2 + width() / 4,0,width() / 4,_h);
		bitBlt(this,0,height() - _h,&_pix2,0,height() - _h,width() / 4,_h);
		bitBlt(this,width() / 2,height() - _h,&_pix2,width() / 2,height() - _h,width() / 4,_h);

		_time.restart();
	      }
	    if ((_pix2.height() / _steps) * _step >= _pix2.height()) break;
	  }
      } break;
    }
}

/*======================= do object effects ======================*/
void Page::doObjEffects()
{
  QPixmap screen_orig(kapp->desktop()->width(),kapp->desktop()->height());
  QList<KPObject> _objList;
  int i;
  QTime _time;
  int _step = 0,_steps1 = 0,_steps2 = 0,x_pos1 = 0,y_pos1 = 0;
  int x_pos2 = kapp->desktop()->width(),y_pos2 = kapp->desktop()->height(),_step_width = 0,_step_height = 0;
  int w_pos1 = 0,h_pos1;
  bool effects = false;
  bool nothingHappens = false;

  bitBlt(&screen_orig,0,0,this,0,0,kapp->desktop()->width(),kapp->desktop()->height());
  QPixmap *screen = new QPixmap(screen_orig);

  KPObject *kpobject = 0;
  
  for (i = 0;i < static_cast<int>(objectList()->count());i++)
    {
      kpobject = objectList()->at(i);
      if (getPageOfObj(i,_presFakt) == static_cast<int>(currPresPage) 
	  && kpobject->getPresNum() == static_cast<int>(currPresStep))
	{
	  if (kpobject->getEffect() != EF_NONE)
	    {
	      _objList.append(kpobject);

	      int x,y,w,h;
	      QRect br = kpobject->getBoundingRect(0,0);
	      x = br.x(); y = br.y(); w = br.width(); h = br.height();

	      switch (kpobject->getEffect())
		{
		case EF_COME_LEFT:
		  x_pos1 = max(x_pos1,x - diffx() + w);
		  break;
		case EF_COME_TOP:
		  y_pos1 = max(y_pos1,y - diffy() + h);
		  break;
		case EF_COME_RIGHT:
		  x_pos2 = min(x_pos2,x - diffx());
		  break;
		case EF_COME_BOTTOM:
		  y_pos2 = min(y_pos2,y - diffy());
		  break;
		case EF_COME_LEFT_TOP:
		  {
		    x_pos1 = max(x_pos1,x - diffx() + w);
		    y_pos1 = max(y_pos1,y - diffy() + h);
		  } break;
		case EF_COME_LEFT_BOTTOM:
		  {
		    x_pos1 = max(x_pos1,x - diffx() + w);
		    y_pos2 = min(y_pos2,y - diffy());
		  } break;
		case EF_COME_RIGHT_TOP:
		  {
		    x_pos2 = min(x_pos2,x - diffx());
		    y_pos1 = max(y_pos1,y - diffy() + h);
		  } break;
		case EF_COME_RIGHT_BOTTOM:
		  {
		    x_pos2 = min(x_pos2,x - diffx());
		    y_pos2 = min(y_pos2,y - diffy());
		  } break;
		case EF_WIPE_LEFT:
		  x_pos1 = max(x_pos1,w);
		  break;
		case EF_WIPE_RIGHT:
		  x_pos1 = max(x_pos1,w);
		  break;
		case EF_WIPE_TOP:
		  y_pos1 = max(y_pos1,h);
		  break;
		case EF_WIPE_BOTTOM:
		  y_pos1 = max(y_pos1,h);
		  break;
		default: break;
		}
	      effects = true;
	    }
	}
    }
  
  if (effects)
    {
      _step_width = static_cast<int>((20.0 * static_cast<float>(kapp->desktop()->width())) / 1000.0);
      _step_height = static_cast<int>((20.0 * static_cast<float>(kapp->desktop()->height())) / 1000.0);
      _steps1 = x_pos1 > y_pos1 ? x_pos1 / _step_width : y_pos1 / _step_height;
      _steps2 = kapp->desktop()->width() - x_pos2 > kapp->desktop()->height() - y_pos2 ?
	(kapp->desktop()->width() - x_pos2) / _step_width : (kapp->desktop()->height() - y_pos2) / _step_height;
      _time.start();

      for (;;)
	{
	  kapp->processEvents();
	  if (nothingHappens) break; // || _step >= _steps1 && _step >= _steps2) break;
	  
	  if (_time.elapsed() >= 1)
	    {
	      nothingHappens = true;
	      _step++;
	      
	      for (i = 0;i < static_cast<int>(_objList.count());i++)
		{
		  kpobject = _objList.at(i);
		  int _w =  kapp->desktop()->width() - (kpobject->getOrig().x() - diffx());
		  int _h =  kapp->desktop()->height() - (kpobject->getOrig().y() - diffy());
		  int ox,oy,ow,oh;
		  ox = kpobject->getOrig().x();
		  oy = kpobject->getOrig().y();
		  ow = kpobject->getSize().width();
		  oh = kpobject->getSize().height();

		  switch (kpobject->getEffect())
		    {
		    case EF_COME_LEFT:
		      {
			if (subPresStep == 0 || subPresStep != 0 && kpobject->getType() == OT_TEXT && kpobject->getEffect2() == EF2T_PARA)
			  { 
			    x_pos1 = _step_width * _step < ox - diffx() + ow ? 
			      ox - diffx() + ow - _step_width * _step : 0;
			    y_pos1 = 0;
			    drawObject(kpobject,screen,-x_pos1,y_pos1,0,0,0,0);
			    if (x_pos1 != 0) nothingHappens = false;
			  }
		      } break;
		    case EF_COME_TOP:
		      {
			if (subPresStep == 0 || subPresStep != 0 && kpobject->getType() == OT_TEXT && kpobject->getEffect2() == EF2T_PARA)
			  { 
			    y_pos1 = _step_height * _step < oy - diffy() + oh ?
			      oy - diffy() + oh - _step_height * _step : 0;
			    x_pos1 = 0;
			    drawObject(kpobject,screen,x_pos1,-y_pos1,0,0,0,0);
			    if (y_pos1 != 0) nothingHappens = false;
			  }
		      } break;
		    case EF_COME_RIGHT:
		      {
			if (subPresStep == 0 || subPresStep != 0 && kpobject->getType() == OT_TEXT && kpobject->getEffect2() == EF2T_PARA)
			  { 
			    x_pos2 = _w - (_step_width * _step) + (ox - diffx()) > ox - diffx() ?
			      _w - (_step_width * _step) : 0;
			    y_pos2 = 0;
			    drawObject(kpobject,screen,x_pos2,y_pos2,0,0,0,0);
			    if (x_pos2 != 0) nothingHappens = false;
			  }
		      } break;
		    case EF_COME_BOTTOM:
		      {
			if (subPresStep == 0 || subPresStep != 0 && kpobject->getType() == OT_TEXT && kpobject->getEffect2() == EF2T_PARA)
			  { 
			    y_pos2 = _h - (_step_height * _step) + (oy - diffy()) > oy - diffy() ?
			      _h - (_step_height * _step) : 0;
			    x_pos2 = 0;
			    drawObject(kpobject,screen,x_pos2,y_pos2,0,0,0,0);
			    if (y_pos2 != 0) nothingHappens = false;
			  }
		      } break;
		    case EF_COME_LEFT_TOP:
		      {
			if (subPresStep == 0 || subPresStep != 0 && kpobject->getType() == OT_TEXT && kpobject->getEffect2() == EF2T_PARA)
			  { 
			    x_pos1 = _step_width * _step < ox - diffx() + ow ? 
			      ox - diffx() + ow - _step_width * _step : 0;
			    y_pos1 = _step_height * _step < oy - diffy() + oh ?
			      oy - diffy() + oh - _step_height * _step : 0;
			    drawObject(kpobject,screen,-x_pos1,-y_pos1,0,0,0,0);
			    if (x_pos1 != 0 || y_pos1 != 0) nothingHappens = false;
			  }
		      } break;
		    case EF_COME_LEFT_BOTTOM:
		      {
			if (subPresStep == 0 || subPresStep != 0 && kpobject->getType() == OT_TEXT && kpobject->getEffect2() == EF2T_PARA)
			  { 
			    x_pos1 = _step_width * _step < ox - diffx() + ow ? 
			      ox - diffx() + ow - _step_width * _step : 0;
			    y_pos2 = _h - (_step_height * _step) + (oy - diffy()) > oy - diffy() ?
			      _h - (_step_height * _step) : 0;
			    drawObject(kpobject,screen,-x_pos1,y_pos2,0,0,0,0);
			    if (x_pos1 != 0 || y_pos2 != 0) nothingHappens = false;
			  }
		      } break;
		    case EF_COME_RIGHT_TOP:
		      {
			if (subPresStep == 0 || subPresStep != 0 && kpobject->getType() == OT_TEXT && kpobject->getEffect2() == EF2T_PARA)
			  { 
			    x_pos2 = _w - (_step_width * _step) + (ox - diffx()) > ox - diffx() ?
			      _w - (_step_width * _step) : 0;
			    y_pos1 = _step_height * _step < oy - diffy() + oh ?
			      oy - diffy() + oh - _step_height * _step : 0;
			    drawObject(kpobject,screen,x_pos2,-y_pos1,0,0,0,0);
			    if (x_pos2 != 0 || y_pos1 != 0) nothingHappens = false;
			  }
		      } break;
		    case EF_COME_RIGHT_BOTTOM:
		      {
			if (subPresStep == 0 || subPresStep != 0 && kpobject->getType() == OT_TEXT && kpobject->getEffect2() == EF2T_PARA)
			  { 
			    x_pos2 = _w - (_step_width * _step) + (ox - diffx()) > ox - diffx() ?
			      _w - (_step_width * _step) : 0;
			    y_pos2 = _h - (_step_height * _step) + (oy - diffy()) > oy - diffy() ?
			      _h - (_step_height * _step) : 0;
			    drawObject(kpobject,screen,x_pos2,y_pos2,0,0,0,0);
			    if (x_pos2 != 0 || y_pos2 != 0) nothingHappens = false;
			  }
		      } break;
		    case EF_WIPE_LEFT:
		      {
			if (subPresStep == 0 || subPresStep != 0 && kpobject->getType() == OT_TEXT && kpobject->getEffect2() == EF2T_PARA)
			  { 
			    w_pos1 = _step_width * (_steps1 - _step) > 0 ? _step_width * (_steps1 - _step) : 0;
			    drawObject(kpobject,screen,0,0,w_pos1,0,0,0);
 			    if (w_pos1 != 0) nothingHappens = false;
			  }
		      } break;
		    case EF_WIPE_RIGHT:
		      {
			if (subPresStep == 0 || subPresStep != 0 && kpobject->getType() == OT_TEXT && kpobject->getEffect2() == EF2T_PARA)
			  { 
			    w_pos1 = _step_width * (_steps1 - _step) > 0 ? _step_width * (_steps1 - _step) : 0;
			    x_pos1 = w_pos1;
			    drawObject(kpobject,screen,0,0,w_pos1,0,x_pos1,0);
 			    if (w_pos1 != 0) nothingHappens = false;
			  }
		      } break;
		    case EF_WIPE_TOP:
		      {
			if (subPresStep == 0 || subPresStep != 0 && kpobject->getType() == OT_TEXT && kpobject->getEffect2() == EF2T_PARA)
			  { 
			    h_pos1 = _step_height * (_steps1 - _step) > 0 ? _step_height * (_steps1 - _step) : 0;
			    drawObject(kpobject,screen,0,0,0,h_pos1,0,0);
 			    if (h_pos1 != 0) nothingHappens = false;
			  }
		      } break;
		    case EF_WIPE_BOTTOM:
		      {
			if (subPresStep == 0 || subPresStep != 0 && kpobject->getType() == OT_TEXT && kpobject->getEffect2() == EF2T_PARA)
			  { 
			    h_pos1 = _step_height * (_steps1 - _step) > 0 ? _step_height * (_steps1 - _step) : 0;
			    y_pos1 = h_pos1;
			    drawObject(kpobject,screen,0,0,0,h_pos1,0,y_pos1);
 			    if (h_pos1 != 0) nothingHappens = false;
			  }
		      } break;
		    default: break;
		    }
		}
	      
	      bitBlt(this,0,0,screen);
	      delete screen;
	      screen = new QPixmap(screen_orig);
	      
	      _time.restart();
	    }
	}
    }

  if (!effects)
    {
      QPainter p;
      p.begin(this);
      p.drawPixmap(0,0,screen_orig);
      drawObjects(&p,QRect(0,0,kapp->desktop()->width(),kapp->desktop()->height()));
      p.end();  
    }
  else
    {
      QPainter p;
      p.begin(screen);
      drawObjects(&p,QRect(0,0,kapp->desktop()->width(),kapp->desktop()->height()));
      p.end();
      bitBlt(this,0,0,screen);
    }

  delete screen;
}

/*======================= draw object ============================*/
void Page::drawObject(KPObject *kpobject,QPixmap *screen,int _x,int _y,int _w,int _h,int _cx,int _cy)
{
  int ox,oy,ow,oh;
  QRect br = kpobject->getBoundingRect(0,0);
  ox = br.x(); oy = br.y(); ow = br.width(); oh = br.height();
  bool ownClipping = true;

  QPainter p;
  p.begin(screen);

  if (_w != 0 || _h != 0)
    {
      p.setClipping(true);
      p.setClipRect(ox - diffx() + _cx,oy - diffy() + _cy,ow - _w,oh - _h);
      ownClipping = false;
    }
  
  if (!editMode && static_cast<int>(currPresStep) == kpobject->getPresNum() && !goingBack)
    {
      kpobject->setSubPresStep(subPresStep);
      kpobject->doSpecificEffects(true);
      kpobject->setOwnClipping(ownClipping);
    }
  kpobject->draw(&p,diffx() - _x,diffy() - _y);
  kpobject->setSubPresStep(0);
  kpobject->doSpecificEffects(false);
  kpobject->setOwnClipping(true);

  p.end();
}

/*======================== print =================================*/
void Page::print(QPainter *painter,QPrinter *printer,float left_margin,float top_margin)
{
  repaint(false);
  kapp->processEvents();

  editMode = false;
  fillBlack = false;
  _presFakt = 1.0;

  int _xOffset = view->getDiffX();
  int _yOffset = view->getDiffY();

  currPresStep = 1000;
  subPresStep = 1000;

  view->setDiffX(-(view->KPresenterDoc()->pageLayout().left - 5 + left_margin) * static_cast<int>((MM_TO_POINT * 100) / 100));
  view->setDiffY(10);
  view->setDiffY(diffy() - ((view->KPresenterDoc()->pageLayout().top - 5 + top_margin) * static_cast<int>((MM_TO_POINT * 100) / 100)));

  QColor c = kapp->winStyleHighlightColor();
  kapp->setWinStyleHighlightColor(kapp->selectColor);

  QProgressBar progBar;

  QProgressDialog progress(i18n("Printing..."),i18n("Cancel"),printer->toPage() - printer->fromPage() + 2,this);
  int j = 0;
  progress.setProgress(0);

  if (printer->fromPage() > 1)
    view->setDiffY(diffy() + (printer->fromPage() - 1) * (getPageSize(1).height() + 10));

  for (int i = printer->fromPage();i <= printer->toPage();i++)
    {
      progress.setProgress(++j);
      kapp->processEvents();

      if (progress.wasCancelled())
	break;

      currPresPage = i;
      if (i > printer->fromPage()) printer->newPage();

      painter->resetXForm();
      painter->fillRect(getPageSize(i - 1),white);

      drawPageInPainter(painter,view->getDiffY(),getPageSize(i - 1));
      kapp->processEvents();

      painter->resetXForm();
      view->presentParts(1.0,painter,getPageSize(i - 1),diffx(),diffy());
      kapp->processEvents();

      view->setDiffY(diffy() + getPageSize(i - 1).height() + 10);
    }

  setCursor(arrowCursor);
  view->setDiffX(_xOffset);
  view->setDiffY(_yOffset);
  
  progress.setProgress(printer->toPage() - printer->fromPage() + 2);
  kapp->setWinStyleHighlightColor(c);

  currPresPage = 1;
  currPresStep = 0;
  subPresStep = 0;
  _presFakt = 1.0;
  fillBlack = true;
  editMode = true;
  repaint(false);
}

