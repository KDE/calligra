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
#include "frame.h"

#include <koIMR.h>
#include <komlMime.h>
#include <strstream>
#include <fstream>
#include <unistd.h>

/******************************************************************/
/* Class: KWFrameSet                                              */
/******************************************************************/

/*================================================================*/
KWFrameSet::KWFrameSet(KWordDocument_impl *_doc)
  : frames()
{ 
  doc = _doc; 
  frames.setAutoDelete(true); 
}

/*================================================================*/
void KWFrameSet::addFrame(KWFrame _rect)
{
  frames.append(new KWFrame(_rect));
  if (frames.count() == 1) init();
}

/*================================================================*/
void KWFrameSet::delFrame(unsigned int _num)
{
  frames.remove(_num);
}

/*================================================================*/
int KWFrameSet::getFrame(int _x,int _y)
{
  for (unsigned int i = 0;i < getNumFrames();i++)
    {
      if (getFrame(i).contains(QPoint(_x,_y))) return i;
    }
  return -1;
}

/*================================================================*/
KWFrame KWFrameSet::getFrame(unsigned int _num)
{
  return *frames.at(_num);
}

/*================================================================*/
KWFrame *KWFrameSet::getFramePtr(unsigned int _num)
{
  return frames.at(_num);
}

/*================================================================*/
bool KWFrameSet::contains(unsigned int mx,unsigned int my)
{
  for (unsigned int i = 0;i < frames.count();i++)
    {
      if (frames.at(i)->contains(QPoint(mx,my))) return true;
    }

  return false;
}

/*================================================================*/
void KWFrameSet::save(ostream &out)
{
  KWFrame frame;

  for (unsigned int i = 0;i < frames.count();i++)
    {
      frame = getFrame(i);
      out << indent << "<FRAME left=\"" << frame.left() << "\" top=\"" << frame.top()
	  << "\" right=\"" << frame.right() << "\" bottom=\"" << frame.bottom() 
	  << "\" runaround=\"" << static_cast<int>(frame.getRunAround()) << "\"/>" << endl;
    }
}

/******************************************************************/
/* Class: KWTextFrameSet                                          */
/******************************************************************/

/*================================================================*/
void KWTextFrameSet::init()
{
  defaultParagLayout = new KWParagLayout(doc);
  defaultParagLayout->setName("Standard");
  defaultParagLayout->setCounterNr(-1);

  parags = 0L;

  parags = new KWParag(this,doc,0L,0L,defaultParagLayout);
  parags->insertText(0," ");
  KWFormat *format = new KWFormat(doc);
  format->setDefaults(doc);
  parags->setFormat(0,1,*format);

//   for (int i = 0;i < 300;i++)
//     {
//       p = new KWParag( this,doc, p, 0L, defaultParagLayout );
//       p->insertText( 0, "Hallo Tester, ich frage mich manchmal, ob das alles so in Ordnung ist, ich meine, dass ich hier so einen Mist erzaehle, in meiner eigenen Textverarbeitung. Und noch mehr dummes Gesülze auf diesem Äther. Ich liebe dummes Geschwätz! Jetzt langt es aber für den 2. Paragraphen. Und noch mehr dummes Gesülze auf diesem Äther. Ich liebe dummes Geschwätz! Jetzt langt es aber für den 2. Paragraphen. Und noch mehr dummes Gesülze auf diesem Äther. Ich liebe dummes Geschwätz! Jetzt langt es aber für den 2. Paragraphen.");
//       KWFormat *format = new KWFormat(doc);
//       format->setDefaults(doc);
//       p->setFormat(0,strlen("Hallo Tester, ich frage mich manchmal, ob das alles so in Ordnung ist, ich meine, dass ich hier so einen Mist erzaehle, in meiner eigenen Textverarbeitung. Und noch mehr dummes Gesülze auf diesem Äther. Ich liebe dummes Geschwätz! Jetzt langt es aber für den 2. Paragraphen. Und noch mehr dummes Gesülze auf diesem Äther. Ich liebe dummes Geschwätz! Jetzt langt es aber für den 2. Paragraphen. Und noch mehr dummes Gesülze auf diesem Äther. Ich liebe dummes Geschwätz! Jetzt langt es aber für den 2. Paragraphen."),*format);
//     }
}

/*================================================================*/
void KWTextFrameSet::update()
{
}

/*================================================================*/
bool KWTextFrameSet::isPTYInFrame(unsigned int _frame,unsigned int _ypos)
{
  KWFrame frame = getFrame(_frame);
  return (static_cast<int>(_ypos) >= frame.top() && static_cast<int>(_ypos) <= frame.bottom());
}

/*================================================================*/
void KWTextFrameSet::deleteParag(KWParag *_parag)
{
  KWParag *p,*p2;

  if (!parags->getPrev() && !parags->getNext()) return;

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
}

/*================================================================*/
void KWTextFrameSet::joinParag(KWParag *_parag1,KWParag *_parag2)
{
  if (!_parag1 || !_parag2) return;

  if (_parag2->getNext()) _parag2->getNext()->setPrev(_parag1);
  _parag1->setNext(_parag2->getNext());

  _parag1->appendText(_parag2->getText(),_parag2->getTextLen());

  delete _parag2;
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
	_new = new KWParag(this,doc,_parag,_next,_parag->getParagLayout());
	if (_next) _next->setPrev(_new);
      } break;
    case I_BEFORE:
      {
	_new = new KWParag(this,doc,_prev,_parag,_parag->getParagLayout());
	if (_parag) _parag->setPrev(_new);
	else setFirstParag(_new);
      } break;
    }
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
}

/*================================================================*/
void KWTextFrameSet::save(ostream &out)
{
  out << otag << "<FRAMESET frameType=\"" << static_cast<int>(getFrameType()) << "\">" << endl;

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
	      parags = new KWParag(this,doc,0L,0L,defaultParagLayout);
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
	      last = new KWParag(this,doc,last,0L,defaultParagLayout);
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
	    }
	  frames.append(new KWFrame(rect));
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
