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

#include "kword_doc.h"
#include "kword_page.h"
#include "kword_page.moc"
#include "kword_view.h"

/******************************************************************/
/* Class: KWPage                                                  */
/******************************************************************/

/*================================================================*/
KWPage::KWPage(QWidget *parent,KWordDocument_impl *_doc,KWordGUI *_gui)
  : QWidget(parent,""), buffer(width(),height()), format(_doc)
{ 
  setBackgroundColor(white);
  buffer.fill(white);
  doc = _doc;
  gui = _gui;

  xOffset = 0;
  yOffset = 0;

  markerIsVisible = true;
  paint_directly = false;
  has_to_copy = false;
    
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

  recalcText();
//   debug("recalc text");
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
void KWPage::mouseMoveEvent(QMouseEvent *e)
{
  if (mousePressed)
    {
      unsigned int mx = e->x() + xOffset;
      unsigned int my = e->y() + yOffset;
      
      QPainter _painter;
      _painter.begin(this);

      int frameset = doc->getFrameSet(mx,my);

      // only if we are in the _same_ frameset as before!!
      if (frameset != -1 && frameset == static_cast<int>(fc->getFrameSet()) - 1)
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
	      
	      if (doc->getProcessingType() == KWordDocument_impl::DTP)
		setRuler2Frame(fc->getFrameSet() - 1,fc->getFrame() - 1);
	      
	      gui->getVertRuler()->setOffset(0,-getVertRulerPos());
	      
	      if (fc->getParag())
		{	  
		  gui->getHorzRuler()->setLeftIndent(POINT_TO_MM(fc->getParag()->getParagLayout()->getPTLeftIndent()));
		  gui->getHorzRuler()->setFirstIndent(POINT_TO_MM(fc->getParag()->getParagLayout()->getPTFirstLineLeftIndent()));
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
    }
}

/*================================================================*/
void KWPage::mousePressEvent(QMouseEvent *e)
{
  switch (e->button())
    {
    case LeftButton:
      {
	mousePressed = true;
	
	unsigned int mx = e->x() + xOffset;
	unsigned int my = e->y() + yOffset;
	
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

	if (frameset != -1)
	  {
	    fc->setFrameSet(frameset + 1);
	    
	    fc->cursorGotoPixelLine(mx,my,_painter);
	    fc->cursorGotoPixelInLine(mx,my,_painter);
	    
	    doc->drawMarker(*fc,&_painter,xOffset,yOffset);
	    markerIsVisible = true;
	    
	    _painter.end();
	    
	    doc->setSelStart(*fc);
	    doc->setSelEnd(*fc);
	    doc->setSelection(false);

	    if (doc->getProcessingType() == KWordDocument_impl::DTP)
	      setRuler2Frame(fc->getFrameSet() - 1,fc->getFrame() - 1);

	    gui->getVertRuler()->setOffset(0,-getVertRulerPos());
	    
	    if (fc->getParag())
	      {	  
		gui->getHorzRuler()->setLeftIndent(POINT_TO_MM(fc->getParag()->getParagLayout()->getPTLeftIndent()));
		gui->getHorzRuler()->setFirstIndent(POINT_TO_MM(fc->getParag()->getParagLayout()->getPTFirstLineLeftIndent()));
	      }
	  }
	else
	  {
	    doc->drawMarker(*fc,&_painter,xOffset,yOffset);
	    markerIsVisible = true;
	    _painter.end();
	  }

      } break;
    case MidButton:
      {
	QClipboard *cb = QApplication::clipboard();
	if (cb->text())
	  editPaste(cb->text());
      } break;
    default: break;
    }
}

/*================================================================*/
void KWPage::mouseReleaseEvent(QMouseEvent *e)
{
  mousePressed = false;
  if (doc->has_selection())
    doc->copySelectedText();
	
  gui->getView()->setFormat(*((KWFormat*)fc));
  gui->getView()->setFlow(fc->getParag()->getParagLayout()->getFlow());
  gui->getView()->setParagBorders(fc->getParag()->getParagLayout()->getLeftBorder(),
				  fc->getParag()->getParagLayout()->getRightBorder(),
				  fc->getParag()->getParagLayout()->getTopBorder(),
				  fc->getParag()->getParagLayout()->getBottomBorder());
}

/*================================================================*/
void KWPage::mouseDoubleClickEvent(QMouseEvent *e)
{
  unsigned int mx = e->x() + xOffset;
  unsigned int my = e->y() + yOffset;

  mousePressed = false;
  
  QPainter _painter;
  _painter.begin(this);

  if (doc->has_selection())
    {
      doc->drawSelection(_painter,xOffset,yOffset);
      doc->setSelection(false);
    }  

  int frameset = doc->getFrameSet(mx,my);
  
  if (frameset != -1)
    {
      fc->setFrameSet(frameset + 1);

      doc->drawMarker(*fc,&_painter,xOffset,yOffset);
      markerIsVisible = false;
  
      fc->cursorGotoPixelLine(mx,my,_painter);
      fc->cursorGotoPixelInLine(mx,my,_painter);
      
      KWFormatContext fc1(doc,fc->getFrameSet() - 1),fc2(doc,fc->getFrameSet() - 1);
      fc->selectWord(fc1,fc2,_painter);

      doc->drawMarker(*fc,&_painter,xOffset,yOffset);
      markerIsVisible = true;
      
      doc->setSelStart(fc1);
      doc->setSelEnd(fc2);
      doc->setSelection(true);
      doc->drawSelection(_painter,xOffset,yOffset);

      if (doc->getProcessingType() == KWordDocument_impl::DTP)
	setRuler2Frame(fc->getFrameSet() - 1,fc->getFrame() - 1);
      
      gui->getVertRuler()->setOffset(0,-getVertRulerPos());
      
      if (fc->getParag())
	{	  
	  gui->getHorzRuler()->setLeftIndent(POINT_TO_MM(fc->getParag()->getParagLayout()->getPTLeftIndent()));
	  gui->getHorzRuler()->setFirstIndent(POINT_TO_MM(fc->getParag()->getParagLayout()->getPTFirstLineLeftIndent()));
	}
    }

  _painter.end();
}

/*================================================================*/
void KWPage::recalcCursor(bool _repaint = true,int _pos = -1,KWFormatContext *_fc = 0L)
{
  if (!_fc) _fc = fc;

  QPainter _painter;
  _painter.begin(this);

  unsigned int pos = _fc->getTextPos();
  if (_pos != -1) pos = static_cast<unsigned int>(_pos);

  _fc->init(_fc->getParag(),_painter,false,false);

  _fc->gotoStartOfParag(_painter);
  _fc->cursorGotoLineStart(_painter);
  for (unsigned int i = 0;i < pos;i++)
    _fc->cursorGotoRight(_painter);

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
void KWPage::editPaste(QString _string)
{
  doc->paste(fc,_string,this);
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
    bend = !_fc.makeNextLineLayout(painter);

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

  painter.eraseRect(e->rect().x() - xOffset,e->rect().y() - yOffset,
		    e->rect().width(),e->rect().height());

  drawBorders(painter,e->rect());
    
  KWFormatContext *paintfc = new KWFormatContext(doc,1);
  for (unsigned i = 0;i < doc->getNumFrameSets();i++)
    {
      KWParag *p = doc->findFirstParagOfPage(firstVisiblePage,i);
      if (p)
	{
	  if (p->getPrev()) p = p->getPrev();
	  paintfc->setFrameSet(i + 1);
	  paintfc->init(p,painter,true,recalcAll);

	  bool bend = false;
	  while (!bend)
	    {
	      if (doc->getFrameSet(i)->getFrame(paintfc->getFrame() - 1)->top() - static_cast<int>(yOffset) >
		  static_cast<int>(lastVisiblePage) * static_cast<int>(ptPaperHeight())) 
		break;
	      doc->printLine(*paintfc,painter,xOffset,yOffset,width(),height());
	      bend = !paintfc->makeNextLineLayout(painter);
	      if (paintfc->getPage() > lastVisiblePage)
		bend = true; 
	    }
      
	} 
    }
  delete paintfc;
  if (doc->has_selection()) doc->drawSelection(painter,xOffset,yOffset);
  
  doc->drawMarker(*fc,&painter,xOffset,yOffset);
  markerIsVisible = true;

  painter.end();

  if (!paint_directly) drawBuffer();
}

/*================================================================*/
void KWPage::keyPressEvent(QKeyEvent *e)
{
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

	unsigned int tmpTextPos = fc->getTextPos();

	if (fc->isCursorAtParagEnd())
	  {
	    frameSet->insertParag(fc->getParag(),I_AFTER);
	    fc->setTextPos(0);
	    fc->init(fc->getParag()->getNext(),painter);
	    fc->cursorGotoLineStart(painter);
	  }
	else if (fc->isCursorAtParagStart())
	  {
	    frameSet->insertParag(fc->getParag(),I_BEFORE);
	    fc->setTextPos(0);
	    fc->init(fc->getParag(),painter);
	    fc->cursorGotoLineStart(painter);
	  }
	else 
	  {
	    bool _insert = fc->isCursorAtLineStart();
	    frameSet->splitParag(fc->getParag(),tmpTextPos);
	    fc->init(fc->getParag()->getNext(),painter);
	    fc->cursorGotoLineStart(painter);
	    if (_insert)
	      {
		frameSet->insertParag(fc->getParag(),I_BEFORE);
		fc->init(fc->getParag(),painter);
		fc->cursorGotoLineStart(painter);
	      }
	  }

	painter.end();
	draw_buffer = false;

	scrollToCursor(*fc);

	buffer.fill(white);
	repaint(false);

	// HACK
	kbdc.auto_repeat_mode = repeat;
	XChangeKeyboardControl(kapp->getDisplay(),KBAutoRepeatMode,&kbdc);

	if (oldPage != fc->getPage())
	  gui->getVertRuler()->setOffset(0,-getVertRulerPos());
	if (oldParag != fc->getParag() && fc->getParag())
	  {	  
	    gui->getHorzRuler()->setLeftIndent(POINT_TO_MM(fc->getParag()->getParagLayout()->getPTLeftIndent()));
	    gui->getHorzRuler()->setFirstIndent(POINT_TO_MM(fc->getParag()->getParagLayout()->getPTFirstLineLeftIndent()));
	  }
	if (doc->getProcessingType() == KWordDocument_impl::DTP && oldFrame != fc->getFrame())
	  setRuler2Frame(fc->getFrameSet() - 1,fc->getFrame() - 1);

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

	if (!del && fc->getParag()->getTextLen() == 0 && fc->getParag()->getNext())
	  {
	    KWParag *p = fc->getParag()->getNext();
	    frameSet->deleteParag(fc->getParag());
	    if (p) fc->init(p,painter);
	  }
	else if (!del && fc->getParag()->getTextLen() > 0)
	  frameSet->joinParag(fc->getParag(),fc->getParag()->getNext());
	  
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

	QRect currFrame = QRect(frameSet->getFrame(paintfc.getFrame() - 1)->x() - xOffset,
				frameSet->getFrame(paintfc.getFrame() - 1)->y() - yOffset,
				frameSet->getFrame(paintfc.getFrame() - 1)->width(),
				frameSet->getFrame(paintfc.getFrame() - 1)->height());
	unsigned int currFrameNum = paintfc.getFrame() - 1;

	while (!bend)
	  {
	    if (frameSet->getFrame(paintfc.getFrame() - 1)->top() - static_cast<int>(yOffset) >
		static_cast<int>(lastVisiblePage) * static_cast<int>(ptPaperHeight())) 
	      break;
	    unsigned int _x = frameSet->getFrame(paintfc.getFrame() - 1)->x() - xOffset;
	    unsigned int _wid = frameSet->getFrame(paintfc.getFrame() - 1)->width();
	    painter.fillRect(_x,paintfc.getPTY() - yOffset,
			     _wid,paintfc.getLineHeight(),QBrush(white));
	    doc->printLine(paintfc,painter,xOffset,yOffset,width(),height());
	    bend = !paintfc.makeNextLineLayout(painter);

	    if (paintfc.getFrame() - 1 != currFrameNum)
	      {
		drawBuffer(currFrame);
		currFrame = KWFrame(frameSet->getFrame(paintfc.getFrame() - 1)->x() - xOffset,
				    frameSet->getFrame(paintfc.getFrame() - 1)->y() - yOffset,
				    frameSet->getFrame(paintfc.getFrame() - 1)->width(),
				    frameSet->getFrame(paintfc.getFrame() - 1)->height());
		currFrameNum = paintfc.getFrame() - 1;
	      }

	    if (paintfc.getPage() > lastVisiblePage)
	      bend = true;
	  }
	drawBuffer(currFrame);

	if ((int)paintfc.getPTY() + (int)paintfc.getLineHeight() < frameSet->getFrame(paintfc.getFrame() - 1)->bottom())
	  {
	    unsigned int _y = (int)paintfc.getPTY() + (int)paintfc.getLineHeight() - (int)yOffset;
	    unsigned int _x = frameSet->getFrame(paintfc.getFrame() - 1)->x() - xOffset;
	    unsigned int _wid = frameSet->getFrame(paintfc.getFrame() - 1)->width();
	    unsigned int _hei = frameSet->getFrame(paintfc.getFrame() - 1)->height() - 
	      (_y - frameSet->getFrame(paintfc.getFrame() - 1)->y());
	    painter.fillRect(_x,_y,_wid,_hei,QBrush(white));
	    drawBuffer(QRect(_x,_y,_wid,_hei));
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
	    draw_buffer = false;
	    repaint(false);
	    // HACK
	    kbdc.auto_repeat_mode = repeat;
	    XChangeKeyboardControl(kapp->getDisplay(),KBAutoRepeatMode,&kbdc);

	    if (oldPage != fc->getPage())
	      gui->getVertRuler()->setOffset(0,-getVertRulerPos());
	    if (oldParag != fc->getParag() && fc->getParag())
	      {	  
		gui->getHorzRuler()->setLeftIndent(POINT_TO_MM(fc->getParag()->getParagLayout()->getPTLeftIndent()));
		gui->getHorzRuler()->setFirstIndent(POINT_TO_MM(fc->getParag()->getParagLayout()->getPTFirstLineLeftIndent()));
	      }
	    if (doc->getProcessingType() == KWordDocument_impl::DTP && oldFrame != fc->getFrame())
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

	if (!del && fc->getParag()->getTextLen() == 0)
	  {
	    KWParag *p = fc->getParag()->getPrev();
	    frameSet->deleteParag(fc->getParag());
	    if (p) 
	      {
		fc->init(p,painter);
		tmpTextPos = p->getTextLen();
	      }
	  }
	else if (!del && fc->getParag()->getTextLen() > 0)
	  {
	    KWParag *p = fc->getParag()->getPrev();
	    frameSet->joinParag(fc->getParag()->getPrev(),fc->getParag());
	    if (p) fc->init(p,painter);
	    joined = p ? true : false;
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

	QRect currFrame = QRect(frameSet->getFrame(paintfc.getFrame() - 1)->x() - xOffset,
				frameSet->getFrame(paintfc.getFrame() - 1)->y() - yOffset,
				frameSet->getFrame(paintfc.getFrame() - 1)->width(),
				frameSet->getFrame(paintfc.getFrame() - 1)->height());
	unsigned int currFrameNum = paintfc.getFrame() - 1;

	while (!bend)
	  {
	    if (frameSet->getFrame(paintfc.getFrame() - 1)->top() - static_cast<int>(yOffset) >
		static_cast<int>(lastVisiblePage) * static_cast<int>(ptPaperHeight())) 
	      break;
	    unsigned int _x = frameSet->getFrame(paintfc.getFrame() - 1)->x() - xOffset;
	    unsigned int _wid = frameSet->getFrame(paintfc.getFrame() - 1)->width();
	    painter.fillRect(_x,paintfc.getPTY() - yOffset,
			     _wid,paintfc.getLineHeight(),QBrush(white));
	    doc->printLine(paintfc,painter,xOffset,yOffset,width(),height());
	    bend = !paintfc.makeNextLineLayout(painter);

	    if (paintfc.getFrame() - 1 != currFrameNum)
	      {
		drawBuffer(currFrame);
		currFrame = KWFrame(frameSet->getFrame(paintfc.getFrame() - 1)->x() - xOffset,
				    frameSet->getFrame(paintfc.getFrame() - 1)->y() - yOffset,
				    frameSet->getFrame(paintfc.getFrame() - 1)->width(),
				    frameSet->getFrame(paintfc.getFrame() - 1)->height());
		currFrameNum = paintfc.getFrame() - 1;
	      }

	    if (paintfc.getPage() > lastVisiblePage)
	      bend = true;
	  }
	drawBuffer(currFrame);

	if ((int)paintfc.getPTY() + (int)paintfc.getLineHeight() < frameSet->getFrame(paintfc.getFrame() - 1)->bottom())
	  {
	    unsigned int _y = (int)paintfc.getPTY() + (int)paintfc.getLineHeight() - (int)yOffset;
	    unsigned int _x = frameSet->getFrame(paintfc.getFrame() - 1)->x() - xOffset;
	    unsigned int _wid = frameSet->getFrame(paintfc.getFrame() - 1)->width();
	    unsigned int _hei = frameSet->getFrame(paintfc.getFrame() - 1)->height() - 
	      (_y - frameSet->getFrame(paintfc.getFrame() - 1)->y());
	    painter.fillRect(_x,_y,_wid,_hei,QBrush(white));
	    drawBuffer(QRect(_x,_y,_wid,_hei));
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
      // these keys do nothing at the moment
      break;
    default:
      {
	if (e->ascii() && e->ascii() > 31)
	  {
	    if (has_to_copy) copyBuffer();
	    
	    draw_buffer = false;
	    char tmpString[2] = {0,0};
	    bool isPrev = false;
	    tmpString[0] = (char)e->ascii();
	    unsigned int tmpTextPos = fc->getTextPos();
	    fc->getParag()->insertText(fc->getTextPos(),tmpString);
	    fc->getParag()->setFormat(fc->getTextPos(),1,format);
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
	    
	    QRect currFrame = QRect(frameSet->getFrame(paintfc.getFrame() - 1)->x() - xOffset,
				    frameSet->getFrame(paintfc.getFrame() - 1)->y() - yOffset,
				    frameSet->getFrame(paintfc.getFrame() - 1)->width(),
				    frameSet->getFrame(paintfc.getFrame() - 1)->height());
	    unsigned int currFrameNum = paintfc.getFrame() - 1;
	    
	    while (!bend)
	      {
		if (frameSet->getFrame(paintfc.getFrame() - 1)->top() - static_cast<int>(yOffset) >
		    static_cast<int>(lastVisiblePage) * static_cast<int>(ptPaperHeight())) 
		  break;
		unsigned int _x = frameSet->getFrame(paintfc.getFrame() - 1)->x() - xOffset;
		unsigned int _wid = frameSet->getFrame(paintfc.getFrame() - 1)->width();
		painter.fillRect(_x,paintfc.getPTY() - yOffset,
				 _wid,paintfc.getLineHeight(),QBrush(white));
		doc->printLine(paintfc,painter,xOffset,yOffset,width(),height());
		bend = !paintfc.makeNextLineLayout(painter);
		
		if (paintfc.getFrame() - 1 != currFrameNum)
		  {
		    drawBuffer(currFrame);
		    currFrame = KWFrame(frameSet->getFrame(paintfc.getFrame() - 1)->x() - xOffset,
					frameSet->getFrame(paintfc.getFrame() - 1)->y() - yOffset,
					frameSet->getFrame(paintfc.getFrame() - 1)->width(),
					frameSet->getFrame(paintfc.getFrame() - 1)->height());
		    currFrameNum = paintfc.getFrame() - 1;
		  }
		
		if (paintfc.getPage() > lastVisiblePage)
		  bend = true;
	      }
	    drawBuffer(currFrame);

	    if (isPrev)
	      fc->cursorGotoNextLine(painter);

	    fc->makeLineLayout(painter);

	    if (tmpTextPos + 1 <= fc->getLineEndPos())
	      fc->cursorGotoPos(tmpTextPos + 1,painter);
	    else 
	      {
		fc->cursorGotoNextLine(painter);
		fc->cursorGotoPos(tmpTextPos + 1,painter);
	      }
	    
	    doc->updateAllViews(gui->getView());
	  }
      }  break;
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
      gui->getHorzRuler()->setLeftIndent(POINT_TO_MM(fc->getParag()->getParagLayout()->getPTLeftIndent()));
      gui->getHorzRuler()->setFirstIndent(POINT_TO_MM(fc->getParag()->getParagLayout()->getPTFirstLineLeftIndent()));
    }
  if (doc->getProcessingType() == KWordDocument_impl::DTP && oldFrame != fc->getFrame())
    setRuler2Frame(fc->getFrameSet() - 1,fc->getFrame() - 1);

  inKeyEvent = false;
}

/*================================================================*/
void KWPage::resizeEvent(QResizeEvent *)
{
  buffer.resize(width(),height());
  buffer.fill(white);
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

/*================================================================*/
void KWPage::formatChanged(KWFormat &_format)
{
  format = _format;

  if (doc->has_selection() && !inKeyEvent)
    {
      QPainter p;

      p.begin(this);
      doc->drawSelection(p,xOffset,yOffset);
      p.end();

      doc->setSelection(false);
      doc->setFormat(format);
      recalcCursor(false,-1,doc->getSelStart());
      recalcCursor(false,-1,doc->getSelEnd());
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
void KWPage::drawBuffer(QRect _rect)
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
void KWPage::drawBorders(QPainter &_painter,QRect v_area)
{
  _painter.save();
  _painter.setBrush(NoBrush);
  _painter.setPen(lightGray);

  KWFrameSet *frameset = 0;
  KWFrame *tmp;
  QRect frame;

  for (unsigned int i = 0;i < doc->getNumFrameSets();i++)
    {
      frameset = doc->getFrameSet(i);
      for (unsigned int j = 0;j < frameset->getNumFrames();j++)
	{
	  tmp = frameset->getFrame(j);
	  frame = QRect(tmp->x() - xOffset - 1,tmp->y() - yOffset - 1,tmp->width() + 2,tmp->height() + 2);

	  if (v_area.intersects(frame))
	    _painter.drawRect(frame);
	}
    }

  _painter.setPen(red);

  for (int k = 0;k < doc->getPages();k++)
    {
      QRect tmp2 = QRect(-xOffset,(k * doc->getPTPaperHeight()) - yOffset,doc->getPTPaperWidth(),doc->getPTPaperHeight());
      if (v_area.intersects(tmp2))
	_painter.drawRect(tmp2);
    }

  _painter.restore();
}

/*================================================================*/
void KWPage::frameSizeChanged(KoPageLayout _layout)
{
  KWFrame *frame = doc->getFrameSet(fc->getFrameSet() - 1)->getFrame(fc->getFrame() - 1);

  unsigned int page = 0;
  for (int i = 0;i < doc->getPages();i++)
    {
      if (frame->intersects(QRect(0,i * ptPaperHeight(),ptPaperWidth(),ptPaperHeight())))
	{
	  page = i;
	  break;
	}
    }
  
  doc->getFrameSet(fc->getFrameSet() - 1)->getFrame(fc->getFrame() - 1)->setCoords(MM_TO_POINT(_layout.left),
										   MM_TO_POINT(_layout.top) + page * ptPaperHeight(),
										   MM_TO_POINT(_layout.width) - MM_TO_POINT(_layout.right),
										   MM_TO_POINT(_layout.height) - MM_TO_POINT(_layout.bottom)
										   + page * ptPaperHeight());
  recalcText();
  recalcCursor();
}

/*================================================================*/
void KWPage::setRuler2Frame(unsigned int _frameset,unsigned int _frame)
{
  KoPageLayout _layout;
  KoColumns _cl;
  doc->getPageLayout(_layout,_cl);
  KWFrame *frame = doc->getFrameSet(_frameset)->getFrame(_frame);

  unsigned int page = 0;
  for (int i = 0;i < doc->getPages();i++)
    {
      if (frame->intersects(QRect(0,i * ptPaperHeight(),ptPaperWidth(),ptPaperHeight())))
	{
	  page = i;
	  break;
	}
    }

  _layout.left = POINT_TO_MM(frame->left());
  _layout.top = POINT_TO_MM(frame->top()) - page * POINT_TO_MM(ptPaperHeight());
  _layout.right = POINT_TO_MM(_layout.width - frame->right());
  _layout.bottom = POINT_TO_MM(_layout.height - frame->bottom()) + page * POINT_TO_MM(ptPaperHeight());
  gui->getHorzRuler()->setPageLayout(_layout);
  gui->getVertRuler()->setPageLayout(_layout);
}
