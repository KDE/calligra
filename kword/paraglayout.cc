#include "paraglayout.h"
#include "kword_doc.h"

#include <koIMR.h>
#include <komlMime.h>
#include <strstream>
#include <fstream>
#include <unistd.h>

KWParagLayout::KWParagLayout( KWordDocument *_doc, bool _add = true )
  : format(_doc)
{
    flow = LEFT;
    mmParagFootOffset = 0;
    mmParagHeadOffset = 0;
    mmFirstLineLeftIndent = 0;
    mmLeftIndent = 0;
    counter.counterType = CT_NONE;
    counter.counterDepth = 0;
    counter.counterBullet = '-';
    counter.counterLeftText = "";
    counter.counterRightText = "";
    followingParagLayout = this;
    //numberLikeParagLayout = 0L;
    ptLineSpacing = 0;
    counter.startCounter = "";
    counter.numberingType = NT_LIST;
    counter.bulletFont = "symbol";

    left.color = white;
    left.style = SOLID;
    left.ptWidth = 0;
    right.color = white;
    right.style = SOLID;
    right.ptWidth = 0;
    top.color = white;
    top.style = SOLID;
    top.ptWidth = 0;
    bottom.color = white;
    bottom.style = SOLID;
    bottom.ptWidth = 0;

    format.setDefaults(_doc);
    
    document = _doc;
    if (_add)
      document->paragLayoutList.append(this);
    document->paragLayoutList.setAutoDelete(true);
}

KWParagLayout::~KWParagLayout()
{
    document->paragLayoutList.removeRef( this );
}

KWParagLayout& KWParagLayout::operator=(KWParagLayout &_layout)
{
  flow = _layout.getFlow();
  mmParagFootOffset = _layout.getMMParagFootOffset();
  mmParagHeadOffset = _layout.getMMParagHeadOffset();
  mmFirstLineLeftIndent = _layout.getMMFirstLineLeftIndent();
  mmLeftIndent = _layout.getMMLeftIndent();
  counter.counterType = static_cast<CounterType>(_layout.getCounterType());
  counter.counterDepth = _layout.getCounterDepth();
  counter.counterBullet = _layout.getCounterBullet();
  counter.counterLeftText = qstrdup(_layout.getCounterLeftText());
  counter.counterRightText = qstrdup(_layout.getCounterRightText());
  followingParagLayout = this;
  ptLineSpacing = _layout.getPTLineSpacing();
  counter.startCounter = _layout.getStartCounter();
  counter.numberingType = _layout.getNumberingType();
  counter.bulletFont = _layout.getBulletFont();

  left = _layout.getLeftBorder();
  right = _layout.getRightBorder();
  top = _layout.getTopBorder();
  bottom = _layout.getBottomBorder();

  format.setDefaults( document );

  return *this;
}

void KWParagLayout::setFollowingParagLayout( const char *_name )
{
    KWParagLayout* p = document->findParagLayout( _name );
    if ( p == 0L )
	followingParagLayout = this;
    else
	followingParagLayout = p;
}

// void KWParagLayout::setNumberLikeParagLayout( const char *_name )
// {
//     KWParagLayout* p = document->findParagLayout( _name );
//     if ( p == 0L )
// 	numberLikeParagLayout = this;
//     else
// 	numberLikeParagLayout = p;
// }

void KWParagLayout::save(ostream &out)
{
  out << indent << "<NAME value=\"" << name << "\"/>" << endl;
  out << indent << "<FLOW value=\"" << static_cast<int>(flow) << "\"/>" << endl;
  out << indent << "<OFFSETS head=\"" << mmParagHeadOffset << "\" foot=\"" << mmParagFootOffset << "\"/>" << endl;
  out << indent << "<INDENTS first=\"" << mmFirstLineLeftIndent << "\" left=\"" << mmLeftIndent << "\"/>" << endl;
  out << indent << "<COUNTER type=\"" << static_cast<int>(counter.counterType) << "\" depth=\"" << counter.counterDepth 
      << "\" bullet=\"" << counter.counterBullet << "\" start=\"" << counter.startCounter << "\" numberingtype=\"" 
      << static_cast<int>(counter.numberingType) << "\" lefttext=\"" << counter.counterLeftText << "\" righttext=\"" 
      << counter.counterRightText << "\" bulletfont=\"" << counter.bulletFont << "\"/>" << endl;
  out << indent << "<LINESPACING value=\"" << ptLineSpacing << "\"/>" << endl;
  out << indent << "<LEFTBORDER red=\"" << left.color.red() << "\" green=\"" << left.color.green() << "\" blue=\""
      << left.color.blue() << "\" style=\"" << static_cast<int>(left.style) << "\" width=\"" << left.ptWidth << "\"/>" << endl; 
  out << indent << "<RIGHTBORDER red=\"" << right.color.red() << "\" green=\"" << right.color.green() << "\" blue=\""
      << right.color.blue() << "\" style=\"" << static_cast<int>(right.style) << "\" width=\"" << right.ptWidth << "\"/>" << endl; 
  out << indent << "<TOPBORDER red=\"" << top.color.red() << "\" green=\"" << top.color.green() << "\" blue=\""
      << top.color.blue() << "\" style=\"" << static_cast<int>(top.style) << "\" width=\"" << top.ptWidth << "\"/>" << endl; 
  out << indent << "<BOTTOMBORDER red=\"" << bottom.color.red() << "\" green=\"" << bottom.color.green() << "\" blue=\""
      << bottom.color.blue() << "\" style=\"" << static_cast<int>(bottom.style) << "\" width=\"" << bottom.ptWidth << "\"/>" << endl; 
}

void KWParagLayout::load(KOMLParser& parser,vector<KOMLAttrib>& lst)
{
  string tag;
  string _name;

  while (parser.open(0L,tag))
    {
      KOMLParser::parseTag(tag.c_str(),_name,lst);
	      
      // text
      if (_name == "NAME")
	{
	  KOMLParser::parseTag(tag.c_str(),_name,lst);
	  vector<KOMLAttrib>::const_iterator it = lst.begin();
	  for(;it != lst.end();it++)
	    {
	      if ((*it).m_strName == "value")
		name = (*it).m_strValue.c_str();
	    }
	}

      // text
      else if (_name == "FLOW")
	{
	  KOMLParser::parseTag(tag.c_str(),_name,lst);
	  vector<KOMLAttrib>::const_iterator it = lst.begin();
	  for(;it != lst.end();it++)
	    {
	      if ((*it).m_strName == "value")
		flow = static_cast<Flow>(atoi((*it).m_strValue.c_str()));
	    }
	}

      // offsets
      else if (_name == "OFFSETS")
	{
	  KOMLParser::parseTag(tag.c_str(),_name,lst);
	  vector<KOMLAttrib>::const_iterator it = lst.begin();
	  for(;it != lst.end();it++)
	    {
	      if ((*it).m_strName == "head")
		mmParagHeadOffset = atoi((*it).m_strValue.c_str());
	      else if ((*it).m_strName == "foot")
		mmParagFootOffset = atoi((*it).m_strValue.c_str());
	    }
	}

      // indents
      else if (_name == "INDENTS")
	{
	  KOMLParser::parseTag(tag.c_str(),_name,lst);
	  vector<KOMLAttrib>::const_iterator it = lst.begin();
	  for(;it != lst.end();it++)
	    {
	      if ((*it).m_strName == "first")
		mmFirstLineLeftIndent = atoi((*it).m_strValue.c_str());
	      else if ((*it).m_strName == "left")
		mmLeftIndent = atoi((*it).m_strValue.c_str());
	    }
	}

      // counter
      else if (_name == "COUNTER")
	{
	  KOMLParser::parseTag(tag.c_str(),_name,lst);
	  vector<KOMLAttrib>::const_iterator it = lst.begin();
	  for(;it != lst.end();it++)
	    {
	      if ((*it).m_strName == "type")
		counter.counterType = static_cast<CounterType>(atoi((*it).m_strValue.c_str()));
	      else if ((*it).m_strName == "depth")
		counter.counterDepth = atoi((*it).m_strValue.c_str());
	      else if ((*it).m_strName == "bullet")
		counter.counterBullet = atoi((*it).m_strValue.c_str());
	      else if ((*it).m_strName == "lefttext")
		counter.counterLeftText = (*it).m_strValue.c_str();
	      else if ((*it).m_strName == "righttext")
		counter.counterRightText = (*it).m_strValue.c_str();
	      else if ((*it).m_strName == "start")
		counter.startCounter = (*it).m_strValue.c_str();
	      else if ((*it).m_strName == "numberingtype")
		counter.numberingType = static_cast<NumType>(atoi((*it).m_strValue.c_str()));
	      else if ((*it).m_strName == "bulletfont")
		counter.bulletFont = (*it).m_strValue.c_str();
	    }
	}

      // line spacing
      else if (_name == "LINESPACING")
	{
	  KOMLParser::parseTag(tag.c_str(),_name,lst);
	  vector<KOMLAttrib>::const_iterator it = lst.begin();
	  for(;it != lst.end();it++)
	    {
	      if ((*it).m_strName == "value")
		ptLineSpacing = atoi((*it).m_strValue.c_str());
	    }
	}

      // left border
      else if (_name == "LEFTBORDER")
	{
	  unsigned int r = 0,g = 0,b = 0;
	  KOMLParser::parseTag(tag.c_str(),_name,lst);
	  vector<KOMLAttrib>::const_iterator it = lst.begin();
	  for(;it != lst.end();it++)
	    {
	      if ((*it).m_strName == "red")
		{
		  r = atoi((*it).m_strValue.c_str());
		  left.color.setRgb(r,g,b);
		}
	      else if ((*it).m_strName == "green")
		{
		  g = atoi((*it).m_strValue.c_str());
		  left.color.setRgb(r,g,b);
		}
	      else if ((*it).m_strName == "blue")
		{
		  b = atoi((*it).m_strValue.c_str());
		  left.color.setRgb(r,g,b);
		}
	      else if ((*it).m_strName == "style")
		left.style = static_cast<BorderStyle>(atoi((*it).m_strValue.c_str()));
	      else if ((*it).m_strName == "width")
		left.ptWidth = atoi((*it).m_strValue.c_str());
	    }
	}

      // right border
      else if (_name == "RIGHTBORDER")
	{
	  unsigned int r = 0,g = 0,b = 0;
	  KOMLParser::parseTag(tag.c_str(),_name,lst);
	  vector<KOMLAttrib>::const_iterator it = lst.begin();
	  for(;it != lst.end();it++)
	    {
	      if ((*it).m_strName == "red")
		{
		  r = atoi((*it).m_strValue.c_str());
		  right.color.setRgb(r,g,b);
		}
	      else if ((*it).m_strName == "green")
		{
		  g = atoi((*it).m_strValue.c_str());
		  right.color.setRgb(r,g,b);
		}
	      else if ((*it).m_strName == "blue")
		{
		  b = atoi((*it).m_strValue.c_str());
		  right.color.setRgb(r,g,b);
		}
	      else if ((*it).m_strName == "style")
		right.style = static_cast<BorderStyle>(atoi((*it).m_strValue.c_str()));
	      else if ((*it).m_strName == "width")
		right.ptWidth = atoi((*it).m_strValue.c_str());
	    }
	}

      // bottom border
      else if (_name == "BOTTOMBORDER")
	{
	  unsigned int r = 0,g = 0,b = 0;
	  KOMLParser::parseTag(tag.c_str(),_name,lst);
	  vector<KOMLAttrib>::const_iterator it = lst.begin();
	  for(;it != lst.end();it++)
	    {
	      if ((*it).m_strName == "red")
		{
		  r = atoi((*it).m_strValue.c_str());
		  bottom.color.setRgb(r,g,b);
		}
	      else if ((*it).m_strName == "green")
		{
		  g = atoi((*it).m_strValue.c_str());
		  bottom.color.setRgb(r,g,b);
		}
	      else if ((*it).m_strName == "blue")
		{
		  b = atoi((*it).m_strValue.c_str());
		  bottom.color.setRgb(r,g,b);
		}
	      else if ((*it).m_strName == "style")
		bottom.style = static_cast<BorderStyle>(atoi((*it).m_strValue.c_str()));
	      else if ((*it).m_strName == "width")
		bottom.ptWidth = atoi((*it).m_strValue.c_str());
	    }
	}

      // top border
      else if (_name == "TOPBORDER")
	{
	  unsigned int r = 0,g = 0,b = 0;
	  KOMLParser::parseTag(tag.c_str(),_name,lst);
	  vector<KOMLAttrib>::const_iterator it = lst.begin();
	  for(;it != lst.end();it++)
	    {
	      if ((*it).m_strName == "red")
		{
		  r = atoi((*it).m_strValue.c_str());
		  top.color.setRgb(r,g,b);
		}
	      else if ((*it).m_strName == "green")
		{
		  g = atoi((*it).m_strValue.c_str());
		  top.color.setRgb(r,g,b);
		}
	      else if ((*it).m_strName == "blue")
		{
		  b = atoi((*it).m_strValue.c_str());
		  top.color.setRgb(r,g,b);
		}
	      else if ((*it).m_strName == "style")
		top.style = static_cast<BorderStyle>(atoi((*it).m_strValue.c_str()));
	      else if ((*it).m_strName == "width")
		top.ptWidth = atoi((*it).m_strValue.c_str());
	    }
	}

      else
	cerr << "Unknown tag '" << tag << "' in PARAGRAPHLAYOUT" << endl;    
      
      if (!parser.close(tag))
	{
	  cerr << "ERR: Closing Child" << endl;
	  return;
	}
    }
}
