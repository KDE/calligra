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
      graphPtr = 0;
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
      presPen.operator=(QPen(red,3,SolidLine));
      picCache.setAutoDelete(true);
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

  QPixmap pix(width(),height());

  painter.begin(&pix);
  painter.setClipping(true);
  painter.setClipRect(paintEvent->rect());
  if (drawBack) paintBackground(&painter,paintEvent->rect());
  if (!objList()->isEmpty()) paintObjects(&painter,paintEvent->rect());

  if (!editMode)
    view->presentParts(_presFakt,&painter,paintEvent->rect(),diffx(),diffy());

  painter.end();

  bitBlt(this,paintEvent->rect().x(),paintEvent->rect().y(),&pix,
	 paintEvent->rect().x(),paintEvent->rect().y(),paintEvent->rect().width(),paintEvent->rect().height());
}

/*====================== paint background ========================*/
void Page::paintBackground(QPainter *painter,QRect rect)
{
  int i,j,pw,ph;

  painter->setPen(NoPen);
  if (editMode)
    painter->setBrush(white);
  else
    painter->setBrush(black);
  painter->drawRect(0,0,width(),20);
  painter->drawRect(0,0,20,height());
  QRect r = painter->viewport();
  for (pagePtr=pageList()->first();pagePtr != 0;pagePtr=pageList()->next())
    {
      painter->setViewport(r);
      painter->resetXForm();
      r = painter->viewport();
      if ((rect.intersects(QRect(getPageSize(pagePtr->pageNum,_presFakt))) && editMode) ||
	  (!editMode && currPresPage == pageList()->at()+1))
	{
	  switch (pagePtr->backType)
	    {
	    case BT_COLOR: /* background color */ 
	      {
		painter->drawPixmap(getPageSize(pagePtr->pageNum,_presFakt).x(),getPageSize(pagePtr->pageNum,_presFakt).y(),
				    *pagePtr->cPix);
		pagePtr->pic->hide();
	      } break;
	    case BT_PIC:  /* background picture */
	      {
		if (pagePtr->backPicView == BV_CENTER)
		  painter->drawPixmap(getPageSize(pagePtr->pageNum,_presFakt).x(),getPageSize(pagePtr->pageNum,_presFakt).y(),
				      *pagePtr->cPix);
		pagePtr->pic->hide();
		switch (pagePtr->backPicView)
		  {
		  case BV_ZOOM:
		    {
		      if (!pagePtr->backPix.isNull())
			painter->drawPixmap(getPageSize(pagePtr->pageNum,_presFakt).x(),getPageSize(pagePtr->pageNum,_presFakt).y(),
					    pagePtr->backPix);
		    } break;
		  case BV_CENTER:
		    {
		      if (!pagePtr->backPix.isNull())
			{
			  if (pagePtr->backPix.width() > getPageSize(pagePtr->pageNum,_presFakt).width())
			    pagePtr->backPix.resize(getPageSize(pagePtr->pageNum,_presFakt).width(),
						    pagePtr->backPix.height());
			  if (pagePtr->backPix.height() > getPageSize(pagePtr->pageNum,_presFakt).height())
			    pagePtr->backPix.resize(pagePtr->backPix.width(),
						    getPageSize(pagePtr->pageNum,_presFakt).height());
			  painter->drawPixmap(getPageSize(pagePtr->pageNum,_presFakt).x()+
					      (getPageSize(pagePtr->pageNum,_presFakt).width()-
										 pagePtr->backPix.width())/2,
					      getPageSize(pagePtr->pageNum,_presFakt).y()+
					      (getPageSize(pagePtr->pageNum,_presFakt).height()-
					       pagePtr->backPix.height())/2,
					      pagePtr->backPix);
			}
		    } break;
		  case BV_TILED:
		    {
		      if (!pagePtr->backPix.isNull())
			{
			  for (i=0;i<=getPageSize(pagePtr->pageNum,_presFakt).height()/pagePtr->backPix.height();i++)
			    {
			      ph = -1;
			      if (getPageSize(pagePtr->pageNum,_presFakt).height()
				  -i*pagePtr->backPix.height() < pagePtr->backPix.height())
				ph = getPageSize(pagePtr->pageNum,_presFakt).height()-i*pagePtr->backPix.height();
			      for (j=0;j<=getPageSize(pagePtr->pageNum,_presFakt).width()/pagePtr->backPix.width();j++)
				{
				  pw = -1;
				  if (getPageSize(pagePtr->pageNum,_presFakt).width()
				      -j*pagePtr->backPix.width() < pagePtr->backPix.width())
				    pw = getPageSize(pagePtr->pageNum,_presFakt).width()-j*pagePtr->backPix.width();
				  painter->drawPixmap(getPageSize(pagePtr->pageNum,_presFakt).x()+j*pagePtr->backPix.width(),
						      getPageSize(pagePtr->pageNum,_presFakt).y()+i*pagePtr->backPix.height(),
						      pagePtr->backPix,0,0,pw,ph);
				}
			    }
			}
		    } break;
		  }
	      } break;
	    case BT_CLIP: /* background clipart */ 
	      {
		painter->drawPixmap(getPageSize(pagePtr->pageNum,_presFakt).x(),getPageSize(pagePtr->pageNum,_presFakt).y(),  
				    *pagePtr->cPix);
		r = painter->viewport();
		painter->setViewport(getPageSize(pagePtr->pageNum,_presFakt).x(),
				     getPageSize(pagePtr->pageNum,_presFakt).y(),
				     getPageSize(pagePtr->pageNum,_presFakt).width(),
				     getPageSize(pagePtr->pageNum,_presFakt).height());
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
      if (editMode)
	painter->setBrush(white);
      else
	painter->setBrush(black);
      painter->drawRect(0,getPageSize(pagePtr->pageNum,_presFakt).y()+getPageSize(pagePtr->pageNum,_presFakt).height(),
			width(),20);
      if (editMode)
	{
	  painter->setPen(QPen(red,1,SolidLine));
	  painter->setBrush(NoBrush);
	  painter->drawRect(getPageSize(pagePtr->pageNum,_presFakt));
	}
    }
  if (pageList() && !pageList()->isEmpty())
    {
      unsigned int num = pageList()->count();
      painter->setPen(NoPen);
      if (editMode)
	painter->setBrush(white);
      else
	painter->setBrush(black);
      painter->drawRect(0,getPageSize(num,_presFakt).y()+getPageSize(num,_presFakt).height()+1,
			width(),height());
      painter->drawRect(getPageSize(num,_presFakt).x()+getPageSize(num,_presFakt).width(),0,
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
	  (!editMode && getPageOfObj(objPtr->objNum,_presFakt) == currPresPage &&
	   objPtr->presNum <= currPresStep))
	{     
	  switch (objPtr->objType)
	    {
	    case OT_PICTURE: /* pciture */
	      {
		painter->drawPixmap(objPtr->ox - diffx(),objPtr->oy - diffy(),
				    objPtr->graphObj->getPix());
		//0,0,objPtr->ow,objPtr->oh);
	      } break;
	    case OT_TEXT: /* text */
	      {
		if (!editMode && currPresStep == objPtr->presNum && !goingBack)
		  {
		    switch (objPtr->effect2)
		      {
		      case EF2T_PARA:
			objPtr->objPic = objPtr->textObj->getPic(objPtr->ox - diffx(),objPtr->oy - diffy(),
								 objPtr->ow,objPtr->oh,!editMode,0,subPresStep);
			break;
		      default:
			objPtr->objPic = objPtr->textObj->getPic(objPtr->ox - diffx(),objPtr->oy - diffy(),
								 objPtr->ow,objPtr->oh,!editMode);
		      }
		  }
		else
		  objPtr->objPic = objPtr->textObj->getPic(objPtr->ox - diffx(),objPtr->oy - diffy(),
							   objPtr->ow,objPtr->oh,!editMode);
 		painter->translate((float)objPtr->ox - (float)diffx(),(float)objPtr->oy - (float)diffy());
 		objPtr->objPic->play(painter);
 		painter->resetXForm();
 		painter->setClipping(false);
 		painter->setClipping(true);
 		painter->setClipRect(rect);
	      } break;
	    default:
	      {
		if (objPtr->objType != OT_CLIPART)
		  painter->translate((float)objPtr->ox - (float)diffx(),(float)objPtr->oy - (float)diffy());

		if (objPtr->objType == OT_CLIPART)
		  {
		    r = painter->viewport();
		    painter->setViewport(objPtr->ox - diffx(),objPtr->oy - diffy(),
					 objPtr->ow,objPtr->oh);
		  }

		objPtr->objPic = objPtr->graphObj->getPic(objPtr->ox - diffx(),objPtr->oy - diffy(),objPtr->ow,objPtr->oh);

		objPtr->objPic->play(painter);
	      
		painter->translate((float)objPtr->ox - (float)diffx(),(float)objPtr->oy - (float)diffy());

		if (objPtr->objType == OT_CLIPART)
		  painter->setViewport(r);

		painter->resetXForm();
	      } break;
	    }

	  /* draw selection, if selected */
	  if (objPtr->isSelected)
	    {
	      RasterOp rastOp = painter->rasterOp();
	      painter->setRasterOp(NotROP);
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
	      painter->setRasterOp(rastOp);
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
	  if (objPtr->objType == OT_TEXT)
	    {
	      objPtr->textObj->recreate(0,0,QPoint(objPtr->ox - diffx(),objPtr->oy - diffy()),false);
	      objPtr->textObj->clearFocus();
	      objPtr->textObj->hide();
	      disconnect(objPtr->textObj,SIGNAL(fontChanged(QFont*)),this,SLOT(toFontChanged(QFont*)));
	      disconnect(objPtr->textObj,SIGNAL(colorChanged(QColor*)),this,SLOT(toColorChanged(QColor*)));
	      disconnect(objPtr->textObj,SIGNAL(horzAlignChanged(TxtParagraph::HorzAlign)),this,SLOT(toAlignChanged(TxtParagraph::HorzAlign)));
	      objPtr->textObj->setShowCursor(false);
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
	  pnt.operator=(QCursor::pos());
	  presMenu->popup(pnt);
	} 
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
  //_repaint(false);
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
  if (!editMode && drawMode)
    {
      QPainter p;
      p.begin(this);
      p.setPen(presPen);
      p.drawLine(oldMx,oldMy,e->x(),e->y());
      oldMx = e->x();
      oldMy = e->y();
      p.end();
    }
  if (!editMode && !drawMode && !presMenu->isVisible())
    setCursor(blankCursor);
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
	      setFocusProxy(objPtr->textObj);
	      setFocusPolicy(QWidget::StrongFocus);
	      objPtr->textObj->setFocus();
	      objNum = getObjectAt(e->x()+diffx(),e->y()+diffy());
	      objPtr = getObject(objNum);
	      objPtr->textObj->setShowCursor(true);
	      connect(objPtr->textObj,SIGNAL(fontChanged(QFont*)),this,SLOT(toFontChanged(QFont*)));
	      connect(objPtr->textObj,SIGNAL(colorChanged(QColor*)),this,SLOT(toColorChanged(QColor*)));
	      connect(objPtr->textObj,SIGNAL(horzAlignChanged(TxtParagraph::HorzAlign)),this,SLOT(toAlignChanged(TxtParagraph::HorzAlign)));
	      //objPtr->textObj->initActive();
	      objPtr->textObj->setCursor(ibeamCursor);
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
	      drawBack = true;
	      _repaint(objPtr->ox - diffx(),objPtr->oy - diffy(),
		      objPtr->ow,objPtr->oh,false);
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
	  if (i == (unsigned int)num && objPtr->isSelected)
	    {
	      objPtr->isSelected = false;
	      drawBack = true;
	      _repaint(objPtr->ox - diffx(),objPtr->oy - diffy(),
		      objPtr->ow,objPtr->oh,false);
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
  graphMenu->insertItem(pixmap,i18n("&Cut"),this,SLOT(clipCut()));
  pixmap.load(pixdir+"/editcopy.xpm");
  graphMenu->insertItem(pixmap,i18n("C&opy"),this,SLOT(clipCopy()));
  pixmap.load(pixdir+"/editpaste.xpm");
  graphMenu->insertItem(pixmap,i18n("&Paste"),this,SLOT(clipPaste()));
  pixdir = KApplication::kde_datadir();
  pixmap.load(pixdir+"/kpresenter/toolbar/delete.xpm");
  graphMenu->insertItem(pixmap,i18n("&Delete"),this,SLOT(deleteObjs()));
  graphMenu->insertSeparator();
  pixmap.load(pixdir+"/kpresenter/toolbar/rotate.xpm");
  graphMenu->insertItem(pixmap,i18n("&Rotate..."),this,SLOT(rotateObjs()));
  graphMenu->insertSeparator();
  pixmap.load(pixdir+"/kpresenter/toolbar/style.xpm");
  graphMenu->insertItem(pixmap,i18n("&Properties..."),this,SLOT(objProperties()));
  graphMenu->insertSeparator();
  pixmap.load(pixdir+"/kpresenter/toolbar/effect.xpm");
  graphMenu->insertItem(pixmap,i18n("&Assign effect..."),this,SLOT(assignEffect()));
  graphMenu->setMouseTracking(true);

  // create right button picture menu
  pixdir = KApplication::kde_toolbardir();
  picMenu = new QPopupMenu();
  CHECK_PTR(picMenu);
  pixmap.load(pixdir+"/editcut.xpm");
  picMenu->insertItem(pixmap,i18n("&Cut"),this,SLOT(clipCut()));
  pixmap.load(pixdir+"/editcopy.xpm");
  picMenu->insertItem(pixmap,i18n("C&opy"),this,SLOT(clipCopy()));
  pixmap.load(pixdir+"/editpaste.xpm");
  picMenu->insertItem(pixmap,i18n("&Paste"),this,SLOT(clipPaste()));
  pixdir = KApplication::kde_datadir();
  pixmap.load(pixdir+"/kpresenter/toolbar/delete.xpm");
  picMenu->insertItem(pixmap,i18n("&Delete"),this,SLOT(deleteObjs()));
  picMenu->insertSeparator();
  pixmap.load(pixdir+"/kpresenter/toolbar/rotate.xpm");
  picMenu->insertItem(pixmap,i18n("&Rotate..."),this,SLOT(rotateObjs()));
  picMenu->insertSeparator();
  pixmap.load(pixdir+"/kpresenter/toolbar/picture.xpm");
  picMenu->insertItem(pixmap,i18n("&Change Picture..."),this,SLOT(chPic()));
  picMenu->insertSeparator();
  pixmap.load(pixdir+"/kpresenter/toolbar/effect.xpm");
  picMenu->insertItem(pixmap,i18n("&Assign effect..."),this,SLOT(assignEffect()));
  picMenu->setMouseTracking(true);

  // create right button clipart menu 
  pixdir = KApplication::kde_toolbardir();
  clipMenu = new QPopupMenu();
  CHECK_PTR(clipMenu);
  pixmap.load(pixdir+"/editcut.xpm");
  clipMenu->insertItem(pixmap,i18n("&Cut"),this,SLOT(clipCut()));
  pixmap.load(pixdir+"/editcopy.xpm");
  clipMenu->insertItem(pixmap,i18n("C&opy"),this,SLOT(clipCopy()));
  pixmap.load(pixdir+"/editpaste.xpm");
  clipMenu->insertItem(pixmap,i18n("&Paste"),this,SLOT(clipPaste()));
  pixdir = KApplication::kde_datadir();
  pixmap.load(pixdir+"/kpresenter/toolbar/delete.xpm");
  clipMenu->insertItem(pixmap,i18n("&Delete"),this,SLOT(deleteObjs()));
  clipMenu->insertSeparator();
  pixmap.load(pixdir+"/kpresenter/toolbar/rotate.xpm");
  clipMenu->insertItem(pixmap,i18n("&Rotate..."),this,SLOT(rotateObjs()));
  clipMenu->insertSeparator();
  pixmap.load(pixdir+"/kpresenter/toolbar/clipart.xpm");
  clipMenu->insertItem(pixmap,i18n("&Change Clipart..."),this,SLOT(chClip()));
  clipMenu->insertSeparator();
  pixmap.load(pixdir+"/kpresenter/toolbar/effect.xpm");
  clipMenu->insertItem(pixmap,i18n("&Assign effect..."),this,SLOT(assignEffect()));
  clipMenu->setMouseTracking(true);

  // create right button text menu 
  pixdir = KApplication::kde_toolbardir();
  txtMenu = new QPopupMenu();
  CHECK_PTR(txtMenu);
  pixmap.load(pixdir+"/editcut.xpm");
  txtMenu->insertItem(pixmap,i18n("&Cut"),this,SLOT(clipCut()));
  pixmap.load(pixdir+"/editcopy.xpm");
  txtMenu->insertItem(pixmap,i18n("C&opy"),this,SLOT(clipCopy()));
  pixmap.load(pixdir+"/editpaste.xpm");
  txtMenu->insertItem(pixmap,i18n("&Paste"),this,SLOT(clipPaste()));
  pixdir = KApplication::kde_datadir();
  pixmap.load(pixdir+"/kpresenter/toolbar/delete.xpm");
  txtMenu->insertItem(pixmap,i18n("&Delete"),this,SLOT(deleteObjs()));
  txtMenu->insertSeparator();
  pixmap.load(pixdir+"/kpresenter/toolbar/rotate.xpm");
  txtMenu->insertItem(pixmap,i18n("&Rotate..."),this,SLOT(rotateObjs()));
  txtMenu->insertSeparator();
  pixmap.load(pixdir+"/kpresenter/toolbar/effect.xpm");
  txtMenu->insertItem(pixmap,i18n("&Assign effect..."),this,SLOT(assignEffect()));
  txtMenu->setMouseTracking(true);

  // create right button presentation menu 
  presMenu = new QPopupMenu();
  CHECK_PTR(presMenu);
  presMenu->setCheckable(true);
  PM_SM = presMenu->insertItem(i18n("&Switching mode"),this,SLOT(switchingMode()));
  PM_DM = presMenu->insertItem(i18n("&Drawing mode"),this,SLOT(drawingMode()));
  presMenu->insertSeparator();
  pixdir = KApplication::kde_datadir();
  pixmap.load(pixdir+"/kpresenter/toolbar/pen.xpm");
  presMenu->insertItem(pixmap,i18n("&Choose Pen..."),this,SLOT(choosePen()));
  presMenu->setItemChecked(PM_SM,true);
  presMenu->setItemChecked(PM_DM,false);
  presMenu->setMouseTracking(true);
}

/*======================== clipboard cut =========================*/
void Page::clipCut()
{
  if (editNum != 0 && getObject(editNum)->objType == OT_TEXT) ;//txtPtr->clipCut();
  else view->editCut();
}

/*======================== clipboard copy ========================*/
void Page::clipCopy()
{
  if (editNum != 0 && getObject(editNum)->objType == OT_TEXT) ;// txtPtr->clipCopy();
  else view->editCopy();
}

/*====================== clipboard paste =========================*/
void Page::clipPaste()
{
  if (editNum != 0 && getObject(editNum)->objType == OT_TEXT) ;//txtPtr->clipPaste();
  else view->editPaste();
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

/*==================== choose pen for presentations ==============*/
void Page::choosePen()
{
}

/*======================= set text font ==========================*/
void Page::setTextFont(QFont *font)
{
  if (editNum != 0 && getObject(editNum)->objType == OT_TEXT)
    {
      getObject(editNum)->textObj->setFocus();
      getObject(editNum)->textObj->setFont(*font);
    }
}

/*======================= set text color =========================*/
void Page::setTextColor(QColor *color)
{
  if (editNum != 0 && getObject(editNum)->objType == OT_TEXT) 
    {
      getObject(editNum)->textObj->setFocus();
      getObject(editNum)->textObj->setColor(*color);
    }
}

/*===================== set text alignment =======================*/
void Page::setTextAlign(TxtParagraph::HorzAlign align)
{
  if (editNum != 0 && getObject(editNum)->objType == OT_TEXT)
    getObject(editNum)->textObj->setHorzAlign(align);
}

/*====================== start screenpresentation ================*/
void Page::startScreenPresentation(bool zoom)
{
  if (editNum > 0)
    {
      objPtr = getObject(editNum);
      editNum = 0;
      if (objPtr->objType == OT_TEXT)
	{
	  objPtr->textObj->recreate(0,0,QPoint(objPtr->ox - diffx(),objPtr->oy - diffy()),false);
	  objPtr->textObj->clearFocus();
	  objPtr->textObj->hide();
	  disconnect(objPtr->textObj,SIGNAL(fontChanged(QFont*)),this,SLOT(toFontChanged(QFont*)));
	  disconnect(objPtr->textObj,SIGNAL(colorChanged(QColor*)),this,SLOT(toColorChanged(QColor*)));
	  disconnect(objPtr->textObj,SIGNAL(horzAlignChanged(TxtParagraph::HorzAlign)),this,SLOT(toAlignChanged(TxtParagraph::HorzAlign)));
	  objPtr->textObj->setShowCursor(false);
	  setFocusProxy(0);
	  setFocusPolicy(QWidget::NoFocus);
	}
    }

  unsigned int i,pgNum;

  if (zoom)
    {
      float _presFaktW = (float)width() / (float)getPageSize(1).width() > 0.0 ? 
	(float)width() / (float)getPageSize(0).width() : 1.0;
      float _presFaktH = (float)height() / (float)getPageSize(1).height() > 0.0 ? 
	(float)height() / (float)getPageSize(1).height() : 1.0;
      _presFakt = min(_presFaktW,_presFaktH);
    }
  else _presFakt = 1.0;

  for (i = 0;i < pageList()->count();i++)
    {
      pagePtr = pageList()->at(i);
      if (pagePtr->backType == BT_PIC && pagePtr->backPicView == BV_ZOOM)
	{
	  QWMatrix m;
 	  m.scale((float)getPageSize(pagePtr->pageNum,_presFakt).width()/pagePtr->obackPix.width(),
 		  (float)getPageSize(pagePtr->pageNum,_presFakt).height()/pagePtr->obackPix.height());
 	  pagePtr->backPix.operator=(pagePtr->obackPix.xForm(m));
	}
      if (pagePtr->backType == BT_PIC && pagePtr->backPicView == BV_CENTER)
	{
	  QWMatrix m;
 	  m.scale(_presFakt,_presFakt);
 	  pagePtr->backPix.operator=(pagePtr->obackPix.xForm(m));
	}
      restoreBackColor(i);
    }

  for (i = 0;i < objList()->count();i++)
    {
      objPtr = objList()->at(i);
      objPtr->oox = objPtr->ox;
      objPtr->ooy = objPtr->oy;
      objPtr->oow = objPtr->ow;
      objPtr->ooh = objPtr->oh;
      objPtr->ox = (int)((float)objPtr->ox * _presFakt);
      objPtr->oy = (int)((float)objPtr->oy * _presFakt);
      pgNum = getPageOfObj(objPtr->objNum,_presFakt);
      objPtr = objList()->at(i);

      objPtr->ow = (int)((float)objPtr->ow * _presFakt);
      objPtr->oh = (int)((float)objPtr->oh * _presFakt);

      pgNum = getPageOfObj(objPtr->objNum,_presFakt);
      objPtr = objList()->at(i);
      objPtr->oy = objPtr->ooy; 
      objPtr->oy -= getPageSize(pgNum).y() + diffy();
      objPtr->oy = (int)((float)objPtr->oy * _presFakt);
      objPtr->oy += getPageSize(pgNum,_presFakt).y() + diffy();

      // correct calculated zoom-values
      if (objPtr->ox + objPtr->ow > getPageSize(1,_presFakt).x()+diffx() + getPageSize(1,_presFakt).width())
	objPtr->ow -= ((objPtr->ox + objPtr->ow) - (getPageSize(1,_presFakt).x()+diffx() + getPageSize(1,_presFakt).width()));

      // *************** TODO: correction of height ****************

      if (objPtr->objType != OT_TEXT)
	objPtr->graphObj->setGeometry(objPtr->ox,objPtr->oy,objPtr->ow,objPtr->oh);
      else
	{
	  objPtr->textObj->setGeometry(objPtr->ox,objPtr->oy,objPtr->ow,objPtr->oh);
	  objPtr->textObj->zoom(_presFakt);
	}
    }

  currPresPage = 1;
  editMode = false;
  drawBack = true;
  drawMode = false;
  presStepList = view->KPresenterDoc()->reorderPage(1,diffx(),diffy(),_presFakt);
  currPresStep = (int)presStepList.first();
  subPresStep = 0;
  repaint(true);
  setFocusPolicy(QWidget::StrongFocus);
  setFocus();
  setCursor(blankCursor);
}

/*====================== stop screenpresentation =================*/
void Page::stopScreenPresentation()
{
  unsigned int i;

  for (i = 0;i < objList()->count();i++)
    {
      objPtr = objList()->at(i);
      objPtr->ox = objPtr->oox;
      objPtr->oy = objPtr->ooy;
      objPtr->ow = objPtr->oow;
      objPtr->oh = objPtr->ooh;
      if (objPtr->objType != OT_TEXT)
	objPtr->graphObj->setGeometry(objPtr->ox,objPtr->oy,objPtr->ow,objPtr->oh);
      else
	{
	  objPtr->textObj->setGeometry(objPtr->ox,objPtr->oy,objPtr->ow,objPtr->oh);
	  objPtr->textObj->zoomOrig();
	}
    }

  _presFakt = 1.0;

  for (i = 0;i < pageList()->count();i++)
    {
      pagePtr = pageList()->at(i);
      if (pagePtr->backType == BT_PIC && pagePtr->backPicView == BV_ZOOM)
	{
	  QWMatrix m;
 	  m.scale((float)getPageSize(pagePtr->pageNum,_presFakt).width()/pagePtr->obackPix.width(),
 		  (float)getPageSize(pagePtr->pageNum,_presFakt).height()/pagePtr->obackPix.height());
 	  pagePtr->backPix.operator=(pagePtr->obackPix.xForm(m));
	}
      if (pagePtr->backType == BT_PIC && pagePtr->backPicView == BV_CENTER)
	pagePtr->backPix.operator=(pagePtr->obackPix);
      pagePtr->cPix->resize(getPageSize(pagePtr->pageNum,_presFakt).width(),
			    getPageSize(pagePtr->pageNum,_presFakt).height());
      restoreBackColor(i);
    }

  goingBack = false;
  currPresPage = 1;
  editMode = true;
  drawBack = true;
  repaint(true);
  setCursor(arrowCursor);
  picCache.clear();
}

/*========================== next ================================*/
bool Page::pNext(bool)
{
  bool addSubPres = false;
  bool clearSubPres = false;

  goingBack = false;

  if ((int*)currPresStep < presStepList.last())
    {

      unsigned int i;
      
      for (i = 0;i < objList()->count();i++)
	{
	  objPtr = objList()->at(i);
	  if (getPageOfObj(objPtr->objNum,_presFakt) == currPresPage && objPtr->presNum == currPresStep
	      && objPtr->objType == OT_TEXT && objPtr->effect2 != EF2_NONE)
	    {
	      if (subPresStep < objPtr->textObj->paragraphs()-1)
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
      
      presStepList.find((int*)currPresStep);
      currPresStep = (int)presStepList.next();
    
      if (currPresStep == 0)
	{
	  QPainter p;
	  p.begin(this);
	  paintBackground(&p,QRect(0,0,kapp->desktop()->width(),kapp->desktop()->height()));
	  p.end();  
	}

      //if (currPresStep > 0)
	doObjEffects();
//       else
// 	{
// 	  drawBack = false;
// 	  repaint(drawBack);
// 	  drawBack = true;
// 	  return false;
// 	}
    }
  else
    {
      if (currPresPage+1 > pageNums())
	{

	  for (unsigned int i = 0;i < objList()->count();i++)
	    {
	      objPtr = objList()->at(i);
	      if (getPageOfObj(objPtr->objNum,_presFakt) == currPresPage && objPtr->presNum == currPresStep
		  && objPtr->objType == OT_TEXT && objPtr->effect2 != EF2_NONE)
		{
		  if (subPresStep < objPtr->textObj->paragraphs()-1)
		    {
		      if (subPresStep < objPtr->textObj->paragraphs()-1)
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

	  presStepList = view->KPresenterDoc()->reorderPage(currPresPage,diffx(),diffy(),_presFakt);
	  currPresStep = (int)presStepList.last();
	  doObjEffects();
	  return false;
	}

      for (unsigned int i = 0;i < objList()->count();i++)
	{
	  objPtr = objList()->at(i);
	  if (getPageOfObj(objPtr->objNum,_presFakt) == currPresPage && objPtr->presNum == currPresStep
	      && objPtr->objType == OT_TEXT && objPtr->effect2 != EF2_NONE)
	    {
	      if (subPresStep < objPtr->textObj->paragraphs()-1)
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
      currPresStep = (int)presStepList.first();
      
      QPixmap _pix2(QApplication::desktop()->width(),QApplication::desktop()->height());
      drawPageInPix(_pix2,view->getDiffY() + view->KPresenterDoc()->getPageSize(1,0,0,_presFakt).height()+10);
      
      changePages(_pix1,_pix2,pageList()->at(currPresPage-2)->pageEffect);
      
      drawBack = true;
      return true;
    }
  return false;
}

/*====================== previous ================================*/
bool Page::pPrev(bool manual)
{
  goingBack = true;
  subPresStep = 0;

  if ((int*)currPresStep > presStepList.first())
    {
      presStepList.find((int*)currPresStep);
      currPresStep = (int)presStepList.prev();
      drawBack = true;
      repaint(drawBack);
      drawBack = true;
      return false;
    }
  else
    {
      if (currPresPage-1 <= 0)
	{
	  presStepList = view->KPresenterDoc()->reorderPage(currPresPage,diffx(),diffy(),_presFakt);
	  currPresStep = (int)presStepList.first();
	  drawBack = false;
	  repaint(drawBack);
	  drawBack = true;
	  return false;
	}
      currPresPage--;
      presStepList = view->KPresenterDoc()->reorderPage(currPresPage,diffx(),diffy(),_presFakt);
      currPresStep = (int)presStepList.last();
      drawBack = true;
      return true;
    }
}

/*======================== can we assign an effect ? =============*/
bool Page::canAssignEffect(int &pgNum,int &objNum)
{
  bool ret = false; 

  pgNum = -1; objNum = -1;

  for (unsigned int i = 0;i < objList()->count();i++)
    {
      objPtr = objList()->at(i);
      if (objPtr->isSelected)
	{
	  if (ret) return false;
	  ret = true;
	  objNum = objPtr->objNum;
	  pgNum = getPageOfObj(objNum);
	}
    }
  return ret;
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
  QPainter *p = new QPainter();
  unsigned int i,sameBackNum = 0;
  bool sameBack = false;
  bool _delete = true;

  for (i = 0;i < pgNum;i++)
    {
      if (pageList()->at(pgNum)->backColor1 == pageList()->at(i)->backColor1 &&
	  pageList()->at(pgNum)->backColor2 == pageList()->at(i)->backColor2 &&
	  pageList()->at(pgNum)->bcType == pageList()->at(i)->bcType)
	{
	  sameBack = true;
	  sameBackNum = i;
	  break;
	}
    }

  if (_presFakt > 1.0 && !sameBack)
    {
      for (i = 0;i < pageList()->count();i++)
	{
	  if (i == pgNum) continue;
	  _delete = pageList()->at(pgNum)->cPix != pageList()->at(i)->cPix;
	  if (!_delete) break;
	}

      if (_delete)
	delete pageList()->at(pgNum)->cPix;

      pageList()->at(pgNum)->cPix = new QPixmap(getPageSize(pgNum+1,_presFakt).width(),
						getPageSize(pgNum+1,_presFakt).height());

      pageList()->at(pgNum)->hasSameCPix = false;
    }
  
  if (!sameBack)
    {
      if (_presFakt == 1.0)
	{
	  for (i = 0;i < pageList()->count();i++)
	    {
	      if (i == pgNum) continue;
	      _delete = pageList()->at(pgNum)->cPix != pageList()->at(i)->cPix;
	      if (!_delete) break;
	    }
	  
	  if (_delete)
	    delete pageList()->at(pgNum)->cPix;
	
	  pageList()->at(pgNum)->cPix = new QPixmap(getPageSize(pgNum+1).width(),
						    getPageSize(pgNum+1).height());
	}

      p->begin(pageList()->at(pgNum)->cPix);
      drawBackColor(pageList()->at(pgNum)->backColor1,pageList()->at(pgNum)->backColor2,
		    pageList()->at(pgNum)->bcType,
		    p,QSize((int)((float)pageList()->at(pgNum)->cPix->width()),
			    (int)((float)pageList()->at(pgNum)->cPix->height())));
      p->end();
      delete p;
      pageList()->at(pgNum)->hasSameCPix = false;
      return;
    }

  if (sameBack) 
    {
      pageList()->at(pgNum)->cPix = pageList()->at(sameBackNum)->cPix;
      pageList()->at(sameBackNum)->hasSameCPix = true;
      pageList()->at(pgNum)->hasSameCPix = true;
    }
}

/*==================== draw a page in a pixmap ===================*/ 
void Page::drawPageInPix(QPixmap &_pix,int __diffy)
{
  int _yOffset = view->getDiffY();
  view->setDiffY(__diffy);

  QPainter p;
  p.begin(&_pix);

  paintBackground(&p,_pix.rect());
  if (!objList()->isEmpty()) paintObjects(&p,_pix.rect());
  
  p.end();

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
	_steps = (int)(50000.0 / (float)kapp->desktop()->height());
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
	_steps = (int)(50000.0 / (float)kapp->desktop()->width());
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
	_steps = (int)(50000.0 / (float)kapp->desktop()->width());
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
	_steps = (int)(50000.0 / (float)kapp->desktop()->height());
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
	_steps = (int)(50000.0 / (float)kapp->desktop()->height());
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
	_steps = (int)(50000.0 / (float)kapp->desktop()->height());
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
    }
}

/*======================= do object effects ======================*/
void Page::doObjEffects()
{
  QPixmap screen_orig(kapp->desktop()->width(),kapp->desktop()->height());
  QList<PageObjects> _objList;
  unsigned int i;
  QTime _time;
  int _step = 0,_steps1 = 0,_steps2 = 0,x_pos1 = 0,y_pos1 = 0;
  int x_pos2 = kapp->desktop()->width(),y_pos2 = kapp->desktop()->height(),_step_width,_step_height;
  int w_pos1 = 0,h_pos1;
  bool effects = false;

  bitBlt(&screen_orig,0,0,this,0,0,kapp->desktop()->width(),kapp->desktop()->height());
  QPixmap *screen = new QPixmap(screen_orig);

  for (i = 0;i < objList()->count();i++)
    {
      objPtr = objList()->at(i);
      if (getPageOfObj(objPtr->objNum,_presFakt) == currPresPage && objPtr->presNum == currPresStep)
	{
	  if (objPtr->effect != EF_NONE)
	    {
	      _objList.append(objPtr);
	      switch (objPtr->effect)
		{
		case EF_COME_LEFT:
		  x_pos1 = max(x_pos1,objPtr->ox - diffx() + objPtr->ow);
		  break;
		case EF_COME_TOP:
		  y_pos1 = max(y_pos1,objPtr->oy - diffy() + objPtr->oh);
		  break;
		case EF_COME_RIGHT:
		  x_pos2 = min(x_pos2,objPtr->ox - diffx());
		  break;
		case EF_COME_BOTTOM:
		  y_pos2 = min(y_pos2,objPtr->oy - diffy());
		  break;
		case EF_COME_LEFT_TOP:
		  {
		    x_pos1 = max(x_pos1,objPtr->ox - diffx() + objPtr->ow);
		    y_pos1 = max(y_pos1,objPtr->oy - diffy() + objPtr->oh);
		  } break;
		case EF_COME_LEFT_BOTTOM:
		  {
		    x_pos1 = max(x_pos1,objPtr->ox - diffx() + objPtr->ow);
		    y_pos2 = min(y_pos2,objPtr->oy - diffy());
		  } break;
		case EF_COME_RIGHT_TOP:
		  {
		    x_pos2 = min(x_pos2,objPtr->ox - diffx());
		    y_pos1 = max(y_pos1,objPtr->oy - diffy() + objPtr->oh);
		  } break;
		case EF_COME_RIGHT_BOTTOM:
		  {
		    x_pos2 = min(x_pos2,objPtr->ox - diffx());
		    y_pos2 = min(y_pos2,objPtr->oy - diffy());
		  } break;
		case EF_WIPE_LEFT:
		  x_pos1 = max(x_pos1,objPtr->ow);
		  break;
		case EF_WIPE_RIGHT:
		  x_pos1 = max(x_pos1,objPtr->ow);
		  break;
		case EF_WIPE_TOP:
		  y_pos1 = max(y_pos1,objPtr->oh);
		  break;
		case EF_WIPE_BOTTOM:
		  y_pos1 = max(y_pos1,objPtr->oh);
		  break;
		}
	      effects = true;
	    }
	}
    }
  
  if (effects)
    {
      _step_width = (int)(((float)20 * kapp->desktop()->width()) / 1000.0);
      _step_height = (int)(((float)20 * kapp->desktop()->height()) / 1000.0);
      _steps1 = x_pos1 > y_pos1 ? x_pos1 / _step_width : y_pos1 / _step_height;
      _steps2 = kapp->desktop()->width() - x_pos2 > kapp->desktop()->height() - y_pos2 ?
	(kapp->desktop()->width() - x_pos2) / _step_width : (kapp->desktop()->height() - y_pos2) / _step_height;
      _time.start();

      bool nothingHappens = false;
      for (;;)
	{
	  kapp->processEvents();
	  if (nothingHappens) break; // || _step >= _steps1 && _step >= _steps2) break;
	  
	  if (_time.elapsed() >= 1)
	    {
	      nothingHappens = true;
	      _step++;
	      
	      for (i = 0;i < _objList.count();i++)
		{
		  objPtr = _objList.at(i);
		  int _w =  kapp->desktop()->width() - (objPtr->ox - diffx());
		  int _h =  kapp->desktop()->height() - (objPtr->oy - diffy());
		  
		  switch (objPtr->effect)
		    {
		    case EF_COME_LEFT:
		      {
			if (subPresStep == 0 || subPresStep != 0 && objPtr->objType == OT_TEXT && objPtr->effect2 == EF2T_PARA)
			  { 
			    x_pos1 = _step_width * _step < objPtr->ox - diffx() + objPtr->ow ? 
			      objPtr->ox - diffx() + objPtr->ow - _step_width * _step : 0;
			    y_pos1 = 0;
			    drawObject(objPtr,screen,-x_pos1,y_pos1,0,0,0,0);
			    if (x_pos1 != 0) nothingHappens = false;
			  }
		      } break;
		    case EF_COME_TOP:
		      {
			if (subPresStep == 0 || subPresStep != 0 && objPtr->objType == OT_TEXT && objPtr->effect2 == EF2T_PARA)
			  { 
			    y_pos1 = _step_height * _step < objPtr->oy - diffy() + objPtr->oh ?
			      objPtr->oy - diffy() + objPtr->oh - _step_height * _step : 0;
			    x_pos1 = 0;
			    drawObject(objPtr,screen,x_pos1,-y_pos1,0,0,0,0);
			    if (y_pos1 != 0) nothingHappens = false;
			  }
		      } break;
		    case EF_COME_RIGHT:
		      {
			if (subPresStep == 0 || subPresStep != 0 && objPtr->objType == OT_TEXT && objPtr->effect2 == EF2T_PARA)
			  { 
			    x_pos2 = _w - (_step_width * _step) + (objPtr->ox - diffx()) > objPtr->ox - diffx() ?
			      _w - (_step_width * _step) : 0;
			    y_pos2 = 0;
			    drawObject(objPtr,screen,x_pos2,y_pos2,0,0,0,0);
			    if (x_pos2 != 0) nothingHappens = false;
			  }
		      } break;
		    case EF_COME_BOTTOM:
		      {
			if (subPresStep == 0 || subPresStep != 0 && objPtr->objType == OT_TEXT && objPtr->effect2 == EF2T_PARA)
			  { 
			    y_pos2 = _h - (_step_height * _step) + (objPtr->oy - diffy()) > objPtr->oy - diffy() ?
			      _h - (_step_height * _step) : 0;
			    x_pos2 = 0;
			    drawObject(objPtr,screen,x_pos2,y_pos2,0,0,0,0);
			    if (y_pos2 != 0) nothingHappens = false;
			  }
		      } break;
		    case EF_COME_LEFT_TOP:
		      {
			if (subPresStep == 0 || subPresStep != 0 && objPtr->objType == OT_TEXT && objPtr->effect2 == EF2T_PARA)
			  { 
			    x_pos1 = _step_width * _step < objPtr->ox - diffx() + objPtr->ow ? 
			      objPtr->ox - diffx() + objPtr->ow - _step_width * _step : 0;
			    y_pos1 = _step_height * _step < objPtr->oy - diffy() + objPtr->oh ?
			      objPtr->oy - diffy() + objPtr->oh - _step_height * _step : 0;
			    drawObject(objPtr,screen,-x_pos1,-y_pos1,0,0,0,0);
			    if (x_pos1 != 0 || y_pos1 != 0) nothingHappens = false;
			  }
		      } break;
		    case EF_COME_LEFT_BOTTOM:
		      {
			if (subPresStep == 0 || subPresStep != 0 && objPtr->objType == OT_TEXT && objPtr->effect2 == EF2T_PARA)
			  { 
			    x_pos1 = _step_width * _step < objPtr->ox - diffx() + objPtr->ow ? 
			      objPtr->ox - diffx() + objPtr->ow - _step_width * _step : 0;
			    y_pos2 = _h - (_step_height * _step) + (objPtr->oy - diffy()) > objPtr->oy - diffy() ?
			      _h - (_step_height * _step) : 0;
			    drawObject(objPtr,screen,-x_pos1,y_pos2,0,0,0,0);
			    if (x_pos1 != 0 || y_pos2 != 0) nothingHappens = false;
			  }
		      } break;
		    case EF_COME_RIGHT_TOP:
		      {
			if (subPresStep == 0 || subPresStep != 0 && objPtr->objType == OT_TEXT && objPtr->effect2 == EF2T_PARA)
			  { 
			    x_pos2 = _w - (_step_width * _step) + (objPtr->ox - diffx()) > objPtr->ox - diffx() ?
			      _w - (_step_width * _step) : 0;
			    y_pos1 = _step_height * _step < objPtr->oy - diffy() + objPtr->oh ?
			      objPtr->oy - diffy() + objPtr->oh - _step_height * _step : 0;
			    drawObject(objPtr,screen,x_pos2,-y_pos1,0,0,0,0);
			    if (x_pos2 != 0 || y_pos1 != 0) nothingHappens = false;
			  }
		      } break;
		    case EF_COME_RIGHT_BOTTOM:
		      {
			if (subPresStep == 0 || subPresStep != 0 && objPtr->objType == OT_TEXT && objPtr->effect2 == EF2T_PARA)
			  { 
			    x_pos2 = _w - (_step_width * _step) + (objPtr->ox - diffx()) > objPtr->ox - diffx() ?
			      _w - (_step_width * _step) : 0;
			    y_pos2 = _h - (_step_height * _step) + (objPtr->oy - diffy()) > objPtr->oy - diffy() ?
			      _h - (_step_height * _step) : 0;
			    drawObject(objPtr,screen,x_pos2,y_pos2,0,0,0,0);
			    if (x_pos2 != 0 || y_pos2 != 0) nothingHappens = false;
			  }
		      } break;
		    case EF_WIPE_LEFT:
		      {
			if (subPresStep == 0 || subPresStep != 0 && objPtr->objType == OT_TEXT && objPtr->effect2 == EF2T_PARA)
			  { 
			    w_pos1 = _step_width * (_steps1 - _step) > 0 ? _step_width * (_steps1 - _step) : 0;
			    drawObject(objPtr,screen,0,0,w_pos1,0,0,0);
 			    if (w_pos1 != 0) nothingHappens = false;
			  }
		      } break;
		    case EF_WIPE_RIGHT:
		      {
			if (subPresStep == 0 || subPresStep != 0 && objPtr->objType == OT_TEXT && objPtr->effect2 == EF2T_PARA)
			  { 
			    w_pos1 = _step_width * (_steps1 - _step) > 0 ? _step_width * (_steps1 - _step) : 0;
			    x_pos1 = w_pos1;
			    drawObject(objPtr,screen,0,0,w_pos1,0,x_pos1,0);
 			    if (w_pos1 != 0) nothingHappens = false;
			  }
		      } break;
		    case EF_WIPE_TOP:
		      {
			if (subPresStep == 0 || subPresStep != 0 && objPtr->objType == OT_TEXT && objPtr->effect2 == EF2T_PARA)
			  { 
			    h_pos1 = _step_height * (_steps1 - _step) > 0 ? _step_height * (_steps1 - _step) : 0;
			    drawObject(objPtr,screen,0,0,0,h_pos1,0,0);
 			    if (h_pos1 != 0) nothingHappens = false;
			  }
		      } break;
		    case EF_WIPE_BOTTOM:
		      {
			if (subPresStep == 0 || subPresStep != 0 && objPtr->objType == OT_TEXT && objPtr->effect2 == EF2T_PARA)
			  { 
			    h_pos1 = _step_height * (_steps1 - _step) > 0 ? _step_height * (_steps1 - _step) : 0;
			    y_pos1 = h_pos1;
			    drawObject(objPtr,screen,0,0,0,h_pos1,0,y_pos1);
 			    if (h_pos1 != 0) nothingHappens = false;
			  }
		      } break;
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
      paintObjects(&p,QRect(0,0,kapp->desktop()->width(),kapp->desktop()->height()));
      //view->presentParts(_presFakt,&p,QRect(0,0,kapp->desktop()->width(),kapp->desktop()->height()),diffx(),diffy());
      p.end();  
    }
  else
    {
      QPainter p;
      p.begin(screen);
      paintObjects(&p,QRect(0,0,kapp->desktop()->width(),kapp->desktop()->height()));
      //view->presentParts(_presFakt,&p,QRect(0,0,kapp->desktop()->width(),kapp->desktop()->height()),diffx(),diffy());
      p.end();
      bitBlt(this,0,0,screen);
    }

  delete screen;
}

/*======================= draw object ============================*/
void Page::drawObject(PageObjects *_objPtr,QPixmap *screen,int _x,int _y,int _w,int _h,int _cx,int _cy)
{
  QPainter p;
  p.begin(screen);
  QRect r = p.viewport();
  int num;
  PicCache *pc;
  bool _clip = !(_w != 0 || _h != 0);

  switch (_objPtr->objType)
    {
    case OT_PICTURE:
      {
	if (_w != 0 || _h != 0)
	  {
	    p.setClipping(true);
	    p.setClipRect(_objPtr->ox - diffx() + _cx,_objPtr->oy - diffy() + _cy,_objPtr->ow - _w,_objPtr->oh - _h);
	  }

	p.drawPixmap(_objPtr->ox - diffx() + _x,_objPtr->oy - diffy() + _y,
		     _objPtr->graphObj->getPix());

	if (_w != 0 || _h != 0)
	  p.setClipping(false);
      } break;
    case OT_TEXT:
      {
	if (!editMode && currPresStep == _objPtr->presNum && !goingBack)
	  {
	    switch (_objPtr->effect2)
	      {
	      case EF2T_PARA:
		{
		  num = isInPicCache(_objPtr->objNum,subPresStep);
		  if (num != -1)
		    {
		      _objPtr->objPic = &picCache.at(num)->pic;
		      //debug("take from cache");
		    }
		  else
		    {
		      _objPtr->objPic = _objPtr->textObj->getPic(0,0,kapp->desktop()->width(),kapp->desktop()->height(),
								 !editMode,subPresStep,subPresStep,_clip);
		      pc = new PicCache;
		      pc->pic.setData(_objPtr->objPic->data(),_objPtr->objPic->size());
		      pc->num = _objPtr->objNum;
		      pc->subPresStep = subPresStep;
		      picCache.append(pc);
		    }
		} break;
	      default:
		{
		  num = isInPicCache(_objPtr->objNum,-1);
		  if (num != -1)
		    {
		      _objPtr->objPic = &picCache.at(num)->pic;
		      //debug("take from cache");
		    }
		  else
		    {
		      _objPtr->objPic = _objPtr->textObj->getPic(0,0,kapp->desktop()->width(),kapp->desktop()->height(),!editMode,
								 -1,-1,_clip);
		      pc = new PicCache;
		      pc->pic.setData(_objPtr->objPic->data(),_objPtr->objPic->size());
		      pc->num = _objPtr->objNum;
		      pc->subPresStep = -1;
		      picCache.append(pc);
		    }
		}
	      }
	  }
	else
	  _objPtr->objPic = _objPtr->textObj->getPic(_objPtr->ox - diffx(),_objPtr->oy - diffy(),
						   _objPtr->ow,_objPtr->oh,!editMode,-1,-1,_clip);
	if (_w != 0 || _h != 0)
	  {
	    p.setClipping(true);
	    p.setClipRect(_objPtr->ox - diffx() + _cx,_objPtr->oy - diffy() + _cy,_objPtr->ow - _w,_objPtr->oh - _h);
	  }

	p.translate((float)_objPtr->ox - (float)diffx() + (float)_x,(float)_objPtr->oy - (float)diffy() + (float)_y);
	
	_objPtr->objPic->play(&p);

	if (_w != 0 || _h != 0)
	  p.setClipping(false);
	
	p.resetXForm();
      } break;
    default:
      {
	if (_w != 0 || _h != 0)
	  {
	    p.setClipping(true);
	    p.setClipRect(_objPtr->ox - diffx() + _cx,_objPtr->oy - diffy() + _cy,_objPtr->ow - _w,_objPtr->oh - _h);
	  }

	if (_objPtr->objType != OT_CLIPART)
	  p.translate((float)_objPtr->ox - (float)diffx() + (float)_x,(float)_objPtr->oy - (float)diffy() + (float)_y);
	
	if (_objPtr->objType == OT_CLIPART)
	  {
	    r = p.viewport();
	    p.setViewport(_objPtr->ox - diffx() + _x,_objPtr->oy - diffy() + _y,
			  _objPtr->ow,_objPtr->oh);
	  }
	
	_objPtr->objPic = _objPtr->graphObj->getPic(_objPtr->ox - diffx(),_objPtr->oy - diffy(),_objPtr->ow,_objPtr->oh);
	
	_objPtr->objPic->play(&p);
	
	if (_w != 0 || _h != 0)
	  p.setClipping(false);

	if (_objPtr->objType == OT_CLIPART)
	  p.setViewport(r);
	
	p.resetXForm();
      } break;
    }

  p.end();
}

/*========== check, if QPicture is in pic-cache ==================*/
int Page::isInPicCache(int num,int _subPresStep)
{
  PicCache *_p;

  if (!picCache.isEmpty())
    {
      for (_p = picCache.first();_p != 0;_p = picCache.next())
	if (_p->num == num && _p->subPresStep == _subPresStep) return picCache.at();
    }

  return -1;
}

