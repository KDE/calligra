#include "img_doc.h"

#include <koIMR.h>

#include <kurl.h>
#include <qmsgbox.h>

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
    QMessageBox::critical( (QWidget*)0L, i18n("KImage Error"), i18n("Only local files are supported"), i18n("Ok") );
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

CORBA::Boolean ImageDocument_impl::openMimePart( OPParts::MimeMultipartDict_ptr _dict, const char *_id )
{
  return false;
}

CORBA::Boolean ImageDocument_impl::open( const char *_url )
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

  // We assume that this document is empty.
  QString name( _url );
  if ( name.right( 4 ) == ".img" )
  {
    /* Store store( _filename, IO_ReadOnly );
    // TODO: check wether opening was successful
    OBJECT obj = store.getRootObject();
    if ( obj == 0 )
      return false;
    
    if ( !loadFromStore( store, obj ) )
      return false;
      */
    return true;
  }
  else
  {    
    if ( !m_imgImage.load( u.path() ) )
      return false;
   
    string url = u.url().data();

    m_strExternFile = url;
    
    emit sig_imageModified();
    return true;
  }
}

CORBA::Boolean ImageDocument_impl::saveAsMimePart( const char *_url, const char *_format, const char *_boundary )
{
  return false;
}

CORBA::Boolean ImageDocument_impl::saveAs( const char *_url, const char *_format )
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

  string url = u.url().data();
  if ( url == m_strExternFile )
  {
    cout << "No need to save file\n" << endl;
    return true;
  }
  
  if ( !u.isLocalFile() )
  {
    QMessageBox::critical( (QWidget*)0L, i18n("KImage Error"), i18n("Only local files are supported"), i18n("Ok") );
    return false;
  }
  
  if ( strcmp( _format, "img" ) == 0 )
  {    
    /* Store store( _filename, IO_WriteOnly );
    // TODO test wether opening was successful
    OBJECT obj = saveToStore( store );
    if ( obj == 0L )
      return false;

    store.setRootObject( obj );
    store.setMimeType( MIME_TYPE );
    store.setEditor( EDITOR );
    store.release();
    */
    return true;
  }
  else
  {
    return m_imgImage.save( u.path(), _format );
  }

  cerr << "Unknown file format " << _format << endl;
  return false;
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
  m_lstChildren.append( ch );
  
  emit sig_insertObject( ch );
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
{
  m_pImageDoc = _img;
  m_rDoc = OPParts::Document::_duplicate( _doc );
  m_geometry = _rect;
}

ImageChild::~ImageChild()
{
  m_rDoc = 0L;
}

#include "img_doc.moc"
