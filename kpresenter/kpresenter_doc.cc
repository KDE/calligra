/******************************************************************/
/* KPresenter - (c) by Reginald Stadlbauer 1998                   */
/* Version: 0.1.0                                                 */
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
KPresenterChild::KPresenterChild(KPresenterDoc *_kpr, const KRect& _rect,KOffice::Document_ptr _doc,
				 int _diffx,int _diffy)
  : KoDocumentChild(_rect,_doc)
{
  m_pKPresenterDoc = _kpr;
  m_rDoc = KOffice::Document::_duplicate(_doc);
  m_geometry = _rect;
  __geometry = KRect(_rect.left() + _diffx,_rect.top() + _diffy,_rect.right(),_rect.bottom());
}

/*====================== constructor =============================*/
KPresenterChild::KPresenterChild( KPresenterDoc *_kpr ) :
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
/* class KPresenterDoc                                  */
/******************************************************************/

/*====================== constructor =============================*/
KPresenterDoc::KPresenterDoc()
  : _pixmapCollection(), _gradientCollection(), _commands()
{
  ADD_INTERFACE("IDL:OPParts/Print:1.0")
  // Use CORBA mechanism for deleting views
  m_lstViews.setAutoDelete(false);
  m_lstChildren.setAutoDelete(true);

  m_bModified = false;

  // init
  _clean = true;
  _objectList = new QList<KPObject>;
  _objectList->setAutoDelete(false);
  _backgroundList.setAutoDelete(true);
  _spInfinitLoop = false;
  _spManualSwitch = true;
  _rastX = 10;
  _rastY = 10;
  _xRnd = 20;
  _yRnd = 20;
  _orastX = 10;
  _orastY = 10;
  _txtBackCol = lightGray;
  _otxtBackCol = lightGray;
  _pageLayout.format = PG_SCREEN;
  _pageLayout.orientation = PG_PORTRAIT;
  _pageLayout.width = PG_SCREEN_WIDTH;
  _pageLayout.height = PG_SCREEN_HEIGHT;
  _pageLayout.left = 0;
  _pageLayout.right = 0;
  _pageLayout.top = 0;
  _pageLayout.bottom = 0;
  _pageLayout.ptWidth = cMM_TO_POINT(PG_SCREEN_WIDTH);
  _pageLayout.ptHeight = cMM_TO_POINT(PG_SCREEN_HEIGHT);
  _pageLayout.ptLeft = 0;
  _pageLayout.ptRight = 0;
  _pageLayout.ptTop = 0;
  _pageLayout.ptBottom = 0;
  _pageLayout.unit = PG_MM;
  objStartY = 0;
  setPageLayout(_pageLayout,0,0);
  _presPen = QPen(red,3,SolidLine);
  presSpeed = PS_NORMAL;
  pasting = false;
  pasteXOffset = pasteYOffset = 0;

  QObject::connect(&_commands,SIGNAL(undoRedoChanged(QString,QString)),this,SLOT(slotUndoRedoChanged(QString,QString)));
}

/*====================== destructor ==============================*/
KPresenterDoc::~KPresenterDoc()
{
  sdeb("KPresenterDoc::~KPresenterDoc()\n");

  _objectList->clear();
  delete _objectList;
  _backgroundList.clear();
  cleanUp();
  edeb("...KPresenterDoc::~KPresenterDoc() %i\n",_refcnt());
}

/*======================== draw contents as QPicture =============*/
void KPresenterDoc::draw(QPaintDevice* _dev,CORBA::Long _width,CORBA::Long _height)
{
  warning("***********************************************");
  warning(i18n("KPresenter doesn't support KoDocument::draw(...) now!"));
  warning("***********************************************");

  return;
  
  if (m_lstViews.count() > 0)
    {
      QPainter painter;
      painter.begin(_dev);
     
      m_lstViews.at(0)->getPage()->draw(KRect(0,0,_width,_height),&painter);
      
      painter.end();
    }
}

/*======================= clean up ===============================*/
void KPresenterDoc::cleanUp()
{
  if (m_bIsClean) return;

  assert(m_lstViews.count() == 0);
  
  m_lstChildren.clear();

  KoDocument::cleanUp();
}

/*========================== save ===============================*/
bool KPresenterDoc::hasToWriteMultipart()
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
void KPresenterDoc::makeChildListIntern(KOffice::Document_ptr _doc,const char *_path)
{
  int i = 0;
  
  QListIterator<KPresenterChild> it(m_lstChildren);
  for(;it.current();++it)
    {
      QString tmp;
      tmp.sprintf("/%i",i++);
      QString path(_path);
      path += tmp.data();
      
      KOffice::Document_var doc = it.current()->document();    
      doc->makeChildList(_doc,path);
    }
}

/*========================== save ===============================*/
bool KPresenterDoc::save( ostream& out, const char* /* format */ )
{
  out << "<?xml version=\"1.0\"?>" << endl;
  out << otag << "<DOC author=\"" << "Reginald Stadlbauer" << "\" email=\"" << "reggie@kde.org" << "\" editor=\"" << "KPresenter"
      << "\" mime=\"" << "application/x-kpresenter" << "\">" << endl;
  
  out << otag << "<PAPER format=\"" << static_cast<int>(pageLayout().format) << "\" width=\"" << pageLayout().width
      << "\" height=\"" << pageLayout().height << "\" orientation=\"" << static_cast<int>(pageLayout().orientation) << "\">" << endl;
  out << indent << "<PAPERBORDERS left=\"" << pageLayout().left << "\" top=\"" << pageLayout().top << "\" right=\"" << pageLayout().right
      << "\" bottom=\"" << pageLayout().bottom << "\"/>" << endl;
  out << etag << "</PAPER>" << endl;
  
  out << otag << "<BACKGROUND" << " rastX=\"" << _rastX << "\" rastY=\""
      << "\" bred=\"" << _txtBackCol.red() << "\" bgreen=\"" << _txtBackCol.green() << "\" bblue=\"" << _txtBackCol.blue() << "\">" << endl;
  saveBackground(out);
  out << etag << "</BACKGROUND>" << endl;

  out << otag << "<OBJECTS>" << endl;
  saveObjects(out);
  out << etag << "</OBJECTS>" << endl;

  out << indent << "<INFINITLOOP value=\"" << _spInfinitLoop << "\"/>" << endl; 
  out << indent << "<MANUALSWITCH value=\"" << _spManualSwitch << "\"/>" << endl; 
  out << indent << "<PRESSPEED value=\"" << static_cast<int>(presSpeed) << "\"/>" << endl; 

  // Write "OBJECT" tag for every child
  QListIterator<KPresenterChild> chl(m_lstChildren);
  for(;chl.current();++chl)
    chl.current()->save( out );

  out << etag << "</DOC>" << endl;
    
  setModified(false);
    
  return true;
}

/*====================== export HTML ============================*/
bool KPresenterDoc::exportHTML(QString _filename)
{
  QMessageBox::critical((QWidget*)0L,i18n("KPresenter Error"),i18n("HTML export is not implemented!"),i18n("OK"));
  return true;
}

/*========================== save background ====================*/
void KPresenterDoc::saveBackground(ostream& out)
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
void KPresenterDoc::saveObjects(ostream& out)
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
bool KPresenterDoc::loadChildren( KOStore::Store_ptr _store )
{
  QListIterator<KPresenterChild> it(m_lstChildren);
  for(;it.current();++it)
    {
      if (!it.current()->loadDocument( _store, it.current()->mimeType() ) )
	return false;
    }
  
  return true;
}

/*========================= load a template =====================*/
bool KPresenterDoc::load_template(const char *_url)
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
  
  if ( !loadXML( parser, 0L ) )
    return false;
 
  m_bModified = true;
  return true;
}

/*========================== load ===============================*/
bool KPresenterDoc::loadXML( KOMLParser& parser, KOStore::Store_ptr _store )
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
      delete _objectList;
      _objectList = new QList<KPObject>;
      _objectList->setAutoDelete(false);
      _spInfinitLoop = false;
      _spManualSwitch = true;
      _rastX = 20;
      _rastY = 20;
      _xRnd = 20;
      _yRnd = 20;
      _txtBackCol = white;
    }

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
	  if ((*it).m_strValue != "application/x-kpresenter")
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
		{
		  __pgLayout.width = static_cast<double>(atof((*it).m_strValue.c_str()));
		  __pgLayout.ptWidth = cMM_TO_POINT(static_cast<double>(atof((*it).m_strValue.c_str())));
		}	      
	      else if ((*it).m_strName == "height")
		{
		  __pgLayout.height = static_cast<double>(atof((*it).m_strValue.c_str()));
		  __pgLayout.ptHeight = cMM_TO_POINT(static_cast<double>(atof((*it).m_strValue.c_str())));
		}	      
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
			  __pgLayout.ptLeft = cMM_TO_POINT((double)atof((*it).m_strValue.c_str()));
			}		      
		      else if ((*it).m_strName == "top")
			{
			  __pgLayout.top = (double)atof((*it).m_strValue.c_str());
			  __pgLayout.ptTop = cMM_TO_POINT((double)atof((*it).m_strValue.c_str()));
			}		      
		      else if ((*it).m_strName == "right")
			{
			  __pgLayout.right = (double)atof((*it).m_strValue.c_str());
			  __pgLayout.ptRight = cMM_TO_POINT((double)atof((*it).m_strValue.c_str()));
			}		      
		      else if ((*it).m_strName == "bottom")
			{
			  __pgLayout.bottom = (double)atof((*it).m_strValue.c_str());
			  __pgLayout.ptBottom = cMM_TO_POINT((double)atof((*it).m_strValue.c_str()));
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
		_spInfinitLoop = static_cast<bool>(atoi((*it).m_strValue.c_str()));
	    }
	}

      else if (name == "PRESSPEED")
	{
	  KOMLParser::parseTag(tag.c_str(),name,lst);
	  vector<KOMLAttrib>::const_iterator it = lst.begin();
	  for(;it != lst.end();it++)
	    {
	      if ((*it).m_strName == "value")
		presSpeed = static_cast<PresSpeed>(atoi((*it).m_strValue.c_str()));
	    }
	}

      else if (name == "MANUALSWITCH")
	{
	  KOMLParser::parseTag(tag.c_str(),name,lst);
	  vector<KOMLAttrib>::const_iterator it = lst.begin();
	  for(;it != lst.end();it++)
	    {
	      if ((*it).m_strName == "value")
		_spManualSwitch = static_cast<bool>(atoi((*it).m_strValue.c_str()));
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
void KPresenterDoc::loadBackground(KOMLParser& parser,vector<KOMLAttrib>& lst)
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
void KPresenterDoc::loadObjects(KOMLParser& parser,vector<KOMLAttrib>& lst)
{
  string tag;
  string name;
  ObjType t = OT_LINE;

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
		t = (ObjType)atoi((*it).m_strValue.c_str());
	    }

	  switch (t)
	    {
	    case OT_LINE:
	      {
		KPLineObject *kplineobject = new KPLineObject();
		kplineobject->load(parser,lst);
		_objectList->append(kplineobject);
	      } break;
	    case OT_RECT:
	      {
		KPRectObject *kprectobject = new KPRectObject();
		kprectobject->setRnds(_xRnd,_yRnd);
		kprectobject->load(parser,lst);
		_objectList->append(kprectobject);
	      } break;
	    case OT_ELLIPSE:
	      {
		KPEllipseObject *kpellipseobject = new KPEllipseObject();
		kpellipseobject->load(parser,lst);
		_objectList->append(kpellipseobject);
	      } break;
	    case OT_PIE:
	      {
		KPPieObject *kppieobject = new KPPieObject();
		kppieobject->load(parser,lst);
		_objectList->append(kppieobject);
	      } break;
	    case OT_AUTOFORM:
	      {
		KPAutoformObject *kpautoformobject = new KPAutoformObject();
		kpautoformobject->load(parser,lst);
		_objectList->append(kpautoformobject);
	      } break;
	    case OT_CLIPART:
	      {
		KPClipartObject *kpclipartobject = new KPClipartObject();
		kpclipartobject->load(parser,lst);
		_objectList->append(kpclipartobject);
	      } break;
	    case OT_TEXT:
	      {
		KPTextObject *kptextobject = new KPTextObject();
		kptextobject->load(parser,lst);
		_objectList->append(kptextobject);
	      } break;
	    case OT_PICTURE:
	      {
		KPPixmapObject *kppixmapobject = new KPPixmapObject(&_pixmapCollection);
		kppixmapobject->load(parser,lst);
		_objectList->append(kppixmapobject);
	      } break;
	    default: break;
	    }
	  
	  if (objStartY > 0) _objectList->last()->moveBy(0,objStartY);
	  if (pasting) 
	    {
	      _objectList->last()->moveBy(pasteXOffset,pasteYOffset);
	      _objectList->last()->setSelected(true);
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
KPresenterView* KPresenterDoc::createPresenterView()
{
  KPresenterView *p = new KPresenterView( 0L, 0L, this );
  p->QWidget::show();
  m_lstViews.append( p );
  
  return p;
}

/*========================= create a view ========================*/
OpenParts::View_ptr KPresenterDoc::createView()
{
  return OpenParts::View::_duplicate( createPresenterView() );
}

/*========================== view list ===========================*/
void KPresenterDoc::viewList(KOffice::Document::ViewList*& _list)
{
  (*_list).length(m_lstViews.count());

  int i = 0;
  QListIterator<KPresenterView> it(m_lstViews);
  for(;it.current();++it)
    (*_list)[i++] = OpenParts::View::_duplicate(it.current());
}

/*========================== output formats ======================*/
QStrList KPresenterDoc::outputFormats()
{
  return new QStrList();
}

/*========================== input formats =======================*/
QStrList KPresenterDoc::inputFormats()
{
  return new QStrList();
}

/*========================= add view =============================*/
void KPresenterDoc::addView(KPresenterView *_view)
{
  m_lstViews.append(_view);
}

/*======================== remove view ===========================*/
void KPresenterDoc::removeView(KPresenterView *_view)
{
  m_lstViews.setAutoDelete(false);
  m_lstViews.removeRef(_view);
  m_lstViews.setAutoDelete(true);
}

/*========================= insert an object =====================*/
void KPresenterDoc::insertObject(const KRect& _rect, const char* _server_name,int _diffx,int _diffy)
{
  KOffice::Document_var doc = imr_createDocByServerName( _server_name);
  if (CORBA::is_nil(doc))
    return;
  
  if (!doc->init())
    {
      QMessageBox::critical((QWidget*)0L,i18n("KPresenter Error"),i18n("Could not init"),i18n("OK"));
      return;
    }

  KPresenterChild* ch = new KPresenterChild(this,_rect,doc,_diffx,_diffy);

  insertChild( ch );
  m_bModified = true;
}

/*========================= insert a child object =====================*/
void KPresenterDoc::insertChild(KPresenterChild *_child)
{
  m_lstChildren.append(_child);
  
  emit sig_insertObject(_child);
  m_bModified = true;
}

/*======================= change child geometry ==================*/
void KPresenterDoc::changeChildGeometry(KPresenterChild *_child,const KRect& _rect,int _diffx,int _diffy)
{
  _child->setGeometry(_rect);
  _child->_setGeometry(KRect(_rect.left() + _diffx,_rect.top() + _diffy,_rect.right(),_rect.bottom()));

  emit sig_updateChildGeometry(_child);

  m_bModified = true;
}

/*======================= child iterator =========================*/
QListIterator<KPresenterChild> KPresenterDoc::childIterator()
{
  return QListIterator<KPresenterChild> (m_lstChildren);
}

/*===================== set page layout ==========================*/
void KPresenterDoc::setPageLayout(KoPageLayout pgLayout,int diffx,int diffy)
{
  _pageLayout = pgLayout;
  KRect r = getPageSize(0,diffx,diffy);

  for (int i = 0;i < static_cast<int>(_backgroundList.count());i++)
    {
      _backgroundList.at(i)->setSize(r.width(),r.height());
      _backgroundList.at(i)->restore();
    }

  m_bModified = true;
  repaint(false);
}

/*==================== insert a new page =========================*/
unsigned int KPresenterDoc::insertNewPage(int diffx,int diffy,bool _restore=true)
{

  KPBackGround *kpbackground = new KPBackGround(&_pixmapCollection,&_gradientCollection);
  _backgroundList.append(kpbackground);

  if (_restore)
    {
      KRect r = getPageSize(0,diffx,diffy);
      _backgroundList.last()->setSize(r.width(),r.height());
      _backgroundList.last()->restore();
      repaint(false);
    }

  m_bModified = true;
  return getPageNums();
}

/*==================== insert a new page with template ===========*/
bool KPresenterDoc::insertNewTemplate(int diffx,int diffy,bool clean=false)
{
  QString templateDir = KApplication::kde_datadir();

  QString _template;
  QString _templatePath = kapp->kde_datadir() + "/kpresenter/templates/";

  if (KoTemplateChooseDia::chooseTemplate(_templatePath,_template,true))
    {
      QFileInfo fileInfo(_template);
      QString fileName(_templatePath + fileInfo.dirPath(false) + "/" + fileInfo.baseName() + ".kpt");
      _clean = clean;
      objStartY = getPageSize(_backgroundList.count() - 1,0,0).y() + getPageSize(_backgroundList.count() - 1,0,0).height();
      load_template(fileName.data());
      objStartY = 0;
      _clean = true;
      m_bModified = true;
      return true;
    }
  else
    return false;
}

/*==================== set background color ======================*/
void KPresenterDoc::setBackColor(unsigned int pageNum,QColor backColor1,QColor backColor2,BCType bcType)
{
  KPBackGround *kpbackground = 0;

  if (pageNum < _backgroundList.count())
    {
      kpbackground = backgroundList()->at(pageNum);
      kpbackground->setBackColor1(backColor1);
      kpbackground->setBackColor2(backColor2);
      kpbackground->setBackColorType(bcType);
    }
  m_bModified = true;
}

/*==================== set background picture ====================*/
void KPresenterDoc::setBackPixFilename(unsigned int pageNum,QString backPix)
{
  if (pageNum < _backgroundList.count())
    backgroundList()->at(pageNum)->setBackPixFilename(backPix);
  m_bModified = true;
}

/*==================== set background clipart ====================*/
void KPresenterDoc::setBackClipFilename(unsigned int pageNum,QString backClip)
{
  if (pageNum < _backgroundList.count())
    backgroundList()->at(pageNum)->setBackClipFilename(backClip);
  m_bModified = true;
}

/*================= set background pic view ======================*/
void KPresenterDoc::setBackView(unsigned int pageNum,BackView backView)
{
  if (pageNum < _backgroundList.count())
    backgroundList()->at(pageNum)->setBackView(backView);
  m_bModified = true;
}

/*==================== set background type =======================*/
void KPresenterDoc::setBackType(unsigned int pageNum,BackType backType)
{
  if (pageNum < _backgroundList.count())
    backgroundList()->at(pageNum)->setBackType(backType);
  m_bModified = true;
}

/*========================== set page effect =====================*/
void KPresenterDoc::setPageEffect(unsigned int pageNum,PageEffect pageEffect)
{
  if (pageNum < _backgroundList.count())
    backgroundList()->at(pageNum)->setPageEffect(pageEffect);
  m_bModified = true;
}

/*===================== set pen and brush ========================*/
bool KPresenterDoc::setPenBrush(QPen pen,QBrush brush,LineEnd lb,LineEnd le,FillType ft,QColor g1,QColor g2, BCType gt)
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
		dynamic_cast<KPRectObject*>(kpobject)->setFillType(ft);
		dynamic_cast<KPRectObject*>(kpobject)->setGColor1(g1);
		dynamic_cast<KPRectObject*>(kpobject)->setGColor2(g2);
		dynamic_cast<KPRectObject*>(kpobject)->setGType(gt);
		ret = true;
		repaint(kpobject);
	      } break;
	    case OT_ELLIPSE:
	      {
		dynamic_cast<KPEllipseObject*>(kpobject)->setPen(pen);
		dynamic_cast<KPEllipseObject*>(kpobject)->setBrush(brush);
		dynamic_cast<KPEllipseObject*>(kpobject)->setFillType(ft);
		dynamic_cast<KPEllipseObject*>(kpobject)->setGColor1(g1);
		dynamic_cast<KPEllipseObject*>(kpobject)->setGColor2(g2);
		dynamic_cast<KPEllipseObject*>(kpobject)->setGType(gt);
		ret = true;
		repaint(kpobject);
	      } break;
	    case OT_AUTOFORM:
	      {
		dynamic_cast<KPAutoformObject*>(kpobject)->setPen(pen);
		dynamic_cast<KPAutoformObject*>(kpobject)->setBrush(brush);
		dynamic_cast<KPAutoformObject*>(kpobject)->setLineBegin(lb);
		dynamic_cast<KPAutoformObject*>(kpobject)->setLineEnd(le);
		dynamic_cast<KPAutoformObject*>(kpobject)->setFillType(ft);
		dynamic_cast<KPAutoformObject*>(kpobject)->setGColor1(g1);
		dynamic_cast<KPAutoformObject*>(kpobject)->setGColor2(g2);
		dynamic_cast<KPAutoformObject*>(kpobject)->setGType(gt);
		ret = true;
		repaint(kpobject);
	      } break;
	    case OT_PIE:
	      {
		dynamic_cast<KPPieObject*>(kpobject)->setPen(pen);
		dynamic_cast<KPPieObject*>(kpobject)->setBrush(brush);
		dynamic_cast<KPPieObject*>(kpobject)->setLineBegin(lb);
		dynamic_cast<KPPieObject*>(kpobject)->setLineEnd(le);
		dynamic_cast<KPPieObject*>(kpobject)->setFillType(ft);
		dynamic_cast<KPPieObject*>(kpobject)->setGColor1(g1);
		dynamic_cast<KPPieObject*>(kpobject)->setGColor2(g2);
		dynamic_cast<KPPieObject*>(kpobject)->setGType(gt);
		ret = true;
		repaint(kpobject);
	      } break;
	    default: break;
	    }
	}
    }

  m_bModified = true;
  return ret;
}

/*================================================================*/
bool KPresenterDoc::setLineBegin(LineEnd lb)
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
		dynamic_cast<KPLineObject*>(kpobject)->setLineBegin(lb);
		ret = true;
		repaint(kpobject);
	      } break;
	    case OT_AUTOFORM:
	      {
		dynamic_cast<KPAutoformObject*>(kpobject)->setLineBegin(lb);
		ret = true;
		repaint(kpobject);
	      } break;
	    case OT_PIE:
	      {
		dynamic_cast<KPPieObject*>(kpobject)->setLineBegin(lb);
		ret = true;
		repaint(kpobject);
	      } break;
	    default: break;
	    }
	}
    }

  m_bModified = true;
  return ret;
}

/*================================================================*/
bool KPresenterDoc::setLineEnd(LineEnd le)
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
		dynamic_cast<KPLineObject*>(kpobject)->setLineEnd(le);
		ret = true;
		repaint(kpobject);
	      } break;
	    case OT_AUTOFORM:
	      {
		dynamic_cast<KPAutoformObject*>(kpobject)->setLineEnd(le);
		ret = true;
		repaint(kpobject);
	      } break;
	    case OT_PIE:
	      {
		dynamic_cast<KPPieObject*>(kpobject)->setLineEnd(le);
		ret = true;
		repaint(kpobject);
	      } break;
	    default: break;
	    }
	}
    }

  m_bModified = true;
  return ret;
}

/*================================================================*/
bool KPresenterDoc::setPieSettings(PieType pieType,int angle,int len)
{
  bool ret = false;

  KPObject *kpobject = 0;
  QList<KPObject> _objects;
  QList<PieValueCmd::PieValues> _oldValues;
  PieValueCmd::PieValues _newValues,*tmp;
  
  _objects.setAutoDelete(false);
  _oldValues.setAutoDelete(false);
  
  _newValues.pieType = pieType;
  _newValues.pieAngle = angle;
  _newValues.pieLength = len;
  
  for (int i = 0;i < static_cast<int>(objectList()->count());i++)
    {
      kpobject = objectList()->at(i);
      if (kpobject->getType() == OT_PIE)
	{
	  tmp = new PieValueCmd::PieValues;
	  tmp->pieType = dynamic_cast<KPPieObject*>(kpobject)->getPieType();
	  tmp->pieAngle = dynamic_cast<KPPieObject*>(kpobject)->getPieAngle();
	  tmp->pieLength = dynamic_cast<KPPieObject*>(kpobject)->getPieLength();
	  _oldValues.append(tmp);
	  if (kpobject->isSelected())
	    _objects.append(kpobject);
	  ret = true;
	}
    }
  
  if (!_objects.isEmpty())
    {
      PieValueCmd *pieValueCmd = new PieValueCmd(i18n("Change Pie/Arc/Chord Values"),_oldValues,_newValues,_objects,this);
      commands()->addCommand(pieValueCmd);
      pieValueCmd->execute();
    }
  else
    {
      _oldValues.setAutoDelete(true);
      _oldValues.clear();
    }

  m_bModified = true;
  return ret;
}

/*================================================================*/
bool KPresenterDoc::setRectSettings(int _rx,int _ry)
{
  bool ret = false;

  KPObject *kpobject = 0;
  QList<KPObject> _objects;
  QList<RectValueCmd::RectValues> _oldValues;
  RectValueCmd::RectValues _newValues,*tmp;
  
  _objects.setAutoDelete(false);
  _oldValues.setAutoDelete(false);
  
  _newValues.xRnd = _rx;
  _newValues.yRnd = _ry;
  
  for (int i = 0;i < static_cast<int>(objectList()->count());i++)
    {
      kpobject = objectList()->at(i);
      if (kpobject->getType() == OT_RECT)
	{
	  tmp = new RectValueCmd::RectValues;
	  dynamic_cast<KPRectObject*>(kpobject)->getRnds(tmp->xRnd,tmp->yRnd);
	  _oldValues.append(tmp);
	  if (kpobject->isSelected())
	    _objects.append(kpobject);
	  ret = true;
	}
    }
  
  if (!_objects.isEmpty())
    {
      RectValueCmd *rectValueCmd = new RectValueCmd(i18n("Change Rectangle values"),_oldValues,_newValues,_objects,this);
      commands()->addCommand(rectValueCmd);
      rectValueCmd->execute();
    }
  else
    {
      _oldValues.setAutoDelete(true);
      _oldValues.clear();
    }

  m_bModified = true;
  return ret;
}

/*=================== get background type ========================*/
BackType KPresenterDoc::getBackType(unsigned int pageNum)
{
  if (pageNum < _backgroundList.count())
    return backgroundList()->at(pageNum)->getBackType();
 
  return BT_COLOR;
}

/*=================== get background pic view ====================*/
BackView KPresenterDoc::getBackView(unsigned int pageNum)
{
  if (pageNum < _backgroundList.count())
    return backgroundList()->at(pageNum)->getBackView();

  return BV_TILED;
}

/*=================== get background picture =====================*/
QString KPresenterDoc::getBackPixFilename(unsigned int pageNum)
{
  if (pageNum < _backgroundList.count())
    return backgroundList()->at(pageNum)->getBackPixFilename();

  return 0;
}

/*=================== get background clipart =====================*/
QString KPresenterDoc::getBackClipFilename(unsigned int pageNum)
{
  if (pageNum < _backgroundList.count())
    return backgroundList()->at(pageNum)->getBackClipFilename();

  return 0;
}

/*=================== get background color 1 ======================*/
QColor KPresenterDoc::getBackColor1(unsigned int pageNum)
{
  if (pageNum < _backgroundList.count())
    return backgroundList()->at(pageNum)->getBackColor1();

  return white;
}

/*=================== get background color 2 ======================*/
QColor KPresenterDoc::getBackColor2(unsigned int pageNum)
{
  if (pageNum < _backgroundList.count())
    return backgroundList()->at(pageNum)->getBackColor2();

  return white;
}

/*=================== get background color type ==================*/
BCType KPresenterDoc::getBackColorType(unsigned int pageNum)
{
  if (pageNum < _backgroundList.count())
    return backgroundList()->at(pageNum)->getBackColorType();

  return BCT_PLAIN;
}

/*====================== get page effect =========================*/
PageEffect KPresenterDoc::getPageEffect(unsigned int pageNum)
{
  if (pageNum < _backgroundList.count())
    return backgroundList()->at(pageNum)->getPageEffect();

  return PEF_NONE;
}

/*========================= get pen ==============================*/
QPen KPresenterDoc::getPen(QPen pen)
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
	    case OT_PIE:
	      return dynamic_cast<KPPieObject*>(kpobject)->getPen();
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
LineEnd KPresenterDoc::getLineBegin(LineEnd lb)
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
	    case OT_PIE:
	      return dynamic_cast<KPPieObject*>(kpobject)->getLineBegin();
	      break;
	    default: break;
	    }
	}      
    }

  return lb;
}

/*========================= get line end =========================*/
LineEnd KPresenterDoc::getLineEnd(LineEnd le)
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
	    case OT_PIE:
	      return dynamic_cast<KPPieObject*>(kpobject)->getLineEnd();
	      break;
	    default: break;
	    }
	}      
    }

  return le;
}

/*========================= get brush =============================*/
QBrush KPresenterDoc::getBrush(QBrush brush)
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
	    case OT_PIE:
	      return dynamic_cast<KPPieObject*>(kpobject)->getBrush();
	      break;
	    default: break;
	    }
	}      
    }

  return brush;
}

/*================================================================*/
FillType KPresenterDoc::getFillType(FillType ft)
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
	      return dynamic_cast<KPRectObject*>(kpobject)->getFillType();
	      break;
	    case OT_ELLIPSE:
	      return dynamic_cast<KPEllipseObject*>(kpobject)->getFillType();
	      break;
	    case OT_AUTOFORM:
	      return dynamic_cast<KPAutoformObject*>(kpobject)->getFillType();
	      break;
	    case OT_PIE:
	      return dynamic_cast<KPPieObject*>(kpobject)->getFillType();
	      break;
	    default: break;
	    }
	}      
    }

  return ft;
}

/*================================================================*/
QColor KPresenterDoc::getGColor1(QColor g1)
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
	      return dynamic_cast<KPRectObject*>(kpobject)->getGColor1();
	      break;
	    case OT_ELLIPSE:
	      return dynamic_cast<KPEllipseObject*>(kpobject)->getGColor1();
	      break;
	    case OT_AUTOFORM:
	      return dynamic_cast<KPAutoformObject*>(kpobject)->getGColor1();
	      break;
	    case OT_PIE:
	      return dynamic_cast<KPPieObject*>(kpobject)->getGColor1();
	      break;
	    default: break;
	    }
	}      
    }

  return g1;
}

/*================================================================*/
QColor KPresenterDoc::getGColor2(QColor g2)
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
	      return dynamic_cast<KPRectObject*>(kpobject)->getGColor2();
	      break;
	    case OT_ELLIPSE:
	      return dynamic_cast<KPEllipseObject*>(kpobject)->getGColor2();
	      break;
	    case OT_AUTOFORM:
	      return dynamic_cast<KPAutoformObject*>(kpobject)->getGColor2();
	      break;
	    case OT_PIE:
	      return dynamic_cast<KPPieObject*>(kpobject)->getGColor2();
	      break;
	    default: break;
	    }
	}      
    }

  return g2;
}

/*================================================================*/
BCType KPresenterDoc::getGType(BCType gt)
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
	      return dynamic_cast<KPRectObject*>(kpobject)->getGType();
	      break;
	    case OT_ELLIPSE:
	      return dynamic_cast<KPEllipseObject*>(kpobject)->getGType();
	      break;
	    case OT_AUTOFORM:
	      return dynamic_cast<KPAutoformObject*>(kpobject)->getGType();
	      break;
	    case OT_PIE:
	      return dynamic_cast<KPPieObject*>(kpobject)->getGType();
	      break;
	    default: break;
	    }
	}      
    }

  return gt;
}

/*================================================================*/
PieType KPresenterDoc::getPieType(PieType pieType)
{
  KPObject *kpobject = 0;
  
  for (int i = 0;i < static_cast<int>(objectList()->count());i++)
    {
      kpobject = objectList()->at(i);
      if (kpobject->isSelected() && kpobject->getType() == OT_PIE)
	return dynamic_cast<KPPieObject*>(kpobject)->getPieType();
    }

  return pieType;
}

/*================================================================*/
int KPresenterDoc::getPieLength(int pieLength)
{
  KPObject *kpobject = 0;
  
  for (int i = 0;i < static_cast<int>(objectList()->count());i++)
    {
      kpobject = objectList()->at(i);
      if (kpobject->isSelected() && kpobject->getType() == OT_PIE)
	return dynamic_cast<KPPieObject*>(kpobject)->getPieLength();
    }

  return pieLength;
}

/*================================================================*/
int KPresenterDoc::getPieAngle(int pieAngle)
{
  KPObject *kpobject = 0;
  
  for (int i = 0;i < static_cast<int>(objectList()->count());i++)
    {
      kpobject = objectList()->at(i);
      if (kpobject->isSelected() && kpobject->getType() == OT_PIE)
	return dynamic_cast<KPPieObject*>(kpobject)->getPieAngle();
    }

  return pieAngle;
}

/*================================================================*/
int KPresenterDoc::getRndX(int _rx)
{
  KPObject *kpobject = 0;
  
  for (int i = 0;i < static_cast<int>(objectList()->count());i++)
    {
      kpobject = objectList()->at(i);
      if (kpobject->isSelected() && kpobject->getType() == OT_RECT)
	{
	  int tmp;
	  dynamic_cast<KPRectObject*>(kpobject)->getRnds(_rx,tmp);
	  return _rx;
	}
    }

  return _rx;
}

/*================================================================*/
int KPresenterDoc::getRndY(int _ry)
{
  KPObject *kpobject = 0;
  
  for (int i = 0;i < static_cast<int>(objectList()->count());i++)
    {
      kpobject = objectList()->at(i);
      if (kpobject->isSelected() && kpobject->getType() == OT_RECT)
	{
	  int tmp;
	  dynamic_cast<KPRectObject*>(kpobject)->getRnds(tmp,_ry);
	  return _ry;
	}
    }

  return _ry;
}

/*======================== lower objects =========================*/
void KPresenterDoc::lowerObjs(int diffx,int diffy)
{
  KPObject *kpobject = 0;
  
  QList<KPObject> *_new = new QList<KPObject>;

  for (unsigned int j = 0; j < _objectList->count();j++)
    _new->append(_objectList->at(j));

  _new->setAutoDelete(false);

  for (int i = 0;i < static_cast<int>(_new->count());i++)
    {
      kpobject = _new->at(i);
      if (kpobject->isSelected())
	{
	  _new->take(i);
	  _new->insert(0,kpobject);
	}
    }      

  LowerRaiseCmd *lrCmd = new LowerRaiseCmd(i18n("Lower Object(s)"),_objectList,_new,this);
  lrCmd->execute();
  _commands.addCommand(lrCmd);

  m_bModified = true;
}

/*========================= raise object =========================*/
void KPresenterDoc::raiseObjs(int diffx,int diffy)
{
  KPObject *kpobject = 0;
  
  QList<KPObject> *_new = new QList<KPObject>;

  for (unsigned int j = 0; j < _objectList->count();j++)
    _new->append(_objectList->at(j));

  _new->setAutoDelete(false);

  for (int i = 0;i < static_cast<int>(_new->count());i++)
    {
      kpobject = objectList()->at(i);
      if (kpobject->isSelected())
	{
	  _new->take(i);
	  _new->append(kpobject);
	}
    }      

  LowerRaiseCmd *lrCmd = new LowerRaiseCmd(i18n("Lower Object(s)"),_objectList,_new,this);
  lrCmd->execute();
  _commands.addCommand(lrCmd);

  m_bModified = true;
}

/*=================== insert a picture ==========================*/
void KPresenterDoc::insertPicture(QString filename,int diffx,int diffy)
{
  KPPixmapObject *kppixmapobject = new KPPixmapObject(&_pixmapCollection,filename);
  kppixmapobject->setOrig(((diffx + 10) / _rastX) * _rastX,((diffy + 10) / _rastY) * _rastY);
  kppixmapobject->setSelected(true);

  InsertCmd *insertCmd = new InsertCmd(i18n("Insert picture"),kppixmapobject,this);
  insertCmd->execute();
  _commands.addCommand(insertCmd);

  m_bModified = true;
}

/*=================== insert a clipart ==========================*/
void KPresenterDoc::insertClipart(QString filename,int diffx,int diffy)
{
  KPClipartObject *kpclipartobject = new KPClipartObject(filename);
  kpclipartobject->setOrig(((diffx + 10) / _rastX) * _rastX,((diffy + 10) / _rastY) * _rastY);
  kpclipartobject->setSize(150,150);
  kpclipartobject->setSelected(true);

  InsertCmd *insertCmd = new InsertCmd(i18n("Insert clipart"),kpclipartobject,this);
  insertCmd->execute();
  _commands.addCommand(insertCmd);

  m_bModified = true;
}

/*======================= change picture ========================*/
void KPresenterDoc::changePicture(QString filename,int diffx,int diffy)
{
  KPObject *kpobject = 0;
  
  for (int i = 0;i < static_cast<int>(objectList()->count());i++)
    {
      kpobject = objectList()->at(i);
      if (kpobject->isSelected() && kpobject->getType() == OT_PICTURE)
	{
	  KPPixmapObject *pix = new KPPixmapObject(&_pixmapCollection,filename);

	  ChgPixCmd *chgPixCmd = new ChgPixCmd(i18n("Change pixmap"),dynamic_cast<KPPixmapObject*>(kpobject),
					       pix,this);
	  chgPixCmd->execute();
	  _commands.addCommand(chgPixCmd);
	  break;
	}
    }

  m_bModified = true;
}

/*======================= change clipart ========================*/
void KPresenterDoc::changeClipart(QString filename,int diffx,int diffy)
{
  KPObject *kpobject = 0;
  
  for (int i = 0;i < static_cast<int>(objectList()->count());i++)
    {
      kpobject = objectList()->at(i);
      if (kpobject->isSelected() && kpobject->getType() == OT_CLIPART)
	{
	  ChgClipCmd *chgClipCmd = new ChgClipCmd(i18n("Change clipart"),dynamic_cast<KPClipartObject*>(kpobject),
						  dynamic_cast<KPClipartObject*>(kpobject)->getFileName(),filename,this);
	  chgClipCmd->execute();
	  _commands.addCommand(chgClipCmd);
	  break;
	}
    }

  m_bModified = true;
}

/*===================== insert a line ===========================*/
void KPresenterDoc::insertLine(KRect r,QPen pen,LineEnd lb,LineEnd le,LineType lt,int diffx,int diffy)
{
  KPLineObject *kplineobject = new KPLineObject(pen,lb,le,lt);
  kplineobject->setOrig(r.x() + diffx,r.y() + diffy);
  kplineobject->setSize(r.width(),r.height());
  kplineobject->setSelected(true);

  InsertCmd *insertCmd = new InsertCmd(i18n("Insert line"),kplineobject,this);
  insertCmd->execute();
  _commands.addCommand(insertCmd);

  m_bModified = true;
}

/*===================== insert a rectangle =======================*/
void KPresenterDoc::insertRectangle(KRect r,QPen pen,QBrush brush,FillType ft,QColor g1,QColor g2,
				    BCType gt,int rndX,int rndY,int diffx,int diffy)
{
  KPRectObject *kprectobject = new KPRectObject(pen,brush,ft,g1,g2,gt,rndX,rndY);
  kprectobject->setOrig(r.x() + diffx,r.y() + diffy);
  kprectobject->setSize(r.width(),r.height());
  kprectobject->setSelected(true);

  InsertCmd *insertCmd = new InsertCmd(i18n("Insert rectangle"),kprectobject,this);
  insertCmd->execute();
  _commands.addCommand(insertCmd);

  m_bModified = true;
}

/*===================== insert a circle or ellipse ===============*/
void KPresenterDoc::insertCircleOrEllipse(KRect r,QPen pen,QBrush brush,FillType ft,QColor g1,QColor g2,
					  BCType gt,int diffx,int diffy)
{
  KPEllipseObject *kpellipseobject = new KPEllipseObject(pen,brush,ft,g1,g2,gt);
  kpellipseobject->setOrig(r.x() + diffx,r.y() + diffy);
  kpellipseobject->setSize(r.width(),r.height());
  kpellipseobject->setSelected(true);

  InsertCmd *insertCmd = new InsertCmd(i18n("Insert ellipse"),kpellipseobject,this);
  insertCmd->execute();
  _commands.addCommand(insertCmd);

  m_bModified = true;
}

/*================================================================*/
void KPresenterDoc::insertPie(KRect r,QPen pen,QBrush brush,FillType ft,QColor g1,QColor g2,
			      BCType gt,PieType pt,int _angle,int _len,LineEnd lb,LineEnd le,int diffx,int diffy)
{
  KPPieObject *kppieobject = new KPPieObject(pen,brush,ft,g1,g2,gt,pt,_angle,_len,lb,le);
  kppieobject->setOrig(r.x() + diffx,r.y() + diffy);
  kppieobject->setSize(r.width(),r.height());
  kppieobject->setSelected(true);

  InsertCmd *insertCmd = new InsertCmd(i18n("Insert pie/arc/chord"),kppieobject,this);
  insertCmd->execute();
  _commands.addCommand(insertCmd);

  m_bModified = true;
}

/*===================== insert a textobject =====================*/
void KPresenterDoc::insertText(KRect r,int diffx,int diffy)
{
  KPTextObject *kptextobject = new KPTextObject();
  kptextobject->setOrig(r.x() + diffx,r.y() + diffy);
  kptextobject->setSize(r.width(),r.height());
  kptextobject->setSelected(true);

  InsertCmd *insertCmd = new InsertCmd(i18n("Insert text"),kptextobject,this);
  insertCmd->execute();
  _commands.addCommand(insertCmd);

  m_bModified = true;
}

/*======================= insert an autoform ====================*/
void KPresenterDoc::insertAutoform(QPen pen,QBrush brush,LineEnd lb,LineEnd le,FillType ft,QColor g1,QColor g2,
					     BCType gt,QString fileName,int diffx,int diffy)
{
  KPAutoformObject *kpautoformobject = new KPAutoformObject(pen,brush,fileName,lb,le,ft,g1,g2,gt);
  kpautoformobject->setOrig(((diffx + 10) / _rastX) * _rastX,((diffy + 10) / _rastY) * _rastY);
  kpautoformobject->setSize(150,150);
  kpautoformobject->setSelected(true);

  InsertCmd *insertCmd = new InsertCmd(i18n("Insert autoform"),kpautoformobject,this);
  insertCmd->execute();
  _commands.addCommand(insertCmd);

  m_bModified = true;
}

/*======================= set rasters ===========================*/
void KPresenterDoc::setRasters(unsigned int rx,unsigned int ry,bool _replace = true)
{
  _orastX = _rastX;
  _orastY = _rastY;
  _rastX = rx; 
  _rastY = ry; 
  if (_replace) replaceObjs();
}

/*=================== repaint all views =========================*/
void KPresenterDoc::repaint(bool erase)
{
  if (!m_lstViews.isEmpty())
    {
      for (viewPtr = m_lstViews.first();viewPtr != 0;viewPtr = m_lstViews.next())
	viewPtr->repaint(erase);
    }
}

/*===================== repaint =================================*/
void KPresenterDoc::repaint(KRect rect)
{
  if (!m_lstViews.isEmpty())
    {
      KRect r;

      for (viewPtr = m_lstViews.first();viewPtr != 0;viewPtr = m_lstViews.next())
	{
	  r = rect;
	  r.moveTopLeft(KPoint(r.x() - viewPtr->getDiffX(),r.y() - viewPtr->getDiffY()));
					
	  viewPtr->repaint(r,false);
	}
    }
}

/*===================== repaint =================================*/
void KPresenterDoc::repaint(KPObject *kpobject)
{
  if (!m_lstViews.isEmpty())
    {
      KRect r;

      for (viewPtr = m_lstViews.first();viewPtr != 0;viewPtr = m_lstViews.next())
	{
	  r = kpobject->getBoundingRect(0,0);
	  r.moveTopLeft(KPoint(r.x() - viewPtr->getDiffX(),r.y() - viewPtr->getDiffY()));
					
	  viewPtr->repaint(r,false);
	}
    }
}

/*==================== reorder page =============================*/
QList<int> KPresenterDoc::reorderPage(unsigned int num,int diffx,int diffy,float fakt = 1.0)
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
		  if (orderList.count() - 1 >= 0)
		    {
		      for (int j = orderList.count() - 1;j >= 0;j--)
			{
			  if ((int*)(kpobject->getPresNum()) > orderList.at(j))
			    {
			      orderList.insert(j+1,(int*)(kpobject->getPresNum()));
			      j = -1;
			      inserted = true;
			    }
			}
		    }
		  if (!inserted) orderList.insert(0,(int*)(kpobject->getPresNum()));
		}
	    }
	}
    }
  
  m_bModified = true;
  return orderList;
}

/*====================== get page of object ======================*/
int KPresenterDoc::getPageOfObj(int objNum,int diffx,int diffy,float fakt = 1.0)
{
  KRect rect;

  KPObject *kpobject = 0;
  
  for (int i = 0;i < static_cast<int>(objectList()->count());i++)
    {
      kpobject = objectList()->at(i);
      if (i == objNum)
	{
	  for (int j = 0;j < static_cast<int>(_backgroundList.count());j++)
	    {
	      rect = getPageSize(j,diffx,diffy,fakt,false);
	      rect.setWidth(QApplication::desktop()->width());
	      if (rect.intersects(kpobject->getBoundingRect(diffx,diffy)))
		return j+1;
	    }
	}
    }
  return -1;
}

/*================== get size of page ===========================*/
KRect KPresenterDoc::getPageSize(unsigned int num,int diffx,int diffy,float fakt=1.0,bool decBorders = true)
{
  double fact = 1;
  if (_pageLayout.unit == PG_CM) fact = 10;
  if (_pageLayout.unit == PG_INCH) fact = 25.4;

  int pw,ph,bl = static_cast<int>(_pageLayout.ptLeft * fact * 100) / 100;
  int br = static_cast<int>(_pageLayout.ptRight * fact * 100) / 100;
  int bt = static_cast<int>(_pageLayout.ptTop * fact * 100) / 100;
  int bb = static_cast<int>(_pageLayout.ptBottom * fact * 100) / 100;
  int wid = static_cast<int>(_pageLayout.ptWidth * fact * 100) / 100;
  int hei = static_cast<int>(_pageLayout.ptHeight * fact * 100) / 100;
  
  if (!decBorders)
    {
      br = 0;
      bt = 0;
      bl = 0;
      bb = 0;
    }

  pw = wid  - (bl + br);
  ph = hei - (bt + bb);

  pw = static_cast<int>(static_cast<float>(pw) * fakt);
  ph = static_cast<int>(static_cast<float>(ph) * fakt);

  return KRect(-diffx + bl,-diffy + bt + num * bt + num * bb + num * ph,pw,ph);
}

/*================================================================*/
int KPresenterDoc::getLeftBorder()
{
  double fact = 1;
  if (_pageLayout.unit == PG_CM) fact = 10;
  if (_pageLayout.unit == PG_INCH) fact = 25.4;
  return static_cast<int>(_pageLayout.ptLeft * fact * 100) / 100;
}

/*================================================================*/
int KPresenterDoc::getTopBorder()
{
  double fact = 1;
  if (_pageLayout.unit == PG_CM) fact = 10;
  if (_pageLayout.unit == PG_INCH) fact = 25.4;
  return static_cast<int>(_pageLayout.ptTop * fact * 100) / 100;
}

/*================================================================*/
int KPresenterDoc::getBottomBorder()
{
  double fact = 1;
  if (_pageLayout.unit == PG_CM) fact = 10;
  if (_pageLayout.unit == PG_INCH) fact = 25.4;
  return static_cast<int>(_pageLayout.ptBottom * fact * 100) / 100;
}

/*================================================================*/
void KPresenterDoc::deletePage(int _page,DelPageMode _delPageMode)
{
  KPObject *kpobject = 0;
  int _h = getPageSize(0,0,0).height();

  if (_delPageMode == DPM_DEL_OBJS || _delPageMode == DPM_DEL_MOVE_OBJS)
    {
      deSelectAllObj();
      for (int i = 0;i < static_cast<int>(objectList()->count());i++)
	{
	  kpobject = objectList()->at(i);
	  if (getPageOfObj(i,0,0) - 1 == _page)
	    kpobject->setSelected(true);
	}
      deleteObjs();
    }

  if (_delPageMode == DPM_MOVE_OBJS || _delPageMode == DPM_DEL_MOVE_OBJS)
    {
      for (int i = 0;i < static_cast<int>(objectList()->count());i++)
	{
	  kpobject = objectList()->at(i);
	  if (getPageOfObj(i,0,0) - 1 > _page)
	    kpobject->setOrig(kpobject->getOrig().x(),kpobject->getOrig().y() - _h);
	}
    }

  _backgroundList.remove(_page);
  repaint(false);
}

/*================================================================*/
void KPresenterDoc::insertPage(int _page,InsPageMode _insPageMode,InsertPos _insPos)
{
  KPObject *kpobject = 0;
  int _h = getPageSize(0,0,0).height();

  if (_insPos == IP_BEFORE) _page--;

  if (_insPageMode == IPM_MOVE_OBJS)
    {
      for (int i = 0;i < static_cast<int>(objectList()->count());i++)
	{
	  kpobject = objectList()->at(i);
	  if (getPageOfObj(i,0,0) - 1 > _page)
	    kpobject->setOrig(kpobject->getOrig().x(),kpobject->getOrig().y() + _h);
	}
    }

  if (_insPos == IP_BEFORE) _page++;

  QString templateDir = KApplication::kde_datadir();

  QString _template;
  QString _templatePath = kapp->kde_datadir() + "/kpresenter/templates/";

  if (KoTemplateChooseDia::chooseTemplate(_templatePath,_template,false))
    {
      QFileInfo fileInfo(_template);
      QString fileName(_templatePath + fileInfo.dirPath(false) + "/" + fileInfo.baseName() + ".kpt");
      _clean = false;

      if (_insPos == IP_AFTER) _page++;
      objStartY = getPageSize(_page - 1,0,0).y() + getPageSize(_page - 1,0,0).height();
      load_template(fileName.data());
      objStartY = 0;
      _clean = true;
      m_bModified = true;
      KPBackGround *kpbackground = _backgroundList.at(_backgroundList.count() - 1);
      _backgroundList.take(_backgroundList.count() - 1);
      _backgroundList.insert(_page,kpbackground);
    }
  
  repaint(false);
}

/*================ return number of selected objs ================*/
int KPresenterDoc::numSelected()
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
KPObject* KPresenterDoc::getSelectedObj()
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
void KPresenterDoc::deleteObjs()
{
  KPObject *kpobject = 0;
  QList<KPObject> _objects;
  _objects.setAutoDelete(false);
  
  for (int i = 0;i < static_cast<int>(objectList()->count());i++)
    {
      kpobject = objectList()->at(i);
      if (kpobject->isSelected())
	_objects.append(kpobject);
    }
 
  DeleteCmd *deleteCmd = new DeleteCmd(i18n("Delete object(s)"),_objects,this);
  deleteCmd->execute();
  _commands.addCommand(deleteCmd);
  m_bModified = true;
}

/*========================== copy objects ========================*/
void KPresenterDoc::copyObjs(int diffx,int diffy)
{
  QClipboard *cb = QApplication::clipboard();
  string clip_str;
  tostrstream out(clip_str);
  KPObject *kpobject = 0;
  
  out << otag << "<DOC author=\"" << "Reginald Stadlbauer" << "\" email=\"" << "reggie@kde.org" << "\" editor=\"" << "KPresenter"
      << "\" mime=\"" << "application/x-kpresenter-selection" << "\">" << endl;
  for (int i = 0;i < static_cast<int>(objectList()->count());i++)
    {
      kpobject = objectList()->at(i);
      if (kpobject->isSelected())
	{
	  out << otag << "<OBJECT type=\"" << static_cast<int>(kpobject->getType()) << "\">" << endl;
	  kpobject->moveBy(-diffx,-diffy);
	  kpobject->save(out);
	  kpobject->moveBy(diffx,diffy);
	  out << etag << "</OBJECT>" << endl;
	}
    }
  out << etag << "</DOC>" << endl;

  cb->setText(clip_str.c_str());
}

/*========================= paste objects ========================*/
void KPresenterDoc::pasteObjs(int diffx,int diffy)
{
  deSelectAllObj();

  pasting = true;
  pasteXOffset = diffx + 20;
  pasteYOffset = diffy + 20;
  string clip_str = QApplication::clipboard()->text();

  if (clip_str.empty()) return;
  
  istrstream in(clip_str.c_str());
  loadStream(in);

  pasting = false;
  m_bModified = true;
}

/*====================== replace objects =========================*/
void KPresenterDoc::replaceObjs()
{
  KPObject *kpobject = 0;
  int ox,oy;
  QList<KPObject> _objects;
  QList<KPoint> _diffs;
  _objects.setAutoDelete(false);
  _diffs.setAutoDelete(false);
  
  for (int i = 0;i < static_cast<int>(objectList()->count());i++)
    {
      kpobject = objectList()->at(i);
      ox = kpobject->getOrig().x();
      oy = kpobject->getOrig().y();

      ox = (ox / _rastX) * _rastX;
      oy = (oy / _rastY) * _rastY;

      _diffs.append(new KPoint(ox - kpobject->getOrig().x(),oy - kpobject->getOrig().y()));
      _objects.append(kpobject);
    }

  SetOptionsCmd *setOptionsCmd = new SetOptionsCmd(i18n("Set new options"),_diffs,_objects,_rastX,_rastY,
						   _orastX,_orastY,_txtBackCol,_otxtBackCol,this);
  _commands.addCommand(setOptionsCmd);
  setOptionsCmd->execute();

  m_bModified = true;
}

/*========================= restore background ==================*/
void KPresenterDoc::restoreBackground(int pageNum)
{
  if (pageNum < static_cast<int>(_backgroundList.count()))
    backgroundList()->at(pageNum)->restore();
}

/*==================== load stream ==============================*/
void KPresenterDoc::loadStream(istream &in)
{
  KOMLStreamFeed feed(in);
  KOMLParser parser(&feed);
  
  string tag;
  vector<KOMLAttrib> lst;
  string name;
 
  // DOC
  if (!parser.open("DOC",tag))
    {
      cerr << "Missing DOC" << endl;
      return;
    }
  
  KOMLParser::parseTag(tag.c_str(),name,lst);
  vector<KOMLAttrib>::const_iterator it = lst.begin();
  for(;it != lst.end();it++)
    {
      if ((*it).m_strName == "mime")
	{
	  if ((*it).m_strValue != "application/x-kpresenter-selection")
	    {
	      cerr << "Unknown mime type " << (*it).m_strValue << endl;
	      return;
	    }
	}
    }
    
  loadObjects(parser,lst);

  repaint(false);
  m_bModified = true;
}

/*================= deselect all objs ===========================*/
void KPresenterDoc::deSelectAllObj()
{
  if (!m_lstViews.isEmpty())
    {
      for (viewPtr = m_lstViews.first();viewPtr != 0;viewPtr = m_lstViews.next())
	viewPtr->getPage()->deSelectAllObj();
    }
}

/*======================== align objects left ===================*/
void KPresenterDoc::alignObjsLeft()
{
  KPObject *kpobject = 0;
  QList<KPObject> _objects;
  QList<KPoint> _diffs;
  _objects.setAutoDelete(false);
  _diffs.setAutoDelete(false);
  int _x = getPageSize(1,0,0).x();

  for (int i = 0;i < static_cast<int>(objectList()->count());i++)
    {
      kpobject = objectList()->at(i);
      if (kpobject->isSelected())
	{
	  _objects.append(kpobject);
	  _diffs.append(new KPoint(_x - kpobject->getOrig().x(),0));
	}
    }

  MoveByCmd2 *moveByCmd2 = new MoveByCmd2(i18n("Align object(s) left"),_diffs,_objects,this);
  _commands.addCommand(moveByCmd2);
  moveByCmd2->execute();
}

/*==================== align objects center h ===================*/
void KPresenterDoc::alignObjsCenterH()
{
  KPObject *kpobject = 0;
  QList<KPObject> _objects;
  QList<KPoint> _diffs;
  _objects.setAutoDelete(false);
  _diffs.setAutoDelete(false);
  int _x = getPageSize(1,0,0).x();
  int _w = getPageSize(1,0,0).width();

  for (int i = 0;i < static_cast<int>(objectList()->count());i++)
    {
      kpobject = objectList()->at(i);
      if (kpobject->isSelected())
	{
	  _objects.append(kpobject);
	  _diffs.append(new KPoint((_w - kpobject->getSize().width()) / 2 - kpobject->getOrig().x() + _x,0));
	}
    }

  MoveByCmd2 *moveByCmd2 = new MoveByCmd2(i18n("Align object(s) centered (horizontal)"),_diffs,_objects,this);
  _commands.addCommand(moveByCmd2);
  moveByCmd2->execute();
}

/*==================== align objects right ======================*/
void KPresenterDoc::alignObjsRight()
{
  KPObject *kpobject = 0;
  QList<KPObject> _objects;
  QList<KPoint> _diffs;
  _objects.setAutoDelete(false);
  _diffs.setAutoDelete(false);
  int _w = getPageSize(1,0,0).x() + getPageSize(1,0,0).width();

  for (int i = 0;i < static_cast<int>(objectList()->count());i++)
    {
      kpobject = objectList()->at(i);
      if (kpobject->isSelected())
	{
	  _objects.append(kpobject);
	  _diffs.append(new KPoint((_w - kpobject->getSize().width()) - kpobject->getOrig().x(),0));
	}
    }

  MoveByCmd2 *moveByCmd2 = new MoveByCmd2(i18n("Align object(s) right"),_diffs,_objects,this);
  _commands.addCommand(moveByCmd2);
  moveByCmd2->execute();
}

/*==================== align objects top ========================*/
void KPresenterDoc::alignObjsTop()
{
  KPObject *kpobject = 0;
  QList<KPObject> _objects;
  QList<KPoint> _diffs;
  _objects.setAutoDelete(false);
  _diffs.setAutoDelete(false);
  int pgnum,_y;

  for (int i = 0;i < static_cast<int>(objectList()->count());i++)
    {
      kpobject = objectList()->at(i);
      if (kpobject->isSelected())
	{
	  pgnum = getPageOfObj(i,0,0);
	  if (pgnum != -1)
	    {
	      _y = getPageSize(pgnum - 1,0,0).y();
	      _objects.append(kpobject);
	      _diffs.append(new KPoint(0,_y - kpobject->getOrig().y()));
	    }
	}
    }

  MoveByCmd2 *moveByCmd2 = new MoveByCmd2(i18n("Align object(s) top"),_diffs,_objects,this);
  _commands.addCommand(moveByCmd2);
  moveByCmd2->execute();
}

/*==================== align objects center v ===================*/
void KPresenterDoc::alignObjsCenterV()
{
  KPObject *kpobject = 0;
  QList<KPObject> _objects;
  QList<KPoint> _diffs;
  _objects.setAutoDelete(false);
  _diffs.setAutoDelete(false);
  int pgnum,_y,_h;

  for (int i = 0;i < static_cast<int>(objectList()->count());i++)
    {
      kpobject = objectList()->at(i);
      if (kpobject->isSelected())
	{
	  pgnum = getPageOfObj(i,0,0);
	  if (pgnum != -1)
	    {
	      _y = getPageSize(pgnum - 1,0,0).y(); 
	      _h = getPageSize(pgnum - 1,0,0).height();
	      _objects.append(kpobject);
	      _diffs.append(new KPoint(0,(_h - kpobject->getSize().height()) / 2 - kpobject->getOrig().y() + _y));
	    }
	}
    }

  MoveByCmd2 *moveByCmd2 = new MoveByCmd2(i18n("Align object(s) top"),_diffs,_objects,this);
  _commands.addCommand(moveByCmd2);
  moveByCmd2->execute();
}

/*==================== align objects top ========================*/
void KPresenterDoc::alignObjsBottom()
{
  KPObject *kpobject = 0;
  QList<KPObject> _objects;
  QList<KPoint> _diffs;
  _objects.setAutoDelete(false);
  _diffs.setAutoDelete(false);
  int pgnum,_h;

  for (int i = 0;i < static_cast<int>(objectList()->count());i++)
    {
      kpobject = objectList()->at(i);
      if (kpobject->isSelected())
	{
	  pgnum = getPageOfObj(i,0,0);
	  if (pgnum != -1)
	    {
	      _h = getPageSize(pgnum - 1,0,0).y() + getPageSize(pgnum - 1,0,0).height();
	      _objects.append(kpobject);
	      _diffs.append(new KPoint(0,_h - kpobject->getSize().height() - kpobject->getOrig().y()));
	    }
	}
    }

  MoveByCmd2 *moveByCmd2 = new MoveByCmd2(i18n("Align object(s) top"),_diffs,_objects,this);
  _commands.addCommand(moveByCmd2);
  moveByCmd2->execute();
}

/*================= undo redo changed ===========================*/
void KPresenterDoc::slotUndoRedoChanged(QString _undo,QString _redo)
{
  if (!m_lstViews.isEmpty())
    {
      for (viewPtr = m_lstViews.first();viewPtr != 0;viewPtr = m_lstViews.next())
	{
	  viewPtr->changeUndo(_undo,!_undo.isEmpty());
	  viewPtr->changeRedo(_redo,!_redo.isEmpty());
	}
    }
}

/*================= count of views ===========================*/
int KPresenterDoc::viewCount()
{
  return m_lstViews.count();
}
