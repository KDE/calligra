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

KWParag::KWParag(KWTextFrameSet *_frameSet,KWordDocument *_doc, KWParag* _prev, KWParag* _next, KWParagLayout* _paragLayout )
{
  prev = _prev;
  next = _next;
  paragLayout = new KWParagLayout(_doc,false);
  *paragLayout = *_paragLayout;
  document = _doc;
  frameSet = _frameSet;
  
  if (prev)
    prev->setNext(this);
  else
    frameSet->setFirstParag(this);
    
  if (next)
    next->setPrev(this);

  startPage = 1;
  startFrame = 1;
  endFrame = 1;
  ptYStart = 0;
  ptYEnd = 0;

  counterText = "";
}

KWParag::~KWParag()
{
}

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

void KWParag::makeCounterWidth()
{
  QString placeholder = CounterPlaceHolder[static_cast<int>(paragLayout->getCounterType())];
  QString str = paragLayout->getCounterLeftText().copy();
  str += paragLayout->getCounterRightText().copy();

  for (int i = 0;i <= paragLayout->getCounterDepth();i++)
    str += placeholder;

  counterWidth = str.copy();
}

void KWParag::insertText( unsigned int _pos,QString _text)
{
  text.insert(_pos,_text);
}

void KWParag::insertPictureAsChar(unsigned int _pos,QString _filename)
{
  KWImage _image = KWImage(document,_filename);
  QString key;

  KWImage *image = document->getImageCollection()->getImage(_image,key);
  KWCharImage *i = new KWCharImage(image);

  text.insert(_pos,i);
}

void KWParag::insertTab(unsigned int _pos)
{
  KWCharTab *_tab = new KWCharTab();
  text.insert(_pos,_tab);
}

void KWParag::appendText(KWChar *_text,unsigned int _len)
{
  text.append(_text,_len);
}

bool KWParag::deleteText( unsigned int _pos, unsigned int _len = 1)
{
  return text.remove(_pos,_len);
}

void KWParag::setFormat( unsigned int _pos, unsigned int _len, const KWFormat &_format )
{
  assert( _pos < text.size() );
  
  for (unsigned int i = 0;i < _len;i++)
    {
      if (text.data()[_pos + i].c == 0) continue;
      freeChar(text.data()[_pos + i]);
      KWFormat *format = document->getFormatCollection()->getFormat(_format);
      KWCharFormat *f = new KWCharFormat(format);
      text.data()[_pos + i].attrib = f;
    }
}

void KWParag::save(ostream &out)
{
  out << indent << "<TEXT>" << text << "</TEXT>" << endl;
  out << otag << "<FORMATS>" << endl;
  text.saveFormat(out);
  out << etag << "</FORMATS>" << endl;
  out << otag << "<LAYOUT>" << endl;
  paragLayout->save(out);
  out << etag << "</LAYOUT>" << endl;
}

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

	   if (text.size() == 1 && tmp2.length() > 0) text.remove(0);
	   text.insert(text.size(),tmp2);
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

void KWParag::applyStyle(QString _style)
{
  KWParagLayout *tmp = document->findParagLayout(_style);

  if (tmp)
    {
      KWParagLayout *pl = new KWParagLayout(document,false);
      *pl = *tmp;

      if (!document->getApplyStyleTemplate() & KWordDocument::U_INDENT)
	{
	  pl->setMMFirstLineLeftIndent(paragLayout->getMMFirstLineLeftIndent());
	  pl->setMMLeftIndent(paragLayout->getMMLeftIndent());
	  pl->setMMParagFootOffset(paragLayout->getMMParagFootOffset());
	  pl->setMMParagHeadOffset(paragLayout->getMMParagHeadOffset());
	  pl->setPTLineSpacing(paragLayout->getPTLineSpacing());
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
		  freeChar(text.data()[i]);
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
		  freeChar(text.data()[i]);
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
		  freeChar(text.data()[i]);
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
		  freeChar(text.data()[i]);
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

void KWParag::tabListChanged(QList<KoTabulator>* _tabList)
{
  paragLayout->setTabList(_tabList);
}



