#include <kfiledialog.h>

#include "img_view.h"
#include "img_doc.h"
#include "img_main.h"

#include <op_app.h>
#include <utils.h>
#include <part_frame_impl.h>

#include <qpainter.h>
#include <qstring.h>
#include <qmsgbox.h>
#include <qkeycode.h>

#include <koPartSelectDia.h>

#define DEBUG

// Qt bug
char *getenv(const char *name);    

/**********************************************************
 *
 * ImageView_impl
 *
 **********************************************************/

ImageView_impl::ImageView_impl( QWidget *_parent = 0L, const char *_name = 0L ) :
  QWidget( _parent, _name ), View_impl(), KImage::ImageView_skel()
{
  setWidget( this );

  Control_impl::setFocusPolicy( OPControls::Control::ClickFocus );
 
  m_pImageDoc = 0L;
  m_bImageModified = false;
  m_bUnderConstruction = true;
  m_bRectSelection = false;

  m_lstFrames.setAutoDelete( true );  

  setGeometry( 5000, 5000, 100, 100 );
}

ImageView_impl::~ImageView_impl()
{
  sdeb("ImageView_impl::~ImageView_impl()\n");

  cleanUp();

  edeb("...ImageView_impl::~ImageView_impl()\n");
}

void ImageView_impl::cleanUp()
{
  sdeb("void ImageView_impl::cleanUp()\n" );
  
  if ( m_bIsClean )
    return;
  
  m_pImageDoc->removeView( this );
  
  m_lstFrames.clear();

  mdeb("rMenuBar %i %i\n",m_rMenuBar->refCount(), m_rMenuBar->_refcnt() );
  
  m_rMenuBar = 0L;
  m_vMenuBarFactory = 0L;

  m_rToolBarFile = 0L;
  m_rToolBarEdit = 0L;
  m_vToolBarFactory = 0L;

  View_impl::cleanUp();

  edeb("... void ImageView_impl::cleanUp()\n" );
}
  
void ImageView_impl::setDocument( ImageDocument_impl *_doc )
{
  if ( m_pImageDoc )
    m_pImageDoc->removeView( this );

  View_impl::setDocument( _doc );
  
  m_pImageDoc = _doc;
  m_bImageModified = true;

  m_pImageDoc->addView( this );

  QObject::connect( m_pImageDoc, SIGNAL( sig_fitToWindow( bool ) ), this, SLOT( slotFitToWindow( bool ) ) );
  QObject::connect( m_pImageDoc, SIGNAL( sig_imageModified() ), this, SLOT( slotImageModified() ) );
  QObject::connect( m_pImageDoc, SIGNAL( sig_insertObject( ImageChild* ) ),
		    this, SLOT( slotInsertObject( ImageChild* ) ) );
  QObject::connect( m_pImageDoc, SIGNAL( sig_updateChildGeometry( ImageChild* ) ),
		    this, SLOT( slotUpdateChildGeometry( ImageChild* ) ) );
}
  
void ImageView_impl::updatePixmap()
{
  if ( !m_bImageModified )
    return;
  
  if ( m_pImageDoc == 0L )
    return;
  
  m_pixImage.convertFromImage( m_pImageDoc->image() );
  scale();

  m_bImageModified = false;
}

void ImageView_impl::construct()
{
  if ( m_pImageDoc == 0L && !m_bUnderConstruction )
    return;
  
  assert( m_pImageDoc != 0L );
  
  m_bUnderConstruction = false;

  m_lstFrames.clear();
  
  updatePixmap();

  QListIterator<ImageChild> it = m_pImageDoc->childIterator();
  for( ; it.current(); ++it )
  {
    slotInsertObject( it.current() );
  }

  // We are now in sync with the document
  m_bImageModified = false;
}

void ImageView_impl::paintEvent( QPaintEvent *_ev )
{
  if ( m_bUnderConstruction )
    construct();
  else if ( m_bImageModified )
    updatePixmap();

  QPainter painter;
  painter.begin( this );
    
  painter.drawPixmap( 0, 0, m_pixImage );
  
  painter.end();
}

void ImageView_impl::resizeEvent( QResizeEvent *_ev )
{
  if ( !m_pImageDoc )
    return;
  
  if ( !m_pImageDoc->isFitToWindow() )
    return;

  m_bImageModified = true;
  updatePixmap();
  update();
}

void ImageView_impl::scale()
{
  if ( !m_pImageDoc->isFitToWindow() )
    return;
  
  QWMatrix m;				// transformation matrix
  m.scale(((double)width())/m_pixImage.width(),	// define scale factors
	  ((double)height())/m_pixImage.height());
  m_pixImage = m_pixImage.xForm( m );		// create scaled pixmap
}

void ImageView_impl::slotImageModified()
{
  m_bImageModified = true;
  updatePixmap();
  update();
}

void ImageView_impl::setFitToWindow( bool _fit )
{
  if ( _fit == m_pImageDoc->isFitToWindow() )
    return;
  
  m_pImageDoc->setFitToWindow( _fit );
  /* if ( m_bFitToWindow )
    resizeEvent( 0L );
  else
    m_pixImage.convertFromImage( m_pImageDoc->image() );

  update(); */
}

void ImageView_impl::slotFitToWindow( bool _fit )
{
  slotImageModified();
}

void ImageView_impl::createGUI()
{
  sdeb("void ImageView_impl::createGUI() %i | %i\n",refCount(),_refcnt());
  
  m_vMenuBarFactory = m_vPartShell->menuBarFactory();
  if ( !CORBA::is_nil( m_vMenuBarFactory ) )
  {
    // Menubar
    m_rMenuBar = m_vMenuBarFactory->createMenuBar( this );

    // Edit
    m_idMenuEdit = m_rMenuBar->insertMenu( CORBA::string_dup( i18n("&Edit") ) );
    m_idMenuEdit_NewImage = m_rMenuBar->insertItem( CORBA::string_dup( i18n("&Insert ...") ), m_idMenuEdit,
						    this, CORBA::string_dup( "insertObject" ) );
    m_rMenuBar->insertSeparator( m_idMenuEdit );
    
    m_idMenuEdit_ImportImage = m_rMenuBar->insertItem( CORBA::string_dup( i18n("Im&port ...") ), m_idMenuEdit,
						       this, CORBA::string_dup( "importImage" ) );
    m_idMenuEdit_ExportImage = m_rMenuBar->insertItem( CORBA::string_dup( i18n("&Export ...") ), m_idMenuEdit,
						       this, CORBA::string_dup( "exportImage" ) );

    // View
    m_idMenuView = m_rMenuBar->insertMenu( CORBA::string_dup( i18n("&View") ) );
    m_idMenuView_FitToWindow = m_rMenuBar->insertItem( CORBA::string_dup( i18n("&Fit To Window") ), m_idMenuView,
						       this, CORBA::string_dup( "toggleFitToWindow" ) );
    m_idMenuView_NewView = m_rMenuBar->insertItem( CORBA::string_dup( i18n("&New View") ), m_idMenuView,
						       this, CORBA::string_dup( "newView" ) );
  }

  m_vToolBarFactory = m_vPartShell->toolBarFactory();
  if ( !CORBA::is_nil( m_vToolBarFactory ) )
  {
    m_rToolBarFile = m_vToolBarFactory->createToolBar( this, CORBA::string_dup( "File" ) );
    m_rToolBarFile->setFileToolBar( true );
    QString tmp = opapp->kde_icondir().copy();
    tmp += "/mini/mini-doc.xpm";
    QString pix = loadPixmap( tmp );
    m_idButtonFile_Open = m_rToolBarFile->insertButton( CORBA::string_dup( pix ), i18n("Open"), 0L, 0L );

    m_rToolBarEdit = m_vToolBarFactory->createToolBar( this, "Edit" );
    tmp = opapp->kde_icondir().copy();
    tmp += "/mini/mini-eyes.xpm";
    pix = loadPixmap( tmp );
    m_idButtonEdit_Darker = m_rToolBarEdit->insertButton( CORBA::string_dup( pix ), i18n("Darker"), 0L, 0L );
  }

  edeb("...void ImageView_impl::createGUI() %i | %i\n",refCount(),_refcnt());
}

void ImageView_impl::toggleFitToWindow()
{
  assert( (m_pImageDoc != 0L) );

  setFitToWindow( !m_pImageDoc->isFitToWindow() );
  
  m_rMenuBar->setItemChecked( m_idMenuView_FitToWindow, m_pImageDoc->isFitToWindow() );
}

void ImageView_impl::newView()
{
  assert( (m_pImageDoc != 0L) );

  ImageShell_impl* shell = new ImageShell_impl;
  shell->enableMenuBar();
  shell->PartShell_impl::enableStatusBar();
  shell->enableToolBars();
  shell->show();
  shell->setDocument( m_pImageDoc );
  
  CORBA::release( shell );
}

void ImageView_impl::insertObject()
{
  KoPartEntry* pe = KoPartSelectDia::selectPart();
  if ( !pe )
    return;
  
  startRectSelection( pe->name() );
}

void ImageView_impl::importImage()
{
  QString file = KFileDialog::getOpenFileName( getenv( "HOME" ) );

  if ( file.isNull() )
    return;
  
  if ( !m_pImageDoc->import( file ) )
  {
    QString tmp;
    tmp.sprintf( i18n( "Could not open\n%s" ), file.data() );
    QMessageBox::critical( this, i18n( "IO Error" ), tmp, i18n( "OK" ) );
  }
}

void ImageView_impl::exportImage()
{
  QString file = KFileDialog::getSaveFileName( getenv( "HOME" ) );

  if ( file.isNull() )
    return;
  
  if ( !m_pImageDoc->export( file, "PPM" ) )
  {
    QString tmp;
    tmp.sprintf( i18n( "Could not write\n%s\nPerhaps permission denied." ), file.data() );
    QMessageBox::critical( this, i18n( "IO Error" ), tmp, i18n( "OK" ) );
  }
}

void ImageView_impl::startRectSelection( const char *_part_name )
{
  m_strNewPart = _part_name;
  m_bRectSelection = true;
}

void ImageView_impl::cancelRectSelection()
{
  m_bRectSelection = false;
  update();
}
 
void ImageView_impl::paintRectSelection()
{
  QPainter painter;
  painter.begin( this );
  
  painter.setRasterOp( NotROP );
  painter.drawRect( m_rctRectSelection );
  painter.end();
}

void ImageView_impl::keyPressEvent( QKeyEvent *_ev )
{
  if ( !m_bRectSelection )
    return;

  if ( _ev->key() == Key_Escape )
    cancelRectSelection();
}

void ImageView_impl::mousePressEvent( QMouseEvent *_ev )
{
  cout << "Mouse pressed" << endl;
  
  if ( !m_bRectSelection )
    return;

  m_rctRectSelection.setTop( _ev->pos().y() );
  m_rctRectSelection.setLeft( _ev->pos().x() );
  m_rctRectSelection.setBottom( _ev->pos().y() );
  m_rctRectSelection.setRight( _ev->pos().x() );

  paintRectSelection();
}

void ImageView_impl::mouseMoveEvent( QMouseEvent *_ev )
{
  if ( !m_bRectSelection )
    return;
  
  paintRectSelection();

  if ( _ev->pos().y() < m_rctRectSelection.top() )
    m_rctRectSelection.setBottom( m_rctRectSelection.top() );
  else
    m_rctRectSelection.setBottom( _ev->pos().y() );

  if ( _ev->pos().x() < m_rctRectSelection.left() )
    m_rctRectSelection.setRight( m_rctRectSelection.left() );
  else
    m_rctRectSelection.setRight( _ev->pos().x() );

  paintRectSelection();
}

void ImageView_impl::mouseReleaseEvent( QMouseEvent *_ev )
{
  if ( !m_bRectSelection )
    return;

  paintRectSelection();
  
  if ( _ev->pos().y() < m_rctRectSelection.top() )
    m_rctRectSelection.setBottom( m_rctRectSelection.top() );
  else
    m_rctRectSelection.setBottom( _ev->pos().y() );

  if ( _ev->pos().x() < m_rctRectSelection.left() )
    m_rctRectSelection.setRight( m_rctRectSelection.left() );
  else
    m_rctRectSelection.setRight( _ev->pos().x() );
  
  m_bRectSelection = false;
  m_pImageDoc->insertObject( m_rctRectSelection, m_strNewPart );
}

void ImageView_impl::slotInsertObject( ImageChild *_child )
{ 
  OPParts::Document_var doc = _child->document();
  OPParts::View_var v;

  try
  { 
    v = doc->createView();
  }
  catch ( OPParts::Document::MultipleViewsNotSupported &_ex )
  {
    // HACK
    printf("void ImageView_impl::slotInsertObject( const QRect& _rect, OPParts::Document_ptr _doc )\n");
    printf("Could not create view\n");
    exit(1);
  }

  if ( CORBA::is_nil( v ) )
  {
    printf("void ImageView_impl::slotInsertObject( const QRect& _rect, OPParts::Document_ptr _doc )\n");
    printf("return value is 0L\n");
    exit(1);
  }

  v->setMode( OPParts::Part::ChildMode );
  v->setPartShell( partShell() );

  ImageFrame *p = new ImageFrame( this, _child );
  p->attach( v );
  p->setGeometry( _child->geometry() );
  p->show();
  m_lstFrames.append( p );
  
  QObject::connect( p, SIGNAL( sig_geometryEnd( PartFrame_impl* ) ),
		    this, SLOT( slotGeometryEnd( PartFrame_impl* ) ) );
  QObject::connect( p, SIGNAL( sig_moveEnd( PartFrame_impl* ) ),
		    this, SLOT( slotMoveEnd( PartFrame_impl* ) ) );  
} 

void ImageView_impl::slotUpdateChildGeometry( ImageChild *_child )
{
  // Find frame for child
  ImageFrame *f = 0L;
  QListIterator<ImageFrame> it( m_lstFrames );
  for ( ; it.current() && !f; ++it )
    if ( it.current()->child() == _child )
      f = it.current();
  
  assert( f != 0L );
  
  // Are we already up to date ?
  if ( _child->geometry() == f->partGeometry() )
    return;
  
  // TODO scaling
  f->setPartGeometry( _child->geometry() );
}

void ImageView_impl::slotGeometryEnd( PartFrame_impl* _frame )
{
  ImageFrame *f = (ImageFrame*)_frame;
  // TODO scaling
  m_pImageDoc->changeChildGeometry( f->child(), _frame->partGeometry() );
}

void ImageView_impl::slotMoveEnd( PartFrame_impl* _frame )
{
  ImageFrame *f = (ImageFrame*)_frame;
  // TODO scaling
  m_pImageDoc->changeChildGeometry( f->child(), _frame->partGeometry() );
}

/**********************************************************
 *
 * ImageFrame
 *
 **********************************************************/

ImageFrame::ImageFrame( ImageView_impl* _view, ImageChild* _child ) : PartFrame_impl( _view )
{
  m_pImageView = _view;
  m_pImageChild = _child;
}

#include "img_view.moc"

