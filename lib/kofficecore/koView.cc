/* This file is part of the KDE project
   Copyright (C) 1998, 1999 Torben Weis <weis@kde.org>

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License as published by the Free Software Foundation; either
   version 2 of the License, or (at your option) any later version.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this library; see the file COPYING.LIB.  If not, write to
   the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
   Boston, MA 02111-1307, USA.
*/

#include "koView.h"
#include "koDocument.h"
#include "koDocumentChild.h"
#include "koFrame.h"

#include <kinstance.h>
#include <kstddirs.h>
#include <kdebug.h>
#include <kparts/plugin.h>
#include <kparts/event.h>
#include <cassert>

class KoViewPrivate
{
public:
  KoViewPrivate()
  {
    m_scaleX = m_scaleY = 1.0;
    m_children.setAutoDelete( true );
    m_manager = 0L;
    m_tempActiveWidget = 0L;
  }
  ~KoViewPrivate()
  {
  }

  KoDocument *m_doc;

  QGuardedPtr<KParts::PartManager> m_manager;

  double m_scaleX;
  double m_scaleY;

  QList<KoViewChild> m_children;

  QWidget *m_tempActiveWidget;
};

KoView::KoView( KoDocument *document, QWidget *parent, const char *name )
 : QWidget( parent, name )
{
  ASSERT( document );
    
  //kdDebug(31000) << "KoView::KoView " << this << endl;
  d = new KoViewPrivate;
  d->m_doc = document;
  PartBase::setObject( this );

  setFocusPolicy( StrongFocus );

  setMouseTracking( true );

  connect( d->m_doc, SIGNAL( childChanged( KoDocumentChild * ) ),
	   this, SLOT( slotChildChanged( KoDocumentChild * ) ) );
}

KoView::~KoView()
{
  //kdDebug(31000) << "KoView::~KoView " << this << endl;
  if ( d->m_manager && !koDocument()->singleViewMode() )
    d->m_manager->removePart( koDocument() );

  delete d;
}

KoDocument *KoView::koDocument() const
{
  return d->m_doc;
}

bool KoView::hasDocumentInWindow( KoDocument *doc )
{
  return child( doc ) != 0L;
}

void KoView::setPartManager( KParts::PartManager *manager )
{
  d->m_manager = manager;
  if ( !koDocument()->singleViewMode() )
    d->m_manager->addPart( koDocument(), false );
}

KParts::PartManager *KoView::partManager() const
{
  return d->m_manager;
}

KAction *KoView::action( const QDomElement &element )
{
  static QString attrName = QString::fromLatin1( "name" );
  QString name = element.attribute( attrName );

  KAction *act = KXMLGUIClient::action( name.utf8() );

  if ( !act )
    act = d->m_doc->KXMLGUIClient::action( name.utf8() );

  return act;
}

KoDocument *KoView::hitTest( const QPoint &pos )
{
  if ( selectedChild() && selectedChild()->frameRegion( matrix() ).contains( pos ) )
    return 0L;

  if ( activeChild() && activeChild()->frameRegion( matrix() ).contains( pos ) )
    return 0L;

  return koDocument()->hitTest( QPoint( pos.x() / xScaling(), pos.y() / yScaling() ) );
}

int KoView::leftBorder() const
{
  return 0;
}

int KoView::rightBorder() const
{
  return 0;
}

int KoView::topBorder() const
{
  return 0;
}

int KoView::bottomBorder() const
{
  return 0;
}

void KoView::setScaling( double x, double y )
{
  d->m_scaleX = x;
  d->m_scaleY = y;
  update();
}

double KoView::xScaling() const
{
  return d->m_scaleX;
}

double KoView::yScaling() const
{
  return d->m_scaleY;
}

QWidget *KoView::canvas()
{
  return this;
}

int KoView::canvasXOffset() const
{
  return 0;
}

int KoView::canvasYOffset() const
{
  return 0;
}

void KoView::customEvent( QCustomEvent *ev )
{
  if ( KParts::PartActivateEvent::test( ev ) )
    partActivateEvent( (KParts::PartActivateEvent *)ev );
  else if ( KParts::PartSelectEvent::test( ev ) )
    partSelectEvent( (KParts::PartSelectEvent *)ev );
}

void KoView::partActivateEvent( KParts::PartActivateEvent *event )
{
  if ( event->part() != (KParts::Part *)koDocument() )
  {
    assert( event->part()->inherits( "KoDocument" ) );

    KoDocumentChild *child = koDocument()->child( (KoDocument *)event->part() );
    if ( child && event->activated() )
    {
      if ( child->isRectangle() && !child->isTransparent() )
      {
        KoFrame *frame = new KoFrame( canvas() );
	KoView *view = child->document()->createView( frame );
	view->setContainerStates( child->document()->viewContainerStates( view ) );
	
	view->setPartManager( partManager() );
	
	view->setScaling( xScaling() * child->yScaling(), yScaling() * child->xScaling() );
	
	QRect geom = child->geometry();
	frame->setGeometry( geom.x() * xScaling() + canvasXOffset(), geom.y() * yScaling() + canvasYOffset(),
			    geom.width() * xScaling(), geom.height() * yScaling() );
	frame->setView( view );
	frame->show();
	frame->raise();
	d->m_children.append( new KoViewChild( child, frame ) );
	connect( view, SIGNAL( activated( bool ) ), this, SLOT( slotChildActivated( bool ) ) );
	
	d->m_manager->setActivePart( child->document(), view );
      }
      else
      {
        emit regionInvalidated( child->frameRegion( matrix() ), true );
	emit childActivated( child );
      }
    }
    else if ( child )
    {
      emit regionInvalidated( child->frameRegion( matrix() ), true );
      emit childDeactivated( child );
    }
    else
      emit invalidated();
  }
  else
    emit activated( event->activated() );
}

void KoView::partSelectEvent( KParts::PartSelectEvent *event )
{
  if ( event->part() != (KParts::Part *)koDocument() )
  {
    assert( event->part()->inherits( "KoDocument" ) );
	
    KoDocumentChild *child = koDocument()->child( (KoDocument *)event->part() );

    if ( child && event->selected() )
    {
      emit regionInvalidated( child->frameRegion( matrix() ), true );
      emit childSelected( child );
    }
    else if ( child )
    {
      emit regionInvalidated( child->frameRegion( matrix() ), true );
      emit childUnselected( child );
    }
    else
      emit invalidated();
  }
  else
    emit selected( event->selected() );
}

KoDocumentChild *KoView::selectedChild()
{
  if ( !d->m_manager )
    return 0L;

  KParts::Part *selectedPart = d->m_manager->selectedPart();

  if ( !selectedPart || !selectedPart->inherits( "KoDocument" ) )
    return 0L;

  return koDocument()->child( (KoDocument *)selectedPart );
}

KoDocumentChild *KoView::activeChild()
{
  if ( !d->m_manager )
    return 0L;

  KParts::Part *activePart = d->m_manager->activePart();

  if ( !activePart || !activePart->inherits( "KoDocument" ) )
    return 0L;

  return koDocument()->child( (KoDocument *)activePart );
}

void KoView::paintEverything( QPainter &painter, const QRect &rect, bool transparent )
{
  koDocument()->paintEverything( painter, rect, transparent, this );
}

KoViewChild *KoView::child( KoView *view )
{
  QListIterator<KoViewChild> it( d->m_children );
  for (; it.current(); ++it )
    if ( it.current()->frame()->view() == view )
      return it.current();

  return 0L;
}

KoViewChild *KoView::child( KoDocument *doc )
{
  QListIterator<KoViewChild> it( d->m_children );
  for (; it.current(); ++it )
    if ( it.current()->documentChild()->document() == doc )
      return it.current();

  return 0L;
}

QWMatrix KoView::matrix() const
{
  QWMatrix m;
  m.scale( xScaling(), yScaling() );
  return m;
}

void KoView::slotChildActivated( bool a )
{
  // Only interested in deactivate events
  if ( a )
    return;

  KoViewChild* ch = child( (KoView*)sender() );
  if ( !ch )
    return;

  KoView* view = ch->frame()->view();

  QWidget *activeWidget = view->d->m_tempActiveWidget;

  if ( d->m_manager->activeWidget() )
    activeWidget = d->m_manager->activeWidget();

  if ( !activeWidget || !activeWidget->inherits( "KoView" ) )
    return;

  // Is the new active view a child of this one ?
  // In this case we may not delete!
  //  QObject *n = d->m_manager->activeWidget();
  QObject *n = activeWidget;
  while( n )
    if ( n == (QObject *)view )
      return;
    else
     n = n->parent();


  d->m_tempActiveWidget = activeWidget;
  d->m_manager->setActivePart( 0L );

  QGuardedPtr<KoDocumentChild> docChild = ch->documentChild();
  QGuardedPtr<KoFrame> chFrame = ch->frame();
  if ( docChild && chFrame && chFrame->view() )
    docChild->document()->setViewContainerStates( chFrame->view(), chFrame->view()->containerStates() );

  d->m_children.remove( ch );

  // #### HACK
  // We want to delete as many views as possible and this
  // trick is used to go upwards in the view-tree.
  emit activated( FALSE );
}

void KoView::slotChildChanged( KoDocumentChild *child )
{
  QRegion region( child->oldPointArray( matrix() ) );
  emit regionInvalidated( child->frameRegion( matrix(), true ).unite( region ), true );
}

KoViewChild::KoViewChild( KoDocumentChild *child, KoFrame *frame )
{
  m_child = child;
  m_frame = frame;
}

KoViewChild::~KoViewChild()
{
  if ( m_frame )
    delete (KoFrame *)m_frame;
}
