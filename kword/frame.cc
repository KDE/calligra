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
/* Module: Frame                                                  */
/******************************************************************/

#include "kword_doc.h"
#include "kword_view.h"
#include "frame.h"

#include <koIMR.h>
#include <komlMime.h>
#include <strstream>
#include <fstream>
#include <unistd.h>

#ifndef max
#define max(a,b) ((a)>(b)?(a):(b))
#endif

#ifndef min
#define min(a,b) ((a)<(b)?(a):(b))
#endif

/******************************************************************/
/* Class: KWFrame                                                 */
/******************************************************************/

/*================================================================*/
KWFrame::KWFrame() 
  : KRect(), intersections(), oldIntersects()
{ 
  runAround = RA_NO; 
  intersections.setAutoDelete(false);
  oldIntersects.setAutoDelete(true);
  selected = false;
  runAroundGap = 1;
  mostRight = false;

  backgroundColor = QBrush(white);
  brd_left.color = getBackgroundColor().color();
  brd_left.style = KWParagLayout::SOLID;
  brd_left.ptWidth = 1;
  brd_right.color = getBackgroundColor().color();
  brd_right.style = KWParagLayout::SOLID;
  brd_right.ptWidth = 1;
  brd_top.color = getBackgroundColor().color();
  brd_top.style = KWParagLayout::SOLID;
  brd_top.ptWidth = 1;
  brd_bottom.color = getBackgroundColor().color();
  brd_bottom.style = KWParagLayout::SOLID;
  brd_bottom.ptWidth = 1;
}

/*================================================================*/
KWFrame::KWFrame(const KPoint &topleft,const QPoint &bottomright) 
  : KRect(topleft,bottomright), intersections(), oldIntersects()
{ 
  runAround = RA_NO; 
  intersections.setAutoDelete(false); 
  oldIntersects.setAutoDelete(true);
  selected = false;
  runAroundGap = 1;
  mostRight = false;

  backgroundColor = QBrush(white);
  brd_left.color = getBackgroundColor().color();
  brd_left.style = KWParagLayout::SOLID;
  brd_left.ptWidth = 1;
  brd_right.color = getBackgroundColor().color();
  brd_right.style = KWParagLayout::SOLID;
  brd_right.ptWidth = 1;
  brd_top.color = getBackgroundColor().color();
  brd_top.style = KWParagLayout::SOLID;
  brd_top.ptWidth = 1;
  brd_bottom.color = getBackgroundColor().color();
  brd_bottom.style = KWParagLayout::SOLID;
  brd_bottom.ptWidth = 1;
} 

/*================================================================*/
KWFrame::KWFrame(const KPoint &topleft,const KSize &size) 
  : KRect(topleft,size), intersections(), oldIntersects()
{ 
  runAround = RA_NO; 
  intersections.setAutoDelete(false); 
  oldIntersects.setAutoDelete(true);
  selected = false;
  runAroundGap = 1;
  mostRight = false;

  backgroundColor = QBrush(white);
  brd_left.color = getBackgroundColor().color();
  brd_left.style = KWParagLayout::SOLID;
  brd_left.ptWidth = 1;
  brd_right.color = getBackgroundColor().color();
  brd_right.style = KWParagLayout::SOLID;
  brd_right.ptWidth = 1;
  brd_top.color = getBackgroundColor().color();
  brd_top.style = KWParagLayout::SOLID;
  brd_top.ptWidth = 1;
  brd_bottom.color = getBackgroundColor().color();
  brd_bottom.style = KWParagLayout::SOLID;
  brd_bottom.ptWidth = 1;
}    

/*================================================================*/
KWFrame::KWFrame(int left,int top,int width,int height) 
  : KRect(left,top,width,height), intersections(), oldIntersects()
{ 
  runAround = RA_NO; 
  intersections.setAutoDelete(false); 
  oldIntersects.setAutoDelete(true);
  selected = false;
  runAroundGap = 1;
  mostRight = false;

  backgroundColor = QBrush(white);
  brd_left.color = getBackgroundColor().color();
  brd_left.style = KWParagLayout::SOLID;
  brd_left.ptWidth = 1;
  brd_right.color = getBackgroundColor().color();
  brd_right.style = KWParagLayout::SOLID;
  brd_right.ptWidth = 1;
  brd_top.color = getBackgroundColor().color();
  brd_top.style = KWParagLayout::SOLID;
  brd_top.ptWidth = 1;
  brd_bottom.color = getBackgroundColor().color();
  brd_bottom.style = KWParagLayout::SOLID;
  brd_bottom.ptWidth = 1;
}

/*================================================================*/
KWFrame::KWFrame(int left,int top,int width,int height,RunAround _ra,int _gap) 
  : KRect(left,top,width,height), intersections(), oldIntersects()
{ 
  runAround = _ra; 
  intersections.setAutoDelete(false); 
  oldIntersects.setAutoDelete(true);
  selected = false;
  runAroundGap = _gap;
  mostRight = false;

  backgroundColor = QBrush(white);
  brd_left.color = getBackgroundColor().color();
  brd_left.style = KWParagLayout::SOLID;
  brd_left.ptWidth = 1;
  brd_right.color = getBackgroundColor().color();
  brd_right.style = KWParagLayout::SOLID;
  brd_right.ptWidth = 1;
  brd_top.color = getBackgroundColor().color();
  brd_top.style = KWParagLayout::SOLID;
  brd_top.ptWidth = 1;
  brd_bottom.color = getBackgroundColor().color();
  brd_bottom.style = KWParagLayout::SOLID;
  brd_bottom.ptWidth = 1;
}

/*================================================================*/
KWFrame::KWFrame(const QRect &_rect)
  : KRect(_rect), intersections(), oldIntersects()
{
  runAround = RA_NO; 
  intersections.setAutoDelete(false); 
  oldIntersects.setAutoDelete(true);
  selected = false;
  runAroundGap = 1;
  mostRight = false;

  backgroundColor = QBrush(white);
  brd_left.color = getBackgroundColor().color();
  brd_left.style = KWParagLayout::SOLID;
  brd_left.ptWidth = 1;
  brd_right.color = getBackgroundColor().color();
  brd_right.style = KWParagLayout::SOLID;
  brd_right.ptWidth = 1;
  brd_top.color = getBackgroundColor().color();
  brd_top.style = KWParagLayout::SOLID;
  brd_top.ptWidth = 1;
  brd_bottom.color = getBackgroundColor().color();
  brd_bottom.style = KWParagLayout::SOLID;
  brd_bottom.ptWidth = 1;
}

/*================================================================*/
void KWFrame::addIntersect(KRect _r)
{ 
  intersections.append(new KRect(_r)); 
}

/*================================================================*/
bool KWFrame::intersectChanged()
{
  return true;

  if (intersections.count() == oldIntersects.count())
    {
      for (unsigned int i = 0;i < intersections.count();i++)
	{
	  if (*(intersections.at(i)) != *(oldIntersects.at(i)))
	    return true;
	}
      return false;
    }
  else return true;
}

/*================================================================*/
int KWFrame::getLeftIndent(int _y,int _h)
{
  if (runAround == RA_NO || intersections.isEmpty()) return 0;

  int _left = 0;
  KRect rect;

  for (unsigned int i = 0;i < intersections.count();i++)
    {
      rect = *intersections.at(i);

      if (rect.intersects(KRect(left(),_y,width(),_h)))
	{
	  if (rect.left() == left())
	    _left = max(_left,rect.width() + MM_TO_POINT(runAroundGap));
	}
    }

  return _left;
}

/*================================================================*/
int KWFrame::getRightIndent(int _y,int _h)
{
  if (runAround == RA_NO || intersections.isEmpty()) return 0;

  int _right = 0;
  KRect rect;

  for (unsigned int i = 0;i < intersections.count();i++)
    {
      rect = *intersections.at(i);

      if (rect.intersects(KRect(left(),_y,width(),_h)))
	{
	  if (rect.right() == right())
	    _right = max(_right,rect.width() + MM_TO_POINT(runAroundGap));
	}
    }

  return _right;
}

/*================================================================*/
QRegion KWFrame::getEmptyRegion()
{
  QRegion region(x(),y(),width(),height());
  QRect rect;

  for (unsigned int i = 0;i < intersections.count();i++)
    {
      rect = *intersections.at(i);
      region = region.subtract(QRect(rect.x() - 1,rect.y() - 1,rect.width() + 2,rect.height() + 2));
    }

  return QRegion(region);
}

/*================================================================*/
QCursor KWFrame::getMouseCursor(int mx,int my,bool table)
{
  if (!table)
    {
      if (mx >= x() && my >= y() && mx <= x() + 6 && my <= y() + 6)
	return sizeFDiagCursor;
      if (mx >= x() && my >= y() + height() / 2 - 3 && mx <= x() + 6 && my <= y() + height() / 2 + 3)
	return sizeHorCursor;
      if (mx >= x() && my >= y() + height() - 6 && mx <= x() + 6 && my <= y() + height())
	return sizeBDiagCursor;
      if (mx >= x() + width() / 2 - 3 && my >= y() && mx <= x() + width() / 2 + 3 && my <= y() + 6)
	return sizeVerCursor;
      if (mx >= x() + width() / 2 - 3 && my >= y() + height() - 6 && mx <= x() + width() / 2 + 3 && my <= y() + height())
	return sizeVerCursor;
      if (mx >= x() + width() - 6 && my >= y() && mx <= x() + width() && my <= y() + 6)
	return sizeBDiagCursor;
      if (mx >= x() + width() - 6 && my >= y() + height() / 2 - 3 && mx <= x() + width() && my <= y() + height() / 2 + 3)
	return sizeHorCursor;
      if (mx >= x() + width() - 6 && my >= y() + height() - 6 && mx <= x() + width() && my <= y() + height())
	return sizeFDiagCursor;
      
      if (selected)
	return sizeAllCursor;
    }
  else
    {
      if (mx >= x() + width() - 6 && my >= y() && mx <= x() + width() && my <= y() + height())
	return sizeHorCursor;
      if (mx >= x() && my >= y() + height() - 6 && mx <= x() + width() && my <= y() + height())
	return sizeVerCursor;
      return sizeAllCursor;
    }

  return arrowCursor;
}

/*================================================================*/
QString KWFrame::leftBrd2String()
{
  QString str;
  str.sprintf(" lWidth=\"%d\" lRed=\"%d\" lGreen=\"%d\" lBlue=\"%d\" lStyle=\"%d\" ",
	      brd_left.ptWidth,brd_left.color.red(),brd_left.color.green(),brd_left.color.blue(),
	      static_cast<int>(brd_left.style));
  return str;
}

/*================================================================*/
QString KWFrame::rightBrd2String()
{
  QString str;
  str.sprintf(" rWidth=\"%d\" rRed=\"%d\" rGreen=\"%d\" rBlue=\"%d\" rStyle=\"%d\" ",
	      brd_right.ptWidth,brd_right.color.red(),brd_right.color.green(),brd_right.color.blue(),
	      static_cast<int>(brd_right.style));
  return str;
}

/*================================================================*/
QString KWFrame::topBrd2String()
{
  QString str;
  str.sprintf(" tWidth=\"%d\" tRed=\"%d\" tGreen=\"%d\" tBlue=\"%d\" tStyle=\"%d\" ",
	      brd_top.ptWidth,brd_top.color.red(),brd_top.color.green(),brd_top.color.blue(),
	      static_cast<int>(brd_top.style));
  return str;
}

/*================================================================*/
QString KWFrame::bottomBrd2String()
{
  QString str;
  str.sprintf(" bWidth=\"%d\" bRed=\"%d\" bGreen=\"%d\" bBlue=\"%d\" bStyle=\"%d\" ",
	      brd_bottom.ptWidth,brd_bottom.color.red(),brd_bottom.color.green(),brd_bottom.color.blue(),
	      static_cast<int>(brd_bottom.style));
  return str;
}

/******************************************************************/
/* Class: KWFrameSet                                              */
/******************************************************************/

/*================================================================*/
KWFrameSet::KWFrameSet(KWordDocument *_doc)
  : frames()
{ 
  doc = _doc; 
  frames.setAutoDelete(true); 
  frameInfo = FI_BODY;
  current = 0;
  grpMgr = 0L;
}

/*================================================================*/
void KWFrameSet::addFrame(KWFrame _frame)
{
  frames.append(new KWFrame(_frame.x(),_frame.y(),_frame.width(),_frame.height(),_frame.getRunAround(),_frame.getRunAroundGap()));
  if (frames.count() == 1) init();
  update();
}

/*================================================================*/
void KWFrameSet::addFrame(KWFrame *_frame)
{
  frames.append(new KWFrame(_frame->x(),_frame->y(),_frame->width(),_frame->height(),_frame->getRunAround(),_frame->getRunAroundGap()));
  if (frames.count() == 1) init();
  update();
}

/*================================================================*/
void KWFrameSet::delFrame(unsigned int _num)
{
  frames.remove(_num);
  update();
}

/*================================================================*/
int KWFrameSet::getFrame(int _x,int _y)
{
  for (unsigned int i = 0;i < getNumFrames();i++)
    {
      if (getFrame(i)->contains(KPoint(_x,_y))) return i;
    }
  return -1;
}

/*================================================================*/
KWFrame *KWFrameSet::getFrame(unsigned int _num)
{
  return frames.at(_num);
}

/*================================================================*/
bool KWFrameSet::contains(unsigned int mx,unsigned int my)
{
  for (unsigned int i = 0;i < frames.count();i++)
    {
      if (frames.at(i)->contains(KPoint(mx,my))) return true;
    }

  return false;
}

/*================================================================*/
int KWFrameSet::selectFrame(unsigned int mx,unsigned int my)
{
  for (unsigned int i = 0;i < frames.count();i++)
    {
      if (frames.at(i)->contains(KPoint(mx,my)))
	{
	  int r = 1;
	  if (frames.at(i)->isSelected()) r = 2;
	  frames.at(i)->setSelected(true);
	  return r;
	}
    }
  return 0;
}

/*================================================================*/
void KWFrameSet::deSelectFrame(unsigned int mx,unsigned int my)
{
  for (unsigned int i = 0;i < frames.count();i++)
    {
      if (frames.at(i)->contains(KPoint(mx,my)))
	frames.at(i)->setSelected(false);
    }
}

/*================================================================*/
QCursor KWFrameSet::getMouseCursor(unsigned int mx,unsigned int my)
{
  int frame = getFrame(mx,my);

  if (frame == -1)
    return arrowCursor;

  if (!getFrame(frame)->isSelected() && !grpMgr) return arrowCursor;

  return getFrame(frame)->getMouseCursor(mx,my,grpMgr ? true : false);
}

/*================================================================*/
void KWFrameSet::save(ostream &out)
{
  KWFrame *frame;

  for (unsigned int i = 0;i < frames.count();i++)
    {
      frame = getFrame(i);
      out << indent << "<FRAME left=\"" << frame->left() << "\" top=\"" << frame->top()
	  << "\" right=\"" << frame->right() << "\" bottom=\"" << frame->bottom() 
	  << "\" runaround=\"" << static_cast<int>(frame->getRunAround()) 
	  << "\" runaroundGap=\"" << frame->getRunAroundGap() << "\""
	  << frame->leftBrd2String() << frame->rightBrd2String() << frame->topBrd2String() 
	  << frame->bottomBrd2String() << "/>" << endl;
    }
}

/*================================================================*/
int KWFrameSet::getNext(KRect _rect)
{
  for (unsigned int i = 0;i < frames.count();i++)
    {
      if (frames.at(i)->intersects(_rect))
	return i;
    }

  return -1;
}

/******************************************************************/
/* Class: KWTextFrameSet                                          */
/******************************************************************/

/*================================================================*/
void KWTextFrameSet::init()
{
  parags = 0L;

  autoCreateNewFrame = true;

  parags = new KWParag(this,doc,0L,0L,doc->getDefaultParagLayout());
  KWFormat *format = new KWFormat(doc);
  format->setDefaults(doc);

  updateCounters();
}

/*================================================================*/
void KWTextFrameSet::update()
{
  typedef QList<KWFrame> FrameList;
  QList<FrameList> frameList;
  frameList.setAutoDelete(true);

  KRect pageRect;
  for (unsigned int i = 0;i < static_cast<unsigned int>(doc->getPages() + 1);i++)
    {
      pageRect = KRect(0,i * doc->getPTPaperHeight(),doc->getPTPaperWidth(),doc->getPTPaperHeight());

      FrameList *l = new FrameList();
      l->setAutoDelete(false);
      for (unsigned int j = 0;j < frames.count();j++)
	{
	  if (frames.at(j)->intersects(pageRect))
	    {
	      frames.at(j)->setPageNum(i);
	      l->append(frames.at(j));
	    }
	}  
      
      if (!l->isEmpty())
	{
	  FrameList *ll = new FrameList();
	  ll->setAutoDelete(false);
	  ll->append(l->first());
	  unsigned int k = 0,m = 0;
	  for (k = 1;k < l->count();k++)
	    {
	      bool inserted = false;
	      for (m = 0;m < ll->count();m++)
		{ 
		  if (l->at(k)->y() < ll->at(m)->y()) 
		    {
		      inserted = true;
		      ll->insert(m,l->at(k));
		      break;
		    }
		}
	      if (!inserted) ll->append(l->at(k));
	    }
	  FrameList *l2 = new FrameList();
	  l2->setAutoDelete(false);
	  l2->append(ll->first());
	  for (k = 1;k < ll->count();k++)
	    {
	      bool inserted = false;
	      for (m = 0;m < l2->count();m++)
		{ 
		  if (ll->at(k)->x() < l2->at(m)->x()) 
		    {
		      inserted = true;
		      l2->insert(m,ll->at(k));
		      break;
		    }
		}
	      if (!inserted) l2->append(ll->at(k));
	    }
	  
	  delete ll;
	  delete l;
	  l = l2;
	}

      frameList.append(l);
    }

  frames.setAutoDelete(false);
  frames.clear();

  int rm = 0;
  for (unsigned int n = 0;n < frameList.count();n++)
    {
      for (unsigned int o = 0;o < frameList.at(n)->count();o++)
	{
	  frames.append(frameList.at(n)->at(o));
	  frames.at(frames.count() - 1)->setMostRight(false);
	  if (frames.count() > 1)
	    {
	      if (frames.at(frames.count() - 2)->right() > frames.at(frames.count() - 1)->right())
		{
		  frames.at(frames.count() - 2)->setMostRight(true);
		  rm++;
		}
	    }
	}
    }

  frames.setAutoDelete(true);
}

/*================================================================*/
void KWTextFrameSet::setFirstParag(KWParag *_parag)
{ 
  parags = _parag;
}

/*================================================================*/
KWParag* KWTextFrameSet::getFirstParag() 
{ 
  return parags;
}

/*================================================================*/
bool KWTextFrameSet::isPTYInFrame(unsigned int _frame,unsigned int _ypos)
{
  KWFrame *frame = getFrame(_frame);
  return (static_cast<int>(_ypos) >= frame->top() && static_cast<int>(_ypos) <= frame->bottom());
}

/*================================================================*/
void KWTextFrameSet::deleteParag(KWParag *_parag)
{
  KWParag *p,*p2;

  if (!getFirstParag()->getPrev() && !getFirstParag()->getNext()) return;

  if (!_parag->getPrev())
    {
      p = _parag->getNext(); 
      p->setPrev(0L);
      setFirstParag(p);
      delete _parag;
    }
  else
    {
      p = _parag->getNext();
      p2 = _parag->getPrev();
      if (p) p->setPrev(p2);
      p2->setNext(p);
      delete _parag;
    }

  updateCounters();
}

/*================================================================*/
void KWTextFrameSet::joinParag(KWParag *_parag1,KWParag *_parag2)
{
  if (!_parag1 || !_parag2) return;

  if (_parag2->getNext()) _parag2->getNext()->setPrev(_parag1);
  _parag1->setNext(_parag2->getNext());

  _parag1->appendText(_parag2->getText(),_parag2->getTextLen());

  delete _parag2;

  updateCounters();
}

/*================================================================*/
void KWTextFrameSet::insertParag(KWParag *_parag,InsertPos _pos)
{
  KWParag *_new = 0L,*_prev = 0L,*_next = 0L;

  if (_parag)
    {
      _prev = _parag->getPrev();
      _next = _parag->getNext();
    }

  switch (_pos)
    {
    case I_AFTER:
      {
	_new = new KWParag(this,doc,_parag,_next,doc->findParagLayout(_parag->getParagLayout()->getFollowingParagLayout()));
	if (_new->getParagLayout()->getName() == _parag->getParagLayout()->getName())
	  _new->setParagLayout(_parag->getParagLayout());
	if (_next) _next->setPrev(_new);
      } break;
    case I_BEFORE:
      {
	_new = new KWParag(this,doc,_prev,_parag,_parag->getParagLayout());
	if (_parag) _parag->setPrev(_new);
	if (!_prev) setFirstParag(_new);
      } break;
    }

  updateCounters();
}

/*================================================================*/
void KWTextFrameSet::splitParag(KWParag *_parag,unsigned int _pos)
{
  KWParag *_new = 0L,*_next = 0L;

  if (_parag) _next = _parag->getNext();
    
  unsigned int len = _parag->getTextLen() - _pos;
  KWChar* _string = _parag->getKWString()->split(_pos);
  _new = new KWParag(this,doc,_parag,_next,_parag->getParagLayout());
  if (_next) _next->setPrev(_new);
  
  _new->appendText(_string,len);

  updateCounters();
}

/*================================================================*/
void KWTextFrameSet::save(ostream &out)
{
  QString grp = "";
  if (grpMgr)
    {
      grp = "\" grpMgr=\"";
      grp += grpMgr->getName().copy();

      unsigned int _row = 0,_col = 0;
      grpMgr->getFrameSet(this,_row,_col);
      QString tmp = "";
      tmp.sprintf("\" row=\"%d\" col=\"%d",_row,_col);
      grp += tmp.copy();
    }

  out << otag << "<FRAMESET frameType=\"" << static_cast<int>(getFrameType()) 
      << "\" autoCreateNewFrame=\"" << autoCreateNewFrame << "\" frameInfo=\"" << static_cast<int>(frameInfo) << grp << "\">" << endl;

  KWFrameSet::save(out);

  KWParag *parag = getFirstParag();
  while (parag)
    {
      out << otag << "<PARAGRAPH>" << endl;
      parag->save(out);
      parag = parag->getNext();
      out << etag << "</PARAGRAPH>" << endl;
    }

  out << etag << "</FRAMESET>" << endl;
}

/*================================================================*/
void KWTextFrameSet::load(KOMLParser& parser,vector<KOMLAttrib>& lst)
{
  init();

  string tag;
  string name;

  KWParag *last = 0L;

  while (parser.open(0L,tag))
    {
      KOMLParser::parseTag(tag.c_str(),name,lst);
      
      // paragraph
      if (name == "PARAGRAPH")
	{    
	  KOMLParser::parseTag(tag.c_str(),name,lst);
	  vector<KOMLAttrib>::const_iterator it = lst.begin();
	  for(;it != lst.end();it++)
	    {
	    }

	  if (!last)
	    {
	      delete parags;
	      parags = new KWParag(this,doc,0L,0L,doc->getDefaultParagLayout());
	      if (doc->getNumFrameSets() == 0)
		{
		  parags->insertText(0," ");
		  KWFormat *format = new KWFormat(doc);
		  format->setDefaults(doc);
		  parags->setFormat(0,1,*format);
		}
	      parags->load(parser,lst);
	      last = parags;
	    }
	  else
	    {
	      last = new KWParag(this,doc,last,0L,doc->getDefaultParagLayout());
	      last->load(parser,lst);
	    }
	}

      else if (name == "FRAME")
	{
	  KWFrame rect;
	  KWParagLayout::Border l,r,t,b;
	  
	  l.color = rect.getBackgroundColor().color();
	  l.style = KWParagLayout::SOLID;
	  l.ptWidth = 1;
	  r.color = rect.getBackgroundColor().color();
	  r.style = KWParagLayout::SOLID;
	  r.ptWidth = 1;
	  t.color = rect.getBackgroundColor().color();
	  t.style = KWParagLayout::SOLID;
	  t.ptWidth = 1;
	  b.color = rect.getBackgroundColor().color();
	  b.style = KWParagLayout::SOLID;
	  b.ptWidth = 1;
	  
	  KOMLParser::parseTag(tag.c_str(),name,lst);
	  vector<KOMLAttrib>::const_iterator it = lst.begin();
	  for(;it != lst.end();it++)
	    {
	      if ((*it).m_strName == "left")
		rect.setLeft(atoi((*it).m_strValue.c_str()));
	      else if ((*it).m_strName == "top")
		rect.setTop(atoi((*it).m_strValue.c_str()));
	      else if ((*it).m_strName == "right")
		rect.setRight(atoi((*it).m_strValue.c_str()));
	      else if ((*it).m_strName == "bottom")
		rect.setBottom(atoi((*it).m_strValue.c_str()));
	      else if ((*it).m_strName == "runaround")
		rect.setRunAround(static_cast<RunAround>(atoi((*it).m_strValue.c_str())));
	      else if ((*it).m_strName == "runaroundGap")
		rect.setRunAroundGap(atoi((*it).m_strValue.c_str()));
	      else if ((*it).m_strName == "lWidth")
		l.ptWidth = atoi((*it).m_strValue.c_str());
	      else if ((*it).m_strName == "rWidth")
		r.ptWidth = atoi((*it).m_strValue.c_str());
	      else if ((*it).m_strName == "tWidth")
		t.ptWidth = atoi((*it).m_strValue.c_str());
	      else if ((*it).m_strName == "bWidth")
		b.ptWidth = atoi((*it).m_strValue.c_str());
	      else if ((*it).m_strName == "lRed")
		l.color.setRgb(atoi((*it).m_strValue.c_str()),l.color.green(),l.color.blue());
	      else if ((*it).m_strName == "rRed")
		r.color.setRgb(atoi((*it).m_strValue.c_str()),r.color.green(),r.color.blue());
	      else if ((*it).m_strName == "tRed")
		t.color.setRgb(atoi((*it).m_strValue.c_str()),t.color.green(),t.color.blue());
	      else if ((*it).m_strName == "bRed")
		b.color.setRgb(atoi((*it).m_strValue.c_str()),b.color.green(),b.color.blue());
	      else if ((*it).m_strName == "lGreen")
		l.color.setRgb(l.color.red(),atoi((*it).m_strValue.c_str()),l.color.blue());
	      else if ((*it).m_strName == "rGreen")
		r.color.setRgb(r.color.red(),atoi((*it).m_strValue.c_str()),r.color.blue());
	      else if ((*it).m_strName == "tGreen")
		t.color.setRgb(t.color.red(),atoi((*it).m_strValue.c_str()),t.color.blue());
	      else if ((*it).m_strName == "bGreen")
		b.color.setRgb(b.color.red(),atoi((*it).m_strValue.c_str()),b.color.blue());
	      else if ((*it).m_strName == "lBlue")
		l.color.setRgb(l.color.red(),l.color.green(),atoi((*it).m_strValue.c_str()));
	      else if ((*it).m_strName == "rBlue")
		r.color.setRgb(r.color.red(),r.color.green(),atoi((*it).m_strValue.c_str()));
	      else if ((*it).m_strName == "tBlue")
		t.color.setRgb(t.color.red(),t.color.green(),atoi((*it).m_strValue.c_str()));
	      else if ((*it).m_strName == "bBlue")
		b.color.setRgb(b.color.red(),b.color.green(),atoi((*it).m_strValue.c_str()));
	      else if ((*it).m_strName == "lStyle")
		l.style = static_cast<KWParagLayout::BorderStyle>(atoi((*it).m_strValue.c_str()));
	      else if ((*it).m_strName == "rStyle")
		r.style = static_cast<KWParagLayout::BorderStyle>(atoi((*it).m_strValue.c_str()));
	      else if ((*it).m_strName == "tStyle")
		t.style = static_cast<KWParagLayout::BorderStyle>(atoi((*it).m_strValue.c_str()));
	      else if ((*it).m_strName == "bStyle")
		b.style = static_cast<KWParagLayout::BorderStyle>(atoi((*it).m_strValue.c_str()));
	    }
	  KWFrame *_frame = new KWFrame(rect.x(),rect.y(),rect.width(),rect.height(),rect.getRunAround(),rect.getRunAroundGap());
	  _frame->setLeftBorder(l);
	  _frame->setRightBorder(r);
	  _frame->setTopBorder(t);
	  _frame->setBottomBorder(b);
	  frames.append(_frame);
	}

      else
	cerr << "Unknown tag '" << tag << "' in FRAMESET" << endl;    
      
      if (!parser.close(tag))
	{
	  cerr << "ERR: Closing Child" << endl;
	  return;
	}
    }

  updateCounters();
}

/*================================================================*/
void KWTextFrameSet::updateCounters()
{
  KWParag *p = getFirstParag();

  int counterData[16],listData[16];
  unsigned int i = 0;
  for (i = 0;i < 16;i++)
    {
      counterData[i] = -2;
      listData[i] = -2;
    }
  KWParagLayout::CounterType ct = KWParagLayout::CT_NONE;

  while (p)
    {
      if (p->getParagLayout()->getCounterType() != KWParagLayout::CT_NONE)
	{
	  if (p->getParagLayout()->getNumberingType() == KWParagLayout::NT_CHAPTER)
	    {
	      counterData[p->getParagLayout()->getCounterDepth()]++;
	      for (i = 0;i < 16;i++)
		{
		  if (counterData[i] < 0)
		    {
		      switch (p->getParagLayout()->getCounterType())
			{
			case KWParagLayout::CT_NUM: case KWParagLayout::CT_ROM_NUM_L: case KWParagLayout::CT_ROM_NUM_U:
			  counterData[i] = atoi(p->getParagLayout()->getStartCounter());
			  break;
			case KWParagLayout::CT_ALPHAB_L:
			  counterData[i] = p->getParagLayout()->getStartCounter()[0];
			  break;
			case KWParagLayout::CT_ALPHAB_U:
			  counterData[i] = p->getParagLayout()->getStartCounter()[0];
			  break;
			default: break;
			}
		    }
		  p->getCounterData()[i] = counterData[i];
		}
	      p->makeCounterText();
	      for (i = p->getParagLayout()->getCounterDepth() + 1;i < 16;i++)
		counterData[i] = -2;
	      if (listData[0] != -2)
		{
		  for (i = 0;i < 16;i++)
		    listData[i] = -2;
		}
	    }
	  else
	    {
	      if (ct != p->getParagLayout()->getCounterType())
		{
		  for (i = 0;i < 16;i++)
		    listData[i] = -2;
		}
	      ct = p->getParagLayout()->getCounterType();
	      if (p->getParagLayout()->getCounterType() != KWParagLayout::CT_BULLET)
		listData[p->getParagLayout()->getCounterDepth()]++;
	      else if (listData[0] != -2)
		{
		  for (i = 0;i < 16;i++)
		    listData[i] = -2;
		}
	      for (i = 0;i < 16;i++)
		{
		  if (listData[i] < 0)
		    {
		      switch (p->getParagLayout()->getCounterType())
			{
			case KWParagLayout::CT_NUM: case KWParagLayout::CT_ROM_NUM_L: case KWParagLayout::CT_ROM_NUM_U:
			  listData[i] = atoi(p->getParagLayout()->getStartCounter());
			  break;
			case KWParagLayout::CT_ALPHAB_L:
			  listData[i] = p->getParagLayout()->getStartCounter()[0];
			  break;
			case KWParagLayout::CT_ALPHAB_U:
			  listData[i] = p->getParagLayout()->getStartCounter()[0];
			  break;
			default: break;
			}
		    }
		  p->getCounterData()[i] = listData[i];
		}
	      p->makeCounterText();
	      for (i = p->getParagLayout()->getCounterDepth() + 1;i < 16;i++)
		listData[i] = -2;
	    }
	}
      else if (listData[0] != -2)
	{
	  for (i = 0;i < 16;i++)
	    listData[i] = -2;
    	}
      p = p->getNext();
    }
}

/*================================================================*/
void KWTextFrameSet::updateAllStyles()
{
  KWParag *p = getFirstParag();

  while (p)
    {
      if (doc->isStyleChanged(p->getParagLayout()->getName()))
	p->applyStyle(p->getParagLayout()->getName());
      p = p->getNext();
    }

  updateCounters();
}

/*================================================================*/
KWParag *KWTextFrameSet::getLastParag()
{
  KWParag *p = getFirstParag();
  KWParag *last = p;

  while (p)
    {
      last = p;
      p = p->getNext();
    }

  return last;
}

/******************************************************************/
/* Class: KWPictureFrameSet                                       */
/******************************************************************/

/*================================================================*/
void KWPictureFrameSet::setFileName(QString _filename)
{
  if (image) 
    {
      image->decRef();
      image = 0L;
    }

  filename = _filename;

  KWImage _image = KWImage(doc,filename);
  QString key;

  image = doc->getImageCollection()->getImage(_image,key);
}

/*================================================================*/
void KWPictureFrameSet::setFileName(QString _filename,KSize _imgSize)
{
  if (image) 
    {
      image->decRef();
      image = 0L;
    }

  filename = _filename;

  KWImage _image = KWImage(doc,filename);
  QString key;

  image = doc->getImageCollection()->getImage(_image,key,_imgSize);
}

/*================================================================*/
void KWPictureFrameSet::setSize(KSize _imgSize)
{
  if (image && _imgSize == image->size()) return;

  setFileName(filename,_imgSize);
}

/*================================================================*/
void KWPictureFrameSet::save(ostream &out)
{
  out << otag << "<FRAMESET frameType=\"" << static_cast<int>(getFrameType()) << "\" frameInfo=\"" 
      << static_cast<int>(frameInfo) << "\">" << endl;

  KWFrameSet::save(out);

  out << otag << "<IMAGE>" << endl;
  image->save(out);
  out << etag << "</IMAGE>" << endl;

  out << etag << "</FRAMESET>" << endl;
}

/*================================================================*/
void KWPictureFrameSet::load(KOMLParser& parser,vector<KOMLAttrib>& lst)
{
  string tag;
  string name;

  while (parser.open(0L,tag))
    {
      KOMLParser::parseTag(tag.c_str(),name,lst);

      if (name == "IMAGE")
	{    
	  KOMLParser::parseTag(tag.c_str(),name,lst);
	  vector<KOMLAttrib>::const_iterator it = lst.begin();
	  for(;it != lst.end();it++)
	    {
	    }
	  
	  KWImage *_image = new KWImage();
	  _image->load(parser,lst,doc);
	  setFileName(_image->getFilename());
	  delete _image;
	}
      else if (name == "FRAME")
	{
	  KWFrame rect;

	  KOMLParser::parseTag(tag.c_str(),name,lst);
	  vector<KOMLAttrib>::const_iterator it = lst.begin();
	  for(;it != lst.end();it++)
	    {
	      if ((*it).m_strName == "left")
		rect.setLeft(atoi((*it).m_strValue.c_str()));
	      else if ((*it).m_strName == "top")
		rect.setTop(atoi((*it).m_strValue.c_str()));
	      else if ((*it).m_strName == "right")
		rect.setRight(atoi((*it).m_strValue.c_str()));
	      else if ((*it).m_strName == "bottom")
		rect.setBottom(atoi((*it).m_strValue.c_str()));
	      else if ((*it).m_strName == "runaround")
		rect.setRunAround(static_cast<RunAround>(atoi((*it).m_strValue.c_str())));
	      else if ((*it).m_strName == "runaroundGap")
		rect.setRunAroundGap(atoi((*it).m_strValue.c_str()));
	    }
	  frames.append(new KWFrame(rect.x(),rect.y(),rect.width(),rect.height(),rect.getRunAround(),rect.getRunAroundGap()));
	}
      else
	cerr << "Unknown tag '" << tag << "' in FRAMESET" << endl;    
      
      if (!parser.close(tag))
	{
	  cerr << "ERR: Closing Child" << endl;
	  return;
	}
    }
}

/******************************************************************/
/* Class: KWPartFrameSet                                          */
/******************************************************************/

/*================================================================*/
QPicture *KWPartFrameSet::getPicture() 
{ 
  return child->draw(1.0,true); 
}

/*================================================================*/
void KWPartFrameSet::activate(QWidget *_widget,int diffx,int diffy)
{
  view->setGeometry(frames.at(0)->x() - diffx + 20,frames.at(0)->y() - diffy + 20,frames.at(0)->width(),frames.at(0)->height());
  view->show();
  view->view()->mainWindow()->setActivePart(view->view()->id());
}

/*================================================================*/
void KWPartFrameSet::deactivate()
{
  view->hide();
  view->view()->mainWindow()->setActivePart(parentID);
}

/*================================================================*/
void KWPartFrameSet::update()
{
  child->setGeometry(QRect(frames.at(0)->x(),frames.at(0)->y(),frames.at(0)->width(),frames.at(0)->height()));
}

/******************************************************************/
/* Class: KWGroupManager                                          */
/******************************************************************/

/*================================================================*/
void KWGroupManager::addFrameSet(KWFrameSet *fs,unsigned int row,unsigned int col)
{
  unsigned int sum = row * 10 + col;
  unsigned int i = 0;

  rows = max(row + 1,rows);
  cols = max(col + 1,cols);

  for (i = 0;i < cells.count();i++)
    {
      if (cells.at(i)->row * 10 + cells.at(i)->col > sum)
	break;
    }
  
  Cell *cell = new Cell;
  cell->frameSet = fs;
  cell->row = row;
  cell->col = col;

  cells.insert(i,cell);
}

/*================================================================*/
KWFrameSet *KWGroupManager::getFrameSet(unsigned int row,unsigned int col)
{
  for (unsigned int i = 0;i < cells.count();i++)
    {
      if (cells.at(i)->row == row && cells.at(i)->col == col)
	return cells.at(i)->frameSet;
    }
  
  return 0L;
}

/*================================================================*/
bool KWGroupManager::getFrameSet(KWFrameSet *fs,unsigned int &row,unsigned int &col)
{
  for (unsigned int i = 0;i < cells.count();i++)
    {
      if (cells.at(i)->frameSet == fs)
	{
	  row = cells.at(i)->row;
	  col = cells.at(i)->col;
	  return true;
	}
    }
  
  return false;
}
  
/*================================================================*/
void KWGroupManager::init(unsigned int x,unsigned int y,unsigned int width,unsigned int height)
{
  unsigned int wid = width / cols - 2;
  unsigned int hei = height / rows - 2;

  // some error checking to avoid infinite loops
  if (static_cast<int>(hei) < doc->getDefaultParagLayout()->getFormat().getPTFontSize() + 10)
    hei = doc->getDefaultParagLayout()->getFormat().getPTFontSize() + 10;
  if (wid < 60) wid = 60;

  KWFrame *frame;

  for (unsigned int i = 0;i < rows;i++)
    {
      for (unsigned int j = 0;j < cols;j++)
	{
	  frame = getFrameSet(i,j)->getFrame(0);
	  frame->setRect(x + j * wid + 2 * j,y + i * hei + 2 * i,wid,hei);
	}
    }

  for (unsigned int k = 0;k < cells.count();k++)
    doc->addFrameSet(cells.at(k)->frameSet);
}

/*================================================================*/
void KWGroupManager::init()
{
  for (unsigned int k = 0;k < cells.count();k++)
    doc->addFrameSet(cells.at(k)->frameSet);
}

/*================================================================*/
void KWGroupManager::recalcCols()
{
  for (unsigned int i = 0;i < cols;i++)
    {
      unsigned int j = 0;
      int wid = -1;
      int _wid = 100000;
      for (j = 0;j < rows;j++)
	{
	  if (getFrameSet(j,i)->getFrame(0)->isSelected())
	    wid = getFrameSet(j,i)->getFrame(0)->width();
	  _wid = min(getFrameSet(j,i)->getFrame(0)->width(),_wid);
	}
      if (wid != -1)
	{
	  if (getBoundingRect().x() + getBoundingRect().width() + (wid - _wid) > 
	      static_cast<int>(doc->getPTPaperWidth()))
	    wid = _wid;
	  for (j = 0;j < rows;j++)
	    getFrameSet(j,i)->getFrame(0)->setWidth(wid);
	}
    }

  unsigned int x = getFrameSet(0,0)->getFrame(0)->x();
  for (unsigned int i = 0;i < cols;i++)
    {
      unsigned int j = 0;
      for (j = 0;j < rows;j++)
	{
	  getFrameSet(j,i)->getFrame(0)->moveTopLeft(KPoint(x,getFrameSet(j,i)->getFrame(0)->y()));
	  getFrameSet(j,i)->update();
	}
      x = getFrameSet(0,i)->getFrame(0)->right() + 3;
    }
}

/*================================================================*/
void KWGroupManager::recalcRows(QPainter &_painter)
{
  for (unsigned int j = 0;j < rows;j++)
    {
      unsigned int i = 0;
      int hei = -1;
      int _hei = 100000;
      for (i = 0;i < cols;i++)
	{
	  if (getFrameSet(j,i)->getFrame(0)->isSelected())
	    hei = getFrameSet(j,i)->getFrame(0)->height();
	  _hei = min(getFrameSet(j,i)->getFrame(0)->height(),_hei);
	}
      if (hei != -1)
	{
	  for (i = 0;i < cols;i++)
	    getFrameSet(j,i)->getFrame(0)->setHeight(hei);
	}
    }
    
  unsigned int y = getFrameSet(0,0)->getFrame(0)->y();
  for (unsigned int j = 0;j < rows;j++)
    {
      unsigned int i = 0;
      for (i = 0;i < cols;i++)
	{
	  if (doc->getProcessingType() == KWordDocument::DTP)
	    {
	      if (j > 0 && y + getFrameSet(j,i)->getFrame(0)->height() > 
		  (getFrameSet(j - 1,i)->getPageOfFrame(0) + 1) * doc->getPTPaperHeight() - doc->getPTBottomBorder())
		y = (getFrameSet(j - 1,i)->getPageOfFrame(0) + 1) * doc->getPTPaperHeight() + doc->getPTTopBorder();
	    }
	  else
	    {
	      if (j > 0 && static_cast<int>(y + getFrameSet(j,i)->getFrame(0)->height()) > 
		  static_cast<int>((doc->getFrameSet(0)->getFrame(getFrameSet(j - 1,i)->getPageOfFrame(0))->bottom())))
		{
		  if (doc->getPages() < getFrameSet(j - 1,i)->getPageOfFrame(0) + 2)
		    doc->appendPage(doc->getPages() - 1,_painter);
		  y = doc->getFrameSet(0)->getFrame(getFrameSet(j - 1,i)->getPageOfFrame(0) + 1)->y();
		}
	    }
	  getFrameSet(j,i)->getFrame(0)->moveTopLeft(KPoint(getFrameSet(j,i)->getFrame(0)->x(),y));
	  getFrameSet(j,i)->update();
	}
      y = getFrameSet(j,0)->getFrame(0)->bottom() + 3;
    }

  if (getBoundingRect().y() + getBoundingRect().height() > 
      static_cast<int>(doc->getPTPaperHeight() * doc->getPages()))
    doc->appendPage(doc->getPages() - 1,_painter);
}

/*================================================================*/
KRect KWGroupManager::getBoundingRect()
{
  KRect r1,r2;
  KWFrame *first = getFrameSet(0,0)->getFrame(0);
  KWFrame *last = getFrameSet(rows - 1,cols - 1)->getFrame(0);
  
  r1 = KRect(first->x(),first->y(),first->width(),first->height());
  r2 = KRect(last->x(),last->y(),last->width(),last->height());
  
  r1 = r1.unite(r2);
  return KRect(r1);
}

/*================================================================*/
bool KWGroupManager::hasSelectedFrame()
{
  for (unsigned int i = 0;i < cells.count();i++)
    {
      if (cells.at(i)->frameSet->getFrame(0)->isSelected())
	return true;
    }
  
  return false;
}

/*================================================================*/
void KWGroupManager::moveBy(unsigned int dx,unsigned int dy)
{
  for (unsigned int i = 0;i < cells.count();i++)
    cells.at(i)->frameSet->getFrame(0)->moveBy(dx,dy);
}

/*================================================================*/
void KWGroupManager::drawAllRects(QPainter &p,int xOffset,int yOffset)
{
  KWFrame *frame = 0L;

  for (unsigned int i = 0;i < cells.count();i++)
    {
      frame = cells.at(i)->frameSet->getFrame(0);
      p.drawRect(frame->x() - xOffset,frame->y() - yOffset,frame->width(),frame->height());
    }
}


/*================================================================*/
void KWGroupManager::deselectAll()
{
  KWFrame *frame = 0L;

  for (unsigned int i = 0;i < cells.count();i++)
    {
      frame = cells.at(i)->frameSet->getFrame(0);
      frame->setSelected(false);
    }
}

/*================================================================*/
void KWGroupManager::selectUntil(KWFrameSet *fs)
{
  unsigned int row = 0,col = 0;
  getFrameSet(fs,row,col);

  for (unsigned int i = 0;i < cells.count();i++)
    {
      if (cells.at(i)->row <= row && cells.at(i)->col <= col)
	cells.at(i)->frameSet->getFrame(0)->setSelected(true);
      else
	cells.at(i)->frameSet->getFrame(0)->setSelected(false);
    }
}

/*================================================================*/
bool isAHeader(FrameInfo fi) 
{ 
  return (fi == FI_FIRST_HEADER || fi == FI_EVEN_HEADER || fi == FI_ODD_HEADER); 
}

/*================================================================*/
bool isAFooter(FrameInfo fi) 
{ 
  return (fi == FI_FIRST_FOOTER || fi == FI_EVEN_FOOTER || fi == FI_ODD_FOOTER); 
}

/*================================================================*/
bool isAWrongHeader(FrameInfo fi,KoHFType t)
{
  switch (fi)
    {
    case FI_FIRST_HEADER:
      {
	if (t == HF_FIRST_DIFF) return false;
	return true;
      } break;
    case FI_EVEN_HEADER:
      {
	return false;
      } break;
    case FI_ODD_HEADER:
      {
	if (t == HF_EO_DIFF) return false;
	return true;
      } break;
    default: return false;
    }

  return false;
}

/*================================================================*/
bool isAWrongFooter(FrameInfo fi,KoHFType t)
{
  switch (fi)
    {
    case FI_FIRST_FOOTER:
      {
	if (t == HF_FIRST_DIFF) return false;
	return true;
      } break;
    case FI_EVEN_FOOTER:
      {
	return false;
      } break;
    case FI_ODD_FOOTER:
      {
	if (t == HF_EO_DIFF) return false;
	return true;
      } break;
    default: return false;
    }

  return false;
}
