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

#include <k2url.h>

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
KWordChild::KWordChild(KWordDocument *_wdoc,const KRect& _rect,KOffice::Document_ptr _doc,int diffx,int diffy)
  : KoDocumentChild(_rect,_doc)
{
  m_pKWordDoc = _wdoc;
  m_rDoc = KOffice::Document::_duplicate(_doc);
  setGeometry(KRect(_rect.left() + diffx,_rect.top() + diffy,_rect.width(),_rect.height()));
}

/*================================================================*/
KWordChild::KWordChild(KWordDocument *_wdoc ) 
  : KoDocumentChild()
{
  m_pKWordDoc = _wdoc;
}

/*================================================================*/
KWordChild::~KWordChild()
{
}


/******************************************************************/
/* Class: KWordDocument                                      */
/******************************************************************/

/*================================================================*/
KWordDocument::KWordDocument()
  : formatCollection(this), imageCollection(this), selStart(this,1), selEnd(this,1),
    ret_pix(ICON("return.xpm"))
{
  ADD_INTERFACE("IDL:KOffice/Print:1.0");

  // Use CORBA mechanism for deleting views
  m_lstViews.setAutoDelete(false);
  m_lstChildren.setAutoDelete(true);

  m_bModified = false;
  hasSelection = false;

  rastX = rastY = 10;

  m_bEmpty = true;
  applyStyleTemplate = 0;
  applyStyleTemplate = applyStyleTemplate | U_FONT_FAMILY_ALL_SIZE | U_COLOR | U_BORDER | U_INDENT | U_NUMBERING | U_ALIGN | U_TABS;
  _loaded = false;
  _header = false;
  _footer = false;
}

/*================================================================*/
CORBA::Boolean KWordDocument::init()
{
  pageLayout.format = PG_DIN_A4;
  pageLayout.orientation = PG_PORTRAIT;
  pageLayout.width = PG_A4_WIDTH;
  pageLayout.height = PG_A4_HEIGHT;
  pageLayout.left = DEFAULT_LEFT_BORDER;
  pageLayout.right = DEFAULT_RIGHT_BORDER;
  pageLayout.top = DEFAULT_TOP_BORDER;
  pageLayout.bottom = DEFAULT_BOTTOM_BORDER;  
  pageLayout.ptWidth = MM_TO_POINT(PG_A4_WIDTH);
  pageLayout.ptHeight = MM_TO_POINT(PG_A4_HEIGHT);
  pageLayout.ptLeft = MM_TO_POINT(DEFAULT_LEFT_BORDER);
  pageLayout.ptRight = MM_TO_POINT(DEFAULT_RIGHT_BORDER);
  pageLayout.ptTop = MM_TO_POINT(DEFAULT_TOP_BORDER);
  pageLayout.ptBottom = MM_TO_POINT(DEFAULT_BOTTOM_BORDER);  
  pageLayout.unit = PG_MM;

//   defaultUserFont = new KWUserFont(this,"times");
//   defaultParagLayout = new KWParagLayout(this);
//   defaultParagLayout->setName("Standard");
//   defaultParagLayout->setCounterNr(-1);
    
  pages = 1;

  pageColumns.columns = 1; //STANDARD_COLUMNS;
  pageColumns.columnSpacing = STANDARD_COLUMN_SPACING;

  QString _template;
  QString _globalTemplatePath = kapp->kde_datadir() + "/kword/templates/";
  QString _personalTemplatePath = kapp->localkdedir() + "/share/apps/kword/templates/";

  if (KoTemplateChooseDia::chooseTemplate(_globalTemplatePath,_personalTemplatePath,_template,false))
    {
      QFileInfo fileInfo(_template);
      QString fileName(fileInfo.dirPath(true) + "/" + fileInfo.baseName() + ".kwt");
      loadTemplate(fileName.data());
      
//       bool __footer = false,__header = false;

//       for (unsigned int k = 0;k < getNumFrameSets();k++)
// 	{
// 	  if (getFrameSet(k)->getFrameInfo() == FI_HEADER) __header = true;
// 	  if (getFrameSet(k)->getFrameInfo() == FI_FOOTER) __footer = true;
// 	}

//       if (!__header)
// 	{
// 	  KWTextFrameSet *fs = new KWTextFrameSet(this);
// 	  fs->setFrameInfo(FI_HEADER);
// 	  KWFrame *frame = new KWFrame(getFrameSet(0)->getFrame(0)->left(),getFrameSet(0)->getFrame(0)->top(),
// 				       getFrameSet(0)->getFrame(0)->width(),20);
// 	  fs->setAutoCreateNewFrame(false);
// 	  fs->addFrame(frame);
// 	  frames.append(fs);
// 	}

//       if (!__footer)
// 	{
// 	  KWTextFrameSet *fs = new KWTextFrameSet(this);
// 	  fs->setFrameInfo(FI_FOOTER);
// 	  KWFrame *frame = new KWFrame(getFrameSet(0)->getFrame(0)->left(),getFrameSet(0)->getFrame(0)->top() + 
// 				       getFrameSet(0)->getFrame(0)->height() - 20,
// 				       getFrameSet(0)->getFrame(0)->width(),20);
// 	  fs->setAutoCreateNewFrame(false);
// 	  fs->addFrame(frame);
// 	  frames.append(fs);
// 	}

//       recalcFrames();
    }
  else
    debug("no template chosen");

  return true;
}

/*================================================================*/
bool KWordDocument::loadTemplate(const char *_url)
{
  K2URL u(_url);
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
  
  if ( !loadXML( parser, 0L ) )
    return false;
 
  setModified( true );

  _loaded = false;
  return true;
}

/*================================================================*/
void KWordDocument::setPageLayout(KoPageLayout _layout,KoColumns _cl)
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
      pageLayout.ptLeft = 0;
      pageLayout.ptRight = 0;
      pageLayout.ptTop = 0;
      pageLayout.ptBottom = 0;
    }

  if (processingType == WP)
    recalcFrames();

  updateAllFrames();
  updateAllCursors(); 
}

/*================================================================*/
void KWordDocument::recalcFrames()
{
  if (processingType == DTP) return;

  pages = 1;
  KWTextFrameSet *frameset = dynamic_cast<KWTextFrameSet*>(frames.at(0));

  unsigned int frms = frameset->getNumFrames();

  ptColumnWidth = (getPTPaperWidth() - getPTLeftBorder() - getPTRightBorder() - getPTColumnSpacing() * (pageColumns.columns - 1)) 
    / pageColumns.columns;

  int headOffset = 0,footOffset = 0;
  KWTextFrameSet *header = 0L,*footer = 0L;
  if (hasHeader() || hasFooter())
    {
      for (unsigned int k = 0;k < getNumFrameSets();k++)
	{
	  if (getFrameSet(k)->getFrameInfo() == FI_HEADER && hasHeader())
	    {
	      header = dynamic_cast<KWTextFrameSet*>(getFrameSet(k));
	      headOffset = 10 + getFrameSet(k)->getFrame(0)->height(); 
	    }
	  if (getFrameSet(k)->getFrameInfo() == FI_FOOTER && hasFooter())
	    {
	      footer = dynamic_cast<KWTextFrameSet*>(getFrameSet(k));
	      footOffset = 10 + getFrameSet(k)->getFrame(0)->height(); 
	    }
	}
    }
 
  for (unsigned int j = 0;j < static_cast<unsigned int>(ceil(static_cast<double>(frms) / static_cast<double>(pageColumns.columns)));j++)
    {
      for (int i = 0;i < pageColumns.columns;i++)
	{
	  if (j * pageColumns.columns + i < frameset->getNumFrames())
	    {
	      frameset->getFrame(j * pageColumns.columns + i)->setRect(getPTLeftBorder() + i * (ptColumnWidth + getPTColumnSpacing()),
								       j * getPTPaperHeight() + getPTTopBorder() + headOffset,ptColumnWidth,
								       getPTPaperHeight() - getPTTopBorder() - getPTBottomBorder() -
								       headOffset - footOffset);
	    }
	  else
	    {
	      frameset->addFrame(new KWFrame(getPTLeftBorder() + i * (ptColumnWidth + getPTColumnSpacing()),
					     j * getPTPaperHeight() + getPTTopBorder() + headOffset,
					     ptColumnWidth,getPTPaperHeight() - getPTTopBorder() - getPTBottomBorder() -
					     headOffset - footOffset));
	    }
	}
    }

  pages = static_cast<int>(ceil(static_cast<double>(frms) / static_cast<double>(pageColumns.columns)));

  if (headOffset > 0 && hasHeader())
    {
      int h = header->getFrame(0)->height();
      bool selected = header->getFrame(0)->isSelected();
      header->clear();
      for (int l = 0;l < pages;l++)
	{
	  KWFrame *frame = new KWFrame(getFrameSet(0)->getFrame(0)->left(),l * getPTPaperHeight() + getPTTopBorder(),
				       getPTPaperWidth() - getPTLeftBorder() - getPTRightBorder(),h);
	  frame->setSelected(selected);
	  header->addFrame(frame);
	}
    }

  if (footOffset > 0 && hasFooter())
    {
      int h = footer->getFrame(0)->height();
      bool selected = footer->getFrame(0)->isSelected();
      footer->clear();
      for (int l = 0;l < pages;l++)
	{
	  KWFrame *frame = new KWFrame(getFrameSet(0)->getFrame(0)->left(),(l + 1) * getPTPaperHeight() - getPTBottomBorder() - h,
				       getPTPaperWidth() - getPTLeftBorder() - getPTRightBorder(),h);
	  frame->setSelected(selected);
	  footer->addFrame(frame);
	}
    }

  recalcWholeText();
  updateAllRanges();
}

/*================================================================*/
KWordDocument::~KWordDocument()
{
  cerr << "KWordDocument::~KWordDocument()" << endl;
  cleanUp();
  cerr << "...KWordDocument::~KWordDocument()" << endl;
}

/*================================================================*/
void KWordDocument::cleanUp()
{
  if (m_bIsClean) return;

  assert( m_lstViews.count() == 0 );
  
  m_lstChildren.clear();

  KoDocument::cleanUp();
}

/*================================================================*/
bool KWordDocument::hasToWriteMultipart()
{  
  if (m_lstChildren.count() == 0) return false;
  
  return true;
}

/*================================================================*/
bool KWordDocument::loadChildren( KOStore::Store_ptr _store )
{
  cerr << "bool KWordDocument::loadChildren( OPParts::MimeMultipartDict_ptr _dict )" << endl;
  
  QListIterator<KWordChild> it(m_lstChildren);
  for(;it.current();++it)
    {
      cerr << "Loading child" << endl;
      if (!it.current()->loadDocument( _store, it.current()->mimeType() ) )
	return false;
    }

  cerr << "Loading done" << endl;
  
  return true;
}

/*================================================================*/
bool KWordDocument::loadXML( KOMLParser& parser, KOStore::Store_ptr )
{
  _loaded = true;

  pageLayout.format = PG_DIN_A4;
  pageLayout.orientation = PG_PORTRAIT;
  pageLayout.width = PG_A4_WIDTH;
  pageLayout.height = PG_A4_HEIGHT;
  pageLayout.left = DEFAULT_LEFT_BORDER;
  pageLayout.right = DEFAULT_RIGHT_BORDER;
  pageLayout.top = DEFAULT_TOP_BORDER;
  pageLayout.bottom = DEFAULT_BOTTOM_BORDER;  
  pageLayout.ptWidth = MM_TO_POINT(PG_A4_WIDTH);
  pageLayout.ptHeight = MM_TO_POINT(PG_A4_HEIGHT);
  pageLayout.ptLeft = MM_TO_POINT(DEFAULT_LEFT_BORDER);
  pageLayout.ptRight = MM_TO_POINT(DEFAULT_RIGHT_BORDER);
  pageLayout.ptTop = MM_TO_POINT(DEFAULT_TOP_BORDER);
  pageLayout.ptBottom = MM_TO_POINT(DEFAULT_BOTTOM_BORDER);  
  pageLayout.unit = PG_MM;

  defaultUserFont = findUserFont("times");
  defaultParagLayout = new KWParagLayout(this);
  defaultParagLayout->setName("Standard");
  defaultParagLayout->setCounterType(KWParagLayout::CT_NONE);
  defaultParagLayout->setCounterDepth(0);
    
  KWFormat f(this);
  f.setUserFont(findUserFont("helvetica"));
  f.setWeight(75);
  f.setPTFontSize(24);
  KWParagLayout *lay = new KWParagLayout(this);
  lay->setName("Head 1");
  lay->setFollowingParagLayout("Standard");
  lay->setCounterType(KWParagLayout::CT_NUM);
  lay->setCounterDepth(0);
  lay->setStartCounter("1");
  lay->setCounterRightText(".");
  lay->setNumberingType(KWParagLayout::NT_CHAPTER);
  lay->setFormat(f);

  f.setPTFontSize(16);
  lay = new KWParagLayout(this);
  lay->setName("Head 2");
  lay->setFollowingParagLayout("Standard");
  lay->setCounterType(KWParagLayout::CT_NUM);
  lay->setCounterDepth(1);
  lay->setStartCounter("1");
  lay->setCounterRightText(".");
  lay->setNumberingType(KWParagLayout::NT_CHAPTER);
  lay->setFormat(f);

  f.setPTFontSize(12);
  lay = new KWParagLayout(this);
  lay->setName("Head 3");
  lay->setFollowingParagLayout("Standard");
  lay->setCounterType(KWParagLayout::CT_NUM);
  lay->setCounterDepth(2);
  lay->setStartCounter("1");
  lay->setCounterRightText(".");
  lay->setNumberingType(KWParagLayout::NT_CHAPTER);
  lay->setFormat(f);

  lay = new KWParagLayout(this);
  lay->setName("Enumerated List");
  lay->setFollowingParagLayout("Enumerated List");
  lay->setCounterType(KWParagLayout::CT_NUM);
  lay->setCounterDepth(0);
  lay->setStartCounter("1");
  lay->setCounterRightText(".");
  lay->setNumberingType(KWParagLayout::NT_LIST);

  lay = new KWParagLayout(this);
  lay->setName("Alphabetical List");
  lay->setFollowingParagLayout("Alphabetical List");
  lay->setCounterType(KWParagLayout::CT_ALPHAB_L);
  lay->setCounterDepth(0);
  lay->setStartCounter("a");
  lay->setCounterRightText(")");
  lay->setNumberingType(KWParagLayout::NT_LIST);

  lay = new KWParagLayout(this);
  lay->setName("Bullet List");
  lay->setFollowingParagLayout("Bullet List");
  lay->setCounterType(KWParagLayout::CT_BULLET);
  lay->setCounterDepth(0);
  lay->setStartCounter("1");
  lay->setCounterRightText("");
  lay->setNumberingType(KWParagLayout::NT_LIST);

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
	  QRect r = ch->geometry();
	  insertChild(ch);
	  KWPartFrameSet *frameset = new KWPartFrameSet(this,ch);
	  KWFrame *frame = new KWFrame(r.x(),r.y(),r.width(),r.height());
	  frameset->addFrame(frame);
	  addFrameSet(frameset);
	  emit sig_insertObject(ch,frameset);
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
		{
		  __pgLayout.width = static_cast<double>(atof((*it).m_strValue.c_str()));
		  __pgLayout.ptWidth = MM_TO_POINT(static_cast<double>(atof((*it).m_strValue.c_str())));
		}	      
	      else if ((*it).m_strName == "height")
		{
		  __pgLayout.height = static_cast<double>(atof((*it).m_strValue.c_str()));
		  __pgLayout.ptHeight = MM_TO_POINT(static_cast<double>(atof((*it).m_strValue.c_str())));
		}	      
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
			{
			  __pgLayout.left = (double)atof((*it).m_strValue.c_str());
			  __pgLayout.ptLeft = MM_TO_POINT((double)atof((*it).m_strValue.c_str()));
			}
		      else if ((*it).m_strName == "top")
			{
			  __pgLayout.top = (double)atof((*it).m_strValue.c_str());
			  __pgLayout.ptTop = MM_TO_POINT((double)atof((*it).m_strValue.c_str()));
			}		      
		      else if ((*it).m_strName == "right")
			{
			  __pgLayout.right = (double)atof((*it).m_strValue.c_str());
			  __pgLayout.ptRight = MM_TO_POINT((double)atof((*it).m_strValue.c_str()));
			}		      
		      else if ((*it).m_strName == "bottom")
			{
			  __pgLayout.bottom = (double)atof((*it).m_strValue.c_str());
			  __pgLayout.ptBottom = MM_TO_POINT((double)atof((*it).m_strValue.c_str()));
			}		      
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

      else if (name == "STYLES")
	{
	  KOMLParser::parseTag(tag.c_str(),name,lst);
	  vector<KOMLAttrib>::const_iterator it = lst.begin();
	  for(;it != lst.end();it++)
	    {
	    }
	  loadStyleTemplates(parser,lst);
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

  bool __footer = false,__header = false;
  
  for (unsigned int k = 0;k < getNumFrameSets();k++)
    {
      if (getFrameSet(k)->getFrameInfo() == FI_HEADER) __header = true;
      if (getFrameSet(k)->getFrameInfo() == FI_FOOTER) __footer = true;
    }
  
  if (!__header)
    {
      KWTextFrameSet *fs = new KWTextFrameSet(this);
      fs->setFrameInfo(FI_HEADER);
      KWFrame *frame = new KWFrame(getFrameSet(0)->getFrame(0)->left(),getFrameSet(0)->getFrame(0)->top(),
				   getFrameSet(0)->getFrame(0)->width(),20);
      fs->setAutoCreateNewFrame(false);
      fs->addFrame(frame);
      frames.append(fs);
    }
  
  if (!__footer)
    {
      KWTextFrameSet *fs = new KWTextFrameSet(this);
      fs->setFrameInfo(FI_FOOTER);
      KWFrame *frame = new KWFrame(getFrameSet(0)->getFrame(0)->left(),getFrameSet(0)->getFrame(0)->top() + 
				   getFrameSet(0)->getFrame(0)->height() - 20,
				   getFrameSet(0)->getFrame(0)->width(),20);
      fs->setAutoCreateNewFrame(false);
      fs->addFrame(frame);
      frames.append(fs);
    }
  
  return true;
}

/*================================================================*/
void KWordDocument::loadStyleTemplates(KOMLParser& parser,vector<KOMLAttrib>& lst)
{
  string tag;
  string name;

  while (parser.open(0L,tag))
    {
      KOMLParser::parseTag(tag.c_str(),name,lst);
	      
      if (name == "STYLE")
	{
	  KOMLParser::parseTag(tag.c_str(),name,lst);
	  vector<KOMLAttrib>::const_iterator it = lst.begin();
	  for(;it != lst.end();it++)
	    {
	    }
	  KWParagLayout *pl = new KWParagLayout(this,false);
	  pl->load(parser,lst);
	  addStyleTemplate(pl);
	}
      
      else
	cerr << "Unknown tag '" << tag << "' in STYLES" << endl;    
      
      if (!parser.close(tag))
	{
	  cerr << "ERR: Closing Child" << endl;
	  return;
	}
    }
}

/*================================================================*/
void KWordDocument::loadFrameSets(KOMLParser& parser,vector<KOMLAttrib>& lst)
{
  string tag;
  string name;

  bool autoCreateNewFrame = true;

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
	      if ((*it).m_strName == "autoCreateNewFrame")
		autoCreateNewFrame = atoi((*it).m_strValue.c_str());
	    }

	  switch (frameType)
	    {
	    case FT_TEXT:
	      {
		KWTextFrameSet *frame = new KWTextFrameSet(this);
		frame->load(parser,lst);
		frame->setAutoCreateNewFrame(autoCreateNewFrame);
		frames.append(frame);
	      } break;
	    case FT_PICTURE:
	      {
		KWPictureFrameSet *frame = new KWPictureFrameSet(this);
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
bool KWordDocument::save( ostream &out, const char* /* _format */ )
{
  out << "<?xml version=\"1.0\"?>" << endl;
  //out << "<!DOCTYPE DOC SYSTEM \"" << kapp->kde_datadir() << "/kword/dtd/kword.dtd\"/>" << endl;
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
      if (frameSet->getFrameType() != FT_PART)
	frameSet->save(out);
    }

  out << etag << "</FRAMESETS>" << endl;

  out << otag << "<STYLES>" << endl;
  for (unsigned int j = 0;j < paragLayoutList.count();j++)
    {
      out << otag << "<STYLE>" << endl;
      paragLayoutList.at(j)->save(out);
      out << etag << "</STYLE>" << endl;
    }
  out << etag << "</STYLES>" << endl;

  // Write "OBJECT" tag for every child
  QListIterator<KWordChild> chl(m_lstChildren);
  for(;chl.current();++chl)
    chl.current()->save( out );

  out << etag << "</DOC>" << endl;
    
  return true;
}

/*================================================================*/
void KWordDocument::makeChildListIntern( KOffice::Document_ptr _doc,const char *_path)
{
  cerr << "void KWordDocument::makeChildList( OPParts::Document_ptr _doc, const char *_path )" << endl;
  
  int i = 0;
  
  QListIterator<KWordChild> it(m_lstChildren);
  for(;it.current();++it)
    {
      QString tmp;
      tmp.sprintf("/%i",i++);
      QString path(_path);
      path += tmp.data();
      cerr << "SETTING NAME To " << path.data() << endl;
    
      KOffice::Document_var doc = it.current()->document();    
      doc->makeChildList(_doc,path);
    }
}

/*================================================================*/
QStrList KWordDocument::outputFormats()
{
  return QStrList();
}

/*================================================================*/
QStrList KWordDocument::inputFormats()
{
  return QStrList();
}

/*================================================================*/
void KWordDocument::viewList(OpenParts::Document::ViewList*& _list)
{
  (*_list).length(m_lstViews.count());

  int i = 0;
  QListIterator<KWordView> it(m_lstViews);
  for(;it.current();++it)
    {
      (*_list)[i++] = OpenParts::View::_duplicate(it.current());
    }
}

/*================================================================*/
void KWordDocument::addView(KWordView *_view)
{
  m_lstViews.append(_view);
}

/*================================================================*/
void KWordDocument::removeView(KWordView *_view)
{
  m_lstViews.setAutoDelete(false);
  m_lstViews.removeRef(_view);
  m_lstViews.setAutoDelete(true);
}

/*================================================================*/
KWordView* KWordDocument::createWordView()
{
  KWordView *p = new KWordView( 0L, 0L, this );
  p->QWidget::show();
  m_lstViews.append( p );
  
  return p;
}

/*================================================================*/
OpenParts::View_ptr KWordDocument::createView()
{
  return OpenParts::View::_duplicate( createWordView() );
}

/*================================================================*/
void KWordDocument::insertObject(const KRect& _rect, KoDocumentEntry& _e, int diffx, int diffy )
{
  KOffice::Document_var doc = imr_createDoc( _e );
  if (CORBA::is_nil(doc))
    return;

  if (!doc->init())
    {
      QMessageBox::critical((QWidget*)0L,i18n("KWord Error"),i18n("Could not init"),i18n("OK"));
      return;
    }

  KWordChild* ch = new KWordChild(this,_rect,doc,diffx,diffy);

  insertChild(ch);
  m_bModified = true;

  KWPartFrameSet *frameset = new KWPartFrameSet(this,ch);
  KWFrame *frame = new KWFrame(_rect.x() + diffx,_rect.y() + diffy,_rect.width(),_rect.height());
  frameset->addFrame(frame);
  addFrameSet(frameset);

  emit sig_insertObject(ch,frameset);

  //updateAllViews(0L);
}

/*================================================================*/
void KWordDocument::insertChild(KWordChild *_child)
{
  m_lstChildren.append(_child);
}

/*================================================================*/
void KWordDocument::changeChildGeometry(KWordChild *_child,const KRect& _rect)
{
  _child->setGeometry(_rect);

  emit sig_updateChildGeometry(_child);
}

/*================================================================*/
QListIterator<KWordChild> KWordDocument::childIterator()
{
  return QListIterator<KWordChild> (m_lstChildren);
}

/*================================================================*/
void KWordDocument::draw(QPaintDevice* _dev,CORBA::Long _width,CORBA::Long _height)
{
}

/*================================================================*/
QPen KWordDocument::setBorderPen(KWParagLayout::Border _brd)
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
KWUserFont* KWordDocument::findUserFont(QString _userfontname)
{
  KWUserFont* font = 0L;
  for (font = userFontList.first();font != 0L;font = userFontList.next())
    if (font->getFontName() == _userfontname)
      return font;
  
  font = new KWUserFont(this,_userfontname);
  return font;
}

/*================================================================*/
KWDisplayFont* KWordDocument::findDisplayFont(KWUserFont* _font,unsigned int _size,int _weight,bool _italic,bool _underline)
{
  KWDisplayFont* font = 0L;
  for (font = displayFontList.first();font != 0L;font = displayFontList.next())
    {
      if (font->getUserFont()->getFontName() == _font->getFontName() && font->getPTSize() == _size &&
	  font->weight() == _weight && font->italic() == _italic && font->underline() == _underline)
	return font;
    }
  
  font = new KWDisplayFont(this,_font,_size,_weight,_italic,_underline);
  return font;
}

/*================================================================*/
KWParagLayout* KWordDocument::findParagLayout(QString _name)
{
  KWParagLayout* p;
  for (p = paragLayoutList.first();p != 0L;p = paragLayoutList.next())
    if (p->getName() == _name)
      return p;
  
  return 0L;
}

/*================================================================*/
bool KWordDocument::isPTYInFrame(unsigned int _frameSet,unsigned int _frame,unsigned int _ypos)
{
  return frames.at(_frameSet)->isPTYInFrame(_frame,_ypos);
}

/*================================================================*/
KWParag* KWordDocument::findFirstParagOfPage(unsigned int _page,unsigned int _frameset)
{
  if (frames.at(_frameset)->getFrameType() != FT_TEXT) return 0L;

  KWParag *p = dynamic_cast<KWTextFrameSet*>(frames.at(_frameset))->getFirstParag();
  while (p)
    {
      if (p->getEndPage() == _page || p->getStartPage() == _page || (p->getEndPage() > _page && p->getStartPage() < _page))
 	return p;
      p = p->getNext();
    }
  
  return 0L;
}

/*================================================================*/
KWParag* KWordDocument::findFirstParagOfRect(unsigned int _ypos,unsigned int _page,unsigned int _frameset)
{
  if (frames.at(_frameset)->getFrameType() != FT_TEXT) return 0L;

  KWParag *p = dynamic_cast<KWTextFrameSet*>(frames.at(_frameset))->getFirstParag();
  while (p)
    {
      if (p->getPTYEnd() >= _ypos || p->getPTYStart() >= _ypos || (p->getPTYEnd() >= _ypos && p->getPTYStart() <= _ypos)
	  || (p->getPTYEnd() <= _ypos && p->getPTYStart() <= _ypos && p->getPTYStart() > p->getPTYEnd() &&
	      (p->getEndPage() == _page || p->getStartPage() == _page || (p->getEndPage() > _page && p->getStartPage() < _page))))
 	return p;
      p = p->getNext();
    }
  
  return 0L;
}

/*================================================================*/
bool KWordDocument::printLine(KWFormatContext &_fc,QPainter &_painter,int xOffset,int yOffset,int _w,int _h,bool _viewFormattingChars = false)
{
  _painter.save();
  //_painter.setRasterOp(CopyROP);

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

  if (static_cast<int>(_fc.getPTY() + _fc.getLineHeight()) > getFrameSet(_fc.getFrameSet() - 1)->getFrame(_fc.getFrame() - 1)->bottom())
    cr = QRegion(0,0,0,0);

  QRegion visible(0,0,_w,_h);

  if (_painter.hasClipping())
    cr = _painter.clipRegion().intersect(cr);

  if (cr.intersect(visible).isEmpty())
    {
      _painter.restore();
      return false;
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
  if (pos == 0 && lay->getCounterType() != KWParagLayout::CT_NONE)
    {
      //_painter.fillRect(_fc.getPTCounterPos() - xOffset,_fc.getPTY(),_fc.getPTCounterWidth(),_fc.getLineHeight(),lightGray);
      KWFormat counterfm(this,_fc);
      counterfm.apply(lay->getFormat());
      if (_fc.getParag()->getParagLayout()->getCounterType() == KWParagLayout::CT_BULLET)
	counterfm.setUserFont(findUserFont(_fc.getParag()->getParagLayout()->getBulletFont()));
      _painter.setFont(*(counterfm.loadFont(this)));
      _painter.setPen(counterfm.getColor());
      
      _painter.drawText(_fc.getPTCounterPos() - xOffset,_fc.getPTY() + _fc.getPTMaxAscender() - yOffset,_fc.getCounterText());
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
  unsigned int lastPTPos = 0;
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
	  return false;
	}

      buffer[i] = text[ _fc.getTextPos() ].c;
      
      if ( buffer[i] == 0 )
	{
	  buffer[i] = '\0';
	  _painter.drawText( tmpPTPos - xOffset, /*_fc.getPTY() + _fc.getPTMaxAscender() - yOffset*/
			     _fc.getPTY() + _fc.getLineHeight() - _fc.getPTMaxDescender() - 
			     _fc.getParag()->getParagLayout()->getPTLineSpacing() - yOffset + plus, buffer );
	  i = 0;

	  switch (text[_fc.getTextPos()].attrib->getClassId())
	    {
	    case ID_KWCharImage:
	      {
		_painter.drawImage(KPoint(tmpPTPos - xOffset, _fc.getPTY() - yOffset + 
					  ((_fc.getLineHeight() - _fc.getParag()->getParagLayout()->getPTLineSpacing()) 
					   - ((KWCharImage*)text[ _fc.getTextPos() ].attrib)->getImage()->height())),
				   *((KWCharImage*)text[ _fc.getTextPos() ].attrib)->getImage());
		_fc.cursorGotoNextChar( _painter );
	      } break;
	    case ID_KWCharTab:
	      {
		lastPTPos = _fc.getPTPos();
		_fc.cursorGotoNextChar( _painter );
		QPen _pen = QPen(_painter.pen());
		_painter.setPen(QPen(blue,1,DotLine));
		if (_viewFormattingChars)
		  _painter.drawLine(lastPTPos,_fc.getPTY() + _fc.getPTMaxAscender(),
				    _fc.getPTPos(),_fc.getPTY() + _fc.getPTMaxAscender());
		_painter.setPen(_pen);
	      } break;
	    }
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
	  //lastPTPos = _fc.getPTPos();
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
	      // Blanks are not printed at all - but we have to underline it in some cases
	      //lastPTPos = _fc.getPTPos();
	      if (text[_fc.getTextPos()].c == ' ')
		{
		  bool goneForward = false;
		  if (_fc.getUnderline() && _fc.getTextPos() > _fc.getLineStartPos() && _fc.getTextPos() < _fc.getLineEndPos() - 1)
		    {
		      if (text[_fc.getTextPos() - 1].c != 0 && text[_fc.getTextPos() + 1].c != 0)
			{
			  KWCharFormat *f1 = (KWCharFormat*)text[_fc.getTextPos() - 1].attrib;
			  KWCharFormat *f2 = (KWCharFormat*)text[_fc.getTextPos() + 1].attrib;
			  if (f1->getFormat()->getUnderline() && f2->getFormat()->getUnderline())
			    {
			      KWFormat *_f = f1->getFormat();
			      QFontMetrics fm(*findDisplayFont(_f->getUserFont(),_f->getPTFontSize(),_f->getWeight(),
							       _f->getItalic(),_f->getUnderline()));
			      
			      _painter.setPen(QPen(_fc.getColor(),fm.lineWidth(),SolidLine));
			      int ly = _fc.getPTY() + _fc.getLineHeight() - _fc.getPTMaxDescender() - yOffset - 
				 _fc.getParag()->getParagLayout()->getPTLineSpacing() + plus + fm.underlinePos() + fm.lineWidth() / 2;
			      int lx1 = _fc.getPTPos();
			      _fc.cursorGotoNextChar(_painter);
			      goneForward = true;
			      int lx2 = _fc.getPTPos();
			      _painter.drawLine(lx1,ly,lx2,ly);
			    }
			}
			  
		    }
		  lastPTPos = _fc.getPTPos();
		  if (!goneForward) _fc.cursorGotoNextChar(_painter);
		  if (_viewFormattingChars)
		    _painter.fillRect(lastPTPos + (_fc.getPTPos() - lastPTPos) / 2,_fc.getPTY() + _fc.getPTMaxAscender() / 2,1,1,blue);
		}
	    }
	}
    }

  if (_viewFormattingChars && _fc.isCursorAtParagEnd())
    _painter.drawPixmap(_fc.getPTPos() + 3,_fc.getPTY() + _fc.getPTMaxAscender() - ret_pix.height(),ret_pix);
  
  _painter.restore();
  return true;
}

/*================================================================*/
void KWordDocument::drawMarker(KWFormatContext &_fc,QPainter *_painter,int xOffset,int yOffset)
{
  RasterOp rop = _painter->rasterOp();
    
  _painter->setRasterOp(NotROP);
  QPen pen;
  pen.setWidth(2);
  _painter->setPen(pen);
  
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
void KWordDocument::updateAllViews(KWordView *_view)
{
  KWordView *viewPtr;

  if (!m_lstViews.isEmpty())
    {
      for (viewPtr = m_lstViews.first();viewPtr != 0;viewPtr = m_lstViews.next())
	if (viewPtr != _view) viewPtr->getGUI()->getPaperWidget()->repaint(false);
    }
}

/*================================================================*/
void KWordDocument::updateAllRanges()
{
  KWordView *viewPtr;

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
void KWordDocument::updateAllCursors()
{
  KWordView *viewPtr;

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
void KWordDocument::updateAllStyleLists()
{
  KWordView *viewPtr;

  if (!m_lstViews.isEmpty())
    {
      for (viewPtr = m_lstViews.first();viewPtr != 0;viewPtr = m_lstViews.next())
	{
	  viewPtr->updateStyleList();
	}
    }
}

/*================================================================*/
void KWordDocument::drawAllBorders(QPainter *_painter = 0)
{
  KWordView *viewPtr;
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
void KWordDocument::updateAllStyles()
{
  KWFrameSet *frameSet = 0L;

  for (unsigned int i = 0;i < getNumFrameSets();i++)
    {
      frameSet = getFrameSet(i);
      if (frameSet->getFrameType() == FT_TEXT)
	dynamic_cast<KWTextFrameSet*>(frameSet)->updateAllStyles();
    }

  updateAllViews(0L);
  changedStyles.clear();
}

/*================================================================*/
void KWordDocument::insertPicture(QString _filename,KWPage *_paperWidget)
{
  _paperWidget->insertPictureAsChar(_filename);
}

/*================================================================*/
void KWordDocument::drawSelection(QPainter &_painter,int xOffset,int yOffset)
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

  if (tmpFC1.getPTY() == tmpFC2.getPTY())
    _painter.drawRect(tmpFC1.getPTPos() - xOffset,tmpFC2.getPTY() - yOffset,
		      tmpFC2.getPTPos() - tmpFC1.getPTPos(),tmpFC2.getLineHeight());
  else
    {
      _painter.drawRect(tmpFC1.getPTPos() - xOffset,tmpFC1.getPTY() - yOffset,
			tmpFC1.getPTLeft() + tmpFC1.getPTWidth() - tmpFC1.getPTPos(),tmpFC1.getLineHeight());
      tmpFC1.makeNextLineLayout(_painter);
      
      while (tmpFC1.getPTY() < tmpFC2.getPTY() || tmpFC1.getFrame() != tmpFC2.getFrame())
	{
	  _painter.drawRect(tmpFC1.getPTLeft() - xOffset,tmpFC1.getPTY() - yOffset,tmpFC1.getPTWidth(),tmpFC1.getLineHeight());
	  tmpFC1.makeNextLineLayout(_painter);
	}
      
      _painter.drawRect(tmpFC2.getPTLeft() - xOffset,tmpFC2.getPTY() - yOffset,tmpFC2.getPTPos() - tmpFC2.getPTLeft(),tmpFC2.getLineHeight());
    }

  _painter.setRasterOp(rop);
  _painter.restore();
}

/*================================================================*/
void KWordDocument::deleteSelectedText(KWFormatContext *_fc,QPainter &_painter)
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
void KWordDocument::copySelectedText()
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
void KWordDocument::setFormat(KWFormat &_format)
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
	  if (parag->getTextLen() > 0)
	    parag->setFormat(0,parag->getTextLen(),_format);
	  parag = parag->getNext();
	}
      tmpFC2.getParag()->setFormat(0,tmpFC2.getTextPos(),_format);
    }
}

/*================================================================*/
void KWordDocument::paste(KWFormatContext *_fc,QString _string,KWPage *_page)
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
void KWordDocument::appendPage(unsigned int _page,QPainter &_painter)
{
  pages++;
  KRect pageRect(0,_page * getPTPaperHeight(),getPTPaperWidth(),getPTPaperHeight());

  QList<KWFrame> frameList;
  frameList.setAutoDelete(false);

  KWFrameSet *frameSet = 0L;
  KWFrame *frame;

  for (unsigned int i = 0;i < getNumFrameSets();i++)
    {
      if (getFrameSet(i)->getFrameType() != FT_TEXT) continue;
      frameSet = getFrameSet(i);

      for (unsigned int j = 0;j < frameSet->getNumFrames();j++)
	{
	  frame = frameSet->getFrame(j);
	  if (frame->intersects(pageRect))
	    frameList.append(new KWFrame(frame->x(),frame->y() + getPTPaperHeight(),frame->width(),frame->height(),frame->getRunAround(),
					 frame->getRunAroundGap()));
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
int KWordDocument::getFrameSet(unsigned int mx,unsigned int my)
{
  KWFrameSet *frameSet = 0L;

  for (unsigned int i = 0;i < getNumFrameSets();i++)
    {
      frameSet = getFrameSet(getNumFrameSets() - 1 - i);
      if (frameSet->contains(mx,my)) 
	{
	  if (frameSet->getFrameType() == FT_TEXT && 
	      ((dynamic_cast<KWTextFrameSet*>(frameSet)->getFrameInfo() == FI_HEADER && !hasHeader()) ||
	       (dynamic_cast<KWTextFrameSet*>(frameSet)->getFrameInfo() == FI_FOOTER && !hasFooter())))
	    continue;
	  return getNumFrameSets() - 1 - i;
	}
    }
  
  return -1;
}

/*================================================================*/
int KWordDocument::selectFrame(unsigned int mx,unsigned int my)
{
  KWFrameSet *frameSet = 0L;

  for (unsigned int i = 0;i < getNumFrameSets();i++)
    {
      frameSet = getFrameSet(getNumFrameSets() - 1 - i);
      if (frameSet->contains(mx,my))
	{
	  if (frameSet->getFrameType() == FT_TEXT && 
	      ((dynamic_cast<KWTextFrameSet*>(frameSet)->getFrameInfo() == FI_HEADER && !hasHeader()) ||
	       (dynamic_cast<KWTextFrameSet*>(frameSet)->getFrameInfo() == FI_FOOTER && !hasFooter())))
	    continue;
	  return frameSet->selectFrame(mx,my);
	}
    }

  deSelectAllFrames();
  return 0;
}

/*================================================================*/
void KWordDocument::deSelectFrame(unsigned int mx,unsigned int my)
{
  KWFrameSet *frameSet = 0L;

  for (unsigned int i = 0;i < getNumFrameSets();i++)
    {
      frameSet = getFrameSet(getNumFrameSets() - 1 - i);
      if (frameSet->contains(mx,my))
	frameSet->deSelectFrame(mx,my);
    }
}

/*================================================================*/
void KWordDocument::deSelectAllFrames()
{
  KWFrameSet *frameSet = 0L;

  for (unsigned int i = 0;i < getNumFrameSets();i++)
    {
      frameSet = getFrameSet(getNumFrameSets() - 1 - i);
      for (unsigned int j = 0;j < frameSet->getNumFrames();j++)
	frameSet->getFrame(j)->setSelected(false);
    }
}

/*================================================================*/
QCursor KWordDocument::getMouseCursor(unsigned int mx,unsigned int my)
{
  KWFrameSet *frameSet = 0L;

  for (unsigned int i = 0;i < getNumFrameSets();i++)
    {
      frameSet = getFrameSet(getNumFrameSets() - 1 - i);
      if (frameSet->contains(mx,my))
	{
	  if (frameSet->getFrameType() == FT_TEXT && 
	      ((dynamic_cast<KWTextFrameSet*>(frameSet)->getFrameInfo() == FI_HEADER && !hasHeader()) ||
	       (dynamic_cast<KWTextFrameSet*>(frameSet)->getFrameInfo() == FI_FOOTER && !hasFooter())))
	    continue;
	  return frameSet->getMouseCursor(mx,my);
	}
    }

  return arrowCursor;
}

/*================================================================*/
KWFrame *KWordDocument::getFirstSelectedFrame()
{
  KWFrameSet *frameSet = 0L;

  for (unsigned int i = 0;i < getNumFrameSets();i++)
    {
      frameSet = getFrameSet(getNumFrameSets() - 1 - i);
      for (unsigned int j = 0;j < frameSet->getNumFrames();j++)
	{	
	  if (frameSet->getFrameType() == FT_TEXT && 
	      ((dynamic_cast<KWTextFrameSet*>(frameSet)->getFrameInfo() == FI_HEADER && !hasHeader()) ||
	       (dynamic_cast<KWTextFrameSet*>(frameSet)->getFrameInfo() == FI_FOOTER && !hasFooter())))
	    continue;
	  if (frameSet->getFrame(j)->isSelected())
	    return frameSet->getFrame(j);
	}
    }

  return 0L;
}

/*================================================================*/
KWFrame *KWordDocument::getFirstSelectedFrame(int &_frameset)
{
  KWFrameSet *frameSet = 0L;
  _frameset = 0;

  for (unsigned int i = 0;i < getNumFrameSets();i++)
    {
      _frameset = getNumFrameSets() - 1 - i;
      frameSet = getFrameSet(getNumFrameSets() - 1 - i);
      for (unsigned int j = 0;j < frameSet->getNumFrames();j++)
	{	
	  if (frameSet->getFrameType() == FT_TEXT && 
	      ((dynamic_cast<KWTextFrameSet*>(frameSet)->getFrameInfo() == FI_HEADER && !hasHeader()) ||
	       (dynamic_cast<KWTextFrameSet*>(frameSet)->getFrameInfo() == FI_FOOTER && !hasFooter())))
	    continue;
	  if (frameSet->getFrame(j)->isSelected())
	    return frameSet->getFrame(j);
	}
    }

  return 0L;
}

/*================================================================*/
void KWordDocument::print(QPainter *painter,QPrinter *printer,float left_margin,float top_margin)
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
      KRect pageRect(0,i * getPTPaperHeight(),getPTPaperWidth(),getPTPaperHeight());
      unsigned int minus = 0;
      if (i + 1> static_cast<unsigned int>(printer->fromPage())) printer->newPage();
      for (j = 0;j < frames.count();j++)
	{
	  switch (frames.at(j)->getFrameType())
	    {
	    case FT_PICTURE:
	      {
		minus++;

		KWPictureFrameSet *picFS = dynamic_cast<KWPictureFrameSet*>(frames.at(j));
		KWFrame *frame = picFS->getFrame(0);
		if (!frame->intersects(pageRect)) break;

		KSize _size = QSize(frame->width(),frame->height());
		if (_size != picFS->getImage()->size())
		  picFS->setSize(_size);

		painter->drawImage(frame->x(),frame->y() - i * getPTPaperHeight(),*picFS->getImage());
	      } break;
	    case FT_PART:
	      {
		minus++;

		KWPartFrameSet *partFS = dynamic_cast<KWPartFrameSet*>(getFrameSet(j));
		KWFrame *frame = partFS->getFrame(0);

		QPicture *pic = partFS->getPicture(); 
	    
		painter->save();
		KRect r = painter->viewport();
		painter->setViewport(frame->x(),frame->y() - i * getPTPaperHeight(),r.width(),r.height());
		painter->drawPicture(*pic);
		painter->setViewport(r);
		painter->restore();
	      } break;
	    case FT_TEXT:
	      {
		bool bend = false;
		fc = fcList.at(j - minus);
		while (fc->getPage() == i + 1 && !bend)
		  {
		    if (i + 1 >= static_cast<unsigned int>(printer->fromPage()) && i + 1 <= static_cast<unsigned int>(printer->toPage()))
		      printLine(*fc,*painter,0,i * getPTPaperHeight(),getPTPaperWidth(),getPTPaperHeight());
		    bend = !fc->makeNextLineLayout(*painter);
		  }
	      } break;
	    default: minus++; break;
	    }
	}
    }
}

/*================================================================*/
void KWordDocument::updateAllFrames()
{
  for (unsigned int i = 0;i < getNumFrameSets();i++)
    {
      getFrameSet(i)->update();
    }

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
	  if (frame1->intersects(KRect(frame2->x(),frame2->y(),frame2->width(),frame2->height())))
	    {
	      KRect r = QRect(frame2->x(),frame2->y(),frame2->width(),frame2->height());
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

/*================================================================*/
void KWordDocument::recalcWholeText()
{
  KWordView *viewPtr;

  if (!m_lstViews.isEmpty())
    {
      viewPtr = m_lstViews.first();
      viewPtr->getGUI()->getPaperWidget()->recalcWholeText();
    }
}

/*================================================================*/
void KWordDocument::addStyleTemplate(KWParagLayout *pl)
{
  KWParagLayout* p;
  for (p = paragLayoutList.first();p != 0L;p = paragLayoutList.next())
    {    
      if (p->getName() == pl->getName())
	{
	  *p = *pl;
	  if (p->getName() == "Standard") defaultParagLayout = p;
	  delete pl;
	  return;
	}
    }
  paragLayoutList.append(pl);
}
/*================================================================*/
void KWordDocument::setStyleChanged(QString _name)
{
  changedStyles.append(_name);
}

/*================================================================*/
bool KWordDocument::isStyleChanged(QString _name)
{
  return (changedStyles.find(_name) != -1);
}

/*================================================================*/
void KWordDocument::hideAllFrames()
{
  KWordView *viewPtr = 0L;

  if (!m_lstViews.isEmpty())
    {
      for (viewPtr = m_lstViews.first();viewPtr != 0;viewPtr = m_lstViews.next())
	viewPtr->hideAllFrames();
    }
}
