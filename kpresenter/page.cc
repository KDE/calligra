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
/* Module: Page                                                   */
/******************************************************************/

#include <qprinter.h>
#include <qprogdlg.h>
#include <qprogbar.h>
#include "page.h"
#include "page.moc"
#include <qdragobject.h>
#include <qfile.h>
#include <qtextstream.h>

#include <kmimemagic.h>
#include <kio_job.h>
#include <kurl.h>
#include <klocale.h>

/******************************************************************/
/* class Page - Page                                              */
/******************************************************************/

/*====================== constructor =============================*/
Page::Page(QWidget *parent=0,const char *name=0,KPresenterView *_view=0)
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
      toolEditMode = TEM_MOUSE;
      tmpObjs.setAutoDelete(false);
      setAcceptDrops(true);
    }
  else
    {
      view = 0;
      hide();
    }

  setFocusPolicy(QWidget::StrongFocus);
  setKeyCompression(true);
}

/*======================== destructor ============================*/
Page::~Page()
{
}

/*============================ draw contents ====================*/
void Page::draw(KRect _rect,QPainter *p)
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
void Page::drawBackground(QPainter *painter,KRect rect)
{
  KPBackGround *kpbackground = 0;

  for (int i = 0;i < static_cast<int>(backgroundList()->count());i++)
    {
      kpbackground = backgroundList()->at(i);
      if ((rect.intersects(KRect(getPageSize(i,_presFakt))) && editMode) ||
  	  (!editMode && static_cast<int>(currPresPage) == i + 1))
	{
	  if (editMode)
	    kpbackground->draw(painter,KPoint(getPageSize(i,_presFakt).x(),
					      getPageSize(i,_presFakt).y()),editMode);
	  else
	    kpbackground->draw(painter,KPoint(getPageSize(i,_presFakt,false).x() + view->kPresenterDoc()->getLeftBorder() * _presFakt,
					      getPageSize(i,_presFakt,false).y() + view->kPresenterDoc()->getTopBorder() * _presFakt),
			       editMode);
	}
    }
}

/*========================= draw objects =========================*/
void Page::drawObjects(QPainter *painter,KRect rect)
{
  KPObject *kpobject = 0;

  for (int i = 0;i < static_cast<int>(objectList()->count());i++)
    {
      kpobject = objectList()->at(i);

      if ((rect.intersects(kpobject->getBoundingRect(diffx(i),diffy(i))) && editMode) ||
	   (!editMode && getPageOfObj(i,_presFakt) == static_cast<int>(currPresPage) &&
	    kpobject->getPresNum() <= static_cast<int>(currPresStep)))
	{
	  if (!editMode && static_cast<int>(currPresStep) == kpobject->getPresNum() && !goingBack)
	    {
	      kpobject->setSubPresStep(subPresStep);
	      kpobject->doSpecificEffects(true,false);
	    }

	  kpobject->draw(painter,diffx(i),diffy(i));
	  kpobject->setSubPresStep(0);
	  kpobject->doSpecificEffects(false);
	}
    }
}

/*==================== handle mouse pressed ======================*/
void Page::mousePressEvent(QMouseEvent *e)
{
  //setFocus();

  view->sendFocusEvent();

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
	  disconnect(kptextobject->getKTextObject(),SIGNAL(giveMeFocus()),view,SLOT(sendFocusEvent()));
	  disconnect(kptextobject->getKTextObject(),SIGNAL(colorChanged(QColor*)),this,SLOT(toColorChanged(QColor*)));
	  disconnect(kptextobject->getKTextObject(),SIGNAL(horzAlignChanged(TxtParagraph::HorzAlign)),
		     this,SLOT(toAlignChanged(TxtParagraph::HorzAlign)));
	  kptextobject->getKTextObject()->setShowCursor(false);
	  //setFocusProxy(0);
	  //setFocusPolicy(QWidget::NoFocus);
	}
      else if (kpobject->getType() == OT_PART)
	{
	  kpobject->deactivate();
	  _repaint(kpobject);
	  //setFocusProxy(0);
	  //setFocusPolicy(QWidget::NoFocus);
	  return;
	}
    }

  if (editMode)
    {
      if (e->button() == LeftButton)
	{
	  mousePressed = true;

	  switch (toolEditMode)
	    {
	    case TEM_MOUSE:
	      {
		bool overObject = false;
		bool deSelAll = true;
		KPObject *kpobject = 0;
		
		firstX = e->x();
		firstY = e->y();
		
		if (objectList()->count() - 1 >= 0)
		  {
		    for (int i = static_cast<int>(objectList()->count()) - 1;i >= 0 ;i--)
		      {
			kpobject = objectList()->at(i);
			if (kpobject->contains(KPoint(e->x(),e->y()),diffx(),diffy()))
			  {
			    overObject = true;
			    if (kpobject->isSelected() && modType == MT_MOVE) deSelAll = false;
			    if (kpobject->isSelected() && modType != MT_MOVE && modType != MT_NONE)
			      {
				oldBoundingRect = kpobject->getBoundingRect(0,0);
				resizeObjNum = i;
			      }
			    break;
			  }
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
		    rubber = KRect(e->x(),e->y(),0,0);
		  }
	      } break;
	    default:
	      {
		deSelectAllObj();
		mousePressed = true;
		insRect = KRect(((e->x() + diffx()) / rastX()) * rastX() - diffx(),((e->y() + diffy()) / rastY()) * rastY() - diffy(),0,0);
	      } break;
	    }
	}
      if (e->button() == RightButton && toolEditMode == TEM_MOUSE)
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
		  KPoint pnt = QCursor::pos();
		  picMenu->popup(pnt);
		  modType = MT_NONE;
		}
	      else if (kpobject->getType() == OT_CLIPART)
		{
		  mousePressed = false;
		  deSelectAllObj();
		  selectObj(kpobject);
		  KPoint pnt = QCursor::pos();
		  clipMenu->popup(pnt);
		  modType = MT_NONE;
		}
	      else if (kpobject->getType() == OT_TEXT)
		{
		  if (!(e->state() & ShiftButton) && !(e->state() & ControlButton) && !kpobject->isSelected())
		    deSelectAllObj();
		  selectObj(kpobject);
		  KPoint pnt = QCursor::pos();
		  txtMenu->popup(pnt);
		  mousePressed = false;
		  modType = MT_NONE;
		}
	      else if (kpobject->getType() == OT_PIE)
		{
		  if (!(e->state() & ShiftButton) && !(e->state() & ControlButton) && !kpobject->isSelected())
		    deSelectAllObj();
		  selectObj(kpobject);
		  KPoint pnt = QCursor::pos();
		  pieMenu->popup(pnt);
		  mousePressed = false;
		  modType = MT_NONE;
		}
	      else if (kpobject->getType() == OT_RECT)
		{
		  if (!(e->state() & ShiftButton) && !(e->state() & ControlButton) && !kpobject->isSelected())
		    deSelectAllObj();
		  selectObj(kpobject);
		  KPoint pnt = QCursor::pos();
		  rectMenu->popup(pnt);
		  mousePressed = false;
		  modType = MT_NONE;
		}
	      else if (kpobject->getType() == OT_PART)
		{
		  if (!(e->state() & ShiftButton) && !(e->state() & ControlButton) && !kpobject->isSelected())
		    deSelectAllObj();
		  selectObj(kpobject);
		  KPoint pnt = QCursor::pos();
		  partMenu->popup(pnt);
		  mousePressed = false;
		  modType = MT_NONE;
		}
	      else
		{
		  if (!(e->state() & ShiftButton) && !(e->state() & ControlButton) && !kpobject->isSelected())
		    deSelectAllObj();
		  selectObj(kpobject);
		  KPoint pnt = QCursor::pos();
		  graphMenu->popup(pnt);
		  mousePressed = false;
		  modType = MT_NONE;
		}
	    }
	  else
	    {
	      KPoint pnt = QCursor::pos();
	      pageMenu->popup(pnt);
	      mousePressed = false;
	      modType = MT_NONE;
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
	  KPoint pnt = QCursor::pos();
	  presMenu->popup(pnt);
	}
    }

  mouseMoveEvent(e);
}

/*=================== handle mouse released ======================*/
void Page::mouseReleaseEvent(QMouseEvent *e)
{
  if (e->button() != LeftButton) return;

  int mx = e->x();
  int my = e->y();
  mx = (mx / rastX()) * rastX();
  my = (my / rastY()) * rastY();
  firstX = (firstX / rastX()) * rastX();
  firstY = (firstY / rastY()) * rastY();
  QList<KPObject> _objects;
  _objects.setAutoDelete(false);
  KPObject *kpobject = 0;

  if (toolEditMode != INS_LINE)
    insRect = insRect.normalize();

  switch (toolEditMode)
    {
    case TEM_MOUSE:
      {
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
		  if (objectList()->count() - 1 >= 0)
		    {
		      for (int i = static_cast<int>(objectList()->count()) - 1;i >= 0;i--)
			{
			  kpobject = objectList()->at(i);
			  if (rubber.intersects(kpobject->getBoundingRect(diffx(),diffy())))
			    selectObj(kpobject);
			}
		    }
		}
	    } break;
	  case MT_MOVE:
	    {
	      if (firstX != mx || firstY != my)
		{
		  if (objectList()->count() - 1 >= 0)
		    {
		      for (int i = static_cast<int>(objectList()->count()) - 1;i >= 0;i--)
			{
			  kpobject = objectList()->at(i);
			  if (kpobject->isSelected())
			    {
			      kpobject->setMove(false);
			      _objects.append(kpobject);
			      _repaint(KRect(kpobject->getBoundingRect(0,0).x() + (firstX - mx),
					     kpobject->getBoundingRect(0,0).y() + (firstY - my),
					     kpobject->getBoundingRect(0,0).width(),
					     kpobject->getBoundingRect(0,0).height()));
			      _repaint(kpobject);
			    }
			}
		    }
		  MoveByCmd *moveByCmd = new MoveByCmd(i18n("Move object(s)"),KPoint(mx - firstX,my - firstY),
						       _objects,view->kPresenterDoc());
		  view->kPresenterDoc()->commands()->addCommand(moveByCmd);
		}
	      else
		if (objectList()->count() - 1 >= 0)
		  {
		    for (int i = static_cast<int>(objectList()->count()) - 1;i >= 0;i--)
		      {
			kpobject = objectList()->at(i);
			if (kpobject->isSelected())
			  {
			    kpobject->setMove(false);
			    _repaint(kpobject);
			  }
		      }
		  }
	    } break;
	  case MT_RESIZE_UP:
	    {
	      if (resizeObjNum < 0) break;
	      if (firstX != mx || firstY != my)
		{
		  kpobject = objectList()->at(resizeObjNum);
		  ResizeCmd *resizeCmd = new ResizeCmd(i18n("Resize object up"),KPoint(0,my - firstY),KSize(0,firstY - my),
						       kpobject,view->kPresenterDoc());
		  kpobject->setMove(false);
		  resizeCmd->unexecute(false);
		  resizeCmd->execute();
		  view->kPresenterDoc()->commands()->addCommand(resizeCmd);
		}
	      kpobject = objectList()->at(resizeObjNum);
	      kpobject->setMove(false);
	      _repaint(oldBoundingRect);
	      _repaint(kpobject);
	    } break;
	  case MT_RESIZE_DN:
	    {
	      if (resizeObjNum < 0) break;
	      if (firstX != mx || firstY != my)
		{
		  kpobject = objectList()->at(resizeObjNum);
		  ResizeCmd *resizeCmd = new ResizeCmd(i18n("Resize object down"),KPoint(0,0),KSize(0,my - firstY),
						       kpobject,view->kPresenterDoc());
		  kpobject->setMove(false);
		  resizeCmd->unexecute(false);
		  resizeCmd->execute();
		  view->kPresenterDoc()->commands()->addCommand(resizeCmd);
		}
	      kpobject = objectList()->at(resizeObjNum);
	      kpobject->setMove(false);
	      _repaint(oldBoundingRect);
	      _repaint(kpobject);
	    } break;
	  case MT_RESIZE_LF:
	    {
	      if (resizeObjNum < 0) break;
	      if (firstX != mx || firstY != my)
		{
		  kpobject = objectList()->at(resizeObjNum);
		  ResizeCmd *resizeCmd = new ResizeCmd(i18n("Resize object left"),KPoint(mx - firstX,0),KSize(firstX - mx,0),
						       kpobject,view->kPresenterDoc());
		  kpobject->setMove(false);
		  resizeCmd->unexecute(false);
		  resizeCmd->execute();
		  view->kPresenterDoc()->commands()->addCommand(resizeCmd);
		}
	      kpobject = objectList()->at(resizeObjNum);
	      kpobject->setMove(false);
	      _repaint(oldBoundingRect);
	      _repaint(kpobject);
	    } break;
	  case MT_RESIZE_RT:
	    {
	      if (resizeObjNum < 0) break;
	      if (firstX != mx || firstY != my)
		{
		  kpobject = objectList()->at(resizeObjNum);
		  ResizeCmd *resizeCmd = new ResizeCmd(i18n("Resize object right"),KPoint(0,0),KSize(mx - firstX,0),
						       kpobject,view->kPresenterDoc());
		  kpobject->setMove(false);
		  resizeCmd->unexecute(false);
		  resizeCmd->execute();
		  view->kPresenterDoc()->commands()->addCommand(resizeCmd);
		}
	      kpobject = objectList()->at(resizeObjNum);
	      kpobject->setMove(false);
	      _repaint(oldBoundingRect);
	      _repaint(kpobject);
	    } break;
	  case MT_RESIZE_LU:
	    {
	      if (resizeObjNum < 0) break;
	      if (firstX != mx || firstY != my)
		{
		  kpobject = objectList()->at(resizeObjNum);
		  ResizeCmd *resizeCmd = new ResizeCmd(i18n("Resize object left up"),KPoint(mx - firstX,my - firstY),
						       KSize(firstX - mx,firstY - my),kpobject,view->kPresenterDoc());
		  kpobject->setMove(false);
		  resizeCmd->unexecute(false);
		  resizeCmd->execute();
		  view->kPresenterDoc()->commands()->addCommand(resizeCmd);
		}
	      kpobject = objectList()->at(resizeObjNum);
	      kpobject->setMove(false);
	      _repaint(oldBoundingRect);
	      _repaint(kpobject);
	    } break;
	  case MT_RESIZE_LD:
	    {
	      if (resizeObjNum < 0) break;
	      if (firstX != mx || firstY != my)
		{
		  kpobject = objectList()->at(resizeObjNum);
		  ResizeCmd *resizeCmd = new ResizeCmd(i18n("Resize object left down"),KPoint(mx - firstX,0),
						       KSize(firstX - mx,my - firstY),kpobject,view->kPresenterDoc());
		  kpobject->setMove(false);
		  resizeCmd->unexecute(false);
		  resizeCmd->execute();
		  view->kPresenterDoc()->commands()->addCommand(resizeCmd);
		}
	      kpobject = objectList()->at(resizeObjNum);
	      kpobject->setMove(false);
	      _repaint(oldBoundingRect);
	      _repaint(kpobject);
	    } break;
	  case MT_RESIZE_RU:
	    {
	      if (resizeObjNum < 0) break;
	      if (firstX != mx || firstY != my)
		{
		  kpobject = objectList()->at(resizeObjNum);
		  ResizeCmd *resizeCmd = new ResizeCmd(i18n("Resize object right up"),KPoint(0,my - firstY),
						       KSize(mx - firstX,firstY - my),kpobject,view->kPresenterDoc());
		  kpobject->setMove(false);
		  resizeCmd->unexecute(false);
		  resizeCmd->execute();
		  view->kPresenterDoc()->commands()->addCommand(resizeCmd);
		}
	      kpobject = objectList()->at(resizeObjNum);
	      kpobject->setMove(false);
	      _repaint(oldBoundingRect);
	      _repaint(kpobject);
	    } break;
	  case MT_RESIZE_RD:
	    {
	      if (resizeObjNum < 0) break;
	      if (firstX != mx || firstY != my)
		{
		  kpobject = objectList()->at(resizeObjNum);
		  ResizeCmd *resizeCmd = new ResizeCmd(i18n("Resize object right down"),KPoint(0,0),KSize(mx - firstX,my - firstY),
						       kpobject,view->kPresenterDoc());
		  kpobject->setMove(false);
		  resizeCmd->unexecute(false);
		  resizeCmd->execute();
		  view->kPresenterDoc()->commands()->addCommand(resizeCmd);
		}
	      kpobject = objectList()->at(resizeObjNum);
	      kpobject->setMove(false);
	      _repaint(oldBoundingRect);
	      _repaint(kpobject);
	    } break;
	  }
      } break;
    case INS_TEXT:
      {
	if (insRect.width() > 0 && insRect.height() > 0)
	  {
	    insertText(insRect);
	    setToolEditMode(TEM_MOUSE);
	  }
      } break;
    case INS_LINE:
      {
	if (insRect.width() != 0 && insRect.height() != 0)
	  {
	    if (insRect.top() == insRect.bottom())
	      {
		bool reverse = insRect.left() > insRect.right();
		insRect = insRect.normalize();
		insRect.setRect(insRect.left(),insRect.top() - rastY() / 2,
				insRect.width(),rastY());
		insertLineH(insRect,reverse);
	      }
	    else if (insRect.left() == insRect.right())
	      {
		bool reverse = insRect.top() > insRect.bottom();
		insRect = insRect.normalize();
		insRect.setRect(insRect.left() - rastX() / 2,insRect.top(),
				rastX(),insRect.height());
		insertLineV(insRect,reverse);
	      }
	    else if (insRect.left() < insRect.right() && insRect.top() < insRect.bottom() ||
		     insRect.left() > insRect.right() && insRect.top() > insRect.bottom())
	      {
		bool reverse = insRect.left() > insRect.right() && insRect.top() > insRect.bottom();
		insertLineD1(insRect.normalize(),reverse);
	      }
	    else
	      {
		bool reverse = insRect.right() < insRect.left() && insRect.top() < insRect.bottom();
		insertLineD2(insRect.normalize(),reverse);
	      }
	  }
      } break;
    case INS_RECT:
      if (insRect.width() > 0 && insRect.height() > 0) insertRect(insRect);
      break;
    case INS_ELLIPSE:
      if (insRect.width() > 0 && insRect.height() > 0) insertEllipse(insRect);
      break;
    case INS_PIE:
      if (insRect.width() > 0 && insRect.height() > 0) insertPie(insRect);
      break;
    case INS_OBJECT:
      {
	if (insRect.width() > 0 && insRect.height() > 0) insertObject(insRect);
	setToolEditMode(TEM_MOUSE);
      } break;
    case INS_DIAGRAMM:
      {
	if (insRect.width() > 0 && insRect.height() > 0) insertDiagramm(insRect);
	setToolEditMode(TEM_MOUSE);
      } break;
    case INS_TABLE:
      {
	if (insRect.width() > 0 && insRect.height() > 0) insertTable(insRect);
	setToolEditMode(TEM_MOUSE);
      } break;
    case INS_FORMULA:
      {
	if (insRect.width() > 0 && insRect.height() > 0) insertFormula(insRect);
	setToolEditMode(TEM_MOUSE);
      } break;
    case INS_AUTOFORM:
      {
	bool reverse = insRect.left() > insRect.right() || insRect.top() > insRect.bottom();
	if (insRect.width() > 0 && insRect.height() > 0) insertAutoform(insRect,reverse);
	setToolEditMode(TEM_MOUSE);
      } break;
    }

  if (toolEditMode != TEM_MOUSE && editMode) repaint(false);

  mousePressed = false;
  modType = MT_NONE;
  resizeObjNum = -1;
  mouseMoveEvent(e);
}

/*==================== handle mouse moved ========================*/
void Page::mouseMoveEvent(QMouseEvent *e)
{
  if (!hasFocus())
    view->sendFocusEvent();

  if (editMode)
    {
      view->setRulerMousePos(e->x(),e->y());

      KPObject *kpobject;

      if ((!mousePressed || (!drawRubber && modType == MT_NONE)) && toolEditMode == TEM_MOUSE)
	{
	  setCursor(arrowCursor);
	  if (objectList()->count() - 1 >= 0)
	    {
	      for (int i = static_cast<int>(objectList()->count()) - 1;i >= 0;i--)
		{
		  kpobject = objectList()->at(i);
		  if (kpobject->contains(KPoint(e->x(),e->y()),diffx(),diffy()))
		    {
		      if (kpobject->isSelected())
			setCursor(kpobject->getCursor(KPoint(e->x(),e->y()),diffx(),diffy(),modType));
		      return;
		    }
		}
	    }
	}
      else if (mousePressed)
	{
	  int mx = e->x();
	  int my = e->y();
	  mx = (mx / rastX()) * rastX();
	  my = (my / rastY()) * rastY();

	  switch (toolEditMode)
	    {
	    case TEM_MOUSE:
	      {
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
		    QPainter p;
		    p.begin(this);
		
		    if (objectList()->count() - 1 >= 0)
		      {
			for (int i = static_cast<int>(objectList()->count()) - 1;i >= 0;i--)
			  {
			    kpobject = objectList()->at(i);
			    if (kpobject->isSelected())
			      {
				kpobject->setMove(true);
				kpobject->draw(&p,diffx(),diffy());
				kpobject->moveBy(KPoint(mx - oldMx,my - oldMy));
				kpobject->draw(&p,diffx(),diffy());
			      }
			  }
		      }
		
		    p.end();
		  }
		else if (modType != MT_NONE && resizeObjNum != -1)
		  {
		    QPainter p;
		    p.begin(this);
		
		    KRect oldRect;
		    kpobject = objectList()->at(resizeObjNum);
		    oldRect = kpobject->getBoundingRect(0,0);
		    kpobject->setMove(true);
		    kpobject->draw(&p,diffx(),diffy());
		
		    switch (modType)
		      {
		      case MT_RESIZE_LU:
			{
			  kpobject->moveBy(KPoint(mx - oldMx,my - oldMy));
			  kpobject->resizeBy(KSize(oldMx - mx,oldMy - my));
			} break;
		      case MT_RESIZE_LF:
			{
			  kpobject->moveBy(KPoint(mx - oldMx,0));
			  kpobject->resizeBy(KSize(oldMx - mx,0));
			} break;
		      case MT_RESIZE_LD:
			{
			  kpobject->moveBy(KPoint(mx - oldMx,0));
			  kpobject->resizeBy(KSize(oldMx - mx,my - oldMy));
			} break;
		      case MT_RESIZE_RU:
			{
			  kpobject->moveBy(KPoint(0,my - oldMy));
			  kpobject->resizeBy(KSize(mx - oldMx,oldMy - my));
			} break;
		      case MT_RESIZE_RT:
			kpobject->resizeBy(KSize(mx - oldMx,0));
			break;
		      case MT_RESIZE_RD:
			kpobject->resizeBy(KSize(mx - oldMx,my - oldMy));
			break;
		      case MT_RESIZE_UP:
			{
			  kpobject->moveBy(KPoint(0,my - oldMy));
			  kpobject->resizeBy(KSize(0,oldMy - my));
			} break;
		      case MT_RESIZE_DN:
			kpobject->resizeBy(KSize(0,my - oldMy));
			break;
		      default: break;
		      }
		    kpobject->draw(&p,diffx(),diffy());
		    p.end();
		  }

		oldMx = e->x();
		oldMy = e->y();
	      } break;
	    case INS_TEXT: case INS_OBJECT: case INS_TABLE: case INS_DIAGRAMM: case INS_FORMULA: case INS_AUTOFORM:
	      {
		QPainter p(this);
		p.setPen(QPen(black,1,SolidLine));
		p.setBrush(NoBrush);
		p.setRasterOp(NotROP);
		if (insRect.width() != 0 && insRect.height() != 0)
		  p.drawRect(insRect);
		insRect.setRight(((e->x() + diffx()) / rastX()) * rastX() - diffx());
		insRect.setBottom(((e->y() + diffy()) / rastY()) * rastY() - diffy());
		p.drawRect(insRect);
		p.end();
	      } break;
	    case INS_ELLIPSE:
	      {
		QPainter p(this);
		p.setPen(QPen(black,1,SolidLine));
		p.setBrush(NoBrush);
		p.setRasterOp(NotROP);
		if (insRect.width() != 0 && insRect.height() != 0)
		  p.drawEllipse(insRect);
		insRect.setRight(((e->x() + diffx()) / rastX()) * rastX() - diffx());
		insRect.setBottom(((e->y() + diffy()) / rastY()) * rastY() - diffy());
		p.drawEllipse(insRect);
		p.end();
	      } break;
	    case INS_RECT:
	      {
		QPainter p(this);
		p.setPen(QPen(black,1,SolidLine));
		p.setBrush(NoBrush);
		p.setRasterOp(NotROP);
		if (insRect.width() != 0 && insRect.height() != 0)
		  p.drawRoundRect(insRect,view->getRndX(),view->getRndY());
		insRect.setRight(((e->x() + diffx()) / rastX()) * rastX() - diffx());
		insRect.setBottom(((e->y() + diffy()) / rastY()) * rastY() - diffy());
		p.drawRoundRect(insRect,view->getRndX(),view->getRndY());
		p.end();
	      } break;
	    case INS_LINE:
	      {
		QPainter p(this);
		p.setPen(QPen(black,1,SolidLine));
		p.setBrush(NoBrush);
		p.setRasterOp(NotROP);
		if (insRect.width() != 0 && insRect.height() != 0)
		  p.drawLine(insRect.topLeft(),insRect.bottomRight());
		insRect.setRight(((e->x() + diffx()) / rastX()) * rastX() - diffx());
		insRect.setBottom(((e->y() + diffy()) / rastY()) * rastY() - diffy());
		p.drawLine(insRect.topLeft(),insRect.bottomRight());
		p.end();
	      } break;
	    case INS_PIE:
	      {
		QPainter p(this);
		p.setPen(QPen(black,1,SolidLine));
		p.setBrush(NoBrush);
		p.setRasterOp(NotROP);
		if (insRect.width() != 0 && insRect.height() != 0)
		  {
		    switch (view->getPieType())
		      {
		      case PT_PIE:
			p.drawPie(insRect.x(),insRect.y(),insRect.width() - 2,insRect.height() - 2,view->getPieAngle(),view->getPieLength());
			break;
		      case PT_ARC:
			p.drawArc(insRect.x(),insRect.y(),insRect.width() - 2,insRect.height() - 2,view->getPieAngle(),view->getPieLength());
			break;
		      case PT_CHORD:
			p.drawChord(insRect.x(),insRect.y(),insRect.width() - 2,insRect.height() - 2,view->getPieAngle(),view->getPieLength());
			break;
		      default: break;
		      }
		  }
		insRect.setRight(((e->x() + diffx()) / rastX()) * rastX() - diffx());
		insRect.setBottom(((e->y() + diffy()) / rastY()) * rastY() - diffy());
		switch (view->getPieType())
		  {
		  case PT_PIE:
		    p.drawPie(insRect.x(),insRect.y(),insRect.width() - 2,insRect.height() - 2,view->getPieAngle(),view->getPieLength());
		    break;
		  case PT_ARC:
		    p.drawArc(insRect.x(),insRect.y(),insRect.width() - 2,insRect.height() - 2,view->getPieAngle(),view->getPieLength());
		    break;
		  case PT_CHORD:
		    p.drawChord(insRect.x(),insRect.y(),insRect.width() - 2,insRect.height() - 2,view->getPieAngle(),view->getPieLength());
		    break;
		  default: break;
		  }
		p.end();
	      } break;
	    }
	}
    }
  else if (!editMode && drawMode)
    {
      QPainter p;
      p.begin(this);
      p.setPen(view->kPresenterDoc()->presPen());
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
  if (toolEditMode != TEM_MOUSE || !editMode) return;

  deSelectAllObj();
  KPObject *kpobject = 0;

  if (objectList()->count() - 1 >= 0)
    {
      for (int i = static_cast<int>(objectList()->count()) - 1;i >= 0;i--)
	{
	  kpobject = objectList()->at(i);
	  if (kpobject->contains(KPoint(e->x(),e->y()),diffx(),diffy()))
	    {
	      if (kpobject->getType() == OT_TEXT)
		{
		  KPTextObject *kptextobject = dynamic_cast<KPTextObject*>(kpobject);
		
		  kpobject->activate(this,diffx(),diffy());
		  kptextobject->getKTextObject()->setBackgroundColor(txtBackCol());
		  //setFocusProxy(kptextobject->getKTextObject());
		  //setFocusPolicy(QWidget::StrongFocus);
		  //kptextobject->getKTextObject()->setFocusPolicy(QWidget::StrongFocus);
		  //kptextobject->getKTextObject()->setFocus();
		  kptextobject->getKTextObject()->setShowCursor(true);
		  connect(kptextobject->getKTextObject(),SIGNAL(fontChanged(QFont*)),this,SLOT(toFontChanged(QFont*)));
		  connect(kptextobject->getKTextObject(),SIGNAL(giveMeFocus()),view,SLOT(sendFocusEvent()));
		  connect(kptextobject->getKTextObject(),SIGNAL(colorChanged(QColor*)),this,SLOT(toColorChanged(QColor*)));
		  connect(kptextobject->getKTextObject(),SIGNAL(horzAlignChanged(TxtParagraph::HorzAlign)),
			  this,SLOT(toAlignChanged(TxtParagraph::HorzAlign)));
		  editNum = i;
		  break;
		}
	      else if (kpobject->getType() == OT_PART)
		{
		  view->kPresenterDoc()->hideAllFrames();
		  view->setFramesToParts();
		  kpobject->activate(this,diffx(),diffy());
		  //setFocusProxy(dynamic_cast<KPPartObject*>(kpobject)->getView());
		  //setFocusPolicy(QWidget::StrongFocus);
		  //dynamic_cast<KPPartObject*>(kpobject)->getView()->setFocusPolicy(QWidget::StrongFocus);
		  //dynamic_cast<KPPartObject*>(kpobject)->getView()->setFocus();
		  editNum = i;
		  break;
		}
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
	case Key_Space: case Key_Right: case Key_Down: case Key_Next:
	  view->screenNext(); break;
	case Key_Backspace: case Key_Left: case Key_Up: case Key_Prior:
	  view->screenPrev(); break;
	case Key_Escape: case Key_Q: case Key_X:
	  view->screenStop(); break;
	case Key_G:
	  slotGotoPage(); break;
	default: break;
	}
    }
  else if (editNum != -1)
    QApplication::sendEvent(dynamic_cast<KPTextObject*>(objectList()->at(editNum))->getKTextObject(),e);
  else
    {
      switch (e->key())
	{
	case Key_Next:
	  view->screenNext(); break;
	case Key_Prior:
	  view->screenPrev(); break;
	case Key_Down:
	  view->getVScrollBar()->addLine(); break;
	case Key_Up:
	  view->getVScrollBar()->subtractLine(); break;
	case Key_Right:
	  view->getHScrollBar()->addLine(); break;
	case Key_Left:
	  view->getHScrollBar()->subtractLine(); break;
	case Key_Tab:
	  selectNext(); break;
	case Key_Backtab:
	  selectPrev(); break;
	case Key_Home:
	  view->getVScrollBar()->setValue(0); break;
	case Key_End:
	  view->getVScrollBar()->setValue( view->getVScrollBar()->maxValue()); break;
	case Key_Delete:
	  view->editDelete(); break;
	default: break;
	}
    }
}

/*========================= resize Event =========================*/
void Page::resizeEvent(QResizeEvent *e)
{
  if (editMode) QWidget::resizeEvent(e);
  else QWidget::resizeEvent(new QResizeEvent(KSize(QApplication::desktop()->width(),QApplication::desktop()->height()),
					     e->oldSize()));
}

/*========================== get object ==========================*/
int Page::getObjectAt(int x,int y)
{
  KPObject *kpobject = 0;

  if (objectList()->count() - 1 >= 0)
    {
      for (int i = static_cast<int>(objectList()->count()) - 1;i >= 0 ;i--)
	{
	  kpobject = objectList()->at(i);
	  if (kpobject->contains(KPoint(x,y),diffx(),diffy()))
	    return i;
	}
    }

  return -1;
}

/*================================================================*/
int Page::diffx(int i = -1)
{
  //if (i == -1)
    return view->getDiffX();

//   if (editMode)
//     return view->getDiffX();

//   return static_cast<int>(view->getDiffX() + view->kPresenterDoc()->getLeftBorder() * _presFakt);
}

/*================================================================*/
int Page::diffy(int i = -1)
{
  //if (i == -1)
    return view->getDiffY();

//   if (editMode)
//     return view->getDiffY();

//   return static_cast<int>(view->getDiffY() + ((i + 1) * view->kPresenterDoc()->getTopBorder() +
// 					      i * view->kPresenterDoc()->getBottomBorder()) * _presFakt);
}
/*======================= select object ==========================*/
void Page::selectObj(int num)
{
  if (num < static_cast<int>(objectList()->count()))
    {
      selectObj(objectList()->at(num));
      if (objectList()->at(num)->getType() == OT_TEXT)
	{
	  KPTextObject *kptextobject = dynamic_cast<KPTextObject*>(objectList()->at(num));
	  QFont *f = new QFont(kptextobject->getKTextObject()->font());
	  QColor *c = new QColor(kptextobject->getKTextObject()->color());
	  toFontChanged(f);
	  toColorChanged(c);
	  toAlignChanged(kptextobject->getKTextObject()->horzAlign());
	  delete c;
	  delete f;
	}
    }
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
  if (kpobject->getType() == OT_TEXT)
    {
      KPTextObject *kptextobject = dynamic_cast<KPTextObject*>(kpobject);
      QFont *f = new QFont(kptextobject->getKTextObject()->font());
      QColor *c = new QColor(kptextobject->getKTextObject()->color());
      toFontChanged(f);
      toColorChanged(c);
      toAlignChanged(kptextobject->getKTextObject()->horzAlign());
      delete c;
      delete f;
    }
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
  // create right button object align menu
  alignMenu1 = new QPopupMenu();
  CHECK_PTR(alignMenu1);
  alignMenu1->insertItem(ICON("aoleft.xpm"),this,SLOT(alignObjLeft()));
  alignMenu1->insertSeparator();
  alignMenu1->insertItem(ICON("aocenterh.xpm"),this,SLOT(alignObjCenterH()));
  alignMenu1->insertSeparator();
  alignMenu1->insertItem(ICON("aoright.xpm"),this,SLOT(alignObjRight()));
  alignMenu1->insertSeparator();
  alignMenu1->insertItem(ICON("aotop.xpm"),this,SLOT(alignObjTop()));
  alignMenu1->insertSeparator();
  alignMenu1->insertItem(ICON("aocenterv.xpm"),this,SLOT(alignObjCenterV()));
  alignMenu1->insertSeparator();
  alignMenu1->insertItem(ICON("aobottom.xpm"),this,SLOT(alignObjBottom()));
  alignMenu1->setMouseTracking(true);
  alignMenu1->setCheckable(false);

  alignMenu2 = new QPopupMenu();
  CHECK_PTR(alignMenu2);
  alignMenu2->insertItem(ICON("aoleft.xpm"),this,SLOT(alignObjLeft()));
  alignMenu2->insertSeparator();
  alignMenu2->insertItem(ICON("aocenterh.xpm"),this,SLOT(alignObjCenterH()));
  alignMenu2->insertSeparator();
  alignMenu2->insertItem(ICON("aoright.xpm"),this,SLOT(alignObjRight()));
  alignMenu2->insertSeparator();
  alignMenu2->insertItem(ICON("aotop.xpm"),this,SLOT(alignObjTop()));
  alignMenu2->insertSeparator();
  alignMenu2->insertItem(ICON("aocenterv.xpm"),this,SLOT(alignObjCenterV()));
  alignMenu2->insertSeparator();
  alignMenu2->insertItem(ICON("aobottom.xpm"),this,SLOT(alignObjBottom()));
  alignMenu2->setMouseTracking(true);
  alignMenu2->setCheckable(false);

  alignMenu3 = new QPopupMenu();
  CHECK_PTR(alignMenu3);
  alignMenu3->insertItem(ICON("aoleft.xpm"),this,SLOT(alignObjLeft()));
  alignMenu3->insertSeparator();
  alignMenu3->insertItem(ICON("aocenterh.xpm"),this,SLOT(alignObjCenterH()));
  alignMenu3->insertSeparator();
  alignMenu3->insertItem(ICON("aoright.xpm"),this,SLOT(alignObjRight()));
  alignMenu3->insertSeparator();
  alignMenu3->insertItem(ICON("aotop.xpm"),this,SLOT(alignObjTop()));
  alignMenu3->insertSeparator();
  alignMenu3->insertItem(ICON("aocenterv.xpm"),this,SLOT(alignObjCenterV()));
  alignMenu3->insertSeparator();
  alignMenu3->insertItem(ICON("aobottom.xpm"),this,SLOT(alignObjBottom()));
  alignMenu3->setMouseTracking(true);
  alignMenu3->setCheckable(false);

  alignMenu4 = new QPopupMenu();
  CHECK_PTR(alignMenu4);
  alignMenu4->insertItem(ICON("aoleft.xpm"),this,SLOT(alignObjLeft()));
  alignMenu4->insertSeparator();
  alignMenu4->insertItem(ICON("aocenterh.xpm"),this,SLOT(alignObjCenterH()));
  alignMenu4->insertSeparator();
  alignMenu4->insertItem(ICON("aoright.xpm"),this,SLOT(alignObjRight()));
  alignMenu4->insertSeparator();
  alignMenu4->insertItem(ICON("aotop.xpm"),this,SLOT(alignObjTop()));
  alignMenu4->insertSeparator();
  alignMenu4->insertItem(ICON("aocenterv.xpm"),this,SLOT(alignObjCenterV()));
  alignMenu4->insertSeparator();
  alignMenu4->insertItem(ICON("aobottom.xpm"),this,SLOT(alignObjBottom()));
  alignMenu4->setMouseTracking(true);
  alignMenu4->setCheckable(false);

  alignMenu5 = new QPopupMenu();
  CHECK_PTR(alignMenu5);
  alignMenu5->insertItem(ICON("aoleft.xpm"),this,SLOT(alignObjLeft()));
  alignMenu5->insertSeparator();
  alignMenu5->insertItem(ICON("aocenterh.xpm"),this,SLOT(alignObjCenterH()));
  alignMenu5->insertSeparator();
  alignMenu5->insertItem(ICON("aoright.xpm"),this,SLOT(alignObjRight()));
  alignMenu5->insertSeparator();
  alignMenu5->insertItem(ICON("aotop.xpm"),this,SLOT(alignObjTop()));
  alignMenu5->insertSeparator();
  alignMenu5->insertItem(ICON("aocenterv.xpm"),this,SLOT(alignObjCenterV()));
  alignMenu5->insertSeparator();
  alignMenu5->insertItem(ICON("aobottom.xpm"),this,SLOT(alignObjBottom()));
  alignMenu5->setMouseTracking(true);
  alignMenu5->setCheckable(false);

  alignMenu6 = new QPopupMenu();
  CHECK_PTR(alignMenu6);
  alignMenu6->insertItem(ICON("aoleft.xpm"),this,SLOT(alignObjLeft()));
  alignMenu6->insertSeparator();
  alignMenu6->insertItem(ICON("aocenterh.xpm"),this,SLOT(alignObjCenterH()));
  alignMenu6->insertSeparator();
  alignMenu6->insertItem(ICON("aoright.xpm"),this,SLOT(alignObjRight()));
  alignMenu6->insertSeparator();
  alignMenu6->insertItem(ICON("aotop.xpm"),this,SLOT(alignObjTop()));
  alignMenu6->insertSeparator();
  alignMenu6->insertItem(ICON("aocenterv.xpm"),this,SLOT(alignObjCenterV()));
  alignMenu6->insertSeparator();
  alignMenu6->insertItem(ICON("aobottom.xpm"),this,SLOT(alignObjBottom()));
  alignMenu6->setMouseTracking(true);
  alignMenu6->setCheckable(false);

  alignMenu7 = new QPopupMenu();
  CHECK_PTR(alignMenu7);
  alignMenu7->insertItem(ICON("aoleft.xpm"),this,SLOT(alignObjLeft()));
  alignMenu7->insertSeparator();
  alignMenu7->insertItem(ICON("aocenterh.xpm"),this,SLOT(alignObjCenterH()));
  alignMenu7->insertSeparator();
  alignMenu7->insertItem(ICON("aoright.xpm"),this,SLOT(alignObjRight()));
  alignMenu7->insertSeparator();
  alignMenu7->insertItem(ICON("aotop.xpm"),this,SLOT(alignObjTop()));
  alignMenu7->insertSeparator();
  alignMenu7->insertItem(ICON("aocenterv.xpm"),this,SLOT(alignObjCenterV()));
  alignMenu7->insertSeparator();
  alignMenu7->insertItem(ICON("aobottom.xpm"),this,SLOT(alignObjBottom()));
  alignMenu7->setMouseTracking(true);
  alignMenu7->setCheckable(false);

  // create right button graph menu
  graphMenu = new QPopupMenu();
  CHECK_PTR(graphMenu);
  graphMenu->insertItem(ICON("editcut.xpm"),i18n("&Cut"),this,SLOT(clipCut()));
  graphMenu->insertItem(ICON("editcopy.xpm"),i18n("C&opy"),this,SLOT(clipCopy()));
  graphMenu->insertItem(ICON("delete.xpm"),i18n("&Delete"),this,SLOT(deleteObjs()));
  graphMenu->insertSeparator();
  graphMenu->insertItem(ICON("rotate.xpm"),i18n("&Rotate..."),this,SLOT(rotateObjs()));
  graphMenu->insertItem(ICON("shadow.xpm"),i18n("&Shadow..."),this,SLOT(shadowObjs()));
  graphMenu->insertSeparator();
  graphMenu->insertItem(ICON("style.xpm"),i18n("&Properties..."),this,SLOT(objProperties()));
  graphMenu->insertSeparator();
  graphMenu->insertItem(ICON("effect.xpm"),i18n("&Assign effect..."),this,SLOT(assignEffect()));
  graphMenu->insertSeparator();
  graphMenu->insertItem(ICON("alignobjs.xpm"),i18n("&Align objects"),alignMenu1);
  graphMenu->setMouseTracking(true);

  // create right button part menu
  partMenu = new QPopupMenu();
  CHECK_PTR(partMenu);
  partMenu->insertItem(ICON("editcut.xpm"),i18n("&Cut"),this,SLOT(clipCut()));
  partMenu->insertItem(ICON("editcopy.xpm"),i18n("C&opy"),this,SLOT(clipCopy()));
  partMenu->insertItem(ICON("delete.xpm"),i18n("&Delete"),this,SLOT(deleteObjs()));
  partMenu->insertSeparator();
  partMenu->insertItem(ICON("rotate.xpm"),i18n("&Rotate..."),this,SLOT(rotateObjs()));
  partMenu->insertSeparator();
  partMenu->insertItem(ICON("style.xpm"),i18n("&Properties..."),this,SLOT(objProperties()));
  partMenu->insertSeparator();
  partMenu->insertItem(ICON("effect.xpm"),i18n("&Assign effect..."),this,SLOT(assignEffect()));
  partMenu->insertSeparator();
  partMenu->insertItem(ICON("alignobjs.xpm"),i18n("&Align objects"),alignMenu7);
  partMenu->setMouseTracking(true);

  // create right button rect menu
  rectMenu = new QPopupMenu();
  CHECK_PTR(rectMenu);
  rectMenu->insertItem(ICON("editcut.xpm"),i18n("&Cut"),this,SLOT(clipCut()));
  rectMenu->insertItem(ICON("editcopy.xpm"),i18n("C&opy"),this,SLOT(clipCopy()));
  rectMenu->insertItem(ICON("delete.xpm"),i18n("&Delete"),this,SLOT(deleteObjs()));
  rectMenu->insertSeparator();
  rectMenu->insertItem(ICON("rotate.xpm"),i18n("&Rotate..."),this,SLOT(rotateObjs()));
  rectMenu->insertItem(ICON("shadow.xpm"),i18n("&Shadow..."),this,SLOT(shadowObjs()));
  rectMenu->insertSeparator();
  rectMenu->insertItem(ICON("style.xpm"),i18n("&Properties..."),this,SLOT(objProperties()));
  rectMenu->insertItem(ICON("rectangle2.xpm"),i18n("&Configure Rectangle..."),this,SLOT(objConfigRect()));
  rectMenu->insertSeparator();
  rectMenu->insertItem(ICON("effect.xpm"),i18n("&Assign effect..."),this,SLOT(assignEffect()));
  rectMenu->insertSeparator();
  rectMenu->insertItem(ICON("alignobjs.xpm"),i18n("&Align objects"),alignMenu6);
  rectMenu->setMouseTracking(true);

  // create right button pie menu
  pieMenu = new QPopupMenu();
  CHECK_PTR(pieMenu);
  pieMenu->insertItem(ICON("editcut.xpm"),i18n("&Cut"),this,SLOT(clipCut()));
  pieMenu->insertItem(ICON("editcopy.xpm"),i18n("C&opy"),this,SLOT(clipCopy()));
  pieMenu->insertItem(ICON("delete.xpm"),i18n("&Delete"),this,SLOT(deleteObjs()));
  pieMenu->insertSeparator();
  pieMenu->insertItem(ICON("rotate.xpm"),i18n("&Rotate..."),this,SLOT(rotateObjs()));
  pieMenu->insertItem(ICON("shadow.xpm"),i18n("&Shadow..."),this,SLOT(shadowObjs()));
  pieMenu->insertSeparator();
  pieMenu->insertItem(ICON("style.xpm"),i18n("&Properties..."),this,SLOT(objProperties()));
  pieMenu->insertItem(ICON("edit_pie.xpm"),i18n("&Configure pie/arc/chord..."),this,SLOT(objConfigPie()));
  pieMenu->insertSeparator();
  pieMenu->insertItem(ICON("effect.xpm"),i18n("&Assign effect..."),this,SLOT(assignEffect()));
  pieMenu->insertSeparator();
  pieMenu->insertItem(ICON("alignobjs.xpm"),i18n("&Align objects"),alignMenu5);
  pieMenu->setMouseTracking(true);

  // create right button picture menu
  picMenu = new QPopupMenu();
  CHECK_PTR(picMenu);
  picMenu->insertItem(ICON("editcut.xpm"),i18n("&Cut"),this,SLOT(clipCut()));
  picMenu->insertItem(ICON("editcopy.xpm"),i18n("C&opy"),this,SLOT(clipCopy()));
  picMenu->insertItem(ICON("delete.xpm"),i18n("&Delete"),this,SLOT(deleteObjs()));
  picMenu->insertSeparator();
  picMenu->insertItem(ICON("rotate.xpm"),i18n("&Rotate..."),this,SLOT(rotateObjs()));
  picMenu->insertItem(ICON("shadow.xpm"),i18n("&Shadow..."),this,SLOT(shadowObjs()));
  picMenu->insertSeparator();
  picMenu->insertItem(ICON("picture.xpm"),i18n("&Change Picture..."),this,SLOT(chPic()));
  picMenu->insertItem(ICON("style.xpm"),i18n("&Properties..."),this,SLOT(objProperties()));
  picMenu->insertSeparator();
  picMenu->insertItem(ICON("effect.xpm"),i18n("&Assign effect..."),this,SLOT(assignEffect()));
  picMenu->insertSeparator();
  picMenu->insertItem(ICON("alignobjs.xpm"),i18n("&Align objects"),alignMenu2);
  picMenu->setMouseTracking(true);

  // create right button clipart menu
  clipMenu = new QPopupMenu();
  CHECK_PTR(clipMenu);
  clipMenu->insertItem(ICON("editcut.xpm"),i18n("&Cut"),this,SLOT(clipCut()));
  clipMenu->insertItem(ICON("editcopy.xpm"),i18n("C&opy"),this,SLOT(clipCopy()));
  clipMenu->insertItem(ICON("delete.xpm"),i18n("&Delete"),this,SLOT(deleteObjs()));
  clipMenu->insertSeparator();
  clipMenu->insertItem(ICON("rotate.xpm"),i18n("&Rotate..."),this,SLOT(rotateObjs()));
  clipMenu->insertItem(ICON("shadow.xpm"),i18n("&Shadow..."),this,SLOT(shadowObjs()));
  clipMenu->insertSeparator();
  clipMenu->insertItem(ICON("clipart.xpm"),i18n("&Change Clipart..."),this,SLOT(chClip()));
  clipMenu->insertItem(ICON("style.xpm"),i18n("&Properties..."),this,SLOT(objProperties()));
  clipMenu->insertSeparator();
  clipMenu->insertItem(ICON("effect.xpm"),i18n("&Assign effect..."),this,SLOT(assignEffect()));
  clipMenu->insertSeparator();
  clipMenu->insertItem(ICON("alignobjs.xpm"),i18n("&Align objects"),alignMenu3);
  clipMenu->setMouseTracking(true);

  // create right button text menu
  txtMenu = new QPopupMenu();
  CHECK_PTR(txtMenu);
  txtMenu->insertItem(ICON("editcut.xpm"),i18n("&Cut"),this,SLOT(clipCut()));
  txtMenu->insertItem(ICON("editcopy.xpm"),i18n("C&opy"),this,SLOT(clipCopy()));
  txtMenu->insertItem(ICON("delete.xpm"),i18n("&Delete"),this,SLOT(deleteObjs()));
  txtMenu->insertSeparator();
  txtMenu->insertItem(ICON("rotate.xpm"),i18n("&Rotate..."),this,SLOT(rotateObjs()));
  txtMenu->insertItem(ICON("shadow.xpm"),i18n("&Shadow..."),this,SLOT(shadowObjs()));
  txtMenu->insertSeparator();
  txtMenu->insertItem(ICON("style.xpm"),i18n("&Properties..."),this,SLOT(objProperties()));
  txtMenu->insertSeparator();
  txtMenu->insertItem(ICON("effect.xpm"),i18n("&Assign effect..."),this,SLOT(assignEffect()));
  txtMenu->insertSeparator();
  txtMenu->insertItem(ICON("alignobjs.xpm"),i18n("&Align objects"),alignMenu4);
  txtMenu->setMouseTracking(true);

  // create right button presentation menu
  presMenu = new QPopupMenu();
  CHECK_PTR(presMenu);
  presMenu->setCheckable(true);
  PM_SM = presMenu->insertItem(i18n("&Switching mode"),this,SLOT(switchingMode()));
  PM_DM = presMenu->insertItem(i18n("&Drawing mode"),this,SLOT(drawingMode()));
  presMenu->insertSeparator();
  presMenu->insertItem(i18n("&Goto Page..."),this,SLOT(slotGotoPage()));
  presMenu->insertSeparator();
  presMenu->insertItem(i18n("&Exit Presentation"),this,SLOT(slotExitPres()));
  presMenu->setItemChecked(PM_SM,true);
  presMenu->setItemChecked(PM_DM,false);
  presMenu->setMouseTracking(true);

  // create right button page menu
  pageMenu = new QPopupMenu();
  CHECK_PTR(pageMenu);
  pageMenu->insertItem(i18n("Pa&ge Layout..."),this,SLOT(pageLayout()));
  pageMenu->insertItem(i18n("Page &Background..."),this,SLOT(pageBackground()));
  pageMenu->insertSeparator();
  pageMenu->insertItem(i18n("&Configure pages..."),this,SLOT(configPages()));
  pageMenu->insertItem(i18n("&Open presentation structure viewer..."),this,SLOT(presStructView()));
  pageMenu->insertSeparator();
  pageMenu->insertItem(ICON("filenew.xpm"),i18n("&Insert Page..."),this,SLOT(pageInsert()));
  pageMenu->insertItem(i18n("&Delete Page..."),this,SLOT(pageDelete()));
  pageMenu->insertSeparator();
  pageMenu->insertItem(ICON("editpaste.xpm"),i18n("&Paste"),this,SLOT(pagePaste()));
  pageMenu->setMouseTracking(true);
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
      //dynamic_cast<KPTextObject*>(objectList()->at(editNum))->getKTextObject()->setFocus();
      dynamic_cast<KPTextObject*>(objectList()->at(editNum))->getKTextObject()->setFont(*font);
    }
  else
    {
      KPObject *kpobject = 0;

      for (unsigned int i = 0;i < objectList()->count();i++)
	{
	  kpobject = objectList()->at(i);
	  if (kpobject->isSelected() && kpobject->getType() == OT_TEXT)
	    dynamic_cast<KPTextObject*>(kpobject)->getKTextObject()->setFontToAll(*font);
	}
      repaint(false);
    }
}

/*======================= set text color =========================*/
void Page::setTextColor(QColor *color)
{
  if (editNum != -1 && objectList()->at(editNum)->getType() == OT_TEXT)
    {
      //dynamic_cast<KPTextObject*>(objectList()->at(editNum))->getKTextObject()->setFocus();
      dynamic_cast<KPTextObject*>(objectList()->at(editNum))->getKTextObject()->setColor(*color);
    }
  else
    {
      KPObject *kpobject = 0;

      for (unsigned int i = 0;i < objectList()->count();i++)
	{
	  kpobject = objectList()->at(i);
	  if (kpobject->isSelected() && kpobject->getType() == OT_TEXT)
	    dynamic_cast<KPTextObject*>(kpobject)->getKTextObject()->setColorToAll(*color);
	}
      repaint(false);
    }
}

/*===================== set text alignment =======================*/
void Page::setTextAlign(TxtParagraph::HorzAlign align)
{
  if (editNum != -1 && objectList()->at(editNum)->getType() == OT_TEXT)
    dynamic_cast<KPTextObject*>(objectList()->at(editNum))->getKTextObject()->setHorzAlign(align);
  else
    {
      KPObject *kpobject = 0;

      for (unsigned int i = 0;i < objectList()->count();i++)
	{
	  kpobject = objectList()->at(i);
	  if (kpobject->isSelected() && kpobject->getType() == OT_TEXT)
	    dynamic_cast<KPTextObject*>(kpobject)->getKTextObject()->setHorzAlignToAll(align);
	}
      repaint(false);
    }
}

/*================================================================*/
KTextObject *Page::haveASelectedTextObj()
{
  KPObject *kpobject = 0;

  for (unsigned int i = 0;i < objectList()->count();i++)
    {
      kpobject = objectList()->at(i);
      if (kpobject->isSelected() && kpobject->getType() == OT_TEXT)
	return dynamic_cast<KPTextObject*>(kpobject)->getKTextObject();
    }

  return 0L;
}

/*====================== start screenpresentation ================*/
void Page::startScreenPresentation(bool zoom)
{
  presMenu->setItemChecked(PM_SM,true);
  presMenu->setItemChecked(PM_DM,false);

  setCursor(waitCursor);
  KPObject *kpobject = 0;

  tmpObjs.clear();

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
	  disconnect(kptextobject->getKTextObject(),SIGNAL(giveMeFocus()),view,SLOT(sendFocusEvent()));
	  disconnect(kptextobject->getKTextObject(),SIGNAL(colorChanged(QColor*)),this,SLOT(toColorChanged(QColor*)));
	  disconnect(kptextobject->getKTextObject(),SIGNAL(horzAlignChanged(TxtParagraph::HorzAlign)),
		     this,SLOT(toAlignChanged(TxtParagraph::HorzAlign)));
	  kptextobject->getKTextObject()->setShowCursor(false);
	  //setFocusProxy(0);
	  //setFocusPolicy(QWidget::NoFocus);
	}
      else if (kpobject->getType() == OT_PART)
	{
	  kpobject->deactivate();
	  _repaint(kpobject);
	  //setFocusProxy(0);
	  //setFocusPolicy(QWidget::NoFocus);
	}
    }

  int i;

  if (zoom)
    {
      float _presFaktW = static_cast<float>(width()) / static_cast<float>(getPageSize(0,1.0,false).width()) > 0.0 ?
	static_cast<float>(width()) / static_cast<float>(getPageSize(0,1.0,false).width()) : 1.0;
      float _presFaktH = static_cast<float>(height()) / static_cast<float>(getPageSize(0,1.0,false).height()) > 0.0 ?
	static_cast<float>(height()) / static_cast<float>(getPageSize(0,1.0,false).height()) : 1.0;
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

      if (getPageOfObj(i,_presFakt) == 1)
	tmpObjs.append(kpobject);
    }

  currPresPage = 1;
  editMode = false;
  drawMode = false;
  presStepList = view->kPresenterDoc()->reorderPage(1,diffx(),diffy(),_presFakt);
  currPresStep = (int)(presStepList.first());
  subPresStep = 0;
  repaint(true);
  //setFocusPolicy(QWidget::StrongFocus);
  //setFocus();
  setCursor(blankCursor);
}

/*====================== stop screenpresentation =================*/
void Page::stopScreenPresentation()
{
  setCursor(waitCursor);
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
  setToolEditMode(toolEditMode);
  tmpObjs.clear();
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
	  drawBackground(&p,KRect(0,0,kapp->desktop()->width(),kapp->desktop()->height()));
	  p.end();
	}

      doObjEffects();
    }
  else
    {
      if (currPresPage + 1 > pageNums())
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

	  presStepList = view->kPresenterDoc()->reorderPage(currPresPage,diffx(),diffy(),_presFakt);
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

      tmpObjs.clear();
      for (int j = 0;j < static_cast<int>(objectList()->count());j++)
	{
	  if (getPageOfObj(j,_presFakt) == static_cast<int>(currPresPage))
	    tmpObjs.append(objectList()->at(j));
	}

      presStepList = view->kPresenterDoc()->reorderPage(currPresPage,diffx(),diffy(),_presFakt);
      currPresStep = (int)(presStepList.first());

      QPixmap _pix2(QApplication::desktop()->width(),QApplication::desktop()->height());
      drawPageInPix(_pix2,view->getDiffY() + view->kPresenterDoc()->getPageSize(0,0,0,_presFakt,false).height());

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
	  presStepList = view->kPresenterDoc()->reorderPage(currPresPage,diffx(),diffy(),_presFakt);
	  currPresStep = (int)(presStepList.first());
	  repaint(false);
	  return false;
	}
      currPresPage--;

      tmpObjs.clear();
      for (int j = 0;j < static_cast<int>(objectList()->count());j++)
	{
	  if (getPageOfObj(j,_presFakt) == static_cast<int>(currPresPage))
	    tmpObjs.append(objectList()->at(j));
	}

      presStepList = view->kPresenterDoc()->reorderPage(currPresPage,diffx(),diffy(),_presFakt);
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

/*================================================================*/
void Page::drawPageInPix2(QPixmap &_pix,int __diffy,int pgnum)
{
  currPresPage = pgnum + 1;
  int _yOffset = view->getDiffY();
  view->setDiffY(__diffy);

  QPainter p;
  p.begin(&_pix);

  KPObject *kpobject = 0L;
  int i;
  
  for (i = 0;i < static_cast<int>(objectList()->count());i++)
    {
      kpobject = objectList()->at(i);
      kpobject->drawSelection(false);
    }

  bool _editMode = editMode;
  
  editMode = false;
  drawBackground(&p,_pix.rect());
  editMode = _editMode;

  drawObjects(&p,_pix.rect());

  p.end();

  view->setDiffY(_yOffset);

  for (i = 0;i < static_cast<int>(objectList()->count());i++)
    {
      kpobject = objectList()->at(i);
      kpobject->drawSelection(true);
    }
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
void Page::drawPageInPainter(QPainter* painter,int __diffy,KRect _rect)
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
	_steps = static_cast<int>(static_cast<float>(kapp->desktop()->height()) / pageSpeedFakt());
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
	_steps = static_cast<int>(static_cast<float>(kapp->desktop()->width()) / pageSpeedFakt());
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
	_steps = static_cast<int>(static_cast<float>(kapp->desktop()->width()) / pageSpeedFakt());
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
	_steps = static_cast<int>(static_cast<float>(kapp->desktop()->height()) / pageSpeedFakt());
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
	_steps = static_cast<int>(static_cast<float>(kapp->desktop()->width()) / pageSpeedFakt());
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
	_steps = static_cast<int>(static_cast<float>(kapp->desktop()->width()) / pageSpeedFakt());
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
	    if ((_pix2.width() / _steps) * _step >= _pix2.width() &&
		(_pix2.height() / _steps) * _step >= _pix2.height()) break;
	  }
      } break;
    case PEF_INTERLOCKING_HORZ_1:
      {
	_steps = static_cast<int>(static_cast<float>(kapp->desktop()->width()) / pageSpeedFakt());
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
	_steps = static_cast<int>(static_cast<float>(kapp->desktop()->width()) / pageSpeedFakt());
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
	_steps = static_cast<int>(static_cast<float>(kapp->desktop()->height()) / pageSpeedFakt());
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
	_steps = static_cast<int>(static_cast<float>(kapp->desktop()->height()) / pageSpeedFakt());
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
    case PEF_SURROUND1:
      {
	int wid = kapp->desktop()->width() / 10;
	int hei = kapp->desktop()->height() / 10;

	int curr = 1;
	int curr2 = 1;

	_steps = static_cast<int>(static_cast<float>(kapp->desktop()->width()) / pageSpeedFakt());
	_time.start();

	for (;;)
	  {
	    kapp->processEvents();
	    if (_time.elapsed() >= 1)
	      {
		_step++;

		if (curr == 1 || curr == 5 || curr == 9 || curr == 13 || curr == 17)
		  {
		    int dx = (curr2 / 4) * wid;
		    int dy = (curr2 / 4) * hei;
		    _h = (_pix2.height() / _steps) * _step;
		    if (_h >= _pix2.height() - 2 * dy)
		      {
			_h = _pix2.height() - 2 * dy;
			curr++;
			_step = 0;
		      }
		    bitBlt(this,dx,dy,&_pix2,dx,dy,wid,_h);
		  }
		else if (curr == 2 || curr == 6 || curr == 10 || curr == 14 || curr == 18)
		  {
		    int dx = (curr2 / 4) * wid;
		    int dy = (curr2 / 4) * hei;
		    _w = (_pix2.width() / _steps) * _step;
		    if (_w >= _pix2.width() - wid - 2 * dx)
		      {
			_w = _pix2.width() - wid - 2 * dx;
			curr++;
			_step = 0;
		      }
		    bitBlt(this,dx + wid,height() - hei - dy,&_pix2,dx + wid,height() - hei - dy,_w,hei);
		  }
		else if (curr == 3 || curr == 7 || curr == 11 || curr == 15 || curr == 19)
		  {
		    int dx = (curr2 / 4) * wid;
		    int dy = (curr2 / 4) * hei;
		    _h = (_pix2.height() / _steps) * _step;
		    if (_h >= _pix2.height() - hei - 2 * dy)
		      {
			_h = _pix2.height() - hei - 2 * dy;
			curr++;
			_step = 0;
		      }
		    bitBlt(this,_pix2.width() - wid - dx,_pix2.height() - hei - dy - _h,&_pix2,
			   _pix2.width() - wid - dx,_pix2.height() - hei - dy - _h,wid,_h);
		  }
		else if (curr == 4 || curr == 8 || curr == 12 || curr == 16 || curr == 20)
		  {
		    int dx = (curr2 / 4) * wid;
		    int dy = (curr2 / 4) * hei;
		    _w = (_pix2.width() / _steps) * _step;
		    if (_w >= _pix2.width() - 2 * wid - 2 * dx)
		      {
			_w = _pix2.width() - 2 * wid - 2 * dx;
			_steps *= 2;
			_steps = static_cast<int>(static_cast<float>(_steps) / 1.5);
			curr++;
			curr2 += 4;
			_step = 0;
		      }
		    bitBlt(this,_pix2.width() - dx - wid - _w,dy,&_pix2,_pix2.width() - dx - wid - _w,dy,_w,hei);
		  }
		_time.restart();
	      }
	    if (curr == 21)
	      {
		bitBlt(this,0,0,&_pix2,0,0,_pix2.width(),_pix2.height());
		break;
	      }
	  }
      } break;
    case PEF_FLY1:
      {
	_steps = static_cast<int>(static_cast<float>(kapp->desktop()->height()) / pageSpeedFakt());
	_time.start();

	int _psteps = _steps / 5;
	KRect oldRect(0,0,kapp->desktop()->width(),kapp->desktop()->height());
	KSize ps;
	QPixmap pix3;
		 	
	for (;;)
	  {
	    kapp->processEvents();
	    if (_time.elapsed() >= 1)
	      {
		_step++;
		if (_step < _psteps)
		  {
		    pix3 = QPixmap(_pix1);
		    QPixmap pix4(_pix2);
		    float dw = static_cast<float>(_step * ((pix3.width() - (pix3.width() / 10)) / (2 * _psteps)));
		    float dh = static_cast<float>(_step * ((pix3.height() - (pix3.height() / 10)) / (2 * _psteps)));
	
		    dw *= 2;
		    dh *= 2;
	
		    QWMatrix m;
		    m.scale(static_cast<float>(pix3.width() - dw) / static_cast<float>(pix3.width()),
			    static_cast<float>(pix3.height() - dh) / static_cast<float>(pix3.height()));
		    pix3 = pix3.xForm(m);
		    //pix3.resize(pix3.width() - dw,pix3.height() - dh);
		    ps = pix3.size();
		
		    bitBlt(&pix4,(pix4.width() - pix3.width()) / 2,(pix4.height() - pix3.height()) / 2,
			   &pix3,0,0,pix3.width(),pix3.height());
		    KRect newRect((pix4.width() - pix3.width()) / 2,(pix4.height() - pix3.height()) / 2,
				  pix3.width(),pix3.height());
		    KRect r = newRect.unite(oldRect);
		    bitBlt(this,r.x(),r.y(),&pix4,r.x(),r.y(),r.width(),r.height());
		    oldRect = newRect;
		  }
		if (_step == _psteps)
		  {
// 		    pix3 = QPixmap(_pix1);
// 		    QWMatrix m;
// 		    m.scale(static_cast<float>(ps.width()) / static_cast<float>(pix3.width()),
// 			    static_cast<float>(ps.height()) / static_cast<float>(pix3.height()));
// 		    pix3 = pix3.xForm(m);
		  }
		if (_step > _psteps && _step < _psteps * 2)
		  {
		    QPixmap pix4(_pix2);
		    int yy = (_pix1.height() - pix3.height()) / 2 - (((_pix1.height() - pix3.height()) / 2) / _psteps) * (_step - _psteps);

		    bitBlt(&pix4,(pix4.width() - pix3.width()) / 2,yy,
			   &pix3,0,0,pix3.width(),pix3.height());
		    KRect newRect((pix4.width() - pix3.width()) / 2,yy,
				  pix3.width(),pix3.height());
		    KRect r = newRect.unite(oldRect);
		    bitBlt(this,r.x(),r.y(),&pix4,r.x(),r.y(),r.width(),r.height());
		    oldRect = newRect;
		  }
		if (_step > 2 * _psteps && _step < _psteps * 3)
		  {
		    QPixmap pix4(_pix2);
		    int xx = (_pix1.width() - pix3.width()) / 2 - (((_pix1.width() - pix3.width()) / 2) / _psteps) * (_step - 2 * _psteps);
		    int yy = (((_pix1.height() - pix3.height()) / 2) / _psteps) * (_step - 2 * _psteps);

		    bitBlt(&pix4,xx,yy,&pix3,0,0,pix3.width(),pix3.height());
		    KRect newRect(xx,yy,pix3.width(),pix3.height());
		    KRect r = newRect.unite(oldRect);
		    bitBlt(this,r.x(),r.y(),&pix4,r.x(),r.y(),r.width(),r.height());
		    oldRect = newRect;
		  }
		if (_step > 3 * _psteps && _step < _psteps * 5)
		  {
		    QPixmap pix4(_pix2);
		    int xx = ((_pix1.width() - pix3.width()) / _psteps) * (_step - 3 * _psteps);
		    int yy = ((_pix1.height() - pix3.height()) / 2) + (((_pix1.height() - pix3.height()) / 2) / _psteps) * (_step - 3 * _psteps);

		    bitBlt(&pix4,xx,yy,&pix3,0,0,pix3.width(),pix3.height());
		    KRect newRect(xx,yy,pix3.width(),pix3.height());
		    KRect r = newRect.unite(oldRect);
		    bitBlt(this,r.x(),r.y(),&pix4,r.x(),r.y(),r.width(),r.height());
		    oldRect = newRect;
		  }
		_time.restart();
	      }
	    if (_step >= _steps)
	      break;
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

	      int x = 0,y = 0,w = 0,h = 0;
	      KRect br = kpobject->getBoundingRect(0,0);
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
      _step_width = static_cast<int>((static_cast<float>(kapp->desktop()->width()) / objSpeedFakt()));
      _step_height = static_cast<int>((static_cast<float>(kapp->desktop()->height()) / objSpeedFakt()));
      _steps1 = x_pos1 > y_pos1 ? x_pos1 / _step_width : y_pos1 / _step_height;
      _steps2 = kapp->desktop()->width() - x_pos2 > kapp->desktop()->height() - y_pos2 ?
	(kapp->desktop()->width() - x_pos2) / _step_width : (kapp->desktop()->height() - y_pos2) / _step_height;
      _time.start();

      QList<KRect> xy;
      xy.setAutoDelete(true);

      for (;;)
	{
	  kapp->processEvents();
	  if (nothingHappens) break; // || _step >= _steps1 && _step >= _steps2) break;
	
	  QList<KRect> changes;
	  changes.setAutoDelete(true);
	
	  if (_time.elapsed() >= 1)
	    {
	      nothingHappens = true;
	      _step++;
	
	      changes.clear();
	
	      for (i = 0;i < static_cast<int>(_objList.count());i++)
		{
		  kpobject = _objList.at(i);
		  int _w =  kapp->desktop()->width() - (kpobject->getOrig().x() - diffx());
		  int _h =  kapp->desktop()->height() - (kpobject->getOrig().y() - diffy());
		  int ox = 0,oy = 0,ow = 0,oh = 0;
		  ox = kpobject->getOrig().x();
		  oy = kpobject->getOrig().y();
		  ow = kpobject->getSize().width();
		  oh = kpobject->getSize().height();

		  KRect br = kpobject->getBoundingRect(0,0);
		  int bx = br.x();
		  int by = br.y();
		  int bw = br.width();
		  int bh = br.height();
		
		  KRect oldRect;
		  KRect newRect;
		
		  if (static_cast<int>(xy.count() - 1) < i)
		    {
		      xy.append(new KRect(0,0,0,0));
		      oldRect.setRect(bx - diffx(),by - diffy(),bw,bh);
		    }
		  else
		    oldRect.setRect(bx - (diffx() - xy.at(i)->x()),by - (diffy() - xy.at(i)->y()),bw - xy.at(i)->width(),bh - xy.at(i)->height());
		
		  switch (kpobject->getEffect())
		    {
		    case EF_NONE:
		      {
			if (subPresStep == 0 || subPresStep != 0 && kpobject->getType() == OT_TEXT && kpobject->getEffect2() == EF2T_PARA)
			  drawObject(kpobject,screen,ox,oy,0,0,0,0);
		      } break;
		    case EF_COME_LEFT:
		      {
			if (subPresStep == 0 || subPresStep != 0 && kpobject->getType() == OT_TEXT && kpobject->getEffect2() == EF2T_PARA)
			  {
			    x_pos1 = _step_width * _step < ox - diffx() + ow ?
			      ox - diffx() + ow - _step_width * _step : 0;
			    y_pos1 = 0;
			    drawObject(kpobject,screen,-x_pos1,y_pos1,0,0,0,0);
			    if (x_pos1 != 0) nothingHappens = false;
			    xy.at(i)->setX(-x_pos1);
			    xy.at(i)->setY(y_pos1);
			    xy.at(i)->setWidth(0);
			    xy.at(i)->setHeight(0);
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
			    xy.at(i)->setX(x_pos1);
			    xy.at(i)->setY(-y_pos1);
			    xy.at(i)->setWidth(0);
			    xy.at(i)->setHeight(0);
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
			    xy.at(i)->setX(x_pos2);
			    xy.at(i)->setY(y_pos2);
			    xy.at(i)->setWidth(0);
			    xy.at(i)->setHeight(0);
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
			    xy.at(i)->setX(x_pos2);
			    xy.at(i)->setY(y_pos2);
			    xy.at(i)->setWidth(0);
			    xy.at(i)->setHeight(0);
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
			    xy.at(i)->setX(-x_pos1);
			    xy.at(i)->setY(-y_pos1);
			    xy.at(i)->setWidth(0);
			    xy.at(i)->setHeight(0);
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
			    xy.at(i)->setX(-x_pos1);
			    xy.at(i)->setY(y_pos2);
			    xy.at(i)->setWidth(0);
			    xy.at(i)->setHeight(0);
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
			    xy.at(i)->setX(x_pos2);
			    xy.at(i)->setY(-y_pos1);
			    xy.at(i)->setWidth(0);
			    xy.at(i)->setHeight(0);
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
			    xy.at(i)->setX(x_pos2);
			    xy.at(i)->setY(y_pos2);
			    xy.at(i)->setWidth(0);
			    xy.at(i)->setHeight(0);
			  }
		      } break;
		    case EF_WIPE_LEFT:
		      {
			if (subPresStep == 0 || subPresStep != 0 && kpobject->getType() == OT_TEXT && kpobject->getEffect2() == EF2T_PARA)
			  {
			    w_pos1 = _step_width * (_steps1 - _step) > 0 ? _step_width * (_steps1 - _step) : 0;
			    drawObject(kpobject,screen,0,0,w_pos1,0,0,0);
 			    if (w_pos1 != 0) nothingHappens = false;
			    xy.at(i)->setX(0);
			    xy.at(i)->setY(0);
			    xy.at(i)->setWidth(w_pos1);
			    xy.at(i)->setHeight(0);
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
			    xy.at(i)->setX(x_pos1);
			    xy.at(i)->setY(0);
			    xy.at(i)->setWidth(w_pos1);
			    xy.at(i)->setHeight(0);
			  }
		      } break;
		    case EF_WIPE_TOP:
		      {
			if (subPresStep == 0 || subPresStep != 0 && kpobject->getType() == OT_TEXT && kpobject->getEffect2() == EF2T_PARA)
			  {
			    h_pos1 = _step_height * (_steps1 - _step) > 0 ? _step_height * (_steps1 - _step) : 0;
			    drawObject(kpobject,screen,0,0,0,h_pos1,0,0);
 			    if (h_pos1 != 0) nothingHappens = false;
			    xy.at(i)->setX(0);
			    xy.at(i)->setY(0);
			    xy.at(i)->setWidth(0);
			    xy.at(i)->setHeight(h_pos1);
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
			    xy.at(i)->setX(0);
			    xy.at(i)->setY(y_pos1);
			    xy.at(i)->setWidth(0);
			    xy.at(i)->setHeight(h_pos1);
			  }
		      } break;
		    default: break;
		    }
		
		  newRect.setRect(bx - (diffx() - xy.at(i)->x()),by - (diffy() - xy.at(i)->y()),bw - xy.at(i)->width(),bh - xy.at(i)->height());
		  newRect = newRect.unite(oldRect);

		  bool append = true;
		  for (unsigned j = 0;j < changes.count();j++)
		    {
		      if (changes.at(j)->intersects(newRect))
			{
			  KRect r = changes.at(j)->intersect(newRect);
			  int s1 = r.width() * r.height();
			  int s2 = newRect.width() * newRect.height();
			  int s3 = changes.at(j)->width() * changes.at(j)->height();
			
			  if (s1 > (s2 / 100) * 50 || s1 > (s3 / 100) * 50)
			    {
			      KRect rr = changes.at(j)->unite(newRect);
			      changes.at(j)->setRect(rr.x(),rr.y(),rr.width(),rr.height());
			      append = false;			
			    }
			
			  break;
			}
		    }
		
		  if (append)
		    changes.append(new KRect(newRect));
		}
	
	      KRect *changed;
	      for (i = 0;i < static_cast<int>(changes.count());i++)
		{
		  changed = changes.at(i);
		  bitBlt(this,changed->x(),changed->y(),screen,changed->x(),changed->y(),changed->width(),changed->height());
		}
	
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
      drawObjects(&p,KRect(0,0,kapp->desktop()->width(),kapp->desktop()->height()));
      p.end();
    }
  else
    {
      QPainter p;
      p.begin(screen);
      drawObjects(&p,KRect(0,0,kapp->desktop()->width(),kapp->desktop()->height()));
      p.end();
      bitBlt(this,0,0,screen);
    }

  delete screen;
}

/*======================= draw object ============================*/
void Page::drawObject(KPObject *kpobject,QPixmap *screen,int _x,int _y,int _w,int _h,int _cx,int _cy)
{
  int ox,oy,ow,oh;
  KRect br = kpobject->getBoundingRect(0,0);
  ox = br.x(); oy = br.y(); ow = br.width(); oh = br.height();
  bool ownClipping = true;

  QPainter p;
  p.begin(screen);
  //p.begin(this);

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

  KPObject *obj = 0;
  for (unsigned int i = tmpObjs.findRef(kpobject) + 1;i < tmpObjs.count();i++)
    {
      obj = tmpObjs.at(i);
      if (kpobject->getBoundingRect(0,0).intersects(obj->getBoundingRect(0,0)) && obj->getPresNum() < static_cast<int>(currPresStep))
	obj->draw(&p,diffx(),diffy());
    }

  p.end();
}

/*======================== print =================================*/
void Page::print(QPainter *painter,QPrinter *printer,float left_margin,float top_margin)
{
  int i = 0;

  repaint(false);
  kapp->processEvents();

  editMode = false;
  fillBlack = false;
  _presFakt = 1.0;

  int _xOffset = view->getDiffX();
  int _yOffset = view->getDiffY();

  currPresStep = 1000;
  subPresStep = 1000;

  for (i = 0;i < static_cast<int>(objectList()->count());i++)
    objectList()->at(i)->drawSelection(false);

  view->setDiffX(-static_cast<int>(MM_TO_POINT(left_margin)));
  view->setDiffY(-static_cast<int>(MM_TO_POINT(top_margin)));

  QColor c = kapp->winStyleHighlightColor();
  kapp->setWinStyleHighlightColor(colorGroup().highlight());

  QProgressBar progBar;

  QProgressDialog progress(i18n("Printing..."),i18n("Cancel"),printer->toPage() - printer->fromPage() + 2,this);
  int j = 0;
  progress.setProgress(0);

  if (printer->fromPage() > 1)
    view->setDiffY((printer->fromPage() - 1) * (getPageSize(1,1.0,false).height()) - MM_TO_POINT(top_margin));

  for (i = printer->fromPage();i <= printer->toPage();i++)
    {
      progress.setProgress(++j);
      kapp->processEvents();

      if (progress.wasCancelled())
	break;

      currPresPage = i;
      if (i > printer->fromPage()) printer->newPage();

      painter->resetXForm();
      painter->fillRect(getPageSize(0),white);

      drawPageInPainter(painter,view->getDiffY(),getPageSize(i - 1));
      kapp->processEvents();

      painter->resetXForm();
      view->presentParts(1.0,painter,getPageSize(i - 1),diffx(),diffy());
      kapp->processEvents();

      view->setDiffY(i * (getPageSize(1,1.0,false).height()) - MM_TO_POINT(top_margin));
    }

  setToolEditMode(toolEditMode);
  view->setDiffX(_xOffset);
  view->setDiffY(_yOffset);

  progress.setProgress(printer->toPage() - printer->fromPage() + 2);
  kapp->setWinStyleHighlightColor(c);

  for (i = 0;i < static_cast<int>(objectList()->count());i++)
    objectList()->at(i)->drawSelection(true);

  currPresPage = 1;
  currPresStep = 0;
  subPresStep = 0;
  _presFakt = 1.0;
  fillBlack = true;
  editMode = true;
  repaint(false);
}

/*================================================================*/
void Page::editSelectedTextArea()
{
  KPObject *kpobject = 0;

  if (objectList()->count() - 1 >= 0)
    {
      for (int i = static_cast<int>(objectList()->count()) - 1;i >= 0;i--)
	{
	  kpobject = objectList()->at(i);
	  if (kpobject->isSelected())
	    {
	      if (kpobject->getType() == OT_TEXT)
		{
		  KPTextObject *kptextobject = dynamic_cast<KPTextObject*>(kpobject);
		
		  kpobject->activate(this,diffx(),diffy());
		  kptextobject->getKTextObject()->setBackgroundColor(txtBackCol());
		  //setFocusProxy(kptextobject->getKTextObject());
		  //setFocusPolicy(QWidget::StrongFocus);
		  //kptextobject->getKTextObject()->setFocus();
		  //kptextobject->getKTextObject()->setShowCursor(true);
		  connect(kptextobject->getKTextObject(),SIGNAL(fontChanged(QFont*)),this,SLOT(toFontChanged(QFont*)));
		  connect(kptextobject->getKTextObject(),SIGNAL(giveMeFocus()),view,SLOT(sendFocusEvent()));
		  connect(kptextobject->getKTextObject(),SIGNAL(colorChanged(QColor*)),this,SLOT(toColorChanged(QColor*)));
		  connect(kptextobject->getKTextObject(),SIGNAL(horzAlignChanged(TxtParagraph::HorzAlign)),
			  this,SLOT(toAlignChanged(TxtParagraph::HorzAlign)));
		  editNum = i;
		  break;
		}
	    }
	}
    }
}

/*================================================================*/
void Page::insertText(KRect _r)
{
  view->kPresenterDoc()->insertText(_r,diffx(),diffy());
}

/*================================================================*/
void Page::insertLineH(KRect _r,bool rev)
{
  view->kPresenterDoc()->insertLine(_r,view->getPen(),
				    !rev ? view->getLineBegin() : view->getLineEnd(),!rev ? view->getLineEnd() : view->getLineBegin(),
				    LT_HORZ,diffx(),diffy());
}

/*================================================================*/
void Page::insertLineV(KRect _r,bool rev)
{
  view->kPresenterDoc()->insertLine(_r,view->getPen(),
				    !rev ? view->getLineBegin() : view->getLineEnd(),!rev ? view->getLineEnd() : view->getLineBegin(),
				    LT_VERT,diffx(),diffy());
}

/*================================================================*/
void Page::insertLineD1(KRect _r,bool rev)
{
  view->kPresenterDoc()->insertLine(_r,view->getPen(),
				    !rev ? view->getLineBegin() : view->getLineEnd(),!rev ? view->getLineEnd() : view->getLineBegin(),
				    LT_LU_RD,diffx(),diffy());
}

/*================================================================*/
void Page::insertLineD2(KRect _r,bool rev)
{
  view->kPresenterDoc()->insertLine(_r,view->getPen(),
				    !rev ? view->getLineBegin() : view->getLineEnd(),!rev ? view->getLineEnd() : view->getLineBegin(),
				    LT_LD_RU,diffx(),diffy());
}

/*================================================================*/
void Page::insertRect(KRect _r)
{
  view->kPresenterDoc()->insertRectangle(_r,view->getPen(),view->getBrush(),view->getFillType(),
					 view->getGColor1(),view->getGColor2(),view->getGType(),view->getRndX(),view->getRndY(),
					 diffx(),diffy());
}

/*================================================================*/
void Page::insertEllipse(KRect _r)
{
  view->kPresenterDoc()->insertCircleOrEllipse(_r,view->getPen(),view->getBrush(),view->getFillType(),
					       view->getGColor1(),view->getGColor2(),
					       view->getGType(),diffx(),diffy());
}

/*================================================================*/
void Page::insertPie(KRect _r)
{
  view->kPresenterDoc()->insertPie(_r,view->getPen(),view->getBrush(),view->getFillType(),
				   view->getGColor1(),view->getGColor2(),view->getGType(),
				   view->getPieType(),view->getPieAngle(),view->getPieLength(),
				   view->getLineBegin(),view->getLineEnd(),diffx(),diffy());
}

/*================================================================*/
void Page::insertAutoform(KRect _r,bool rev)
{
  rev = false;
  view->kPresenterDoc()->insertAutoform(_r,view->getPen(),view->getBrush(),
					!rev ? view->getLineBegin() : view->getLineEnd(),!rev ? view->getLineEnd() : view->getLineBegin(),
					view->getFillType(),view->getGColor1(),view->getGColor2(),view->getGType(),
					autoform,diffx(),diffy());
}

/*================================================================*/
void Page::insertObject(KRect _r)
{
  view->kPresenterDoc()->insertObject(_r,partEntry,diffx(),diffy());
}

/*================================================================*/
void Page::insertTable(KRect _r)
{
  view->kPresenterDoc()->insertObject(_r,partEntry,diffx(),diffy());
}

/*================================================================*/
void Page::insertDiagramm(KRect _r)
{
  view->kPresenterDoc()->insertObject(_r,partEntry,diffx(),diffy());
}

/*================================================================*/
void Page::insertFormula(KRect _r)
{
  view->kPresenterDoc()->insertObject(_r,partEntry,diffx(),diffy());
}

/*================================================================*/
void Page::setToolEditMode(ToolEditMode _m)
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
	  disconnect(kptextobject->getKTextObject(),SIGNAL(giveMeFocus()),view,SLOT(sendFocusEvent()));
	  disconnect(kptextobject->getKTextObject(),SIGNAL(colorChanged(QColor*)),this,SLOT(toColorChanged(QColor*)));
	  disconnect(kptextobject->getKTextObject(),SIGNAL(horzAlignChanged(TxtParagraph::HorzAlign)),
		     this,SLOT(toAlignChanged(TxtParagraph::HorzAlign)));
	  kptextobject->getKTextObject()->setShowCursor(false);
	  //setFocusProxy(0);
	  //setFocusPolicy(QWidget::NoFocus);
	}
      else if (kpobject->getType() == OT_PART)
	{
	  kpobject->deactivate();
	  _repaint(kpobject);
	  //setFocusProxy(0);
	  //setFocusPolicy(QWidget::NoFocus);
	}
    }

  toolEditMode = _m;

  if (toolEditMode == TEM_MOUSE)
    {
      setCursor(arrowCursor);
      for (int i = static_cast<int>(objectList()->count()) - 1;i >= 0;i--)
	{
	  kpobject = objectList()->at(i);
	  if (kpobject->contains(QCursor::pos(),diffx(),diffy()))
	    {
	      if (kpobject->isSelected())
		setCursor(kpobject->getCursor(QCursor::pos(),diffx(),diffy(),modType));
	      break;
	    }
	}
    }
  else
    setCursor(crossCursor);

  view->setTool(toolEditMode);
}

/*================================================================*/
void Page::selectNext()
{
  if (objectList()->count() == 0) return;

  if (view->kPresenterDoc()->numSelected() == 0)
    objectList()->at(0)->setSelected(true);
  else
    {
      int i = objectList()->findRef(view->kPresenterDoc()->getSelectedObj());
      if (i < static_cast<int>(objectList()->count()) - 1)
	{
	  view->kPresenterDoc()->deSelectAllObj();
	  objectList()->at(++i)->setSelected(true);
	}
      else
	{
	  view->kPresenterDoc()->deSelectAllObj();
	   objectList()->at(0)->setSelected(true);
	}
    }
  if (!KRect(diffx(),diffy(),width(),height()).contains(view->kPresenterDoc()->getSelectedObj()->getBoundingRect(0,0)))
    view->makeRectVisible(view->kPresenterDoc()->getSelectedObj()->getBoundingRect(0,0));
  _repaint(false);
}

/*================================================================*/
void Page::selectPrev()
{
  if (objectList()->count() == 0) return;

  if (view->kPresenterDoc()->numSelected() == 0)
    objectList()->at(objectList()->count() - 1)->setSelected(true);
  else
    {
      int i = objectList()->findRef(view->kPresenterDoc()->getSelectedObj());
      if (i > 0)
	{
	  view->kPresenterDoc()->deSelectAllObj();
	  objectList()->at(--i)->setSelected(true);
	}
      else
	{
	  view->kPresenterDoc()->deSelectAllObj();
	  objectList()->at(objectList()->count() - 1)->setSelected(true);
	}
    }
  view->makeRectVisible(view->kPresenterDoc()->getSelectedObj()->getBoundingRect(0,0));
  _repaint(false);
}

/*================================================================*/
void Page::dragEnterEvent(QDragEnterEvent *e)
{
  if (QUrlDrag::canDecode(e) ||
      QTextDrag::canDecode(e) ||
      QImageDrag::canDecode(e))
    e->accept();
}

/*================================================================*/
void Page::dragLeaveEvent(QDragLeaveEvent *e)
{
}

/*================================================================*/
void Page::dragMoveEvent(QDragMoveEvent *e)
{
}

/*================================================================*/
void Page::dropEvent(QDropEvent *e)
{
  if (QImageDrag::canDecode(e))
    {
      setToolEditMode(TEM_MOUSE);
      deSelectAllObj();

      QImage pix;
      QImageDrag::decode(e,pix);

      QString uid = getenv("USER");
      QString num;
      num.setNum(objectList()->count());
      uid += "_";
      uid += num;

      QString filename = "/tmp/kpresenter";
      filename += uid;
      filename += ".xpm";

      pix.save(filename,"XPM");
      view->kPresenterDoc()->insertPicture(filename,e->pos().x(),e->pos().y());

      QString cmd = "rm -f ";
      cmd += filename;
      system(cmd.ascii());
    }
  else if (QTextDrag::canDecode(e))
    {
      setToolEditMode(TEM_MOUSE);
      deSelectAllObj();

      QString text;
      QTextDrag::decode(e,text);

      view->kPresenterDoc()->insertText(KRect(e->pos().x(),e->pos().y(),250,250),diffx(),diffy(),text,view);
    }
  else if (QUrlDrag::canDecode(e))
    {
      setToolEditMode(TEM_MOUSE);
      deSelectAllObj();

      QStrList lst;
      QUrlDrag::decode(e,lst);

      QString str;
      for (str = lst.first();!str.isEmpty();str = lst.next())
	{
// 	  QString uid = getenv("USER");
// 	  QString num;
// 	  num.setNum(objectList()->count());
// 	  uid += "_";
// 	  uid += num;
		
// 	  QString filename = "/tmp/kpresenter";
// 	  filename += uid;
		
// 	  KIOJob *job = new KIOJob("kpresenter job");
// 	  job->copy(str,filename);

	  // Currently we only allow local files - this should be changed later
	  KURL url(str);
	  if (!url.isLocalFile()) return;

	  QString filename = url.path();
 	  KMimeMagicResult *res = KMimeMagic::self()->findFileType(filename);
	
	  if (res && res->isValid())
	    {
	      QString mimetype = res->mimeType();
	      if (mimetype.contains("image"))
		{
		  view->kPresenterDoc()->insertPicture(filename,e->pos().x(),e->pos().y());
      		  continue;
		}	
	
	    }
	
	  // open any non-picture as text
	  // in the future we should open specific mime types with "their" programms and embed them
	  QFile f(filename);
	  QTextStream t(&f);
	  QString text = "",tmp;

	  if (f.open(IO_ReadOnly))
	    {
	      while (!t.eof())
		{
		  tmp = t.readLine();
		  tmp += "\n";
		  text.append(tmp);
		}
	      f.close();
	    }
	  view->kPresenterDoc()->insertText(KRect(e->pos().x(),e->pos().y(),250,250),diffx(),diffy(),text,view);

// 	  QString cmd = "rm -f ";
// 	  cmd += filename;
// 	  system(cmd.ascii());
	
// 	  delete job;
	}
    }
}

/*================================================================*/
void Page::slotGotoPage()
{
  setCursor(blankCursor);
  int pg = currPresPage;
  pg = KPGotoPage::gotoPage(1,pageNums(),pg,this);

  if (pg != static_cast<int>(currPresPage))
    {
      currPresPage = pg;
      editMode = false;
      drawMode = false;
      presStepList = view->kPresenterDoc()->reorderPage(currPresPage,diffx(),diffy(),_presFakt);
      currPresStep = (int)(presStepList.first());
      subPresStep = 0;

      int yo = view->kPresenterDoc()->getPageSize(0,0,0,presFakt(),false).height() * (pg - 1);
      view->setDiffY(yo);
      resize(QApplication::desktop()->width(),QApplication::desktop()->height());
      repaint(false);
      setFocus();

      QPainter p(this);
      view->presentParts(presFakt(),&p,KRect(0,0,0,0),diffx(),diffy());
      p.end();
    }
}

