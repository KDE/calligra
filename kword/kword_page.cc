/******************************************************************/
/* KWord - (c) by Reginald Stadlbauer and Torben Weis 1997-1998   */
/* Version: 0.0.1                                                 */
/* Author: Reginald Stadlbauer, Torben Weis                       */
/* E-Mail: reggie@kde.org, weis@kde.org                           */
/* Homepage: http://boch35.kfunigraz.ac.at/~rs                    */
/* needs c++ library Qt (http://www.troll.no)                     */
/* written for KDE (http://www.kde.org)                           */
/* needs mico (http://diamant.vsb.cs.uni-frankfurt.de/~mico/)     */
/* needs OpenParts and Kom (weis@kde.org)                         */
/* License: GNU GPL                                               */
/******************************************************************/
/* Module: Page                                                   */
/******************************************************************/

#include <kfiledialog.h>
#include "kword_doc.h"
#include "kword_page.h"
#include "kword_page.moc"
#include "kword_view.h"
#include "footnote.h"
#include "clipbrd_dnd.h"

/******************************************************************/
/* Class: KWPage                                                  */
/******************************************************************/

/*================================================================*/
KWPage::KWPage(QWidget *parent,KWordDocument *_doc,KWordGUI *_gui)
  : QWidget(parent,""), buffer(width(),height()), format(_doc)
{
  setFocusPolicy(QWidget::StrongFocus);

  editNum = -1;
  recalcingText = false;
  maybeDrag = false;

  setBackgroundColor(white);
  buffer.fill(white);
  doc = _doc;
  gui = _gui;

  xOffset = 0;
  yOffset = 0;

  markerIsVisible = true;
  paint_directly = false;
  has_to_copy = false;
  redrawAllWhileScrolling = false;

  if (doc) calcVisiblePages();

  QPainter painter;
  painter.begin(&buffer);
  fc = new KWFormatContext(doc,1);
  fc->init(doc->getFirstParag(0),painter);
  fc->cursorGotoLine(0,painter);
  painter.end();
  drawBuffer();

  setCursor(ibeamCursor);
  mousePressed = false;
  setMouseTracking(true);

  inKeyEvent = false;
  recalcAll = false;

  recalcWholeText();
  mouseMode = MM_EDIT;

  setupMenus();
  inKeyEvent = true;
  setMouseMode(MM_EDIT);
  inKeyEvent = false;

  oldMx = oldMy = 0;
  deleteMovingRect = true;

  hiliteFrameSet = -1;

  frameDia = 0;
  pixmap_name = "";
  doRaster = true;

  currFindParag = 0L;
  currFindPos = 0;
  currFindFS = 0;
  currFindLen = 0;

  painter.begin(this);
  fc->init(fc->getParag(),painter,true,true);
  painter.end();
  recalcWholeText(false);
  recalcCursor(false,0,fc);

  editMode = EM_NONE;
  setKeyCompression(true);

  doc->setSelStart(*fc);
  doc->setSelEnd(*fc);
  doc->setSelection(false);

  curTable = 0L;

  setAcceptDrops(true);
}

unsigned int KWPage::ptLeftBorder() { return doc->getPTLeftBorder(); }
unsigned int KWPage::ptRightBorder() { return doc->getPTRightBorder(); }
unsigned int KWPage::ptTopBorder() { return doc->getPTTopBorder(); }
unsigned int KWPage::ptBottomBorder() { return doc->getPTBottomBorder(); }
unsigned int KWPage::ptPaperWidth() { return doc->getPTPaperWidth(); }
unsigned int KWPage::ptPaperHeight() { return doc->getPTPaperHeight(); }
unsigned int KWPage::ptColumnWidth() { return doc->getPTColumnWidth(); }
unsigned int KWPage::ptColumnSpacing() { return doc->getPTColumnSpacing(); }

/*================================================================*/
void KWPage::init()
{
  if (fc->getParag())
    {	
      gui->getView()->updateStyle(fc->getParag()->getParagLayout()->getName(),false);
      gui->getView()->setFormat(*((KWFormat*)fc),true,false);
      gui->getView()->setFlow(fc->getParag()->getParagLayout()->getFlow());
      gui->getView()->setLineSpacing(fc->getParag()->getParagLayout()->getLineSpacing().pt());
      gui->getView()->setParagBorders(fc->getParag()->getParagLayout()->getLeftBorder(),
				      fc->getParag()->getParagLayout()->getRightBorder(),
				      fc->getParag()->getParagLayout()->getTopBorder(),
				      fc->getParag()->getParagLayout()->getBottomBorder());
      setRulerFirstIndent(gui->getHorzRuler(),fc->getParag()->getParagLayout()->getFirstLineLeftIndent());
      setRulerLeftIndent(gui->getHorzRuler(),fc->getParag()->getParagLayout()->getLeftIndent());
      gui->getHorzRuler()->setFrameStart(doc->getFrameSet(fc->getFrameSet() - 1)->getFrame(fc->getFrame() - 1)->x());
      gui->getHorzRuler()->setTabList(fc->getParag()->getParagLayout()->getTabList());
      gui->getVertRuler()->setOffset(0,-getVertRulerPos());
    }
}

/*================================================================*/
void KWPage::mouseMoveEvent(QMouseEvent *e)
{
  if (maybeDrag)
    {
      maybeDrag = false;
      mousePressed = false;
      mouseMoved = false;
      startDrag();
      return;
    }

  if (!hasFocus())
    gui->getView()->sendFocusEvent();

  mouseMoved = true;

  if (mousePressed)
    {
      switch (mouseMode)
	{
	case MM_EDIT:
	  {
	    unsigned int mx = e->x() + xOffset;
	    unsigned int my = e->y() + yOffset;
	
	    QPainter _painter;
	    _painter.begin(this);
	
	    int frameset = doc->getFrameSet(mx,my);
	
	    // only if we are in the _same_ frameset as before!!
	    if (frameset != -1 && frameset == static_cast<int>(fc->getFrameSet()) - 1 && doc->getFrameSet(frameset)->getFrameType() == FT_TEXT)
	      {
		doc->drawMarker(*fc,&_painter,xOffset,yOffset);
		
		fc->setFrameSet(frameset + 1);
		
		fc->cursorGotoPixelLine(mx,my,_painter);
		fc->cursorGotoPixelInLine(mx,my,_painter);
		
		_painter.end();
		
		if (fc->getPTPos() != doc->getSelStart()->getPTPos() ||
		    fc->getPTY() != doc->getSelStart()->getPTY())
		  {
		    _painter.begin(this);
		    if (doc->has_selection())
		      doc->drawSelection(_painter,xOffset,yOffset);
		    doc->setSelEnd(*fc);
		    doc->setSelection(false);
		    _painter.end();
		
		    scrollToCursor(*fc);
		
		    doc->setSelection(true);
		    _painter.begin(this);
		    doc->drawMarker(*fc,&_painter,xOffset,yOffset);
		    doc->drawSelection(_painter,xOffset,yOffset);
		    _painter.end();
		
		    if (doc->getProcessingType() == KWordDocument::DTP)
		      setRuler2Frame(fc->getFrameSet() - 1,fc->getFrame() - 1);
		
		    gui->getVertRuler()->setOffset(0,-getVertRulerPos());
		
		    if (fc->getParag())
		      {	
			setRulerFirstIndent(gui->getHorzRuler(),fc->getParag()->getParagLayout()->getFirstLineLeftIndent());
			setRulerLeftIndent(gui->getHorzRuler(),fc->getParag()->getParagLayout()->getLeftIndent());
		      }
		  }
		else
		  {
		    _painter.begin(this);
		    doc->drawMarker(*fc,&_painter,xOffset,yOffset);
		    _painter.end();
		  }
	      }
	    else _painter.end();
	  } break;
	case MM_EDIT_FRAME:
	  {
	    int mx = e->x() + xOffset;
	    int my = e->y() + yOffset;

	    mx = (mx / doc->getRastX()) * doc->getRastX();
	    my = (my / doc->getRastY()) * doc->getRastY();

	    switch (cursor().shape())
	      {
	      case SizeAllCursor:
		{
		  if (mx != oldMy || my != oldMy)
		    {
		      QList<KWGroupManager> undos,updates;
		      undos.setAutoDelete(false);
		      updates.setAutoDelete(false);

		      QPainter p;
		      p.begin(this);
		      p.setRasterOp(NotROP);
		      p.setPen(black);
		      p.setBrush(NoBrush);
		
		      KWFrameSet *frameset;
		      KWFrame *frame;
		      for (unsigned int i = 0;i < doc->getNumFrameSets();i++)
			{
			  frameset = doc->getFrameSet(i);
			  if (doc->getProcessingType() == KWordDocument::WP && i == 0 ||
			      frameset->getFrameType() == FT_TEXT && frameset->getFrameInfo() != FI_BODY) continue;

			  for (unsigned int j = 0;j < frameset->getNumFrames();j++)
			    {
			      frame = frameset->getFrame(j);
			      if (frame->isSelected())
				{
				  if (frameset->getGroupManager())
				    {
				      if (updates.findRef(frameset->getGroupManager()) == -1)
					updates.append(frameset->getGroupManager());
				    }
				  else
				    {
				      if (deleteMovingRect)
					p.drawRect(frame->x() - xOffset,frame->y() - yOffset,frame->width(),frame->height());
				      frame->moveBy(mx - oldMx,my - oldMy);
				      if (frame->x() < 0 ||
					  frame->y() < getPageOfRect(KRect(frame->x(),frame->y(),frame->width(),frame->height())) *
					  static_cast<int>(ptPaperHeight()) ||
					  frame->right() > static_cast<int>(ptPaperWidth()) ||
					  frame->bottom() > (getPageOfRect(KRect(frame->x(),frame->y(),frame->width(),frame->height())) + 1) *
					  static_cast<int>(ptPaperHeight()))
					frame->moveBy(oldMx - mx,oldMy - my);
				      p.drawRect(frame->x() - xOffset,frame->y() - yOffset,frame->width(),frame->height());
				    }				
				}
			    }
			}

		      if (!updates.isEmpty())
			{
			  for (unsigned int i = 0;i < updates.count();i++)
			    {
			      KWGroupManager *grpMgr = updates.at(i);
			      for (unsigned k = 0;k < grpMgr->getNumCells();k++)
				{
				  frame = grpMgr->getCell(k)->frameSet->getFrame(0);
				  if (deleteMovingRect)
				    p.drawRect(frame->x() - xOffset,frame->y() - yOffset,frame->width(),frame->height());
				  frame->moveBy(mx - oldMx,my - oldMy);
				  if (frame->x() < 0 || frame->right() > static_cast<int>(ptPaperWidth()) || frame->y() < 0)
				    {
				      if (undos.findRef(grpMgr) == -1)
					undos.append(grpMgr);
				    }
				  p.drawRect(frame->x() - xOffset,frame->y() - yOffset,frame->width(),frame->height());
				}
			    }
			}

		      if (!undos.isEmpty())
			{
			  for (unsigned int i = 0;i < undos.count();i++)
			    {
			      undos.at(i)->drawAllRects(p,xOffset,yOffset);
			      undos.at(i)->moveBy(oldMx - mx,oldMy - my);
			      undos.at(i)->drawAllRects(p,xOffset,yOffset);
			    }			
			}
		
		      p.end();
		    }
		} break;
	      case SizeVerCursor:
		{
		  int frameset = 0;
		  KWFrame *frame = doc->getFirstSelectedFrame(frameset);
		  if (frameset < 1 && doc->getProcessingType() == KWordDocument::WP) break;
		
		  QPainter p;
		  p.begin(this);
		  p.setRasterOp(NotROP);
		  p.setPen(black);
		  p.setBrush(NoBrush);

		  if (deleteMovingRect)
		    p.drawRect(!doc->getFrameSet(frameset)->getGroupManager() ? frame->x() - xOffset :
			       doc->getFrameSet(frameset)->getGroupManager()->getBoundingRect().x() - xOffset,
			       frame->y() - yOffset,
			       !doc->getFrameSet(frameset)->getGroupManager() ? frame->width() :
			       doc->getFrameSet(frameset)->getGroupManager()->getBoundingRect().width(),frame->height());
		
		  if (my < frame->top() + frame->height() / 2)
		    {
		      if (isAHeader(doc->getFrameSet(frameset)->getFrameInfo())) break;
		      frame->setHeight(frame->height() + (oldMy - my));
		      frame->moveBy(0,my - oldMy);
		      if (frame->x() < 0 ||
			  frame->y() < getPageOfRect(KRect(frame->x(),frame->y(),frame->width(),frame->height())) *
			  static_cast<int>(ptPaperHeight()) ||
			  frame->right() > static_cast<int>(ptPaperWidth()) ||
			  frame->bottom() > (getPageOfRect(KRect(frame->x(),frame->y(),frame->width(),frame->height())) + 1) *
			  static_cast<int>(ptPaperHeight()) ||
			  frame->height() < 2 * doc->getRastY() || frame->width() < 2 * doc->getRastX())
			{
			  frame->setHeight(frame->height() - (oldMy - my));
			  frame->moveBy(0,-my + oldMy);
			}
		    }
		  else
		    {
		      if (isAFooter(doc->getFrameSet(frameset)->getFrameInfo())) break;
		      frame->setHeight(frame->height() + (my - oldMy));
		      if (frame->x() < 0 ||
			  frame->y() < getPageOfRect(KRect(frame->x(),frame->y(),frame->width(),frame->height())) *
			  static_cast<int>(ptPaperHeight()) ||
			  frame->right() > static_cast<int>(ptPaperWidth()) ||
			  frame->bottom() > (getPageOfRect(KRect(frame->x(),frame->y(),frame->width(),frame->height())) + 1) *
			  static_cast<int>(ptPaperHeight()) ||
			  frame->height() < 2 * doc->getRastY() || frame->width() < 2 * doc->getRastX())
			frame->setHeight(frame->height() - (my - oldMy));
		    }
		  p.drawRect(!doc->getFrameSet(frameset)->getGroupManager() ? frame->x() - xOffset :
			     doc->getFrameSet(frameset)->getGroupManager()->getBoundingRect().x() - xOffset,
			     frame->y() - yOffset,
			     !doc->getFrameSet(frameset)->getGroupManager() ? frame->width() :
			     doc->getFrameSet(frameset)->getGroupManager()->getBoundingRect().width(),frame->height());
		  p.end();		
		} break;
	      case SizeHorCursor:
		{
		  int frameset = 0;
		  KWFrame *frame = doc->getFirstSelectedFrame(frameset);
		  if (doc->getFrameSet(frameset)->getFrameInfo() != FI_BODY) break;
		  if (frameset < 1 && doc->getProcessingType() != KWordDocument::DTP) break;

		  QPainter p;
		  p.begin(this);
		  p.setRasterOp(NotROP);
		  p.setPen(black);
		  p.setBrush(NoBrush);

		  if (deleteMovingRect)
		    p.drawRect(frame->x() - xOffset,
			       !doc->getFrameSet(frameset)->getGroupManager() ? frame->y() - yOffset :
			       doc->getFrameSet(frameset)->getGroupManager()->getBoundingRect().y() - yOffset,frame->width(),
			       !doc->getFrameSet(frameset)->getGroupManager() ? frame->height() :
			       doc->getFrameSet(frameset)->getGroupManager()->getBoundingRect().height());
		
		  if (mx < frame->left() + frame->width() / 2)
		    {
		      frame->setWidth(frame->width() + (oldMx - mx));
		      frame->moveBy(mx - oldMx,0);
		      if (frame->x() < 0 ||
			  frame->y() < getPageOfRect(KRect(frame->x(),frame->y(),frame->width(),frame->height())) *
			  static_cast<int>(ptPaperHeight()) ||
			  frame->right() > static_cast<int>(ptPaperWidth()) ||
			  frame->bottom() > (getPageOfRect(KRect(frame->x(),frame->y(),frame->width(),frame->height())) + 1) *
			  static_cast<int>(ptPaperHeight()) ||
			  frame->height() < 2 * doc->getRastY() || frame->width() < 2 * doc->getRastX())
			{
			  frame->setWidth(frame->width() - (oldMx - mx));
			  frame->moveBy(-mx + oldMx,0);
			}
		    }
		  else
		    {
		      frame->setWidth(frame->width() + (mx - oldMx));
		      if (frame->x() < 0 ||
			  frame->y() < getPageOfRect(KRect(frame->x(),frame->y(),frame->width(),frame->height())) *
			  static_cast<int>(ptPaperHeight()) ||
			  frame->right() > static_cast<int>(ptPaperWidth()) ||
			  frame->bottom() > (getPageOfRect(KRect(frame->x(),frame->y(),frame->width(),frame->height())) + 1) *
			  static_cast<int>(ptPaperHeight()) ||
			  frame->height() < 2 * doc->getRastY() || frame->width() < 2 * doc->getRastX())
			frame->setWidth(frame->width() - (mx - oldMx));
		    }
		  p.drawRect(frame->x() - xOffset,
			     !doc->getFrameSet(frameset)->getGroupManager() ? frame->y() - yOffset :
			     doc->getFrameSet(frameset)->getGroupManager()->getBoundingRect().y() - yOffset,frame->width(),
			     !doc->getFrameSet(frameset)->getGroupManager() ? frame->height() :
			     doc->getFrameSet(frameset)->getGroupManager()->getBoundingRect().height());
		  p.end();		
		} break;
	      case SizeFDiagCursor:
		{
		  int frameset = 0;
		  KWFrame *frame = doc->getFirstSelectedFrame(frameset);
		  if (doc->getFrameSet(frameset)->getFrameInfo() != FI_BODY) break;
		  if (frameset < 1 && doc->getProcessingType() == KWordDocument::WP) break;

		  QPainter p;
		  p.begin(this);
		  p.setRasterOp(NotROP);
		  p.setPen(black);
		  p.setBrush(NoBrush);

		  if (deleteMovingRect)
		    p.drawRect(frame->x() - xOffset,frame->y() - yOffset,frame->width(),frame->height());
		
		  if (mx < frame->left() + frame->width() / 2)
		    {
		      frame->setWidth(frame->width() + (oldMx - mx));
		      frame->setHeight(frame->height() + (oldMy - my));
		      frame->moveBy(mx - oldMx,my - oldMy);
		      if (frame->x() < 0 ||
			  frame->y() < getPageOfRect(KRect(frame->x(),frame->y(),frame->width(),frame->height())) *
			  static_cast<int>(ptPaperHeight()) ||
			  frame->right() > static_cast<int>(ptPaperWidth()) ||
			  frame->bottom() > (getPageOfRect(KRect(frame->x(),frame->y(),frame->width(),frame->height())) + 1) *
			  static_cast<int>(ptPaperHeight()) ||
			  frame->height() < 2 * doc->getRastY() || frame->width() < 2 * doc->getRastX())
			{
			  frame->setWidth(frame->width() - (oldMx - mx));
			  frame->setHeight(frame->height() - (oldMy - my));
			  frame->moveBy(-mx + oldMx,-my + oldMy);
			}
		    }
		  else
		    {
		      frame->setWidth(frame->width() + (mx - oldMx));
		      frame->setHeight(frame->height() + (my - oldMy));
		      if (frame->x() < 0 ||
			  frame->y() < getPageOfRect(KRect(frame->x(),frame->y(),frame->width(),frame->height())) *
			  static_cast<int>(ptPaperHeight()) ||
			  frame->right() > static_cast<int>(ptPaperWidth()) ||
			  frame->bottom() > (getPageOfRect(KRect(frame->x(),frame->y(),frame->width(),frame->height())) + 1) *
			  static_cast<int>(ptPaperHeight()) ||
			  frame->height() < 2 * doc->getRastY() || frame->width() < 2 * doc->getRastX())
			{			
			  frame->setWidth(frame->width() - (mx - oldMx));
			  frame->setHeight(frame->height() - (my - oldMy));
			}
		    }

		  p.drawRect(frame->x() - xOffset,frame->y() - yOffset,frame->width(),frame->height());
		  p.end();		
		} break;
	      case SizeBDiagCursor:
		{
		  int frameset = 0;
		  KWFrame *frame = doc->getFirstSelectedFrame(frameset);
		  if (doc->getFrameSet(frameset)->getFrameInfo() != FI_BODY) break;
		  if (frameset < 1 && doc->getProcessingType() == KWordDocument::WP) break;

		  QPainter p;
		  p.begin(this);
		  p.setRasterOp(NotROP);
		  p.setPen(black);
		  p.setBrush(NoBrush);

		  if (deleteMovingRect)
		    p.drawRect(frame->x() - xOffset,frame->y() - yOffset,frame->width(),frame->height());
		
		  if (mx > frame->left() + frame->width() / 2)
		    {
		      frame->setWidth(frame->width() + (mx - oldMx));
		      frame->setHeight(frame->height() + (oldMy - my));
		      frame->moveBy(0,my - oldMy);
		      if (frame->x() < 0 ||
			  frame->y() < getPageOfRect(KRect(frame->x(),frame->y(),frame->width(),frame->height())) *
			  static_cast<int>(ptPaperHeight()) ||
			  frame->right() > static_cast<int>(ptPaperWidth()) ||
			  frame->bottom() > (getPageOfRect(KRect(frame->x(),frame->y(),frame->width(),frame->height())) + 1) *
			  static_cast<int>(ptPaperHeight()) ||
			  frame->height() < 2 * doc->getRastY() || frame->width() < 2 * doc->getRastX())
			{
			  frame->setWidth(frame->width() - (mx - oldMx));
			  frame->setHeight(frame->height() - (oldMy - my));
			  frame->moveBy(0,-my + oldMy);
			}
		    }
		  else
		    {
		      frame->setWidth(frame->width() + (oldMx - mx));
		      frame->setHeight(frame->height() + (my - oldMy));
		      frame->moveBy(mx - oldMx,0);
		      if (frame->x() < 0 ||
			  frame->y() < getPageOfRect(KRect(frame->x(),frame->y(),frame->width(),frame->height())) *
			  static_cast<int>(ptPaperHeight()) ||
			  frame->right() > static_cast<int>(ptPaperWidth()) ||
			  frame->bottom() > (getPageOfRect(KRect(frame->x(),frame->y(),frame->width(),frame->height())) + 1) *
			  static_cast<int>(ptPaperHeight()) ||
			  frame->height() < 2 * doc->getRastY() || frame->width() < 2 * doc->getRastX())
			{			
			  frame->setWidth(frame->width() - (oldMx - mx));
			  frame->setHeight(frame->height() - (my - oldMy));
			  frame->moveBy(-mx + oldMx,0);
			}
		    }

		  p.drawRect(frame->x() - xOffset,frame->y() - yOffset,frame->width(),frame->height());
		  p.end();		
		} break;
	      default: break;
	      }
	    deleteMovingRect = true;
	    oldMx = mx; oldMy = my;
	  } break;
	case MM_CREATE_TEXT: case MM_CREATE_PIX: case MM_CREATE_PART: case MM_CREATE_TABLE: case MM_CREATE_FORMULA: case MM_CREATE_KSPREAD_TABLE:
	  {
	    int mx = e->x();
	    int my = e->y();
	    if (doRaster)
	      {
		mx = (mx / doc->getRastX()) * doc->getRastX();
		my = (my / doc->getRastY()) * doc->getRastY();
	      }

	    QPainter p;
	    p.begin(this);
	    p.setRasterOp(NotROP);
	    p.setPen(black);
	    p.setBrush(NoBrush);
	
	    if (deleteMovingRect)
	      p.drawRect(insRect);
	    insRect.setWidth(insRect.width() + mx - oldMx);
	    insRect.setHeight(insRect.height() + my - oldMy);

	    if (insRect.normalize().x() + static_cast<int>(xOffset) < 0 || insRect.normalize().y() + static_cast<int>(yOffset) <
		getPageOfRect(KRect(insRect.normalize().x() + static_cast<int>(xOffset),insRect.normalize().y() + static_cast<int>(yOffset),
				    insRect.normalize().width(),insRect.normalize().height())) * static_cast<int>(ptPaperHeight()) ||
		insRect.normalize().right() + static_cast<int>(xOffset) > static_cast<int>(ptPaperWidth()) ||
		insRect.normalize().bottom() + static_cast<int>(yOffset) > (getPageOfRect(KRect(insRect.normalize().x() +
												static_cast<int>(xOffset),
									      insRect.normalize().y() + static_cast<int>(yOffset),
									      insRect.normalize().width(),insRect.normalize().height())) + 1) *
		static_cast<int>(ptPaperHeight()))
	      {
		insRect.setWidth(insRect.width() - (mx - oldMx));
		insRect.setHeight(insRect.height() - (my - oldMy));
	      }
	
	    p.drawRect(insRect);
	    p.end();
	    oldMx = mx; oldMy = my;
	    deleteMovingRect = true;
	  } break;
	default: break;
	}
    }
  else
    {
      unsigned int mx = e->x() + xOffset;
      unsigned int my = e->y() + yOffset;

      switch (mouseMode)
	{
	case MM_EDIT_FRAME:
	  setCursor(doc->getMouseCursor(mx,my));
	  break;
	default: break;
	}
    }
  doRaster = true;
}

/*================================================================*/
void KWPage::mousePressEvent(QMouseEvent *e)
{
  maybeDrag = false;

  if (gui->getView()) gui->getView()->sendFocusEvent();

  if (editNum != -1)
    {
      if (doc->getFrameSet(editNum)->getFrameType() == FT_PART)
	dynamic_cast<KWPartFrameSet*>(doc->getFrameSet(editNum))->deactivate();
    }

  oldMx = e->x() + xOffset;
  oldMy = e->y() + yOffset;

  switch (e->button())
    {
    case LeftButton:
      {
	mousePressed = true;
	
	unsigned int mx = e->x() + xOffset;
	unsigned int my = e->y() + yOffset;
	
	QPainter _painter;
	_painter.begin(this);
	
	if (doc->has_selection() && mouseMode != MM_EDIT)
	  {
	    doc->drawSelection(_painter,xOffset,yOffset);
	    doc->setSelection(false);
	  }

	_painter.end();
	
	switch (mouseMode)
	  {
	  case MM_EDIT:
	    {
	      _painter.begin(this);
	      doc->drawMarker(*fc,&_painter,xOffset,yOffset);
	      markerIsVisible = false;
	
	      int frameset = doc->getFrameSet(mx,my);
	
	      selectedFrameSet = selectedFrame = -1;
	      if (frameset != -1 && doc->getFrameSet(frameset)->getFrameType() == FT_TEXT)
		{
		  fc->setFrameSet(frameset + 1);
		
		  fc->cursorGotoPixelLine(mx,my,_painter);
		  fc->cursorGotoPixelInLine(mx,my,_painter);
		
		  _painter.end();
		  scrollToCursor(*fc);
		  _painter.begin(this);

		  doc->drawMarker(*fc,&_painter,xOffset,yOffset);
		  markerIsVisible = true;
		
		  _painter.end();

		  if (isInSelection(fc))
		    {
		      maybeDrag = true;
		      return;
		    }
		  else
		    {
		      if (doc->has_selection())
			{
			  _painter.begin(this);
			  doc->drawSelection(_painter,xOffset,yOffset);
			  doc->setSelection(false);
			  _painter.end();
			}
		    }
		
		  doc->setSelStart(*fc);
		  doc->setSelEnd(*fc);
		  doc->setSelection(false);
		
		  if (doc->getProcessingType() == KWordDocument::DTP)
		    {
		      int frame = doc->getFrameSet(frameset)->getFrame(mx,my);
		      if (frame != -1)
			{
			  if (doc->getProcessingType() == KWordDocument::DTP)
			    setRuler2Frame(frameset,frame);
			}
		      selectedFrame = frame;
		      selectedFrameSet = frameset;
		    }
		
		  gui->getVertRuler()->setOffset(0,-getVertRulerPos());
		
		  if (fc->getParag())
		    {	
		      gui->getView()->updateStyle(fc->getParag()->getParagLayout()->getName(),false);
		      gui->getView()->setFormat(*((KWFormat*)fc),true,false);
		      gui->getView()->setFlow(fc->getParag()->getParagLayout()->getFlow());
		      gui->getView()->setLineSpacing(fc->getParag()->getParagLayout()->getLineSpacing().pt());
		      gui->getView()->setParagBorders(fc->getParag()->getParagLayout()->getLeftBorder(),
					fc->getParag()->getParagLayout()->getRightBorder(),
					fc->getParag()->getParagLayout()->getTopBorder(),
					fc->getParag()->getParagLayout()->getBottomBorder());
		      setRulerFirstIndent(gui->getHorzRuler(),fc->getParag()->getParagLayout()->getFirstLineLeftIndent());
		      setRulerLeftIndent(gui->getHorzRuler(),fc->getParag()->getParagLayout()->getLeftIndent());
		      gui->getHorzRuler()->setFrameStart(doc->getFrameSet(fc->getFrameSet() - 1)->getFrame(fc->getFrame() - 1)->x());
		      gui->getHorzRuler()->setTabList(fc->getParag()->getParagLayout()->getTabList());
		    }
		}
	      else
		{
		  doc->drawMarker(*fc,&_painter,xOffset,yOffset);
		  markerIsVisible = true;
		  _painter.end();
		}
	    } break;
	  case MM_EDIT_FRAME:
	    {
	      int r = doc->selectFrame(mx,my);
	      bool goon = true;

	      if (r == 0)
		doc->deSelectAllFrames();
	
	      if (r != 0 && (e->state() & ShiftButton) && doc->getFrameSet(doc->getFrameSet(mx,my))->getGroupManager())
		{
		  doc->getFrameSet(doc->getFrameSet(mx,my))->getGroupManager()->selectUntil(doc->getFrameSet(doc->getFrameSet(mx,my)));
		  curTable = doc->getFrameSet(doc->getFrameSet(mx,my))->getGroupManager();
		  goon = false;
		}

	      if (goon)
		{
		  if (r == 1)
		    {
		      if (!(e->state() & ControlButton || e->state() & ShiftButton))
			doc->deSelectAllFrames();
		      doc->selectFrame(mx,my);
		      curTable = doc->getFrameSet(doc->getFrameSet(mx,my))->getGroupManager();		
		    }
		  else if (r == 2)
		    {
		      if (e->state() & ControlButton || e->state() & ShiftButton)
			{
			  doc->deSelectFrame(mx,my);
			  curTable = doc->getFrameSet(doc->getFrameSet(mx,my))->getGroupManager();
			}
		      else if (cursor().shape() != SizeAllCursor)
			{
			  doc->deSelectAllFrames();
			  doc->selectFrame(mx,my);
			  curTable = doc->getFrameSet(doc->getFrameSet(mx,my))->getGroupManager();			
			}
		    }
		}

	      repaint(false);

	      mousePressed = false;
	      mouseMoveEvent(e);
	      mousePressed = true;

	      deleteMovingRect = false;
	      mx = (mx / doc->getRastX()) * doc->getRastX();
	      oldMx = mx;
	      my = (my / doc->getRastX()) * doc->getRastY();
	      oldMy = my;
	    } break;
	  case MM_CREATE_TEXT: case MM_CREATE_PART: case MM_CREATE_TABLE: case MM_CREATE_FORMULA: case MM_CREATE_KSPREAD_TABLE:
	    {
	      mx -= xOffset;
	      my -= yOffset;
	      mx = (mx / doc->getRastX()) * doc->getRastX();
	      oldMx = mx;
	      my = (my / doc->getRastX()) * doc->getRastY();
	      oldMy = my;
	      insRect = KRect(mx,my,0,0);
	      deleteMovingRect = false;
	    } break;
	  case MM_CREATE_PIX:
	    {
	      mx -= xOffset;
	      my -= yOffset;
	      if (!pixmap_name.isEmpty())
		{
		  QPixmap _pix(pixmap_name);
		  mx = (mx / doc->getRastX()) * doc->getRastX();
		  oldMx = mx;
		  my = (my / doc->getRastX()) * doc->getRastY();
		  oldMy = my;
		  insRect = KRect(mx,my,0,0);
		  deleteMovingRect = false;
		  doRaster = false;
 		  QCursor::setPos(mapToGlobal(QPoint(mx + _pix.width(),my + _pix.height())));
		}
	      else pixmap_name = "";
	    } break;
	  default: break;
	  }
      } break;
    case MidButton:
      {
	QClipboard *cb = QApplication::clipboard();

	if (cb->data()->provides(MIME_TYPE))
	  {
	    if (cb->data()->encodedData(MIME_TYPE).size())
	      editPaste(cb->data()->encodedData(MIME_TYPE),MIME_TYPE);
	  }
	else if (cb->data()->provides("text/plain"))
	  {
	    if (cb->data()->encodedData("text/plain").size())
	      editPaste(cb->data()->encodedData("text/plain"));
	  }
	else if (!cb->text().isEmpty())
	  editPaste(cb->text());
      } break;
    case RightButton:
      {
	unsigned int mx = e->x() + xOffset;
	unsigned int my = e->y() + yOffset;

	if (mouseMode != MM_EDIT_FRAME)
	  {
	    KWCharAttribute *attrib = 0L;
	
	    QPainter painter;	
	    painter.begin(this);
	    attrib = fc->getObjectType(mx,my,painter);
	    painter.end();
	
	    if (attrib)
	      debug("ObjectType: %d, %p",attrib->getClassId(),attrib);
	    break;
	  }
	if (doc->getFrameSet(mx,my) == -1)
	  {
	    KPoint pnt(QCursor::pos());
	    mm_menu->popup(pnt);
	  }
	else
	  {
	    switch (mouseMode)
	      {
	      case MM_EDIT_FRAME:
		{
		  oldMx = mx;
		  oldMy = my;
		  if (doc->selectFrame(mx,my) == 1 && !(e->state() & ControlButton || e->state() & ShiftButton)) doc->deSelectAllFrames();
 		  doc->selectFrame(mx,my);
 		  repaint(false);
		  KPoint pnt(QCursor::pos());
		  frame_edit_menu->popup(pnt);
		} break;
	      default: break;
	      }
	  }
      } break;
    default: break;
    }
  mouseMoved = false;
}

/*================================================================*/
void KWPage::mouseReleaseEvent(QMouseEvent *e)
{
  if (maybeDrag && doc->has_selection() && mouseMode == MM_EDIT)
    {
      doc->setSelection(false);
      buffer.fill(white);
      repaint(false);
    }

  mousePressed = false;
  maybeDrag = false;

  switch (mouseMode)
    {
    case MM_EDIT:
      {
	if (doc->has_selection())
	  doc->copySelectedText();
	
	gui->getView()->updateStyle(fc->getParag()->getParagLayout()->getName(),false);
	gui->getView()->setFormat(*((KWFormat*)fc),true,false);
	gui->getView()->setFlow(fc->getParag()->getParagLayout()->getFlow());
	gui->getView()->setLineSpacing(fc->getParag()->getParagLayout()->getLineSpacing().pt());
	gui->getView()->setParagBorders(fc->getParag()->getParagLayout()->getLeftBorder(),
					fc->getParag()->getParagLayout()->getRightBorder(),
					fc->getParag()->getParagLayout()->getTopBorder(),
					fc->getParag()->getParagLayout()->getBottomBorder());
	setRulerFirstIndent(gui->getHorzRuler(),fc->getParag()->getParagLayout()->getFirstLineLeftIndent());
	setRulerLeftIndent(gui->getHorzRuler(),fc->getParag()->getParagLayout()->getLeftIndent());
	gui->getHorzRuler()->setFrameStart(doc->getFrameSet(fc->getFrameSet() - 1)->getFrame(fc->getFrame() - 1)->x());
	gui->getHorzRuler()->setTabList(fc->getParag()->getParagLayout()->getTabList());
	format = *((KWFormat*)fc);
      } break;
    case MM_EDIT_FRAME:
      {
	QPainter p;
	p.begin(this);
	for (unsigned int i = 0;i < doc->getNumGroupManagers();i++)
	  {
	    if (!doc->getGroupManager(i)->isActive()) continue;
	
	    doc->getGroupManager(i)->recalcCols();
	    doc->getGroupManager(i)->recalcRows(p);
	  }
	p.end();

	unsigned int mx = e->x() + xOffset;
	unsigned int my = e->y() + yOffset;
	selectedFrameSet = selectedFrame = -1;
	int frameset = doc->getFrameSet(mx,my),frame;
	if (frameset != -1)
	  {
	    frame = doc->getFrameSet(frameset)->getFrame(mx,my);
	    if (frame != -1)
	      {
		if (doc->getProcessingType() == KWordDocument::DTP)
		  setRuler2Frame(frameset,frame);
		gui->getHorzRuler()->setFrameStart(doc->getFrameSet(frameset)->getFrame(frame)->x());
	      }
	  }
	selectedFrame = frame;
	selectedFrameSet = frameset;
	if (mouseMoved)
	  {
	    doc->recalcFrames();
	    doc->updateAllFrames();
	    recalcAll = true;
	    recalcText();
	    recalcCursor();
	    recalcAll = false;
	  }
	else doc->updateAllViews(0L);
      } break;
    case MM_CREATE_TEXT:
      {
	repaint(false);
	KWFrame *frame = new KWFrame(insRect.x() + xOffset,insRect.y() + yOffset,insRect.width(),insRect.height());

	insRect = insRect.normalize();
	if (insRect.width() > doc->getRastX() && insRect.height() > doc->getRastY())
	  {
	    if (frameDia)
	      {
		frameDia->close();
		disconnect(frameDia,SIGNAL(frameDiaClosed()),this,SLOT(frameDiaClosed()));
		disconnect(frameDia,SIGNAL(applyButtonPressed()),this,SLOT(frameDiaClosed()));
		disconnect(frameDia,SIGNAL(cancelButtonPressed()),this,SLOT(frameDiaClosed()));
		disconnect(frameDia,SIGNAL(defaultButtonPressed()),this,SLOT(frameDiaClosed()));
		delete frameDia;
		frameDia = 0;
	      }
	
	    frameDia = new KWFrameDia(0,"",frame,doc,this,FD_FRAME_CONNECT | FD_FRAME | FD_PLUS_NEW_FRAME | FD_BORDERS);
	    connect(frameDia,SIGNAL(frameDiaClosed()),this,SLOT(frameDiaClosed()));
	    connect(frameDia,SIGNAL(applyButtonPressed()),this,SLOT(frameDiaClosed()));
	    connect(frameDia,SIGNAL(cancelButtonPressed()),this,SLOT(frameDiaClosed()));
	    connect(frameDia,SIGNAL(defaultButtonPressed()),this,SLOT(frameDiaClosed()));
	    frameDia->setCaption(i18n("KWord - Frame settings"));
	    frameDia->show();
	  }
      } break;
    case MM_CREATE_PIX:
      {
	repaint(false);

	insRect = insRect.normalize();
	if (insRect.width() > doc->getRastX() && insRect.height() > doc->getRastY() && !pixmap_name.isEmpty())
	  {
	    KWPictureFrameSet *frameset = new KWPictureFrameSet(doc);
	    frameset->setFileName(pixmap_name,KSize(insRect.width(),insRect.height()));
	    insRect = insRect.normalize();
	    KWFrame *frame = new KWFrame(insRect.x() + xOffset,insRect.y() + yOffset,insRect.width(),insRect.height());
	    frameset->addFrame(frame);
	    doc->addFrameSet(frameset);
	    repaint(false);
	  }
	mmEdit();
      } break;
    case MM_CREATE_PART: case MM_CREATE_KSPREAD_TABLE: case MM_CREATE_FORMULA:
      {
	repaint(false);

	insRect = insRect.normalize();
	if (insRect.width() > doc->getRastX() && insRect.height() > doc->getRastY())
	  {
	    doc->insertObject(insRect,partEntry,xOffset,yOffset);
	    buffer.fill(white);
	    repaint(false);
	  }
	mmEdit();
      } break;
    case MM_CREATE_TABLE:
      {
	repaint(false);

	insRect = insRect.normalize();
	if (insRect.width() > doc->getRastX() && insRect.height() > doc->getRastY())
	  {
	    if (tcols * 70 + insRect.x() > doc->getPTPaperWidth())
	      QMessageBox::critical(0L,i18n("Error"),i18n("There is not enough space to insert this table!"),i18n("OK"));
	    else
	      {
		KWGroupManager *grpMgr = new KWGroupManager(doc);
		QString _name;
		_name.sprintf("grpmgr_%d",doc->getNumGroupManagers());
		grpMgr->setName(_name);
		doc->addGroupManager(grpMgr);
		for (unsigned int i = 0;i < trows;i++)
		  {
		    for (unsigned int j = 0;j < tcols;j++)
		      {
			KWFrame *frame = new KWFrame(insRect.x() + xOffset,insRect.y() + yOffset,insRect.width(),insRect.height());
			KWTextFrameSet *_frameSet = new KWTextFrameSet(doc);
			_frameSet->addFrame(frame);
			_frameSet->setAutoCreateNewFrame(false);
			_frameSet->setGroupManager(grpMgr);
			grpMgr->addFrameSet(_frameSet,i,j);
		      }
		  }
		grpMgr->init(insRect.x() + xOffset,insRect.y() + yOffset,insRect.width(),insRect.height());
		QPainter p;
		p.begin(this);
		grpMgr->recalcRows(p);
		p.end();
	      }
	    buffer.fill(white);
	    repaint(false);
	  }
	mmEdit();
      } break;
    default: repaint(false);
    }
}

/*================================================================*/
void KWPage::mouseDoubleClickEvent(QMouseEvent *e)
{
  if (e->button() != LeftButton) return;

  unsigned int mx = e->x() + xOffset;
  unsigned int my = e->y() + yOffset;

  mousePressed = false;

  if (mouseMode == MM_EDIT)
    {
      QPainter _painter;
      _painter.begin(this);

      if (doc->has_selection())
	{
	  doc->drawSelection(_painter,xOffset,yOffset);
	  doc->setSelection(false);
	}

      int frameset = doc->getFrameSet(mx,my);

      if (frameset != -1 && doc->getFrameSet(frameset)->getFrameType() == FT_TEXT)
	{
	  fc->setFrameSet(frameset + 1);
	
	  doc->drawMarker(*fc,&_painter,xOffset,yOffset);
	  markerIsVisible = false;
	
	  fc->cursorGotoPixelLine(mx,my,_painter);
	  fc->cursorGotoPixelInLine(mx,my,_painter);
	
	  KWFormatContext fc1(doc,fc->getFrameSet()),fc2(doc,fc->getFrameSet());
	  fc->selectWord(fc1,fc2,_painter);
	
	  doc->drawMarker(*fc,&_painter,xOffset,yOffset);
	  markerIsVisible = true;
	
	  doc->setSelStart(fc1);
	  doc->setSelEnd(fc2);
	  doc->setSelection(true);
	  doc->drawSelection(_painter,xOffset,yOffset);
	
	  if (doc->getProcessingType() == KWordDocument::DTP)
	    setRuler2Frame(fc->getFrameSet() - 1,fc->getFrame() - 1);
	
	  gui->getVertRuler()->setOffset(0,-getVertRulerPos());
	
	  if (fc->getParag())
	    {	
	      setRulerFirstIndent(gui->getHorzRuler(),fc->getParag()->getParagLayout()->getFirstLineLeftIndent());
	      setRulerLeftIndent(gui->getHorzRuler(),fc->getParag()->getParagLayout()->getLeftIndent());
	      gui->getHorzRuler()->setFrameStart(doc->getFrameSet(fc->getFrameSet() - 1)->getFrame(fc->getFrame() - 1)->x());
	      gui->getHorzRuler()->setTabList(fc->getParag()->getParagLayout()->getTabList());
	    }
	}

      _painter.end();
    }
  else if (mouseMode == MM_EDIT_FRAME)
    {
      int frameset = doc->getFrameSet(mx,my);
      if (doc->getFrameSet(frameset)->getFrameType() == FT_PART)
	{
	  KWPartFrameSet *fs = dynamic_cast<KWPartFrameSet*>(doc->getFrameSet(frameset));
	  doc->hideAllFrames();
	  gui->getView()->setFramesToParts();
	  fs->activate(this,xOffset,yOffset,gui->getVertRuler()->width() + gui->getDocStruct()->width());
	  editNum = frameset;
	}
    }
}

/*================================================================*/
void KWPage::recalcCursor(bool _repaint = true,int _pos = -1,KWFormatContext *_fc = 0L)
{
  if (!_fc) _fc = fc;

  QPainter _painter;
  _painter.begin(this);

  unsigned int pos = _fc->getTextPos();
  if (_pos != -1) pos = static_cast<unsigned int>(_pos);

  _fc->init(_fc->getParag()->getPrev() ? _fc->getParag()->getPrev()->getNext() :
	    dynamic_cast<KWTextFrameSet*>(doc->getFrameSet(_fc->getFrameSet() - 1))->getFirstParag(),
	    _painter,false,false);

  _fc->gotoStartOfParag(_painter);
  _fc->cursorGotoLineStart(_painter);
  _fc->cursorGotoRight(_painter,pos);

  _painter.end();

  if (_repaint)
    {
      paint_directly = false;
      buffer.fill(white);
      repaint(false);
    }
}

/*================================================================*/
int KWPage::getVertRulerPos()
{
  int page = fc->getPage() - 1;

  return (-yOffset + page * ZOOM(ptPaperHeight()));
}

/*================================================================*/
void KWPage::insertPictureAsChar(QString _filename)
{
  fc->getParag()->insertPictureAsChar(fc->getTextPos(),_filename);
  recalcCursor();
}

/*================================================================*/
void KWPage::editCut()
{
  if (doc->has_selection())
    {
      QPainter p;
      p.begin(this);
      doc->copySelectedText();
      doc->deleteSelectedText(fc,p);
      p.end();
      doc->setSelection(false);
      buffer.fill(white);
      recalcCursor();
    }
}

/*================================================================*/
void KWPage::editCopy()
{
  if (doc->has_selection())
    {
      doc->copySelectedText();
      doc->setSelection(false);
      buffer.fill(white);
      repaint(false);
    }
}

/*================================================================*/
void KWPage::editPaste(QString _string,const QString &_mime = "text/plain")
{
  doc->paste(fc,_string,this,0L,_mime);
  buffer.fill(white);
  doc->setSelection(false);
  recalcText();
  recalcCursor();
}

/*================================================================*/
void KWPage::recalcText()
{
  QPainter painter;
  painter.begin(this);

  KWFormatContext _fc(doc,fc->getFrameSet());
  _fc.init(doc->getFirstParag(fc->getFrameSet() - 1),painter,true);

  bool bend = false;

  while (!bend)
    {
      bend = !_fc.makeNextLineLayout(painter);
      if (doc->getFrameSet(_fc.getFrameSet() - 1)->getFrame(_fc.getFrame() - 1)->y() > static_cast<int>(yOffset + height() + 20))
	bend = true;
    }

  painter.end();
}

/*================================================================*/
void KWPage::recalcWholeText(bool _cursor = false,bool _fast = false)
{
  if (recalcingText) return;

  setCursor(waitCursor);

  recalcingText = true;
  QPainter painter;
  painter.begin(this);

  for (unsigned int i = 0;i < doc->getNumFrameSets();i++)
    {
      if (doc->getFrameSet(i)->getFrameType() != FT_TEXT || doc->getFrameSet(i)->getNumFrames() == 0) continue;
      KWFormatContext _fc(doc,i + 1);
      _fc.init(doc->getFirstParag(i),painter,true);

      bool bend = false;

      while (!bend)
	{
	  bend = !_fc.makeNextLineLayout(painter);
	  if (/*_fast &&*/ doc->getFrameSet(_fc.getFrameSet() - 1)->getFrame(_fc.getFrame() - 1)->y() > static_cast<int>(yOffset + height() + 20))
	    bend = true;
	}
    }

  painter.end();
  if (_cursor) recalcCursor();
  recalcingText = false;

  setCursor(ibeamCursor);
}

/*================================================================*/
void KWPage::footerHeaderDisappeared()
{
  if (isAHeader(doc->getFrameSet(fc->getFrameSet() - 1)->getFrameInfo()) ||
      isAFooter(doc->getFrameSet(fc->getFrameSet() - 1)->getFrameInfo()))
    {
      fc->setFrameSet(1);
      QPainter p;
      p.begin(this);
      fc->init(doc->getFirstParag(0),p,false,false);
      p.end();

      gui->getView()->updateStyle(fc->getParag()->getParagLayout()->getName(),false);
      gui->getView()->setFormat(*((KWFormat*)fc),true,false);
      gui->getView()->setFlow(fc->getParag()->getParagLayout()->getFlow());
      gui->getView()->setLineSpacing(fc->getParag()->getParagLayout()->getLineSpacing().pt());
      gui->getView()->setParagBorders(fc->getParag()->getParagLayout()->getLeftBorder(),
				      fc->getParag()->getParagLayout()->getRightBorder(),
				      fc->getParag()->getParagLayout()->getTopBorder(),
				      fc->getParag()->getParagLayout()->getBottomBorder());
      setRulerFirstIndent(gui->getHorzRuler(),fc->getParag()->getParagLayout()->getFirstLineLeftIndent());
      setRulerLeftIndent(gui->getHorzRuler(),fc->getParag()->getParagLayout()->getLeftIndent());
      gui->getHorzRuler()->setFrameStart(doc->getFrameSet(fc->getFrameSet() - 1)->getFrame(fc->getFrame() - 1)->x());
      gui->getHorzRuler()->setTabList(fc->getParag()->getParagLayout()->getTabList());
      format = *((KWFormat*)fc);
    }
}

/*================================================================*/
void KWPage::recalcPage(KWParag *_p)
{
  QPainter painter;
  painter.begin(this);

  calcVisiblePages();
  KWFormatContext *paintfc = new KWFormatContext(doc,1);
  for (unsigned i = 0;i < doc->getNumFrameSets();i++)
    {
      switch (doc->getFrameSet(i)->getFrameType())
	{
	case FT_TEXT:
	  {
	    KWParag *p = doc->findFirstParagOfRect(yOffset,firstVisiblePage,i);
	    if (p)
	      {
		paintfc->setFrameSet(i + 1);
		paintfc->init(p,painter,true,recalcAll);
		
		if (i == fc->getFrameSet() - 1 && _p)
		  {
		    while (paintfc->getParag() != _p->getNext())
		      paintfc->makeNextLineLayout(painter);
		  }
		else
		  {
		    bool bend = false;
		    while (!bend)
		      {
			if (paintfc->getFrameSet() == 1 && doc->getProcessingType() == KWordDocument::WP &&
			    static_cast<int>(paintfc->getPTY() - yOffset) > height() && doc->getColumns() == 1) break;
			if (doc->getFrameSet(i)->getFrame(paintfc->getFrame() - 1)->isMostRight() &&
			    doc->getFrameSet(i)->getNumFrames() > paintfc->getFrame() &&
			    doc->getFrameSet(i)->getFrame(paintfc->getFrame())->top() -
			    static_cast<int>(yOffset) >
			    static_cast<int>(lastVisiblePage) * static_cast<int>(ptPaperHeight()) &&
			    static_cast<int>(paintfc->getPTY() - yOffset) > height())
			  break;
			if (doc->getFrameSet(i)->getFrame(paintfc->getFrame() - 1)->top() - static_cast<int>(yOffset) >
			    static_cast<int>(lastVisiblePage) * static_cast<int>(ptPaperHeight()))
			  break;
			bend = !paintfc->makeNextLineLayout(painter);
			if (paintfc->getPage() > lastVisiblePage)
			  bend = true;
		      }
		  }
	      }
	  } break;
	default: break;
	}
    }

  delete paintfc;
  painter.end();
}

/*================================================================*/
void KWPage::paintEvent(QPaintEvent* e)
{
  QPainter painter;
  if (paint_directly)
    painter.begin(this);
  else
    {
      if (has_to_copy) copyBuffer();
      painter.begin(&buffer);
    }
  painter.setClipRect(e->rect());

  painter.eraseRect(e->rect().x() + xOffset,e->rect().y() + yOffset,
		    e->rect().width(),e->rect().height());

  drawBorders(painter,e->rect());

  KWFormatContext *paintfc = new KWFormatContext(doc,1);
  for (unsigned i = 0;i < doc->getNumFrameSets();i++)
    {
      switch (doc->getFrameSet(i)->getFrameType())
	{
	case FT_PICTURE:
	  {
	    KWPictureFrameSet *picFS = dynamic_cast<KWPictureFrameSet*>(doc->getFrameSet(i));
	    KWFrame *frame = picFS->getFrame(0);
	    KSize _size = QSize(frame->width(),frame->height());
	
	    if (_size != picFS->getImage()->size())
	      picFS->setSize(_size);

	    painter.drawImage(frame->x() - xOffset,frame->y() - yOffset,*picFS->getImage());
	    if (frame->isSelected() && mouseMode == MM_EDIT_FRAME)
	      drawFrameSelection(painter,frame);
	  } break;
	case FT_PART:
	  {
	    KWPartFrameSet *partFS = dynamic_cast<KWPartFrameSet*>(doc->getFrameSet(i));
	    KWFrame *frame = partFS->getFrame(0);

	    bool hc = painter.hasClipping();
	    painter.end();

	    if (!paint_directly)
	      {
		if (hc)
		  drawBuffer(e->rect());
		else
		  drawBuffer();
	      }
	
	    QPicture *pic = partFS->getPicture();
	
	    if (paint_directly)
	      painter.begin(this);
	    else
	      {
		if (has_to_copy) copyBuffer();
		painter.begin(&buffer);
	      }

	    painter.save();
	    KRect r = painter.viewport();
	    painter.setViewport(frame->x() - xOffset,frame->y() - yOffset,r.width(),r.height());
 	    if (pic) painter.drawPicture(*pic);
	    painter.setViewport(r);
	    painter.restore();
	  } break;
	case FT_TEXT:
	  {
	    if (isAHeader(doc->getFrameSet(i)->getFrameInfo()) && !doc->hasHeader() ||
		isAFooter(doc->getFrameSet(i)->getFrameInfo()) && !doc->hasFooter() ||
		isAWrongHeader(doc->getFrameSet(i)->getFrameInfo(),doc->getHeaderType()) ||
		isAWrongFooter(doc->getFrameSet(i)->getFrameInfo(),doc->getFooterType()))
	      break;

	    if (doc->getFrameSet(i)->getFrameInfo() == FI_BODY)
	      {
		KWParag *p = 0L;
		p = doc->findFirstParagOfRect(e->rect().y() + yOffset,firstVisiblePage,i);
		
		if (p)
		  {
		    paintfc->setFrameSet(i + 1);
		    paintfc->init(p,painter,true,recalcAll);

		    bool bend = false;
		    while (!bend)
		      {
			if (paintfc->getFrameSet() == 1 && doc->getProcessingType() == KWordDocument::WP &&
			    static_cast<int>(paintfc->getParag()->getPTYStart() - yOffset) > height() && doc->getColumns() == 1) break;
			if (doc->getFrameSet(i)->getFrame(paintfc->getFrame() - 1)->isMostRight() &&
			    doc->getFrameSet(i)->getNumFrames() > paintfc->getFrame() &&
			    doc->getFrameSet(i)->getFrame(paintfc->getFrame())->top() -
			    static_cast<int>(yOffset) >
			    static_cast<int>(lastVisiblePage) * static_cast<int>(ptPaperHeight()) &&
			    static_cast<int>(paintfc->getPTY() - yOffset) > height())
			  break;
			if (doc->getFrameSet(i)->getFrame(paintfc->getFrame() - 1)->top() - static_cast<int>(yOffset) >
			    static_cast<int>(lastVisiblePage) * static_cast<int>(ptPaperHeight()))
			  break;
			doc->printLine(*paintfc,painter,xOffset,yOffset,width(),height(),gui->getView()->getViewFormattingChars());
			bend = !paintfc->makeNextLineLayout(painter);
			if (paintfc->getPage() > lastVisiblePage)
			  bend = true;
		      }
		  }
	      }
	    else
	      {
		KWParag *p = 0L;

		KWFrameSet *fs = doc->getFrameSet(i);
		KRect r = KRect(e->rect().x() + xOffset,e->rect().y() + yOffset,
				e->rect().width(),e->rect().height());
		KRect v = KRect(e->rect().x() + xOffset,e->rect().y() + yOffset,
				e->rect().width(),e->rect().height());

		while (true)
		  {
		    int frm = fs->getNext(r);
		    if (!v.intersects(r)) break;
		    if (frm == -1) break;
		
		    r.moveBy(0,fs->getFrame(frm)->height() + 1);

		    p = dynamic_cast<KWTextFrameSet*>(fs)->getFirstParag();
		    paintfc->setFrameSet(i + 1);
		    paintfc->init(p,painter,true,true,frm + 1,fs->getFrame(frm)->getPageNum() + 1);
		
		    bool bend = false;
		    while (!bend)
		      {
			if (paintfc->getFrameSet() == 1 && doc->getProcessingType() == KWordDocument::WP &&
			    static_cast<int>(paintfc->getParag()->getPTYStart() - yOffset) > height() && doc->getColumns() == 1) break;
			if (doc->getFrameSet(i)->getFrame(paintfc->getFrame() - 1)->isMostRight() &&
			    doc->getFrameSet(i)->getNumFrames() > paintfc->getFrame() &&
			    doc->getFrameSet(i)->getFrame(paintfc->getFrame())->top() -
			    static_cast<int>(yOffset) >
			    static_cast<int>(lastVisiblePage) * static_cast<int>(ptPaperHeight()) &&
			    static_cast<int>(paintfc->getPTY() - yOffset) > height())
			  break;
			if (doc->getFrameSet(i)->getFrame(paintfc->getFrame() - 1)->top() - static_cast<int>(yOffset) >
			    static_cast<int>(lastVisiblePage) * static_cast<int>(ptPaperHeight()))
			  break;
			doc->printLine(*paintfc,painter,xOffset,yOffset,width(),height(),gui->getView()->getViewFormattingChars());
			bend = !paintfc->makeNextLineLayout(painter);
			if (paintfc->getPage() > lastVisiblePage)
			  bend = true;
		      }
		  }
	      }
	  } break;
	default: break;
	}
    }
  delete paintfc;

  KWFormatContext _fc(doc,fc->getFrameSet());
  _fc = *fc;
  KWFrameSet *frameSet = doc->getFrameSet(_fc.getFrameSet() - 1);
  unsigned int _x = frameSet->getFrame(_fc.getFrame() - 1)->x() - xOffset;
  unsigned int _wid = frameSet->getFrame(_fc.getFrame() - 1)->width();
  if (e->rect().intersects(QRect(_x + frameSet->getFrame(_fc.getFrame() - 1)->getLeftIndent(_fc.getPTY(),_fc.getLineHeight()),
				 _fc.getPTY() - yOffset,
				 _wid - frameSet->getFrame(_fc.getFrame() - 1)->getLeftIndent(_fc.getPTY(),_fc.getLineHeight()) -
				 frameSet->getFrame(_fc.getFrame() - 1)->getRightIndent(_fc.getPTY(),_fc.getLineHeight()),
				 _fc.getLineHeight())))
    {
      if (!e->rect().contains(QRect(_x + frameSet->getFrame(_fc.getFrame() - 1)->getLeftIndent(_fc.getPTY(),_fc.getLineHeight()),
				    _fc.getPTY() - yOffset,
				    _wid - frameSet->getFrame(_fc.getFrame() - 1)->getLeftIndent(_fc.getPTY(),_fc.getLineHeight()) -
				    frameSet->getFrame(_fc.getFrame() - 1)->getRightIndent(_fc.getPTY(),_fc.getLineHeight()),
				    _fc.getLineHeight())))
	painter.setClipping(false);
    }

  painter.fillRect(_x + frameSet->getFrame(_fc.getFrame() - 1)->getLeftIndent(_fc.getPTY(),_fc.getLineHeight()),
		   _fc.getPTY() - yOffset,
		   _wid - frameSet->getFrame(_fc.getFrame() - 1)->getLeftIndent(_fc.getPTY(),_fc.getLineHeight()) -
		   frameSet->getFrame(_fc.getFrame() - 1)->getRightIndent(_fc.getPTY(),_fc.getLineHeight()),
		   _fc.getLineHeight(),QBrush(frameSet->getFrame(_fc.getFrame() - 1)->getBackgroundColor()));
  doc->printLine(_fc,painter,xOffset,yOffset,width(),height(),gui->getView()->getViewFormattingChars());

  if (doc->has_selection()) doc->drawSelection(painter,xOffset,yOffset);

  doc->drawMarker(*fc,&painter,xOffset,yOffset);
  markerIsVisible = true;

  bool hasClipping = painter.hasClipping();
  painter.end();

  if (!paint_directly)
    {
      if (hasClipping)
	drawBuffer(e->rect());
      else
	drawBuffer();
    }
}

/*================================================================*/
void KWPage::keyPressEvent(QKeyEvent *e)
{
  if (mouseMode != MM_EDIT) return;

  editModeChanged(e);

  // if we are in a table and CTRL-Return was pressed
  if ((e->key() == Key_Return || e->key() == Key_Return) && (e->state() & ControlButton) &&
      doc->getFrameSet(fc->getFrameSet() - 1)->getGroupManager())
    {
      KWGroupManager *grpMgr = doc->getFrameSet(fc->getFrameSet() - 1)->getGroupManager();
      unsigned int row,col;
      grpMgr->getFrameSet(doc->getFrameSet(fc->getFrameSet() - 1),row,col);

      QPainter p;
      p.begin(this);
      grpMgr->insertRow(row + 1,p);
      p.end();
			
      doc->recalcFrames();
      doc->updateAllFrames();

      fc->setFrameSet(doc->getFrameSetNum(grpMgr->getFrameSet(row + 1,col)) + 1);
      p.begin(this);
      doc->drawMarker(*fc,&p,xOffset,yOffset);
      fc->init(dynamic_cast<KWTextFrameSet*>(doc->getFrameSet(fc->getFrameSet() - 1))->getFirstParag(),p,true,true);
      fc->gotoStartOfParag(p);
      fc->cursorGotoLineStart(p);
      p.end();

      scrollToCursor(*fc);

      gui->getVertRuler()->setOffset(0,-getVertRulerPos());
      gui->getView()->updateStyle(fc->getParag()->getParagLayout()->getName());
      setRulerFirstIndent(gui->getHorzRuler(),fc->getParag()->getParagLayout()->getFirstLineLeftIndent());
      setRulerLeftIndent(gui->getHorzRuler(),fc->getParag()->getParagLayout()->getLeftIndent());
      gui->getView()->setFormat(*((KWFormat*)fc));
      gui->getView()->setFlow(fc->getParag()->getParagLayout()->getFlow());
      gui->getView()->setLineSpacing(fc->getParag()->getParagLayout()->getLineSpacing().pt());
      gui->getView()->setParagBorders(fc->getParag()->getParagLayout()->getLeftBorder(),
				      fc->getParag()->getParagLayout()->getRightBorder(),
				      fc->getParag()->getParagLayout()->getTopBorder(),
				      fc->getParag()->getParagLayout()->getBottomBorder());
      gui->getHorzRuler()->setFrameStart(doc->getFrameSet(fc->getFrameSet() - 1)->getFrame(fc->getFrame() - 1)->x());
      gui->getHorzRuler()->setTabList(fc->getParag()->getParagLayout()->getTabList());

      if (doc->getProcessingType() == KWordDocument::DTP)
	setRuler2Frame(fc->getFrameSet() - 1,fc->getFrame() - 1);

      doc->updateAllViews(0L);
      return;
    }

  inKeyEvent = true;
  unsigned int oldPage = fc->getPage();
  unsigned int oldFrame = fc->getFrame();
  KWParag* oldParag = fc->getParag();
  KWTextFrameSet *frameSet = dynamic_cast<KWTextFrameSet*>(doc->getFrameSet(fc->getFrameSet() - 1));

  XKeyboardControl kbdc;
  XKeyboardState kbds;
  bool repeat = true;
  bool continueSelection = false;

  // HACK
  XGetKeyboardControl(kapp->getDisplay(),&kbds);
  repeat = kbds.global_auto_repeat;
  kbdc.auto_repeat_mode = false;
  XChangeKeyboardControl(kapp->getDisplay(),KBAutoRepeatMode,&kbdc);

  QPainter painter;
  painter.begin(this);

  if (doc->has_selection())
    {
      if (e->key() == Key_Shift || (e->state() & ShiftButton) && (e->key() == Key_Left || e->key() == Key_Right ||
								  e->key() == Key_Up || e->key() == Key_Down))
	continueSelection = true;
      else
	{
	  doc->setSelection(false);
	  doc->drawSelection(painter,xOffset,yOffset);
	  if (e->key() == Key_Delete || e->key() == Key_Backspace || e->key() == Key_Return ||
	      e->key() == Key_Enter || e->ascii() >= 32)
	    {
	      doc->deleteSelectedText(fc,painter);
	      painter.end();
	      recalcCursor();
	      if (e->key() == Key_Delete || e->key() == Key_Backspace || e->key() == Key_Return || e->key() == Key_Enter)
		{
		  buffer.fill(white);
		  has_to_copy = false;
		  repaint(false);
		  kbdc.auto_repeat_mode = repeat;
		  XChangeKeyboardControl(kapp->getDisplay(),KBAutoRepeatMode,&kbdc);
		  inKeyEvent = false;
		  return;
		}
	      else
		painter.begin(this);
	    }
	}
    }

  doc->drawMarker(*fc,&painter,xOffset,yOffset);
  painter.end();

  markerIsVisible = false;

  painter.begin(&buffer);
  doc->drawMarker(*fc,&painter,xOffset,yOffset);
  bool draw_buffer = false;

  switch(e->key())
    {
    case Key_Home:
      {
	fc->cursorGotoLineStart(painter);
	gui->getView()->setFormat(*((KWFormat*)fc));
	gui->getView()->setFlow(fc->getParag()->getParagLayout()->getFlow());
	gui->getView()->setLineSpacing(fc->getParag()->getParagLayout()->getLineSpacing().pt());
	gui->getView()->setParagBorders(fc->getParag()->getParagLayout()->getLeftBorder(),
					fc->getParag()->getParagLayout()->getRightBorder(),
					fc->getParag()->getParagLayout()->getTopBorder(),
					fc->getParag()->getParagLayout()->getBottomBorder());
      } break;
    case Key_End:
      {
	fc->cursorGotoLineEnd(painter);
	gui->getView()->setFormat(*((KWFormat*)fc));
	gui->getView()->setFlow(fc->getParag()->getParagLayout()->getFlow());
	gui->getView()->setLineSpacing(fc->getParag()->getParagLayout()->getLineSpacing().pt());
	gui->getView()->setParagBorders(fc->getParag()->getParagLayout()->getLeftBorder(),
					fc->getParag()->getParagLayout()->getRightBorder(),
					fc->getParag()->getParagLayout()->getTopBorder(),
					fc->getParag()->getParagLayout()->getBottomBorder());
      } break;
    case Key_Right:
      {
	if (!doc->has_selection() && e->state() & ShiftButton)
	  {
	    doc->setSelStart(*fc);
	    doc->setSelEnd(*fc);
	    doc->setSelection(true);
	  }
	
	fc->cursorGotoRight(painter);
	gui->getView()->setFormat(*((KWFormat*)fc));
	gui->getView()->setFlow(fc->getParag()->getParagLayout()->getFlow());
	gui->getView()->setLineSpacing(fc->getParag()->getParagLayout()->getLineSpacing().pt());
	gui->getView()->setParagBorders(fc->getParag()->getParagLayout()->getLeftBorder(),
					fc->getParag()->getParagLayout()->getRightBorder(),
					fc->getParag()->getParagLayout()->getTopBorder(),
					fc->getParag()->getParagLayout()->getBottomBorder());

	if (continueSelection || e->state() & ShiftButton)
	  {
	    painter.end();

	    painter.begin(this);
	    doc->drawSelection(painter,xOffset,yOffset);
	    doc->setSelEnd(*fc);
	    doc->setSelection(false);
	    painter.end();
	
	    scrollToCursor(*fc);
	
	    doc->setSelection(true);
	    painter.begin(this);
	    doc->drawMarker(*fc,&painter,xOffset,yOffset);
	    doc->drawSelection(painter,xOffset,yOffset);
	    painter.end();
	
	    kbdc.auto_repeat_mode = repeat;
	    XChangeKeyboardControl(kapp->getDisplay(),KBAutoRepeatMode,&kbdc);
	    inKeyEvent = false;
	    return;
	  }
      } break;
    case Key_Left:
      {
	if (!doc->has_selection() && e->state() & ShiftButton)
	  {
	    doc->setSelStart(*fc);
	    doc->setSelEnd(*fc);
	    doc->setSelection(true);
	  }
	
	fc->cursorGotoLeft(painter);
	gui->getView()->setFormat(*((KWFormat*)fc));
	gui->getView()->setFlow(fc->getParag()->getParagLayout()->getFlow());
	gui->getView()->setLineSpacing(fc->getParag()->getParagLayout()->getLineSpacing().pt());
	gui->getView()->setParagBorders(fc->getParag()->getParagLayout()->getLeftBorder(),
					fc->getParag()->getParagLayout()->getRightBorder(),
					fc->getParag()->getParagLayout()->getTopBorder(),
					fc->getParag()->getParagLayout()->getBottomBorder());

	if (continueSelection || e->state() & ShiftButton)
	  {
	    painter.end();

	    painter.begin(this);
	    doc->drawSelection(painter,xOffset,yOffset);
	    doc->setSelEnd(*fc);
	    doc->setSelection(false);
	    painter.end();
	
	    scrollToCursor(*fc);
	
	    doc->setSelection(true);
	    painter.begin(this);
	    doc->drawMarker(*fc,&painter,xOffset,yOffset);
	    doc->drawSelection(painter,xOffset,yOffset);
	    painter.end();
	
	    kbdc.auto_repeat_mode = repeat;
	    XChangeKeyboardControl(kapp->getDisplay(),KBAutoRepeatMode,&kbdc);
	    inKeyEvent = false;
	    return;
	  }
      } break;
    case Key_Up:
      {
	if (!doc->has_selection() && e->state() & ShiftButton)
	  {
	    doc->setSelStart(*fc);
	    doc->setSelEnd(*fc);
	    doc->setSelection(true);
	  }
	
	fc->cursorGotoUp(painter);
	gui->getView()->setFormat(*((KWFormat*)fc));
	gui->getView()->setFlow(fc->getParag()->getParagLayout()->getFlow());
	gui->getView()->setLineSpacing(fc->getParag()->getParagLayout()->getLineSpacing().pt());
	gui->getView()->setParagBorders(fc->getParag()->getParagLayout()->getLeftBorder(),
					fc->getParag()->getParagLayout()->getRightBorder(),
					fc->getParag()->getParagLayout()->getTopBorder(),
					fc->getParag()->getParagLayout()->getBottomBorder());

	if (continueSelection || e->state() & ShiftButton)
	  {
	    painter.end();

	    painter.begin(this);
	    doc->drawSelection(painter,xOffset,yOffset);
	    doc->setSelEnd(*fc);
	    doc->setSelection(false);
	    painter.end();
	
	    scrollToCursor(*fc);
	
	    doc->setSelection(true);
	    painter.begin(this);
	    doc->drawMarker(*fc,&painter,xOffset,yOffset);
	    doc->drawSelection(painter,xOffset,yOffset);
	    painter.end();
	
	    kbdc.auto_repeat_mode = repeat;
	    XChangeKeyboardControl(kapp->getDisplay(),KBAutoRepeatMode,&kbdc);
	    inKeyEvent = false;
	    return;
	  }
      } break;
    case Key_Down:
      {
	if (!doc->has_selection() && e->state() & ShiftButton)
	  {
	    doc->setSelStart(*fc);
	    doc->setSelEnd(*fc);
	    doc->setSelection(true);
	  }

	fc->cursorGotoDown(painter);
	gui->getView()->setFormat(*((KWFormat*)fc));
	gui->getView()->setFlow(fc->getParag()->getParagLayout()->getFlow());
	gui->getView()->setLineSpacing(fc->getParag()->getParagLayout()->getLineSpacing().pt());
	gui->getView()->setParagBorders(fc->getParag()->getParagLayout()->getLeftBorder(),
					fc->getParag()->getParagLayout()->getRightBorder(),
					fc->getParag()->getParagLayout()->getTopBorder(),
					fc->getParag()->getParagLayout()->getBottomBorder());

	if (continueSelection || e->state() & ShiftButton)
	  {
	    painter.end();

	    painter.begin(this);
	    doc->drawSelection(painter,xOffset,yOffset);
	    doc->setSelEnd(*fc);
	    doc->setSelection(false);
	    painter.end();
	
	    scrollToCursor(*fc);
	
	    doc->setSelection(true);
	    painter.begin(this);
	    doc->drawMarker(*fc,&painter,xOffset,yOffset);
	    doc->drawSelection(painter,xOffset,yOffset);
	    painter.end();
	
	    kbdc.auto_repeat_mode = repeat;
	    XChangeKeyboardControl(kapp->getDisplay(),KBAutoRepeatMode,&kbdc);
	    inKeyEvent = false;
	    return;
	  }
      } break;
    case Key_Return: case Key_Enter:
      {
	if (has_to_copy) copyBuffer();
	draw_buffer = true;

	QString pln = fc->getParag()->getParagLayout()->getName();
	KWFormat _format(doc);
	_format = *((KWFormat*)fc);
	unsigned int tmpTextPos = fc->getTextPos();

	if (fc->isCursorAtParagEnd())
	  {
	    frameSet->insertParag(fc->getParag(),I_AFTER);
	    fc->setTextPos(0);
	    recalcPage(0L);
	    if (e->state() & ControlButton) fc->getParag()->getNext()->setHardBreak(true);
	    fc->init(fc->getParag()->getNext(),painter,false,e->state() & ControlButton);
	  }
	else if (fc->isCursorAtParagStart())
	  {
	    KWParag *oldFirst = frameSet->getFirstParag();
	    frameSet->insertParag(fc->getParag(),I_BEFORE);
	    fc->setTextPos(0);
	    recalcPage(0L);
	    if (e->state() & ControlButton) fc->getParag()->setHardBreak(true);
	    fc->init(fc->getParag(),painter,false,oldFirst != frameSet->getFirstParag() || e->state() & ControlButton);
	  }
	else
	  {
	    bool _insert = fc->isCursorAtLineStart();
	    frameSet->splitParag(fc->getParag(),tmpTextPos);
	    fc->setTextPos(0);
	    recalcPage(0L);
	    if ((e->state() & ControlButton) && !_insert) fc->getParag()->getNext()->setHardBreak(true);
	    fc->init(fc->getParag()->getNext(),painter,false,(e->state() & ControlButton) && !_insert);
	    if (_insert)
	      {
		frameSet->insertParag(fc->getParag(),I_BEFORE);
		fc->setTextPos(0);
		recalcPage(0L);
		if (e->state() & ControlButton) fc->getParag()->setHardBreak(true);
		fc->init(fc->getParag(),painter,false,e->state() & ControlButton);
	      }
	  }

	painter.end();
	draw_buffer = false;

	recalcCursor(false,0);

	buffer.fill(white);
	redrawAllWhileScrolling = true;
	scrollToCursor(*fc);
	redrawAllWhileScrolling = false;

 	painter.begin(this);
 	doc->drawMarker(*fc,&painter,xOffset,yOffset);
 	painter.end();

	// HACK
	kbdc.auto_repeat_mode = repeat;
	XChangeKeyboardControl(kapp->getDisplay(),KBAutoRepeatMode,&kbdc);

	if (oldPage != fc->getPage())
	  gui->getVertRuler()->setOffset(0,-getVertRulerPos());
	if (oldParag != fc->getParag() && fc->getParag())
	  {	
	    gui->getView()->updateStyle(fc->getParag()->getParagLayout()->getName());
	    setRulerFirstIndent(gui->getHorzRuler(),fc->getParag()->getParagLayout()->getFirstLineLeftIndent());
	    setRulerLeftIndent(gui->getHorzRuler(),fc->getParag()->getParagLayout()->getLeftIndent());
	    gui->getView()->setFormat(*((KWFormat*)fc));
	    gui->getView()->setFlow(fc->getParag()->getParagLayout()->getFlow());
	    gui->getView()->setLineSpacing(fc->getParag()->getParagLayout()->getLineSpacing().pt());
	    gui->getView()->setParagBorders(fc->getParag()->getParagLayout()->getLeftBorder(),
					    fc->getParag()->getParagLayout()->getRightBorder(),
					    fc->getParag()->getParagLayout()->getTopBorder(),
					    fc->getParag()->getParagLayout()->getBottomBorder());
	    gui->getHorzRuler()->setFrameStart(doc->getFrameSet(fc->getFrameSet() - 1)->getFrame(fc->getFrame() - 1)->x());
	    gui->getHorzRuler()->setTabList(fc->getParag()->getParagLayout()->getTabList());
	  }
	if (doc->getProcessingType() == KWordDocument::DTP && oldFrame != fc->getFrame())
	  setRuler2Frame(fc->getFrameSet() - 1,fc->getFrame() - 1);

	if (pln != fc->getParag()->getParagLayout()->getName())
	  {
	    format = fc->getParag()->getParagLayout()->getFormat();
	    fc->apply(format);
	  }
	else
	  fc->apply(_format);
	gui->getView()->setFormat(*((KWFormat*)fc));
	inKeyEvent = false;
	return;
      } break;
    case Key_Delete:
      {
	if (has_to_copy) copyBuffer();
	draw_buffer = false;

	unsigned int tmpTextPos = fc->getTextPos();
	bool del = fc->getParag()->deleteText(tmpTextPos,1);
	bool recalc = false;
	bool goNext = false;
	unsigned int lineEndPos;
	bool exitASAP = true;

	if (!del && fc->getParag()->getTextLen() == 0 && fc->getParag()->getNext())
	  {
	    KWParag *p = fc->getParag()->getNext();
	    int ptYStart = fc->getParag()->getPTYStart();
	    int startPage = fc->getParag()->getStartPage();
	    int startFrame = fc->getParag()->getStartFrame();
	    p->setPTYStart(ptYStart);
	    p->setStartPage(startPage);
	    p->setStartFrame(startFrame);
	    frameSet->deleteParag(fc->getParag());
	    if (p) fc->init(p,painter,false,false);
	    exitASAP = false;
	  }
	else if (!del && fc->getParag()->getTextLen() > 0)
	  {
	    frameSet->joinParag(fc->getParag(),fc->getParag()->getNext());
	    exitASAP = false;
	  }

	lineEndPos = fc->getLineEndPos();
	KWFormatContext paintfc(doc,fc->getFrameSet());

	if (!fc->isCursorInFirstLine())
	  {
	    goNext = true;
	    fc->cursorGotoPrevLine(painter);
	  }
	else
	  fc->makeLineLayout(painter);

	paintfc = *fc;
	bool bend = false;

	unsigned int currFrameNum = paintfc.getFrame() - 1;
	unsigned int ptYEnd = fc->getParag()->getPTYEnd();

	while (!bend)
	  {
	    if (paintfc.getParag() != fc->getParag() && paintfc.getParag() != fc->getParag()->getPrev() &&
		fc->getParag()->getPTYEnd() == ptYEnd && exitASAP) break;
	    if (frameSet->getFrame(currFrameNum)->isMostRight() && frameSet->getNumFrames() > currFrameNum + 1 &&
		frameSet->getFrame(paintfc.getFrame())->top() - static_cast<int>(yOffset) >
		static_cast<int>(lastVisiblePage) * static_cast<int>(ptPaperHeight()) &&
		static_cast<int>(paintfc.getPTY() - yOffset) > height())
	      break;
	    if (frameSet->getFrame(paintfc.getFrame() - 1)->top() - static_cast<int>(yOffset) >
		static_cast<int>(lastVisiblePage) * static_cast<int>(ptPaperHeight()))
	      break;
	    unsigned int _x = frameSet->getFrame(paintfc.getFrame() - 1)->x() - xOffset;
	    unsigned int _wid = frameSet->getFrame(paintfc.getFrame() - 1)->width();
	    painter.fillRect(_x + frameSet->getFrame(paintfc.getFrame() - 1)->getLeftIndent(paintfc.getPTY(),paintfc.getLineHeight()),
			     paintfc.getPTY() - yOffset,
			     _wid - frameSet->getFrame(paintfc.getFrame() - 1)->getLeftIndent(paintfc.getPTY(),paintfc.getLineHeight()) -
			     frameSet->getFrame(paintfc.getFrame() - 1)->getRightIndent(paintfc.getPTY(),paintfc.getLineHeight()),
			     paintfc.getLineHeight(),QBrush(frameSet->getFrame(paintfc.getFrame() - 1)->getBackgroundColor()));
	    if (doc->printLine(paintfc,painter,xOffset,yOffset,width(),height(),gui->getView()->getViewFormattingChars()))
	      {
		drawBuffer(QRect(_x + frameSet->getFrame(paintfc.getFrame() - 1)->getLeftIndent(paintfc.getPTY(),paintfc.getLineHeight()),
				 paintfc.getPTY() - yOffset,
				 _wid - frameSet->getFrame(paintfc.getFrame() - 1)->getLeftIndent(paintfc.getPTY(),
												  paintfc.getLineHeight()) -
				 frameSet->getFrame(paintfc.getFrame() - 1)->getRightIndent(paintfc.getPTY(),paintfc.getLineHeight()),
				 paintfc.getLineHeight()));
	      }
	    bend = !paintfc.makeNextLineLayout(painter);
	
	    if (paintfc.getFrame() - 1 != currFrameNum)
	      currFrameNum = paintfc.getFrame() - 1;
	
	    if (paintfc.getPage() > lastVisiblePage)
	      bend = true;
	  }

	if ((int)paintfc.getPTY() + (int)paintfc.getLineHeight() < frameSet->getFrame(paintfc.getFrame() - 1)->bottom() &&
	    !paintfc.getParag()->getNext())
	  {
	    painter.save();
	    QRegion rg = frameSet->getFrame(paintfc.getFrame() - 1)->getEmptyRegion();
	    rg.translate(-xOffset,-yOffset);
	    painter.setClipRegion(rg);
	    unsigned int _y = (int)paintfc.getParag()->getPTYEnd() - (int)yOffset;
	    unsigned int _x = frameSet->getFrame(paintfc.getFrame() - 1)->x() - xOffset ;
	    unsigned int _wid = frameSet->getFrame(paintfc.getFrame() - 1)->width();
	    unsigned int _hei = frameSet->getFrame(paintfc.getFrame() - 1)->height() -
	      (_y - frameSet->getFrame(paintfc.getFrame() - 1)->y());
	    painter.fillRect(_x,_y,_wid,_hei,QBrush(frameSet->getFrame(paintfc.getFrame() - 1)->getBackgroundColor()));
	    drawBuffer(KRect(_x,_y,_wid,_hei));
	    painter.restore();
	  }

	if (goNext)
	  fc->cursorGotoNextLine(painter);
	recalc = lineEndPos != fc->getLineEndPos();

	if (recalc && goNext)
	  recalcCursor(false,tmpTextPos);
	else
	  fc->cursorGotoPos(tmpTextPos,painter);
      } break;
    case Key_Backspace:
      {
	if (has_to_copy) copyBuffer();
	
	if (fc->isCursorAtLineStart() && fc->isCursorAtParagStart() && fc->getParag() == frameSet->getFirstParag())
	  break;
	
	draw_buffer = false;

	// HACK
	if (fc->isCursorAtLineStart() && !fc->isCursorAtParagStart())
	  {
	    fc->cursorGotoLeft(painter);
	    painter.end();
	    repaint(false);
	    // HACK
	    kbdc.auto_repeat_mode = repeat;
	    XChangeKeyboardControl(kapp->getDisplay(),KBAutoRepeatMode,&kbdc);

	    if (oldPage != fc->getPage())
	      gui->getVertRuler()->setOffset(0,-getVertRulerPos());
	    if (oldParag != fc->getParag() && fc->getParag())
	      {	
		gui->getView()->updateStyle(fc->getParag()->getParagLayout()->getName());
		setRulerFirstIndent(gui->getHorzRuler(),fc->getParag()->getParagLayout()->getFirstLineLeftIndent());
		setRulerLeftIndent(gui->getHorzRuler(),fc->getParag()->getParagLayout()->getLeftIndent());
		gui->getHorzRuler()->setFrameStart(doc->getFrameSet(fc->getFrameSet() - 1)->getFrame(fc->getFrame() - 1)->x());
		gui->getHorzRuler()->setTabList(fc->getParag()->getParagLayout()->getTabList());
		gui->getView()->setFormat(*((KWFormat*)fc));
		gui->getView()->setFlow(fc->getParag()->getParagLayout()->getFlow());
		gui->getView()->setLineSpacing(fc->getParag()->getParagLayout()->getLineSpacing().pt());
		gui->getView()->setParagBorders(fc->getParag()->getParagLayout()->getLeftBorder(),
						fc->getParag()->getParagLayout()->getRightBorder(),
						fc->getParag()->getParagLayout()->getTopBorder(),
						fc->getParag()->getParagLayout()->getBottomBorder());
	      }
	    if (doc->getProcessingType() == KWordDocument::DTP && oldFrame != fc->getFrame())
	      setRuler2Frame(fc->getFrameSet() - 1,fc->getFrame() - 1);

	    inKeyEvent = false;
	    return;
	  }

	unsigned int tmpTextPos = fc->getTextPos() - 1;
	unsigned int paraLen = (fc->getParag()->getPrev() ? fc->getParag()->getPrev()->getTextLen() : 0);
	bool del = fc->getParag()->deleteText(tmpTextPos,1);
	bool joined = false;
	bool recalc = false;
	bool goNext = false;
	unsigned int lineEndPos;
	bool exitASAP = true;

	if (!del && fc->getParag()->getTextLen() == 0)
	  {
	    KWParag *p = fc->getParag()->getPrev();
	    frameSet->deleteParag(fc->getParag());
	    if (p)
	      {
		fc->init(p,painter,false,false);
		tmpTextPos = p->getTextLen();
	      }
	    exitASAP = false;
	  }
	else if (!del && fc->getParag()->getTextLen() > 0)
	  {
	    KWParag *p = fc->getParag()->getPrev();
	    frameSet->joinParag(fc->getParag()->getPrev(),fc->getParag());
	    if (p) fc->init(p,painter,false,false);
	    joined = p ? true : false;
	    exitASAP = false;
	  }

	lineEndPos = fc->getLineEndPos();
	KWFormatContext paintfc(doc,fc->getFrameSet());

	if (!fc->isCursorInFirstLine())
	  {
	    goNext = true;
	    fc->cursorGotoPrevLine(painter);
	  }
	else
	  fc->makeLineLayout(painter);

	paintfc = *fc;
	bool bend = false;

	unsigned int currFrameNum = paintfc.getFrame() - 1;
	unsigned int ptYEnd = fc->getParag()->getPTYEnd();

	while (!bend)
	  {
	    if (paintfc.getParag() != fc->getParag() && paintfc.getParag() != fc->getParag()->getPrev() &&
		fc->getParag()->getPTYEnd() == ptYEnd && exitASAP) break;
	    if (frameSet->getFrame(currFrameNum)->isMostRight() && frameSet->getNumFrames() > currFrameNum + 1 &&
		frameSet->getFrame(paintfc.getFrame())->top() - static_cast<int>(yOffset) >
		static_cast<int>(lastVisiblePage) * static_cast<int>(ptPaperHeight()) &&
		static_cast<int>(paintfc.getPTY() - yOffset) > height())
	      break;
	    if (frameSet->getFrame(paintfc.getFrame() - 1)->top() - static_cast<int>(yOffset) >
		static_cast<int>(lastVisiblePage) * static_cast<int>(ptPaperHeight()))
	      break;
	    unsigned int _x = frameSet->getFrame(paintfc.getFrame() - 1)->x() - xOffset;
	    unsigned int _wid = frameSet->getFrame(paintfc.getFrame() - 1)->width();
	    painter.fillRect(_x + frameSet->getFrame(paintfc.getFrame() - 1)->getLeftIndent(paintfc.getPTY(),paintfc.getLineHeight()),
			     paintfc.getPTY() - yOffset,
			     _wid - frameSet->getFrame(paintfc.getFrame() - 1)->getLeftIndent(paintfc.getPTY(),paintfc.getLineHeight()) -
			     frameSet->getFrame(paintfc.getFrame() - 1)->getRightIndent(paintfc.getPTY(),paintfc.getLineHeight()),
			     paintfc.getLineHeight(),
			     doc->getFrameSet(paintfc.getFrameSet() - 1)->getFrame(paintfc.getFrame() - 1)->getBackgroundColor());
	    if (doc->printLine(paintfc,painter,xOffset,yOffset,width(),height(),gui->getView()->getViewFormattingChars()))
	      {
		drawBuffer(QRect(_x + frameSet->getFrame(paintfc.getFrame() - 1)->getLeftIndent(paintfc.getPTY(),paintfc.getLineHeight()),
				 paintfc.getPTY() - yOffset,
				 _wid - frameSet->getFrame(paintfc.getFrame() - 1)->getLeftIndent(paintfc.getPTY(),
												  paintfc.getLineHeight()) -
				 frameSet->getFrame(paintfc.getFrame() - 1)->getRightIndent(paintfc.getPTY(),paintfc.getLineHeight()),
				 paintfc.getLineHeight()));
	      }
	    bend = !paintfc.makeNextLineLayout(painter);
	
	    if (paintfc.getFrame() - 1 != currFrameNum)
	      currFrameNum = paintfc.getFrame() - 1;
	
	    if (paintfc.getPage() > lastVisiblePage)
	      bend = true;
	  }

	if ((int)paintfc.getPTY() + (int)paintfc.getLineHeight() < frameSet->getFrame(paintfc.getFrame() - 1)->bottom() &&
	    !paintfc.getParag()->getNext())
	  {
	    painter.save();
	    QRegion rg = frameSet->getFrame(paintfc.getFrame() - 1)->getEmptyRegion();
	    rg.translate(-xOffset,-yOffset);
	    painter.setClipRegion(rg);
	    unsigned int _y = (int)paintfc.getParag()->getPTYEnd() - (int)yOffset;
	    unsigned int _x = frameSet->getFrame(paintfc.getFrame() - 1)->x() - xOffset;
	    unsigned int _wid = frameSet->getFrame(paintfc.getFrame() - 1)->width();
	    unsigned int _hei = frameSet->getFrame(paintfc.getFrame() - 1)->height() -
	      (_y - frameSet->getFrame(paintfc.getFrame() - 1)->y());
	    painter.fillRect(_x,_y,_wid,_hei,
			     doc->getFrameSet(paintfc.getFrameSet() - 1)->getFrame(paintfc.getFrame() - 1)->getBackgroundColor());
	    drawBuffer(KRect(_x,_y,_wid,_hei));
	    painter.restore();
	  }

	if (goNext)
	  fc->cursorGotoNextLine(painter);
	recalc = lineEndPos != fc->getLineEndPos();
	
	if (recalc && goNext)
	  recalcCursor(false,tmpTextPos);
	else
	  {
	    if (!joined)
	      {
		if (!del)
		  {
		    fc->init(fc->getParag(),painter,false,false);
		    fc->cursorGotoLineStart(painter);
		    while (paraLen > fc->getLineEndPos())
		      fc->cursorGotoNextLine(painter);
		    fc->cursorGotoPos(paraLen,painter);
		  }
		else
		  fc->cursorGotoPos(tmpTextPos,painter);
	      }
	    else
	      {
		fc->init(fc->getParag(),painter,false,false);
		fc->cursorGotoLineStart(painter);
		while (paraLen > fc->getLineEndPos())
		  fc->cursorGotoNextLine(painter);
		fc->cursorGotoPos(paraLen,painter);
	      }
	  }
      } break;
    case Key_Shift: case Key_Control: case Key_Alt: case Key_Meta:
      break;
    case Key_Tab:
      {
	if (fc->getParag()->getParagLayout()->getTabList()->isEmpty()) break;
	if (has_to_copy) copyBuffer();
	
	draw_buffer = false;
	unsigned int tmpTextPos = fc->getTextPos();
	fc->getParag()->insertTab(fc->getTextPos());
	fc->makeLineLayout(painter);
	KWFormatContext paintfc(doc,fc->getFrameSet());

	paintfc = *fc;
	bool bend = false;
	
	unsigned int currFrameNum = paintfc.getFrame() - 1;
	unsigned int ptYEnd = fc->getParag()->getPTYEnd();
	
	while (!bend)
	  {
	    if (paintfc.getParag() != fc->getParag() && paintfc.getParag() != fc->getParag()->getPrev() &&
		fc->getParag()->getPTYEnd() == ptYEnd) break;
	    if (frameSet->getFrame(currFrameNum)->isMostRight() && frameSet->getNumFrames() > currFrameNum + 1 &&
		frameSet->getFrame(paintfc.getFrame())->top() - static_cast<int>(yOffset) >
		static_cast<int>(lastVisiblePage) * static_cast<int>(ptPaperHeight()) &&
		static_cast<int>(paintfc.getPTY() - yOffset) > height())
	      break;
	    if (frameSet->getFrame(paintfc.getFrame() - 1)->top() - static_cast<int>(yOffset) >
		static_cast<int>(lastVisiblePage) * static_cast<int>(ptPaperHeight()))
	      break;
	    unsigned int _x = frameSet->getFrame(paintfc.getFrame() - 1)->x() - xOffset;
	    unsigned int _wid = frameSet->getFrame(paintfc.getFrame() - 1)->width();
	    painter.fillRect(_x + frameSet->getFrame(paintfc.getFrame() - 1)->getLeftIndent(paintfc.getPTY(),paintfc.getLineHeight()),
			     paintfc.getPTY() - yOffset,
			     _wid - frameSet->getFrame(paintfc.getFrame() - 1)->getLeftIndent(paintfc.getPTY(),paintfc.getLineHeight()) -
			     frameSet->getFrame(paintfc.getFrame() - 1)->getRightIndent(paintfc.getPTY(),paintfc.getLineHeight()),
			     paintfc.getLineHeight(),
			     doc->getFrameSet(paintfc.getFrameSet() - 1)->getFrame(paintfc.getFrame() - 1)->getBackgroundColor());
	    if (doc->printLine(paintfc,painter,xOffset,yOffset,width(),height(),gui->getView()->getViewFormattingChars()))
	      {
		drawBuffer(KRect(_x + frameSet->getFrame(paintfc.getFrame() - 1)->getLeftIndent(paintfc.getPTY(),paintfc.getLineHeight()),
				 paintfc.getPTY() - yOffset,
				 _wid - frameSet->getFrame(paintfc.getFrame() - 1)->getLeftIndent(paintfc.getPTY(),
												  paintfc.getLineHeight()) -
				 frameSet->getFrame(paintfc.getFrame() - 1)->getRightIndent(paintfc.getPTY(),paintfc.getLineHeight()),
				 paintfc.getLineHeight()));
	      }
	    bend = !paintfc.makeNextLineLayout(painter);
	
	    if (paintfc.getFrame() - 1 != currFrameNum)
	      currFrameNum = paintfc.getFrame() - 1;
	
	    if (paintfc.getPage() > lastVisiblePage)
	      bend = true;
	  }
	
	fc->makeLineLayout(painter);
	
	if (tmpTextPos + 1 <= fc->getLineEndPos())
	  fc->cursorGotoPos(tmpTextPos + 1,painter);
	else
	  {
	    fc->cursorGotoNextLine(painter);
	    fc->cursorGotoPos(tmpTextPos + 1,painter);
	  }
	
	doc->updateAllViews(gui->getView());
      } break;
    default:
      {
	if (!e->text().isEmpty()) //e->ascii() && e->ascii() > 31)
	  {
	    if (has_to_copy) copyBuffer();
	
	    draw_buffer = false;
	    //char tmpString[2] = {0,0};
	    bool isPrev = false;
	    //tmpString[0] = (char)e->ascii();
	    unsigned int tmpTextPos = fc->getTextPos();
	    fc->getParag()->insertText(fc->getTextPos(),e->text());
	    fc->getParag()->setFormat(fc->getTextPos(),e->text().length(),format);
	    fc->makeLineLayout(painter);
	    KWFormatContext paintfc(doc,fc->getFrameSet());

	    if (e->ascii() == ' ' && !fc->isCursorInFirstLine())
	      {
		fc->cursorGotoPrevLine(painter);
		fc->makeLineLayout(painter);
		isPrev = true;
	      }

	    paintfc = *fc;
	    bool bend = false;
	
	    unsigned int currFrameNum = paintfc.getFrame() - 1;
	    unsigned int ptYEnd = fc->getParag()->getPTYEnd();
	    	
	    while (!bend)
	      {
		if (paintfc.getParag() != fc->getParag() && paintfc.getParag() != fc->getParag()->getPrev() &&
		    fc->getParag()->getPTYEnd() == ptYEnd) break;
		if (frameSet->getFrame(currFrameNum)->isMostRight() && frameSet->getNumFrames() > currFrameNum + 1 &&
		    frameSet->getFrame(paintfc.getFrame())->top() - static_cast<int>(yOffset) >
		    static_cast<int>(lastVisiblePage) * static_cast<int>(ptPaperHeight()) &&
		    static_cast<int>(paintfc.getPTY() - yOffset) > height())
 		  break;
		if (frameSet->getFrame(paintfc.getFrame() - 1)->top() - static_cast<int>(yOffset) >
		    static_cast<int>(lastVisiblePage) * static_cast<int>(ptPaperHeight()))
		  break;
		unsigned int _x = frameSet->getFrame(paintfc.getFrame() - 1)->x() - xOffset;
		unsigned int _wid = frameSet->getFrame(paintfc.getFrame() - 1)->width();
		painter.fillRect(_x + frameSet->getFrame(paintfc.getFrame() - 1)->getLeftIndent(paintfc.getPTY(),paintfc.getLineHeight()),
				 paintfc.getPTY() - yOffset,
				 _wid - frameSet->getFrame(paintfc.getFrame() - 1)->getLeftIndent(paintfc.getPTY(),paintfc.getLineHeight()) -
				 frameSet->getFrame(paintfc.getFrame() - 1)->getRightIndent(paintfc.getPTY(),paintfc.getLineHeight()),
				 paintfc.getLineHeight(),
				 doc->getFrameSet(paintfc.getFrameSet() - 1)->getFrame(paintfc.getFrame() - 1)->getBackgroundColor());
		if (doc->printLine(paintfc,painter,xOffset,yOffset,width(),height(),gui->getView()->getViewFormattingChars()))
		  {
		    drawBuffer(KRect(_x + frameSet->getFrame(paintfc.getFrame() - 1)->getLeftIndent(paintfc.getPTY(),paintfc.getLineHeight()),
				     paintfc.getPTY() - yOffset,
				     _wid - frameSet->getFrame(paintfc.getFrame() - 1)->getLeftIndent(paintfc.getPTY(),
												      paintfc.getLineHeight()) -
				     frameSet->getFrame(paintfc.getFrame() - 1)->getRightIndent(paintfc.getPTY(),paintfc.getLineHeight()),
				     paintfc.getLineHeight()));
		  }
		bend = !paintfc.makeNextLineLayout(painter);
		
		if (paintfc.getFrame() - 1 != currFrameNum)
		  currFrameNum = paintfc.getFrame() - 1;
		
		if (paintfc.getPage() > lastVisiblePage)
		  bend = true;
	      }
	
	    if (isPrev)
	      fc->cursorGotoNextLine(painter);

	    fc->makeLineLayout(painter);

	    if (tmpTextPos + e->text().length() <= fc->getLineEndPos())
	      fc->cursorGotoPos(tmpTextPos + e->text().length(),painter);
	    else
	      {
		fc->cursorGotoNextLine(painter);
		fc->cursorGotoPos(tmpTextPos + e->text().length(),painter);
	      }
	
	    QPaintDevice *dev = painter.device();
	    painter.end();

	    if (doc->needRedraw())
	      buffer.fill(white);
	
	    doc->updateAllViews(doc->needRedraw() ? 0L : gui->getView());
	    doc->setNeedRedraw(false);

	    painter.begin(dev);

	  }
      } break;
    }

  if (painter.isActive())
    {
      doc->drawMarker(*fc,&painter,xOffset,yOffset);
      painter.end();
    }

  if (draw_buffer) drawBuffer();

  scrollToCursor(*fc);

  painter.begin(this);
  doc->drawMarker(*fc,&painter,xOffset,yOffset);
  markerIsVisible = true;

  painter.end();

  // HACK
  kbdc.auto_repeat_mode = repeat;
  XChangeKeyboardControl(kapp->getDisplay(),KBAutoRepeatMode,&kbdc);

  if (oldPage != fc->getPage())
    gui->getVertRuler()->setOffset(0,-getVertRulerPos());
  if (oldParag != fc->getParag() && fc->getParag())
    {	
      gui->getView()->updateStyle(fc->getParag()->getParagLayout()->getName());
      setRulerFirstIndent(gui->getHorzRuler(),fc->getParag()->getParagLayout()->getFirstLineLeftIndent());
      setRulerLeftIndent(gui->getHorzRuler(),fc->getParag()->getParagLayout()->getLeftIndent());
      gui->getView()->setFormat(*((KWFormat*)fc));
      gui->getView()->setFlow(fc->getParag()->getParagLayout()->getFlow());
      gui->getView()->setLineSpacing(fc->getParag()->getParagLayout()->getLineSpacing().pt());
      gui->getView()->setParagBorders(fc->getParag()->getParagLayout()->getLeftBorder(),
				      fc->getParag()->getParagLayout()->getRightBorder(),
				      fc->getParag()->getParagLayout()->getTopBorder(),
				      fc->getParag()->getParagLayout()->getBottomBorder());
      gui->getHorzRuler()->setFrameStart(doc->getFrameSet(fc->getFrameSet() - 1)->getFrame(fc->getFrame() - 1)->x());
      gui->getHorzRuler()->setTabList(fc->getParag()->getParagLayout()->getTabList());
    }
  if (doc->getProcessingType() == KWordDocument::DTP && oldFrame != fc->getFrame())
    setRuler2Frame(fc->getFrameSet() - 1,fc->getFrame() - 1);

  inKeyEvent = false;
}

/*================================================================*/
void KWPage::resizeEvent(QResizeEvent *)
{
  buffer.resize(width() < static_cast<int>(doc->getPTPaperWidth() + 20) ? width() : static_cast<int>(doc->getPTPaperWidth() + 20),
		height());
  buffer.fill(white);
  calcVisiblePages();
}

/*================================================================*/
void KWPage::scrollToCursor(KWFormatContext &_fc)
{
  int cy = isCursorYVisible(_fc);
  int cx = isCursorXVisible(_fc);

  if (cx == 0 && cy == 0)
    {
      if (redrawAllWhileScrolling)
	{
	  repaint(false);
	  QPainter painter;
	  painter.begin(this);
	  doc->drawMarker(_fc,&painter,xOffset,yOffset);
	  painter.end();
	}
      return;
    }

  int oy = yOffset, ox = xOffset;
  if (cy < 0)
    {
      oy = _fc.getPTY();
      if (oy < 0) oy = 0;
    }
  else if (cy > 0)
    oy = _fc.getPTY() - height() + _fc.getLineHeight() + 10;

  if (cx < 0)
    {
      ox = _fc.getPTPos() - width() / 3;
      if (ox < 0) ox = 0;
    }
  else if (cx > 0)
    ox = _fc.getPTPos() - width() * 2 / 3;

  scrollToOffset(ox,oy,_fc);
}

/*================================================================*/
void KWPage::scrollToParag(KWParag *_parag)
{
  QPainter p;
  p.begin(this);

  doc->drawMarker(*fc,&p,xOffset,yOffset);
  fc->init(_parag,p,true,true);
  fc->gotoStartOfParag(p);
  fc->cursorGotoLineStart(p);
  p.end();

  scrollToCursor(*fc);

  p.begin(this);
  doc->drawMarker(*fc,&p,xOffset,yOffset);
  p.end();
}

/*================================================================*/
void KWPage::scrollToOffset(int _x,int _y,KWFormatContext &_fc)
{
  QPainter painter;
  painter.begin(this);
  doc->drawMarker(_fc,&painter,xOffset,yOffset);
  painter.end();

  gui->getHorzScrollBar()->setValue(_x);
  gui->getVertScrollBar()->setValue(_y);

  painter.begin(this);
  doc->drawMarker(_fc,&painter,xOffset,yOffset);
  painter.end();
}

/*================================================================*/
void KWPage::scroll(int dx,int dy)
{
  unsigned int ox = xOffset,oy = yOffset;

  xOffset -= dx;
  yOffset -= dy;
  calcVisiblePages();

  if (!redrawAllWhileScrolling)
    {
      int x1,y1,x2,y2,w = width(),h = height();

      if (dx > 0)
	{
	  x1 = 0;
	  x2 = dx;
	  w -= dx;
	}
      else
	{
	  x1 = -dx;
	  x2 = 0;
	  w += dx;
	}

      if (dy > 0)
	{
	  y1 = 0;
	  y2 = dy;
	  h -= dy;
	}
      else
	{
	  y1 = -dy;
	  y2 = 0;
	  h += dy;
	}

      bitBlt(this,x2,y2,this,x1,y1,w,h);

      paint_directly = true;

      if (yOffset > oy)
	{
	  dy = abs(dy);
	  repaint(0,height() - abs(dy),width(),abs(dy),true);
	}
      else if (yOffset < oy)
	{
	  dy = abs(dy);
	  repaint(0,0,width(),abs(dy),true);
	}

      if (xOffset > ox)
	{
	  dx = abs(dx);
	  repaint(width() - abs(dx),0,abs(dx),height(),true);
	}
      else if (xOffset < ox)
	{
	  dx = abs(dx);
	  repaint(0,0,abs(dx),height(),true);
	}

      paint_directly = false;
      has_to_copy = true;
    }
  else
    repaint(false);
}

/*================================================================*/
void KWPage::formatChanged(KWFormat &_format,bool _redraw = true)
{
  format = _format;

  if (doc->has_selection() && !inKeyEvent && _redraw)
    {
      QPainter p;

      p.begin(this);
      doc->drawSelection(p,xOffset,yOffset);
      p.end();

      doc->setSelection(false);
      doc->setFormat(format);
      KWFormatContext fc1(doc,doc->getSelStart()->getFrameSet()),fc2(doc,doc->getSelEnd()->getFrameSet());
      fc1 = *doc->getSelStart();
      fc2 = *doc->getSelEnd();
      recalcCursor(false,-1,&fc1);
      recalcCursor(false,-1,&fc2);
      doc->setSelStart(fc1);
      doc->setSelEnd(fc2);

      recalcCursor();
      doc->setSelection(true);

      p.begin(this);
      doc->drawSelection(p,xOffset,yOffset);
      p.end();
    }
}

/*================================================================*/
int KWPage::isCursorYVisible(KWFormatContext &_fc)
{
  if ((int)_fc.getPTY() - (int)yOffset < 0)
    return -1;

  if (_fc.getPTY() - yOffset + _fc.getLineHeight() > (unsigned int)height())
    return 1;

  return 0;
}

/*================================================================*/
int KWPage::isCursorXVisible(KWFormatContext &_fc)
{
  if ((int)_fc.getPTPos() - (int)xOffset < 0)
    return -1;

  if (_fc.getPTPos() - xOffset + 2 > (unsigned int)width())
    return 1;

  return 0;
}

/*================================================================*/
void KWPage::calcVisiblePages()
{
  firstVisiblePage = 1 + (unsigned int)floor((float)yOffset / (float)ZOOM(ptPaperHeight()));
  lastVisiblePage = (unsigned int)ceil((float)(yOffset + height()) /
				       (float)ZOOM(ptPaperHeight()));
}

/*================================================================*/
void KWPage::drawBuffer()
{
  bitBlt(this,0,0,&buffer,0,0,buffer.width(),buffer.height());
}

/*================================================================*/
void KWPage::drawBuffer(KRect _rect)
{
  bitBlt(this,_rect.x(),_rect.y(),&buffer,_rect.x(),_rect.y(),_rect.width(),_rect.height());
}

/*================================================================*/
void KWPage::copyBuffer()
{
  bitBlt(&buffer,0,0,this,0,0,width(),height());
  has_to_copy = false;
}

/*================================================================*/
void KWPage::drawBorders(QPainter &_painter,KRect v_area)
{
  _painter.save();
  _painter.setBrush(NoBrush);

  KWFrameSet *frameset = 0;
  KWFrame *tmp;
  KRect frame;
  bool should_draw;

  for (unsigned int i = 0;i < doc->getNumFrameSets();i++)
    {
      frameset = doc->getFrameSet(i);
      if (!frameset->isVisible()) continue;

      if (isAHeader(doc->getFrameSet(i)->getFrameInfo()) && !doc->hasHeader() ||
	  isAFooter(doc->getFrameSet(i)->getFrameInfo()) && !doc->hasFooter() ||
	  isAWrongHeader(doc->getFrameSet(i)->getFrameInfo(),doc->getHeaderType()) ||
	  isAWrongFooter(doc->getFrameSet(i)->getFrameInfo(),doc->getFooterType()))
	continue;
      for (unsigned int j = 0;j < frameset->getNumFrames();j++)
	{
	  tmp = frameset->getFrame(j);
	  _painter.setBrush(isAHeader(frameset->getFrameInfo()) || isAFooter(frameset->getFrameInfo()) ?
			    frameset->getFrame(0)->getBackgroundColor() : tmp->getBackgroundColor());
	  _painter.setPen(lightGray);
	  should_draw = true;
	  if (frameset->getGroupManager())
	    {
	      if (gui->getView()->getViewTableGrid())
		_painter.setPen(QPen(black,1,DotLine));
	      else
		_painter.setPen(NoPen);
	    }
	  if (static_cast<int>(i) == hiliteFrameSet)
	    _painter.setPen(blue);
	  else if (!gui->getView()->getViewFrameBorders()) should_draw = false;
	  frame = KRect(tmp->x() - xOffset - 1,tmp->y() - yOffset - 1,tmp->width() + 2,tmp->height() + 2);
	
	  if (v_area.intersects(frame) && should_draw && !frameset->getGroupManager())
	    _painter.drawRect(frame);
	  _painter.setBrush(NoBrush);
	  if (v_area.intersects(frame) && frameset->getGroupManager())
	    {
	      _painter.fillRect(frame,tmp->getBackgroundColor());
	      _painter.drawLine(tmp->right() - xOffset + 1,tmp->y() - yOffset - 1,tmp->right() - xOffset + 1,tmp->bottom() - yOffset + 1);
	      _painter.drawLine(tmp->x() - xOffset - 1,tmp->bottom() - yOffset + 1,tmp->right() - xOffset + 1,tmp->bottom() - yOffset + 1);
	      unsigned int row = 0,col = 0;
	      frameset->getGroupManager()->getFrameSet(frameset,row,col);
	      if (row == 0)
		_painter.drawLine(tmp->x() - xOffset - 1,tmp->y() - yOffset - 1,tmp->right() - xOffset + 1,tmp->y() - yOffset - 1);
	      if (col == 0)
		_painter.drawLine(tmp->x() - xOffset - 1,tmp->y() - yOffset - 1,tmp->x() - xOffset - 1,tmp->bottom() - yOffset + 1);
	    }

	  if (mouseMode == MM_EDIT_FRAME && tmp->isSelected())
	    {
	      _painter.save();
	      _painter.setRasterOp(NotROP);
	      if (!frameset->getGroupManager())
		{
		  _painter.fillRect(frame.x(),frame.y(),6,6,black);
		  _painter.fillRect(frame.x() + frame.width() / 2 - 3,frame.y(),6,6,black);
		  _painter.fillRect(frame.x(),frame.y() + frame.height() / 2 - 3,6,6,black);
		  _painter.fillRect(frame.x() + frame.width() - 6,frame.y(),6,6,black);
		  _painter.fillRect(frame.x(),frame.y() + frame.height() - 6,6,6,black);
		  _painter.fillRect(frame.x() + frame.width() / 2 - 3,frame.y() + frame.height() - 6,6,6,black);
		  _painter.fillRect(frame.x() + frame.width() - 6,frame.y() + frame.height() / 2 - 3,6,6,black);
		  _painter.fillRect(frame.x() + frame.width() - 6,frame.y() + frame.height() - 6,6,6,black);
		  _painter.restore();
		}
	      else
		{
		  _painter.restore();
		  _painter.fillRect(frame.x(),frame.y(),frame.width() - 1,frame.height() - 1,colorGroup().highlight());
		  _painter.fillRect(frame.x() + frame.width() - 6,frame.y() + frame.height() / 2 - 3,6,6,black);
		  _painter.fillRect(frame.x() + frame.width() / 2 - 3,frame.y() + frame.height() - 6,6,6,black);
		}	
	    }
	
	  if (isAHeader(frameset->getFrameInfo()) || isAFooter(frameset->getFrameInfo()))
	    tmp = frameset->getFrame(0);
	  else
	    tmp = frameset->getFrame(j);
	  if (tmp->getLeftBorder().ptWidth > 0 && tmp->getLeftBorder().color != tmp->getBackgroundColor().color())
	    {
 	      QPen p(doc->setBorderPen(tmp->getLeftBorder()));
 	      _painter.setPen(p);
 	      _painter.drawLine(frame.x() + tmp->getLeftBorder().ptWidth / 2,frame.y(),
 				frame.x() + tmp->getLeftBorder().ptWidth / 2,frame.bottom() + 1);
	    }
	  if (tmp->getRightBorder().ptWidth > 0 && tmp->getRightBorder().color != tmp->getBackgroundColor().color())
	    {
 	      QPen p(doc->setBorderPen(tmp->getRightBorder()));
 	      _painter.setPen(p);
	      int w = tmp->getRightBorder().ptWidth;
	      if ((w / 2) * 2 == w) w--;
	      w /= 2;
	      _painter.drawLine(frame.right() - w,frame.y(),
				frame.right() - w,frame.bottom() + 1);
	    }
	  if (tmp->getTopBorder().ptWidth > 0 && tmp->getTopBorder().color != tmp->getBackgroundColor().color())
	    {
 	      QPen p(doc->setBorderPen(tmp->getTopBorder()));
 	      _painter.setPen(p);
	      _painter.drawLine(frame.x(),frame.y() + tmp->getTopBorder().ptWidth / 2,
				frame.right() + 1,
				frame.y() + tmp->getTopBorder().ptWidth / 2);
	    }
	  if (tmp->getBottomBorder().ptWidth > 0 && tmp->getBottomBorder().color != tmp->getBackgroundColor().color())
	    {
	      int w = tmp->getBottomBorder().ptWidth;
	      if ((w / 2) * 2 == w) w--;
	      w /= 2;
 	      QPen p(doc->setBorderPen(tmp->getBottomBorder()));
 	      _painter.setPen(p);
	      _painter.drawLine(frame.x(),frame.bottom() - w,
				frame.right() + 1,
				frame.bottom() - w);
	    }
	}
    }

  _painter.setPen(red);

  for (int k = 0;k < doc->getPages();k++)
    {
      KRect tmp2 = KRect(-xOffset,(k * doc->getPTPaperHeight()) - yOffset,doc->getPTPaperWidth(),doc->getPTPaperHeight());
      if (v_area.intersects(tmp2))
	_painter.drawRect(tmp2);
    }

  _painter.restore();
}

/*================================================================*/
void KWPage::drawFrameSelection(QPainter &_painter,KWFrame *_frame)
{
  _painter.save();
  _painter.setRasterOp(NotROP);

  _painter.fillRect(_frame->x() - xOffset,_frame->y() - yOffset,6,6,black);
  _painter.fillRect(_frame->x() - xOffset + _frame->width() / 2 - 3,_frame->y() - yOffset,6,6,black);
  _painter.fillRect(_frame->x() - xOffset,_frame->y() - yOffset + _frame->height() / 2 - 3,6,6,black);
  _painter.fillRect(_frame->x() - xOffset + _frame->width() - 6,_frame->y() - yOffset,6,6,black);
  _painter.fillRect(_frame->x() - xOffset,_frame->y() - yOffset + _frame->height() - 6,6,6,black);
  _painter.fillRect(_frame->x() - xOffset + _frame->width() / 2 - 3,_frame->y() - yOffset + _frame->height() - 6,6,6,black);
  _painter.fillRect(_frame->x() - xOffset + _frame->width() - 6,_frame->y() - yOffset + _frame->height() / 2 - 3,6,6,black);
  _painter.fillRect(_frame->x() - xOffset + _frame->width() - 6,_frame->y() - yOffset + _frame->height() - 6,6,6,black);

  _painter.restore();
}

/*================================================================*/
void KWPage::frameSizeChanged(KoPageLayout _layout)
{
  setRuler2Frame(fc->getFrameSet() - 1,fc->getFrame() - 1);
  return;

//   if (mouseMode != MM_EDIT_FRAME)
//     {
//       selectedFrame = fc->getFrame() - 1;
//       selectedFrameSet = fc->getFrameSet() - 1;
//     }
//   else if (selectedFrame == -1 || selectedFrameSet == -1)
//     return;

//   gui->getHorzRuler()->setFrameStart(doc->getFrameSet(selectedFrameSet)->getFrame(selectedFrame)->x());

//   if (doc->getProcessingType() != KWordDocument::DTP) return;

//   KWFrame *frame = doc->getFrameSet(selectedFrameSet)->getFrame(selectedFrame);

//   unsigned int page = 0;
//   for (int i = 0;i < doc->getPages();i++)
//     {
//       if (frame->intersects(KRect(0,i * ptPaperHeight(),ptPaperWidth(),ptPaperHeight())))
// 	{
// 	  page = i;
// 	  break;
// 	}
//     }

//   doc->getFrameSet(selectedFrameSet)->getFrame(selectedFrame)->setCoords(_layout.ptLeft,
// 									 _layout.ptTop + page * ptPaperHeight(),
// 									 _layout.ptWidth - _layout.ptRight,
// 									 _layout.ptHeight - _layout.ptBottom
// 									 + page * ptPaperHeight());
//   doc->updateAllFrames();
//   recalcText();
//   recalcCursor();
}

/*================================================================*/
void KWPage::setRuler2Frame(unsigned int _frameset,unsigned int _frame)
{
  if (doc->getProcessingType() != KWordDocument::DTP) return;

  KoPageLayout _layout;
  KoColumns _cl;
  KoKWHeaderFooter hf;
  doc->getPageLayout(_layout,_cl,hf);
  KWFrame *frame = doc->getFrameSet(_frameset)->getFrame(_frame);

  unsigned int page = 0;
  for (int i = 0;i < doc->getPages();i++)
    {
      if (frame->intersects(KRect(0,i * ptPaperHeight(),ptPaperWidth(),ptPaperHeight())))
	{
	  page = i;
	  break;
	}
    }

  _layout.left = _layout.mmLeft = POINT_TO_MM(frame->left());
  _layout.top = _layout.mmTop = POINT_TO_MM(frame->top()) - page * doc->getMMPaperHeight();
  _layout.right = _layout.mmRight = _layout.mmWidth - POINT_TO_MM(frame->right());
  _layout.bottom = _layout.mmBottom = _layout.mmHeight - POINT_TO_MM(frame->bottom()) + page * doc->getMMPaperHeight();
  _layout.ptLeft = frame->left();
  _layout.inchLeft = POINT_TO_INCH(frame->left());
  _layout.inchTop = POINT_TO_INCH(frame->top()) - page * doc->getINCHPaperHeight();
  _layout.inchRight = _layout.inchWidth - POINT_TO_INCH(frame->right());
  _layout.inchBottom = _layout.inchHeight - POINT_TO_INCH(frame->bottom()) + page * doc->getINCHPaperHeight();
  _layout.ptLeft = frame->left();
  _layout.ptTop = frame->top() - page * ptPaperHeight();
  _layout.ptRight = _layout.ptWidth - frame->right();
  _layout.ptBottom = _layout.ptHeight - frame->bottom() + page * ptPaperHeight();
  gui->getHorzRuler()->setPageLayout(_layout);
  gui->getVertRuler()->setPageLayout(_layout);
  gui->getHorzRuler()->setFrameStart(doc->getFrameSet(fc->getFrameSet() - 1)->getFrame(fc->getFrame() - 1)->x());
}

/*================================================================*/
void KWPage::setMouseMode(MouseMode _mm)
{
  if (editNum != -1)
    {
      if (doc->getFrameSet(editNum)->getFrameType() == FT_PART)
	{
	  dynamic_cast<KWPartFrameSet*>(doc->getFrameSet(editNum))->deactivate();
	  //setFocusProxy(0);
	  //setFocusPolicy(QWidget::NoFocus);
	}
    }

  mouseMode = _mm;
  mmUncheckAll();
  gui->getView()->uncheckAllTools();
  gui->getView()->setTool(mouseMode);

  switch (mouseMode)
    {
    case MM_EDIT:
      {
	setCursor(ibeamCursor);
	mm_menu->setItemChecked(mm_edit,true);
	if (!inKeyEvent)
	  {
	    setRulerFirstIndent(gui->getHorzRuler(),fc->getParag()->getParagLayout()->getFirstLineLeftIndent());
	    setRulerLeftIndent(gui->getHorzRuler(),fc->getParag()->getParagLayout()->getLeftIndent());
	    gui->getHorzRuler()->setFrameStart(doc->getFrameSet(fc->getFrameSet() - 1)->getFrame(fc->getFrame() - 1)->x());
	    gui->getHorzRuler()->setTabList(fc->getParag()->getParagLayout()->getTabList());
	    if (doc->getProcessingType() == KWordDocument::DTP)
	      setRuler2Frame(fc->getFrameSet() - 1,fc->getFrame() - 1);
	  }
      } break;
    case MM_EDIT_FRAME:
      {
	setCursor(arrowCursor);
	mm_menu->setItemChecked(mm_edit_frame,true);
      } break;
    case MM_CREATE_TEXT:
      {
	setCursor(crossCursor);
	mm_menu->setItemChecked(mm_create_text,true);
      } break;
    case MM_CREATE_PIX:
      {
	setCursor(crossCursor);
	mm_menu->setItemChecked(mm_create_pix,true);
      } break;
    case MM_CREATE_CLIPART:
      {
	setCursor(crossCursor);
	mm_menu->setItemChecked(mm_create_clipart,true);
      } break;
    case MM_CREATE_TABLE:
      {
	setCursor(crossCursor);
	mm_menu->setItemChecked(mm_create_table,true);
      } break;
    case MM_CREATE_KSPREAD_TABLE:
      {
	setCursor(crossCursor);
	mm_menu->setItemChecked(mm_create_kspread_table,true);
      } break;
    case MM_CREATE_FORMULA:
      {
	setCursor(crossCursor);
	mm_menu->setItemChecked(mm_create_formula,true);
      } break;
    case MM_CREATE_PART:
      {
	setCursor(crossCursor);
	mm_menu->setItemChecked(mm_create_part,true);
      } break;
    }

  repaint(false);
}

/*================================================================*/
void KWPage::setupMenus()
{
  QString pixdir;
  QPixmap pixmap;

  mm_menu = new QPopupMenu();
  CHECK_PTR(mm_menu);
  mm_edit = mm_menu->insertItem(i18n("Edit"),this,SLOT(mmEdit()));
  mm_edit_frame = mm_menu->insertItem(i18n("Edit Frames"),this,SLOT(mmEditFrame()));
  mm_create_text = mm_menu->insertItem(i18n("Create Text-Frame"),this,SLOT(mmCreateText()));
  mm_create_pix = mm_menu->insertItem(i18n("Create Pixmap-Frame"),this,SLOT(mmCreatePix()));
  mm_create_clipart = mm_menu->insertItem(i18n("Create Clipart-Frame"),this,SLOT(mmClipart()));
  mm_create_table = mm_menu->insertItem(i18n("Create Table-Frame"),this,SLOT(mmTable()));
  mm_create_kspread_table = mm_menu->insertItem(i18n("Create KSpread Table-Frame"),this,SLOT(mmKSpreadTable()));
  mm_create_formula = mm_menu->insertItem(i18n("Create Formula-Frame"),this,SLOT(mmFormula()));
  mm_create_part = mm_menu->insertItem(i18n("Create Part-Frame"),this,SLOT(mmPart()));
  mm_menu->setCheckable(true);

  frame_edit_menu = new QPopupMenu();
  CHECK_PTR(frame_edit_menu);
  frame_edit_menu->insertItem(i18n("Properties..."),this,SLOT(femProps()));
}

/*================================================================*/
void KWPage::mmUncheckAll()
{
  mm_menu->setItemChecked(mm_edit,false);
  mm_menu->setItemChecked(mm_edit_frame,false);
  mm_menu->setItemChecked(mm_create_text,false);
  mm_menu->setItemChecked(mm_create_pix,false);
  mm_menu->setItemChecked(mm_create_clipart,false);
  mm_menu->setItemChecked(mm_create_table,false);
  mm_menu->setItemChecked(mm_create_kspread_table,false);
  mm_menu->setItemChecked(mm_create_formula,false);
  mm_menu->setItemChecked(mm_create_part,false);
}

/*================================================================*/
int KWPage::getPageOfRect(KRect _rect)
{
  for (int i = 0;i < doc->getPages();i++)
    {
      if (_rect.intersects(KRect(0,i * ptPaperHeight(),ptPaperWidth(),ptPaperHeight())))
	return i;
    }

  return -1;
}

/*================================================================*/
void KWPage::femProps()
{
  if (frameDia)
    {
      frameDia->close();
      disconnect(frameDia,SIGNAL(frameDiaClosed()),this,SLOT(frameDiaClosed()));
      disconnect(frameDia,SIGNAL(applyButtonPressed()),this,SLOT(frameDiaClosed()));
      disconnect(frameDia,SIGNAL(cancelButtonPressed()),this,SLOT(frameDiaClosed()));
      disconnect(frameDia,SIGNAL(defaultButtonPressed()),this,SLOT(frameDiaClosed()));
      delete frameDia;
      frameDia = 0;
    }

  repaint(false);
  frameDia = new KWFrameDia(0,"",0L,doc,this,FD_FRAME_SET | FD_FRAME | FD_GEOMETRY | FD_BORDERS);
  connect(frameDia,SIGNAL(frameDiaClosed()),this,SLOT(frameDiaClosed()));
  connect(frameDia,SIGNAL(applyButtonPressed()),this,SLOT(frameDiaClosed()));
  connect(frameDia,SIGNAL(cancelButtonPressed()),this,SLOT(frameDiaClosed()));
  connect(frameDia,SIGNAL(defaultButtonPressed()),this,SLOT(frameDiaClosed()));
  frameDia->setCaption(i18n("KWord - Frame settings"));
  frameDia->show();
}

/*================================================================*/
void KWPage::newLeftIndent(int _left)
{
  KWUnit u;
  u.setPT(_left);
  setLeftIndent(u);

  switch (KWUnit::unitType(doc->getUnit()))
    {
    case U_MM:
      gui->getHorzRuler()->setLeftIndent(fc->getParag()->getParagLayout()->getLeftIndent().mm());
      break;
    case U_INCH:
      gui->getHorzRuler()->setLeftIndent(fc->getParag()->getParagLayout()->getLeftIndent().inch());
      break;
    case U_PT:
      gui->getHorzRuler()->setLeftIndent(fc->getParag()->getParagLayout()->getLeftIndent().pt());
      break;
    }
}

/*================================================================*/
void KWPage::newFirstIndent(int _first)
{
  KWUnit u;
  u.setPT(_first);
  setFirstLineIndent(u);

  switch (KWUnit::unitType(doc->getUnit()))
    {
    case U_MM:
      gui->getHorzRuler()->setFirstIndent(fc->getParag()->getParagLayout()->getFirstLineLeftIndent().mm());
      break;
    case U_INCH:
      gui->getHorzRuler()->setFirstIndent(fc->getParag()->getParagLayout()->getFirstLineLeftIndent().inch());
      break;
    case U_PT:
      gui->getHorzRuler()->setFirstIndent(fc->getParag()->getParagLayout()->getFirstLineLeftIndent().pt());
      break;
    }
}

/*================================================================*/
void KWPage::frameDiaClosed()
{
  hiliteFrameSet = -1;
  recalcAll = true;
  recalcText();
  recalcCursor();
  recalcAll = false;
}

/*================================================================*/
void KWPage::applyStyle(QString _style)
{
  if (!doc->has_selection())
    fc->getParag()->applyStyle(_style);
  else
    {
      KWParag *p = doc->getSelStart()->getParag();
      while (p && p != doc->getSelEnd()->getParag()->getNext())
	{
	  p->applyStyle(_style);
	  p = p->getNext();
	}
      recalcCursor(false,-1,doc->getSelStart());
      recalcCursor(false,-1,doc->getSelEnd());
    }

  dynamic_cast<KWTextFrameSet*>(doc->getFrameSet(fc->getFrameSet() - 1))->updateCounters();
  recalcAll = true;
  recalcText();
  recalcCursor();
  recalcAll = false;
}
/*================================================================*/
void KWPage::setCounter(KWParagLayout::Counter _counter)
{
  fc->getParag()->getParagLayout()->setCounter(_counter);
  dynamic_cast<KWTextFrameSet*>(doc->getFrameSet(fc->getFrameSet() - 1))->updateCounters();
  recalcAll = true;
  recalcText();
  recalcCursor();
  recalcAll = false;
}
/*================================================================*/
void KWPage::setEnumList()
{
  int f = doc->getApplyStyleTemplate();
  doc->setApplyStyleTemplate(KWordDocument::U_NUMBERING);

  applyStyle("Enumerated List");

  doc->setApplyStyleTemplate(f);
  gui->getView()->updateStyle(fc->getParag()->getParagLayout()->getName());
  gui->getView()->setFormat(*((KWFormat*)fc));
  gui->getView()->setFlow(fc->getParag()->getParagLayout()->getFlow());
  gui->getView()->setLineSpacing(fc->getParag()->getParagLayout()->getLineSpacing().pt());
  gui->getView()->setParagBorders(fc->getParag()->getParagLayout()->getLeftBorder(),
				  fc->getParag()->getParagLayout()->getRightBorder(),
				  fc->getParag()->getParagLayout()->getTopBorder(),
				  fc->getParag()->getParagLayout()->getBottomBorder());
}

/*================================================================*/
void KWPage::setBulletList()
{
  int f = doc->getApplyStyleTemplate();
  doc->setApplyStyleTemplate(KWordDocument::U_NUMBERING);

  applyStyle("Bullet List");

  doc->setApplyStyleTemplate(f);
  gui->getView()->updateStyle(fc->getParag()->getParagLayout()->getName());
  gui->getView()->setFormat(*((KWFormat*)fc));
  gui->getView()->setFlow(fc->getParag()->getParagLayout()->getFlow());
  gui->getView()->setLineSpacing(fc->getParag()->getParagLayout()->getLineSpacing().pt());
  gui->getView()->setParagBorders(fc->getParag()->getParagLayout()->getLeftBorder(),
				  fc->getParag()->getParagLayout()->getRightBorder(),
				  fc->getParag()->getParagLayout()->getTopBorder(),
				  fc->getParag()->getParagLayout()->getBottomBorder());
}

/*================================================================*/
void KWPage::setNormalText()
{
  int f = doc->getApplyStyleTemplate();
  doc->setApplyStyleTemplate(KWordDocument::U_NUMBERING);

  applyStyle("Standard");

  doc->setApplyStyleTemplate(f);
  gui->getView()->updateStyle(fc->getParag()->getParagLayout()->getName());
  gui->getView()->setFormat(*((KWFormat*)fc));
  gui->getView()->setFlow(fc->getParag()->getParagLayout()->getFlow());
  gui->getView()->setLineSpacing(fc->getParag()->getParagLayout()->getLineSpacing().pt());
  gui->getView()->setParagBorders(fc->getParag()->getParagLayout()->getLeftBorder(),
				  fc->getParag()->getParagLayout()->getRightBorder(),
				  fc->getParag()->getParagLayout()->getTopBorder(),
				  fc->getParag()->getParagLayout()->getBottomBorder());
}

/*================================================================*/
void KWPage::forceFullUpdate()
{
  gui->getView()->updateStyle(fc->getParag()->getParagLayout()->getName(),false);
  gui->getView()->setFormat(*((KWFormat*)fc),true,false);
  gui->getView()->setFlow(fc->getParag()->getParagLayout()->getFlow());
  gui->getView()->setLineSpacing(fc->getParag()->getParagLayout()->getLineSpacing().pt());
  gui->getView()->setParagBorders(fc->getParag()->getParagLayout()->getLeftBorder(),
				  fc->getParag()->getParagLayout()->getRightBorder(),
				  fc->getParag()->getParagLayout()->getTopBorder(),
				  fc->getParag()->getParagLayout()->getBottomBorder());
  setRulerFirstIndent(gui->getHorzRuler(),fc->getParag()->getParagLayout()->getFirstLineLeftIndent());
  setRulerLeftIndent(gui->getHorzRuler(),fc->getParag()->getParagLayout()->getLeftIndent());
  gui->getHorzRuler()->setFrameStart(doc->getFrameSet(fc->getFrameSet() - 1)->getFrame(fc->getFrame() - 1)->x());
  gui->getHorzRuler()->setTabList(fc->getParag()->getParagLayout()->getTabList());
  format = *((KWFormat*)fc);
}
/*================================================================*/
void KWPage::setFlow(KWParagLayout::Flow _flow)
{
  if (!doc->has_selection())
    fc->getParag()->getParagLayout()->setFlow(_flow);
  else
    {
      KWParag *p = doc->getSelStart()->getParag();
      while (p && p != doc->getSelEnd()->getParag()->getNext())
	{
	  p->getParagLayout()->setFlow(_flow);
	  p = p->getNext();
	}
      recalcCursor(false,-1,doc->getSelStart());
      recalcCursor(false,-1,doc->getSelEnd());
    }
  recalcCursor();
}

/*================================================================*/
void KWPage::setLeftIndent(KWUnit _left)
{
  if (!doc->has_selection())
    fc->getParag()->getParagLayout()->setLeftIndent(_left);
  else
    {
      KWParag *p = doc->getSelStart()->getParag();
      while (p && p != doc->getSelEnd()->getParag()->getNext())
	{
	  p->getParagLayout()->setLeftIndent(_left);
	  p = p->getNext();
	}
      recalcCursor(false,-1,doc->getSelStart());
      recalcCursor(false,-1,doc->getSelEnd());
    }
  recalcCursor();
}

/*================================================================*/
void KWPage::setFirstLineIndent(KWUnit _first)
{
  if (!doc->has_selection())
    fc->getParag()->getParagLayout()->setFirstLineLeftIndent(_first);
  else
    {
      KWParag *p = doc->getSelStart()->getParag();
      while (p && p != doc->getSelEnd()->getParag()->getNext())
	{
	  p->getParagLayout()->setFirstLineLeftIndent(_first);
	  p = p->getNext();
	}
      recalcCursor(false,-1,doc->getSelStart());
      recalcCursor(false,-1,doc->getSelEnd());
    }
  recalcCursor();
}

/*================================================================*/
void KWPage::setSpaceBeforeParag(KWUnit _before)
{
  recalcAll = true;
  if (!doc->has_selection())
    fc->getParag()->getParagLayout()->setParagHeadOffset(_before);
  else
    {
      KWParag *p = doc->getSelStart()->getParag();
      while (p && p != doc->getSelEnd()->getParag()->getNext())
	{
	  p->getParagLayout()->setParagHeadOffset(_before);
	  p = p->getNext();
	}
      recalcCursor(false,-1,doc->getSelStart());
      recalcCursor(false,-1,doc->getSelEnd());
    }
  recalcCursor();
  recalcAll = false;
}

/*================================================================*/
void KWPage::setSpaceAfterParag(KWUnit _after)
{
  recalcAll = true;
  if (!doc->has_selection())
    fc->getParag()->getParagLayout()->setParagFootOffset(_after);
  else
    {
      KWParag *p = doc->getSelStart()->getParag();
      while (p && p != doc->getSelEnd()->getParag()->getNext())
	{
	  p->getParagLayout()->setParagFootOffset(_after);
	  p = p->getNext();
	}
      recalcCursor(false,-1,doc->getSelStart());
      recalcCursor(false,-1,doc->getSelEnd());
    }
  recalcCursor();
  recalcAll = false;
}

/*================================================================*/
void KWPage::setLineSpacing(KWUnit _spacing)
{
  recalcAll = true;
  if (!doc->has_selection())
    fc->getParag()->getParagLayout()->setLineSpacing(_spacing);
  else
    {
      KWParag *p = doc->getSelStart()->getParag();
      while (p && p != doc->getSelEnd()->getParag()->getNext())
	{
	  p->getParagLayout()->setLineSpacing(_spacing);
	  p = p->getNext();
	}
      recalcCursor(false,-1,doc->getSelStart());
      recalcCursor(false,-1,doc->getSelEnd());
    }
  recalcCursor();
  recalcAll = false;
}

/*================================================================*/
void KWPage::setParagLeftBorder(KWParagLayout::Border _brd)
{
  if (!doc->has_selection())
    fc->getParag()->getParagLayout()->setLeftBorder(_brd);
  else
    {
      KWParag *p = doc->getSelStart()->getParag();
      while (p && p != doc->getSelEnd()->getParag()->getNext())
	{
	  p->getParagLayout()->setLeftBorder(_brd);
	  p = p->getNext();
	}
      recalcCursor(false,-1,doc->getSelStart());
      recalcCursor(false,-1,doc->getSelEnd());
    }
  recalcCursor();
}

/*================================================================*/
void KWPage::setParagRightBorder(KWParagLayout::Border _brd)
{
  if (!doc->has_selection())
    fc->getParag()->getParagLayout()->setRightBorder(_brd);
  else
    {
      KWParag *p = doc->getSelStart()->getParag();
      while (p && p != doc->getSelEnd()->getParag()->getNext())
	{
	  p->getParagLayout()->setRightBorder(_brd);
	  p = p->getNext();
	}
      recalcCursor(false,-1,doc->getSelStart());
      recalcCursor(false,-1,doc->getSelEnd());
    }
  recalcCursor();
}

/*================================================================*/
void KWPage::setParagTopBorder(KWParagLayout::Border _brd)
{
  if (!doc->has_selection())
    fc->getParag()->getParagLayout()->setTopBorder(_brd);
  else
    {
      KWParag *p = doc->getSelStart()->getParag();
      while (p && p != doc->getSelEnd()->getParag()->getNext())
	{
	  p->getParagLayout()->setTopBorder(_brd);
	  p = p->getNext();
	}
      recalcCursor(false,-1,doc->getSelStart());
      recalcCursor(false,-1,doc->getSelEnd());
    }
  recalcCursor();
}

/*================================================================*/
void KWPage::setParagBottomBorder(KWParagLayout::Border _brd)
{
  if (!doc->has_selection())
    fc->getParag()->getParagLayout()->setBottomBorder(_brd);
  else
    {
      KWParag *p = doc->getSelStart()->getParag();
      while (p && p != doc->getSelEnd()->getParag()->getNext())
	{
	  p->getParagLayout()->setBottomBorder(_brd);
	  p = p->getNext();
	}
      recalcCursor(false,-1,doc->getSelStart());
      recalcCursor(false,-1,doc->getSelEnd());
    }
  recalcCursor();
}

/*================================================================*/
void KWPage::tabListChanged(QList<KoTabulator> *_tablist)
{
  gui->getHorzRuler()->setFrameStart(doc->getFrameSet(fc->getFrameSet() - 1)->getFrame(fc->getFrame() - 1)->x());
  if (!doc->has_selection())
    fc->getParag()->tabListChanged(_tablist);
  else
    {
      KWParag *p = doc->getSelStart()->getParag();
      while (p && p != doc->getSelEnd()->getParag()->getNext())
	{
	  p->tabListChanged(_tablist);
	  p = p->getNext();
	}
      recalcCursor(false,-1,doc->getSelStart());
      recalcCursor(false,-1,doc->getSelEnd());
    }
  recalcCursor();
}

/*================================================================*/
bool KWPage::find(QString _expr,KWSearchDia::KWSearchEntry *_format,bool _first,bool _cs,bool _whole,
		  bool _regexp,bool _wildcard,bool &_addlen,bool _select = true)
{
  if (_first || !currFindParag)
    {
      for (unsigned int i = 0;i < doc->getNumFrameSets();i++)
	{
	  if (doc->getFrameSet(i)->getFrameType() == FT_TEXT && doc->getFrameSet(i)->getFrameInfo() == FI_BODY)
	    {
	      currFindParag = dynamic_cast<KWTextFrameSet*>(doc->getFrameSet(i))->getFirstParag();
	      currFindPos = 0;
	      currFindFS = i;
	      currFindLen = 0;
	      break;
	    }
	}
    }

  while (true)
    {
      int tmpFindPos = currFindPos;
      if (!_regexp)
	{
	  currFindPos = currFindParag->find(_expr,_format,currFindPos,_cs,_whole);
	  currFindLen = _expr.length();
	}
      else
	currFindPos = currFindParag->find(QRegExp(_expr),_format,currFindPos,currFindLen,_cs,_wildcard);

      if (currFindPos >= 0)
	{
	  selectText(currFindPos,currFindLen,currFindFS,dynamic_cast<KWTextFrameSet*>(doc->getFrameSet(currFindFS)),currFindParag,_select);
	  if (_addlen)
	    currFindPos += currFindLen;
	  _addlen = !_addlen;
	  return true;
	}
      else
	{
	  if (currFindPos == -2 && tmpFindPos + currFindLen < static_cast<int>(currFindParag->getTextLen()))
	    currFindPos = tmpFindPos + currFindLen;
	  else if (currFindParag->getNext())
	    {
	      currFindParag = currFindParag->getNext();
	      currFindPos = 0;
	    }
	  else if (!currFindParag->getNext() && currFindFS <= static_cast<int>(doc->getNumFrameSets()))
	    {
	      currFindPos = -1;
	      for (unsigned int i = currFindFS + 1;i < doc->getNumFrameSets();i++)
		{
		  if (doc->getFrameSet(i)->getFrameType() == FT_TEXT && doc->getFrameSet(i)->getFrameInfo() == FI_BODY)
		    {
		      currFindParag = dynamic_cast<KWTextFrameSet*>(doc->getFrameSet(i))->getFirstParag();
		      currFindPos = 0;
		      currFindFS = i;
		      currFindLen = 0;
		      break;
		    }
		}
	      if (currFindPos == -1)
		return false;
	    }
	  else
	    return false;
	}
    }
}

/*================================================================*/
bool KWPage::findRev(QString _expr,KWSearchDia::KWSearchEntry *_format,bool _first,bool _cs,bool _whole,
		     bool _regexp,bool _wildcard,bool &_addlen,bool _select = true)
{
  _addlen = false;
  if (_first || !currFindParag)
    {
      for (unsigned int i = doc->getNumFrameSets() - 1;i >= 0;i--)
	{
	  if (doc->getFrameSet(i)->getFrameType() == FT_TEXT && doc->getFrameSet(i)->getFrameInfo() == FI_BODY)
	    {
	      currFindParag = dynamic_cast<KWTextFrameSet*>(doc->getFrameSet(i))->getLastParag();
	      currFindPos = currFindParag->getTextLen() - 1;
	      currFindFS = i;
	      currFindLen = 0;
	      break;
	    }
	}
    }

  while (true)
    {
      int tmpFindPos = currFindPos;
      if (!_regexp)
	{
	  currFindPos = currFindParag->findRev(_expr,_format,currFindPos,_cs,_whole);
	  currFindLen = _expr.length();
	}
      else
	currFindPos = currFindParag->findRev(QRegExp(_expr),_format,currFindPos,currFindLen,_cs,_wildcard);

      if (currFindPos >= 0)
	{
	  selectText(currFindPos,currFindLen,currFindFS,dynamic_cast<KWTextFrameSet*>(doc->getFrameSet(currFindFS)),currFindParag,_select);
	  if (currFindPos > 0) currFindPos--;
	  return true;
	}
      else
	{
	  if (currFindPos == -2 && tmpFindPos - currFindLen > 0)
	    currFindPos = tmpFindPos - currFindLen;
	  else if (currFindParag->getPrev())
	    {
	      currFindParag = currFindParag->getPrev();
	      currFindPos = currFindParag->getTextLen() - 1;
	    }
	  else if (!currFindParag->getPrev() && currFindFS <= static_cast<int>(doc->getNumFrameSets()))
	    {
	      currFindPos = -1;
	      if (currFindFS > 0)
		{
		  for (unsigned int i = currFindFS - 1;i >= 0;i--)
		    {
		      if (doc->getFrameSet(i)->getFrameType() == FT_TEXT && doc->getFrameSet(i)->getFrameInfo() == FI_BODY)
			{
			  currFindParag = dynamic_cast<KWTextFrameSet*>(doc->getFrameSet(i))->getLastParag();
			  currFindPos = currFindParag->getTextLen() - 1;
			  currFindFS = i;
			  currFindLen = 0;
			  break;
			}
		    }
		}
	      if (currFindPos == -1)
		return false;
	    }
	  else
	    return false;
	}
    }
}

/*================================================================*/
void KWPage::replace(QString _expr,KWSearchDia::KWSearchEntry *_format,bool _addlen)
{
  QPainter p;
  p.begin(this);

  KWFormat *format = new KWFormat(doc);
  *format = *(dynamic_cast<KWCharFormat*>(currFindParag->getKWString()->data()[fc->getTextPos()].attrib)->getFormat());

  if (_format->checkFamily && _format->family != format->getUserFont()->getFontName())
    format->setUserFont(doc->findUserFont(_format->family));
  if (_format->checkColor && _format->color != format->getColor())
    format->setColor(_format->color);
  if (_format->checkSize && _format->size != format->getPTFontSize())
    format->setPTFontSize(_format->size);
  if (_format->checkBold && _format->bold != (format->getWeight() == QFont::Bold))
    format->setWeight(_format->bold ? QFont::Bold : QFont::Normal);
  if (_format->checkItalic && _format->italic != format->getItalic())
    format->setItalic(_format->italic);
  if (_format->checkUnderline && _format->underline != format->getUnderline())
    format->setUnderline(_format->underline);
  if (_format->checkVertAlign && _format->vertAlign != format->getVertAlign())
    format->setVertAlign(_format->vertAlign);

  doc->deleteSelectedText(fc,p);
  p.end();

  // NOTE: KWordDoc::paste() deletes the format, so it _MUST_NOT_ be deleted here!
  doc->paste(fc,_expr,this,format);

  if (_addlen) currFindPos += currFindLen;

  buffer.fill(white);
  doc->setSelection(false);
  recalcText();
  recalcCursor();
}

/*================================================================*/
void KWPage::selectText(int _pos,int _len,int _frameSetNum,KWTextFrameSet *_frameset,KWParag *_parag,bool _select = true)
{
  removeSelection();

  QPainter p;
  p.begin(this);

  doc->drawMarker(*fc,&p,xOffset,yOffset);

  KWFormatContext fc1(doc,_frameSetNum + 1);
  KWFormatContext fc2(doc,_frameSetNum + 1);

  fc1.init(_parag,p,true,true);
  fc2.init(_parag,p,true,true);

  fc1.gotoStartOfParag(p);
  fc1.cursorGotoLineStart(p);
  fc2.gotoStartOfParag(p);
  fc2.cursorGotoLineStart(p);
  fc1.cursorGotoRight(p,_pos);
  fc2 = fc1;
  fc2.cursorGotoRight(p,_len);
  *fc = fc1;

  doc->setSelStart(fc1);
  doc->setSelEnd(fc2);

  if (_select)
    {
      doc->setSelection(true);
      doc->drawSelection(p,xOffset,yOffset);
    }

  p.end();
  scrollToCursor(*fc);
  p.begin(this);

  doc->drawMarker(*fc,&p,xOffset,yOffset);

  p.end();
}

/*================================================================*/
void KWPage::removeSelection()
{
  QPainter p;
  p.begin(this);

  if (doc->has_selection())
    {
      doc->drawSelection(p,xOffset,yOffset);
      doc->setSelection(false);
    }

  p.end();
}

/*================================================================*/
void KWPage::setLeftFrameBorder(KWParagLayout::Border _brd,bool _enable)
{
  KWFrameSet *frameset = 0L;
  KWFrame *frame = 0L;
  QList<KWGroupManager> grpMgrs;
  grpMgrs.setAutoDelete(false);

  for (unsigned int i = 0;i < doc->getNumFrameSets();i++)
    {
      frameset = doc->getFrameSet(i);
      for (unsigned int j = 0;j < frameset->getNumFrames();j++)
	{
	  frame = frameset->getFrame(j);
	  if (frame->isSelected())
	    {
	      if (!_enable)
		{
		  _brd.ptWidth = 1;
		  _brd.color = frame->getBackgroundColor().color();
		}
	      frame->setLeftBorder(_brd);
	      if (frameset->getGroupManager() && grpMgrs.findRef(frameset->getGroupManager()) == -1)
		grpMgrs.append(frameset->getGroupManager());
	    }
	}
    }

  doc->updateTableHeaders(grpMgrs);
  doc->updateAllViews(0L);
}

/*================================================================*/
void KWPage::setRightFrameBorder(KWParagLayout::Border _brd,bool _enable)
{
  KWFrameSet *frameset = 0L;
  KWFrame *frame = 0L;
  QList<KWGroupManager> grpMgrs;
  grpMgrs.setAutoDelete(false);

  for (unsigned int i = 0;i < doc->getNumFrameSets();i++)
    {
      frameset = doc->getFrameSet(i);
      for (unsigned int j = 0;j < frameset->getNumFrames();j++)
	{
	  frame = frameset->getFrame(j);
	  if (frame->isSelected())
	    {
	      if (!_enable)
		{
		  _brd.ptWidth = 1;
		  _brd.color = frame->getBackgroundColor().color();
		}
	      frame->setRightBorder(_brd);
	      if (frameset->getGroupManager() && grpMgrs.findRef(frameset->getGroupManager()) == -1)
		grpMgrs.append(frameset->getGroupManager());
	    }
	}
    }

  doc->updateTableHeaders(grpMgrs);
  doc->updateAllViews(0L);
}

/*================================================================*/
void KWPage::setTopFrameBorder(KWParagLayout::Border _brd,bool _enable)
{
  KWFrameSet *frameset = 0L;
  KWFrame *frame = 0L;
  QList<KWGroupManager> grpMgrs;
  grpMgrs.setAutoDelete(false);

  for (unsigned int i = 0;i < doc->getNumFrameSets();i++)
    {
      frameset = doc->getFrameSet(i);
      for (unsigned int j = 0;j < frameset->getNumFrames();j++)
	{
	  frame = frameset->getFrame(j);
	  if (frame->isSelected())
	    {
	      if (!_enable)
		{
		  _brd.ptWidth = 1;
		  _brd.color = frame->getBackgroundColor().color();
		}
	      frame->setTopBorder(_brd);
	      if (frameset->getGroupManager() && grpMgrs.findRef(frameset->getGroupManager()) == -1)
		grpMgrs.append(frameset->getGroupManager());
	    }
	}
    }

  doc->updateTableHeaders(grpMgrs);
  doc->updateAllViews(0L);
}

/*================================================================*/
void KWPage::setBottomFrameBorder(KWParagLayout::Border _brd,bool _enable)
{
  KWFrameSet *frameset = 0L;
  KWFrame *frame = 0L;
  QList<KWGroupManager> grpMgrs;
  grpMgrs.setAutoDelete(false);

  for (unsigned int i = 0;i < doc->getNumFrameSets();i++)
    {
      frameset = doc->getFrameSet(i);
      for (unsigned int j = 0;j < frameset->getNumFrames();j++)
	{
	  frame = frameset->getFrame(j);
	  if (frame->isSelected())
	    {
	      if (!_enable)
		{
		  _brd.ptWidth = 1;
		  _brd.color = frame->getBackgroundColor().color();
		}
	      frame->setBottomBorder(_brd);
	      if (frameset->getGroupManager() && grpMgrs.findRef(frameset->getGroupManager()) == -1)
		grpMgrs.append(frameset->getGroupManager());
	    }
	}
    }

  doc->updateTableHeaders(grpMgrs);
  doc->updateAllViews(0L);
}

/*================================================================*/
void KWPage::setFrameBackgroundColor(QBrush _color)
{
  KWFrameSet *frameset = 0L;
  KWFrame *frame = 0L;
  QList<KWGroupManager> grpMgrs;
  grpMgrs.setAutoDelete(false);

  for (unsigned int i = 0;i < doc->getNumFrameSets();i++)
    {
      frameset = doc->getFrameSet(i);
      for (unsigned int j = 0;j < frameset->getNumFrames();j++)
	{
	  frame = frameset->getFrame(j);
	  if (frame->isSelected())
	    {
	      if (frame->getLeftBorder().color == frame->getBackgroundColor().color())
		frame->getLeftBorder().color = _color.color();
	      if (frame->getRightBorder().color == frame->getBackgroundColor().color())
		frame->getRightBorder().color = _color.color();
	      if (frame->getTopBorder().color == frame->getBackgroundColor().color())
		frame->getTopBorder().color = _color.color();
	      if (frame->getBottomBorder().color == frame->getBackgroundColor().color())
		frame->getBottomBorder().color = _color.color();
	      frame->setBackgroundColor(_color);
	      if (frameset->getGroupManager() && grpMgrs.findRef(frameset->getGroupManager()) == -1)
		grpMgrs.append(frameset->getGroupManager());
	    }
	}
    }

  doc->updateTableHeaders(grpMgrs);
  doc->updateAllViews(0L);
}

/*================================================================*/
KWGroupManager *KWPage::getTable()
{
  return doc->getFrameSet(fc->getFrameSet() - 1)->getGroupManager();
}

/*================================================================*/
void KWPage::setRulerFirstIndent(KoRuler *ruler,KWUnit _value)
{
  switch (KWUnit::unitType(doc->getUnit()))
    {
    case U_MM:
      ruler->setFirstIndent(_value.mm());
      break;
    case U_INCH:
      ruler->setFirstIndent(_value.inch());
      break;
    case U_PT:
      ruler->setFirstIndent(_value.pt());
      break;
    }
}

/*================================================================*/
void KWPage::setRulerLeftIndent(KoRuler *ruler,KWUnit _value)
{
  switch (KWUnit::unitType(doc->getUnit()))
    {
    case U_MM:
      ruler->setLeftIndent(_value.mm());
      break;
    case U_INCH:
      ruler->setLeftIndent(_value.inch());
      break;
    case U_PT:
      ruler->setLeftIndent(_value.pt());
      break;
    }
}

/*================================================================*/
bool KWPage::editModeChanged(QKeyEvent *e)
{
  KWFrameSet *fs = doc->getFrameSet(fc->getFrameSet() - 1);
  KWGroupManager *grpMgr = fs->getGroupManager();

  if (grpMgr && grpMgr->isTableHeader(fs))
    {
      grpMgr->updateTempHeaders();
      repaintTableHeaders(grpMgr);
    }

  switch (e->key())
    {
    case Key_Delete:
      {
	if (editMode != EM_DELETE)
	  {
	    editMode = EM_DELETE;
	    doc->saveParagInUndoBuffer(fc->getParag(),fc->getFrameSet() - 1,fc);
	    return true;
	  }
      } break;
    case Key_Backspace:
      {
	if (editMode != EM_BACKSPACE)
	  {
	    editMode = EM_BACKSPACE;
	    doc->saveParagInUndoBuffer(fc->getParag(),fc->getFrameSet() - 1,fc);
	    return true;
	  }
      } break;
    case Key_Return: case Key_Enter:
      {
	if (editMode != EM_RETURN)
	  {
	    editMode = EM_RETURN;
	    doc->saveParagInUndoBuffer(fc->getParag(),fc->getFrameSet() - 1,fc);
	    return true;
	  }
      } break;
    default:
      {
	if (e->ascii() && e->ascii() > 31)
	  {
	    if (editMode != EM_INSERT)
	      {
		editMode = EM_INSERT;
		doc->saveParagInUndoBuffer(fc->getParag(),fc->getFrameSet() - 1,fc);
		return true;
	      }
	  }
	else
	  {
	    if (editMode != EM_NONE)
	      {
		editMode = EM_NONE;
		return true;
	      }
	  }
      }
    }

  return false;
}

/*================================================================*/
void KWPage::repaintTableHeaders(KWGroupManager *grpMgr)
{
  QPainter painter;

  if (paint_directly)
    painter.begin(this);
  else
    {
      if (has_to_copy) copyBuffer();
      painter.begin(&buffer);
    }

  KRect r = grpMgr->getBoundingRect();
  r = KRect(r.x() - yOffset,r.y() - yOffset,r.width(),r.height());
  painter.setClipRect(r);
  KWTextFrameSet *fs;

  KWFormatContext *paintfc = new KWFormatContext(doc,doc->getFrameSetNum(grpMgr->getCell(0)->frameSet) + 1);
  for (unsigned i = 0;i < grpMgr->getNumCells();i++)
    {
      fs = dynamic_cast<KWTextFrameSet*>(grpMgr->getCell(i)->frameSet);
      if (!fs->isRemoveableHeader()) continue;

      KWParag *p = 0L;
      p = fs->getFirstParag();
		
      if (p)
	{
	  paintfc->setFrameSet(doc->getFrameSetNum(grpMgr->getCell(i)->frameSet) + 1);
	  paintfc->init(p,painter,true,true);

	  bool bend = false;
	  while (!bend)
	    {
	      doc->printLine(*paintfc,painter,xOffset,yOffset,width(),height(),gui->getView()->getViewFormattingChars());
	      bend = !paintfc->makeNextLineLayout(painter);
	      if (paintfc->getPage() > lastVisiblePage)
		bend = true;
	    }
	}
    }

  delete paintfc;

  painter.end();

  drawBuffer(r);
}

/*================================================================*/
void KWPage::insertVariable(VariableType type)
{
  if (!doc->getVarFormats().find(static_cast<int>(type)))
    {
      warning("HUHU... No variable format for type %d available!",static_cast<int>(type));
      return;
    }

  switch (type)
    {
    case VT_DATE_FIX:
      {
	KWDateVariable *var = new KWDateVariable(doc,true,QDate::currentDate());
	var->setVariableFormat(doc->getVarFormats().find(static_cast<int>(type)));
	fc->getParag()->insertVariable(fc->getTextPos(),var);
	fc->getParag()->setFormat(fc->getTextPos(),1,format);
      } break;
    case VT_DATE_VAR:
      {
	KWDateVariable *var = new KWDateVariable(doc,false,QDate::currentDate());
	var->setVariableFormat(doc->getVarFormats().find(static_cast<int>(type)));
	fc->getParag()->insertVariable(fc->getTextPos(),var);
	fc->getParag()->setFormat(fc->getTextPos(),1,format);
      } break;
    case VT_TIME_FIX:
      {
	KWTimeVariable *var = new KWTimeVariable(doc,true,QTime::currentTime());
	var->setVariableFormat(doc->getVarFormats().find(static_cast<int>(type)));
	fc->getParag()->insertVariable(fc->getTextPos(),var);
	fc->getParag()->setFormat(fc->getTextPos(),1,format);
      } break;
    case VT_TIME_VAR:
      {
	KWTimeVariable *var = new KWTimeVariable(doc,false,QTime::currentTime());
	var->setVariableFormat(doc->getVarFormats().find(static_cast<int>(type)));
	fc->getParag()->insertVariable(fc->getTextPos(),var);
	fc->getParag()->setFormat(fc->getTextPos(),1,format);
      } break;
    case VT_PGNUM:
      {
	KWPgNumVariable *var = new KWPgNumVariable(doc);
	var->setVariableFormat(doc->getVarFormats().find(static_cast<int>(type)));
	fc->getParag()->insertVariable(fc->getTextPos(),var);
	fc->getParag()->setFormat(fc->getTextPos(),1,format);
      } break;
    default: break;
    }

  recalcPage(0L);
  recalcCursor(true);
}

/*================================================================*/
void KWPage::insertFootNote(KWFootNote *fn)
{
  fc->getParag()->insertFootNote(fc->getTextPos(),fn);
  doc->getFootNoteManager().insertFootNote(fn);
  KWFormat fmt(doc,format);
  if (doc->getFootNoteManager().showFootNotesSuperscript())
    fmt.setVertAlign(KWFormat::VA_SUPER);
  else
    fmt.setVertAlign(KWFormat::VA_NORMAL);

  fc->getParag()->setFormat(fc->getTextPos(),1,fmt);

  recalcPage(0L);
  recalcCursor(true);
}

/*================================================================*/
void KWPage::startDrag()
{
  //debug("void KWPage::startDrag()");

  KWordDrag *drag = new KWordDrag(this);
  editCopy();
  QClipboard *cb = QApplication::clipboard();
  drag->setKWord(cb->data()->encodedData(MIME_TYPE));
  drag->setPlain(cb->data()->encodedData("text/plain"));
  //drag->setPixmap(ICON("txt.xpm"),KPoint(10,10));
  drag->dragCopy();
}

/*================================================================*/
void KWPage::dragEnterEvent(QDragEnterEvent *e)
{
  //debug("void KWPage::dragEnterEvent(QDragEnterEvent *e)");

  if (KWordDrag::canDecode(e) ||
      QImageDrag::canDecode(e) ||
      QUrlDrag::canDecode(e))
    {
      if (mouseMode != MM_EDIT)
	setMouseMode(MM_EDIT);
	
      unsigned int mx = e->pos().x() + xOffset;
      unsigned int my = e->pos().y() + yOffset;
	
      QPainter _painter;
      _painter.begin(this);
	
      if (doc->has_selection())
	{
	  doc->drawSelection(_painter,xOffset,yOffset);
	  doc->setSelection(false);
	}

      doc->drawMarker(*fc,&_painter,xOffset,yOffset);
      markerIsVisible = false;
	
      int frameset = doc->getFrameSet(mx,my);
	
      selectedFrameSet = selectedFrame = -1;
      if (frameset != -1 && doc->getFrameSet(frameset)->getFrameType() == FT_TEXT)
	{
	  fc->setFrameSet(frameset + 1);
		
	  fc->cursorGotoPixelLine(mx,my,_painter);
	  fc->cursorGotoPixelInLine(mx,my,_painter);
		
	  _painter.end();
	  scrollToCursor(*fc);
	  _painter.begin(this);

	  doc->drawMarker(*fc,&_painter,xOffset,yOffset);
	  markerIsVisible = true;
		
	  _painter.end();

	  doc->setSelection(false);
		
	  if (doc->getProcessingType() == KWordDocument::DTP)
	    {
	      int frame = doc->getFrameSet(frameset)->getFrame(mx,my);
	      if (frame != -1)
		{
		  if (doc->getProcessingType() == KWordDocument::DTP)
		    setRuler2Frame(frameset,frame);
		}
	      selectedFrame = frame;
	      selectedFrameSet = frameset;
	    }
		
	  gui->getVertRuler()->setOffset(0,-getVertRulerPos());
		
	  if (fc->getParag())
	    {	
	      gui->getView()->updateStyle(fc->getParag()->getParagLayout()->getName(),false);
	      gui->getView()->setFormat(*((KWFormat*)fc),true,false);
	      gui->getView()->setFlow(fc->getParag()->getParagLayout()->getFlow());
	      gui->getView()->setLineSpacing(fc->getParag()->getParagLayout()->getLineSpacing().pt());
	      gui->getView()->setParagBorders(fc->getParag()->getParagLayout()->getLeftBorder(),
					      fc->getParag()->getParagLayout()->getRightBorder(),
					      fc->getParag()->getParagLayout()->getTopBorder(),
					      fc->getParag()->getParagLayout()->getBottomBorder());
	      setRulerFirstIndent(gui->getHorzRuler(),fc->getParag()->getParagLayout()->getFirstLineLeftIndent());
	      setRulerLeftIndent(gui->getHorzRuler(),fc->getParag()->getParagLayout()->getLeftIndent());
	      gui->getHorzRuler()->setFrameStart(doc->getFrameSet(fc->getFrameSet() - 1)->getFrame(fc->getFrame() - 1)->x());
	      gui->getHorzRuler()->setTabList(fc->getParag()->getParagLayout()->getTabList());
	    }
	
	  e->accept();
	}
      else
	{
	  doc->drawMarker(*fc,&_painter,xOffset,yOffset);
	  markerIsVisible = true;
	  _painter.end();
	}
    }
}

/*================================================================*/
void KWPage::dragMoveEvent(QDragMoveEvent *e)
{
  //debug("void KWPage::dragMoveEvent(QDragMoveEvent *e)");
}

/*================================================================*/
void KWPage::dragLeaveEvent(QDragLeaveEvent *e)
{
  //debug("void KWPage::dragLeaveEvent(QDragLeaveEvent *e)");
}

/*================================================================*/
void KWPage::dropEvent(QDropEvent *e)
{
  //debug("void KWPage::dropEvent(QDropEvent *e)");

  QDropEvent *drop = e;

  if (KWordDrag::canDecode(e))
    {
      if (drop->provides(MIME_TYPE))
	{
	  if (drop->encodedData(MIME_TYPE).size())
	    editPaste(drop->encodedData(MIME_TYPE),MIME_TYPE);
	}
      else if (drop->provides("text/plain"))
	{
	  if (drop->encodedData("text/plain").size())
	    editPaste(drop->encodedData("text/plain"));
	}
    }
  else if (QImageDrag::canDecode(e))
    {
      QImage pix;
      QImageDrag::decode(e,pix);

      QString uid = getenv("USER");
      QString num;
      num.setNum(doc->getNumFrameSets());
      uid += "_";
      uid += num;

      QString filename = "/tmp/kword";
      filename += uid;
      filename += ".xpm";

      pix.save(filename,"XPM");
      KWPictureFrameSet *frameset = new KWPictureFrameSet(doc);
      frameset->setFileName(filename,KSize(pix.width(),pix.height()));
      KWFrame *frame = new KWFrame(e->pos().x() + xOffset,e->pos().y() + yOffset,pix.width(),pix.height());
      frameset->addFrame(frame);
      doc->addFrameSet(frameset);
      repaint(false);

      QString cmd = "rm -f ";
      cmd += filename;
      system(cmd.ascii());
    }
  else if (QUrlDrag::canDecode(e))
    {
      QStrList lst;
      QUrlDrag::decode(e,lst);

      QString str;
      for (str = lst.first();!str.isEmpty();str = lst.next())
	{
	  KURL url(str);
	  if (!url.isLocalFile()) return;

	  QString filename = url.path();
 	  KMimeMagicResult *res = KMimeMagic::self()->findFileType(filename);
	
	  if (res && res->isValid())
	    {
	      QString mimetype = res->mimeType();
	      if (mimetype.contains("image"))
		{
		  QPixmap pix(filename);
		  KWPictureFrameSet *frameset = new KWPictureFrameSet(doc);
		  frameset->setFileName(filename,KSize(pix.width(),pix.height()));
		  KWFrame *frame = new KWFrame(e->pos().x() + xOffset,e->pos().y() + yOffset,pix.width(),pix.height());
		  frameset->addFrame(frame);
		  doc->addFrameSet(frameset);
		  repaint(false);
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
	  doc->paste(fc,text,this);
	  repaint(false);
	}
    }
}

/*================================================================*/
bool KWPage::isInSelection(KWFormatContext *_fc)
{
  if (!doc->has_selection())
    return false;

  if (doc->getSelStart()->getParag() == _fc->getParag())
    {
      if (_fc->getTextPos() >= doc->getSelStart()->getTextPos())
	{
	  if (doc->getSelStart()->getParag() == doc->getSelEnd()->getParag())
	    {
	      if (_fc->getTextPos() <= doc->getSelEnd()->getTextPos())
		return true;
	      return false;
	    }
	  return true;
	}
      return false;
    }

  if (doc->getSelStart()->getParag() == doc->getSelEnd()->getParag())
    return false;

  if (doc->getSelEnd()->getParag() == _fc->getParag())
    {
      if (_fc->getTextPos() <= doc->getSelEnd()->getTextPos())
	return true;
      return false;
    }

  KWParag *parag = doc->getSelStart()->getParag()->getNext();

  while (parag && parag != doc->getSelEnd()->getParag())
    {
      if (parag == _fc->getParag())
	return true;

      parag = parag->getNext();
    }

  return false;
}

