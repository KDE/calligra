#include "img_doc.h"

#include <koIMR.h>
#include <komlMime.h>
#include <koStream.h>

#include <kurl.h>
#include <qmsgbox.h>

#include <strstream>
#include <fstream>
#include <unistd.h>

ImageDocument_impl::ImageDocument_impl()
{
  // Use CORBA mechanism for deleting views
  m_lstViews.setAutoDelete( false );
  m_lstChildren.setAutoDelete( true );

  m_bModified = false;
  m_bFitToWindow = false;
}

CORBA::Boolean ImageDocument_impl::init()
{
  return true;
}

ImageDocument_impl::~ImageDocument_impl()
{
  sdeb("ImageDocument_impl::~ImageDocument_impl()\n");
  cleanUp();
  edeb("...ImageDocument_impl::~ImageDocument_impl()\n");
}

void ImageDocument_impl::cleanUp()
{
  if ( m_bIsClean )
    return;

  /* QList<ImageView_impl> tmp( m_lstViews );
  ImageView_impl *v;
  for( v = tmp.first(); v != 0L; v = tmp.next() )
    v->View_impl::destroy( true );
  
  m_lstViews.clear(); */
  assert( m_lstViews.count() == 0 );
  
  m_lstChildren.clear();

  Document_impl::cleanUp();
}

CORBA::Boolean ImageDocument_impl::import( const char *_url )
{
  KURL u( _url );
  if ( u.isMalformed() )
  {
    QMessageBox::critical( (QWidget*)0L, i18n("KImage Error"), i18n("Malformed URL"), i18n("Ok") );
    return false;
  }
  if ( !u.isLocalFile() )
  {
    QMessageBox::critical( (QWidget*)0L, i18n("KImage Error"), i18n("Only local files are supported"), i18n("OK") );
    return false;
  }

  if ( !m_imgImage.load( u.path() ) )
    return false;
  
  emit sig_imageModified();
  
  string url = u.url().data();
  setId( url.c_str() );
  m_strExternFile = url.c_str();
  
  return true;
}

CORBA::Boolean ImageDocument_impl::export( const char *_url, const char *_format )
{
  KURL u( _url );
  if ( u.isMalformed() )
  {
    QMessageBox::critical( (QWidget*)0L, i18n("KImage Error"), i18n("Malformed URL"), i18n("Ok") );
    return false;
  }
  if ( !u.isLocalFile() )
  {
    QMessageBox::critical( (QWidget*)0L, i18n("KImage Error"), i18n("Only local files are supported"), i18n("Ok") );
    return false;
  }

  return m_imgImage.save( u.path(), _format );
}

bool ImageDocument_impl::hasToWriteMultipart()
{  
  if ( m_lstChildren.count() == 0 )
    return false;
  
  return true;
}

bool ImageDocument_impl::loadChildren( OPParts::MimeMultipartDict_ptr _dict )
{
  cerr << "bool ImageDocument_impl::loadChildren( OPParts::MimeMultipartDict_ptr _dict )" << endl;
  
  QListIterator<ImageChild> it( m_lstChildren );
  for( ; it.current(); ++it )
  {
    cerr << "Loading child" << endl;
    if ( !it.current()->loadDocument( _dict ) )
      return false;
  }

  cerr << "Loading done" << endl;
  
  return true;
}

bool ImageDocument_impl::load( KOMLParser& parser )
{
  cerr << "bool ImageDocument_impl::load( KOMLParser& parser )" << endl;
  
  string tag;
  vector<KOMLAttrib> lst;
  string name;
 
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
      if ( (*it).m_strValue != "application/x-kimage" )
      {
	cerr << "Unknown mime type " << (*it).m_strValue << endl;
	return false;
      }
    }
  }
    
  // DATA, OBJECT
  while( parser.open( 0L, tag ) )
  {
    KOMLParser::parseTag( tag.c_str(), name, lst );
 
    if ( name == "DATA" )
    {
      string data64;
      if ( !parser.readText( data64 ) )
      {
	cerr << "ERROR in readText" << endl;
	return false;
      }

      cerr << "GOT TEXT len=" << data64.length() << endl;
      
      istrstream strin( data64.c_str() );
      Base64IStream b64( strin );
      if ( !load( b64, false ) )
      {
	cerr << "ERROR: Loading base64 image" << endl;
	return false;
      }
    }
    else if ( name == "OBJECT" )
    {
      cerr << "OBJECT" << endl;
      ImageChild *ch = new ImageChild( this );
      ch->load( parser, lst );
      insertChild( ch );
    }
    else
      cerr << "Unknown tag '" << tag << "' in DOC" << endl;    

    if ( !parser.close( tag ) )
    {
      cerr << "ERR: Closing DOC" << endl;
      return false;
    }
  }

  return true;
}

bool ImageDocument_impl::load( istream &in, bool _randomaccess )
{
  cerr << "bool ImageDocument_impl::load( istream &in, bool _randomaccess )" << endl;
  
  static int counter = 0;
  
  if ( !_randomaccess )
  {
    cerr << "NO RANDOM ACCESS" << endl;
    
    // string data;
    // Write to random access device ( memory )
    {
      char buffer[ 100 ];
      sprintf( buffer, "/tmp/kimage_%i_%i", (int)getpid(),counter++);
      ofstream fout( buffer );
      pump p( in, fout );
      p.run();
      fout.close();
      m_imgImage.load( buffer );
      // unlink( buffer );
      // Not implemented in STD C++ lib of egcs :-((
      /* ostrstream strout( data );
      pump p( in, strout );
      p.run(); */
    }
    // istrstream in( data );
  }
  else
    in >> m_imgImage;
  
  return true;
}

bool ImageDocument_impl::save( ostream &out )
{
  if ( hasToWriteMultipart() )
  {
    out << "<?xml version=\"1.0\"?>" << endl;
    out << otag << "<DOC author=\"" << "Torben Weis" << "\" email=\"" << "weis@kde.org" << "\" editor=\"" << "KImage"
	<< "\" mime=\"" << "application/x-kimage" << "\" >" << endl;

    out << "<DATA format=\"image/bmp\">";
    {
      Base64OStream b64( out );
      b64 << m_imgImage;
    }
    out << "</DATA>" << endl;
    
    QListIterator<ImageChild> chl( m_lstChildren );
    for( ; chl.current(); ++chl )
      chl.current()->save( out );

    out << "</DOC>" << endl;
  }
  else
  {
    out << m_imgImage;
  }
  
  return true;
}

void ImageDocument_impl::makeChildListIntern( OPParts::Document_ptr _doc, const char *_path )
{
  cerr << "void ImageDocument_impl::makeChildList( OPParts::Document_ptr _doc, const char *_path )" << endl;
  
  int i = 0;
  
  QListIterator<ImageChild> it( m_lstChildren );
  for( ; it.current(); ++it )
  {
    QString tmp;
    tmp.sprintf("/%i", i++ );
    QString path( _path );
    path += tmp.data();
    cerr << "SETTING NAME To " << path.data() << endl;
    
    OPParts::Document_var doc = it.current()->document();    
    doc->makeChildList( _doc, path );
  }
}

const QImage& ImageDocument_impl::image()
{
  return m_imgImage;
}

void ImageDocument_impl::setFitToWindow( bool _fit )
{
  if ( m_bFitToWindow == _fit )
    return;
  
  m_bFitToWindow = _fit;
  emit sig_fitToWindow( _fit );
}

QStrList ImageDocument_impl::outputFormats()
{
  return QImageIO::outputFormats();
}

QStrList ImageDocument_impl::inputFormats()
{
  return QImageIO::inputFormats();
}

void ImageDocument_impl::viewList( OPParts::Document::ViewList*& _list )
{
  (*_list).length( m_lstViews.count() );

  int i = 0;
  QListIterator<ImageView_impl> it( m_lstViews );
  for( ; it.current(); ++it )
  {
    (*_list)[i++] = OPParts::View::_duplicate( it.current() );
  }
}

void ImageDocument_impl::addView( ImageView_impl *_view )
{
  m_lstViews.append( _view );
}

void ImageDocument_impl::removeView( ImageView_impl *_view )
{
  m_lstViews.setAutoDelete( false );
  m_lstViews.removeRef( _view );
  m_lstViews.setAutoDelete( true );
}

OPParts::View_ptr ImageDocument_impl::createView()
{
  cout << "OPParts::View_ptr ImageDocument_impl::createView()" << endl;
  
  ImageView_impl *p = new ImageView_impl( 0L );
  p->setDocument( this );
  p->QWidget::show();
  
  return OPParts::View::_duplicate( p );
}

void ImageDocument_impl::insertObject( const QRect& _rect, const char *_server_name )
{
  OPParts::Document_var doc = imr_createDocByServerName( _server_name );
  if ( CORBA::is_nil( doc ) )
    return;

  if ( !doc->init() )
  {
    QMessageBox::critical( (QWidget*)0L, i18n("KImage Error"), i18n("Could not init"), i18n("Ok") );
    return;
  }

  ImageChild* ch = new ImageChild( this, _rect, doc );

  insertChild( ch );
}

void ImageDocument_impl::insertChild( ImageChild *_child )
{
  m_lstChildren.append( _child );
  
  emit sig_insertObject( _child );
}

void ImageDocument_impl::changeChildGeometry( ImageChild *_child, const QRect& _rect )
{
  _child->setGeometry( _rect );

  emit sig_updateChildGeometry( _child );
}

QListIterator<ImageChild> ImageDocument_impl::childIterator()
{
  return QListIterator<ImageChild> ( m_lstChildren );
}

/**********************************************************
 *
 * ImageChild
 *
 **********************************************************/

ImageChild::ImageChild( ImageDocument_impl *_img, const QRect& _rect, OPParts::Document_ptr _doc )
  : KoDocumentChild( _rect, _doc )
{
  m_pImageDoc = _img;
}

ImageChild::ImageChild( ImageDocument_impl *_img ) : KoDocumentChild()
{
  m_pImageDoc = _img;
}

ImageChild::~ImageChild()
{
}

#include "img_doc.moc"
