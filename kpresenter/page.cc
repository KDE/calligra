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
/* Module: Page                                                   */
/******************************************************************/

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
      resizeObjNum = 0;
      editNum = 0;
      drawBack = true;
      txtPtr = 0;
      graphPtr = 0;
      setupMenus();
      setBackgroundColor(white);
      view = _view;
      setMouseTracking(true);
      show();
      editMode = true;
      currPresPage = 1;
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

/*======================== paint event ===========================*/
void Page::paintEvent(QPaintEvent* paintEvent)
{
  QPainter painter;
  QBrush brush(darkGray);
  QPen pen(NoPen);

  painter.begin(this);
  painter.setClipping(true);
  painter.setClipRect(paintEvent->rect());
  /* paint background */
  if (drawBack) paintBackground(&painter,paintEvent->rect());
  if (!objList()->isEmpty()) paintObjects(&painter,paintEvent->rect());
  painter.end();
}

/*====================== paint background ========================*/
void Page::paintBackground(QPainter *painter,QRect rect)
{
  int i,j,pw,ph;

  painter->setPen(NoPen);
  painter->setBrush(white);
  painter->drawRect(0,0,width(),20);
  painter->drawRect(0,0,20,height());
  QRect r = painter->viewport();
  for (pagePtr=pageList()->first();pagePtr != 0;pagePtr=pageList()->next())
    {
      painter->setViewport(r);
      painter->resetXForm();
      r = painter->viewport();
      if ((rect.intersects(QRect(getPageSize(pagePtr->pageNum))) && editMode) ||
	  (!editMode && currPresPage == pageList()->at()+1))
	{
	  switch (pagePtr->backType)
	    {
	    case BT_COLOR: /* background color */ 
	      {
		painter->drawPixmap(getPageSize(pagePtr->pageNum).x(),getPageSize(pagePtr->pageNum).y(),
				    *pagePtr->cPix);
		pagePtr->pic->hide();
	      } break;
	    case BT_PIC:  /* background picture */
	      {
		if (pagePtr->backPicView == BV_CENTER)
		  painter->drawPixmap(getPageSize(pagePtr->pageNum).x(),getPageSize(pagePtr->pageNum).y(),
				      *pagePtr->cPix);
		pagePtr->pic->hide();
		switch (pagePtr->backPicView)
		  {
		  case BV_ZOOM:
		    {
		      if (!pagePtr->backPix.isNull())
			painter->drawPixmap(getPageSize(pagePtr->pageNum).x(),getPageSize(pagePtr->pageNum).y(),
					    pagePtr->backPix);
		    } break;
		  case BV_CENTER:
		    {
		      if (!pagePtr->backPix.isNull())
			{
			  if (pagePtr->backPix.width() > getPageSize(pagePtr->pageNum).width())
			    pagePtr->backPix.resize(getPageSize(pagePtr->pageNum).width(),
						    pagePtr->backPix.height());
			  if (pagePtr->backPix.height() > getPageSize(pagePtr->pageNum).height())
			    pagePtr->backPix.resize(pagePtr->backPix.width(),
						    getPageSize(pagePtr->pageNum).height());
			  painter->drawPixmap(getPageSize(pagePtr->pageNum).x()+(getPageSize(pagePtr->pageNum).width()-
										 pagePtr->backPix.width())/2,
					      getPageSize(pagePtr->pageNum).y()+(getPageSize(pagePtr->pageNum).height()-
										 pagePtr->backPix.height())/2,
					      pagePtr->backPix);
			}
		    } break;
		  case BV_TILED:
		    {
		      if (!pagePtr->backPix.isNull())
			{
			  for (i=0;i<=getPageSize(pagePtr->pageNum).height()/pagePtr->backPix.height();i++)
			    {
			      ph = -1;
			      if (getPageSize(pagePtr->pageNum).height()-i*pagePtr->backPix.height() < pagePtr->backPix.height())
				ph = getPageSize(pagePtr->pageNum).height()-i*pagePtr->backPix.height();
			      for (j=0;j<=getPageSize(pagePtr->pageNum).width()/pagePtr->backPix.width();j++)
				{
				  pw = -1;
				  if (getPageSize(pagePtr->pageNum).width()-j*pagePtr->backPix.width() < pagePtr->backPix.width())
				    pw = getPageSize(pagePtr->pageNum).width()-j*pagePtr->backPix.width();
				  painter->drawPixmap(getPageSize(pagePtr->pageNum).x()+j*pagePtr->backPix.width(),
						      getPageSize(pagePtr->pageNum).y()+i*pagePtr->backPix.height(),
						      pagePtr->backPix,0,0,pw,ph);
				}
			    }
			}
		    } break;
		  }
	      } break;
	    case BT_CLIP: /* background clipart */ 
	      {
		painter->drawPixmap(getPageSize(pagePtr->pageNum).x(),getPageSize(pagePtr->pageNum).y(),
				    *pagePtr->cPix);
		r = painter->viewport();
		painter->setViewport(getPageSize(pagePtr->pageNum).x(),
				     getPageSize(pagePtr->pageNum).y(),
				     getPageSize(pagePtr->pageNum).width(),
				     getPageSize(pagePtr->pageNum).height());
		QPicture *pic = pagePtr->pic->getPic();
		pic->play(painter);
		painter->setViewport(r);
		painter->resetXForm();
	      } break;
	    }
	  painter->setViewport(r);
	  painter->resetXForm();
	  r = painter->viewport();
	}
      painter->setPen(NoPen);
      painter->setBrush(white);
      painter->drawRect(0,getPageSize(pagePtr->pageNum).y()+getPageSize(pagePtr->pageNum).height(),
			width(),20);
      if (editMode)
	{
	  painter->setPen(QPen(red,1,SolidLine));
	  painter->setBrush(NoBrush);
	  painter->drawRect(getPageSize(pagePtr->pageNum));
	}
    }
  if (pageList() && !pageList()->isEmpty())
      {
	unsigned int num = pageList()->count();
	painter->setPen(NoPen);
	painter->setBrush(white);
	painter->drawRect(0,getPageSize(num).y()+getPageSize(num).height()+1,
			  width(),height());
 	painter->drawRect(getPageSize(num).x()+getPageSize(num).width(),0,
 			  width(),height());
      }
}

/*====================== paint objects ==========================*/
void Page::paintObjects(QPainter *painter,QRect rect)
{
  QRect r = painter->viewport();
  objPtr = 0;
  for (objPtr=objList()->first();objPtr != 0;objPtr=objList()->next())
    {
      painter->setViewport(r);
      painter->resetXForm();
      r = painter->viewport();
      /* draw the objects */
      if ((rect.intersects(QRect(objPtr->ox - diffx(),objPtr->oy - diffy(),
				 objPtr->ow,objPtr->oh)) && editMode) ||
	  (!editMode && QRect(objPtr->ox - diffx(),objPtr->oy - diffy(),
			      objPtr->ow,objPtr->oh).intersects(QRect((width() - getPageSize(currPresPage).width()) / 2 + 10,
								      (height() - getPageSize(currPresPage).height()) / 2 + 10,
								      getPageSize(currPresPage).width(),
								      getPageSize(currPresPage).height()))))
	{     
	  switch (objPtr->objType)
	    {
	    case OT_PICTURE: /* pciture */
	      {
		if (!mousePressed || mousePressed && !objPtr->isSelected)
		  {
		    painter->drawPixmap(objPtr->ox - diffx(),objPtr->oy - diffy(),
					objPtr->graphObj->getPix());
		    //0,0,objPtr->ow,objPtr->oh);
		  }
	      } break;
	    case OT_TEXT: /* text */
	      {
 		objPtr->objPic = objPtr->textObj->getPic(objPtr->ox - diffx(),objPtr->oy - diffy(),
							 objPtr->ow,objPtr->oh);
 		painter->translate((float)objPtr->ox - diffx(),(float)objPtr->oy - diffy());
 		objPtr->objPic->play(painter);
 		painter->resetXForm();
 		painter->setClipping(false);
 		painter->setClipping(true);
 		painter->setClipRect(rect);
	      } break;
	    default:
	      {
		if (objPtr->objType != OT_CLIPART)
		  painter->translate((float)objPtr->ox - diffx(),(float)objPtr->oy - diffy());
		if (objPtr->objType == OT_CLIPART)
		  {
		    r = painter->viewport();
		    painter->setViewport(objPtr->ox - diffx(),objPtr->oy - diffy(),
					 objPtr->ow,objPtr->oh);
		  }
		if (objPtr->objType != OT_CLIPART || 
		    objPtr->objType == OT_CLIPART && !mousePressed ||
		    mousePressed && !objPtr->isSelected)
		  {
		    objPtr->objPic = objPtr->graphObj->getPic(objPtr->ox - diffx(),objPtr->oy - diffy(),objPtr->ow,objPtr->oh);
		    objPtr->objPic->play(painter);
		  }
		painter->translate((float)objPtr->ox - diffx(),(float)objPtr->oy - diffy());
		if (objPtr->objType == OT_CLIPART)
		  {
		    painter->setViewport(r);
		    painter->resetXForm();
		  }
		painter->resetXForm();
		painter->setClipping(false);
		painter->setClipping(true);
		painter->setClipRect(rect);
	      } break;
	    }
	  /* draw selection, if selected */
	  if (objPtr->isSelected)
	    {
	      painter->setBrush(black);
	      painter->setPen(NoPen);
	      painter->drawRect(objPtr->ox - diffx(),objPtr->oy - diffy(),
				6,6);
	      painter->drawRect(objPtr->ox - diffx() + objPtr->ow - 6,objPtr->oy - diffy(),
				6,6);
	      painter->drawRect(objPtr->ox - diffx(),objPtr->oy - diffy() + objPtr->oh - 6,
				6,6);
	      painter->drawRect(objPtr->ox - diffx() + objPtr->ow - 6,objPtr->oy - diffy() + objPtr->oh - 6,
				6,6);
	      painter->drawRect(objPtr->ox - diffx() + objPtr->ow / 2 - 3,objPtr->oy - diffy(),
				6,6);
	      painter->drawRect(objPtr->ox - diffx() + objPtr->ow / 2 - 3,objPtr->oy - diffy() + objPtr->oh - 6,
				6,6);
	      painter->drawRect(objPtr->ox - diffx(),objPtr->oy - diffy() + objPtr->oh / 2 - 3,
				6,6);
	      painter->drawRect(objPtr->ox - diffx() + objPtr->ow - 6,objPtr->oy - diffy() + objPtr->oh / 2 - 3,
				6,6);
	      //painter->setBrush(NoBrush);
	      //if (mousePressed && (objPtr->objType == OT_PICTURE || objPtr->objType == OT_CLIPART))
	      //painter->setPen(QPen(gray,1,DotLine));
	      //else
	      //painter->setPen(QPen(gray,1,DashLine));
	      //painter->drawRect(objPtr->ox - diffx(),objPtr->oy - diffy(),
	      //objPtr->ow,objPtr->oh);
	    }
	}
    }
}

/*==================== handle mouse pressed ======================*/
void Page::mousePressEvent(QMouseEvent *e)
{
  int objNum;
  QPoint pnt;
  
  oldMx = e->x()+diffx();
  oldMy = e->y()+diffy();
  
  if (editMode)
    {
      if (editNum > 0)
	{
	  objPtr = getObject(editNum);
	  editNum = 0;
	  if (txtPtr)
	    {
	      objPtr->textObj->recreate(0,0,QPoint(objPtr->ox - diffx(),objPtr->oy - diffy()),false);
	      objPtr->textObj->clearFocus();
	      objPtr->textObj->hide();
	      disconnect(objPtr->textObj,SIGNAL(fontChanged(QFont*)),this,SLOT(toFontChanged(QFont*)));
	      disconnect(objPtr->textObj,SIGNAL(colorChanged(QColor*)),this,SLOT(toColorChanged(QColor*)));
	      disconnect(objPtr->textObj,SIGNAL(horzAlignChanged(TxtParagraph::HorzAlign)),this,SLOT(toAlignChanged(TxtParagraph::HorzAlign)));
	      txtPtr->setShowCursor(false);
	      txtPtr = 0;
	      setFocusProxy(0);
	      setFocusPolicy(QWidget::NoFocus);
	    }
	  if (graphPtr)
	    {
	      objPtr->graphObj->hide();
	      graphPtr = 0;
	    }
	}      
      if ((e->button() == LeftButton || e->button() == RightButton) && (!mousePressed))
	{
	  modType = MT_NONE;
	  mousePressed = true;
	  objNum = getObjectAt(e->x()+diffx(),e->y()+diffy());
	  if (objNum > 0)
	    {
	      objPtr = getObject(objNum);
	      if (!objPtr->isSelected)
		{
		  if (((e->state() & ShiftButton) != 0) ||
		      ((e->state() & ControlButton) != 0))
		    selectObj(objNum);
		  else
		    {
		      deSelectAllObj();
		      objPtr = getObject(objNum);
		      selectObj(objNum);
		    }
		}
	    }
	  else
	    {
	      deSelectAllObj();
	      objPtr = getObject(objNum);
	    }
	}  
      if (e->button() == RightButton)
	{
	  objNum = getObjectAt(e->x()+diffx(),e->y()+diffy());
	  if (objNum > 0)
	    {
	      objPtr = getObject(objNum);
	      if (objPtr->objType == OT_PICTURE)
		{
		  mousePressed = false;
		  deSelectAllObj();
		  selectObj(objNum);
		  pnt.operator=(QCursor::pos());
		  picMenu->popup(pnt);
		  modType = MT_NONE;
		  resizeObjNum = 0;
		}
	      else if (objPtr->objType == OT_CLIPART)
		{
		  mousePressed = false;
		  deSelectAllObj();
		  selectObj(objNum);
		  pnt.operator=(QCursor::pos());
		  clipMenu->popup(pnt);
		  modType = MT_NONE;
		  resizeObjNum = 0;
		}
	      else if (objPtr->objType == OT_TEXT)
		{
		  pnt.operator=(QCursor::pos());
		  txtMenu->popup(pnt);
		  mousePressed = false;
		  modType = MT_NONE;
		  resizeObjNum = 0;
		}
	      else
		{
		  pnt.operator=(QCursor::pos());
		  graphMenu->popup(pnt);
		  mousePressed = false;
		  modType = MT_NONE;
		  resizeObjNum = 0;
		}
	    }
	}
    }
  else
    {
    }
  mouseMoveEvent(e);
}

/*=================== handle mouse released ======================*/
void Page::mouseReleaseEvent(QMouseEvent *e)
{
  mousePressed = false;
  modType = MT_NONE;
  resizeObjNum = 0;
  mouseMoveEvent(e);
  drawBack = false;
  _repaint(false);
  drawBack = true;
}

/*==================== handle mouse moved ========================*/
void Page::mouseMoveEvent(QMouseEvent *e)
{
  int mx,my,oox,ooy,oow,ooh,ox,oy,ow,oh,objNum;
  unsigned int i;
  
  if (!objList()->isEmpty() && editMode)
    {
      mx = e->x()+diffx(); my = e->y()+diffy();
      if ((!mousePressed) || (modType == MT_NONE))
	{
	  objNum = getObjectAt(mx,my);
	  if (objNum > 0)
	    {
	      objPtr = getObject(objNum);
	      if (objPtr->isSelected)
		{
		  ox = objPtr->ox; oy = objPtr->oy;
		  ow = objPtr->ow; oh = objPtr->oh;
		  setCursor(sizeAllCursor);
		  modType = MT_MOVE;
		  if ((mx >= ox+6) && (my >= oy) && (mx <= ox+ow-6) && (my <= oy+6))
		    {
		      setCursor(sizeVerCursor);
		      modType = MT_RESIZE_UP;
		    }
		  if ((mx >= ox) && (my >= oy) && (mx <= ox+6) && (my <= oy+6))
		    {
		      setCursor(sizeFDiagCursor);
		      modType = MT_RESIZE_LU;
		    }
		  if ((mx >= ox+ow-6) && (my >= oy) && (mx <= ox+ow) && (my <= oy+6))
		    {
		      setCursor(sizeBDiagCursor);
		      modType = MT_RESIZE_RU;
		    }
		  if ((mx >= ox) && (my >= oy+6) && (mx <= ox+6) && (my <= oy+oh-6))
		    {
		      setCursor(sizeHorCursor);
		      modType = MT_RESIZE_LF;
		    }
		  if ((mx >= ox) && (my >= oy+oh-6) && (mx <= ox+6) && (my <= oy+oh))
		    {
		      setCursor(sizeBDiagCursor);
		      modType = MT_RESIZE_LD;
		    }
		  if ((mx >= ox+ow-6) && (my >= oy+6) && (mx <= ox+ow) && (my <= oy+oh-6))
		    {
		      setCursor(sizeHorCursor);
		      modType = MT_RESIZE_RT;
		    }
		  if ((mx >= ox+ow-6) && (my >= oy+oh-6) && (mx <= ox+ow) && (my <= oy+oh))
		    {
		      setCursor(sizeFDiagCursor);
		      modType = MT_RESIZE_RD;
		    }
		  if ((mx >= ox+6) && (my >= oy+oh-6) && (mx <= ox+ow-6) && (my <= oy+oh))
		    {
		      setCursor(sizeVerCursor);
		      modType = MT_RESIZE_DN;
		    }
		}
	      else setCursor(arrowCursor);
	    }
	  else setCursor(arrowCursor);
	}
      if (mousePressed)
	{
	  if (modType == MT_MOVE) /* move object(s) */
	    {
	      mx = (mx / rastX()) * rastX();
	      my = (my / rastY()) * rastY();
	      oldMx = (oldMx / rastX()) * rastX();
	      oldMy = (oldMy / rastY()) * rastY();
	      for (i=0;i<=objList()->count()-1;i++)
		{
		  if (objList()->at(i)->isSelected)
		    {
		      objPtr = objList()->at(i);
		      oox = objPtr->ox; ooy = objPtr->oy;
		      oow = objPtr->ow; ooh = objPtr->oh;
		      objPtr->ox = objPtr->ox + (mx - oldMx);
		      objPtr->oy = objPtr->oy + (my - oldMy);
		      ox = objPtr->ox; oy = objPtr->oy;
		      ow = objPtr->ow; oh = objPtr->oh;
		      ox = ox - diffx();
		      oy = oy - diffy();
		      oox = oox - diffx();
		      ooy = ooy - diffy();
		      drawBack = true;
		      _repaint(ox,oy,ow,oh,false);
 		      if (ox > oox) _repaint(oox,oy,ox-oox,oh,false);
 		      if (oy > ooy) _repaint(ox,ooy,ow,oy-ooy,false);
 		      if (oox > ox) _repaint(ox+ow,oy,oox-ox,oh,false);
 		      if (ooy > oy) _repaint(ox,oy+oh,ow,ooy-oy,false);
 		      if ((ox > oox) && (oy > ooy)) _repaint(oox,ooy,ox-oox,oy-ooy,false);
 		      if ((ox > oox) && (oy < ooy)) _repaint(oox,oy+oh,ox-oox,ooy-oy,false);
 		      if ((ox < oox) && (oy > ooy)) _repaint(ox+ow,ooy,oox-ox,oy-ooy,false);
 		      if ((ox < oox) && (oy < ooy)) _repaint(ox+ow,oy+oh,oox-ox,ooy-oy,false);
		    }
		}
	    }
	  else
	    {	    
	      if (resizeObjNum == 0)
		{
		  resizeObjNum = getObjectAt(mx,my);
		  deSelectAllObj();
		  selectObj(resizeObjNum);
		}
	      objNum = resizeObjNum;
	      mx = (mx / rastX()) * rastX();
	      my = (my / rastY()) * rastY();
	      oldMx = (oldMx / rastX()) * rastX();
	      oldMy = (oldMy / rastY()) * rastY();
	      if (objNum > 0)
		{
		  switch (modType)
		    {
		    case MT_RESIZE_UP: /* resize top */
		      resizeObjTop(oldMy-my,getObject(objNum));
		      break;
		    case MT_RESIZE_LF: /* resize left */
		      resizeObjLeft(oldMx-mx,getObject(objNum));
		      break;
		    case MT_RESIZE_RT: /* resize right */
		      resizeObjRight(oldMx-mx,getObject(objNum));
		      break;
		    case MT_RESIZE_DN: /* resize bottom */
		      resizeObjBot(oldMy-my,getObject(objNum));
		      break;
		    case MT_RESIZE_LU: /* resize left/top */
		      {
			resizeObjLeft(oldMx-mx,getObject(objNum));
			resizeObjTop(oldMy-my,getObject(objNum));
		      } break;
		    case MT_RESIZE_LD: /* resize left/bottom */
		      {
			resizeObjLeft(oldMx-mx,getObject(objNum));
			resizeObjBot(oldMy-my,getObject(objNum));
		      } break;
		    case MT_RESIZE_RU: /* resize right/top */
		      {
			resizeObjRight(oldMx-mx,getObject(objNum));
			resizeObjTop(oldMy-my,getObject(objNum));
		      } break;
		    case MT_RESIZE_RD: /* resize right/bottom */
		      {
			resizeObjRight(oldMx-mx,getObject(objNum));
			resizeObjBot(oldMy-my,getObject(objNum));
		      } break;
		    }
		}
	    }
	}
      oldMx = e->x()+diffx();
      oldMy = e->y()+diffy();
    }
  if (!editMode)
    {
    }
}

/*==================== mouse double click ========================*/
void Page::mouseDoubleClickEvent(QMouseEvent *e)
{
  int objNum;

  modType = MT_NONE;
  mousePressed = false;
  deSelectAllObj();
  if (editMode)
    {
      objNum = getObjectAt(e->x()+diffx(),e->y()+diffy());
      if (objNum > 0 && getObject(objNum)->objType == OT_TEXT)
	{
	  objPtr = getObject(objNum);
	  if (objPtr->textObj->parentWidget() == 0)
	    {
	      editNum = objNum;
	      objPtr->textObj->recreate(this,0,QPoint(objPtr->ox - diffx(),objPtr->oy - diffy()),false);
	      //objPtr->textObj->move(objPtr->ox - diffx(),objPtr->oy - diffy());
	      objPtr->textObj->resize(objPtr->ow,objPtr->oh);
	      objPtr->textObj->setBackgroundColor(txtBackCol());
	      //objPtr->textObj->setSelectionColor(txtSelCol());
	      objPtr->textObj->show();
	      objPtr->textObj->setFocus();
	      txtPtr = objPtr->textObj;
	      txtPtr->setShowCursor(true);
	      connect(objPtr->textObj,SIGNAL(fontChanged(QFont*)),this,SLOT(toFontChanged(QFont*)));
	      connect(objPtr->textObj,SIGNAL(colorChanged(QColor*)),this,SLOT(toColorChanged(QColor*)));
	      connect(objPtr->textObj,SIGNAL(horzAlignChanged(TxtParagraph::HorzAlign)),this,SLOT(toAlignChanged(TxtParagraph::HorzAlign)));
	      //objPtr->textObj->initActive();
	      setFocusProxy(objPtr->textObj);
	      setFocusPolicy(QWidget::StrongFocus);
	      txtPtr->setCursor(ibeamCursor);
	    }
	}
      if (objNum > 0 && getObject(objNum)->objType == OT_AUTOFORM)
	{
	  editNum = objNum;
	  objPtr = getObject(objNum);
	  objPtr->graphObj->move(objPtr->ox - diffx(),objPtr->oy - diffy());
	  objPtr->graphObj->resize(objPtr->ow,objPtr->oh);
	  objPtr->graphObj->setBackgroundColor(txtBackCol());
	  objPtr->graphObj->show();
	  objPtr->graphObj->setFocus();
	  graphPtr = objPtr->graphObj;
	  _repaint(objPtr->ox - diffx(),objPtr->oy - diffy(),
		   objPtr->ow,objPtr->oh,true);
	}
    }
}

/*====================== key press event =========================*/
void Page::keyPressEvent(QKeyEvent *e)
{
  printf("Hallo\n");
}

/*========================== get object ==========================*/
int Page::getObjectAt(int x,int y)
{
  int ox,oy,ow,oh;
  
  if (!objList()->isEmpty())
    {
      for (objPtr=objList()->last();objList()->find(objPtr) >= 0;objPtr=objList()->prev())
	{
	  ox = objPtr->ox; oy = objPtr->oy;
	  ow = objPtr->ow; oh = objPtr->oh;
	  if ((x >= ox) && (y >= oy) && (x <= ox+ow) && (y <= oy+oh)) return objPtr->objNum;
	}
    }
  return 0;
}

/*========================== get object ==========================*/
PageObjects* Page::getObject(int num)
{
  if (!objList()->isEmpty())
    {
      for (objPtr=objList()->first();objPtr != 0;objPtr=objList()->next())
	if (objPtr->objNum == num) return objPtr;
    }
  return 0;
}

/*======================= select object ==========================*/
void Page::selectObj(int num)
{
  if (!objList()->isEmpty())
    {
      for (unsigned int i=1;i<=objList()->count();i++)
     	{
	  objPtr = objList()->at(i-1);
	  if (i == (unsigned int)num)
	    {
	      objPtr->isSelected = true;
	      _repaint(objPtr->ox - diffx(),objPtr->oy - diffy(),
		      objPtr->ow,objPtr->oh,true);
	      return;
	    }
	}
    }
}

/*====================== select all objects ======================*/
void Page::selectAllObj()
{
  if (!objList()->isEmpty())
    {
      for (unsigned int i=1;i<=objList()->count();i++)
	selectObj(i);
      drawBack = true;
    }
}

/*======================= deselect object ========================*/
void Page::deSelectObj(int num)
{
  unsigned int i;

  if (!objList()->isEmpty())
    {
      for (i=1;i<=objList()->count();i++)
     	{
	  objPtr = objList()->at(i-1);
	  if (i == (unsigned int)num)
	    {
	      objPtr->isSelected = false;
	      _repaint(objPtr->ox - diffx(),objPtr->oy - diffy(),
		      objPtr->ow,objPtr->oh,true);
	      return;
	    }
	}
    }
}

/*==================== deselect all objects ======================*/
void Page::deSelectAllObj()
{
  unsigned int i;

  if (!objList()->isEmpty())
    {
      for (i=1;i<=objList()->count();i++)
	deSelectObj(i);
    }
}

/*===================== resize object top ========================*/
void Page::resizeObjTop(int diff,PageObjects* obj)
{
  int ox,oy,ow,oh,ooy,ooh;
  
  if ((diff > 0) || (obj->oh > 2* rastY()))
    {
      ooy = obj->oy; ooh = obj->oh;
      obj->oy = obj->oy - diff;
      obj->oh = obj->oh + diff;
      if (obj->objType == OT_TEXT)
	{
	  //obj->textObj->breakLines(obj->ow);
	  obj->textObj->resize(obj->ow,obj->oh);
	}
      else
	obj->graphObj->resize(obj->ow,obj->oh);
      ox = obj->ox; oy = obj->oy;
      ow = obj->ow; oh = obj->oh;
      ox = ox - diffx();
      oy = oy - diffy();
      ooy = ooy - diffy();
      drawBack = true;
      _repaint(ox,oy,ow,oh,false);
      if (oh < ooh) _repaint(ox,ooy,ow,ooh-oh,false); 
    }
}

/*==================== resize object left ========================*/
void Page::resizeObjLeft(int diff,PageObjects* obj)
{
  int ox,oy,ow,oh,oox,oow;
  
  if ((diff > 0) || (obj->ow > 2* rastX()))
    {
      oox = obj->ox; oow = obj->ow;
      obj->ox = obj->ox - diff;
      obj->ow = obj->ow + diff;
      if (obj->objType == OT_TEXT)
	{
	  //obj->textObj->breakLines(obj->ow);
	  obj->textObj->resize(obj->ow,obj->oh);
	}
      else
	obj->graphObj->resize(obj->ow,obj->oh);
      ox = obj->ox; oy = obj->oy;
      ow = obj->ow; oh = obj->oh;
      ox = ox - diffx();
      oy = oy - diffy();
      oox = oox - diffx();
      drawBack = true;
      _repaint(ox,oy,ow,oh,false);
      if (ow < oow) _repaint(oox,oy,oow-ow,oh,false); 
    }
}

/*================== resize object bottom ========================*/
void Page::resizeObjBot(int diff,PageObjects* obj)
{
  int ox,oy,ow,oh,ooh;
  
  if ((diff < 0) || (obj->oh > 2* rastY()))
    {
      ooh = obj->oh;
      obj->oh = obj->oh - diff;
      if (obj->objType == OT_TEXT)
	{
	  //obj->textObj->breakLines(obj->ow);
	  obj->textObj->resize(obj->ow,obj->oh);
	}
      else
	obj->graphObj->resize(obj->ow,obj->oh);
      ox = obj->ox; oy = obj->oy;
      ow = obj->ow; oh = obj->oh;
      ox = ox - diffx();
      oy = oy - diffy();
      drawBack = true;
      _repaint(ox,oy,ow,oh,false);
      if (oh < ooh) _repaint(ox,oy+oh,ow,ooh-oh,false); 
    }
}

/*================== resize object right =========================*/
void Page::resizeObjRight(int diff,PageObjects* obj)
{
  int ox,oy,ow,oh,oow;
  
  if ((diff < 0) || (obj->ow > 2* rastX()))
    {
      oow = obj->ow;
      obj->ow = obj->ow - diff;
      if (obj->objType == OT_TEXT)
	{
	  //obj->textObj->breakLines(obj->ow);
	  obj->textObj->resize(obj->ow,obj->oh);
	}
      else
	obj->graphObj->resize(obj->ow,obj->oh);
      ox = obj->ox; oy = obj->oy;
      ow = obj->ow; oh = obj->oh;
      ox = ox - diffx();
      oy = oy - diffy();
      drawBack = true;
      _repaint(ox,oy,ow,oh,false);
      if (ow < oow) _repaint(ox+ow,oy,oow-ow,oh,false); 
    }
}

/*======================== setup menus ===========================*/
void Page::setupMenus()
{
  QString pixdir;
  QPixmap pixmap;
  pixdir = KApplication::kde_toolbardir();

  // create right button graph menu 
  graphMenu = new QPopupMenu();
  CHECK_PTR(graphMenu);
  pixmap.load(pixdir+"/editcut.xpm");
  graphMenu->insertItem(pixmap,"&Cut",this,SLOT(clipCut()));
  pixmap.load(pixdir+"/editcopy.xpm");
  graphMenu->insertItem(pixmap,"C&opy",this,SLOT(clipCopy()));
  pixmap.load(pixdir+"/editpaste.xpm");
  graphMenu->insertItem(pixmap,"&Paste",this,SLOT(clipPaste()));
  pixdir = KApplication::kde_datadir();
  pixmap.load(pixdir+"/kpresenter/toolbar/delete.xpm");
  graphMenu->insertItem(pixmap,"&Delete",this,SLOT(deleteObjs()));
  graphMenu->insertSeparator();
  pixmap.load(pixdir+"/kpresenter/toolbar/rotate.xpm");
  graphMenu->insertItem(pixmap,"&Rotate...",this,SLOT(rotateObjs()));
  graphMenu->insertSeparator();
  pixmap.load(pixdir+"/kpresenter/toolbar/style.xpm");
  graphMenu->insertItem(pixmap,"&Properties...",this,SLOT(objProperties()));
  graphMenu->setMouseTracking(true);

  // create right button picture menu
  pixdir = KApplication::kde_toolbardir();
  picMenu = new QPopupMenu();
  CHECK_PTR(picMenu);
  pixmap.load(pixdir+"/editcut.xpm");
  picMenu->insertItem(pixmap,"&Cut",this,SLOT(clipCut()));
  pixmap.load(pixdir+"/editcopy.xpm");
  picMenu->insertItem(pixmap,"C&opy",this,SLOT(clipCopy()));
  pixmap.load(pixdir+"/editpaste.xpm");
  picMenu->insertItem(pixmap,"&Paste",this,SLOT(clipPaste()));
  pixdir = KApplication::kde_datadir();
  pixmap.load(pixdir+"/kpresenter/toolbar/delete.xpm");
  picMenu->insertItem(pixmap,"&Delete",this,SLOT(deleteObjs()));
  picMenu->insertSeparator();
  pixmap.load(pixdir+"/kpresenter/toolbar/rotate.xpm");
  picMenu->insertItem(pixmap,"&Rotate...",this,SLOT(rotateObjs()));
  picMenu->insertSeparator();
  pixmap.load(pixdir+"/kpresenter/toolbar/picture.xpm");
  picMenu->insertItem(pixmap,"&Change Picture...",this,SLOT(chPic()));
  picMenu->setMouseTracking(true);

  // create right button clipart menu 
  pixdir = KApplication::kde_toolbardir();
  clipMenu = new QPopupMenu();
  CHECK_PTR(clipMenu);
  pixmap.load(pixdir+"/editcut.xpm");
  clipMenu->insertItem(pixmap,"&Cut",this,SLOT(clipCut()));
  pixmap.load(pixdir+"/editcopy.xpm");
  clipMenu->insertItem(pixmap,"C&opy",this,SLOT(clipCopy()));
  pixmap.load(pixdir+"/editpaste.xpm");
  clipMenu->insertItem(pixmap,"&Paste",this,SLOT(clipPaste()));
  pixdir = KApplication::kde_datadir();
  pixmap.load(pixdir+"/kpresenter/toolbar/delete.xpm");
  clipMenu->insertItem(pixmap,"&Delete",this,SLOT(deleteObjs()));
  clipMenu->insertSeparator();
  pixmap.load(pixdir+"/kpresenter/toolbar/rotate.xpm");
  clipMenu->insertItem(pixmap,"&Rotate...",this,SLOT(rotateObjs()));
  clipMenu->insertSeparator();
  pixmap.load(pixdir+"/kpresenter/toolbar/clipart.xpm");
  clipMenu->insertItem(pixmap,"&Change Clipart...",this,SLOT(chClip()));
  clipMenu->setMouseTracking(true);

  // create right button text menu 
  pixdir = KApplication::kde_toolbardir();
  txtMenu = new QPopupMenu();
  CHECK_PTR(txtMenu);
  pixmap.load(pixdir+"/editcut.xpm");
  txtMenu->insertItem(pixmap,"&Cut",this,SLOT(clipCut()));
  pixmap.load(pixdir+"/editcopy.xpm");
  txtMenu->insertItem(pixmap,"C&opy",this,SLOT(clipCopy()));
  pixmap.load(pixdir+"/editpaste.xpm");
  txtMenu->insertItem(pixmap,"&Paste",this,SLOT(clipPaste()));
  pixdir = KApplication::kde_datadir();
  pixmap.load(pixdir+"/kpresenter/toolbar/delete.xpm");
  txtMenu->insertItem(pixmap,"&Delete",this,SLOT(deleteObjs()));
  txtMenu->insertSeparator();
  pixmap.load(pixdir+"/kpresenter/toolbar/rotate.xpm");
  txtMenu->insertItem(pixmap,"&Rotate...",this,SLOT(rotateObjs()));
  txtMenu->setMouseTracking(true);
}

/*======================== clipboard cut =========================*/
void Page::clipCut()
{
  //if (txtPtr) txtPtr->clipCut();
}

/*======================== clipboard copy ========================*/
void Page::clipCopy()
{
  //if (txtPtr) txtPtr->clipCopy();
}

/*====================== clipboard paste =========================*/
void Page::clipPaste()
{
  //if (txtPtr) txtPtr->clipPaste();
}

/*======================= object properties  =====================*/
void Page::objProperties()
{
  view->extraPenBrush();
}

/*======================= change picture  ========================*/
void Page::chPic()
{
  if (!objList()->isEmpty())
    {
      for (unsigned int i=0;i<=objList()->count()-1;i++)
	{
	  objPtr = objList()->at(i);
	  if (objPtr->isSelected && objPtr->objType == OT_PICTURE)
	    {
	      view->changePicture(i,(const char*)objPtr->graphObj->getFileName());
	     
	      // only the first selected picture can be changed!
	      break;
	    }
	}
    }
}

/*======================= change clipart  ========================*/
void Page::chClip()
{
  if (!objList()->isEmpty())
    {
      for (unsigned int i=0;i<=objList()->count()-1;i++)
	{
	  objPtr = objList()->at(i);
	  if (objPtr->isSelected && objPtr->objType == OT_CLIPART)
	    {
	      view->changeClipart(i,(const char*)objPtr->graphObj->getFileName());
	     
	      // only the first selected clipart can be changed!
	      break;
	    }
	}
    }
}

/*======================= set text font ==========================*/
void Page::setTextFont(QFont *font)
{
  if (txtPtr)
    {
      txtPtr->setFocus();
      txtPtr->setFont(*font);
    }
}

/*======================= set text color =========================*/
void Page::setTextColor(QColor *color)
{
  if (txtPtr) 
    {
      txtPtr->setFocus();
      txtPtr->setColor(*color);
    }
}

/*===================== set text alignment =======================*/
void Page::setTextAlign(TxtParagraph::HorzAlign align)
{
  if (txtPtr)
    txtPtr->setHorzAlign(align);
}

/*====================== start screenpresentation ================*/
void Page::startScreenPresentation()
{
  currPresPage = 1;
  editMode = false;
  drawBack = true;
  repaint(true);
}

/*====================== stop screenpresentation =================*/
void Page::stopScreenPresentation()
{
  currPresPage = 1;
  editMode = true;
  drawBack = true;
  repaint(true);
}

/*===================== next page ================================*/
bool Page::pNextPage(bool manual)
{
  if (currPresPage+1 > pageNums())
    return false;

  currPresPage++;
  return true;
}

/*===================== prev page ================================*/
bool Page::pPrevPage(bool manual)
{
  if (currPresPage-1 <= 0)
    return false;

  currPresPage--;
  return true;
}

/*======================== draw back color =======================*/
void Page::drawBackColor(QColor cb,QColor ca,BCType bcType,
			 QPainter* painter,QSize size)
{
  int ncols = 4;
  int depth = QColor::numBitPlanes(),dx = 0,dy = 0;
  if (view)
    {
      dx = diffy();
      dy = diffy();
    }

  switch (bcType)
    {
    case BCT_PLAIN:
      {
  	painter->setPen(NoPen);
  	painter->setBrush(cb);
  	painter->drawRect(QRect(0,0,size.width(),size.height()));
      } break;
    case BCT_GHORZ: case BCT_GVERT:
      {
	if (ca == cb)
	  {
	    painter->setPen(NoPen);
	    painter->setBrush(cb);
	    painter->drawRect(QRect(0,0,size.width(),size.height()));
	    break;
	  }

	QPixmap pmCrop;
	QColor cRow;
	int ySize;
	int rca, gca, bca;
	int rDiff, gDiff, bDiff;
	float rat;
	uint *p;
	uint rgbRow;
		
	if (bcType == BCT_GHORZ)
	  ySize = size.height();
	else
	  ySize = size.width();
    
	pmCrop.resize(30,ySize);
	QImage image(30,ySize,32);
    
	rca = ca.red();
	gca = ca.green();
	bca = ca.blue();
	rDiff = cb.red() - ca.red();
	gDiff = cb.green() - ca.green();
	bDiff = cb.blue() - ca.blue();
    
	for (int y = ySize - 1;y > 0;y--) 
	  {
	    p = (unsigned int*) image.scanLine(ySize - y - 1);
	    rat = 1.0 * y / ySize;
	    
	    cRow.setRgb(rca + (int)(rDiff * rat),
			gca + (int)(gDiff * rat), 
			bca + (int)(bDiff * rat));
	    
	    rgbRow = cRow.rgb();
	    
	    for(int x = 0;x < 30;x++) 
	      {
		*p = rgbRow;
		p++;
	      }
	  }
	
	if (depth <= 16)
	  {
	    if(depth == 16) ncols = 32;
	    if (ncols < 2 || ncols > 256) ncols = 3;

	    QColor *dPal = new QColor[ncols];
	    for (int i = 0;i < ncols;i++) 
	      {
		dPal[i].setRgb(rca + rDiff * i / (ncols - 1),
			       gca + gDiff * i / (ncols - 1),
			       bca + bDiff * i / (ncols - 1));
	      }

	    kFSDither dither(dPal,ncols);
	    QImage dImage = dither.dither(image);
	    pmCrop.convertFromImage(dImage);
	    
	    delete [] dPal;	
		
	  } 
	else 
	  pmCrop.convertFromImage(image);
	
	int s;
	int sSize = 20;
	int sOffset = 5;
	
	if (bcType == BCT_GHORZ)
	  s = size.width() / sSize + 1;
	else
	  s = size.height() / sSize + 1;
	
	if (bcType == BCT_GHORZ)	
	  for(int i = 0;i < s;i++)
	    painter->drawPixmap(sSize*i,0,pmCrop,sOffset,0,sSize,ySize);
	else 
	  {
 	    QWMatrix matrix;
 	    matrix.translate((float)size.width(),0.0);
 	    matrix.rotate(90.0);
 	    painter->setWorldMatrix(matrix);
	    for(int i = 0;i < s;i++)
	      painter->drawPixmap(sSize*i,0,pmCrop,sOffset,0,sSize,ySize);
 	    matrix.rotate(-90.0);
 	    matrix.translate(-(float)size.width(),0.0);
 	    painter->setWorldMatrix(matrix);
	  }
      } break;
    }
}

/*======================== restore back color ====================*/
void Page::restoreBackColor(unsigned int pgNum)
{
  QPainter p;
  p.begin(pageList()->at(pgNum)->cPix);
  drawBackColor(pageList()->at(pgNum)->backColor1,pageList()->at(pgNum)->backColor2,
 		pageList()->at(pgNum)->bcType,
 		&p,QSize(pageList()->at(pgNum)->cPix->width(),pageList()->at(pgNum)->cPix->height()));
  p.end();
}


