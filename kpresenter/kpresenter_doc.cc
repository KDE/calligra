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

#include <kfiledialog.h>
#include "kpresenter_doc.h"
#include "kpresenter_doc.moc"
#include "page.h"

/******************************************************************/
/* class KPresenterChild                                          */
/******************************************************************/

/*====================== constructor =============================*/
KPresenterChild::KPresenterChild(KPresenterDocument_impl *_kpr, const QRect& _rect,OPParts::Document_ptr _doc,
				 int _diffx,int _diffy)
  : KoDocumentChild(_rect,_doc)
{
  m_pKPresenterDoc = _kpr;
  m_rDoc = OPParts::Document::_duplicate(_doc);
  m_geometry = _rect;
  __geometry = QRect(_rect.left() + _diffx,_rect.top() + _diffy,_rect.right(),_rect.bottom());
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
  : _pixmapCollection(), _gradientCollection()
{
  ADD_INTERFACE("IDL:OPParts/Print:1.0")
  // Use CORBA mechanism for deleting views
  m_lstViews.setAutoDelete(false);
  m_lstChildren.setAutoDelete(true);

  m_bModified = false;

  // init
  _clean = true;
  _objectList.setAutoDelete(true);
  _backgroundList.setAutoDelete(true);
  _spInfinitLoop = false;
  _spManualSwitch = true;
  _rastX = 10;
  _rastY = 10;
  _xRnd = 20;
  _yRnd = 20;
  _txtBackCol = white;
  _txtSelCol = lightGray;
  _pageLayout.format = PG_SCREEN;
  _pageLayout.orientation = PG_PORTRAIT;
  _pageLayout.width = PG_SCREEN_WIDTH;
  _pageLayout.height = PG_SCREEN_HEIGHT;
  _pageLayout.left = 0;
  _pageLayout.right = 0;
  _pageLayout.top = 0;
  _pageLayout.bottom = 0;
  _pageLayout.unit = PG_MM;
  objStartY = 0;
  objStartNum = 0;
  setPageLayout(_pageLayout,0,0);
  _presPen = QPen(red,3,SolidLine);
}

/*====================== constructor =============================*/
KPresenterDocument_impl::KPresenterDocument_impl(const CORBA::BOA::ReferenceData &_refdata)
  : KPresenter::KPresenterDocument_skel(_refdata), _pixmapCollection(), _gradientCollection()
{
  ADD_INTERFACE("IDL:OPParts/Print:1.0")
  // Use CORBA mechanism for deleting views
  m_lstViews.setAutoDelete(false);
  m_lstChildren.setAutoDelete(true);

  m_bModified = false;

  // init
  _objectList.setAutoDelete(true);
  _backgroundList.setAutoDelete(true);
  _spInfinitLoop = false;
  _spManualSwitch = true;
  _rastX = 20;
  _rastY = 20;
  _xRnd = 20;
  _yRnd = 20;
  _txtBackCol = white;
  _txtSelCol = lightGray;
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
  objStartY = 0;
  objStartNum = 0;
  insertNewTemplate(0,0,true);
  _presPen = QPen(red,3,SolidLine);
}

/*====================== destructor ==============================*/
KPresenterDocument_impl::~KPresenterDocument_impl()
{
  sdeb("KPresenterDocument_impl::~KPresenterDocument_impl()\n");

  _objectList.clear();
  _backgroundList.clear();
  cleanUp();
  edeb("...KPresenterDocument_impl::~KPresenterDocument_impl() %i\n",_refcnt());
}

/*======================== draw contents as QPicture =============*/
void KPresenterDocument_impl::draw(QPaintDevice* _dev,CORBA::Long _width,CORBA::Long _height)
{
  warning("***********************************************");
  warning(i18n("KPresenter doesn't support KoDocument::draw(...) now!"));
  warning("***********************************************");
  return;
  
  if (m_lstViews.count() > 0)
    {
      QPainter painter;
      painter.begin(_dev);
     
      m_lstViews.at(0)->getPage()->draw(QRect(0,0,_width,_height),&painter);
      
      painter.end();
    }
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
  QListIterator<KPresenterChild> it(m_lstChildren);
  for(;it.current();++it)
    {
      if (!it.current()->isStoredExtern())
	return true;    
    }
  return false;
}

/*======================= make child list intern ================*/
void KPresenterDocument_impl::makeChildListIntern(OPParts::Document_ptr _doc,const char *_path)
{
  int i = 0;
  
  QListIterator<KPresenterChild> it(m_lstChildren);
  for(;it.current();++it)
    {
      QString tmp;
      tmp.sprintf("/%i",i++);
      QString path(_path);
      path += tmp.data();
      
      OPParts::Document_var doc = it.current()->document();    
      doc->makeChildList(_doc,path);
    }
}

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
  
  out << otag << "<BACKGROUND" << " rastX=\"" << _rastX << "\" rastY=\""
      << _rastY << "\" xRnd=\"" << _xRnd << "\" yRnd=\"" << _yRnd << "\" bred=\"" << _txtBackCol.red() << "\" bgreen=\""
      << _txtBackCol.green() << "\" bblue=\"" << _txtBackCol.blue() << "\" sred=\"" << _txtSelCol.red() << "\" sgreen=\""
      << _txtSelCol.green() << "\" sblue=\"" << _txtSelCol.blue() << "\">" << endl;
  saveBackground(out);
  out << etag << "</BACKGROUND>" << endl;

  out << otag << "<OBJECTS>" << endl;
  saveObjects(out);
  out << etag << "</OBJECTS>" << endl;

  out << indent << "<INFINITLOOP value=\"" << _spInfinitLoop << "\"/>" << endl; 
  out << indent << "<MANUALSWITCH value=\"" << _spManualSwitch << "\"/>" << endl; 

  // Write "OBJECT" tag for every child
  QListIterator<KPresenterChild> chl(m_lstChildren);
  for(;chl.current();++chl)
    chl.current()->save( out );

  out << etag << "</DOC>" << endl;
    
  setModified(false);
    
  return true;
}

/*========================== save background ====================*/
void KPresenterDocument_impl::saveBackground(ostream& out)
{
  KPBackGround *kpbackground = 0;
  
  for (int i = 0;i < static_cast<int>(_backgroundList.count());i++)
    {
      kpbackground = _backgroundList.at(i);
      out << otag << "<PAGE>" << endl;
      kpbackground->save(out);
      out << etag << "</PAGE>" << endl;
    }
}

/*========================== save objects =======================*/
void KPresenterDocument_impl::saveObjects(ostream& out)
{
  KPObject *kpobject = 0;
  
  for (int i = 0;i < static_cast<int>(objectList()->count());i++)
    {
      kpobject = objectList()->at(i);
      out << otag << "<OBJECT type=\"" << static_cast<int>(kpobject->getType()) << "\">" << endl;
      kpobject->save(out);
      out << etag << "</OBJECT>" << endl;
    }
}

/*========================== load ===============================*/
bool KPresenterDocument_impl::loadChildren( OPParts::MimeMultipartDict_ptr _dict )
{
  QListIterator<KPresenterChild> it(m_lstChildren);
  for(;it.current();++it)
    {
      if (!it.current()->loadDocument(_dict))
	return false;
    }
  
  return true;
}

/*========================= load a template =====================*/
bool KPresenterDocument_impl::load_template(const char *_url)
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
  
  return true;
}

/*========================== load ===============================*/
bool KPresenterDocument_impl::load(KOMLParser& parser)
{
  string tag;
  vector<KOMLAttrib> lst;
  string name;

  KoPageLayout __pgLayout;
  __pgLayout.unit = PG_MM;
  
  // clean
  if (_clean)
    {
      if (!_backgroundList.isEmpty())
	_backgroundList.clear();
      if (!_objectList.isEmpty())
	_objectList.clear();
      _spInfinitLoop = false;
      _spManualSwitch = true;
      _rastX = 20;
      _rastY = 20;
      _xRnd = 20;
      _yRnd = 20;
      _txtBackCol = white;
      _txtSelCol = lightGray;
    }

  // DOC
  if ( !parser.open( "DOC", tag ) )
  {
    cerr << "Missing DOC" << endl;
    return false;
  }
  
  KOMLParser::parseTag( tag.c_str(), name, lst );
  vector<KOMLAttrib>::const_iterator it = lst.begin();
  for(;it != lst.end();it++)
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
      
      if (name == "OBJECT")
	{
	  KPresenterChild *ch = new KPresenterChild(this);
	  ch->load(parser,lst);
	  insertChild(ch);
	  ch->_setGeometry(ch->geometry());
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
	  insertNewPage(0,0,false);
	  KPBackGround *kpbackground = _backgroundList.last();
	  kpbackground->load(parser,lst);
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
	  KOMLParser::parseTag(tag.c_str(),name,lst);
	  vector<KOMLAttrib>::const_iterator it = lst.begin();
	  for(;it != lst.end();it++)
	    {
	      if ((*it).m_strName == "type")
		{
		  ObjType t = (ObjType)atoi((*it).m_strValue.c_str());
	  
		  switch (t)
		    {
		    case OT_LINE:
		      {
			KPLineObject *kplineobject = new KPLineObject();
			kplineobject->load(parser,lst);
			_objectList.append(kplineobject);
		      } break;
		    case OT_RECT:
		      {
			KPRectObject *kprectobject = new KPRectObject();
			kprectobject->load(parser,lst);
			kprectobject->setRnds(_xRnd,_yRnd);
			_objectList.append(kprectobject);
		      } break;
		    case OT_ELLIPSE:
		      {
			KPEllipseObject *kpellipseobject = new KPEllipseObject();
			kpellipseobject->load(parser,lst);
			_objectList.append(kpellipseobject);
		      } break;
		    case OT_AUTOFORM:
		      {
			KPAutoformObject *kpautoformobject = new KPAutoformObject();
			kpautoformobject->load(parser,lst);
			_objectList.append(kpautoformobject);
		      } break;
		    case OT_CLIPART:
		      {
			KPClipartObject *kpclipartobject = new KPClipartObject();
			kpclipartobject->load(parser,lst);
			_objectList.append(kpclipartobject);
		      } break;
		    case OT_TEXT:
		      {
			KPTextObject *kptextobject = new KPTextObject();
			kptextobject->load(parser,lst);
			_objectList.append(kptextobject);
		      } break;
		    case OT_PICTURE:
		      {
			KPPixmapObject *kppixmapobject = new KPPixmapObject(&_pixmapCollection);
			kppixmapobject->load(parser,lst);
			_objectList.append(kppixmapobject);
		      } break;
		    default: break;
		    }

		  if (objStartY > 0) _objectList.last()->moveBy(0,objStartY);
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
void KPresenterDocument_impl::insertObject(const QRect& _rect, const char* _server_name,int _diffx,int _diffy)
{
  OPParts::Document_var doc = imr_createDocByServerName( _server_name);
  if (CORBA::is_nil(doc))
    return;
  
  if (!doc->init())
    {
      QMessageBox::critical((QWidget*)0L,i18n("KPresenter Error"),i18n("Could not init"),i18n("OK"));
      return;
    }

  KPresenterChild* ch = new KPresenterChild(this,_rect,doc,_diffx,_diffy);

  insertChild( ch );
}

/*========================= insert a child object =====================*/
void KPresenterDocument_impl::insertChild(KPresenterChild *_child)
{
  m_lstChildren.append( _child );
  
  emit sig_insertObject( _child );
}

/*======================= change child geometry ==================*/
void KPresenterDocument_impl::changeChildGeometry(KPresenterChild *_child,const QRect& _rect,int _diffx,int _diffy)
{
  _child->setGeometry(_rect);
  _child->_setGeometry(QRect(_rect.left() + _diffx,_rect.top() + _diffy,_rect.right(),_rect.bottom()));

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
  QRect r = getPageSize(0,diffx,diffy);

  for (int i = 0;i < static_cast<int>(_backgroundList.count());i++)
    {
      _backgroundList.at(i)->setSize(r.width(),r.height());
      _backgroundList.at(i)->restore();
    }

  repaint(false);
}

/*==================== insert a new page =========================*/
unsigned int KPresenterDocument_impl::insertNewPage(int diffx,int diffy,bool _restore=true)
{

  KPBackGround *kpbackground = new KPBackGround(&_pixmapCollection,&_gradientCollection);
  _backgroundList.append(kpbackground);

  if (_restore)
    {
      QRect r = getPageSize(0,diffx,diffy);
      _backgroundList.last()->setSize(r.width(),r.height());
      _backgroundList.last()->restore();
      repaint(false);
    }

  return getPageNums();
}

/*==================== insert a new page with template ===========*/
unsigned int KPresenterDocument_impl::insertNewTemplate(int diffx,int diffy,bool clean=false)
{
  QString templateDir = KApplication::kde_datadir();

  QString file = KFilePreviewDialog::getOpenFileName(templateDir + "/kpresenter/templates/plain.kpt","*.kpt|KPresenter templates",0);
  _clean = clean;
  objStartY = getPageSize(_backgroundList.count() - 1,0,0).y() + getPageSize(_backgroundList.count() - 1,0,0).height();
  objStartNum = _objectList.count();
  if (!file.isEmpty()) load_template(file);
  objStartNum = 0;
  objStartY = 0;
  _clean = true;

  return 0;
}

/*==================== set background color ======================*/
void KPresenterDocument_impl::setBackColor(unsigned int pageNum,QColor backColor1,QColor backColor2,BCType bcType)
{
  KPBackGround *kpbackground = 0;

  if (pageNum < _backgroundList.count())
    {
      kpbackground = backgroundList()->at(pageNum);
      kpbackground->setBackColor1(backColor1);
      kpbackground->setBackColor2(backColor2);
      kpbackground->setBackColorType(bcType);
    }
}

/*==================== set background picture ====================*/
void KPresenterDocument_impl::setBackPixFilename(unsigned int pageNum,QString backPix)
{
  if (pageNum < _backgroundList.count())
    backgroundList()->at(pageNum)->setBackPixFilename(backPix);
}

/*==================== set background clipart ====================*/
void KPresenterDocument_impl::setBackClipFilename(unsigned int pageNum,QString backClip)
{
  if (pageNum < _backgroundList.count())
    backgroundList()->at(pageNum)->setBackClipFilename(backClip);
}

/*================= set background pic view ======================*/
void KPresenterDocument_impl::setBackView(unsigned int pageNum,BackView backView)
{
  if (pageNum < _backgroundList.count())
    backgroundList()->at(pageNum)->setBackView(backView);
}

/*==================== set background type =======================*/
void KPresenterDocument_impl::setBackType(unsigned int pageNum,BackType backType)
{
  if (pageNum < _backgroundList.count())
    backgroundList()->at(pageNum)->setBackType(backType);
}

/*========================== set page effect =====================*/
void KPresenterDocument_impl::setPageEffect(unsigned int pageNum,PageEffect pageEffect)
{
  if (pageNum < _backgroundList.count())
    backgroundList()->at(pageNum)->setPageEffect(pageEffect);
}

/*===================== set pen and brush ========================*/
bool KPresenterDocument_impl::setPenBrush(QPen pen,QBrush brush,LineEnd lb,LineEnd le,int diffx,int diffy)
{
  KPObject *kpobject = 0;
  bool ret = false;
  
  for (int i = 0;i < static_cast<int>(objectList()->count());i++)
    {
      kpobject = objectList()->at(i);
      if (kpobject->isSelected())
	{
	  switch (kpobject->getType())
	    {
	    case OT_LINE:
	      {
		dynamic_cast<KPLineObject*>(kpobject)->setPen(pen);
		dynamic_cast<KPLineObject*>(kpobject)->setLineBegin(lb);
		dynamic_cast<KPLineObject*>(kpobject)->setLineEnd(le);
		ret = true;
		repaint(kpobject);
	      } break;
	    case OT_RECT:
	      {
		dynamic_cast<KPRectObject*>(kpobject)->setPen(pen);
		dynamic_cast<KPRectObject*>(kpobject)->setBrush(brush);
		ret = true;
		repaint(kpobject);
	      } break;
	    case OT_ELLIPSE:
	      {
		dynamic_cast<KPEllipseObject*>(kpobject)->setPen(pen);
		dynamic_cast<KPEllipseObject*>(kpobject)->setBrush(brush);
		ret = true;
		repaint(kpobject);
	      } break;
	    case OT_AUTOFORM:
	      {
		dynamic_cast<KPAutoformObject*>(kpobject)->setPen(pen);
		dynamic_cast<KPAutoformObject*>(kpobject)->setBrush(brush);
		dynamic_cast<KPAutoformObject*>(kpobject)->setLineBegin(lb);
		dynamic_cast<KPAutoformObject*>(kpobject)->setLineEnd(le);
		ret = true;
		repaint(kpobject);
	      } break;
	    default: break;
	    }
	}
    }

  return ret;
}

/*=================== get background type ========================*/
BackType KPresenterDocument_impl::getBackType(unsigned int pageNum)
{
  if (pageNum < _backgroundList.count())
    return backgroundList()->at(pageNum)->getBackType();
 
  return BT_COLOR;
}

/*=================== get background pic view ====================*/
BackView KPresenterDocument_impl::getBackView(unsigned int pageNum)
{
  if (pageNum < _backgroundList.count())
    return backgroundList()->at(pageNum)->getBackView();

  return BV_TILED;
}

/*=================== get background picture =====================*/
QString KPresenterDocument_impl::getBackPixFilename(unsigned int pageNum)
{
  if (pageNum < _backgroundList.count())
    return backgroundList()->at(pageNum)->getBackPixFilename();

  return 0;
}

/*=================== get background clipart =====================*/
QString KPresenterDocument_impl::getBackClipFilename(unsigned int pageNum)
{
  if (pageNum < _backgroundList.count())
    return backgroundList()->at(pageNum)->getBackClipFilename();

  return 0;
}

/*=================== get background color 1 ======================*/
QColor KPresenterDocument_impl::getBackColor1(unsigned int pageNum)
{
  if (pageNum < _backgroundList.count())
    return backgroundList()->at(pageNum)->getBackColor1();

  return white;
}

/*=================== get background color 2 ======================*/
QColor KPresenterDocument_impl::getBackColor2(unsigned int pageNum)
{
  if (pageNum < _backgroundList.count())
    return backgroundList()->at(pageNum)->getBackColor2();

  return white;
}

/*=================== get background color type ==================*/
BCType KPresenterDocument_impl::getBackColorType(unsigned int pageNum)
{
  if (pageNum < _backgroundList.count())
    return backgroundList()->at(pageNum)->getBackColorType();

  return BCT_PLAIN;
}

/*====================== get page effect =========================*/
PageEffect KPresenterDocument_impl::getPageEffect(unsigned int pageNum)
{
  if (pageNum < _backgroundList.count())
    return backgroundList()->at(pageNum)->getPageEffect();

  return PEF_NONE;
}

/*========================= get pen ==============================*/
QPen KPresenterDocument_impl::getPen(QPen pen)
{
  KPObject *kpobject = 0;
  
  for (int i = 0;i < static_cast<int>(objectList()->count());i++)
    {
      kpobject = objectList()->at(i);
      if (kpobject->isSelected())
	{
	  switch (kpobject->getType())
	    {
	    case OT_LINE:
	      return dynamic_cast<KPLineObject*>(kpobject)->getPen();
	      break;
	    case OT_RECT:
	      return dynamic_cast<KPRectObject*>(kpobject)->getPen();
	      break;
	    case OT_ELLIPSE:
	      return dynamic_cast<KPEllipseObject*>(kpobject)->getPen();
	      break;
	    case OT_AUTOFORM:
	      return dynamic_cast<KPAutoformObject*>(kpobject)->getPen();
	      break;
	    default: break;
	    }
	}      
    }

  return pen;
}

/*========================= get line begin ========================*/
LineEnd KPresenterDocument_impl::getLineBegin(LineEnd lb)
{
  KPObject *kpobject = 0;
  
  for (int i = 0;i < static_cast<int>(objectList()->count());i++)
    {
      kpobject = objectList()->at(i);
      if (kpobject->isSelected())
	{
	  switch (kpobject->getType())
	    {
	    case OT_LINE:
	      return dynamic_cast<KPLineObject*>(kpobject)->getLineBegin();
	      break;
	    case OT_AUTOFORM:
	      return dynamic_cast<KPAutoformObject*>(kpobject)->getLineBegin();
	      break;
	    default: break;
	    }
	}      
    }

  return lb;
}

/*========================= get line end =========================*/
LineEnd KPresenterDocument_impl::getLineEnd(LineEnd le)
{
  KPObject *kpobject = 0;
  
  for (int i = 0;i < static_cast<int>(objectList()->count());i++)
    {
      kpobject = objectList()->at(i);
      if (kpobject->isSelected())
	{
	  switch (kpobject->getType())
	    {
	    case OT_LINE:
	      return dynamic_cast<KPLineObject*>(kpobject)->getLineEnd();
	      break;
	    case OT_AUTOFORM:
	      return dynamic_cast<KPAutoformObject*>(kpobject)->getLineEnd();
	      break;
	    default: break;
	    }
	}      
    }

  return le;
}

/*========================= get brush =============================*/
QBrush KPresenterDocument_impl::getBrush(QBrush brush)
{
  KPObject *kpobject = 0;
  
  for (int i = 0;i < static_cast<int>(objectList()->count());i++)
    {
      kpobject = objectList()->at(i);
      if (kpobject->isSelected())
	{
	  switch (kpobject->getType())
	    {
	    case OT_RECT:
	      return dynamic_cast<KPRectObject*>(kpobject)->getBrush();
	      break;
	    case OT_ELLIPSE:
	      return dynamic_cast<KPEllipseObject*>(kpobject)->getBrush();
	      break;
	    case OT_AUTOFORM:
	      return dynamic_cast<KPAutoformObject*>(kpobject)->getBrush();
	      break;
	    default: break;
	    }
	}      
    }

  return brush;
}

/*======================== lower objects =========================*/
void KPresenterDocument_impl::lowerObjs(int diffx,int diffy)
{
  KPObject *kpobject = 0;
  
  for (int i = 0;i < static_cast<int>(objectList()->count());i++)
    {
      kpobject = objectList()->at(i);
      if (kpobject->isSelected())
	{
	  _objectList.take(i);
	  _objectList.insert(0,kpobject);
	  repaint(kpobject);
	}
    }      
}

/*========================= raise object =========================*/
void KPresenterDocument_impl::raiseObjs(int diffx,int diffy)
{
  KPObject *kpobject = 0;
  
  for (int i = 0;i < static_cast<int>(objectList()->count());i++)
    {
      kpobject = objectList()->at(i);
      if (kpobject->isSelected())
	{
	  _objectList.take(i);
	  _objectList.append(kpobject);
	  repaint(kpobject);
	}
    }      
}

/*=================== insert a picture ==========================*/
void KPresenterDocument_impl::insertPicture(QString filename,int diffx,int diffy)
{
  KPPixmapObject *kppixmapobject = new KPPixmapObject(&_pixmapCollection,filename);
  kppixmapobject->setOrig(diffx + 10,diffy + 10);
  kppixmapobject->setSelected(true);

  _objectList.append(kppixmapobject);

  repaint(kppixmapobject);
}

/*=================== insert a clipart ==========================*/
void KPresenterDocument_impl::insertClipart(QString filename,int diffx,int diffy)
{
  KPClipartObject *kpclipartobject = new KPClipartObject(filename);
  kpclipartobject->setOrig(diffx + 10,diffy + 10);
  kpclipartobject->setSize(150,150);
  kpclipartobject->setSelected(true);

  _objectList.append(kpclipartobject);

  repaint(kpclipartobject);
}

/*======================= change picture ========================*/
void KPresenterDocument_impl::changePicture(QString filename,int diffx,int diffy)
{
  KPObject *kpobject = 0;
  
  for (int i = 0;i < static_cast<int>(objectList()->count());i++)
    {
      kpobject = objectList()->at(i);
      if (kpobject->isSelected() && kpobject->getType() == OT_PICTURE)
	{
	  dynamic_cast<KPPixmapObject*>(kpobject)->setFileName(filename);
	  repaint(false);
	  break;
	}
    }
}

/*======================= change clipart ========================*/
void KPresenterDocument_impl::changeClipart(QString filename,int diffx,int diffy)
{
  KPObject *kpobject = 0;
  
  for (int i = 0;i < static_cast<int>(objectList()->count());i++)
    {
      kpobject = objectList()->at(i);
      if (kpobject->isSelected() && kpobject->getType() == OT_CLIPART)
	{
	  dynamic_cast<KPClipartObject*>(kpobject)->setFileName(filename);
	  repaint(false);
	  break;
	}
    }
}

/*===================== insert a line ===========================*/
void KPresenterDocument_impl::insertLine(QPen pen,LineEnd lb,LineEnd le,LineType lt,int diffx,int diffy)
{
  KPLineObject *kplineobject = new KPLineObject(pen,lb,le,lt);
  kplineobject->setOrig(diffx + 10,diffy + 10);
  kplineobject->setSize(150,150);
  kplineobject->setSelected(true);

  _objectList.append(kplineobject);

  repaint(kplineobject);
}

/*===================== insert a rectangle =======================*/
void KPresenterDocument_impl::insertRectangle(QPen pen,QBrush brush,RectType rt,int diffx,int diffy)
{
  KPRectObject *kprectobject = new KPRectObject(pen,brush,rt,getRndX(),getRndY());
  kprectobject->setOrig(diffx + 10,diffy + 10);
  kprectobject->setSize(150,150);
  kprectobject->setSelected(true);

  _objectList.append(kprectobject);

  repaint(kprectobject);
}

/*===================== insert a circle or ellipse ===============*/
void KPresenterDocument_impl::insertCircleOrEllipse(QPen pen,QBrush brush,int diffx,int diffy)
{
  KPEllipseObject *kpellipseobject = new KPEllipseObject(pen,brush);
  kpellipseobject->setOrig(diffx + 10,diffy + 10);
  kpellipseobject->setSize(150,150);
  kpellipseobject->setSelected(true);

  _objectList.append(kpellipseobject);

  repaint(kpellipseobject);
}

/*===================== insert a textobject =====================*/
void KPresenterDocument_impl::insertText(int diffx,int diffy)
{
  KPTextObject *kptextobject = new KPTextObject();
  kptextobject->setOrig(diffx + 10,diffy + 10);
  kptextobject->setSize(170,150);
  kptextobject->setSelected(true);

  _objectList.append(kptextobject);

  repaint(kptextobject);
}

/*======================= insert an autoform ====================*/
void KPresenterDocument_impl::insertAutoform(QPen pen,QBrush brush,LineEnd lb,LineEnd le,QString fileName,int diffx,int diffy)
{
  KPAutoformObject *kpautoformobject = new KPAutoformObject(pen,brush,fileName,lb,le);
  kpautoformobject->setOrig(diffx + 10,diffy + 10);
  kpautoformobject->setSize(150,150);
  kpautoformobject->setSelected(true);

  _objectList.append(kpautoformobject);

  repaint(kpautoformobject);
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

/*===================== repaint =================================*/
void KPresenterDocument_impl::repaint(QRect rect)
{
  if (!m_lstViews.isEmpty())
    {
      QRect r;

      for (viewPtr = m_lstViews.first();viewPtr != 0;viewPtr = m_lstViews.next())
	{
	  r = rect;
	  r.setX(r.x() - viewPtr->getDiffX());
	  r.setY(r.y() - viewPtr->getDiffY());
					
	  viewPtr->repaint(r,false);
	}
    }
}

/*===================== repaint =================================*/
void KPresenterDocument_impl::repaint(KPObject *kpobject)
{
  if (!m_lstViews.isEmpty())
    {
      QRect r;

      for (viewPtr = m_lstViews.first();viewPtr != 0;viewPtr = m_lstViews.next())
	{
	  r = kpobject->getBoundingRect(0,0);
	  r.setX(r.x() - viewPtr->getDiffX());
	  r.setY(r.y() - viewPtr->getDiffY());
					
	  viewPtr->repaint(r,false);
	}
    }
}

/*==================== reorder page =============================*/
QList<int> KPresenterDocument_impl::reorderPage(unsigned int num,int diffx,int diffy,float fakt = 1.0)
{
  QList<int> orderList;
  bool inserted;
  
  orderList.append((int*)(0));

  KPObject *kpobject = 0;
  
  for (int i = 0;i < static_cast<int>(objectList()->count());i++)
    {
      kpobject = objectList()->at(i);
      if (getPageOfObj(i,diffx,diffy,fakt) == static_cast<int>(num))
	{
	  if (orderList.find((int*)kpobject->getPresNum()) == -1)
	    {
	      if (orderList.isEmpty())
		orderList.append((int*)(kpobject->getPresNum()));
	      else
		{
		  inserted = false;
		  for (int j = orderList.count()-1;j >= 0;j--)
		    {
		      if ((int*)(kpobject->getPresNum()) > orderList.at(j))
			{
			  orderList.insert(j+1,(int*)(kpobject->getPresNum()));
			  j = -1;
			  inserted = true;
			}
		    }
		  if (!inserted) orderList.insert(0,(int*)(kpobject->getPresNum()));
		}
	    }
	}
    }
  
  return orderList;
}

/*====================== get page of object ======================*/
int KPresenterDocument_impl::getPageOfObj(int objNum,int diffx,int diffy,float fakt = 1.0)
{
  QRect rect;

  KPObject *kpobject = 0;
  
  for (int i = 0;i < static_cast<int>(objectList()->count());i++)
    {
      kpobject = objectList()->at(i);
      if (i == objNum)
	{
	  for (int j = 0;j < static_cast<int>(_backgroundList.count());j++)
	    {
	      rect = getPageSize(j,diffx,diffy,fakt);
	      rect.setWidth(QApplication::desktop()->width());
	      if (rect.intersects(kpobject->getBoundingRect(diffx,diffy)))
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

  int pw,ph,bl = static_cast<int>(_pageLayout.left * fact * 100) / 100;
  int br = static_cast<int>(_pageLayout.right * fact * 100) / 100;
  int bt = static_cast<int>(_pageLayout.top * fact * 100) / 100;
  int bb = static_cast<int>(_pageLayout.bottom * fact * 100) / 100;
  int wid = static_cast<int>(_pageLayout.width * fact * 100) / 100;
  int hei = static_cast<int>(_pageLayout.height * fact * 100) / 100;
  
  pw = wid * static_cast<int>(MM_TO_POINT * 100) / 100 - 
    (bl + br) * static_cast<int>(MM_TO_POINT * 100) / 100;
  ph = hei * static_cast<int>(MM_TO_POINT * 100) / 100 -
    (bt + bb) * static_cast<int>(MM_TO_POINT * 100) / 100;

  pw = static_cast<int>(static_cast<float>(pw) * fakt);
  ph = static_cast<int>(static_cast<float>(ph) * fakt);

  QRect rect(10 - diffx,(10 + ph * num +
			 num * 10) - diffy,pw,ph);
  return rect;
}

/*================ return number of selected objs ================*/
int KPresenterDocument_impl::numSelected()
{
  int num = 0;

  KPObject *kpobject = 0;
  
  for (int i = 0;i < static_cast<int>(objectList()->count());i++)
    {
      kpobject = objectList()->at(i);
      if (kpobject->isSelected()) num++;
    }

  return num;
}

/*==================== return selected obj ======================*/
KPObject* KPresenterDocument_impl::getSelectedObj()
{
  KPObject *kpobject = 0;
  
  for (int i = 0;i < static_cast<int>(objectList()->count());i++)
    {
      kpobject = objectList()->at(i);
      if (kpobject->isSelected()) return kpobject;
    }

  return 0;
}

/*======================= delete objects =========================*/
void KPresenterDocument_impl::deleteObjs()
{
  bool changed = false;
  KPObject *kpobject = 0;
  
  for (int i = 0;i < static_cast<int>(objectList()->count());i++)
    {
      kpobject = objectList()->at(i);
      if (kpobject->isSelected())
	{
	  _objectList.remove(i--);
	  changed = true;
	}
    }
 
  if (changed) repaint(false);
}

/*========================== copy objects ========================*/
void KPresenterDocument_impl::copyObjs(int diffx,int diffy)
{
//   QClipboard *cb = QApplication::clipboard();
//   QString clipStr = "";
//   char str[255];
  
//   clipStr += "[KPRESENTER-DATA]";

//   if (!_objList.isEmpty())
//     {
//       for (unsigned int i=0;i < _objList.count();i++)
// 	{
// 	  objPtr = _objList.at(i);
// 	  if (objPtr->isSelected)
// 	    {
// 	      if (objPtr->objType == OT_TEXT)
// 		debug("At the moment text can't be copied to the clipboard. SORRY!");
// 	      else
// 		{
// 		  clipStr += "[NEW_OBJECT_START]";
		  
// 		  clipStr += "[OBJ_TYPE]{";
// 		  sprintf(str,"%d",(int)objPtr->objType);
// 		  clipStr += str;
// 		  clipStr += "}";
		  
// 		  clipStr += "[OBJ_X]{";
// 		  sprintf(str,"%d",objPtr->ox - diffx);
// 		  clipStr += str;
// 		  clipStr += "}";
		  
// 		  clipStr += "[OBJ_Y]{";
// 		  sprintf(str,"%d",objPtr->oy - diffy);
// 		  clipStr += str;
// 		  clipStr += "}";
		  
// 		  clipStr += "[OBJ_W]{";
// 		  sprintf(str,"%d",objPtr->ow);
// 		  clipStr += str;
// 		  clipStr += "}";
		  
// 		  clipStr += "[OBJ_H]{";
// 		  sprintf(str,"%d",objPtr->oh);
// 		  clipStr += str;
// 		  clipStr += "}";
		  
// 		  clipStr += "[OBJ_PRESNUM]{";
// 		  sprintf(str,"%d",objPtr->presNum);
// 		  clipStr += str;
// 		  clipStr += "}";
		  
// 		  clipStr += "[OBJ_EFFECT]{";
// 		  sprintf(str,"%d",(int)objPtr->effect);
// 		  clipStr += str;
// 		  clipStr += "}";

// 		  clipStr += "[OBJ_EFFECT2]{";
// 		  sprintf(str,"%d",(int)objPtr->effect2);
// 		  clipStr += str;
// 		  clipStr += "}";

// 		  clipStr += "[GRAPHOBJ]";
		  
// 		  clipStr += "[LINE_BEGIN]{";
// 		  sprintf(str,"%d",(int)objPtr->graphObj->getLineBegin());
// 		  clipStr += str;
// 		  clipStr += "}";
		  
// 		  clipStr += "[LINE_END]{";
// 		  sprintf(str,"%d",(int)objPtr->graphObj->getLineEnd());
// 		  clipStr += str;
// 		  clipStr += "}";

// 		  clipStr += "[LINE_TYPE]{";
// 		  sprintf(str,"%d",(int)objPtr->graphObj->getLineType());
// 		  clipStr += str;
// 		  clipStr += "}";

// 		  clipStr += "[RECT_TYPE]{";
// 		  sprintf(str,"%d",(int)objPtr->graphObj->getRectType());
// 		  clipStr += str;
// 		  clipStr += "}";

// 		  clipStr += "[PEN_WIDTH]{";
// 		  sprintf(str,"%d",(int)objPtr->graphObj->getObjPen().width());
// 		  clipStr += str;
// 		  clipStr += "}";

// 		  clipStr += "[PEN_STYLE]{";
// 		  sprintf(str,"%d",(int)objPtr->graphObj->getObjPen().style());
// 		  clipStr += str;
// 		  clipStr += "}";

// 		  clipStr += "[PEN_RED]{";
// 		  sprintf(str,"%d",objPtr->graphObj->getObjPen().color().red());
// 		  clipStr += str;
// 		  clipStr += "}";

// 		  clipStr += "[PEN_GREEN]{";
// 		  sprintf(str,"%d",objPtr->graphObj->getObjPen().color().green());
// 		  clipStr += str;
// 		  clipStr += "}";

// 		  clipStr += "[PEN_BLUE]{";
// 		  sprintf(str,"%d",objPtr->graphObj->getObjPen().color().blue());
// 		  clipStr += str;
// 		  clipStr += "}";

// 		  clipStr += "[BRUSH_STYLE]{";
// 		  sprintf(str,"%d",(int)objPtr->graphObj->getObjBrush().style());
// 		  clipStr += str;
// 		  clipStr += "}";

// 		  clipStr += "[BRUSH_RED]{";
// 		  sprintf(str,"%d",objPtr->graphObj->getObjBrush().color().red());
// 		  clipStr += str;
// 		  clipStr += "}";

// 		  clipStr += "[BRUSH_GREEN]{";
// 		  sprintf(str,"%d",objPtr->graphObj->getObjBrush().color().green());
// 		  clipStr += str;
// 		  clipStr += "}";

// 		  clipStr += "[BRUSH_BLUE]{";
// 		  sprintf(str,"%d",objPtr->graphObj->getObjBrush().color().blue());
// 		  clipStr += str;
// 		  clipStr += "}";

// 		  clipStr += "[FILENAME]{";
// 		  sprintf(str,"%s",(const char*)objPtr->graphObj->getFileName());
// 		  clipStr += str;
// 		  clipStr += "}";

// 		  clipStr += "[RND_X]{";
// 		  sprintf(str,"%d",objPtr->graphObj->getRndX());
// 		  clipStr += str;
// 		  clipStr += "}";

// 		  clipStr += "[RND_Y]{";
// 		  sprintf(str,"%d",objPtr->graphObj->getRndY());
// 		  clipStr += str;
// 		  clipStr += "}";

// 		  clipStr += "[ANGLE]{";
// 		  sprintf(str,"%f",objPtr->angle);
// 		  clipStr += str;
// 		  clipStr += "}";

// 		  clipStr += "[SHADOW_DIRECTION]{";
// 		  sprintf(str,"%d",(int)objPtr->shadowDirection);
// 		  clipStr += str;
// 		  clipStr += "}";

// 		  clipStr += "[SHADOW_DISTANCE]{";
// 		  sprintf(str,"%d",objPtr->shadowDistance);
// 		  clipStr += str;
// 		  clipStr += "}";

// 		  clipStr += "[SHADOW_RED]{";
// 		  sprintf(str,"%d",objPtr->shadowColor.red());
// 		  clipStr += str;
// 		  clipStr += "}";

// 		  clipStr += "[SHADOW_GREEN]{";
// 		  sprintf(str,"%d",objPtr->shadowColor.green());
// 		  clipStr += str;
// 		  clipStr += "}";

// 		  clipStr += "[SHADOW_BLUE]{";
// 		  sprintf(str,"%d",objPtr->shadowColor.blue());
// 		  clipStr += str;
// 		  clipStr += "}";

// 		  clipStr += "[NEW_OBJECT_END]";
// 		}
// 	    }
// 	}
//     }
  
//   cb->setText((const char*)clipStr);

}

/*========================= paste objects ========================*/
void KPresenterDocument_impl::pasteObjs(int diffx,int diffy)
{
//   QClipboard *cb = QApplication::clipboard();
//   QString clipStr = cb->text(),tag,value;
//   bool tagStarted = false,valueStarted = false;
//   QPen pen;
//   QBrush brush;
//   QColor color;
  
//   objPtr = 0;


//   if (!clipStr.isEmpty() && clipStr.left(strlen("[KPRESENTER-DATA]")) == "[KPRESENTER-DATA]")
//     {
//       for (unsigned int i = 0;i < clipStr.length();i++)
// 	{
	  
// 	  // start tag
// 	  if (clipStr.mid(i,1) == "[")
// 	    {
// 	      tagStarted = true;
// 	      tag = "";
// 	    }

// 	  // end tag
// 	  else if (clipStr.mid(i,1) == "]")
// 	    {
// 	      if (tagStarted)
// 		{
// 		  tagStarted = false;
// 		  if (tag == "NEW_OBJECT_START")
// 		    {
// 		      objPtr = new PageObjects;
// 		      objPtr->isSelected = true;
// 		      objPtr->textObj = 0;
// 		    }
// 		  else if (tag == "NEW_OBJECT_END")
// 		    {
// 		      //_objNums++;
// 		      //objPtr->objNum = _objNums;
// 		      objPtr->graphObj->hide();
// 		      _objList.append(objPtr);
// 		      //repaint(objPtr->ox,objPtr->oy,
// 		      //      objPtr->ow,objPtr->oh,_objNums,false);
// 		      objPtr = 0;
// 		    }
// 		  else if (tag == "GRAPHOBJ")
// 		    {
// 		      objPtr->graphObj = new GraphObj(0,"graphObj",objPtr->objType,"");
// 		      objPtr->graphObj->resize(objPtr->ow,objPtr->oh);
// 		    }
// 		}
// 	    }

// 	  // start value
// 	  else if (clipStr.mid(i,1) == "{")
// 	    {
// 	      valueStarted = true;
// 	      value = "";
// 	    }

// 	  // end value
// 	  else if (clipStr.mid(i,1) == "}")
// 	    {
// 	      if (valueStarted)
// 		{
// 		  valueStarted = false;
// 		  if (tag == "OBJ_TYPE" && objPtr)
// 		    objPtr->objType = (ObjType)atoi(value);
// 		  else if (tag == "OBJ_X" && objPtr)
// 		    objPtr->ox = atoi(value) + diffx;
// 		  else if (tag == "ANGLE" && objPtr)
// 		    objPtr->angle = atof(value);
// 		  else if (tag == "SHADOW_DIRECTION" && objPtr)
// 		    objPtr->shadowDirection = (ShadowDirection)atoi(value);
// 		  else if (tag == "SHADOW_DISTANCE" && objPtr)
// 		    objPtr->shadowDistance = atoi(value);
// 		  else if (tag == "SHADOW_RED" && objPtr)
// 		    {
// 		      color = objPtr->shadowColor;
// 		      color.setRgb(atoi(value),color.green(),color.blue());
// 		      objPtr->shadowColor = color;
// 		    }
// 		  else if (tag == "SHADOW_GREEN" && objPtr)
// 		    {
// 		      color = objPtr->shadowColor;
// 		      color.setRgb(color.red(),atoi(value),color.blue());
// 		      objPtr->shadowColor = color;
// 		    }
// 		  else if (tag == "SHADOW_BLUE" && objPtr)
// 		    {
// 		      color = objPtr->shadowColor;
// 		      color.setRgb(color.red(),color.green(),atoi(value));
// 		      objPtr->shadowColor = color;
// 		    }
// 		  else if (tag == "OBJ_Y" && objPtr)
// 		    objPtr->oy = atoi(value) + diffy;
// 		  else if (tag == "OBJ_W" && objPtr)
// 		    objPtr->ow = atoi(value);
// 		  else if (tag == "OBJ_H" && objPtr)
// 		    objPtr->oh = atoi(value);
// 		  else if (tag == "OBJ_PRESNUM" && objPtr)
// 		    objPtr->presNum = atoi(value);
// 		  else if (tag == "OBJ_EFFECT" && objPtr)
// 		    objPtr->effect = (Effect)atoi(value);
// 		  else if (tag == "OBJ_EFFECT2" && objPtr)
// 		    objPtr->effect2 = (Effect2)atoi(value);
// 		  else if (tag == "LINE_TYPE" && objPtr && objPtr->graphObj)
// 		    objPtr->graphObj->setLineType((LineType)atoi(value));
// 		  else if (tag == "LINE_BEGIN" && objPtr && objPtr->graphObj)
// 		    objPtr->graphObj->setLineBegin((LineEnd)atoi(value));
// 		  else if (tag == "LINE_END" && objPtr && objPtr->graphObj)
// 		    objPtr->graphObj->setLineEnd((LineEnd)atoi(value));
// 		  else if (tag == "RECT_TYPE" && objPtr && objPtr->graphObj)
// 		    objPtr->graphObj->setRectType((RectType)atoi(value));
// 		  else if (tag == "PEN_WIDTH" && objPtr && objPtr->graphObj)
// 		    {
// 		      pen = objPtr->graphObj->getObjPen();
// 		      pen.setWidth(atoi(value));
// 		      objPtr->graphObj->setObjPen(pen);
// 		    }
// 		  else if (tag == "PEN_STYLE" && objPtr && objPtr->graphObj)
// 		    {
// 		      pen = objPtr->graphObj->getObjPen();
// 		      pen.setStyle((PenStyle)atoi(value));
// 		      objPtr->graphObj->setObjPen(pen);
// 		    }
// 		  else if (tag == "PEN_RED" && objPtr && objPtr->graphObj)
// 		    {
// 		      pen = objPtr->graphObj->getObjPen();
// 		      color = pen.color();
// 		      color.setRgb(atoi(value),color.green(),color.blue());
// 		      pen.setColor(color);
// 		      objPtr->graphObj->setObjPen(pen);
// 		    }
// 		  else if (tag == "PEN_GREEN" && objPtr && objPtr->graphObj)
// 		    {
// 		      pen = objPtr->graphObj->getObjPen();
// 		      color = pen.color();
// 		      color.setRgb(color.red(),atoi(value),color.blue());
// 		      pen.setColor(color);
// 		      objPtr->graphObj->setObjPen(pen);
// 		    }
// 		  else if (tag == "PEN_BLUE" && objPtr && objPtr->graphObj)
// 		    {
// 		      pen = objPtr->graphObj->getObjPen();
// 		      color = pen.color();
// 		      color.setRgb(color.red(),color.green(),atoi(value));
// 		      pen.setColor(color);
// 		      objPtr->graphObj->setObjPen(pen);
// 		    }
// 		  else if (tag == "BRUSH_STYLE" && objPtr && objPtr->graphObj)
// 		    {
// 		      brush = objPtr->graphObj->getObjBrush();
// 		      brush.setStyle((BrushStyle)atoi(value));
// 		      objPtr->graphObj->setObjBrush(brush);
// 		    }
// 		  else if (tag == "BRUSH_RED" && objPtr && objPtr->graphObj)
// 		    {
// 		      brush = objPtr->graphObj->getObjBrush();
// 		      color = brush.color();
// 		      color.setRgb(atoi(value),color.green(),color.blue());
// 		      brush.setColor(color);
// 		      objPtr->graphObj->setObjBrush(brush);
// 		    }
// 		  else if (tag == "BRUSH_GREEN" && objPtr && objPtr->graphObj)
// 		    {
// 		      brush = objPtr->graphObj->getObjBrush();
// 		      color = brush.color();
// 		      color.setRgb(color.red(),atoi(value),color.blue());
// 		      brush.setColor(color);
// 		      objPtr->graphObj->setObjBrush(brush);
// 		    }
// 		  else if (tag == "BRUSH_BLUE" && objPtr && objPtr->graphObj)
// 		    {
// 		      brush = objPtr->graphObj->getObjBrush();
// 		      color = brush.color();
// 		      color.setRgb(color.red(),color.green(),atoi(value));
// 		      brush.setColor(color);
// 		      objPtr->graphObj->setObjBrush(brush);
// 		    }
// 		  else if (tag == "FILENAME" && objPtr && objPtr->graphObj)
// 		    objPtr->graphObj->setFileName(value);
// 		  else if (tag == "RND_X" && objPtr && objPtr->graphObj)
// 		    objPtr->graphObj->setRnds(atoi(value),objPtr->graphObj->getRndY());
// 		  else if (tag == "RND_Y" && objPtr && objPtr->graphObj)
// 		    objPtr->graphObj->setRnds(objPtr->graphObj->getRndX(),atoi(value));
// 		}
// 	    }
	  
// 	  // get char
// 	  else
// 	    {
// 	      if (valueStarted)
// 		value += clipStr.mid(i,1);
// 	      else if (tagStarted)
// 		tag += clipStr.mid(i,1);
// 	    }
// 	}
//     }
}

/*====================== replace objects =========================*/
void KPresenterDocument_impl::replaceObjs()
{
  KPObject *kpobject = 0;
  int ox,oy;
  
  for (int i = 0;i < static_cast<int>(objectList()->count());i++)
    {
      kpobject = objectList()->at(i);
      ox = kpobject->getOrig().x();
      oy = kpobject->getOrig().y();

      ox = (ox / _rastX) * _rastX;
      oy = (oy / _rastY) * _rastY;
      kpobject->setOrig(ox,oy);

      if (kpobject->getType() == OT_RECT && dynamic_cast<KPRectObject*>(kpobject)->getRectType() == RT_ROUND)
	dynamic_cast<KPRectObject*>(kpobject)->setRnds(_xRnd,_yRnd);
    }
}

/*========================= restore background ==================*/
void KPresenterDocument_impl::restoreBackground(int pageNum)
{
  if (pageNum < static_cast<int>(_backgroundList.count()))
    backgroundList()->at(pageNum)->restore();
}

