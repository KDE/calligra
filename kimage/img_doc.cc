#include "img_doc.h"

ImageDocument_impl::ImageDocument_impl()
{
  // Use CORBA mechanism for deleting views
  m_lstViews.setAutoDelete( false );
  m_lstChildren.setAutoDelete( true );

  m_bModified = false;
  m_bFitToWindow = false;
}

ImageDocument_impl::~ImageDocument_impl()
{
  sdeb("ImageDocument_impl::~ImageDocument_impl()\n");
  cleanUp();
  edeb("...ImageDocument_impl::~ImageDocument_impl() %i\n",_refcnt());
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

CORBA::Boolean ImageDocument_impl::import( const char *_filename )
{
  bool res =  m_imgImage.load( _filename );
  if ( res )
    emit sig_imageModified();
  
  return res;
}

CORBA::Boolean ImageDocument_impl::export( const char *_filename, const char *_format )
{
  return m_imgImage.save( _filename, _format );
}

CORBA::Boolean ImageDocument_impl::open( const char *_filename )
{
  // We assume that this document is empty.
  QString name( _filename );
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
    if ( !m_imgImage.load( _filename ) )
      return false;
    
    emit sig_imageModified();
    return true;
  }
  
  return false;
}

CORBA::Boolean ImageDocument_impl::saveAs( const char *_filename, const char *_format )
{
  if ( strcmp( _format, "ppm" ) == 0 )
  {
    return m_imgImage.save( _filename, _format );
  }
  else if ( strcmp( _format, "img" ) == 0 )
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

#ifdef DEBUG
  warning( "Unknown file format '%s'\n", _format );
#endif

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
  ImageView_impl *p = new ImageView_impl( 0L );
  p->setDocument( this );
  
  return OPParts::View::_duplicate( p );
}

void ImageDocument_impl::insertObject( const QRect& _rect )
{
  OPParts::Document_var doc = new ImageDocument_impl;
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

/* OBJECT ImageDocument_impl::saveToStore( Store &_store )
{
    TYPE t_doc = _store.registerType( "KDE:KOffice:EmbeddedDoc" );
    TYPE t_ppm = _store.registerType( "Mime:image:x-ppm" );
    TYPE t_type = _store.registerType( "KDE:KImage:Image" );
    OBJECT obj = _store.newObject( t_type );

    PROPERTY p_geometry = _store.registerProperty( "KDE:KOffice:Geometry" );
    PROPERTY p_list = _store.registerProperty( "KDE:KOffice:EmbeddedDocList" );
    PROPERTY p_content = _store.registerProperty( "KDE:KOffice:EmbeddedDocContent" );
    PROPERTY p_mime = _store.registerProperty( "KDE:Store:MimeType" );
    PROPERTY p_edit = _store.registerProperty( "KDE:Store:Editor" );
    PROPERTY p_image = _store.registerProperty( "KDE:KImage:Image" );

    _store.writeStringValue( obj, p_mime, MIME_TYPE );
    _store.writeStringValue( obj, p_edit, EDITOR );

    VALUE value = _store.newValue( obj, p_image, t_ppm );
    StoreValueDevice *device = _store.getDeviceForValue( value );
    QDataStream stream;
    stream.setDevice( device );
    stream << m_imgImage;
    stream.unsetDevice();
    _store.release( device );

    QArray<OBJECT> arr( m_lstChildren.count() );
    QListIterator<ImageChild> it( m_lstChildren );
    int i = 0;
    for( ; it.current(); ++it )
    {
      OBJECT o = _store.newObject( t_doc );
      arr[i++] = o;

      OPParts::Document_var doc = it.current()->document();
      CORBA::String_var mime = doc->mimeType();
      _store.writeStringValue( o, p_mime, mime );

      // VALUE val = _store.newValue( o, p_content, TYPE_ID_Store );
      // _store.appendValue( val );
      // doc->save( _store.name(), true );
      // _store.closeValue();
    }

    _store.writeObjectReferenceArrayValue( obj, p_list, arr );

    return obj;
}

bool ImageDocument_impl::loadFromStore( Store &_store, OBJECT obj )
{
    TYPE t_doc = _store.findType( "KDE:KOffice:EmbeddedDoc" );
    TYPE t_ppm = _store.findType( "Mime:image:x-ppm" );
    TYPE t_type = _store.findType( "KDE:KImage:Image" );

    if ( !t_doc || !t_ppm || !t_type )
      return false;
    
    PROPERTY p_geometry = _store.findProperty( "KDE:KOffice:Geometry" );
    PROPERTY p_list = _store.findProperty( "KDE:KOffice:EmbeddedDocList" );
    PROPERTY p_content = _store.findProperty( "KDE:KOffice:EmbeddedDocContent" );
    PROPERTY p_mime = _store.findProperty( "KDE:Store:MimeType" );
    PROPERTY p_edit = _store.findProperty( "KDE:Store:Editor" );
    PROPERTY p_image = _store.findProperty( "KDE:KImage:Image" );
    
    if ( !p_geometry || !p_list || !p_content || !p_mime || !p_edit || !p_image )
      return false;
    
    QString mime = _store.readStringValue( obj, p_mime );
    if ( mime != MIME_TYPE )
      return false;

    VALUE value = _store.readValue( obj, p_image, t_ppm );
    if ( value == 0L )    
      return false;
    
    StoreValueDevice *dev = _store.getDeviceForValue( value );
    QDataStream stream;
    stream.setDevice( dev );   
    stream >> m_imgImage;
    stream.unsetDevice();
    _store.release( dev );
    delete dev;            

    emit sig_imageModified();

    QArray<OBJECT> arr;
    if ( readObjectReferenceArrayValue( obj, p_list, arr ) )
    {
      int i;
      int size = arr.size();
      for( i = 0; i < size; i++ )
      {
	OBJECT o = arr[i];
	QString m = _store.readStringValue( o, p_mime );
	QString e = _store.readStringValue( o, p_edit );
	QRect rect;
	_store.readRectValue( o, p_geoemtry );
	OPParts::Document_var doc = loadDocumentFromStore( o, m, e );
	if ( !CORBA::is_nil( doc ) )
	{
	  ImageChild *p;
	  m_lstChildren.append( p = new ImageChild( this, rect, OPParts::Document::_duplicate( doc ) ) );
	  emit sig_insertObject( p );
	}
      }
    }
    
    return true;
}
*/

/* OPParts::Document_ptr ImageDocument_impl::loadDocumentFromStore( OBJECT _obj,
								  const char *_mime, const char *_editor )
{
  // HACK

}
*/
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
