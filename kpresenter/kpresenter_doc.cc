/******************************************************************/
/* KPresenter - (c) by Reginald Stadlbauer 1998                   */
/* Version: 0.0.1                                                 */
/* Author: Reginald Stadlbauer                                    */
/* E-Mail: reggie@kde.org                                         */
/* needs c++ library Qt (http://www.troll.no)                     */
/* needs mico (http://diamant.vsb.cs.uni-frankfurt.de/~mico/)     */
/* needs OpenParts and Kom (weis@kde.org)                         */
/* written for KDE (http://www.kde.org)                           */
/* KPresenter is under GNU GPL                                    */
/******************************************************************/
/* Module: KPresenter Document                                    */
/******************************************************************/

#include "kpresenter_doc.h"
#include "kpresenter_doc.moc"

/******************************************************************/
/* class BackPic                                                  */
/******************************************************************/

/*======================= constructor ============================*/
BackPic::BackPic(QWidget* parent=0,const char* name=0)
  : QWidget(parent,name)
{
  pic = 0;
  pic = new QPicture;
}

/*======================= destructor =============================*/
BackPic::~BackPic()
{
  delete pic;
}

/*==================== set clipart ===============================*/
void BackPic::setClipart(const char* fn)
{
  fileName = qstrdup(fn);
  wmf.load(fileName);
  wmf.paint(pic);
  repaint();
}

/*======================= get pic ================================*/
QPicture* BackPic::getPic()
{
  return pic;
}

/******************************************************************/
/* class KPresenterChild                                          */
/******************************************************************/

/*====================== constructor =============================*/
KPresenterChild::KPresenterChild(KPresenterDocument_impl *_kpr, const QRect& _rect,OPParts::Document_ptr _doc)
  : KoDocumentChild( _rect, _doc )
{
  m_pKPresenterDoc = _kpr;
  m_rDoc = OPParts::Document::_duplicate(_doc);
  m_geometry = _rect;
}

/*====================== constructor =============================*/
KPresenterChild::KPresenterChild( KPresenterDocument_impl *_kpr ) :
  KoDocumentChild()
{
  m_pKPresenterDoc = _kpr;
}

/*====================== destructor ==============================*/
KPresenterChild::~KPresenterChild()
{
  m_rDoc = 0L;
}

/******************************************************************/
/* class KPresenterDocument_impl                                  */
/******************************************************************/

/*====================== constructor =============================*/
KPresenterDocument_impl::KPresenterDocument_impl()
{
  // Use CORBA mechanism for deleting views
  m_lstViews.setAutoDelete(false);
  m_lstChildren.setAutoDelete(true);

  m_bModified = false;

  // init
  _pageList.setAutoDelete(true);
  _objList.setAutoDelete(true);
  _objNums = 0;
  _pageNums = 0;
  _spInfinitLoop = false;
  _spManualSwitch = true;
  _rastX = 10;
  _rastY = 10;
  _xRnd = 20;
  _yRnd = 20;
  _txtBackCol.operator=(white);
  _txtSelCol.operator=(lightGray);
  _pageLayout.format = PG_SCREEN;
  _pageLayout.orientation = PG_PORTRAIT;
  _pageLayout.width = PG_SCREEN_WIDTH;
  _pageLayout.height = PG_SCREEN_HEIGHT;
  _pageLayout.left = 0;
  _pageLayout.right = 0;
  _pageLayout.top = 0;
  _pageLayout.bottom = 0;
  _pageLayout.unit = PG_MM;
  setPageLayout(_pageLayout,0,0);
}

/*====================== constructor =============================*/
KPresenterDocument_impl::KPresenterDocument_impl(const CORBA::BOA::ReferenceData &_refdata)
  : KPresenter::KPresenterDocument_skel(_refdata)
{
  // Use CORBA mechanism for deleting views
  m_lstViews.setAutoDelete(false);
  m_lstChildren.setAutoDelete(true);

  m_bModified = false;

  // init
  _pageList.setAutoDelete(true);
  _objList.setAutoDelete(true);
  _objNums = 0;
  _pageNums = 0;
  _spInfinitLoop = false;
  _spManualSwitch = true;
  _rastX = 20;
  _rastY = 20;
  _xRnd = 20;
  _yRnd = 20;
  _txtBackCol.operator=(white);
  _txtSelCol.operator=(lightGray);
  _pageLayout.format = PG_SCREEN;
  _pageLayout.orientation = PG_PORTRAIT;
  _pageLayout.width = PG_SCREEN_WIDTH;
  _pageLayout.height = PG_SCREEN_HEIGHT;
  _pageLayout.left = 0;
  _pageLayout.right = 0;
  _pageLayout.top = 0;
  _pageLayout.bottom = 0;
  _pageLayout.unit = PG_MM;
  setPageLayout(_pageLayout,0,0);
  insertNewPage(0,0);
}

/*====================== destructor ==============================*/
KPresenterDocument_impl::~KPresenterDocument_impl()
{
  sdeb("KPresenterDocument_impl::~KPresenterDocument_impl()\n");
  cleanUp();
  edeb("...KPresenterDocument_impl::~KPresenterDocument_impl() %i\n",_refcnt());
}

/*======================= clean up ===============================*/
void KPresenterDocument_impl::cleanUp()
{
  if (m_bIsClean) return;

  assert(m_lstViews.count() == 0);
  
  m_lstChildren.clear();

  Document_impl::cleanUp();
}

/*========================== save ===============================*/
bool KPresenterDocument_impl::hasToWriteMultipart()
{  
  QListIterator<KPresenterChild> it( m_lstChildren );
  for( ; it.current(); ++it )
  {
    if ( !it.current()->isStoredExtern() )
      return true;    
  }
  return false;
}

void KPresenterDocument_impl::makeChildListIntern( OPParts::Document_ptr _doc, const char *_path )
{
  int i = 0;
  
  QListIterator<KPresenterChild> it( m_lstChildren );
  for( ; it.current(); ++it )
  {
    QString tmp;
    tmp.sprintf("/%i", i++ );
    QString path( _path );
    path += tmp.data();
    
    OPParts::Document_var doc = it.current()->document();    
    doc->makeChildList( _doc, path );
  }
}

/*
bool KPresenterDocument_impl::save(const char *_url)
{
  KURL u(_url);
  if (u.isMalformed())
    {
      cerr << "malformed URL" << endl;
      return false;
    }
  
  if (!u.isLocalFile())
    {
      QMessageBox::critical((QWidget*)0L,i18n("KPresenter Error"),i18n("Can not save to remote URL\n"),i18n("OK"));
      return false;
    }

  ofstream out(u.path());
  if (!out)
    {
      QString tmp;
      tmp.sprintf(i18n("Could not write to\n%s"),u.path());
      cerr << tmp << endl;
      return false;
    }

  out << "<?xml version=\"1.0\"?>" << endl;
  
  save(out);
  
  m_strFileURL = _url;
    
  return true;
}
*/

/*========================== save ===============================*/
bool KPresenterDocument_impl::save(ostream& out)
{
  out << otag << "<DOC author=\"" << "Reginald Stadlbauer" << "\" email=\"" << "reggie@kde.org" << "\" editor=\"" << "KPresenter"
      << "\" mime=\"" << "application/x-kpresenter" << "\">" << endl;
  
  out << otag << "<PAPER format=\"" << pageLayout().format << "\" width=\"" << pageLayout().width
      << "\" height=\"" << pageLayout().height << "\" orientation=\"" << pageLayout().orientation << "\">" << endl;
  out << indent << "<PAPERBORDERS left=\"" << pageLayout().left << "\" top=\"" << pageLayout().top << "\" right=\"" << pageLayout().right
      << "\" bottom=\"" << pageLayout().bottom << "\"/>" << endl;
  out << etag << "</PAPER>" << endl;
  
  out << otag << "<BACKGROUND pages=\"" << _pageList.count() << "\" rastX=\"" << _rastX << "\" rastY=\""
      << _rastY << "\" xRnd=\"" << _xRnd << "\" yRnd=\"" << _yRnd << "\" bred=\"" << _txtBackCol.red() << "\" bgreen=\""
      << _txtBackCol.green() << "\" bblue=\"" << _txtBackCol.blue() << "\" sred=\"" << _txtSelCol.red() << "\" sgreen=\""
      << _txtSelCol.green() << "\" sblue=\"" << _txtSelCol.blue() << "\">" << endl;
  saveBackground(out);
  out << etag << "</BACKGROUND>" << endl;

  out << otag << "<OBJECTS objects=\"" << _objList.count() << "\">" << endl;
  saveObjects(out);
  out << etag << "</OBJECTS>" << endl;

  out << indent << "<INFINITLOOP value=\"" << _spInfinitLoop << "\"/>" << endl; 
  out << indent << "<MANUALSWITCH value=\"" << _spManualSwitch << "\"/>" << endl; 

  // Write "OBJECT" tag for every child
  QListIterator<KPresenterChild> chl( m_lstChildren );
  for( ; chl.current(); ++chl )
    chl.current()->save( out );

  out << etag << "</DOC>" << endl;
    
  setModified(false);
    
  return true;
}

/*========================== save background ====================*/
void KPresenterDocument_impl::saveBackground(ostream& out)
{
  for (pagePtr = _pageList.first();pagePtr != 0;pagePtr = _pageList.next())
    {
      out << otag << "<PAGE>" << endl;
      out << indent << "<BACKTYPE value=\"" << pagePtr->backType << "\"/>" << endl; 
      out << indent << "<BACKVIEW value=\"" << pagePtr->backPicView << "\"/>" << endl; 
      out << indent << "<BACKCOLOR1 red=\"" << pagePtr->backColor1.red() << "\" green=\"" 
	  << pagePtr->backColor1.green() << "\" blue=\"" << pagePtr->backColor1.blue() << "\"/>" << endl; 
      out << indent << "<BACKCOLOR2 red=\"" << pagePtr->backColor2.red() << "\" green=\"" 
	  << pagePtr->backColor2.green() << "\" blue=\"" << pagePtr->backColor2.blue() << "\"/>" << endl; 
      out << indent << "<BCTYPE value=\"" << pagePtr->bcType << "\"/>" << endl; 
      if (pagePtr->backPic)
	out << indent << "<BACKPIC value=\"" << pagePtr->backPic << "\"/>" << endl;

      if (pagePtr->backClip)
	out << indent << "<BACKCLIP value=\"" << pagePtr->backClip << "\"/>" << endl; 

      out << otag << "<TIMEPARTS>" << endl;
      for (unsigned int i = 0;i < pagePtr->timeParts.count();i++)
	  out << indent << "<PART time=\"" << (int)pagePtr->timeParts.at(i) << "\"/>" << endl;
      out << etag << "</TIMEPARTS>" << endl;
      out << etag << "</PAGE>" << endl;
    }
}

/*========================== save objects =======================*/
void KPresenterDocument_impl::saveObjects(ostream& out)
{
  for (objPtr = _objList.first();objPtr != 0;objPtr = _objList.next())
    {
      out << otag << "<OBJECT>" << endl;
      out << indent << "<OBJTYPE value=\"" << objPtr->objType << "\"/>" << endl; 
      out << indent << "<ISSELECTED value=\"" << objPtr->isSelected << "\"/>" << endl; 
      out << indent << "<OBJNUM value=\"" << objPtr->objNum << "\"/>" << endl; 
      out << indent << "<COORDINATES x=\"" << objPtr->ox << "\" y=\"" << objPtr->oy
	  << "\" w=\"" << objPtr->ow << "\" h=\"" << objPtr->oh << "\"/>" << endl; 
      out << indent << "<PRESNUM value=\"" << objPtr->presNum << "\"/>" << endl; 
      out << indent << "<EFFECT value=\"" << objPtr->effect << "\"/>" << endl; 
      
      if (objPtr->objType == OT_TEXT)
	saveTxtObj(out,objPtr->textObj);
      else
	{
	  out << otag << "<GRAPHOBJ>" << endl;
	  objPtr->graphObj->save(out);
	  out << etag << "</GRAPHOBJ>" << endl;
	}
 
      out << etag << "</OBJECT>" << endl;
    }
}

/*========================== save textobject ====================*/
void KPresenterDocument_impl::saveTxtObj(ostream& out,KTextObject *txtPtr)
{
  TxtObj *txtObj;
  TxtLine *txtLine;
  TxtParagraph *txtParagraph;
  unsigned int i,j,k;
  QFont font;

  out << otag << "<TEXTOBJ objType=\"" << txtPtr->objType() << "\">" << endl;
  out << indent << "<ENUMLISTTYPE type=\"" << txtPtr->enumListType().type << "\" before=\""
      << txtPtr->enumListType().before << "\" after=\"" << txtPtr->enumListType().after
      << "\" start=\"" << txtPtr->enumListType().start << "\" family=\"" 
      << txtPtr->enumListType().font.family() << "\" pointSize=\"" << txtPtr->enumListType().font.pointSize()
      << "\" bold=\"" << txtPtr->enumListType().font.bold() << "\" italic=\"" << txtPtr->enumListType().font.italic()
      << "\" underline=\"" << txtPtr->enumListType().font.underline() << "\" red=\"" 
      << txtPtr->enumListType().color.red() << "\" green=\"" << txtPtr->enumListType().color.green() 
      << "\" blue=\"" << txtPtr->enumListType().color.blue() << "\"/>" << endl; 
  out << indent << "<UNSORTEDLISTTYPE type=\"" << txtPtr->enumListType().type << "\" family=\"" 
      << txtPtr->unsortListType().font.family() << "\" pointSize=\"" << txtPtr->unsortListType().font.pointSize()
      << "\" bold=\"" << txtPtr->unsortListType().font.bold() << "\" italic=\"" << txtPtr->unsortListType().font.italic()
      << "\" underline=" << txtPtr->unsortListType().font.underline() << " red=\"" 
      << txtPtr->unsortListType().color.red() << "\" green=\"" << txtPtr->unsortListType().color.green() 
      << "\" blue=\"" << txtPtr->unsortListType().color.blue() << "\" chr=\"" << txtPtr->unsortListType().chr
      << "\"/>" << endl; 

  for (i = 0;i < txtPtr->paragraphs();i++)
    {
      txtParagraph = txtPtr->paragraphAt(i);

      out << otag << "<PARAGRAPH horzAlign=\"" << txtParagraph->horzAlign() << "\">" << endl; 

      for (j = 0;j < txtParagraph->lines();j++)
	{
	  txtLine = txtParagraph->lineAt(j);

	  out << otag << "<LINE>" << endl;

	  for (k = 0;k < txtLine->items();k++)
	    {
	      txtObj = txtLine->itemAt(k);
	      font = txtObj->font();
	      
	      out << otag << "<OBJ>" << endl;
	      out << indent << "<TYPE value=\"" << txtObj->type() << "\"/>" << endl;
	      out << indent << "<FONT family=\"" << font.family() << "\" pointSize=\""
		  << font.pointSize() << "\" bold=\"" << font.bold() << "\" italic=\"" << font.italic()
		  << "\" underline=\"" << font.underline() << "\"/>" << endl;
	      out << indent << "<COLOR red=\"" << txtObj->color().red() << "\" green=\""
		  << txtObj->color().green() << "\" blue=\"" << txtObj->color().blue() << "\"/>" << endl;
	      out << indent << "<VERTALIGN value=\"" << txtObj->vertAlign() << "\"/>" << endl;
	      out << indent << "<TEXT value=\"" << txtObj->text() << "\"/>" << endl;
	      out << etag << "</OBJ>" << endl;
	    }

	  out << etag << "</LINE>" << endl;

	}
      
      out << etag << "</PARAGRAPH>" << endl;

    }

  out << etag << "</TEXTOBJ>" << endl;
}

/*========================== load ===============================*/
bool KPresenterDocument_impl::loadChildren( OPParts::MimeMultipartDict_ptr _dict )
{
  QListIterator<KPresenterChild> it( m_lstChildren );
  for( ; it.current(); ++it )
  {
    if ( !it.current()->loadDocument( _dict ) )
      return false;
  }

  return true;
}

/*
bool KPresenterDocument_impl::load(const char *_url)
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

  KOMLStreamFeed feed( in );
  KOMLParser parser(&feed);
  
  string tag;
  vector<KOMLAttrib> lst;
  string name;
  
  // DOC
  if (!parser.open("DOC",tag))
    {
      cerr << "Missing DOC" << endl;
      return false;
    }
  
  KOMLParser::parseTag(tag.c_str(),name,lst);
  vector<KOMLAttrib>::const_iterator it = lst.begin();
  for (;it != lst.end();it++)
    {
      if ((*it).m_strName == "mime")
	{
	  if ((*it).m_strValue != "application/x-kpresenter")
	    {
	      cerr << "Unknown mime type" << (*it).m_strValue << endl;
	      return false;
	    }
	}
    }
  
  if (!load(parser))
    return false;
  
  parser.close(tag);
  
  m_strFileURL = _url;
  
  return true;
}
*/

/*========================== load ===============================*/
bool KPresenterDocument_impl::load(KOMLParser& parser)
{
  string tag;
  vector<KOMLAttrib> lst;
  string name;

  KoPageLayout __pgLayout;
  
  // clean
  if (!_pageList.isEmpty())
    _pageList.clear();
  if (!_objList.isEmpty())
    _objList.clear();
  _objNums = 0;
  _pageNums = 0;
  _spInfinitLoop = false;
  _spManualSwitch = true;
  _rastX = 20;
  _rastY = 20;
  _xRnd = 20;
  _yRnd = 20;
  _txtBackCol.operator=(white);
  _txtSelCol.operator=(lightGray);
      
  // DOC
  if ( !parser.open( "DOC", tag ) )
  {
    cerr << "Missing DOC" << endl;
    return false;
  }
  
  KOMLParser::parseTag( tag.c_str(), name, lst );
  vector<KOMLAttrib>::const_iterator it = lst.begin();
  for( ; it != lst.end(); it++ )
  {
    if ( (*it).m_strName == "mime" )
    {
      if ( (*it).m_strValue != "application/x-kpresenter" )
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
      
      if ( name == "OBJECT" )
	{
	  KPresenterChild *ch = new KPresenterChild( this );
	  ch->load( parser, lst );
	  insertChild( ch );
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
		__pgLayout.width = (double)atof((*it).m_strValue.c_str());
	      else if ((*it).m_strName == "height")
		__pgLayout.height = (double)atof((*it).m_strValue.c_str());
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
      
      else if (name == "BACKGROUND")
	{
	  KOMLParser::parseTag(tag.c_str(),name,lst);
	  vector<KOMLAttrib>::const_iterator it = lst.begin();
	  for(;it != lst.end();it++)
	    {
	      if ((*it).m_strName == "rastX")
		_rastX = atoi((*it).m_strValue.c_str());
	      else if ((*it).m_strName == "rastY")
		_rastY = atoi((*it).m_strValue.c_str());
	      else if ((*it).m_strName == "xRnd")
		_xRnd = atoi((*it).m_strValue.c_str());
	      else if ((*it).m_strName == "yRnd")
		_yRnd = atoi((*it).m_strValue.c_str());
	      else if ((*it).m_strName == "bred")
		_txtBackCol.setRgb(atoi((*it).m_strValue.c_str()),
				   _txtBackCol.green(),_txtBackCol.blue());
	      else if ((*it).m_strName == "bgreen")
		_txtBackCol.setRgb(_txtBackCol.red(),atoi((*it).m_strValue.c_str()),
				   _txtBackCol.blue());
	      else if ((*it).m_strName == "bblue")
		_txtBackCol.setRgb(_txtBackCol.red(),_txtBackCol.green(),
				   atoi((*it).m_strValue.c_str()));
	      else if ((*it).m_strName == "sred")
		_txtSelCol.setRgb(atoi((*it).m_strValue.c_str()),
				   _txtSelCol.green(),_txtSelCol.blue());
	      else if ((*it).m_strName == "sgreen")
		_txtSelCol.setRgb(_txtSelCol.red(),atoi((*it).m_strValue.c_str()),
				   _txtSelCol.blue());
	      else if ((*it).m_strName == "sblue")
		_txtSelCol.setRgb(_txtSelCol.red(),_txtSelCol.green(),
				   atoi((*it).m_strValue.c_str()));
	      else
		cerr << "Unknown attrib BACKGROUND:'" << (*it).m_strName << "'" << endl;
	    }
	  loadBackground(parser,lst);
	}

      else if (name == "OBJECTS")
	{
	  KOMLParser::parseTag(tag.c_str(),name,lst);
	  vector<KOMLAttrib>::const_iterator it = lst.begin();
	  for(;it != lst.end();it++)
	    {
	    }
	  loadObjects(parser,lst);
	}

      else if (name == "INFINITLOOP")
	{
	  KOMLParser::parseTag(tag.c_str(),name,lst);
	  vector<KOMLAttrib>::const_iterator it = lst.begin();
	  for(;it != lst.end();it++)
	    {
	      if ((*it).m_strName == "value")
		_spInfinitLoop = (bool)atoi((*it).m_strValue.c_str());
	    }
	}

      else if (name == "MANUALSWITCH")
	{
	  KOMLParser::parseTag(tag.c_str(),name,lst);
	  vector<KOMLAttrib>::const_iterator it = lst.begin();
	  for(;it != lst.end();it++)
	    {
	      if ((*it).m_strName == "value")
		_spManualSwitch = (bool)atoi((*it).m_strValue.c_str());
	    }
	}

      else
	cerr << "Unknown tag '" << tag << "' in PRESENTATION" << endl;    
	
      if (!parser.close(tag))
	{
	  cerr << "ERR: Closing Child" << endl;
	  return false;
	}
    }

  
  setPageLayout(__pgLayout,0,0);
  //repaint(true);
  return true;
}

/*====================== load background =========================*/
void KPresenterDocument_impl::loadBackground(KOMLParser& parser,vector<KOMLAttrib>& lst)
{
  string tag;
  string name;

  while (parser.open(0L,tag))
    {
      KOMLParser::parseTag(tag.c_str(),name,lst);
      
      // page
      if (name == "PAGE")
	{    
	  insertNewPage(0,0);

	  unsigned int _num = _pageList.count();
	  pagePtr = _pageList.last();

	  while (parser.open(0L,tag))
	    {
	      KOMLParser::parseTag(tag.c_str(),name,lst);
	      
	      // time parts
	      if (name == "TIMEPARTS")
		{
		  pagePtr = _pageList.last();
		  pagePtr->timeParts.clear();
		  while (parser.open(0L,tag))
		    {
		      KOMLParser::parseTag(tag.c_str(),name,lst);
		      
		      // part
		      if (name == "PART")
			{
			  pagePtr = _pageList.last();
			  KOMLParser::parseTag(tag.c_str(),name,lst);
			  vector<KOMLAttrib>::const_iterator it = lst.begin();
			  for(;it != lst.end();it++)
			    {
			      if ((*it).m_strName == "time")
				pagePtr->timeParts.append((int*)atoi((*it).m_strValue.c_str()));
			    }
			}

		      if (!parser.close(tag))
			{
			  cerr << "ERR: Closing Child" << endl;
			  return;
			}
		    }
		}
 
	      // backtype
	      else if (name == "BACKTYPE")
		{
		  pagePtr = _pageList.last();
		  KOMLParser::parseTag(tag.c_str(),name,lst);
		  vector<KOMLAttrib>::const_iterator it = lst.begin();
		  for(;it != lst.end();it++)
		    {
		      if ((*it).m_strName == "value")
			setBackType(_num,(BackType)atoi((*it).m_strValue.c_str()));
		    }
		}
	      
	      // backview
	      else if (name == "BACKVIEW")
		{
		  pagePtr = _pageList.last();
		  KOMLParser::parseTag(tag.c_str(),name,lst);
		  vector<KOMLAttrib>::const_iterator it = lst.begin();
		  for(;it != lst.end();it++)
		    {
		      if ((*it).m_strName == "value")
			setBPicView(_num,(BackView)atoi((*it).m_strValue.c_str()));
		    }
		}
	      
	      // backcolor 1
	      else if (name == "BACKCOLOR1")
		{
		  pagePtr = _pageList.last();
		  KOMLParser::parseTag(tag.c_str(),name,lst);
		  vector<KOMLAttrib>::const_iterator it = lst.begin();
		  for(;it != lst.end();it++)
		    {
		      pagePtr = _pageList.last();
		      if ((*it).m_strName == "red")
			setBackColor(_num,QColor(atoi((*it).m_strValue.c_str()),
						 pagePtr->backColor1.green(),pagePtr->backColor1.blue()),
				     pagePtr->backColor2,pagePtr->bcType);
		      if ((*it).m_strName == "green")
			setBackColor(_num,QColor(pagePtr->backColor1.red(),
						 atoi((*it).m_strValue.c_str()),pagePtr->backColor1.blue()),
				     pagePtr->backColor2,pagePtr->bcType);
		      
		      if ((*it).m_strName == "blue")
			setBackColor(_num,QColor(pagePtr->backColor1.red(),pagePtr->backColor1.green(),
						 atoi((*it).m_strValue.c_str())),
				     pagePtr->backColor2,pagePtr->bcType);
		    }
		}
	      
	      // backcolor 2
	      else if (name == "BACKCOLOR2")
		{
		  pagePtr = _pageList.last();
		  KOMLParser::parseTag(tag.c_str(),name,lst);
		  vector<KOMLAttrib>::const_iterator it = lst.begin();
		  for(;it != lst.end();it++)
		    {
		      pagePtr = _pageList.last();
		      if ((*it).m_strName == "red")
			setBackColor(_num,pagePtr->backColor1,QColor(atoi((*it).m_strValue.c_str()),
								     pagePtr->backColor2.green(),pagePtr->backColor2.blue()),
				     pagePtr->bcType);
		      if ((*it).m_strName == "green")
			setBackColor(_num,pagePtr->backColor1,QColor(pagePtr->backColor2.red(),
								     atoi((*it).m_strValue.c_str()),pagePtr->backColor2.blue()),
				     pagePtr->bcType);
		      
		      if ((*it).m_strName == "blue")
			setBackColor(_num,pagePtr->backColor1,QColor(pagePtr->backColor2.red(),pagePtr->backColor2.green(),
								     atoi((*it).m_strValue.c_str())),
				     pagePtr->bcType);
		    }
		}
	      
	      // backColorType
	      else if (name == "BCTYPE")
		{
		  pagePtr = _pageList.last();
		  KOMLParser::parseTag(tag.c_str(),name,lst);
		  vector<KOMLAttrib>::const_iterator it = lst.begin();
		  for(;it != lst.end();it++)
		    {
		      if ((*it).m_strName == "value")
			setBackColor(_num,pagePtr->backColor1,pagePtr->backColor2,
				     (BCType)atoi((*it).m_strValue.c_str()));
		    }
		}
	      
	      // backpic
	      else if (name == "BACKPIC")
		{
		  pagePtr = _pageList.last();
		  KOMLParser::parseTag(tag.c_str(),name,lst);
		  vector<KOMLAttrib>::const_iterator it = lst.begin();
		  for(;it != lst.end();it++)
		    {
		      if ((*it).m_strName == "value")
			{
			  QString _fileName = (*it).m_strValue.c_str();
			  if (!_fileName.isEmpty())
			    {
			      if (int _envVarB = _fileName.find('$') >= 0)
				{
				  int _envVarE = _fileName.find('/',_envVarB);
				  QString path = (const char*)getenv((const char*)_fileName.mid(_envVarB,_envVarE-_envVarB));
				  _fileName.replace(_envVarB-1,_envVarE-_envVarB+1,path);
				}
			    }
			  setBackPic(_num,(const char*)_fileName);
			}
		    }
		}
	      
	      // backclip
	      else if (name == "BACKCLIP")
		{
		  pagePtr = _pageList.last();
		  KOMLParser::parseTag(tag.c_str(),name,lst);
		  vector<KOMLAttrib>::const_iterator it = lst.begin();
		  for(;it != lst.end();it++)
		    {
		      if ((*it).m_strName == "value")
			{
			  QString _fileName = (*it).m_strValue.c_str();
			  if (!_fileName.isEmpty())
			    {
			      if (int _envVarB = _fileName.find('$') >= 0)
				{
				  int _envVarE = _fileName.find('/',_envVarB);
				  QString path = (const char*)getenv((const char*)_fileName.mid(_envVarB,_envVarE-_envVarB));
				  _fileName.replace(_envVarB-1,_envVarE-_envVarB+1,path);
				}
			    }
			  setBackClip(_num,(const char*)_fileName);
			}
		    }
		}

	      else
		cerr << "Unknown tag '" << tag << "' in PAGE" << endl;    
	      
	      if (!parser.close(tag))
		{
		  cerr << "ERR: Closing Child" << endl;
		  return;
		}
	    }
	}
      else
	cerr << "Unknown tag '" << tag << "' in BACKGROUND" << endl;    
      
      if (!parser.close(tag))
	{
	  cerr << "ERR: Closing Child" << endl;
	  return;
	}
    }
}

/*========================= load objects =========================*/
void KPresenterDocument_impl::loadObjects(KOMLParser& parser,vector<KOMLAttrib>& lst)
{
  string tag;
  string name;

  while (parser.open(0L,tag))
    {
      KOMLParser::parseTag(tag.c_str(),name,lst);
      
      // object
      if (name == "OBJECT")
	{    

	  _objNums++;
	  objPtr = new PageObjects;
	  objPtr->isSelected = false;
	  objPtr->objType = OT_LINE;
	  objPtr->objNum = _objNums;
	  _objList.append(objPtr);

	  while (parser.open(0L,tag))
	    {
	      KOMLParser::parseTag(tag.c_str(),name,lst);
	      
	      // objType
	      if (name == "OBJTYPE")
		{
		  KOMLParser::parseTag(tag.c_str(),name,lst);
		  vector<KOMLAttrib>::const_iterator it = lst.begin();
		  for(;it != lst.end();it++)
		    {
		      if ((*it).m_strName == "value")
			objPtr->objType = (ObjType)atoi((*it).m_strValue.c_str());
		    }
		}

	      // isSelected
	      else if (name == "ISSELECTED")
		{
		  KOMLParser::parseTag(tag.c_str(),name,lst);
		  vector<KOMLAttrib>::const_iterator it = lst.begin();
		  for(;it != lst.end();it++)
		    {
		      if ((*it).m_strName == "value")
			objPtr->isSelected = (bool)atoi((*it).m_strValue.c_str());
		    }
		}
	      
	      // objNum
	      else if (name == "OBJNUM")
		{
		  KOMLParser::parseTag(tag.c_str(),name,lst);
		  vector<KOMLAttrib>::const_iterator it = lst.begin();
		  for(;it != lst.end();it++)
		    {
		      if ((*it).m_strName == "value")
			objPtr->objNum = atoi((*it).m_strValue.c_str());
		    }
		}

	      // presNum
	      else if (name == "PRESNUM")
		{
		  KOMLParser::parseTag(tag.c_str(),name,lst);
		  vector<KOMLAttrib>::const_iterator it = lst.begin();
		  for(;it != lst.end();it++)
		    {
		      if ((*it).m_strName == "value")
			objPtr->presNum = atoi((*it).m_strValue.c_str());
		    }
		}

	      // effect
	      else if (name == "EFFECT")
		{
		  KOMLParser::parseTag(tag.c_str(),name,lst);
		  vector<KOMLAttrib>::const_iterator it = lst.begin();
		  for(;it != lst.end();it++)
		    {
		      if ((*it).m_strName == "value")
			objPtr->effect = (Effect)atoi((*it).m_strValue.c_str());
		    }
		}

	      // coordinates
	      else if (name == "COORDINATES")
		{
		  KOMLParser::parseTag(tag.c_str(),name,lst);
		  vector<KOMLAttrib>::const_iterator it = lst.begin();
		  for(;it != lst.end();it++)
		    {
		      if ((*it).m_strName == "x")
			objPtr->ox = atoi((*it).m_strValue.c_str());
		      if ((*it).m_strName == "y")
			objPtr->oy = atoi((*it).m_strValue.c_str());
		      if ((*it).m_strName == "w")
			objPtr->ow = atoi((*it).m_strValue.c_str());
		      if ((*it).m_strName == "h")
			objPtr->oh = atoi((*it).m_strValue.c_str());
		    }
		}
	      
	      // graphic object
 	      else if (name == "GRAPHOBJ")
 		{
 		  objPtr->graphObj = new GraphObj(0,"graphObj",objPtr->objType);
 		  objPtr->graphObj->load(parser,lst);
 		  objPtr->graphObj->resize(objPtr->ow,objPtr->oh);
		  if (objPtr->objType == OT_PICTURE)
		    objPtr->graphObj->loadPixmap();
		  if (objPtr->objType == OT_CLIPART)
		    objPtr->graphObj->loadClipart();
 		}

	      // text object
 	      else if (name == "TEXTOBJ")
 		{
 		  objPtr->textObj = new KTextObject(0,"textObj",KTextObject::PLAIN);
		  objPtr->textObj->clear();
		  KOMLParser::parseTag(tag.c_str(),name,lst);
		  vector<KOMLAttrib>::const_iterator it = lst.begin();
		  for(;it != lst.end();it++)
		    {
		      if ((*it).m_strName == "objType")
			objPtr->textObj->setObjType((KTextObject::ObjType)atoi((*it).m_strValue.c_str()));
		      else
			cerr << "Unknown attrib TEXTOBJ:'" << (*it).m_strName << "'" << endl;
		    }
 		  loadTxtObj(parser,lst,objPtr->textObj);
 		  objPtr->textObj->resize(objPtr->ow,objPtr->oh);
		  objPtr->textObj->setShowCursor(false);
		}

	      else
		cerr << "Unknown tag '" << tag << "' in OBJECT" << endl;    
	      
	      if (!parser.close(tag))
		{
		  cerr << "ERR: Closing Child" << endl;
		  return;
		}
	    }
	}
      else
	cerr << "Unknown tag '" << tag << "' in OBJECTS" << endl;    
      
      if (!parser.close(tag))
	{
	  cerr << "ERR: Closing Child" << endl;
	  return;
	}
    }
}

/*========================= load textobject ======================*/
void KPresenterDocument_impl::loadTxtObj(KOMLParser& parser,vector<KOMLAttrib>& lst,KTextObject *txtPtr)
{
  string tag;
  string name;

  KTextObject::EnumListType elt;
  KTextObject::UnsortListType ult;
  QFont font;
  QColor color;
  int r = 0,g = 0,b = 0;
  TxtParagraph *txtParagraph;
  TxtObj *objPtr;
  
  while (parser.open(0L,tag))
    {
      KOMLParser::parseTag(tag.c_str(),name,lst);
      
      // enumListType
      if (name == "ENUMLISTTYPE")
	{
	  KOMLParser::parseTag(tag.c_str(),name,lst);
	  vector<KOMLAttrib>::const_iterator it = lst.begin();
	  for(;it != lst.end();it++)
	    {
	      if ((*it).m_strName == "type")
		elt.type = atoi((*it).m_strValue.c_str());
	      if ((*it).m_strName == "before")
		elt.before = (*it).m_strValue.c_str();
	      if ((*it).m_strName == "after")
		elt.after = (*it).m_strValue.c_str();
	      if ((*it).m_strName == "start")
		elt.start = atoi((*it).m_strValue.c_str());
	      if ((*it).m_strName == "family")
		font.setFamily((*it).m_strValue.c_str());
	      if ((*it).m_strName == "pointSize")
		font.setPointSize(atoi((*it).m_strValue.c_str()));
	      if ((*it).m_strName == "bold")
		font.setBold((bool)atoi((*it).m_strValue.c_str()));
	      if ((*it).m_strName == "italic")
		font.setItalic((bool)atoi((*it).m_strValue.c_str()));
	      if ((*it).m_strName == "underline")
		font.setUnderline((bool)atoi((*it).m_strValue.c_str()));
	      if ((*it).m_strName == "red")
		r = atoi((*it).m_strValue.c_str());
	      if ((*it).m_strName == "green")
		g = atoi((*it).m_strValue.c_str());
	      if ((*it).m_strName == "blue")
		b = atoi((*it).m_strValue.c_str());
	    }
	  color.setRgb(r,g,b);
	  elt.font.operator=(font);
	  elt.color.operator=(color);
	  txtPtr->setEnumListType(elt);
	}
      
      // unsortListType
      else if (name == "UNSORTEDLISTTYPE")
	{
	  KOMLParser::parseTag(tag.c_str(),name,lst);
	  vector<KOMLAttrib>::const_iterator it = lst.begin();
	  for(;it != lst.end();it++)
	    {
	      if ((*it).m_strName == "chr")
		ult.chr = atoi((*it).m_strValue.c_str());
	      if ((*it).m_strName == "family")
		font.setFamily((*it).m_strValue.c_str());
	      if ((*it).m_strName == "pointSize")
		font.setPointSize(atoi((*it).m_strValue.c_str()));
	      if ((*it).m_strName == "bold")
		font.setBold((bool)atoi((*it).m_strValue.c_str()));
	      if ((*it).m_strName == "italic")
		font.setItalic((bool)atoi((*it).m_strValue.c_str()));
	      if ((*it).m_strName == "underline")
		font.setUnderline((bool)atoi((*it).m_strValue.c_str()));
	      if ((*it).m_strName == "red")
		r = atoi((*it).m_strValue.c_str());
	      if ((*it).m_strName == "green")
		g = atoi((*it).m_strValue.c_str());
	      if ((*it).m_strName == "blue")
		b = atoi((*it).m_strValue.c_str());
	    }
	  color.setRgb(r,g,b);
	  ult.font.operator=(font);
	  ult.color.operator=(color);
	  txtPtr->setUnsortListType(ult);
	}

      // paragraph
      else if (name == "PARAGRAPH")
	{
	  txtParagraph = txtPtr->addParagraph();
	  KOMLParser::parseTag(tag.c_str(),name,lst);
	  vector<KOMLAttrib>::const_iterator it = lst.begin();
	  for(;it != lst.end();it++)
	    {
	      if ((*it).m_strName == "horzAlign")
		txtParagraph->setHorzAlign((TxtParagraph::HorzAlign)atoi((*it).m_strValue.c_str()));
	    }

	  while (parser.open(0L,tag))
	    {
	      KOMLParser::parseTag(tag.c_str(),name,lst);
	      
	      // line
	      if (name == "LINE")
		{
		  while (parser.open(0L,tag))
		    {
		      KOMLParser::parseTag(tag.c_str(),name,lst);
		      
		      // object
		      if (name == "OBJ")
			{
			  objPtr = new TxtObj();

			  while (parser.open(0L,tag))
			    {
			      KOMLParser::parseTag(tag.c_str(),name,lst);
			      
			      // type
			      if (name == "TYPE")
				{
				  KOMLParser::parseTag(tag.c_str(),name,lst);
				  vector<KOMLAttrib>::const_iterator it = lst.begin();
				  for(;it != lst.end();it++)
				    {
				      if ((*it).m_strName == "value")
					objPtr->setType((TxtObj::ObjType)atoi((*it).m_strValue.c_str()));
				    }
				}

			      // font
			      else if (name == "FONT")
				{
				  KOMLParser::parseTag(tag.c_str(),name,lst);
				  vector<KOMLAttrib>::const_iterator it = lst.begin();
				  for(;it != lst.end();it++)
				    {
				      if ((*it).m_strName == "family")
					font.setFamily((*it).m_strValue.c_str());
				      if ((*it).m_strName == "pointSize")
					font.setPointSize(atoi((*it).m_strValue.c_str()));
				      if ((*it).m_strName == "bold")
					font.setBold((bool)atoi((*it).m_strValue.c_str()));
				      if ((*it).m_strName == "italic")
					font.setItalic((bool)atoi((*it).m_strValue.c_str()));
				      if ((*it).m_strName == "underline")
					font.setUnderline((bool)atoi((*it).m_strValue.c_str()));
				    }
				  objPtr->setFont(font);
				}

			      // color
			      else if (name == "COLOR")
				{
				  KOMLParser::parseTag(tag.c_str(),name,lst);
				  vector<KOMLAttrib>::const_iterator it = lst.begin();
				  for(;it != lst.end();it++)
				    {
				      if ((*it).m_strName == "red")
					r = atoi((*it).m_strValue.c_str());
				      if ((*it).m_strName == "green")
					g = atoi((*it).m_strValue.c_str());
				      if ((*it).m_strName == "blue")
					b = atoi((*it).m_strValue.c_str());
				    }
				  color.setRgb(r,g,b);
				  objPtr->setColor(color);
				}

			      // vertical align
			      else if (name == "VERTALIGN")
				{
				  KOMLParser::parseTag(tag.c_str(),name,lst);
				  vector<KOMLAttrib>::const_iterator it = lst.begin();
				  for(;it != lst.end();it++)
				    {
				      if ((*it).m_strName == "value")
					objPtr->setVertAlign((TxtObj::VertAlign)atoi((*it).m_strValue.c_str()));
				    }
				}

			      // text
			      else if (name == "TEXT")
				{
				  KOMLParser::parseTag(tag.c_str(),name,lst);
				  vector<KOMLAttrib>::const_iterator it = lst.begin();
				  for(;it != lst.end();it++)
				    {
				      if ((*it).m_strName == "value")
					objPtr->append((*it).m_strValue.c_str());
				    }
				}

			      else
				cerr << "Unknown tag '" << tag << "' in OBJ" << endl;    
			      
			      if (!parser.close(tag))
				{
				  cerr << "ERR: Closing Child" << endl;
				  return;
				}

			    }
			  txtParagraph->append(objPtr);
			}
		      
		      else
			cerr << "Unknown tag '" << tag << "' in LINE" << endl;    
		      
		      if (!parser.close(tag))
			{
			  cerr << "ERR: Closing Child" << endl;
			  return;
			}
		    }
		}

	      else
		cerr << "Unknown tag '" << tag << "' in PARAGRAPH" << endl;    
	      
	      if (!parser.close(tag))
		{
		  cerr << "ERR: Closing Child" << endl;
		  return;
		}
	    }
	}
	  
      else
	cerr << "Unknown tag '" << tag << "' in TEXTOBJ" << endl;    
      
      if (!parser.close(tag))
	{
	  cerr << "ERR: Closing Child" << endl;
	  return;
	}
    }
}

/*========================== open ================================*/
/* CORBA::Boolean KPresenterDocument_impl::open(const char *_filename)
{
  return load(_filename);;
} */

/*========================== save as =============================*/
/* CORBA::Boolean KPresenterDocument_impl::saveAs(const char *_filename,const char *_format)
{
  return save(_filename);
} */
  
/*========================= create a view ========================*/
OPParts::View_ptr KPresenterDocument_impl::createView()
{
  KPresenterView_impl *p = new KPresenterView_impl(0L);
  p->setDocument(this);
  
  return OPParts::View::_duplicate(p);
}

/*========================== view list ===========================*/
void KPresenterDocument_impl::viewList(OPParts::Document::ViewList*& _list)
{
  (*_list).length(m_lstViews.count());

  int i = 0;
  QListIterator<KPresenterView_impl> it(m_lstViews);
  for(;it.current();++it)
    (*_list)[i++] = OPParts::View::_duplicate(it.current());
}

/*========================== output formats ======================*/
QStrList KPresenterDocument_impl::outputFormats()
{
  return new QStrList();
}

/*========================== input formats =======================*/
QStrList KPresenterDocument_impl::inputFormats()
{
  return new QStrList();
}

/*========================= add view =============================*/
void KPresenterDocument_impl::addView(KPresenterView_impl *_view)
{
  m_lstViews.append(_view);
}

/*======================== remove view ===========================*/
void KPresenterDocument_impl::removeView(KPresenterView_impl *_view)
{
  m_lstViews.setAutoDelete(false);
  m_lstViews.removeRef(_view);
  m_lstViews.setAutoDelete(true);
}

/*========================= insert an object =====================*/
void KPresenterDocument_impl::insertObject(const QRect& _rect, const char* _server_name)
{
  OPParts::Document_var doc = imr_createDocByServerName( _server_name);
  if (CORBA::is_nil(doc))
    return;
  
  if (!doc->init())
    {
      QMessageBox::critical((QWidget*)0L,i18n("KPresenter Error"),i18n("Could not init"),i18n("OK"));
      return;
    }

  KPresenterChild* ch = new KPresenterChild(this,_rect,doc);

  insertChild( ch );
}

/*========================= insert a child object =====================*/
void KPresenterDocument_impl::insertChild( KPresenterChild *_child )
{
  m_lstChildren.append( _child );
  
  emit sig_insertObject( _child );
}

/*======================= change child geometry ==================*/
void KPresenterDocument_impl::changeChildGeometry(KPresenterChild *_child,const QRect& _rect)
{
  _child->setGeometry(_rect);

  emit sig_updateChildGeometry(_child);
}

/*======================= child iterator =========================*/
QListIterator<KPresenterChild> KPresenterDocument_impl::childIterator()
{
  return QListIterator<KPresenterChild> (m_lstChildren);
}

/*===================== set page layout ==========================*/
void KPresenterDocument_impl::setPageLayout(KoPageLayout pgLayout,int diffx,int diffy)
{
  _pageLayout = pgLayout;
  for (pagePtr=_pageList.first();pagePtr != 0;pagePtr=_pageList.next())
    {
       if ((!pagePtr->backPix.isNull()) && (pagePtr->backPicView == BV_ZOOM))
	 {
	  QWMatrix m;
 	  m.scale((float)getPageSize(pagePtr->pageNum,diffx,diffy).width()/pagePtr->obackPix.width(),
 		  (float)getPageSize(pagePtr->pageNum,diffx,diffy).height()/pagePtr->obackPix.height());
 	  pagePtr->backPix.operator=(pagePtr->obackPix.xForm(m));
	 }
       if (pagePtr->backType == BT_CLIP)
	 {
	   pagePtr->pic->resize(getPageSize(pagePtr->pageNum,diffx,diffy).width(),
				getPageSize(pagePtr->pageNum,diffx,diffy).height());
	 }
       pagePtr->cPix->resize(getPageSize(pagePtr->pageNum,diffx,diffy).width(),
			     getPageSize(pagePtr->pageNum,diffx,diffy).height());
       emit restoreBackColor(pagePtr->pageNum-1);
    }

  repaint(true);
}

/*==================== insert a new page =========================*/
unsigned int KPresenterDocument_impl::insertNewPage(int diffx,int diffy)
{
  _pageNums++;

  pagePtr = new Background;
  pagePtr->pageNum = _pageNums;
  pagePtr->backType = BT_COLOR;
  pagePtr->backPicView = BV_CENTER;
  pagePtr->backPic = 0;
  pagePtr->backClip = 0;
  pagePtr->pic = new BackPic(0);
  pagePtr->pic->resize(getPageSize(pagePtr->pageNum,diffx,diffy).width(),
		       getPageSize(pagePtr->pageNum,diffx,diffy).height());
  pagePtr->pic->move(getPageSize(pagePtr->pageNum,diffx,diffy).x(),
 		     getPageSize(pagePtr->pageNum,diffx,diffy).y());
  pagePtr->pic->hide();
  pagePtr->backColor1.operator=(white);
  pagePtr->backColor2.operator=(white);
  pagePtr->bcType = BCT_PLAIN;
  pagePtr->cPix = new QPixmap(getPageSize(pagePtr->pageNum,diffx,diffy).width(),
			      getPageSize(pagePtr->pageNum,diffx,diffy).height()); 
  pagePtr->timeParts.setAutoDelete(false);
  pagePtr->timeParts.append((int*)10);
  _pageList.append(pagePtr);

  emit restoreBackColor(_pageNums-1);
  repaint(true);

  return _pageNums;
}

/*==================== set background color ======================*/
void KPresenterDocument_impl::setBackColor(unsigned int pageNum,QColor backColor1,QColor backColor2,BCType bcType)
{
  for (pagePtr=_pageList.first();pagePtr != 0;pagePtr=_pageList.next())
    {
      if (pagePtr->pageNum == pageNum)
	{
	  pagePtr->backColor1.operator=(backColor1);
	  pagePtr->backColor2.operator=(backColor2);
	  pagePtr->bcType = bcType;
	  emit restoreBackColor(pageNum-1);
	  return;
	}
    }
}

/*==================== set background picture ====================*/
void KPresenterDocument_impl::setBackPic(unsigned int pageNum,const char* backPic)
{
  for (pagePtr=_pageList.first();pagePtr != 0;pagePtr=_pageList.next())
    {
      if (pagePtr->pageNum == pageNum)
	{
	  pagePtr->backPic = qstrdup(backPic);
	  pagePtr->backPix.load(pagePtr->backPic);
	  pagePtr->obackPix.load(pagePtr->backPic);
	  emit restoreBackColor(pageNum-1);
	  return;
	}
    }
}

/*==================== set background clipart ====================*/
void KPresenterDocument_impl::setBackClip(unsigned int pageNum,const char* backClip)
{
  for (pagePtr=_pageList.first();pagePtr != 0;pagePtr=_pageList.next())
    {
      if (pagePtr->pageNum == pageNum)
	{
	  pagePtr->backClip = qstrdup(backClip);
	  if (backClip)
	    pagePtr->pic->setClipart(backClip);
	  emit restoreBackColor(pageNum-1);
	  return;
	}
    }
}

/*================= set background pic view ======================*/
void KPresenterDocument_impl::setBPicView(unsigned int pageNum,BackView picView)
{
  for (pagePtr=_pageList.first();pagePtr != 0;pagePtr=_pageList.next())
    {
      if (pagePtr->pageNum == pageNum)
	{
	  pagePtr->backPicView = picView;
	  if ((picView == BV_ZOOM) && (!pagePtr->backPix.isNull()))
	    {
	      QWMatrix m;
	      m.scale((float)getPageSize(pagePtr->pageNum,0,0).width()/pagePtr->obackPix.width(),
		      (float)getPageSize(pagePtr->pageNum,0,0).height()/pagePtr->obackPix.height());
	      pagePtr->backPix.operator=(pagePtr->obackPix.xForm(m));
	    }
	  emit restoreBackColor(pageNum-1);
	  return;
	}
    }
}

/*==================== set background type =======================*/
void KPresenterDocument_impl::setBackType(unsigned int pageNum,BackType backType)
{
  for (pagePtr=_pageList.first();pagePtr != 0;pagePtr=_pageList.next())
    {
      if (pagePtr->pageNum == pageNum)
	{
	  pagePtr->backType = backType;
	  emit restoreBackColor(pageNum-1);
	  repaint(true);
	  return;
	}
    }
}

/*===================== set pen and brush ========================*/
bool KPresenterDocument_impl::setPenBrush(QPen pen,QBrush brush,int diffx,int diffy)
{
  bool ret = false;
  unsigned int i;

  if (!_objList.isEmpty())
    {
      for (i = 0;i <= _objList.count()-1;i++)
	{
	  if (_objList.at(i)->isSelected)
	    {
	      objPtr = _objList.at(i);
	      if (objPtr->objType != OT_TEXT && objPtr->objType != OT_PICTURE
		  && objPtr->objType != OT_CLIPART)
		{
		  objPtr->graphObj->setObjPen(pen);
		  objPtr->graphObj->setObjBrush(brush);
		  repaint(objPtr->ox,objPtr->oy,
			  objPtr->ow,objPtr->oh,TRUE);
		  ret = true;
		}
	    }      
	}
    }
  return ret;
}

/*=================== get background type ========================*/
BackType KPresenterDocument_impl::getBackType(unsigned int pageNum)
{
  for (pagePtr=_pageList.first();pagePtr != 0;pagePtr=_pageList.next())
    if (pagePtr->pageNum == pageNum) return pagePtr->backType;
  return BT_COLOR;
}

/*=================== get background pic view ====================*/
BackView KPresenterDocument_impl::getBPicView(unsigned int pageNum)
{
  for (pagePtr=_pageList.first();pagePtr != 0;pagePtr=_pageList.next())
    if (pagePtr->pageNum == pageNum) return pagePtr->backPicView;
  return BV_TILED;
}

/*=================== get background picture =====================*/
const char* KPresenterDocument_impl::getBackPic(unsigned int pageNum)
{
  for (pagePtr=_pageList.first();pagePtr != 0;pagePtr=_pageList.next())
    if (pagePtr->pageNum == pageNum)
      return pagePtr->backPic;
  return 0;
}

/*=================== get background clipart =====================*/
const char* KPresenterDocument_impl::getBackClip(unsigned int pageNum)
{
  for (pagePtr=_pageList.first();pagePtr != 0;pagePtr=_pageList.next())
    if (pagePtr->pageNum == pageNum) return pagePtr->backClip;
  return 0;
}

/*=================== get background color 1 ======================*/
QColor KPresenterDocument_impl::getBackColor1(unsigned int pageNum)
{
  for (pagePtr=_pageList.first();pagePtr != 0;pagePtr=_pageList.next())
    if (pagePtr->pageNum == pageNum) return pagePtr->backColor1;
  return white;
}

/*=================== get background color 2 ======================*/
QColor KPresenterDocument_impl::getBackColor2(unsigned int pageNum)
{
  for (pagePtr=_pageList.first();pagePtr != 0;pagePtr=_pageList.next())
    if (pagePtr->pageNum == pageNum) return pagePtr->backColor2;
  return white;
}

/*=================== get background color type ==================*/
BCType KPresenterDocument_impl::getBackColorType(unsigned int pageNum)
{
  for (pagePtr=_pageList.first();pagePtr != 0;pagePtr=_pageList.next())
    if (pagePtr->pageNum == pageNum) return pagePtr->bcType;
  return BCT_PLAIN;
}

/*========================= get pen ==============================*/
QPen KPresenterDocument_impl::getPen(QPen pen)
{
  if (!_objList.isEmpty())
    {
      for (unsigned int i = 0;i <= _objList.count()-1;i++)
	{
	  if (_objList.at(i)->isSelected)
	    {
	      objPtr = _objList.at(i);
	      if (objPtr->objType != OT_TEXT && objPtr->objType != OT_PICTURE
		  && objPtr->objType != OT_CLIPART)
		return objPtr->graphObj->getObjPen();
	    }      
	}
    }
  return pen;
}

/*========================= get brush =============================*/
QBrush KPresenterDocument_impl::getBrush(QBrush brush)
{
  if (!_objList.isEmpty())
    {
      for (unsigned int i = 0;i <= _objList.count()-1;i++)
	{
	  if (_objList.at(i)->isSelected)
	    {
	      objPtr = _objList.at(i);
	      if (objPtr->objType != OT_TEXT && objPtr->objType != OT_PICTURE
		  && objPtr->objType != OT_CLIPART)
		return objPtr->graphObj->getObjBrush();
	    }      
	}
    }
  return brush;
}

/*======================== raise objects =========================*/
void KPresenterDocument_impl::raiseObjs(int diffx,int diffy)
{
  if (!_objList.isEmpty())
    {
      for (unsigned int i = 0;i <= _objList.count()-1;i++)
	{
	  objPtr = _objList.at(i);
	  if (objPtr->isSelected)
	    {
	      _objList.take(i);
	      _objList.append(objPtr);
	      repaint(objPtr->ox,objPtr->oy,
		      objPtr->ow,objPtr->oh,TRUE);
	    
	    }
	}      
      reArrangeObjs();
    }
}

/*======================== lower objects =========================*/
void KPresenterDocument_impl::lowerObjs(int diffx,int diffy)
{
  if (!_objList.isEmpty())
    {
      for (unsigned int i = 0;i <= _objList.count()-1;i++)
	{
	  objPtr = _objList.at(i);
	  if (objPtr->isSelected)
	    {
	      _objList.take(i);
	      _objList.insert(0,objPtr);
	      repaint(objPtr->ox,objPtr->oy,
		      objPtr->ow,objPtr->oh,false);
	    }
	}      
      reArrangeObjs();
    }
}

/*=================== insert a picture ==========================*/
void KPresenterDocument_impl::insertPicture(const char *filename,int diffx,int diffy)
{
  QApplication::setOverrideCursor(waitCursor);
  
  if (filename)
    {
      QPixmap pix(filename);
      if (!pix.isNull())
	{
	  _objNums++;
	  objPtr = new PageObjects;
	  objPtr->isSelected = true;
	  objPtr->objNum = _objNums;
	  objPtr->objType = OT_PICTURE;
	  objPtr->ox = diffx + 10;
	  objPtr->oy = diffy + 10;
	  objPtr->ow = pix.width();
	  objPtr->oh = pix.height();
	  objPtr->graphObj = new GraphObj(0,"graphObj",OT_PICTURE,QString(filename));
	  objPtr->graphObj->resize(objPtr->ow,objPtr->oh);
	  objPtr->graphObj->loadPixmap();
	  objPtr->presNum = 0;
	  objPtr->effect = EF_NONE;
	  _objList.append(objPtr);
	  repaint(objPtr->ox,objPtr->oy,
		  objPtr->ow,objPtr->oh,false);
	}
    }
  QApplication::restoreOverrideCursor();
}

/*=================== insert a clipart ==========================*/
void KPresenterDocument_impl::insertClipart(const char *filename,int diffx,int diffy)
{
  QApplication::setOverrideCursor(waitCursor);
  
  if (filename)
    {
      _objNums++;
      objPtr = new PageObjects;
      objPtr->isSelected = true;
      objPtr->objNum = _objNums;
      objPtr->objType = OT_CLIPART;
      objPtr->ox = diffx + 10;
      objPtr->oy = diffy + 10;
      objPtr->ow = 150;
      objPtr->oh = 150;
      objPtr->graphObj = new GraphObj(0,"graphObj",OT_CLIPART,QString(filename));
      objPtr->graphObj->resize(objPtr->ow,objPtr->oh);
      objPtr->graphObj->loadClipart();
      objPtr->presNum = 0;
      objPtr->effect = EF_NONE;
      _objList.append(objPtr);
      repaint(objPtr->ox,objPtr->oy,
	      objPtr->ow,objPtr->oh,false);
    }
  QApplication::restoreOverrideCursor();
}

/*======================= change picture ========================*/
void KPresenterDocument_impl::changePicture(const char *filename,int diffx,int diffy)
{
  QApplication::setOverrideCursor(waitCursor);

  if (filename)
    {
      QPixmap pix(filename);
      if (!_objList.isEmpty() && !pix.isNull())
	{
	  for (unsigned int i = 0;i <= _objList.count()-1;i++)
	    {
	      if (_objList.at(i)->isSelected)
		{
		  objPtr = _objList.at(i);
		  if (objPtr->objType == OT_PICTURE)
		    {
		      objPtr->graphObj->setFileName(QString(filename));
		      objPtr->graphObj->loadPixmap();
		      objPtr->isSelected = false;
		      objPtr->graphObj->resize(pix.size());
		      repaint(objPtr->ox,objPtr->oy,
			      objPtr->ow,objPtr->oh,false);
		      objPtr = _objList.at(i);
		      objPtr->ow = pix.width();
		      objPtr->oh = pix.height();
		      objPtr->isSelected = true;
		      repaint(objPtr->ox,objPtr->oy,
			      objPtr->ow,objPtr->oh,false);
		      break;
		    }
		}      
	    }
	}
    }
 
  QApplication::restoreOverrideCursor();
}

/*======================= change clipart ========================*/
void KPresenterDocument_impl::changeClipart(const char *filename,int diffx,int diffy)
{
  QApplication::setOverrideCursor(waitCursor);

  if (filename)
    {
      if (!_objList.isEmpty())
	{
	  for (unsigned int i = 0;i <= _objList.count()-1;i++)
	    {
	      if (_objList.at(i)->isSelected)
		{
		  objPtr = _objList.at(i);
		  if (objPtr->objType == OT_CLIPART)
		    {
		      objPtr->graphObj->setFileName(QString(filename));
		      objPtr->graphObj->loadClipart();
		      objPtr->isSelected = false;
		      repaint(objPtr->ox,objPtr->oy,
			      objPtr->ow,objPtr->oh,false);
		      objPtr = _objList.at(i);
		      objPtr->isSelected = true;
		      repaint(objPtr->ox,objPtr->oy,
			      objPtr->ow,objPtr->oh,false);
		      break;
		    }
		}      
	    }
	}
    }
 
  QApplication::restoreOverrideCursor();
}

/*===================== insert a line ===========================*/
void KPresenterDocument_impl::insertLine(QPen pen,LineType lt,int diffx,int diffy)
{
  _objNums++;
  objPtr = new PageObjects;
  objPtr->isSelected = true;
  objPtr->objType = OT_LINE;
  objPtr->objNum = _objNums;
  objPtr->ox = diffx + 10;
  objPtr->oy = diffy + 10;
  objPtr->ow = 150;
  objPtr->oh = 150;
  objPtr->graphObj = new GraphObj(0,"graphObj",OT_LINE);
  objPtr->graphObj->setObjPen(pen);
  objPtr->graphObj->setLineType(lt);
  objPtr->graphObj->resize(objPtr->ow,objPtr->oh);
  objPtr->presNum = 0;
  objPtr->effect = EF_NONE;
  _objList.append(objPtr);
  repaint(objPtr->ox,objPtr->oy,
	  objPtr->ow,objPtr->oh,false);
}

/*===================== insert a rectangle =======================*/
void KPresenterDocument_impl::insertRectangle(QPen pen,QBrush brush,RectType rt,int diffx,int diffy)
{
  _objNums++;
  objPtr = new PageObjects;
  objPtr->isSelected = true;
  objPtr->objType = OT_RECT;
  objPtr->objNum = _objNums;
  objPtr->ox = diffx + 10;
  objPtr->oy = diffy + 10;
  objPtr->ow = 150;
  objPtr->oh = 150;
  objPtr->graphObj = new GraphObj(0,"graphObj",OT_RECT);
  objPtr->graphObj->setObjPen(pen);
  objPtr->graphObj->setObjBrush(brush);
  objPtr->graphObj->setRectType(rt);
  objPtr->graphObj->setRnds(_xRnd,_yRnd);
  objPtr->graphObj->resize(objPtr->ow,objPtr->oh);
  objPtr->presNum = 0;
  objPtr->effect = EF_NONE;
  _objList.append(objPtr);
  repaint(objPtr->ox,objPtr->oy,
	  objPtr->ow,objPtr->oh,false);
}

/*===================== insert a circle or ellipse ===============*/
void KPresenterDocument_impl::insertCircleOrEllipse(QPen pen,QBrush brush,int diffx,int diffy)
{
  _objNums++;
  objPtr = new PageObjects;
  objPtr->isSelected = true;
  objPtr->objType = OT_CIRCLE;
  objPtr->objNum = _objNums;
  objPtr->ox = diffx + 10;
  objPtr->oy = diffy + 10;
  objPtr->ow = 150;
  objPtr->oh = 150;
  objPtr->graphObj = new GraphObj(0,"graphObj",OT_CIRCLE);
  objPtr->graphObj->setObjPen(pen);
  objPtr->graphObj->setObjBrush(brush);
  objPtr->graphObj->resize(objPtr->ow,objPtr->oh);
  objPtr->presNum = 0;
  objPtr->effect = EF_NONE;
  _objList.append(objPtr);
  repaint(objPtr->ox,objPtr->oy,
	  objPtr->ow,objPtr->oh,false);
}

/*===================== insert a textobject =====================*/
void KPresenterDocument_impl::insertText(int diffx,int diffy)
{
  _objNums++;
  objPtr = new PageObjects;
  objPtr->isSelected = true;
  objPtr->objNum = _objNums;
  objPtr->objType = OT_TEXT;
  objPtr->ox = diffx + 10;
  objPtr->oy = diffy + 10;
  objPtr->ow = 150;
  objPtr->oh = 150;
  objPtr->textObj = new KTextObject(0,"textObj",KTextObject::PLAIN);
  objPtr->textObj->setBackgroundColor(_txtBackCol);
  //objPtr->textObj->breakLines(objPtr->ow);
  objPtr->textObj->resize(objPtr->ow,objPtr->oh);
  objPtr->textObj->setShowCursor(false);
  objPtr->presNum = 0;
  objPtr->effect = EF_NONE;
  _objList.append(objPtr);
  repaint(objPtr->ox,objPtr->oy,
	  objPtr->ow,objPtr->oh,false);
}

/*======================= insert an autoform ====================*/
void KPresenterDocument_impl::insertAutoform(QPen pen,QBrush brush,const char *fileName,int diffx,int diffy)
{
  _objNums++;
  objPtr = new PageObjects;
  objPtr->objType = OT_AUTOFORM;
  objPtr->isSelected = true;
  objPtr->objNum = _objNums;
  objPtr->ox = diffx + 10;
  objPtr->oy = diffy + 10;
  objPtr->ow = 150;
  objPtr->oh = 150;
  objPtr->graphObj = new GraphObj(0,"graphObj",OT_AUTOFORM,QString(fileName));
  objPtr->graphObj->setObjPen(pen);
  objPtr->graphObj->setObjBrush(brush);
  objPtr->presNum = 0;
  objPtr->effect = EF_NONE;
  _objList.append(objPtr);
  objPtr->graphObj->resize(objPtr->ow,objPtr->oh);
  repaint(objPtr->ox,objPtr->oy,
 	  objPtr->ow,objPtr->oh,false);
}

/*=================== repaint all views =========================*/
void KPresenterDocument_impl::repaint(bool erase)
{
  if (!m_lstViews.isEmpty())
    {
      for (viewPtr = m_lstViews.first();viewPtr != 0;viewPtr = m_lstViews.next())
	viewPtr->repaint(erase);
    }
}

/*=================== repaint all views =========================*/
void KPresenterDocument_impl::repaint(unsigned int x,unsigned int y,unsigned int w,
				      unsigned int h,bool erase)
{
  if (!m_lstViews.isEmpty())
    {
      for (viewPtr = m_lstViews.first();viewPtr != 0;viewPtr = m_lstViews.next())
	{
	  x -= viewPtr->getDiffX();
	  y -= viewPtr->getDiffY();
	  viewPtr->repaint(x,y,w,h,erase);
	}
    }
}

/*==================== reorder page =============================*/
QList<int> KPresenterDocument_impl::reorderPage(unsigned int num,int diffx,int diffy,float fakt = 1.0)
{
  QList<int> orderList;
  bool inserted;
  
  if (!_objList.isEmpty())
    {
      orderList.append((int*)0);
      for (unsigned int i = 0;i < _objList.count();i++)
	{
	  objPtr = _objList.at(i);
	  if (getPageOfObj(objPtr->objNum,diffx,diffy,fakt) == num)
	    {
	      objPtr = _objList.at(i);
	      if (orderList.find((int*)objPtr->presNum) == -1)
		{
		  if (orderList.isEmpty())
		    orderList.append((int*)objPtr->presNum);
		  else
		    {
		      inserted = false;
		      for (int j = orderList.count()-1;j >= 0;j--)
			{
			  if ((int*)objPtr->presNum > orderList.at(j))
			    {
			      orderList.insert(j+1,(int*)objPtr->presNum);
			      j = -1;
			      inserted = true;
			    }
			}
		      if (!inserted) orderList.insert(0,(int*)objPtr->presNum);
		    }
		}
	    }
	}
    } 

  return orderList;
}

/*====================== get page of object ======================*/
int KPresenterDocument_impl::getPageOfObj(int objNum,int diffx,int diffy,float fakt = 1.0)
{
  int i,j;
  QRect rect;

  for (i = 0;i < _objList.count();i++)
    {
      objPtr = _objList.at(i);
      if (objPtr->objNum == objNum)
	{
	  for (j = 0;j < _pageList.count();j++)
	    {
	      rect = getPageSize(j+1,diffx,diffy,fakt);
	      rect.setWidth(QApplication::desktop()->width());
	      if (rect.intersects(QRect(objPtr->ox - diffx,objPtr->oy - diffy,
					objPtr->ow,objPtr->oh)))	  
		return j+1;
	    }
	}
    }
  return -1;
}

/*================== get size of page ===========================*/
QRect KPresenterDocument_impl::getPageSize(unsigned int num,int diffx,int diffy,float fakt=1.0)
{
  double fact = 1;
  if (_pageLayout.unit == PG_CM) fact = 10;
  if (_pageLayout.unit == PG_INCH) fact = 25.4;

  int pw,ph,bl = (int)(_pageLayout.left * fact * 100)/100,br = (int)(_pageLayout.right * fact * 100)/100;
  int bt = (int)(_pageLayout.top * fact * 100)/100,bb = (int)(_pageLayout.bottom * fact * 100)/100;
  int wid = (int)(_pageLayout.width * fact * 100)/100,hei = (int)(_pageLayout.height * fact * 100)/100;
  
  pw = wid*(int)(MM_TO_POINT * 100) / 100 - 
    (bl + br);
  ph = hei*(int)(MM_TO_POINT * 100) / 100 -
    (bt + bb);

  pw = (int)((float)pw * fakt);
  ph = (int)((float)ph * fakt);

  QRect rect(10 - diffx,(10 + ph * (num - 1) +
			 (num - 1) * 10) - diffy,pw,ph);
  return rect;
}

/*==================== rearrange objects =========================*/
void KPresenterDocument_impl::reArrangeObjs()
{
  _objNums = 0;
  if (!_objList.isEmpty())
    {
      for (objPtr=_objList.first();objPtr != 0;objPtr=_objList.next())
	{
	  _objNums++;
	  objPtr->objNum = _objNums;
	}
    }
}

/*======================= delete objects =========================*/
void KPresenterDocument_impl::deleteObjs()
{
  bool changed = false;
  
  if (!_objList.isEmpty())
    {
      for (unsigned int i=0;i < _objList.count();i++)
	{
	  objPtr = _objList.at(i);
	  if (objPtr->isSelected)
	    {
	      _objList.remove(i);
	      i--;
	      changed = true;
	    }
	}
      if (changed) repaint(true);
      reArrangeObjs();
    }      
}

/*========================== copy objects ========================*/
void KPresenterDocument_impl::copyObjs(int diffx,int diffy)
{
  QClipboard *cb = QApplication::clipboard();
  QString clipStr = "";
  char str[255];
  
  clipStr += "[KPRESENTER-DATA]";

  if (!_objList.isEmpty())
    {
      for (unsigned int i=0;i < _objList.count();i++)
	{
	  objPtr = _objList.at(i);
	  if (objPtr->isSelected)
	    {
	      if (objPtr->objType == OT_TEXT)
		debug("At the moment text can't be copied to the clipboard. SORRY!");
	      else
		{
		  clipStr += "[NEW_OBJECT_START]";
		  
		  clipStr += "[OBJ_TYPE]{";
		  sprintf(str,"%d",(int)objPtr->objType);
		  clipStr += str;
		  clipStr += "}";
		  
		  clipStr += "[OBJ_X]{";
		  sprintf(str,"%d",objPtr->ox - diffx);
		  clipStr += str;
		  clipStr += "}";
		  
		  clipStr += "[OBJ_Y]{";
		  sprintf(str,"%d",objPtr->oy - diffy);
		  clipStr += str;
		  clipStr += "}";
		  
		  clipStr += "[OBJ_W]{";
		  sprintf(str,"%d",objPtr->ow);
		  clipStr += str;
		  clipStr += "}";
		  
		  clipStr += "[OBJ_H]{";
		  sprintf(str,"%d",objPtr->oh);
		  clipStr += str;
		  clipStr += "}";
		  
		  clipStr += "[OBJ_PRESNUM]{";
		  sprintf(str,"%d",objPtr->presNum);
		  clipStr += str;
		  clipStr += "}";
		  
		  clipStr += "[OBJ_EFFECT]{";
		  sprintf(str,"%d",(int)objPtr->effect);
		  clipStr += str;
		  clipStr += "}";

		  clipStr += "[GRAPHOBJ]";
		  
		  clipStr += "[LINE_TYPE]{";
		  sprintf(str,"%d",(int)objPtr->graphObj->getLineType());
		  clipStr += str;
		  clipStr += "}";
		  
		  clipStr += "[RECT_TYPE]{";
		  sprintf(str,"%d",(int)objPtr->graphObj->getRectType());
		  clipStr += str;
		  clipStr += "}";

		  clipStr += "[PEN_WIDTH]{";
		  sprintf(str,"%d",(int)objPtr->graphObj->getObjPen().width());
		  clipStr += str;
		  clipStr += "}";

		  clipStr += "[PEN_STYLE]{";
		  sprintf(str,"%d",(int)objPtr->graphObj->getObjPen().style());
		  clipStr += str;
		  clipStr += "}";

		  clipStr += "[PEN_RED]{";
		  sprintf(str,"%d",objPtr->graphObj->getObjPen().color().red());
		  clipStr += str;
		  clipStr += "}";

		  clipStr += "[PEN_GREEN]{";
		  sprintf(str,"%d",objPtr->graphObj->getObjPen().color().green());
		  clipStr += str;
		  clipStr += "}";

		  clipStr += "[PEN_BLUE]{";
		  sprintf(str,"%d",objPtr->graphObj->getObjPen().color().blue());
		  clipStr += str;
		  clipStr += "}";

		  clipStr += "[BRUSH_STYLE]{";
		  sprintf(str,"%d",(int)objPtr->graphObj->getObjBrush().style());
		  clipStr += str;
		  clipStr += "}";

		  clipStr += "[BRUSH_RED]{";
		  sprintf(str,"%d",objPtr->graphObj->getObjBrush().color().red());
		  clipStr += str;
		  clipStr += "}";

		  clipStr += "[BRUSH_GREEN]{";
		  sprintf(str,"%d",objPtr->graphObj->getObjBrush().color().green());
		  clipStr += str;
		  clipStr += "}";

		  clipStr += "[BRUSH_BLUE]{";
		  sprintf(str,"%d",objPtr->graphObj->getObjBrush().color().blue());
		  clipStr += str;
		  clipStr += "}";

		  clipStr += "[FILENAME]{";
		  sprintf(str,"%s",(const char*)objPtr->graphObj->getFileName());
		  clipStr += str;
		  clipStr += "}";

		  clipStr += "[RND_X]{";
		  sprintf(str,"%d",objPtr->graphObj->getRndX());
		  clipStr += str;
		  clipStr += "}";

		  clipStr += "[RND_Y]{";
		  sprintf(str,"%d",objPtr->graphObj->getRndY());
		  clipStr += str;
		  clipStr += "}";

		  clipStr += "[NEW_OBJECT_END]";
		}
	    }
	}
    }
  
  cb->setText((const char*)clipStr);

}

/*========================= paste objects ========================*/
void KPresenterDocument_impl::pasteObjs(int diffx,int diffy)
{
  QClipboard *cb = QApplication::clipboard();
  QString clipStr = cb->text(),tag,value;
  bool tagStarted = false,valueStarted = false;
  QPen pen;
  QBrush brush;
  QColor color;
  
  objPtr = 0;


  if (!clipStr.isEmpty() && clipStr.left(strlen("[KPRESENTER-DATA]")) == "[KPRESENTER-DATA]")
    {
      for (unsigned int i = 0;i < clipStr.length();i++)
	{
	  
	  // start tag
	  if (clipStr.mid(i,1) == "[")
	    {
	      tagStarted = true;
	      tag = "";
	    }

	  // end tag
	  else if (clipStr.mid(i,1) == "]")
	    {
	      if (tagStarted)
		{
		  tagStarted = false;
		  if (tag == "NEW_OBJECT_START")
		    {
		      objPtr = new PageObjects;
		      objPtr->isSelected = true;
		    }
		  else if (tag == "NEW_OBJECT_END")
		    {
		      _objNums++;
		      objPtr->objNum = _objNums;
		      objPtr->graphObj->hide();
		      _objList.append(objPtr);
		      repaint(objPtr->ox,objPtr->oy,
			      objPtr->ow,objPtr->oh,false);
		      objPtr = 0;
		    }
		  else if (tag == "GRAPHOBJ")
		    {
		      objPtr->graphObj = new GraphObj(0,"graphObj",objPtr->objType,"");
		      objPtr->graphObj->resize(objPtr->ow,objPtr->oh);
		    }
		}
	    }

	  // start value
	  else if (clipStr.mid(i,1) == "{")
	    {
	      valueStarted = true;
	      value = "";
	    }

	  // end value
	  else if (clipStr.mid(i,1) == "}")
	    {
	      if (valueStarted)
		{
		  valueStarted = false;
		  if (tag == "OBJ_TYPE" && objPtr)
		    objPtr->objType = (ObjType)atoi(value);
		  else if (tag == "OBJ_X" && objPtr)
		    objPtr->ox = atoi(value) + diffx;
		  else if (tag == "OBJ_Y" && objPtr)
		    objPtr->oy = atoi(value) + diffy;
		  else if (tag == "OBJ_W" && objPtr)
		    objPtr->ow = atoi(value);
		  else if (tag == "OBJ_H" && objPtr)
		    objPtr->oh = atoi(value);
		  else if (tag == "OBJ_PRESNUM" && objPtr)
		    objPtr->presNum = atoi(value);
		  else if (tag == "OBJ_EFFECT" && objPtr)
		    objPtr->effect = (Effect)atoi(value);
		  else if (tag == "LINE_TYPE" && objPtr && objPtr->graphObj)
		    objPtr->graphObj->setLineType((LineType)atoi(value));
		  else if (tag == "RECT_TYPE" && objPtr && objPtr->graphObj)
		    objPtr->graphObj->setRectType((RectType)atoi(value));
		  else if (tag == "PEN_WIDTH" && objPtr && objPtr->graphObj)
		    {
		      pen = objPtr->graphObj->getObjPen();
		      pen.setWidth(atoi(value));
		      objPtr->graphObj->setObjPen(pen);
		    }
		  else if (tag == "PEN_STYLE" && objPtr && objPtr->graphObj)
		    {
		      pen = objPtr->graphObj->getObjPen();
		      pen.setStyle((PenStyle)atoi(value));
		      objPtr->graphObj->setObjPen(pen);
		    }
		  else if (tag == "PEN_RED" && objPtr && objPtr->graphObj)
		    {
		      pen = objPtr->graphObj->getObjPen();
		      color = pen.color();
		      color.setRgb(atoi(value),color.green(),color.blue());
		      pen.setColor(color);
		      objPtr->graphObj->setObjPen(pen);
		    }
		  else if (tag == "PEN_GREEN" && objPtr && objPtr->graphObj)
		    {
		      pen = objPtr->graphObj->getObjPen();
		      color = pen.color();
		      color.setRgb(color.red(),atoi(value),color.blue());
		      pen.setColor(color);
		      objPtr->graphObj->setObjPen(pen);
		    }
		  else if (tag == "PEN_BLUE" && objPtr && objPtr->graphObj)
		    {
		      pen = objPtr->graphObj->getObjPen();
		      color = pen.color();
		      color.setRgb(color.red(),color.green(),atoi(value));
		      pen.setColor(color);
		      objPtr->graphObj->setObjPen(pen);
		    }
		  else if (tag == "BRUSH_STYLE" && objPtr && objPtr->graphObj)
		    {
		      brush = objPtr->graphObj->getObjBrush();
		      brush.setStyle((BrushStyle)atoi(value));
		      objPtr->graphObj->setObjBrush(brush);
		    }
		  else if (tag == "BRUSH_RED" && objPtr && objPtr->graphObj)
		    {
		      brush = objPtr->graphObj->getObjBrush();
		      color = brush.color();
		      color.setRgb(atoi(value),color.green(),color.blue());
		      brush.setColor(color);
		      objPtr->graphObj->setObjBrush(brush);
		    }
		  else if (tag == "BRUSH_GREEN" && objPtr && objPtr->graphObj)
		    {
		      brush = objPtr->graphObj->getObjBrush();
		      color = brush.color();
		      color.setRgb(color.red(),atoi(value),color.blue());
		      brush.setColor(color);
		      objPtr->graphObj->setObjBrush(brush);
		    }
		  else if (tag == "BRUSH_BLUE" && objPtr && objPtr->graphObj)
		    {
		      brush = objPtr->graphObj->getObjBrush();
		      color = brush.color();
		      color.setRgb(color.red(),color.green(),atoi(value));
		      brush.setColor(color);
		      objPtr->graphObj->setObjBrush(brush);
		    }
		  else if (tag == "FILENAME" && objPtr && objPtr->graphObj)
		    objPtr->graphObj->setFileName(value);
		  else if (tag == "RND_X" && objPtr && objPtr->graphObj)
		    objPtr->graphObj->setRnds(atoi(value),objPtr->graphObj->getRndY());
		  else if (tag == "RND_Y" && objPtr && objPtr->graphObj)
		    objPtr->graphObj->setRnds(objPtr->graphObj->getRndX(),atoi(value));
		}
	    }
	  
	  // get char
	  else
	    {
	      if (valueStarted)
		value += clipStr.mid(i,1);
	      else if (tagStarted)
		tag += clipStr.mid(i,1);
	    }
	}
    }
}

/*======================= rotate objects =========================*/
void KPresenterDocument_impl::rotateObjs()
{
}

/*====================== replace objects =========================*/
void KPresenterDocument_impl::replaceObjs()
{
  for (objPtr = _objList.first();objPtr != 0;objPtr = _objList.next())
    {
      objPtr->ox = (objPtr->ox / _rastX) * _rastX;
      objPtr->oy = (objPtr->oy / _rastY) * _rastY;
      if (objPtr->objType == OT_RECT && objPtr->graphObj->getRectType() == RT_ROUND)
	objPtr->graphObj->setRnds(_xRnd,_yRnd);
    }
}


