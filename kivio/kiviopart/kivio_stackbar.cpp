/*
 * Kivio - Visual Modelling and Flowcharting
 * Copyright (C) 2000 theKompany.com
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
 */
#include "kivio_stackbar.h"
#include "stencilbarbutton.h"

#include <qlayout.h>
#include <qpushbutton.h>
#include <qapplication.h>
#include <kdebug.h>


KivioStackBar::KivioStackBar( QWidget* parent, const char* name )
:QFrame(parent,name,WDestructiveClose)
{
  m_layout = new QVBoxLayout( this );
  m_visiblePage = 0;
}

KivioStackBar::~KivioStackBar()
{
   kdDebug() << "KivioStackBar::~KivioStackBar()" << endl;
}

void KivioStackBar::insertPage( QWidget* w, const QString& title )
{
  if (w->parent() != this)
    w->reparent(this,QPoint(0,0));
  
  w->hide();

  setMinimumWidth( QMAX(minimumSize().width(),w->minimumSize().width() ) );
  setMaximumWidth( QMAX(maximumSize().width(),w->maximumSize().width() ) );

  DragBarButton* b = new DragBarButton( title, this );
  connect( b, SIGNAL(clicked()), SLOT(showButtonPage()) );
  connect( b, SIGNAL(beginDrag()), SLOT(buttonBeginDrag()) );
  connect( b, SIGNAL(finishDrag()), SLOT(buttonFinishDrag()) );
  connect( b, SIGNAL(closeRequired(DragBarButton*)), SLOT(slotDeleteButton(DragBarButton*)) );

  m_layout->addWidget( b );
  m_layout->addWidget( w, 1 );

  m_data.insert(b,w);
  b->show();

  if (m_data.count()==1)
    showPage(w);
}

void KivioStackBar::slotDeleteButton( DragBarButton *b )
{
    QWidget *pWidget = m_data[b];
    kdDebug() << "Emitting deleteButton" << endl;
    emit deleteButton(b, pWidget, this);
}

void KivioStackBar::showPage( QWidget* w )
{
  emit aboutToShow( w );

  if ( w == m_visiblePage ) return;

  if ( m_visiblePage ){
    m_visiblePage->hide();
    w->show();
  } else {
    w->show();
  }
  m_visiblePage = w;
}

void KivioStackBar::showButtonPage()
{
  DragBarButton* b = (DragBarButton*)sender();
  showPage(findPage(b));
}

QWidget* KivioStackBar::findPage( DragBarButton* w )
{
  return m_data[w];
}

/*
 * This does *NOT* delete the widget
*/
void KivioStackBar::removePage( QWidget* widget )
{
  QPtrDictIterator<QWidget> it(m_data); // iterator for dict
  DragBarButton* pBtn;
  while ( it.current() ) {
    if ( it.current() == widget ){
      widget->hide();
      pBtn = (DragBarButton*)it.currentKey();
      it.current()->reparent(0, QPoint(0,0));
      m_data.remove( it.currentKey() );
      delete pBtn;
      break;
    }
    ++it;
  }

  if ( it.toFirst() ){
    showPage( it.current() );
  } else {
    m_visiblePage = 0L;
  }
}

void KivioStackBar::deletePageAndButton( DragBarButton *pBtn )
{
    QWidget *pPage;

    if( !pBtn )
    {
       kdDebug() << "KivioStackBar::deletePageAndButton() - pBtn is NULL!" << endl;
        return;
    }

    pPage = m_data[pBtn];
    if( !pPage )
    {
       kdDebug() << "KivioStackBar::deletePageAndButton() - failed to find the key/value pair" << endl;
        return;
    }

    if( m_data.remove( pBtn )==false )
    {
       kdDebug() << "KivioStackBar::deletePageAndButton() - remove failed" << endl;
        return;
    }

    delete pBtn;
    delete pPage;


    // Set the next current page, or set it to nothing
    QPtrDictIterator<QWidget> it(m_data); // iterator for dict
    if ( it.toFirst() )
    {
        showPage( it.current() );
    }
    else
    {
        m_visiblePage = 0L;
    }
}

QWidget* KivioStackBar::findPage( const QString& name )
{
  QPtrDictIterator<QWidget> it(m_data); // iterator for dict
  while ( it.current() ) {
    if ( it.current()->name() == name )
      return it.current();
    ++it;
  }
  return 0L;
}

void KivioStackBar::buttonBeginDrag()
{
  emit beginDragPage((DragBarButton*)sender());
}

void KivioStackBar::buttonFinishDrag()
{
  emit finishDragPage((DragBarButton*)sender());
}

void KivioStackBar::closeEvent(QCloseEvent* ev)
{
  QPtrDictIterator<QWidget> it(m_data); // iterator for dict
  while ( it.current() ) {
    slotDeleteButton((DragBarButton*)it.currentKey());
    if (it.current())
      ++it;
  }
  QFrame::closeEvent(ev);
}

#include "kivio_stackbar.moc"
