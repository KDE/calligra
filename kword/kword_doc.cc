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
/* Module: Document (header)                                      */
/******************************************************************/

#include <qprinter.h>
#include "kword_doc.h"
#include "kword_page.h"

#include <koIMR.h>
#include <komlMime.h>
#include <koStream.h>
#include <komlParser.h>
#include <komlStreamFeed.h>
#include <komlWriter.h>

#include <kurl.h>

#include <qmsgbox.h>
#include <qcolor.h>

#include <strstream>
#include <fstream>
#include <unistd.h>

#include "kword_doc.moc"

/******************************************************************/
/* Class: KWordChild                                              */
/******************************************************************/

/*================================================================*/
KWordChild::KWordChild(KWordDocument_impl *_wdoc,const QRect& _rect,OPParts::Document_ptr _doc)
  : KoDocumentChild(_rect,_doc)
{
  m_pKWordDoc = _wdoc;
}

/*================================================================*/
KWordChild::KWordChild(KWordDocument_impl *_wdoc ) 
  : KoDocumentChild()
{
  m_pKWordDoc = _wdoc;
}

/*================================================================*/
KWordChild::~KWordChild()
{
}


/******************************************************************/
/* Class: KWordDocument_impl                                      */
/******************************************************************/

/*================================================================*/
KWordDocument_impl::KWordDocument_impl()
  : formatCollection(this), imageCollection(this), selStart(this,1), selEnd(this,1)
{
  ADD_INTERFACE("IDL:OPParts/Print:1.0");

  // Use CORBA mechanism for deleting views
  m_lstViews.setAutoDelete(false);
  m_lstChildren.setAutoDelete(true);

  m_bModified = false;
  hasSelection = false;
}

/*================================================================*/
CORBA::Boolean KWordDocument_impl::init()
{
  pageLayout.format = PG_DIN_A4;
  pageLayout.orientation = PG_PORTRAIT;
  pageLayout.width = PG_A4_WIDTH;
  pageLayout.height = PG_A4_HEIGHT;
  pageLayout.left = DEFAULT_LEFT_BORDER;
  pageLayout.right = DEFAULT_RIGHT_BORDER;
  pageLayout.top = DEFAULT_TOP_BORDER;
  pageLayout.bottom = DEFAULT_BOTTOM_BORDER;  
  pageLayout.unit = PG_MM;

  defaultUserFont = new KWUserFont(this,"times");
  defaultParagLayout = new KWParagLayout(this);
  defaultParagLayout->setName("Standard");
  defaultParagLayout->setCounterNr(-1);
    
  pages = 1;

  pageColumns.columns = 1; //STANDARD_COLUMNS;
  pageColumns.columnSpacing = STANDARD_COLUMN_SPACING;

  QString _template;
  QString _templatePath = kapp->kde_datadir() + "/kword/templates/";

  if (KoTemplateChooseDia::chooseTemplate(_templatePath,_template))
    {
      QFileInfo fileInfo(_template);
      QString fileName(_templatePath + fileInfo.dirPath(false) + "/" + fileInfo.baseName() + ".kwt");
      loadTemplate(fileName.data());
    }
  else
    debug("no template chosen");

  return true;
}

/*================================================================*/
bool KWordDocument_impl::loadTemplate(const char *_url)
{
  KURL u(_url);
  if (u.isMalformed())
    return false;
  
  if (!u.isLocalFile())
    {
      cerr << "Can not open remote URL" << endl;
      return false;
    }

  ifstream in(u.path());
  if (!in)
    {
      cerr << "Could not open" << u.path() << endl;
      return false;
    }

  KOMLStreamFeed feed(in);
  KOMLParser parser(&feed);
  
  if (!load(parser))
    return false;
 
  m_bModified = true;
  return true;
}

/*================================================================*/
void KWordDocument_impl::setPageLayout(KoPageLayout _layout,KoColumns _cl)
{ 
  if (processingType == WP)
    {
      pageLayout = _layout; 
      pageColumns = _cl; 
    }
  else
    {
      pageLayout = _layout; 
      pageLayout.left = 0;
      pageLayout.right = 0;
      pageLayout.top = 0;
      pageLayout.bottom = 0;
    }

  if (processingType == WP)
    recalcFrames();

  updateAllCursors(); 
  updateAllFrames();
}

/*================================================================*/
void KWordDocument_impl::recalcFrames()
{
  pages = 1;
  KWTextFrameSet *frameset = dynamic_cast<KWTextFrameSet*>(frames.at(0));
 
  unsigned int frms = frameset->getNumFrames();

  ptColumnWidth = (getPTPaperWidth() - getPTLeftBorder() - getPTRightBorder() - getPTColumnSpacing() * (pageColumns.columns - 1)) 
    / pageColumns.columns;
 
  for (int i = 0;i < pageColumns.columns;i++)
    frameset->addFrame(KWFrame(getPTLeftBorder() + i * (ptColumnWidth + getPTColumnSpacing()),getPTTopBorder(),
			       ptColumnWidth,getPTPaperHeight() - getPTTopBorder() - getPTBottomBorder()));
  for (unsigned int j = 0;j < frms;j++)
    frameset->delFrame(0);
}

/*================================================================*/
KWordDocument_impl::~KWordDocument_impl()
{
  sdeb("KWordDocument_impl::~KWordDocument_impl()\n");
  cleanUp();
  edeb("...KWordDocument_impl::~KWordDocument_impl()\n");
}

/*================================================================*/
void KWordDocument_impl::cleanUp()
{
  if (m_bIsClean) return;

  assert( m_lstViews.count() == 0 );
  
  m_lstChildren.clear();

  Document_impl::cleanUp();
}

/*================================================================*/
bool KWordDocument_impl::hasToWriteMultipart()
{  
  if (m_lstChildren.count() == 0) return false;
  
  return true;
}

/*================================================================*/
bool KWordDocument_impl::loadChildren(OPParts::MimeMultipartDict_ptr _dict)
{
  cerr << "bool KWordDocument_impl::loadChildren( OPParts::MimeMultipartDict_ptr _dict )" << endl;
  
  QListIterator<KWordChild> it(m_lstChildren);
  for(;it.current();++it)
    {
      cerr << "Loading child" << endl;
      if (!it.current()->loadDocument(_dict))
	return false;
    }

  cerr << "Loading done" << endl;
  
  return true;
}

/*================================================================*/
bool KWordDocument_impl::load(KOMLParser& parser)
{
  pageLayout.format = PG_DIN_A4;
  pageLayout.orientation = PG_PORTRAIT;
  pageLayout.width = PG_A4_WIDTH;
  pageLayout.height = PG_A4_HEIGHT;
  pageLayout.left = DEFAULT_LEFT_BORDER;
  pageLayout.right = DEFAULT_RIGHT_BORDER;
  pageLayout.top = DEFAULT_TOP_BORDER;
  pageLayout.bottom = DEFAULT_BOTTOM_BORDER;  
  pageLayout.unit = PG_MM;

  defaultUserFont = new KWUserFont(this,"times");
  defaultParagLayout = new KWParagLayout(this);
  defaultParagLayout->setName("Standard");
  defaultParagLayout->setCounterNr(-1);
    
  pages = 1;

  pageColumns.columns = 1; //STANDARD_COLUMNS;
  pageColumns.columnSpacing = STANDARD_COLUMN_SPACING;

  string tag;
  vector<KOMLAttrib> lst;
  string name;

  KoPageLayout __pgLayout;
  __pgLayout.unit = PG_MM;
  KoColumns __columns;

  
  // DOC
  if (!parser.open("DOC",tag))
    {
      cerr << "Missing DOC" << endl;
      return false;
    }
  
  KOMLParser::parseTag( tag.c_str(), name, lst );
  vector<KOMLAttrib>::const_iterator it = lst.begin();
  for(;it != lst.end();it++)
    {
      if ((*it).m_strName == "mime")
	{
	  if ((*it).m_strValue != "application/x-kword")
	    {
	      cerr << "Unknown mime type " << (*it).m_strValue << endl;
	      return false;
	    }
	}
    }

  // PAPER
  while (parser.open(0L,tag))
    {
      KOMLParser::parseTag(tag.c_str(),name,lst);
      
      if (name == "OBJECT")
	{
	  KWordChild *ch = new KWordChild(this);
	  ch->load(parser,lst);
	  insertChild(ch);
	}
      else if (name == "PAPER")
	{
	  KOMLParser::parseTag(tag.c_str(),name,lst);
	  vector<KOMLAttrib>::const_iterator it = lst.begin();
	  for(;it != lst.end();it++)
	    {
	      if ((*it).m_strName == "format")
		__pgLayout.format = (KoFormat)atoi((*it).m_strValue.c_str());
	      else if ((*it).m_strName == "orientation")
		__pgLayout.orientation = (KoOrientation)atoi((*it).m_strValue.c_str());
	      else if ((*it).m_strName == "width")
		__pgLayout.width = static_cast<double>(atof((*it).m_strValue.c_str()));
	      else if ((*it).m_strName == "height")
		__pgLayout.height = static_cast<double>(atof((*it).m_strValue.c_str()));
	      else if ((*it).m_strName == "columns")
		__columns.columns = atoi((*it).m_strValue.c_str());
	      else if ((*it).m_strName == "columnspacing")
		__columns.columnSpacing = atoi((*it).m_strValue.c_str());
	      else
		cerr << "Unknown attrib PAPER:'" << (*it).m_strName << "'" << endl;
	    }

	  // PAPERBORDERS, HEAD, FOOT
	  while (parser.open(0L,tag))
	    {
	      KOMLParser::parseTag(tag.c_str(),name,lst);
	      if (name == "PAPERBORDERS")
		{    
		  KOMLParser::parseTag(tag.c_str(),name,lst);
		  vector<KOMLAttrib>::const_iterator it = lst.begin();
		  for(;it != lst.end();it++)
		    {
		      if ((*it).m_strName == "left")
			__pgLayout.left = (double)atof((*it).m_strValue.c_str());
		      else if ((*it).m_strName == "top")
			__pgLayout.top = (double)atof((*it).m_strValue.c_str());
		      else if ((*it).m_strName == "right")
			__pgLayout.right = (double)atof((*it).m_strValue.c_str());
		      else if ((*it).m_strName == "bottom")
			__pgLayout.bottom = (double)atof((*it).m_strValue.c_str());
		      else
			cerr << "Unknown attrib 'PAPERBORDERS:" << (*it).m_strName << "'" << endl;
		    } 
		}
	      else
		cerr << "Unknown tag '" << tag << "' in PAPER" << endl;    
	      
	      if (!parser.close(tag))
		{
		  cerr << "ERR: Closing Child" << endl;
		  return false;
		}
	    }

	}
      
      else if (name == "ATTRIBUTES")
	{
	  KOMLParser::parseTag(tag.c_str(),name,lst);
	  vector<KOMLAttrib>::const_iterator it = lst.begin();
	  for(;it != lst.end();it++)
	    {
	      if ((*it).m_strName == "processing")
		processingType = static_cast<ProcessingType>(atoi((*it).m_strValue.c_str()));
	      else if ((*it).m_strName == "standardpage")
		  ;
	    }
	}

      else if (name == "FRAMESETS")
	{
	  KOMLParser::parseTag(tag.c_str(),name,lst);
	  vector<KOMLAttrib>::const_iterator it = lst.begin();
	  for(;it != lst.end();it++)
	    {
	    }
	  loadFrameSets(parser,lst);
	}

      else
	cerr << "Unknown tag '" << tag << "' in the DOCUMENT" << endl;    
	
      if (!parser.close(tag))
	{
	  cerr << "ERR: Closing Child" << endl;
	  return false;
	}
    }

  setPageLayout(__pgLayout,__columns);

  return true;
}

/*================================================================*/
void KWordDocument_impl::loadFrameSets(KOMLParser& parser,vector<KOMLAttrib>& lst)
{
  string tag;
  string name;

  while (parser.open(0L,tag))
    {
      KOMLParser::parseTag(tag.c_str(),name,lst);
      
      // paragraph
      if (name == "FRAMESET")
	{    
	  FrameType frameType = FT_BASE;

	  KOMLParser::parseTag(tag.c_str(),name,lst);
	  vector<KOMLAttrib>::const_iterator it = lst.begin();
	  for(;it != lst.end();it++)
	    {
	      if ((*it).m_strName == "frameType")
		frameType = static_cast<FrameType>(atoi((*it).m_strValue.c_str()));
	    }

	  switch (frameType)
	    {
	    case FT_TEXT:
	      {
		KWTextFrameSet *frame = new KWTextFrameSet(this);
		frame->load(parser,lst);
		frames.append(frame);
	      } break;
	    default: break;
	    }
	}
      else
	cerr << "Unknown tag '" << tag << "' in FRAMESETS" << endl;    
      
      if (!parser.close(tag))
	{
	  cerr << "ERR: Closing Child" << endl;
	  return;
	}
    }
}

/*================================================================*/
bool KWordDocument_impl::save(ostream &out)
{
  out << otag << "<DOC author=\"" << "Reginald Stadlbauer and Torben Weis" << "\" email=\"" << "reggie@kde.org and weis@kde.org" 
      << "\" editor=\"" << "KWord" << "\" mime=\"" << "application/x-kword" << "\">" << endl;
  
  out << otag << "<PAPER format=\"" << static_cast<int>(pageLayout.format) << "\" width=\"" << pageLayout.width
      << "\" height=\"" << pageLayout.height << "\" orientation=\"" << static_cast<int>(pageLayout.orientation) 
      << "\" columns=\"" << pageColumns.columns << "\" columnspacing=\"" << pageColumns.columnSpacing << "\">" << endl;
  out << indent << "<PAPERBORDERS left=\"" << pageLayout.left << "\" top=\"" << pageLayout.top << "\" right=\"" << pageLayout.right
      << "\" bottom=\"" << pageLayout.bottom << "\"/>" << endl;
  out << etag << "</PAPER>" << endl;
  out << indent << "<ATTRIBUTES processing=\"" << static_cast<int>(processingType) << "\" standardpage=\"" << 1 << "\"/>" << endl;

  out << otag << "<FRAMESETS>" << endl;

  KWFrameSet *frameSet = 0L;
  for (unsigned int i = 0;i < getNumFrameSets();i++)
    {
      frameSet = getFrameSet(i);
      frameSet->save(out);
    }

  out << etag << "</FRAMESETS>" << endl;

  // Write "OBJECT" tag for every child
  QListIterator<KWordChild> chl(m_lstChildren);
  for(;chl.current();++chl)
    chl.current()->save( out );

  out << etag << "</DOC>" << endl;
    
  return true;
}

/*================================================================*/
void KWordDocument_impl::makeChildListIntern(OPParts::Document_ptr _doc,const char *_path)
{
  cerr << "void KWordDocument_impl::makeChildList( OPParts::Document_ptr _doc, const char *_path )" << endl;
  
  int i = 0;
  
  QListIterator<KWordChild> it(m_lstChildren);
  for(;it.current();++it)
    {
      QString tmp;
      tmp.sprintf("/%i",i++);
      QString path(_path);
      path += tmp.data();
      cerr << "SETTING NAME To " << path.data() << endl;
    
      OPParts::Document_var doc = it.current()->document();    
      doc->makeChildList(_doc,path);
    }
}

/*================================================================*/
QStrList KWordDocument_impl::outputFormats()
{
  return QStrList();
}

/*================================================================*/
QStrList KWordDocument_impl::inputFormats()
{
  return QStrList();
}

/*================================================================*/
void KWordDocument_impl::viewList(OPParts::Document::ViewList*& _list)
{
  (*_list).length(m_lstViews.count());

  int i = 0;
  QListIterator<KWordView_impl> it(m_lstViews);
  for(;it.current();++it)
    {
      (*_list)[i++] = OPParts::View::_duplicate(it.current());
    }
}

/*================================================================*/
void KWordDocument_impl::addView(KWordView_impl *_view)
{
  m_lstViews.append(_view);
}

/*================================================================*/
void KWordDocument_impl::removeView(KWordView_impl *_view)
{
  m_lstViews.setAutoDelete(false);
  m_lstViews.removeRef(_view);
  m_lstViews.setAutoDelete(true);
}

/*================================================================*/
OPParts::View_ptr KWordDocument_impl::createView()
{
  KWordView_impl *p = new KWordView_impl(0L);
  p->setDocument(this);

  return OPParts::View::_duplicate(p);
}

/*================================================================*/
void KWordDocument_impl::insertObject(const QRect& _rect,const char *_server_name)
{
  OPParts::Document_var doc = imr_createDocByServerName(_server_name);
  if (CORBA::is_nil(doc)) return;

  if (!doc->init())
    {
      QMessageBox::critical((QWidget*)0L,i18n("KWord Error"),i18n("Could not init"),i18n("OK"));
      return;
    }

  KWordChild* ch = new KWordChild(this,_rect,doc);

  insertChild(ch);
}

/*================================================================*/
void KWordDocument_impl::insertChild(KWordChild *_child)
{
  m_lstChildren.append(_child);
  
  emit sig_insertObject(_child);
}

/*================================================================*/
void KWordDocument_impl::changeChildGeometry(KWordChild *_child,const QRect& _rect)
{
  _child->setGeometry(_rect);

  emit sig_updateChildGeometry(_child);
}

/*================================================================*/
QListIterator<KWordChild> KWordDocument_impl::childIterator()
{
  return QListIterator<KWordChild> (m_lstChildren);
}

/*================================================================*/
void KWordDocument_impl::draw(QPaintDevice* _dev,CORBA::Long _width,CORBA::Long _height)
{
}

/*================================================================*/
QPen KWordDocument_impl::setBorderPen(KWParagLayout::Border _brd)
{
  QPen pen(black,1,SolidLine);

  pen.setWidth(_brd.ptWidth);
  pen.setColor(_brd.color);
  
  switch (_brd.style)
    {
    case KWParagLayout::SOLID:
      pen.setStyle(SolidLine);
      break;
    case KWParagLayout::DASH:
      pen.setStyle(DashLine);
      break;
    case KWParagLayout::DOT:
      pen.setStyle(DotLine);
      break;
    case KWParagLayout::DASH_DOT:
      pen.setStyle(DashDotLine);
      break;
    case KWParagLayout::DASH_DOT_DOT:
      pen.setStyle(DashDotDotLine);
      break;
    }

  return QPen(pen);
}

/*================================================================*/
KWUserFont* KWordDocument_impl::findUserFont(char* _userfontname)
{
  KWUserFont* font;
  for (font = userFontList.first();font != 0L;font = userFontList.next())
    if (strcmp(font->getFontName(),_userfontname) == 0)
      return font;
  
  return 0L;
}

/*================================================================*/
KWDisplayFont* KWordDocument_impl::findDisplayFont(KWUserFont* _font,unsigned int _size,int _weight,bool _italic,bool _underline)
{
  KWDisplayFont* font;
  for (font = displayFontList.first();font != 0L;font = displayFontList.next())
    {
      if (font->getUserFont() == _font && font->getPTSize() == _size &&
	  font->weight() == _weight && font->italic() == _italic && font->underline() == _underline)
	return font;
    }
  
  return 0L;
}

/*================================================================*/
KWParagLayout* KWordDocument_impl::findParagLayout(const char *_name)
{
  KWParagLayout* p;
  for (p = paragLayoutList.first();p != 0L;p = paragLayoutList.next())
    if (strcmp( p->getName(),_name ) == 0)
      return p;
  
  return 0L;
}


/*================================================================*/
// void KWordDocument_impl::calcColumnWidth()
// {
//   ptColumnWidth = (getPTPaperWidth() - getPTLeftBorder() - getPTRightBorder() - getPTColumnSpacing() * (pageColumns.columns - 1)) 
//     / pageColumns.columns;
// }

/*================================================================*/
bool KWordDocument_impl::isPTYInFrame(unsigned int _frameSet,unsigned int _frame,unsigned int _ypos)
{
  return frames.at(_frameSet)->isPTYInFrame(_frame,_ypos);
}

/*================================================================*/
KWParag* KWordDocument_impl::findFirstParagOfPage(unsigned int _page,unsigned int _frameset)
{
  if (frames.at(_frameset)->getFrameType() != FT_TEXT) return 0L;

  KWParag *p = dynamic_cast<KWTextFrameSet*>(frames.at(_frameset))->getFirstParag();
  while (p)
    {
      if (p->getEndPage() == _page || p->getStartPage() == _page)
 	return p;
      p = p->getNext();
    }
  
  return 0L;
}

/*================================================================*/
void KWordDocument_impl::printLine( KWFormatContext &_fc, QPainter &_painter, int xOffset, int yOffset, int _w, int _h )
{
  _painter.save();

  unsigned int xShift = getFrameSet(_fc.getFrameSet() - 1)->getFrame(_fc.getFrame() - 1)->left();

  QRegion cr = QRegion(xShift - xOffset - _fc.getParag()->getParagLayout()->getLeftBorder().ptWidth / 2 -
		       _fc.getParag()->getParagLayout()->getLeftBorder().ptWidth,
		       _fc.getPTY() - yOffset - _fc.getParag()->getParagLayout()->getTopBorder().ptWidth - 
		       _fc.getParag()->getParagLayout()->getTopBorder().ptWidth / 2,
		       getFrameSet(_fc.getFrameSet() - 1)->getFrame(_fc.getFrame() - 1)->width() + 
		       _fc.getParag()->getParagLayout()->getLeftBorder().ptWidth + 
		       _fc.getParag()->getParagLayout()->getRightBorder().ptWidth + 
		       _fc.getParag()->getParagLayout()->getLeftBorder().ptWidth,
		       _fc.getLineHeight() + _fc.getParag()->getParagLayout()->getTopBorder().ptWidth +
		       _fc.getParag()->getParagLayout()->getBottomBorder().ptWidth + 
		       _fc.getParag()->getParagLayout()->getTopBorder().ptWidth);

  QRegion visible(0,0,_w,_h);

  if (_painter.hasClipping())
    cr = _painter.clipRegion().intersect(cr);

  if (cr.intersect(visible).isEmpty())
    {
      _painter.restore();
      return;
    }

  _painter.setClipRegion(cr);

  if (_fc.isCursorInFirstLine() && _fc.getParag()->getParagLayout()->getTopBorder().ptWidth > 0)
    {
      unsigned int _x1 = getFrameSet(_fc.getFrameSet() - 1)->getFrame(_fc.getFrame() - 1)->left() - xOffset -
	_fc.getParag()->getParagLayout()->getLeftBorder().ptWidth - _fc.getParag()->getParagLayout()->getLeftBorder().ptWidth / 2;
      unsigned int _y = _fc.getPTY() - yOffset - _fc.getParag()->getParagLayout()->getTopBorder().ptWidth;
      unsigned int _x2 = _x1 + getFrameSet(_fc.getFrameSet() - 1)->getFrame(_fc.getFrame() - 1)->width() +
	_fc.getParag()->getParagLayout()->getLeftBorder().ptWidth + _fc.getParag()->getParagLayout()->getRightBorder().ptWidth + 
	_fc.getParag()->getParagLayout()->getLeftBorder().ptWidth / 2 + _fc.getParag()->getParagLayout()->getRightBorder().ptWidth / 2 -
	((_fc.getParag()->getParagLayout()->getRightBorder().ptWidth / 2) * 2 == 
	 _fc.getParag()->getParagLayout()->getRightBorder().ptWidth ? 1 : 0) ;
      
      _painter.setPen(setBorderPen(_fc.getParag()->getParagLayout()->getTopBorder()));
      _painter.drawLine(_x1,_y,_x2,_y);
    }
  if (_fc.isCursorInLastLine() && _fc.getParag()->getParagLayout()->getBottomBorder().ptWidth > 0)
    {
      unsigned int _x1 = getFrameSet(_fc.getFrameSet() - 1)->getFrame(_fc.getFrame() - 1)->left() - xOffset -
	_fc.getParag()->getParagLayout()->getLeftBorder().ptWidth - _fc.getParag()->getParagLayout()->getLeftBorder().ptWidth / 2;
      unsigned int _y = _fc.getPTY() + _fc.getLineHeight() - yOffset + _fc.getParag()->getParagLayout()->getBottomBorder().ptWidth - 1;
      unsigned int _x2 = _x1 + getFrameSet(_fc.getFrameSet() - 1)->getFrame(_fc.getFrame() - 1)->width() +
	_fc.getParag()->getParagLayout()->getLeftBorder().ptWidth + _fc.getParag()->getParagLayout()->getRightBorder().ptWidth + 
	_fc.getParag()->getParagLayout()->getLeftBorder().ptWidth / 2 + _fc.getParag()->getParagLayout()->getRightBorder().ptWidth / 2 -
	((_fc.getParag()->getParagLayout()->getRightBorder().ptWidth / 2) * 2 == 
	 _fc.getParag()->getParagLayout()->getRightBorder().ptWidth ? 1 : 0) ;
      
      _painter.setPen(setBorderPen(_fc.getParag()->getParagLayout()->getBottomBorder()));
      _painter.drawLine(_x1,_y,_x2,_y);
    }
  if (_fc.getParag()->getParagLayout()->getLeftBorder().ptWidth > 0)
    {
      unsigned int _x = getFrameSet(_fc.getFrameSet() - 1)->getFrame(_fc.getFrame() - 1)->left() - xOffset - 
	_fc.getParag()->getParagLayout()->getLeftBorder().ptWidth;
      unsigned int _y1 = _fc.getPTY() - yOffset - _fc.getParag()->getParagLayout()->getTopBorder().ptWidth;
      unsigned int _y2 = _fc.getPTY() + _fc.getLineHeight() - yOffset + _fc.getParag()->getParagLayout()->getBottomBorder().ptWidth;
      
      _painter.setPen(setBorderPen(_fc.getParag()->getParagLayout()->getLeftBorder()));
      _painter.drawLine(_x,_y1,_x,_y2);
    }
  if (_fc.getParag()->getParagLayout()->getRightBorder().ptWidth > 0)
    {
      unsigned int _x = getFrameSet(_fc.getFrameSet() - 1)->getFrame(_fc.getFrame() - 1)->left() - xOffset + 
	_fc.getParag()->getParagLayout()->getRightBorder().ptWidth + getFrameSet(_fc.getFrameSet() - 1)->getFrame(_fc.getFrame() - 1)->width() - 1;
      unsigned int _y1 = _fc.getPTY() - yOffset - _fc.getParag()->getParagLayout()->getTopBorder().ptWidth;
      unsigned int _y2 = _fc.getPTY() + _fc.getLineHeight() - yOffset + _fc.getParag()->getParagLayout()->getBottomBorder().ptWidth;
      
      _painter.setPen(setBorderPen(_fc.getParag()->getParagLayout()->getRightBorder()));
      _painter.drawLine(_x,_y1,_x,_y2);
    }
  
  // Shortcut to the text memory segment
  unsigned int textLen = _fc.getParag()->getTextLen() - 1;
  KWChar* text = _fc.getParag()->getText();
  // Shortcut to the current paragraph layout
  KWParagLayout *lay = _fc.getParag()->getParagLayout();
  // Index in the text memory segment that points to the line start
  unsigned int pos = _fc.getLineStartPos();
  int plus = 0;

  // First line ? Draw the counter ?
  if ( pos == 0 && lay->getCounterNr() != -1 )
    {
      KWFormat counterfm(this, _fc );
      counterfm.apply( lay->getCounterFormat() );
      _painter.setFont( *( counterfm.loadFont( this ) ) );
      _painter.setPen( counterfm.getColor() );
      
      _painter.drawText( _fc.getPTCounterPos() - xOffset, 
			 _fc.getPTY() + _fc.getPTMaxAscender() - yOffset, _fc.getCounterText() );
    }
    
  // paint it character for character. Provisionally! !!HACK!!
  _fc.cursorGotoLineStart( _painter );

  // Init font and style
  _painter.setFont( *_fc.loadFont( this ) );
  _painter.setPen( _fc.getColor() );

  //cerr << "Starting with color " << _fc.getColor().red() << " "<< _fc.getColor().green() << " "<< _fc.getColor().blue() << endl;
  
  char buffer[200];
  int i = 0;
  unsigned int tmpPTPos = 0;
  while ( !_fc.isCursorAtLineEnd() )
    {
      // Init position
      if ( i == 0 )
	{
	  // Change the painter
	  tmpPTPos = _fc.getPTPos();
	  _painter.setFont( *_fc.loadFont( this ) );
	  _painter.setPen( _fc.getColor() );
	  
	  //cerr << "Switch1 " << _fc.getColor().red() << " "<< _fc.getColor().green() << " "<< _fc.getColor().blue() << endl;
	}
      
      //debug("%d",i);
      if (i > 200 || _fc.getTextPos() > textLen) 
	{
	  warning("Reggie: WOW - something has gone really wrong here!!!!!");
	  return;
	}

      buffer[i] = text[ _fc.getTextPos() ].c;
      
      if ( buffer[i] == 0 )
	{
	  buffer[i] = '\0';
	  _painter.drawText( tmpPTPos - xOffset, /*_fc.getPTY() + _fc.getPTMaxAscender() - yOffset*/
			     _fc.getPTY() + _fc.getLineHeight() - _fc.getPTMaxDescender() - 
			     _fc.getParag()->getParagLayout()->getPTLineSpacing() - yOffset + plus, buffer );
	  i = 0;

	  _painter.drawImage(QPoint(tmpPTPos - xOffset, _fc.getPTY() - yOffset + 
				    ((_fc.getLineHeight() - _fc.getParag()->getParagLayout()->getPTLineSpacing()) 
				     - ((KWCharImage*)text[ _fc.getTextPos() ].attrib)->getImage()->height())),
			     *((KWCharImage*)text[ _fc.getTextPos() ].attrib)->getImage());
	  _fc.cursorGotoNextChar( _painter );
	  // Torben: TODO: Handle special objects like images here
	}
      else
	{
	  if ( text[ _fc.getTextPos() ].attrib != 0L )
	    {
	      // Change text format here
	      assert( text[ _fc.getTextPos() ].attrib->getClassId() == ID_KWCharFormat );
	      KWCharFormat *f = (KWCharFormat*)text[ _fc.getTextPos() ].attrib;
	      _fc.apply( *f->getFormat() );
	      // Change the painter
	      if (_fc.getVertAlign() == KWFormat::VA_NORMAL)
		{
		  _painter.setFont( *_fc.loadFont( this ) );
		  plus = 0;
		}
	      else if (_fc.getVertAlign() == KWFormat::VA_SUB)
		{
		  QFont _font = *_fc.loadFont( this );
		  _font.setPointSize((2 * _font.pointSize()) / 3);
		  _painter.setFont(_font);
		  plus = _font.pointSize() / 2;
		}
	      else if (_fc.getVertAlign() == KWFormat::VA_SUPER)
		{
		  QFont _font = *_fc.loadFont( this );
		  _font.setPointSize((2 * _font.pointSize()) / 3);
		  _painter.setFont(_font);
		  plus = - _fc.getPTAscender() + _font.pointSize() / 2;
		}
	      _painter.setPen( _fc.getColor() );
	      //cerr << "Switch 2 " << _fc.getColor().red() << " "<< _fc.getColor().green() << " "<< _fc.getColor().blue() << endl;
	    }
	  
	  // Test next character.
	  i++;
	  if ( _fc.cursorGotoNextChar( _painter ) != 1 || text[_fc.getTextPos()].c == ' ' || i >= 199 )
	    {
	      // there was a blank _or_ there will be a font switch _or_ a special object next, so print 
	      // what we have so far
	      buffer[i] = '\0';
	      _painter.drawText( tmpPTPos - xOffset, /*_fc.getPTY() + _fc.getPTMaxAscender() - yOffset*/
				 _fc.getPTY() + _fc.getLineHeight() - _fc.getPTMaxDescender() - yOffset - 
				 _fc.getParag()->getParagLayout()->getPTLineSpacing() + plus,buffer );
	      //cerr << "#'" << buffer << "'" << endl;
	      i = 0;
	      // Blanks are not printed at all
	      if ( text[_fc.getTextPos()].c == ' ' )
 		_fc.cursorGotoNextChar(_painter);
	    }
	}
    }

  _painter.restore();
}

/*================================================================*/
void KWordDocument_impl::drawMarker(KWFormatContext &_fc,QPainter *_painter,int xOffset,int yOffset)
{
  RasterOp rop = _painter->rasterOp();
    
  _painter->setRasterOp(NotROP);
  QPen pen;
  pen.setWidth(2);
  _painter->setPen(pen);
  
//   _painter->drawLine(_fc.getPTPos() - xOffset + 1,
// 		     _fc.getPTY() + _fc.getPTMaxAscender() - _fc.getPTAscender() - yOffset,
// 		     _fc.getPTPos() - xOffset + 1,
// 		     _fc.getPTY() + _fc.getPTMaxAscender() + _fc.getPTDescender() - yOffset);
  unsigned int diffx1 = 1;
  unsigned int diffx2 = 1;
  if (_fc.getItalic())
    {
      diffx1 = static_cast<int>(static_cast<float>(_fc.getLineHeight()) / 3.732);
      diffx2 = 0;
    }

  _painter->drawLine(_fc.getPTPos() - xOffset + diffx1,
		     _fc.getPTY() - yOffset,
		     _fc.getPTPos() - xOffset + diffx2,
		     _fc.getPTY() + _fc.getLineHeight() - _fc.getParag()->getParagLayout()->getPTLineSpacing() - yOffset);

  _painter->setRasterOp(rop);
}

/*================================================================*/
void KWordDocument_impl::updateAllViews(KWordView_impl *_view)
{
  KWordView_impl *viewPtr;

  if (!m_lstViews.isEmpty())
    {
      for (viewPtr = m_lstViews.first();viewPtr != 0;viewPtr = m_lstViews.next())
	if (viewPtr != _view) viewPtr->getGUI()->getPaperWidget()->repaint(false);
    }
}

/*================================================================*/
void KWordDocument_impl::updateAllRanges()
{
  KWordView_impl *viewPtr;

  if (!m_lstViews.isEmpty())
    {
      for (viewPtr = m_lstViews.first();viewPtr != 0;viewPtr = m_lstViews.next())
	{
	  if (viewPtr->getGUI())
	    viewPtr->getGUI()->setRanges();
	}
    }
}

/*================================================================*/
void KWordDocument_impl::updateAllCursors()
{
  KWordView_impl *viewPtr;

  if (!m_lstViews.isEmpty())
    {
      for (viewPtr = m_lstViews.first();viewPtr != 0;viewPtr = m_lstViews.next())
	{
	  if (viewPtr->getGUI())
	    {
	      viewPtr->getGUI()->getPaperWidget()->recalcText();
	      viewPtr->getGUI()->getPaperWidget()->recalcCursor();
	    }
	}
    }
}

/*================================================================*/
void KWordDocument_impl::drawAllBorders(QPainter *_painter = 0)
{
  KWordView_impl *viewPtr;
  QPainter p;

  if (!m_lstViews.isEmpty())
    {
      for (viewPtr = m_lstViews.first();viewPtr != 0;viewPtr = m_lstViews.next())
	{
	  if (viewPtr->getGUI())
	    {
	      if (!_painter)
		{
		  p.begin(viewPtr->getGUI()->getPaperWidget());
		  viewPtr->getGUI()->getPaperWidget()->drawBorders(p,viewPtr->getGUI()->getPaperWidget()->rect());
		  p.end();
		}
	      else
		viewPtr->getGUI()->getPaperWidget()->drawBorders(*_painter,viewPtr->getGUI()->getPaperWidget()->rect());
	    }
	}
    }
}

/*================================================================*/
void KWordDocument_impl::insertPicture(QString _filename,KWPage *_paperWidget)
{
  _paperWidget->insertPictureAsChar(_filename);
}

/*================================================================*/
void KWordDocument_impl::drawSelection(QPainter &_painter,int xOffset,int yOffset)
{
  _painter.save();
  RasterOp rop = _painter.rasterOp();
    
  _painter.setRasterOp(NotROP);
  _painter.setBrush(black);
  _painter.setPen(NoPen);
  
  KWFormatContext tmpFC2(this,selStart.getFrameSet() - 1);
  KWFormatContext tmpFC1(this,selStart.getFrameSet() - 1);

  if (selStart.getParag() == selEnd.getParag())
    {
      if (selStart.getTextPos() < selEnd.getTextPos())
	{  
	  tmpFC1 = selStart;
	  tmpFC2 = selEnd;
	}
      else
	{
	  tmpFC1 = selEnd;
	  tmpFC2 = selStart;
	}
    }
  else
    {
      KWParag *parag = getFirstParag(selStart.getFrameSet() - 1);
      while (parag)
	{
	  if (parag == selStart.getParag())
	    {
	      tmpFC1 = selStart;
	      tmpFC2 = selEnd;
	      break;
	    }
	  if (parag == selEnd.getParag())
	    {
	      tmpFC2 = selStart;
	      tmpFC1 = selEnd;
	      break;
	    }
	  parag = parag->getNext();
	}
    }

  int _x = 0,_y = 0,_w = 0,_h = 0;

  _x = tmpFC1.getPTPos();
  _y = tmpFC1.getPTY();
  _h = tmpFC1.getLineHeight();

  while (!(tmpFC1.getParag() == tmpFC2.getParag() && tmpFC1.getTextPos() == tmpFC2.getTextPos()))
    {
      _w = tmpFC1.getPTPos() - _x;
      tmpFC1.cursorGotoRight(_painter);
      if (tmpFC1.isCursorAtLineStart())
	{
	  _painter.drawRect(_x - xOffset,_y - yOffset,_w,_h);
	  _x = tmpFC1.getPTPos();
	  _y = tmpFC1.getPTY();
	  _h = tmpFC1.getLineHeight();
	}
    }
  _w = tmpFC1.getPTPos() - _x;
  _painter.drawRect(_x - xOffset,_y - yOffset,_w,_h);

  _painter.setRasterOp(rop);
  _painter.restore();
}

/*================================================================*/
void KWordDocument_impl::deleteSelectedText(KWFormatContext *_fc,QPainter &_painter)
{
  KWFormatContext tmpFC2(this,selStart.getFrameSet() - 1);
  KWFormatContext tmpFC1(this,selStart.getFrameSet() - 1);

  if (selStart.getParag() == selEnd.getParag())
    {
      if (selStart.getTextPos() < selEnd.getTextPos())
	{  
	  tmpFC1 = selStart;
	  tmpFC2 = selEnd;
	}
      else
	{
	  tmpFC1 = selEnd;
	  tmpFC2 = selStart;
	}
      
      tmpFC1.getParag()->deleteText(tmpFC1.getTextPos(),tmpFC2.getTextPos() - tmpFC1.getTextPos());
      
      _fc->setTextPos(tmpFC1.getTextPos());
    
      KWParag *parag = 0;

      if (_fc->getParag()->getTextLen() == 0)
	{
	  if (_fc->getParag()->getNext())
	    {
	      parag = _fc->getParag()->getNext();
	      dynamic_cast<KWTextFrameSet*>(frames.at(_fc->getFrameSet() - 1))->deleteParag(_fc->getParag());
	      _fc->init(parag,_painter);
	    }
	  else if (_fc->getParag()->getPrev())
	    {
	      parag = _fc->getParag()->getPrev();
	      dynamic_cast<KWTextFrameSet*>(frames.at(_fc->getFrameSet() - 1))->deleteParag(_fc->getParag());
	      _fc->init(parag,_painter);
	    }
	}
      _fc->setTextPos(tmpFC1.getTextPos());
    }
  else
    {
      KWParag *parag = getFirstParag(selStart.getFrameSet() - 1),*tmpParag = 0;
      while (parag)
	{
	  if (parag == selStart.getParag())
	    {
	      tmpFC1 = selStart;
	      tmpFC2 = selEnd;
	      break;
	    }
	  if (parag == selEnd.getParag())
	    {
	      tmpFC2 = selStart;
	      tmpFC1 = selEnd;
	      break;
	    }
	  parag = parag->getNext();
	}
      tmpFC1.getParag()->deleteText(tmpFC1.getTextPos(),tmpFC1.getParag()->getTextLen() - tmpFC1.getTextPos());
      parag = tmpFC1.getParag()->getNext();
      while (parag && parag != tmpFC2.getParag())
	{
	  tmpParag = parag->getNext();
	  dynamic_cast<KWTextFrameSet*>(frames.at(_fc->getFrameSet() - 1))->deleteParag(parag);
	  parag = tmpParag;
	}
      tmpFC2.getParag()->deleteText(0,tmpFC2.getTextPos());

      dynamic_cast<KWTextFrameSet*>(frames.at(_fc->getFrameSet() - 1))->joinParag(tmpFC1.getParag(),tmpFC2.getParag());
      _fc->init(tmpFC1.getParag(),_painter);
      _fc->setTextPos(tmpFC1.getTextPos());

      if (_fc->getParag()->getTextLen() == 0)
	{
	  if (_fc->getParag()->getNext())
	    {
	      parag = _fc->getParag()->getNext();
	      dynamic_cast<KWTextFrameSet*>(frames.at(_fc->getFrameSet() - 1))->deleteParag(_fc->getParag());
	      _fc->init(parag,_painter);
	    }
	  else if (_fc->getParag()->getPrev())
	    {
	      parag = _fc->getParag()->getPrev();
	      dynamic_cast<KWTextFrameSet*>(frames.at(_fc->getFrameSet() - 1))->deleteParag(_fc->getParag());
	      _fc->init(parag,_painter);
	    }
	}
      _fc->setTextPos(tmpFC1.getTextPos());
    }
}

/*================================================================*/
void KWordDocument_impl::copySelectedText()
{
  KWFormatContext tmpFC2(this,selStart.getFrameSet() - 1);
  KWFormatContext tmpFC1(this,selStart.getFrameSet() - 1);

  QString clipString = "";

  if (selStart.getParag() == selEnd.getParag())
    {
      if (selStart.getTextPos() < selEnd.getTextPos())
	{  
	  tmpFC1 = selStart;
	  tmpFC2 = selEnd;
	}
      else
	{
	  tmpFC1 = selEnd;
	  tmpFC2 = selStart;
	}
      
      clipString = tmpFC1.getParag()->getKWString()->toString(tmpFC1.getTextPos(),tmpFC2.getTextPos() - tmpFC1.getTextPos());
    }
  else
    {
      KWParag *parag = getFirstParag(selStart.getFrameSet() - 1);
      while (parag)
	{
	  if (parag == selStart.getParag())
	    {
	      tmpFC1 = selStart;
	      tmpFC2 = selEnd;
	      break;
	    }
	  if (parag == selEnd.getParag())
	    {
	      tmpFC2 = selStart;
	      tmpFC1 = selEnd;
	      break;
	    }
	  parag = parag->getNext();
	}
 
      clipString = tmpFC1.getParag()->getKWString()->toString(tmpFC1.getTextPos(),tmpFC1.getParag()->getTextLen() - tmpFC1.getTextPos());
      parag = tmpFC1.getParag()->getNext();
      while (parag && parag != tmpFC2.getParag())
	{
	  clipString += "\n";
	  if (parag->getTextLen() > 0)
	    clipString += parag->getKWString()->toString(0,parag->getTextLen());
	  else
	    clipString += " ";
	  parag = parag->getNext();
	}
      clipString += "\n";
      if (tmpFC2.getParag()->getTextLen() > 0)
	clipString += tmpFC2.getParag()->getKWString()->toString(0,tmpFC2.getTextPos());
    }

  QClipboard *cb = QApplication::clipboard();
  cb->setText(clipString.data());
}

/*================================================================*/
void KWordDocument_impl::setFormat(KWFormat &_format)
{
  KWFormatContext tmpFC2(this,selStart.getFrameSet() - 1);
  KWFormatContext tmpFC1(this,selStart.getFrameSet() - 1);

  if (selStart.getParag() == selEnd.getParag())
    {
      if (selStart.getTextPos() < selEnd.getTextPos())
	{  
	  tmpFC1 = selStart;
	  tmpFC2 = selEnd;
	}
      else
	{
	  tmpFC1 = selEnd;
	  tmpFC2 = selStart;
	}
      
      tmpFC1.getParag()->setFormat(tmpFC1.getTextPos(),tmpFC2.getTextPos() - tmpFC1.getTextPos(),_format);
    }
  else
    {
      KWParag *parag = getFirstParag(selStart.getFrameSet() - 1);
      while (parag)
	{
	  if (parag == selStart.getParag())
	    {
	      tmpFC1 = selStart;
	      tmpFC2 = selEnd;
	      break;
	    }
	  if (parag == selEnd.getParag())
	    {
	      tmpFC2 = selStart;
	      tmpFC1 = selEnd;
	      break;
	    }
	  parag = parag->getNext();
	}

      tmpFC1.getParag()->setFormat(tmpFC1.getTextPos(),tmpFC1.getParag()->getTextLen() - tmpFC1.getTextPos(),_format);
      parag = tmpFC1.getParag()->getNext();
      while (parag && parag != tmpFC2.getParag())
	{
	  parag->setFormat(0,parag->getTextLen(),_format);
	  parag = parag->getNext();
	}
      tmpFC2.getParag()->setFormat(0,tmpFC2.getTextPos(),_format);
    }
}

/*================================================================*/
void KWordDocument_impl::paste(KWFormatContext *_fc,QString _string,KWPage *_page)
{
  QStrList strList;
  int index;
  QPainter painter;

  if (_string.isEmpty()) return;

  while (true)
    {
      index = _string.find('\n',0);
      if (index == -1) break;
      
      if (index > 0 && !_string.left(index).simplifyWhiteSpace().isEmpty())
	strList.append(QString(_string.left(index)));
      _string.remove(0,index + 1);
    }
  
  if (!_string.isEmpty() && !_string.simplifyWhiteSpace().isEmpty())
    strList.append(QString(_string));

  if (!strList.isEmpty())
    {
      if (strList.count() == 1)
	{
	  QString str;
	  unsigned int len;
	  KWFormat *format = new KWFormat(this);
	  format->setDefaults(this);
	  str = QString(strList.at(0));
	  len = str.length();
	  _fc->getParag()->insertText(_fc->getTextPos(),str);
	  _fc->getParag()->setFormat(_fc->getTextPos(),len,*format);

	  painter.begin(_page);
	  for (unsigned int j = 0;j < len;j++)
	    _fc->cursorGotoRight(painter);
	  painter.end();
	}
      else if (strList.count() == 2)
	{
	  QString str;
	  unsigned int len;
	  KWFormat *format = new KWFormat(this);
	  format->setDefaults(this);
	  str = QString(strList.at(0));
	  len = str.length();
	  _fc->getParag()->insertText(_fc->getTextPos(),str);
	  _fc->getParag()->setFormat(_fc->getTextPos(),len,*format);

	  painter.begin(_page);
	  for (unsigned int j = 0;j <= len;j++)
	    _fc->cursorGotoRight(painter);
	  painter.end();

	  QKeyEvent ev(Event_KeyPress,Key_Return,13,0);
	  _page->keyPressEvent(&ev);

	  str = QString(strList.at(1));
	  len = str.length();
	  _fc->getParag()->insertText(_fc->getTextPos(),str);
	  _fc->getParag()->setFormat(_fc->getTextPos(),len,*format);

	  painter.begin(_page);
	  for (unsigned int j = 0;j < len;j++)
	    _fc->cursorGotoRight(painter);
	  painter.end();
	}
      else
	{
	  QString str;
	  unsigned int len;
	  KWFormat *format = new KWFormat(this);
	  format->setDefaults(this);
	  str = QString(strList.at(0));
	  len = str.length();
	  _fc->getParag()->insertText(_fc->getTextPos(),str);
	  _fc->getParag()->setFormat(_fc->getTextPos(),len,*format);

	  painter.begin(_page);
	  for (unsigned int j = 0;j < len;j++)
	    _fc->cursorGotoRight(painter);
	  painter.end();

	  QKeyEvent ev(Event_KeyPress,Key_Return,13,0);
	  _page->keyPressEvent(&ev);
	  
	  painter.begin(_page);
	  _fc->cursorGotoLeft(painter);
	  _fc->cursorGotoLeft(painter);
	  painter.end();
	  KWParag *p = _fc->getParag(),*next = _fc->getParag()->getNext();

	  for (unsigned int i = 1;i < strList.count();i++)
	    {
	      str = QString(strList.at(i));
	      len = str.length();
	      p = new KWParag(dynamic_cast<KWTextFrameSet*>(getFrameSet(_fc->getFrameSet() - 1)),this,p,0L,defaultParagLayout);
	      p->insertText(0,str);
	      p->setFormat(0,len,*format);
	    }
	  p->setNext(next);
	  if (next) next->setPrev(p);
	}
    }
}

/*================================================================*/
void KWordDocument_impl::appendPage(unsigned int _page,QPainter &_painter)
{
  pages++;
  QRect pageRect(0,_page * getPTPaperHeight(),getPTPaperWidth(),getPTPaperHeight());

  QList<KWFrame> frameList;
  frameList.setAutoDelete(false);

  KWFrameSet *frameSet = 0L;
  KWFrame *frame;

  for (unsigned int i = 0;i < getNumFrameSets();i++)
    {
      frameSet = getFrameSet(i);

      for (unsigned int j = 0;j < frameSet->getNumFrames();j++)
	{
	  frame = frameSet->getFrame(j);
	  if (frame->intersects(pageRect))
	    frameList.append(new KWFrame(frame->x(),frame->y() + getPTPaperHeight(),frame->width(),frame->height(),frame->getRunAround()));
	}

      if (!frameList.isEmpty())
	{
	  for (unsigned int k = 0;k < frameList.count();k++)
	    frameSet->addFrame(frameList.at(k));
	}

      frameList.clear();
    }
  updateAllRanges();
  drawAllBorders(&_painter);
  updateAllFrames();
}

/*================================================================*/
int KWordDocument_impl::getFrameSet(unsigned int mx,unsigned int my)
{
  KWFrameSet *frameSet = 0L;

  for (unsigned int i = 0;i < getNumFrameSets();i++)
    {
      frameSet = getFrameSet(i);
      if (frameSet->contains(mx,my)) return i;
    }
  
  return -1;
}

/*================================================================*/
void KWordDocument_impl::print(QPainter *painter,QPrinter *printer,float left_margin,float top_margin)
{
  QList<KWFormatContext> fcList;
  fcList.setAutoDelete(true);

  KWFormatContext *fc = 0L;
  unsigned int i = 0,j = 0;

  for (i = 0;i < frames.count();i++)
    {
      if (frames.at(i)->getFrameType() == FT_TEXT)
	{
	  fc = new KWFormatContext(this,i + 1);
	  fc->init(dynamic_cast<KWTextFrameSet*>(frames.at(i))->getFirstParag(),*painter,false,true);
	  fcList.append(fc);
	}
    }

  for (i = 0;i < static_cast<unsigned int>(pages);i++)
    {
      if (i + 1> static_cast<unsigned int>(printer->fromPage())) printer->newPage();
      for (j = 0;j < frames.count();j++)
	{
	  bool bend = false;
	  fc = fcList.at(j);
	  while (fc->getPage() == i + 1 && !bend)
	    {
	      if (i + 1 >= static_cast<unsigned int>(printer->fromPage()) && i + 1 <= static_cast<unsigned int>(printer->toPage()))
		printLine(*fc,*painter,0,i * getPTPaperHeight(),getPTPaperWidth(),getPTPaperHeight());
	      bend = !fc->makeNextLineLayout(*painter);
	    }
	}
    }
}

/*================================================================*/
void KWordDocument_impl::updateAllFrames()
{
  QList<KWFrame> _frames;
  _frames.setAutoDelete(false);
  unsigned int i = 0,j = 0;
  KWFrameSet *frameset = 0L;
  KWFrame *frame1,*frame2;
  KWFrame *framePtr = 0L;

  for (i = 0;i < frames.count();i++)
    {
      frameset = frames.at(i);
      for (j = 0;j < frameset->getNumFrames();j++)
	_frames.append(frameset->getFrame(j));
    }

  for (i = 0;i < _frames.count();i++)
    {
      framePtr = _frames.at(i);
      frame1 = _frames.at(i);
      _frames.at(i)->clearIntersects();

      for (j = 0;j < _frames.count();j++)
	{
	  if (i == j) continue;

	  frame2 = _frames.at(j); 
	  if (frame1->intersects(QRect(frame2->x(),frame2->y(),frame2->width(),frame2->height())))
	    {
	      QRect r = QRect(frame2->x(),frame2->y(),frame2->width(),frame2->height()); //frame1->intersect(QRect(frame2->x(),frame2->y(),frame2->width(),frame2->height()));
	      if (r.left() > frame1->left() || r.top() > frame1->top() || r.right() < frame1->right() || r.bottom() < frame1->bottom())
		{
		  if (r.left() < frame1->left()) r.setLeft(frame1->left());
		  if (r.top() < frame1->top()) r.setTop(frame1->top());
		  if (r.right() > frame1->right()) r.setRight(frame1->right());
		  if (r.bottom() > frame1->bottom()) r.setBottom(frame1->bottom());
		  if (r.left() - frame1->left() > frame1->right() - r.right())
		    r.setRight(frame1->right());
		  else
		    r.setLeft(frame1->left());
		  
		  framePtr->addIntersect(r);
		}
	    }
	}
    }
}
