/******************************************************************/
/* KWord - (c) by Reginald Stadlbauer 1997-1998                   */
/* based on Torben Weis' KWord                                    */
/* Version: 0.0.1                                                 */
/* Author: Reginald Stadlbauer                                    */
/* E-Mail: reggie@kde.org                                         */
/* Homepage: http://boch35.kfunigraz.ac.at/~rs                    */
/* needs c++ library Qt (http://www.troll.no)                     */
/* written for KDE (http://www.kde.org)                           */
/* needs mico (http://diamant.vsb.cs.uni-frankfurt.de/~mico/)     */
/* needs OpenParts and Kom (weis@kde.org)                         */
/* License: GNU GPL                                               */
/******************************************************************/
/* Module: Page                                                   */
/******************************************************************/

#include "kword_doc.h"
#include "kword_page.h"
#include "kword_view.h"

/******************************************************************/
/* Class: KWPage                                                  */
/******************************************************************/

/*================================================================*/
KWPage::KWPage(QWidget *parent,KWordDocument_impl *_doc,KWordGUI *_gui)
  : QWidget(parent,""), buffer(width(),height())
{ 
  buffer.fill(white);
  doc = _doc;
  gui = _gui;

  xOffset = 0;
  yOffset = 0;

  markerIsVisible = true;
    
  calcVisiblePages();

  QPainter painter;
  painter.begin(&buffer);
  fc = new KWFormatContext(doc);
  fc->init(doc->getFirstParag(),painter);
  fc->cursorGotoLine(0,painter);
  painter.end();
  drawBuffer();
}

// these methods are implemented here, because it didn't compile for
// some reason when they were in kword_page.h
unsigned int KWPage::ptLeftBorder() { return doc->getPTLeftBorder(); }
unsigned int KWPage::ptRightBorder() { return doc->getPTRightBorder(); }
unsigned int KWPage::ptTopBorder() { return doc->getPTTopBorder(); }
unsigned int KWPage::ptBottomBorder() { return doc->getPTBottomBorder(); }
unsigned int KWPage::ptPaperWidth() { return doc->getPTPaperWidth(); }
unsigned int KWPage::ptPaperHeight() { return doc->getPTPaperHeight(); }
unsigned int KWPage::ptColumnWidth() { return doc->getPTColumnWidth(); }
unsigned int KWPage::ptColumnSpacing() { return doc->getPTColumnSpacing(); }

/*================================================================*/
void KWPage::paintEvent(QPaintEvent* e)
{
  QPainter painter;
  painter.begin(&buffer);
  painter.setClipRect(e->rect());

  painter.eraseRect(e->rect().x() - xOffset,e->rect().y() - yOffset,
		    e->rect().width(),e->rect().height());

  // Draw the page borders
  for (unsigned int i = firstVisiblePage - 1;i < lastVisiblePage;i++)
    {
      painter.setPen(lightGray);
      painter.drawRect(-xOffset + ZOOM(ptLeftBorder()) - 1,
		       -yOffset + i * ZOOM(ptPaperHeight()) + ZOOM(ptTopBorder()) - 1,
		       ZOOM(ptPaperWidth()) - ZOOM(ptLeftBorder()) - ZOOM(ptRightBorder()) + 2,
		       ZOOM(ptPaperHeight()) - ZOOM(ptTopBorder()) - ZOOM(ptBottomBorder()) + 2);

      for (unsigned int j = 1;j < doc->getColumns();j++)
	{
	  int x = -xOffset + ZOOM(ptLeftBorder() + j * ptColumnWidth() + (j - 1) * ptColumnSpacing()) + 1;
	  painter.drawLine(x,-yOffset + ZOOM(i * ptPaperHeight() + ptTopBorder()),
			   x, -yOffset + ZOOM( (i + 1) * ptPaperHeight() - ptBottomBorder()));
	    
	  x = -xOffset + ZOOM(ptLeftBorder() + j * ptColumnWidth() + j * ptColumnSpacing()) - 1;
	  painter.drawLine(x,-yOffset + ZOOM(i * ptPaperHeight() + ptTopBorder()),
			   x, -yOffset + ZOOM((i + 1) * ptPaperHeight() - ptBottomBorder()));
	}

      if (i + 1 < lastVisiblePage)
	{
	  painter.setPen(red);
	  painter.drawLine(0,(i + 1) * ZOOM(ptPaperHeight()) - yOffset,
			   width(),(i + 1) * ZOOM(ptPaperHeight()) - yOffset);
	}
    }

    
  // Any text on the display ?
  KWParag *p = doc->findFirstParagOfPage(firstVisiblePage);
  if (p)
    {
      KWFormatContext *paintfc = new KWFormatContext(doc);
      paintfc->init(p,painter);

      bool bend = false;
      while (!bend)
	{
	  doc->printLine(*paintfc,painter,xOffset,yOffset);
	  bend = !paintfc->makeNextLineLayout(painter);
	  if (paintfc->getPage() > lastVisiblePage)
	    bend = true; 
	}
	
      delete paintfc;
    }
  
  doc->drawMarker(*fc,&painter,xOffset,yOffset);
    
  painter.end();
  drawBuffer();
}

/*================================================================*/
void KWPage::keyPressEvent(QKeyEvent *e)
{
  QPainter painter;

  painter.begin(this);
  doc->drawMarker(*fc,&painter,xOffset,yOffset);
  painter.end();

  markerIsVisible = false;
    
  painter.begin(&buffer);
  bool draw_buffer = false;

  switch(e->key())
    {
    case Key_Right:
      fc->cursorGotoRight(painter);
      break;
    case Key_Left:
      fc->cursorGotoLeft(painter);
      break;
    case Key_Up:
      fc->cursorGotoUp(painter);
      break;
    case Key_Down:
      fc->cursorGotoDown(painter);
      break;
    case Key_Shift: case Key_Control: case Key_Alt: case Key_Meta:
      // these keys do nothing at the moment
      break;
    default:
      {
	draw_buffer = true;
	char tmpString[2] = {0,0};
	tmpString[0] = (char)e->ascii();
	unsigned int tmpTextPos = fc->getTextPos();
	fc->getParag()->insertText(fc->getTextPos(),tmpString); 
	fc->makeLineLayout(painter);
	KWFormatContext paintfc(doc);
	paintfc = *fc;
	bool bend = false;
	while (!bend)
	  {
	    painter.fillRect(paintfc.getPTLeft() - xOffset,
			     paintfc.getPTY() - yOffset,
			     paintfc.getPTWidth(),
			     paintfc.getPTAscender() + paintfc.getPTDescender(),
			     QBrush(white));
	    doc->printLine(paintfc,painter,xOffset,yOffset);
	    bend = !paintfc.makeNextLineLayout(painter);
	    if (paintfc.getPage() > lastVisiblePage)
	      bend = true; 
	  }
	
	if (tmpTextPos + 1 <= fc->getLineEndPos())
	  fc->cursorGotoPos(tmpTextPos + 1,painter);
	else 
	  fc->cursorGotoNextLine(painter);

	doc->updateAllViews(gui->getView());
      }  break;
    }
  painter.end();
  if (draw_buffer) drawBuffer();

  scrollToCursor(*fc);
  
  painter.begin(this);
  doc->drawMarker(*fc,&painter,xOffset,yOffset);
  markerIsVisible = true;
    
  painter.end();
}

/*================================================================*/
void KWPage::resizeEvent(QResizeEvent *)
{
  buffer.resize(width(),height());
  calcVisiblePages();
}

/*================================================================*/
void KWPage::scrollToCursor(KWFormatContext &_fc)
{
  int cy = isCursorYVisible(_fc);
  int cx = isCursorXVisible(_fc);    

  if (cx == 0 && cy == 0)
    return;
  
  int oy = yOffset, ox = xOffset;
  if (cy < 0)
    {
      oy = _fc.getPTY() - 10;
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
void KWPage::scrollToOffset(int _x,int _y,KWFormatContext &_fc)
{
  //int dx = _x - xOffset;
  //int dy = _y - yOffset;
  
  QPainter painter;
  painter.begin(&buffer);
  doc->drawMarker(_fc,&painter,xOffset,yOffset);
  painter.end();
  drawBuffer();
    
  xOffset = _x;
  yOffset = _y;

  gui->scrollTo(xOffset,yOffset);
  //scroll(-dx,-dy);
  //gui->getHorzScrollBar()->setValue(xOffset);
  //gui->getVertScrollBar()->setValue(yOffset);
  //gui->setOffset(xOffset,yOffset);
  
//   painter.begin(&buffer);
//   doc->drawMarker(_fc,&painter,yOffset,yOffset);
//   painter.end();
//   drawBuffer();
}

/*================================================================*/
void KWPage::scroll(int dx,int dy)
{ 
  // has to be implemented better !
  // is unefficient now
  xOffset -= dx; 
  yOffset -= dy; 
  buffer.fill(white);
  repaint(false);
  calcVisiblePages(); 
}

/*================================================================*/
int KWPage::isCursorYVisible(KWFormatContext &_fc)
{
  if ((int )_fc.getPTY() - yOffset < 0)
    return -1;
  if (_fc.getPTY() - (unsigned int)yOffset + _fc.getLineHeight() > (unsigned int)height())
    return 1;
  return 0;
}

/*================================================================*/
int KWPage::isCursorXVisible(KWFormatContext &)
{
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
  QPainter painter;
  painter.begin(this);
  painter.drawPixmap(0,0,buffer);
  painter.end();
}
