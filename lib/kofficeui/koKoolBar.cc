#include "koKoolBar.h"

#include <qpainter.h>
#include <qcolor.h>

static int g_koKoolBarId = 0;

KoKoolBar::KoKoolBar( QWidget *_parent = 0L, const char *_name = 0L ) :
  QWidget( _parent, _name ), m_iActiveGroup( -1 ), m_pButtonUp( 0L ),
  m_pButtonDown( 0L )
{
  m_pBox = new KoKoolBarBox( *this );
}

int KoKoolBar::insertGroup( const char *_text )
{
  KoKoolBarGroup *p = new KoKoolBarGroup( *this, _text );
  m_mapGroups[ p->id() ] = p;
  if ( m_iActiveGroup == -1 )
    setActiveGroup( p->id() );
  else
    resizeEvent( 0L );
  return p->id();
}

int KoKoolBar::insertItem( int _grp, QPixmap& _pix, const char *_text, QObject *_obj, const char *_slot )
{
  map<int,Group_ptr>::iterator pos = m_mapGroups.find( _grp );
  if ( pos == m_mapGroups.end() )
    return -1;
  KoKoolBarItem *item = new KoKoolBarItem( *(pos->second), _pix, _text );
  
  if ( _obj != 0L && _slot != 0L )
    connect( item, SIGNAL( pressed( int, int ) ), _obj, _slot );
  pos->second->append( item );
  
  if ( pos->second->id() == m_iActiveGroup )
    m_pBox->update();

  return item->id();
}

void KoKoolBar::removeGroup( int _grp )
{
  map<int,Group_ptr>::iterator pos = m_mapGroups.find( _grp );
  if ( pos == m_mapGroups.end() )
    return;
  int id = pos->second->id();
  m_mapGroups.erase( pos );
  
  if ( id == m_iActiveGroup )
  {
    if ( m_mapGroups.size() == 0 )
    {
      m_iActiveGroup = -1;
      m_pBox->setActiveGroup( 0L );
    }
    else
    {
      pos = m_mapGroups.begin();
      m_iActiveGroup = pos->second->id();
      m_pBox->setActiveGroup( pos->second );
    }
  }

  resizeEvent( 0L );
}

void KoKoolBar::removeItem( int _grp, int _id )
{
  map<int,Group_ptr>::iterator pos = m_mapGroups.find( _grp );
  if ( pos == m_mapGroups.end() )
    return;
  pos->second->remove( _id );

  if ( pos->second->id() == m_iActiveGroup )
    m_pBox->update();
}

void KoKoolBar::setActiveGroup( int _grp )
{
  map<int,Group_ptr>::iterator pos = m_mapGroups.find( _grp );
  if ( pos == m_mapGroups.end() )
    return;
  m_iActiveGroup = pos->second->id();
  m_pBox->setActiveGroup( pos->second );

  resizeEvent( 0L );
}

void KoKoolBar::resizeEvent( QResizeEvent *_ev )
{
  cout << "ResizeEvent " << width() << " " << height() << endl;
  
  if ( m_iActiveGroup == -1 )
    return;
  
  int buttonheight = fontMetrics().height() + 4;
  
  map<int,Group_ptr>::iterator pos = m_mapGroups.find( m_iActiveGroup );
  if ( pos == m_mapGroups.end() )
    return;
  map<int,Group_ptr>::iterator it = pos;
  it++;
  
  int result = 0;   
  map<int,Group_ptr>::iterator i = it;
  for( ; i != m_mapGroups.end(); ++i )
    result++;
  
  int y = height() - buttonheight * result;
  for( ; it != m_mapGroups.end(); ++it )
  {    
    it->second->button().setGeometry( 0, y, width(), buttonheight );
    it->second->button().show();
    y += buttonheight;
  }
  
  int y2 = 0;
  it = m_mapGroups.begin();
  pos++;
  while( it != pos )
  {
    it->second->button().setGeometry( 0, y2, width(), buttonheight );
    it->second->button().show();
    it++;
    y2 += buttonheight;
  }

  if ( height() - y2 - result * buttonheight >= 0 )
  {
    m_pBox->show();
    m_pBox->setGeometry( 0, y2, width(), height() - y2 - result * buttonheight );
  }
  else
    m_pBox->hide();

  if ( m_pBox->needsScrolling() )
  {    
    if ( m_pButtonUp == 0L )
    {
      m_pButtonUp = new QPushButton( this );
      connect( m_pButtonUp, SIGNAL( clicked() ), this, SLOT( slotUp() ) );
    }
    if ( m_pButtonDown == 0L )
    {
      m_pButtonDown = new QPushButton( this );
      connect( m_pButtonDown, SIGNAL( clicked() ), this, SLOT( slotDown() ) );
    }
    m_pButtonUp->show();
    m_pButtonUp->raise();
    m_pButtonDown->show();
    m_pButtonDown->raise();
    m_pButtonUp->setGeometry( width() - 12, height() - result * buttonheight - 2 * 12, 12, 12 );
    m_pButtonDown->setGeometry( width() - 12, height() - result * buttonheight - 12, 12, 12 );
    updateScrollButtons();
  }
  else
  {
    if ( m_pButtonUp )
      m_pButtonUp->hide();
    if ( m_pButtonDown )
      m_pButtonDown->hide();
  }
}

void KoKoolBar::updateScrollButtons()
{
  if ( m_pBox->isAtTop() )
    m_pButtonUp->setEnabled( false );
  else
    m_pButtonUp->setEnabled( true );

  if ( m_pBox->isAtBottom() )
    m_pButtonDown->setEnabled( false );
  else
    m_pButtonDown->setEnabled( true );
}

void KoKoolBar::slotUp()
{
  m_pBox->scrollUp();
  updateScrollButtons();
}

void KoKoolBar::slotDown()
{
  m_pBox->scrollDown();
  updateScrollButtons();
}

void KoKoolBar::enableItem( int _grp, int _id, bool _enable )
{
  map<int,Group_ptr>::iterator pos = m_mapGroups.find( _grp );
  if ( pos == m_mapGroups.end() )
    return;
  KoKoolBarItem *item = pos->second->item( _id );
  if ( !item )
    return; 
  item->setEnabled( _enable );
}

void KoKoolBar::enableGroup( int _grp, bool _enable )
{
  map<int,Group_ptr>::iterator pos = m_mapGroups.find( _grp );
  if ( pos == m_mapGroups.end() )
    return;
  pos->second->setEnabled( _enable );
}

KoKoolBarBox::KoKoolBarBox( KoKoolBar &_bar ) : 
  QWidget( &_bar ), m_bar( _bar )
{
  m_iYOffset = 0;
  m_iYIcon = 0;
  m_pGroup = 0L;
  
  // setBackgroundMode( PaletteBase );
  setBackgroundColor( darkGray );
}

void KoKoolBarBox::setActiveGroup( KoKoolBarGroup *_grp )
{
  m_pGroup = _grp;
  m_iYOffset = 0;
  m_iYIcon = 0;
  update();
}

bool KoKoolBarBox::needsScrolling()
{
  if ( m_pGroup == 0L )
    return false;

  int y = 0;
  
  map<int,KoKoolBarGroup::Item_ptr>::iterator it = m_pGroup->beginIterator();
  for ( ; it != m_pGroup->endIterator(); ++it )
    y += it->second->height();
  
  if ( y > height() )
    return true;
  
  /** TODO **/
  return false;
}

KoKoolBarItem* KoKoolBarBox::findByPos( int _abs_y )
{
  if ( m_pGroup == 0L )
    return 0L;
  
  int y = 0;
  
  map<int,KoKoolBarGroup::Item_ptr>::iterator it = m_pGroup->beginIterator();
  for ( ; it != m_pGroup->endIterator(); ++it )
  {
    int dy = it->second->height();
    if ( y <= _abs_y && _abs_y <= y + dy )
      return it->second;
    y += dy;
  }
  
  return 0L;
}

int KoKoolBarBox::maxHeight()
{
  int y = 0;
  
  map<int,KoKoolBarGroup::Item_ptr>::iterator it = m_pGroup->beginIterator();
  for ( ; it != m_pGroup->endIterator(); ++it )
    y += it->second->height();

  return y;
}

bool KoKoolBarBox::isAtTop()
{
  if ( m_iYIcon == 0 )
    return true;
  return false;
}

bool KoKoolBarBox::isAtBottom()
{  
  if ( m_pGroup->items() == 0 )
    return true;
  int h = maxHeight();
  if ( height() + m_iYOffset >= h )
    return true;
  if ( m_pGroup->items() - 1 == m_iYIcon )
    return true;
  return false;
}

void KoKoolBarBox::scrollUp()
{
  if ( isAtTop() )
    return;
  
  int y = 0;
  int i = 0;
  m_iYIcon--;
  
  map<int,KoKoolBarGroup::Item_ptr>::iterator it = m_pGroup->beginIterator();
  for ( ; i < m_iYIcon && it != m_pGroup->endIterator(); ++it )
  {    
    y += it->second->height();
    i++;
  }
  
  int old = m_iYOffset;
  m_iYOffset = y;
  
  QWidget::scroll( 0, old - m_iYOffset );
}

void KoKoolBarBox::scrollDown()
{
  if ( isAtBottom() )
    return;
  
  int y = 0;
  int i = 0;
  m_iYIcon++;
  
  map<int,KoKoolBarGroup::Item_ptr>::iterator it = m_pGroup->beginIterator();
  for ( ; i < m_iYIcon && it != m_pGroup->endIterator(); ++it )
  {    
    y += it->second->height();
    i++;
  }
  
  int old = m_iYOffset;
  m_iYOffset = y;
  
  QWidget::scroll( 0, old - m_iYOffset );
}

void KoKoolBarBox::paintEvent( QPaintEvent *_ev )
{
  QPainter painter;
  painter.begin( this );

  static QColorGroup g2( black, white, white, darkGray, lightGray, white, black );
  static QBrush fill2( darkGray );
  qDrawShadePanel( &painter, 0, 0, width(), height(), g2, true, 1, &fill2 );
  
  if ( m_pGroup == 0L )
  {
    painter.end();
    return;
  }
  
  int y = -m_iYOffset;
  
  map<int,KoKoolBarGroup::Item_ptr>::iterator it = m_pGroup->beginIterator();
  for ( ; it != m_pGroup->endIterator(); ++it )
  {
    cout << "drawing " << y << endl;
    if ( y + it->second->height() >= 0 && y <= height() )
      painter.drawPixmap( ( width() - it->second->pixmap().width() ) / 2, y, it->second->pixmap() );
    if ( it->second->text() != 0L && it->second->text()[0] != 0 )
    {
      int y2 = y;
      y2 += it->second->pixmap().height() + 2; 
      painter.drawText( ( width() - painter.fontMetrics().width( it->second->text() ) ) / 2,
			y2 + painter.fontMetrics().ascent(), it->second->text() );
    }
    
    y += it->second->height();
  }
  
  painter.end();
}

KoKoolBarGroup::KoKoolBarGroup( KoKoolBar &_bar, const char *_text ) :
  m_bar( _bar ), m_button( _text, &_bar )
{
  m_bEnabled = true;
  
  connect( &m_button, SIGNAL( clicked() ), this, SLOT( pressed() ) );
  m_id = g_koKoolBarId++;
}

void KoKoolBarGroup::remove( int _id )
{
  map<int,KoKoolBarGroup::Item_ptr>::iterator pos = m_mapItems.find( _id );
  if ( pos == m_mapItems.end() )
    return;
  m_mapItems.erase( pos );
}

void KoKoolBarGroup::pressed()
{
  m_bar.setActiveGroup( m_id );
}

KoKoolBarItem::KoKoolBarItem( KoKoolBarGroup &_grp, QPixmap& _pix, const char *_text = "" ) : m_group( _grp )
{
  m_pixmap = _pix;
  m_strText = _text;
  m_bEnabled = true;
  m_id = g_koKoolBarId++;
  calc( _grp.bar() );
}

void KoKoolBarItem::calc( QWidget &_widget )
{
  m_iHeight = pixmap().height() + 8;
  
  if ( !m_strText.empty() )
    m_iHeight += _widget.fontMetrics().height() + 2;

  cout << "Height=" << m_iHeight << " Text=" << m_strText << endl;  
}

void KoKoolBarItem::press()
{
  cout << "Pressed id" << m_id << endl;
  
  emit pressed();
  emit pressed( m_group.id(), m_id );
}

/*
#include <kapp.h>

int main( int argc, char **argv )
{
  KApplication app( argc, argv );
  KoKoolBar bar;
  int file = bar.insertGroup("File");
  QPixmap pix;
  pix.load( "/opt/kde/share/icons/image.xpm" );
  bar.insertItem( file, pix );
  pix.load( "/opt/kde/share/icons/html.xpm" );
  bar.insertItem( file, pix );
  pix.load( "/opt/kde/share/icons/txt.xpm" );
  bar.insertItem( file, pix );
  pix.load( "/opt/kde/share/icons/kfm.xpm" );
  bar.insertItem( file, pix );

  bar.insertGroup("Edit");
  bar.insertGroup("View");
  bar.insertGroup("Layout");
  bar.insertGroup("Help");
  bar.setGeometry( 100, 100, 80, 300 );
  bar.show();
  
  app.exec();
}
*/

#include "koKoolBar.moc"

