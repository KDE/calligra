/* -*- C++ -*-

  $Id$

  This file is part of KIllustrator.
  Copyright (C) 2001 Igor Janssen (rm@linux.ru.net)

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU Library General Public License as
  published by
  the Free Software Foundation; either version 2 of the License, or
  (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU Library General Public License
  along with this program; if not, write to the Free Software
  Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.

*/

#include <qapplication.h>
#include <qstring.h>
#include <qpopupmenu.h>
#include <qtimer.h>

#include <stdio.h>
#include <stdlib.h>

#include <klocale.h>
#include <kiconloader.h>
#include <kmessagebox.h>
#include <knotifyclient.h>
#include <klineeditdlg.h>
#include <kdebug.h>

#include "TabBar.h"
#include "Canvas.h"
#include "KIllustrator_view.h"
#include "KIllustrator_doc.h"
#include "GDocument.h"
#include "GPage.h"
#include "KIllustrator_factory.h"

TabBar::TabBar( QWidget *parent, KIllustratorView *view )
: QWidget(parent), m_pView(view)
{
  m_pPopupMenu = 0L;
  doc = view->activeDocument();

  leftTab = 1;
  m_rightTab = 0;
  activeTab = 1;
  m_moveTab = 0;

  setFixedHeight(16);
}

void TabBar::moveTab( int _from, int _to, bool _before )
{
  doc->movePage(_from, _to, _before);
  if(_from < _to)
    activeTab = _to - 2;
  repaint();
}


void TabBar::scrollLeft()
{
  if ( leftTab == 1 )
    return;

  leftTab--;
  repaint( false );
}

void TabBar::scrollRight()
{
  if ( m_rightTab == static_cast<int>(doc->getPages().count()) )
    return;

  if ( leftTab == static_cast<int>(doc->getPages().count()) )
    return;

  leftTab++;
  repaint( false );
}

void TabBar::scrollFirst()
{
  activeTab = 1;
  setActiveTab();
}

void TabBar::scrollLast()
{
  activeTab = doc->getPages().count();
  setActiveTab();
}

void TabBar::setActiveTab()
{
  doc->setActivePage(activeTab - 1);
  update();
}

void TabBar::slotRemove( )
{
  if(doc->getPages().count() <= 1)
  {
    QApplication::beep();
    KMessageBox::error( this,i18n("You cannot delete the only page of the map."), i18n("Remove page") );
    return;
  }
  int ret = KMessageBox::warningYesNo( this, i18n("You are going to remove the active page.\nDo you want to continue?"), i18n("Remove page"));
  if( ret == 3 )
  {
    doc->deletePage(doc->pageForIndex(activeTab - 1));
    if(activeTab >= 1)
      activeTab--;
    setActiveTab();
  }
}

void TabBar::slotAdd()
{
  doc->addPage();
}

void TabBar::paintEvent( QPaintEvent* )
{
  QPainter painter;
  QPixmap pm(size());
  pm.fill( backgroundColor() );
  painter.begin( &pm, this );

  if ( leftTab > 1 )
    paintTab( painter, -10, QString(""), 0, 0, FALSE );

  int i = 1;
  int x = 0;
  QString text;
  QString active_text;
  int active_x = -1;
  int active_width = 0;
  int active_y = 0;

  for(QListIterator<GPage> it(doc->getPages()); it.current(); ++it)
  {
        text = ((GPage *)it)->name();
        QFontMetrics fm = painter.fontMetrics();
        int text_width = fm.width( text );
        int text_y = ( height() - fm.ascent() - fm.descent() ) / 2 + fm.ascent();

        if ( i == activeTab )
        {
            active_text = text;
            active_x = x;
            active_y = text_y;
            active_width = text_width;

            if ( i >= leftTab )
                x += 10 + text_width;
        }
        else if ( i >= leftTab )
        {
        if ( m_moveTab == i )
                  paintTab( painter, x, text, text_width, text_y, false, true );
                else
                    paintTab( painter, x, text, text_width, text_y, false );
            x += 10 + text_width;
        }
        if ( x - 10 < width() )
                m_rightTab = i;
        i++;
    }

//    if ( active_text != 0L )
    paintTab( painter, active_x, active_text, active_width, active_y, TRUE );

    painter.end();
    bitBlt( this, 0, 0, &pm );
}

void TabBar::paintTab( QPainter & painter, int x, const QString& text, int text_width, int text_y,
                              bool isactive, bool ismovemarked )
{
    QPointArray parr;
    parr.setPoints( 4, x,0, x+10,height()-1, x+10+text_width,height()-1, x+20+text_width,0 );
    QRegion reg( parr );
    painter.setClipping( TRUE );
    painter.setClipRegion( reg );
    if ( isactive )
        painter.setBackgroundColor( colorGroup().base() );
    else
        painter.setBackgroundColor( colorGroup().background() );
    painter.eraseRect( x, 0, text_width + 20, height() );
    painter.setClipping( FALSE );

    painter.drawLine( x, 0, x + 10, height() - 1 );
    painter.drawLine( x + 10, height() - 1, x + text_width + 10, height() - 1 );
    painter.drawLine( x + 10 + text_width, height() - 1, x + 20 + text_width, 0 );
    if ( !isactive )
        painter.drawLine( x, 0, x + 20 + text_width, 0 );
    if ( ismovemarked )
    {
            if ( m_moveTabFlag == moveTabBefore )
            {
                QPointArray movmark;
                movmark.setPoints(3, x, 0, x + 7, 0, x + 4, 6);
                QBrush oldBrush = painter.brush();
                painter.setBrush( QColor( 0, 0, 0 ) );
                painter.drawPolygon(movmark);
                painter.setBrush( oldBrush );
            }
            else
            {
                QPointArray movmark;
                movmark.setPoints(3, x + 20 + text_width, 0, x + 13 + text_width, 0, x + 16 + text_width, 6);
                QBrush oldBrush = painter.brush();
                painter.setBrush( QColor( 0, 0, 0 ) );
                painter.drawPolygon(movmark);
                painter.setBrush( oldBrush );
            }
    }
    painter.drawText( x + 10, text_y , text );
}


void TabBar::openPopupMenu( const QPoint &_global )
{
  if(!doc->document()->isReadWrite())
    return;

  if ( m_pPopupMenu != 0L )
    delete m_pPopupMenu;
  m_pPopupMenu = new QPopupMenu();

  m_pPopupMenu->insertItem( BarIcon("item_rename", KIllustratorFactory::global()), i18n( "Rename page..." ), this, SLOT( slotRename() ) );
  m_pPopupMenu->insertItem( BarIcon("item_add", KIllustratorFactory::global()), i18n( "Insert page" ), this, SLOT( slotAdd() ) );
  m_pPopupMenu->insertItem( BarIcon("item_remove", KIllustratorFactory::global()),i18n( "Remove page" ), this, SLOT( slotRemove() ) );
  m_pPopupMenu->popup( _global );
}

void TabBar::slotRename()
{
    // Store the current name of the active page
    GPage* page = doc->activePage();

    bool ok;
    QString activeName = page->name();
    QString newName = KLineEditDlg::getText( i18n("Page Name"), activeName, &ok, this );

    // Have a different name ?
    if ( ok ) // User pushed an OK button.
    {
        if ( (newName.stripWhiteSpace()).isEmpty() ) // Page name is empty.
        {
            KNotifyClient::beep();
            KMessageBox::information( this, i18n("Page name cannot be empty."), i18n("Change page name") );
            // Recursion
            slotRename();
        }
        else if ( newName != activeName ) // Page name changed.
        {
             /* // Is the name already used
             if ( !page->setTableName( newName ) )
             {
                KNotifyClient::beep();
                KMessageBox::information( this, i18n("This name is already used."), i18n("Change page name") );
                 // Recursion
                 slotRename();
             }*/
             page->setName( newName );
             update();
             doc->setModified( true );
        }
    }
}

void TabBar::mousePressEvent( QMouseEvent* _ev )
{
  int old_active = activeTab;

  QPainter painter;
  painter.begin( this );

  int i = 1;
  int x = 0;
  QString text;
  const char *active_text = 0L;

  for(QListIterator<GPage> it(doc->getPages()); it.current(); ++it)
  {
    text = ((GPage *)it)->name();
    QFontMetrics fm = painter.fontMetrics();
    int text_width = fm.width( text );

    if ( i >= leftTab )
    {
      if ( x <= _ev->pos().x() && _ev->pos().y() <= x + 20 + text_width )
      {
        activeTab = i;
        active_text = text.ascii();
      }

      x += 10 + text_width;
    }
    i++;
  }

  painter.end();

  if ( activeTab != old_active )
  {
    setActiveTab();
    repaint( false );
    emit tabChanged( activeTab );
  }

  if ( _ev->button() == LeftButton )
  {
    m_moveTabFlag = moveTabBefore;
  }
  else
    if ( _ev->button() == RightButton )
    {
      openPopupMenu( _ev->globalPos() );
    }
}


void TabBar::mouseReleaseEvent( QMouseEvent* _ev )
{
  if(!doc->document()->isReadWrite())
    return;

  if ( _ev->button() == LeftButton && m_moveTab != 0 )
  {
/*        m_pView->doc()->map()->movePage( (*tabsList.at( activeTab - 1 )),
                                          (*tabsList.at( m_moveTab - 1 )),
                                          m_moveTabFlag == moveTabBefore );*/
    moveTab( activeTab - 1, m_moveTab - 1, m_moveTabFlag == moveTabBefore );

    m_moveTabFlag = moveTabNo;
    if ( activeTab < m_moveTab && m_moveTabFlag == moveTabBefore )
      m_moveTab--;
    activeTab = m_moveTab;

    m_moveTab = 0;
    repaint( false );
  }
}

void TabBar::mouseMoveEvent( QMouseEvent* _ev )
{
  if(!doc->document()->isReadWrite())
    return;

  if ( m_moveTabFlag == 0)
    return;

  QPainter painter;
  painter.begin( this );

    if ( _ev->pos().x() > size().width() )
    {
/*      int i = tabsList.count();
      if ( activeTab != i && m_moveTab != i && activeTab != i - 1 )
      {
        m_moveTabFlag = moveTabAfter;
        m_moveTab = tabsList.count();
        repaint( false );
      }
      if ( m_rightTab != (int)tabsList.count() && m_autoScroll == 0 )
      {
        kdDebug(0) << "3"<< endl;
        m_autoScroll = autoScrollRight;
        m_moveTab = leftTab;
        scrollRight();
        m_pAutoScrollTimer->start( 400 );
      }*/
    }
    else // ftf
    {
      int i = 1;
      int x = 0;
      for(QListIterator<GPage> it(doc->getPages()); it.current(); ++it)
      {
        QFontMetrics fm = painter.fontMetrics();
        int text_width = fm.width( ((GPage *)it)->name() );

        if ( i >= leftTab )
        {
          if ( x <= _ev->pos().x() && _ev->pos().x() <= x + 20 + text_width )
          {
            if ( ( activeTab != i && activeTab != i - 1 && m_moveTab != i ) || m_moveTabFlag == moveTabAfter )
            {
              m_moveTabFlag = moveTabBefore;
              m_moveTab = i;
              repaint( false );
            }
            else
	      if ( (m_moveTab != i && m_moveTab != 0) || (activeTab == i - 1 && m_moveTab != 0) )
              {
                m_moveTab = 0;
                repaint( false );
              }
          }
          x += 10 + text_width;
        }
        i++;
      }
      --i;

      if ( x + 10 <= _ev->pos().x() && _ev->pos().x() < size().width() )
      {
        if ( activeTab != i && m_moveTabFlag != moveTabAfter )
        {
          m_moveTabFlag = moveTabAfter;
          m_moveTab = i;
          repaint( false );
        }
      }
    }
  painter.end();
}

void TabBar::mouseDoubleClickEvent( QMouseEvent*  )
{
  if(!doc->document()->isReadWrite())
    return;
  slotRename();
}

#include "TabBar.moc"
