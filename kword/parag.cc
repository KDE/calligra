#include <string.h>
#include <stdio.h>
#include <assert.h>

#include "parag.h"
#include "kword_doc.h"

#include <koIMR.h>
#include <komlMime.h>
#include <strstream>
#include <fstream>
#include <unistd.h>

/******************************************************************/
/* Class: KWParag                                                 */
/******************************************************************/

/*================================================================*/
KWParag::KWParag(KWTextFrameSet *_frameSet,KWordDocument *_doc,KWParag* _prev,KWParag* _next,
		 KWParagLayout* _paragLayout,bool useForFirst)
  : text(_doc)
{
  prev = _prev;
  next = _next;
  paragLayout = new KWParagLayout(_doc,false);
  *paragLayout = *_paragLayout;
  document = _doc;
  frameSet = _frameSet;

  if (prev)
    prev->setNext(this);
  else if (useForFirst)
    frameSet->setFirstParag(this);

  if (next)
    next->setPrev(this);

  startPage = 1;
  startFrame = 1;
  endFrame = 1;
  ptYStart = 0;
  ptYEnd = 0;

  hardBreak = false;

  counterText = "";

  paragName.sprintf("%d",++(document->getNumParags()));

  info = PI_NONE;
}

/*================================================================*/
KWParag::KWParag(const KWParag& _parag)
  : text(_parag.document)
{
  prev = _parag.prev;
  next = _parag.next;
  document = _parag.document;
  paragLayout = new KWParagLayout(document,false);
  *paragLayout = *_parag.paragLayout;
  frameSet = _parag.frameSet;

  startPage = _parag.startPage;
  startFrame = _parag.startFrame;
  endFrame = _parag.endFrame;
  ptYStart = _parag.ptYStart;
  ptYEnd = _parag.ptYEnd;

  hardBreak = _parag.hardBreak;

  counterText = _parag.counterText;

  // I'm not sure if this is correct. Perhaps we should use the
  // name of _parag too?
  paragName.sprintf("%d",++(document->getNumParags()));

  counterData = _parag.counterData;
  counterWidth = _parag.counterWidth;

  text = _parag.text;

  info = _parag.info;
}

/*================================================================*/
KWParag::~KWParag()
{
}

/*================================================================*/
void KWParag::makeCounterText()
{
  QString buffer = "";

  switch (paragLayout->getCounterType())
    {
    case KWParagLayout::CT_BULLET:
      {
	for (int i = 0;i < paragLayout->getCounterDepth();i++)
	  buffer += "WW";

	buffer += paragLayout->getCounterBullet();
      } break;
    case KWParagLayout::CT_NUM:
      {
	buffer = paragLayout->getCounterLeftText().copy();
	QString tmp;
	for (int i = 0;i <= paragLayout->getCounterDepth();i++)
	  {
	    tmp.sprintf("%d",counterData[i]);
	    buffer += tmp;
	    if (i < paragLayout->getCounterDepth())
	      buffer += ".";
	  }
	buffer += paragLayout->getCounterRightText().copy();
      } break;
    case KWParagLayout::CT_ROM_NUM_L:
      {
	buffer = paragLayout->getCounterLeftText().copy();
	QString tmp;
	for (int i = 0;i <= paragLayout->getCounterDepth();i++)
	  {
	    tmp.sprintf("%s",makeRomanNumber(counterData[i]).lower().data());
	    buffer += tmp;
	    if (i < paragLayout->getCounterDepth())
	      buffer += ".";
	  }
	buffer += paragLayout->getCounterRightText().copy();
      } break;
    case KWParagLayout::CT_ROM_NUM_U:
      {
	buffer = paragLayout->getCounterLeftText().copy();
	QString tmp;
	for (int i = 0;i <= paragLayout->getCounterDepth();i++)
	  {
	    tmp.sprintf("%s",makeRomanNumber(counterData[i]).upper().data());
	    buffer += tmp;
	    if (i < paragLayout->getCounterDepth())
	      buffer += ".";
	  }
	buffer += paragLayout->getCounterRightText().copy();
      } break;
    case KWParagLayout::CT_ALPHAB_L:
      {
	buffer = paragLayout->getCounterLeftText().copy();
	QString tmp;
	for (int i = 0;i <= paragLayout->getCounterDepth();i++)
	  {
	    tmp.sprintf("%c",counterData[i]);
	    tmp = tmp.lower();
	    buffer += tmp;
	    if (i < paragLayout->getCounterDepth())
	      buffer += ".";
	  }
	buffer += paragLayout->getCounterRightText().copy();
      } break;
    case KWParagLayout::CT_ALPHAB_U:
      {
	buffer = paragLayout->getCounterLeftText().copy();
	QString tmp;
	for (int i = 0;i <= paragLayout->getCounterDepth();i++)
	  {
	    tmp.sprintf("%c",counterData[i]);
	    tmp = tmp.upper();
	    buffer += tmp;
	    if (i < paragLayout->getCounterDepth())
	      buffer += ".";
	  }
	buffer += paragLayout->getCounterRightText().copy();
      } break;
    default: break;
    }

  //buffer += " ";
  counterText = buffer.copy();

  makeCounterWidth();
}

/*================================================================*/
void KWParag::makeCounterWidth()
{
  QString placeholder = CounterPlaceHolder[static_cast<int>(paragLayout->getCounterType())];
  QString str = paragLayout->getCounterLeftText().copy();
  str += paragLayout->getCounterRightText().copy();

  for (int i = 0;i <= paragLayout->getCounterDepth();i++)
    str += placeholder;

  counterWidth = str.copy();
}

/*================================================================*/
void KWParag::insertText( unsigned int _pos,QString _text)
{
  text.insert(_pos,_text);
}

/*================================================================*/
void KWParag::insertText( unsigned int _pos,KWString *_text)
{
  text.insert(_pos,_text);
}

/*================================================================*/
void KWParag::insertVariable(unsigned int _pos,KWVariable *_var)
{
  KWCharVariable *v = new KWCharVariable(_var);
  text.insert(_pos,v);
}

/*================================================================*/
void KWParag::insertFootNote(unsigned int _pos,KWFootNote *_fn)
{
  KWCharFootNote *fn = new KWCharFootNote(_fn);
  text.insert(_pos,fn);
}

/*================================================================*/
void KWParag::insertPictureAsChar(unsigned int _pos,QString _filename)
{
  KWImage _image = KWImage(document,_filename);
  QString key;

  KWImage *image = document->getImageCollection()->getImage(_image,key);
  KWCharImage *i = new KWCharImage(image);

  text.insert(_pos,i);
}

/*================================================================*/
void KWParag::insertTab(unsigned int _pos)
{
  KWCharTab *_tab = new KWCharTab();
  text.insert(_pos,_tab);
}

/*================================================================*/
void KWParag::appendText(KWChar *_text,unsigned int _len)
{
  text.append(_text,_len);
}

/*================================================================*/
bool KWParag::deleteText( unsigned int _pos, unsigned int _len = 1)
{
  return text.remove(_pos,_len);
}

/*================================================================*/
void KWParag::setFormat( unsigned int _pos, unsigned int _len, const KWFormat &_format )
{
  if (text.size() == 0) return;

  assert( _pos < text.size() );

  for (unsigned int i = 0;i < _len;i++)
    {
      if (text.data()[_pos + i].c == 0)
	{
	  switch (text.data()[_pos + i].attrib->getClassId())
	    {
	    case ID_KWCharVariable:
	      {
		KWFormat *format = document->getFormatCollection()->getFormat(_format);
		dynamic_cast<KWCharVariable*>(text.data()[_pos + i].attrib)->setFormat(format);
	      } break;
	    case ID_KWCharFootNote:
	      {
		KWFormat *format = document->getFormatCollection()->getFormat(_format);
		KWFormat f(document,_format);
		if (document->getFootNoteManager().showFootNotesSuperscript())
		  f.setVertAlign(KWFormat::VA_SUPER);
		else
		  f.setVertAlign(KWFormat::VA_NORMAL);
		format = document->getFormatCollection()->getFormat(f);
		dynamic_cast<KWCharFootNote*>(text.data()[_pos + i].attrib)->setFormat(format);
	      } break;
	    default: break;
	    }
	continue;
	}
      freeChar(text.data()[_pos + i],document);
      KWFormat *format = document->getFormatCollection()->getFormat(_format);
      KWCharFormat *f = new KWCharFormat(format);
      text.data()[_pos + i].attrib = f;
    }
}

/*================================================================*/
void KWParag::save(ostream &out)
{
  out << indent << "<TEXT>" << (const char*)text.utf8() << "</TEXT>" << endl;
  if (info == PI_FOOTNOTE)
    out << indent << "<NAME name=\"" << paragName.ascii() << "\"/>" << endl;
  out << indent << "<INFO info=\"" << static_cast<int>(info) << "\"/>" << endl;
  out << indent << "<HARDBRK frame=\"" << static_cast<int>(hardBreak) << "\"/>" << endl;
  out << otag << "<FORMATS>" << endl;
  text.saveFormat(out);
  out << etag << "</FORMATS>" << endl;
  out << otag << "<LAYOUT>" << endl;
  paragLayout->save(out);
  out << etag << "</LAYOUT>" << endl;
}

/*================================================================*/
void KWParag::load(KOMLParser& parser,vector<KOMLAttrib>& lst)
{
  string tag;
  string name;
  string tmp;

  QString tmp2;

  while (parser.open(0L,tag))
    {
      KOMLParser::parseTag(tag.c_str(),name,lst);
	
      // text
      if (name == "TEXT")
	{
	  KOMLParser::parseTag(tag.c_str(),name,lst);
	  vector<KOMLAttrib>::const_iterator it = lst.begin();
	  for(;it != lst.end();it++)
	    {
	      if ((*it).m_strName == "value")
		tmp2 = (*it).m_strValue.c_str();
	    }

	   if (parser.readText(tmp))
	     {
	       QString s = tmp.c_str();
	       if (s.simplifyWhiteSpace().length() > 0)
		 tmp2 = tmp.c_str();
	     }

	   tmp2 = QString::fromUtf8(tmp2.ascii());

	   if (text.size() == 1 && tmp2.length() > 0) text.remove(0);
	   text.insert(text.size(),tmp2);
	}

      // hard break
      else if (name == "HARDBRK")
	{
	  KOMLParser::parseTag(tag.c_str(),name,lst);
	  vector<KOMLAttrib>::const_iterator it = lst.begin();
	  for(;it != lst.end();it++)
	    {
	      if ((*it).m_strName == "frame")
		hardBreak = static_cast<bool>(atoi((*it).m_strValue.c_str()));
	    }
	}

      // info
      else if (name == "INFO")
	{
	  KOMLParser::parseTag(tag.c_str(),name,lst);
	  vector<KOMLAttrib>::const_iterator it = lst.begin();
	  for(;it != lst.end();it++)
	    {
	      if ((*it).m_strName == "info")
		info = static_cast<Info>(atoi((*it).m_strValue.c_str()));
	    }
	}

      // name
      else if (name == "NAME")
	{
	  KOMLParser::parseTag(tag.c_str(),name,lst);
	  vector<KOMLAttrib>::const_iterator it = lst.begin();
	  for(;it != lst.end();it++)
	    {
	      if ((*it).m_strName == "name")
		paragName = (*it).m_strValue.c_str();
	    }
	}

      // format
      else if (name == "FORMATS")
	{
	  KOMLParser::parseTag(tag.c_str(),name,lst);
	  vector<KOMLAttrib>::const_iterator it = lst.begin();
	  for(;it != lst.end();it++)
	    {
	    }
	  text.loadFormat(parser,lst,document,frameSet);
	}

      // layout
      else if (name == "LAYOUT")
	{
	  KOMLParser::parseTag(tag.c_str(),name,lst);
	  vector<KOMLAttrib>::const_iterator it = lst.begin();
	  for(;it != lst.end();it++)
	    {
	    }
	  paragLayout->load(parser,lst);
	}

      else
	cerr << "Unknown tag '" << tag << "' in PARAGRAPH" << endl;

      if (!parser.close(tag))
	{
	  cerr << "ERR: Closing Child" << endl;
	  return;
	}
    }

  for (unsigned int i = 0;i < text.size();i++)
    {
      if (!text.data()[i].attrib)
	setFormat(i,1,paragLayout->getFormat());
    }
}

/*================================================================*/
void KWParag::applyStyle(QString _style)
{
  KWParagLayout *tmp = document->findParagLayout(_style);

  if (tmp)
    {
      KWParagLayout *pl = new KWParagLayout(document,false);
      *pl = *tmp;

      if (!document->getApplyStyleTemplate() & KWordDocument::U_INDENT)
	{
	  pl->setFirstLineLeftIndent(paragLayout->getFirstLineLeftIndent());
	  pl->setLeftIndent(paragLayout->getLeftIndent());
	  pl->setParagFootOffset(paragLayout->getParagFootOffset());
	  pl->setParagHeadOffset(paragLayout->getParagHeadOffset());
	  pl->setLineSpacing(paragLayout->getLineSpacing());
	}

      if (!document->getApplyStyleTemplate() & KWordDocument::U_BORDER)
	{
	  pl->setLeftBorder(paragLayout->getLeftBorder());
	  pl->setRightBorder(paragLayout->getRightBorder());
	  pl->setTopBorder(paragLayout->getTopBorder());
	  pl->setBottomBorder(paragLayout->getBottomBorder());
	}

      if (!document->getApplyStyleTemplate() & KWordDocument::U_NUMBERING)
	pl->setCounter(paragLayout->getCounter());

      if (!document->getApplyStyleTemplate() & KWordDocument::U_ALIGN)
	pl->setFlow(paragLayout->getFlow());

      if (document->getApplyStyleTemplate() & KWordDocument::U_FONT_FAMILY_SAME_SIZE)
	{
	  KWFormat f(document);
	  KWFormat *f2;

	  for (unsigned int i = 0;i < getTextLen();i++)
	    {
	      f2 = ((KWCharFormat*)text.data()[i].attrib)->getFormat();
	      if (f2->getPTFontSize() == paragLayout->getFormat().getPTFontSize() &&
		  f2->getUserFont()->getFontName() == paragLayout->getFormat().getUserFont()->getFontName())
		{
		  f = *f2;
		  f.setUserFont(pl->getFormat().getUserFont());
		  freeChar(text.data()[i],document);
		  KWFormat *format = document->getFormatCollection()->getFormat(f);
		  KWCharFormat *fm = new KWCharFormat(format);
		  text.data()[i].attrib = fm;
		}
	    }
	}

      if (document->getApplyStyleTemplate() & KWordDocument::U_FONT_FAMILY_ALL_SIZE)
	{
	  KWFormat f(document);
	  KWFormat *f2;

	  for (unsigned int i = 0;i < getTextLen();i++)
	    {
	      f2 = ((KWCharFormat*)text.data()[i].attrib)->getFormat();
	      if (f2->getUserFont()->getFontName() == paragLayout->getFormat().getUserFont()->getFontName())
		{
		  f = *f2;
		  f.setUserFont(pl->getFormat().getUserFont());
		  freeChar(text.data()[i],document);
		  KWFormat *format = document->getFormatCollection()->getFormat(f);
		  KWCharFormat *fm = new KWCharFormat(format);
		  text.data()[i].attrib = fm;
		}
	    }
	}

      if (document->getApplyStyleTemplate() & KWordDocument::U_FONT_ALL_SAME_SIZE)
	{
	  KWFormat f(document);
	  KWFormat *f2;

	  for (unsigned int i = 0;i < getTextLen();i++)
	    {
	      f2 = ((KWCharFormat*)text.data()[i].attrib)->getFormat();
	      if (f2->getPTFontSize() == paragLayout->getFormat().getPTFontSize() &&
		  f2->getUserFont()->getFontName() == paragLayout->getFormat().getUserFont()->getFontName())
		{
		  QColor c = paragLayout->getFormat().getColor();
		  f = pl->getFormat();
		  f.setColor(c);
		  freeChar(text.data()[i],document);
		  KWFormat *format = document->getFormatCollection()->getFormat(f);
		  KWCharFormat *fm = new KWCharFormat(format);
		  text.data()[i].attrib = fm;
		}
	    }
	}

      if (document->getApplyStyleTemplate() & KWordDocument::U_FONT_ALL_ALL_SIZE)
	{
	  KWFormat f(document);
	  KWFormat *f2;

	  for (unsigned int i = 0;i < getTextLen();i++)
	    {
	      f2 = ((KWCharFormat*)text.data()[i].attrib)->getFormat();
	      if (f2->getUserFont()->getFontName() == paragLayout->getFormat().getUserFont()->getFontName())
		{
		  QColor c = paragLayout->getFormat().getColor();
		  f = pl->getFormat();
		  f.setColor(c);
		  freeChar(text.data()[i],document);
		  KWFormat *format = document->getFormatCollection()->getFormat(f);
		  KWCharFormat *fm = new KWCharFormat(format);
		  text.data()[i].attrib = fm;
		}
	    }
	}

      if (!document->getApplyStyleTemplate() & KWordDocument::U_COLOR)
	{
	  QColor c = paragLayout->getFormat().getColor();
	  pl->getFormat().setColor(c);
	}
      else
	{
	  QColor c = tmp->getFormat().getColor();
	  pl->getFormat().setColor(c);
	}

      if (!document->getApplyStyleTemplate() & KWordDocument::U_TABS)
	pl->setTabList(paragLayout->getTabList());

      delete paragLayout;
      paragLayout = pl;
    }
}

/*================================================================*/
void KWParag::tabListChanged(QList<KoTabulator>* _tabList)
{
  paragLayout->setTabList(_tabList);
}

/*================================================================*/
int KWParag::find(QString _expr,KWSearchDia::KWSearchEntry *_format,int _index,bool _cs,bool _whole)
{
  return text.find(_expr,_format,_index,_cs,_whole);
}

/*================================================================*/
int KWParag::find(QRegExp _regexp,KWSearchDia::KWSearchEntry *_format,int _index,int &_len,bool _cs,bool _wildcard = false)
{
  return text.find(_regexp,_format,_index,_len,_cs,_wildcard);
}

/*================================================================*/
int KWParag::findRev(QString _expr,KWSearchDia::KWSearchEntry *_format,int _index,bool _cs,bool _whole)
{
  return text.findRev(_expr,_format,_index,_cs,_whole);
}

/*================================================================*/
int KWParag::findRev(QRegExp _regexp,KWSearchDia::KWSearchEntry *_format,int _index,int &_len,bool _cs,bool _wildcard = false)
{
  return text.findRev(_regexp,_format,_index,_len,_cs,_wildcard);
}

/*================================================================*/
void KWParag::replace(int _pos,int _len,QString _text,KWFormat &_format)
{
  deleteText(_pos,_len);
  insertText(_pos,_text);
  setFormat(_pos,_text.length(),_format);
}
