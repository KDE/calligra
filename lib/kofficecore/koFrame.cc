#include "koFrame.h"

#include <qpainter.h>
#include <qbrush.h>

// HACK for debugging
#include <iostream.h>

KoFrame::KoFrame( QWidget *_parent, const char *_name ) :
  OPFrame( _parent, _name ), KOffice::Frame_skel()
{
  m_pPicture = 0L;
  
  m_pResizeWin1 = 0L;
  m_pResizeWin2 = 0L;
  m_pResizeWin3 = 0L;
  m_pResizeWin4 = 0L;
  m_pResizeWin5 = 0L;
  m_pResizeWin6 = 0L;
  m_pResizeWin7 = 0L;
  m_pResizeWin8 = 0L;
  m_pMoveWin1 = 0L;
  m_pMoveWin2 = 0L;    
  m_pMoveWin3 = 0L;
  m_pMoveWin4 = 0L;
  
  m_bShowBorders = false;
  m_bResizeMode = false;
  m_bShowGUI = false;
}

KoFrame::~KoFrame()
{  
}

void KoFrame::attach( QPicture *_pic )
{
  m_pPicture = _pic;
  update();
}

bool KoFrame::attachView( KOffice::View_ptr _view )
{
  if ( !OPFrame::attach( _view ) )
    return false;
  
  m_vView = KOffice::View::_duplicate( _view );
  m_wView = m_vView->window();  

  Window win = (Window)m_wView;
  if ( m_bShowBorders )
    XMoveResizeWindow( qt_xdisplay(), win, 5, 5, width() - 10, height() - 10 );
  else
    XMoveResizeWindow( qt_xdisplay(), win, 0, 0, width(), height() );
  
  m_vView->connect( "stateChanged", this, "viewChangedState" );
    
  return true;
}

void KoFrame::detach()
{
  if ( CORBA::is_nil( m_vView ) )
    return;

  // We do not want to receive further signals or events from this object
  m_vView->disconnectObject( this );
  
  m_vView = 0L;
  OPFrame::detach();
}

void KoFrame::viewChangedState( CORBA::Boolean is_marked, CORBA::Boolean has_focus )
{
  assert( !CORBA::is_nil( m_vView ) );
  
  if ( m_vView->mode() == KOffice::View::RootMode )
    return;
  
  if ( is_marked )
  {    
    showBorders( true );
    if ( has_focus )
      setResizeMode( false );
    else
      setResizeMode( true );
  }
  else
    showBorders( false );

  if ( has_focus )
    showGUI( true );
  else
    showGUI( false );
}

CORBA::ULong KoFrame::leftGUISize()
{
  assert( !CORBA::is_nil( m_vView ) );
  
  return m_vView->leftGUISize();
}

CORBA::ULong KoFrame::rightGUISize()
{
  assert( !CORBA::is_nil( m_vView ) );

  return m_vView->rightGUISize();
}

CORBA::ULong KoFrame::topGUISize()
{
  assert( !CORBA::is_nil( m_vView ) );

  return m_vView->topGUISize();
}

CORBA::ULong KoFrame::bottomGUISize()
{
  assert( !CORBA::is_nil( m_vView ) );

  return m_vView->bottomGUISize();
}

void KoFrame::showGUI( bool _mode )
{
  if ( _mode == m_bShowGUI )
    return;
  
  m_bShowGUI = _mode;
  
  if ( _mode )
  {
    setGeometry( x() - leftGUISize(), y() - topGUISize(),
		 width() + rightGUISize() + leftGUISize(),
		 height() + topGUISize() + bottomGUISize() );
  }
  else
  {
    setGeometry( x() + leftGUISize(), y() + topGUISize(),
		 width() - rightGUISize() - leftGUISize(),
		 height() - topGUISize() - bottomGUISize() );
  }
}

void KoFrame::showBorders( bool _mode )
{
  if ( _mode == m_bShowBorders )
    return;
  
  m_bShowBorders = _mode;

  if ( _mode )
  {
    setGeometry( x() - 5, y() - 5, width() + 10, height() + 10 );
  }
  else
  {
    setResizeMode( false );
    setGeometry( x() + 5, y() + 5, width() - 10, height() - 10 );
  }
}

void KoFrame::setResizeMode( bool _m )
{
  if ( _m == m_bResizeMode )
    return;
  
  m_bResizeMode = _m;

  // Turn resize mode off
  if ( !_m )
  {  
    if ( m_pMoveWin1 )
      delete m_pMoveWin1;
    m_pMoveWin1 = 0L;
    if ( m_pMoveWin2 )
      delete m_pMoveWin2;
    m_pMoveWin2 = 0L;
    if ( m_pMoveWin3 )
      delete m_pMoveWin3;
    m_pMoveWin3 = 0L;
    if ( m_pMoveWin4 )
      delete m_pMoveWin4;
    m_pMoveWin4 = 0L;
    
    if ( m_pResizeWin1 )
      delete m_pResizeWin1;
    m_pResizeWin1 = 0L;
    if ( m_pResizeWin2 )
      delete m_pResizeWin2;
    m_pResizeWin2 = 0L;
    if ( m_pResizeWin3 )
      delete m_pResizeWin3;
    m_pResizeWin3 = 0L;
    if ( m_pResizeWin4 )
      delete m_pResizeWin4;
    m_pResizeWin4 = 0L;
    if ( m_pResizeWin5 )
      delete m_pResizeWin5;
    m_pResizeWin5 = 0L;
    if ( m_pResizeWin6 )
      delete m_pResizeWin6;
    m_pResizeWin6 = 0L;
    if ( m_pResizeWin7 )
      delete m_pResizeWin7;
    m_pResizeWin7 = 0L;
    if ( m_pResizeWin8 )
      delete m_pResizeWin8;
    m_pResizeWin8 = 0L;
  }
  // Turn resize mode on
  else
  {
    m_pMoveWin1 = new KoFrameMove( this, sizeAllCursor, 1 );
    m_pMoveWin2 = new KoFrameMove( this, sizeAllCursor, 2 );	
    m_pMoveWin3 = new KoFrameMove( this, sizeAllCursor, 3 );
    m_pMoveWin4 = new KoFrameMove( this, sizeAllCursor, 4 );	
	
    m_pResizeWin1 = new KoFrameResize( this, sizeFDiagCursor, 1 );
    m_pResizeWin2 = new KoFrameResize( this, sizeHorCursor, 2 );
    m_pResizeWin3 = new KoFrameResize( this, sizeBDiagCursor, 3 );
    m_pResizeWin4 = new KoFrameResize( this, sizeVerCursor, 4 );
    m_pResizeWin5 = new KoFrameResize( this, sizeFDiagCursor, 5 );
    m_pResizeWin6 = new KoFrameResize( this, sizeHorCursor, 6 );
    m_pResizeWin7 = new KoFrameResize( this, sizeBDiagCursor, 7 );
    m_pResizeWin8 = new KoFrameResize( this, sizeVerCursor, 8 );
  }
}

void KoFrame::mousePressEvent( QMouseEvent *_ev )
{
  if ( _ev->button() == RightButton )
  {
    emit sig_popupMenu( this, _ev->pos() );
    return;
  }

  /** Experimental code that deal with attaching QPicture as stubs for the real part
  if ( _ev->button() == LeftButton && CORBA::is_nil( m_rPart ) && m_eState == KOffice::View::Marked )
  {
    cout << "Please activate me" << endl;
    partChangedState( KOffice::View::Active );
    emit sig_attachPart( this );
  }
  */
}

void KoFrame::paintEvent( QPaintEvent * )
{
  /*
  if ( m_pPicture && ( m_eState == KOffice::View::Inactive || m_eState == KOffice::View::Marked ) )
  {
    QPainter painter;
    painter.begin( this );
    if ( m_bShowBorders )
      painter.translate( 5, 5 );
    painter.drawPicture( *m_pPicture );
    painter.end();
    return;
  }
  */
  if ( !m_bShowBorders )
    return;
    
  QPainter painter;
  painter.begin( this );
  painter.setBackgroundColor( white );
  QBrush brush( BDiagPattern );
  brush.setColor( black );
  painter.fillRect( 0, 0, width(), height(), brush );
  painter.end();
}

void KoFrame::resizeEvent( QResizeEvent *_ev )
{
  if ( !CORBA::is_nil( m_vView ) )
  {
    Window win = (Window)m_wView;
    if ( m_bShowBorders )
      XMoveResizeWindow( qt_xdisplay(), win, 5, 5, width() - 10, height() - 10 );
    else
      XMoveResizeWindow( qt_xdisplay(), win, 0, 0, width(), height() );
  }
  else if ( !CORBA::is_nil( m_rPart ) )
  {
    OPFrame::resizeEvent( _ev );
    return;
  }

  if ( m_bResizeMode )
  {
    m_pResizeWin1->move( 0, 0 );
    m_pResizeWin2->move( 0, ( height() - 5 ) / 2 );
    m_pResizeWin3->move( 0, height() - 5 );
    m_pResizeWin4->move( ( width() - 5 ) / 2, height() - 5 );
    m_pResizeWin5->move( width() - 5, height() - 5 );
    m_pResizeWin6->move( width() - 5, ( height() - 5 ) / 2 );
    m_pResizeWin7->move( width() - 5, 0 );
    m_pResizeWin8->move( ( width() - 5 ) / 2, 0 );
    
    m_pMoveWin1->setGeometry( 0, 5, 5, height() - 10 );
    m_pMoveWin2->setGeometry( 5, height() - 5, width() - 10, 5 );
    m_pMoveWin3->setGeometry( width() - 5, 5, 5, height() - 10  );
    m_pMoveWin4->setGeometry( 5, 0, width() - 10, 5 );
  }
}

KOffice::View_ptr KoFrame::view()
{
  return KOffice::View::_duplicate( m_vView );
}

void KoFrame::popupMenu( const QPoint& _point )
{
  emit sig_popupMenu( this, _point );
}

void KoFrame::geometryStart()
{
  emit sig_geometryStart( this );
}

void KoFrame::geometryEnd()
{
  emit sig_geometryEnd( this );
}

void KoFrame::moveStart()
{
  emit sig_moveStart( this );
}

void KoFrame::moveEnd()
{
  emit sig_moveEnd( this );
}

QRect KoFrame::partGeometry()
{
  QRect r = geometry();
  
  if ( m_bShowBorders )
  {
    r.setTop( r.top() + 5 );
    r.setLeft( r.left() + 5 );
    r.setRight( r.right() - 5 );
    r.setBottom( r.bottom() - 5 );
  }
  
  return r;
}

void KoFrame::setPartGeometry( const QRect& _rect )
{
  QRect r = _rect;
  
  if ( m_bShowBorders )
  {
    r.setTop( r.top() - 5 );
    r.setLeft( r.left() - 5 );
    r.setRight( r.right() + 5 );
    r.setBottom( r.bottom() + 5 );
  }

  setGeometry( r );
}

/************************************************************************
 *
 * KoFrameResize
 *
 ************************************************************************/

KoFrameResize::KoFrameResize( KoFrame *_frame, const QCursor &cursor, int _pos ) : QWidget( _frame )
{
  m_pFrame = _frame;
  
  setCursor( cursor );
  position = _pos;
  // Left, top
  if ( position == 1 )
    setGeometry( 0, 0, 5, 5 );
  // Left
  if ( position == 2 )
    setGeometry( 0, ( parentWidget()->height() - 5 ) / 2, 5, 5 );
    // Left, bottom
  if ( position == 3 )
    setGeometry( 0, parentWidget()->height() - 5, 5, 5 );
  // Bottom
  if ( position == 4 )
    setGeometry( ( parentWidget()->width() - 5 ) / 2, parentWidget()->height() - 5, 5, 5 );
  // Right bottom
  if ( position == 5 )
    setGeometry( parentWidget()->width() - 5, parentWidget()->height() - 5, 5, 5 );
  // Right
  if ( position == 6 )
    setGeometry( parentWidget()->width() - 5, ( parentWidget()->height() - 5 ) / 2, 5, 5 );
  // Right, top
  if ( position == 7 )
    setGeometry( parentWidget()->width() - 5, 0, 5, 5 );
  // Top
  if ( position == 8 )
    setGeometry( ( parentWidget()->width() - 5 ) / 2, 0, 5, 5 );
  
  setBackgroundColor( black );
  show();
  raise();
}

KoFrameResize::~KoFrameResize()
{
}

void KoFrameResize::mousePressEvent( QMouseEvent *_ev )
{
  m_pFrame->geometryStart();
    
  if ( position == 1 || position == 3 || position == 2 )
    xPress = _ev->pos().x();
  else
    xPress = _ev->pos().x() - width();
  if ( position == 1 || position == 7 || position == 8 )
    yPress = _ev->pos().x();
  else
    yPress = _ev->pos().x() - height();
}

void KoFrameResize::mouseMoveEvent( QMouseEvent *_ev )
{
  int xpos = _ev->pos().x() - xPress + x();
  int ypos = _ev->pos().y() - yPress + y();

  // Left, top
  if ( position == 1 )
  {
    if ( xpos > parentWidget()->width() - 10 )
      xpos = parentWidget()->width() - 10;
    if ( ypos > parentWidget()->height() - 10 )
      ypos = parentWidget()->height() - 10;
    parentWidget()->setGeometry( parentWidget()->x() + xpos, parentWidget()->y() + ypos,
				 parentWidget()->width() - xpos, parentWidget()->height() - ypos );
  }
  // Left
  if ( position == 2 )
  {
    if ( xpos > parentWidget()->width() - 10 )
      xpos = parentWidget()->width() - 10;
    parentWidget()->setGeometry( parentWidget()->x() + xpos, parentWidget()->y(),
				 parentWidget()->width() - xpos, parentWidget()->height() );
  }
  // Left, bottom
  else if ( position == 3 )
  {
    if ( xpos > parentWidget()->width() - 10 )
      xpos = parentWidget()->width() - 10;
    if ( ypos < 10 )
      ypos = 10;
    parentWidget()->setGeometry( parentWidget()->x() + xpos, parentWidget()->y(),
				 parentWidget()->width() - xpos, ypos );
  }
  // Bottom
  else if ( position == 4 )
  {
    if ( ypos < 10 )
      ypos = 10;
    parentWidget()->setGeometry( parentWidget()->x(), parentWidget()->y(),
				 parentWidget()->width(), ypos );
  }
  // Right, bottom
  else if ( position == 5 )
  {
    if ( xpos < 10 )
      xpos = 10;
    if ( ypos < 10 )
      ypos = 10;
    parentWidget()->setGeometry( parentWidget()->x(), parentWidget()->y(), xpos, ypos );
  }
  // Right
  else if ( position == 6 )
  {
    if ( xpos < 10 )
      xpos = 10;
    parentWidget()->setGeometry( parentWidget()->x(), parentWidget()->y(),
				 xpos, parentWidget()->height() );
  }
  // Right, top
  else if ( position == 7 )
  {
    if ( xpos < 10 )
      xpos = 10;
    if ( ypos > parentWidget()->height() - 10 )
      ypos = parentWidget()->height() - 10;
    parentWidget()->setGeometry( parentWidget()->x(), parentWidget()->y() + ypos,
				 xpos, parentWidget()->height() - ypos );
  }
  // Top
  else if ( position == 8 )
  {
    if ( ypos > parentWidget()->height() - 10 )
      ypos = parentWidget()->height() - 10;
    parentWidget()->setGeometry( parentWidget()->x(), parentWidget()->y() + ypos,
				 parentWidget()->width(), parentWidget()->height() - ypos );
  }
}

void KoFrameResize::mouseReleaseEvent( QMouseEvent *_ev )
{
  mouseMoveEvent( _ev );

  m_pFrame->geometryEnd();
}

/************************************************************************
 *
 * KoFrameMove
 *
 ************************************************************************/

KoFrameMove::KoFrameMove( KoFrame *_frame, const QCursor &cursor, int _pos ) : QWidget( _frame )
{
  m_pFrame = _frame;
  
  setCursor( cursor );
  position = _pos;
  // Left
  if ( position == 1 )
    setGeometry( 0, 5, 5, parentWidget()->height() - 10 );
  // Bottom
  if ( position == 2 )
    setGeometry( 5, parentWidget()->height() - 5, parentWidget()->width() - 10, 5 );
  // Right
  if ( position == 3 )
    setGeometry( parentWidget()->width() - 5, 5, 5, parentWidget()->height() - 10 );
  // Top
  if ( position == 4 )
    setGeometry( 5, 0, parentWidget()->width() - 10, 5 );
  
  setBackgroundColor( lightGray );
  show();
  raise();
}

KoFrameMove::~KoFrameMove()
{
}

void KoFrameMove::mousePressEvent( QMouseEvent *_ev )
{
  m_pFrame->moveStart();
  
  QPoint p( mapToGlobal( QPoint( _ev->pos().x(), _ev->pos().y() ) ) );
  xPress = p.x();
  yPress = p.y();
  xInit = parentWidget()->x();
  yInit = parentWidget()->y();    
}

void KoFrameMove::mouseMoveEvent( QMouseEvent *_ev )
{
  QPoint p( mapToGlobal( QPoint( _ev->pos().x(), _ev->pos().y() ) ) );

  parentWidget()->move( xInit + p.x() - xPress, yInit + p.y() - yPress );
}

void KoFrameMove::mouseReleaseEvent( QMouseEvent *_ev )
{
  mouseMoveEvent( _ev );

  m_pFrame->moveEnd();
}

#include "koFrame.moc"
