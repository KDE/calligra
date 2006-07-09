/*
 * Kivio - Visual Modelling and Flowcharting
 * Copyright (C) 2000-2001 theKompany.com & Dave Marotti
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
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 */
#include "kivio_stackbar.h"
#include "stencilbarbutton.h"

#include <qlayout.h>
#include <qpushbutton.h>
#include <qapplication.h>

#include <kdebug.h>


KivioStackBar::KivioStackBar(KivioView* view, QWidget* parent, const char* name)
  : QDockWindow(parent, name), m_view(view)
{
  QDockWindow::boxLayout()->setSpacing(0);
  QDockWindow::boxLayout()->setMargin(0);
  setResizeEnabled(true);
  setNewLine(true);
  m_visiblePage = 0;
  
  connect(this, SIGNAL(placeChanged(QDockWindow::Place)), this, SLOT(newPlace(QDockWindow::Place)));
}

KivioStackBar::~KivioStackBar()
{
  kdDebug(43000) << "KivioStackBar::~KivioStackBar()" << endl;
}

void KivioStackBar::insertPage( QWidget* w, const QString& title )
{
  if (w->parent() != this) {
    w->reparent(this,QPoint(0,0));
  }

  w->hide();
  w->setFocusPolicy(NoFocus);

  setMinimumWidth( QMAX(minimumSize().width(),w->minimumSize().width() ) );
  setMaximumWidth( QMAX(maximumSize().width(),w->maximumSize().width() ) );

  DragBarButton* b = new DragBarButton( title, this );
  b->setOrientation(orientation());
  connect( b, SIGNAL(clicked()), SLOT(showButtonPage()) );
  connect( b, SIGNAL(beginDrag()), SLOT(buttonBeginDrag()) );
  connect( b, SIGNAL(finishDrag()), SLOT(buttonFinishDrag()) );
  connect( b, SIGNAL(closeRequired(DragBarButton*)), SLOT(slotDeleteButton(DragBarButton*)) );
  connect(this, SIGNAL(orientationChanged(Orientation)), b, SLOT(setOrientation(Orientation)));

  boxLayout()->addWidget(b);
  boxLayout()->addWidget(w, 1);

  m_data.insert(b, w);
  b->show();

  if (m_data.count() == 1) {
    showPage(w);
  }
}

void KivioStackBar::slotDeleteButton( DragBarButton *b )
{
  QWidget *pWidget = m_data[b];
  kdDebug(43000) << "Emitting deleteButton" << endl;
  emit deleteButton(b, pWidget, this);
}

void KivioStackBar::showPage( QWidget* w )
{
  emit aboutToShow( w );

  if(w == m_visiblePage) {
    return;
  }

  if ( m_visiblePage ) {
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
    if ( it.current() == widget ) {
      widget->hide();
      pBtn = (DragBarButton*)it.currentKey();
      it.current()->reparent(0, QPoint(0,0));
      m_data.remove( it.currentKey() );
      delete pBtn;
      break;
    }
    ++it;
  }

  if ( it.toFirst() ) {
    showPage( it.current() );
  } else {
    m_visiblePage = 0L;
  }
}

void KivioStackBar::deletePageAndButton( DragBarButton *pBtn )
{
  QWidget *pPage;

  if( !pBtn ) {
    kdDebug(43000) << "KivioStackBar::deletePageAndButton() - pBtn is NULL!" << endl;
    return;
  }

  pPage = m_data[pBtn];
  if( !pPage ) {
    kdDebug(43000) << "KivioStackBar::deletePageAndButton() - failed to find the key/value pair" << endl;
    return;
  }

  if( m_data.remove( pBtn )==false ) {
    kdDebug(43000) << "KivioStackBar::deletePageAndButton() - remove failed" << endl;
    return;
  }

  if(pPage == m_visiblePage) {
    m_visiblePage = 0L;
  }

  delete pBtn;
  delete pPage;


  // Set the next current page, or set it to nothing
  QPtrDictIterator<QWidget> it(m_data); // iterator for dict
  if ( it.toFirst() ) {
    showPage( it.current() );
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

  ev->ignore();
}

void KivioStackBar::newPlace(QDockWindow::Place place)
{
  if((place == OutsideDock) && (orientation() == Qt::Horizontal)) {
    setOrientation(Qt::Vertical);
  }
}

#include "kivio_stackbar.moc"
