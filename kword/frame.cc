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

/******************************************************************/
/* Class: KWFrame                                                 */
/******************************************************************/

/*================================================================*/
KWFrame::KWFrame() 
  : KRect(), intersections()
{ 
  runAround = RA_NO; 
  intersections.setAutoDelete(true); 
  selected = false;
  runAroundGap = 1;
  mostRight = false;
}

/*================================================================*/
KWFrame::KWFrame(const KPoint &topleft,const QPoint &bottomright) 
  : KRect(topleft,bottomright), intersections() 
{ 
  runAround = RA_NO; 
  intersections.setAutoDelete(true); 
  selected = false;
  runAroundGap = 1;
  mostRight = false;
} 

/*================================================================*/
KWFrame::KWFrame(const KPoint &topleft,const KSize &size) 
  : KRect(topleft,size), intersections()
{ 
  runAround = RA_NO; 
  intersections.setAutoDelete(true); 
  selected = false;
  runAroundGap = 1;
  mostRight = false;
}    

/*================================================================*/
KWFrame::KWFrame(int left,int top,int width,int height) 
  : KRect(left,top,width,height), intersections() 
{ 
  runAround = RA_NO; 
  intersections.setAutoDelete(true); 
  selected = false;
  runAroundGap = 1;
  mostRight = false;
}

/*================================================================*/
KWFrame::KWFrame(int left,int top,int width,int height,RunAround _ra,int _gap) 
  : KRect(left,top,width,height), intersections() 
{ 
  runAround = _ra; 
  intersections.setAutoDelete(true); 
  selected = false;
  runAroundGap = _gap;
  mostRight = false;
}

/*================================================================*/
KWFrame::KWFrame(const QRect &_rect)
  : KRect(_rect)
{
  runAround = RA_NO; 
  intersections.setAutoDelete(true); 
  selected = false;
  runAroundGap = 1;
  mostRight = false;
}

/*================================================================*/
void KWFrame::addIntersect(KRect _r)
{ 
  intersections.append(new KRect(_r)); 
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
  if (runAround == RA_NO|| intersections.isEmpty()) return 0;

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
QCursor KWFrame::getMouseCursor(int mx,int my)
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

  return arrowCursor;
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

  if (!getFrame(frame)->isSelected()) return arrowCursor;

  return getFrame(frame)->getMouseCursor(mx,my);
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
	  << "\" runaroundGap=\"" << frame->getRunAroundGap() << "\"/>" << endl;
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
  //parags->insertText(0," ");
  KWFormat *format = new KWFormat(doc);
  format->setDefaults(doc);
  //parags->setFormat(0,1,*format);

//   KWParag *p = parags;
//   for (int i = 0;i < 500;i++)
//     {
//       p = new KWParag( this,doc, p, 0L, doc->getDefaultParagLayout());
//       p->insertText( 0, "Hallo Tester, ich frage mich manchmal, ob das alles so in Ordnung ist, ich meine, dass ich hier so einen Mist erzaehle, in meiner eigenen Textverarbeitung. Und noch mehr dummes Gesülze auf diesem Äther. Ich liebe dummes Geschwätz! Jetzt langt es aber für den 2. Paragraphen. Und noch mehr dummes Gesülze auf diesem Äther. Ich liebe dummes Geschwätz! Jetzt langt es aber für den 2. Paragraphen. Und noch mehr dummes Gesülze auf diesem Äther. Ich liebe dummes Geschwätz! Jetzt langt es aber für den 2. Paragraphen.");
//       KWFormat *format = new KWFormat(doc);
//       format->setDefaults(doc);
//       p->setFormat(0,strlen("Hallo Tester, ich frage mich manchmal, ob das alles so in Ordnung ist, ich meine, dass ich hier so einen Mist erzaehle, in meiner eigenen Textverarbeitung. Und noch mehr dummes Gesülze auf diesem Äther. Ich liebe dummes Geschwätz! Jetzt langt es aber für den 2. Paragraphen. Und noch mehr dummes Gesülze auf diesem Äther. Ich liebe dummes Geschwätz! Jetzt langt es aber für den 2. Paragraphen. Und noch mehr dummes Gesülze auf diesem Äther. Ich liebe dummes Geschwätz! Jetzt langt es aber für den 2. Paragraphen."),*format);
//     }

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

      //debug("%d %d %d %d",pageRect.x(),pageRect.y(),pageRect.width(),pageRect.height());

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
	  _new->getParagLayout()->setTabList(_parag->getParagLayout()->getTabList());
	if (_next) _next->setPrev(_new);
      } break;
    case I_BEFORE:
      {
	_new = new KWParag(this,doc,_prev,_parag,_parag->getParagLayout());
	if (_parag) _parag->setPrev(_new);
	else setFirstParag(_new);
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
  out << otag << "<FRAMESET frameType=\"" << static_cast<int>(getFrameType()) 
      << "\" autoCreateNewFrame=\"" << autoCreateNewFrame << "\" frameInfo=\"" << frameInfo << "\">" << endl;

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
  out << otag << "<FRAMESET frameType=\"" << static_cast<int>(getFrameType()) << "\" frameInfo=\"" << frameInfo << "\">" << endl;

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
  return child->draw(); 
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
