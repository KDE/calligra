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
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
 */
#include <qapplication.h>
#include <qmessagebox.h>
#include <qpointarray.h>
#include <qstring.h>
#include <qpopupmenu.h>

#include <stdio.h>
#include <stdlib.h>

#include <klocale.h>
#include <kiconloader.h>
#include <kmessagebox.h>
#include <klineeditdlg.h>
#include <knotifyclient.h>

#include "kivio_canvas.h"
#include "kivio_doc.h"
#include "kivio_view.h"
#include "kivio_page.h"
#include "kivio_tabbar.h"
#include "kivio_map.h"

KivioTabBar::KivioTabBar( QWidget* parent, KivioView* view )
: QWidget(parent)
{
  m_pView = view;
  m_pPopupMenu = 0L;

  m_pAutoScrollTimer = new QTimer(this);
  connect( m_pAutoScrollTimer, SIGNAL(timeout()), SLOT(slotAutoScroll()));

  leftTab = 1;
  m_rightTab = 0;
  activeTab = 0;
  m_moveTab = 0;
  m_autoScroll = 0;

  setFixedHeight(16);
}

void KivioTabBar::addTab( const QString& _text )
{
  tabsList.append( _text );
  update();
}

void KivioTabBar::removeTab( const QString& _text )
{
    int i = tabsList.findIndex( _text );
    if ( i == -1 )
    {
        return;
    }

    if ( activeTab == i + 1 )
        activeTab = i;

    if ( activeTab == 0 )
        leftTab = 1;
    else if ( leftTab > activeTab )
        leftTab = activeTab;

    tabsList.remove( _text );

    update();
}

void KivioTabBar::removeAllTabs()
{
    tabsList.clear();
    pagehide.clear();
    activeTab = 0;
    leftTab = 1;

    update();
}

void KivioTabBar::moveTab( int _from, int _to, bool _before )
{
    QStringList::Iterator it;

    it = tabsList.at( _from );
    const QString tabname = *it;

    if ( !_before )
        ++_to;

    if ( _to > (int)tabsList.count() )
    {
        tabsList.append( tabname );
        tabsList.remove( it );
    }
    else if ( _from < _to )
    {
        tabsList.insert( tabsList.at( _to ), tabname );
        tabsList.remove( it );
    }
    else
    {
        tabsList.remove( it );
        tabsList.insert( tabsList.at( _to ), tabname );
    }

    repaint();
}


void KivioTabBar::scrollLeft()
{
    if ( tabsList.count() == 0 )
        return;

    if ( leftTab == 1 )
        return;

    leftTab--;
    repaint( false );
}

void KivioTabBar::scrollRight()
{
    if ( tabsList.count() == 0 )
        return;

        if ( m_rightTab == (int)tabsList.count() )
        return;

    if ( (unsigned int )leftTab == tabsList.count() )
        return;

    leftTab++;
    repaint( false );
}

void KivioTabBar::scrollFirst()
{
    if ( tabsList.count() == 0 )
        return;

    if ( leftTab == 1 )
        return;

    leftTab = 1;
    repaint( false );
}

void KivioTabBar::scrollLast()
{
    if ( tabsList.count() == 0 )
        return;

    QPainter painter;
    painter.begin( this );

    int i = tabsList.count();
    int x = 0;

        if ( m_rightTab == i )
        return;

    QStringList::Iterator it;
        it = tabsList.end();
        do
        {
                --it;
                QFontMetrics fm = painter.fontMetrics();

            x += 10 + fm.width( *it );
                if ( x > width() )
                {
                        leftTab = i + 1;
                        break;
                }
                --i;
    } while ( it != tabsList.begin() );
    painter.end();
    repaint( false );
}

void KivioTabBar::setActiveTab( const QString& _text )
{
    int i = tabsList.findIndex( _text );
    if ( i == -1 )
        return;

    if ( i + 1 == activeTab )
        return;

    activeTab = i + 1;
    repaint( false );

    emit tabChanged( _text );
}

void KivioTabBar::slotRemove( )
{
    if ( (m_pView->doc()->map()->count() <= 1 )||(tabsList.count()<=1) )
    {
        QApplication::beep();
        KMessageBox::error( this,i18n("You cannot delete the only page of the map."), i18n("Remove page") ); // FIXME bad english? no english!
        return;
    }
    QApplication::beep();
    int ret = KMessageBox::warningYesNo( this, i18n("You are going to remove the active page.\nDo you want to continue?"), i18n("Remove page"));
    if ( ret == 3 )
    {
        KivioPage *tbl = m_pView->activePage();
        m_pView->doc()->map()->removePage( tbl );
                m_pView->removePage(tbl);
        delete tbl;
    }
}

void KivioTabBar::slotAdd()
{
    m_pView->insertPage();
    m_pView->activePage()->setHidden(false);
}

void KivioTabBar::paintEvent( QPaintEvent* )
{
    if ( tabsList.count() == 0 )
    {
        erase();
        return;
    }

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

    QStringList::Iterator it;
    for ( it = tabsList.begin(); it != tabsList.end(); ++it )
    {
        text = *it;
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


void KivioTabBar::paintTab( QPainter & painter, int x, const QString& text, int text_width, int text_y,
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


void KivioTabBar::openPopupMenu( const QPoint &_global )
{
    if ( !m_pView->koDocument()->isReadWrite() )
      return;

    if ( m_pPopupMenu != 0L )
        delete m_pPopupMenu;
    m_pPopupMenu = new QPopupMenu();

    m_pPopupMenu->insertItem( BarIcon("item_rename"), i18n( "Rename page..." ), this, SLOT( slotRename() ) );
    m_pPopupMenu->insertItem( BarIcon("item_add"), i18n( "Insert page" ), this, SLOT( slotAdd() ) );
    m_pPopupMenu->insertItem( BarIcon("item_remove"),i18n( "Remove page" ), this, SLOT( slotRemove() ) );
    m_pPopupMenu->popup( _global );
}

void KivioTabBar::renameTab( const QString& old_name, const QString& new_name )
{
    QStringList::Iterator it = tabsList.find( old_name );
    (*it) = new_name;

    update();
}

void KivioTabBar::slotRename()
{
    // Store the current name of the active page
    KivioPage* page = m_pView->activePage();

    bool ok;
    QString activeName = page->pageName();
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
             // Is the name already used
             if ( !page->setPageName( newName ) )
             {
                KNotifyClient::beep();
                KMessageBox::information( this, i18n("This name is already used."), i18n("Change page name") );
                 // Recursion
                 slotRename();
             }
             m_pView->koDocument()->setModified( true );
        }
    }
}

void KivioTabBar::mousePressEvent( QMouseEvent* _ev )
{
    int old_active = activeTab;

    if ( tabsList.count() == 0 )
    {
        erase();
        return;
    }

    QPainter painter;
    painter.begin( this );

    int i = 1;
    int x = 0;
    QString text;
    const char *active_text = 0L;

    QStringList::Iterator it;
    for ( it = tabsList.begin(); it != tabsList.end(); ++it )
    {
        text = *it;
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
        repaint( false );
        emit tabChanged( active_text );
    }

    if ( _ev->button() == LeftButton )
    {
        m_moveTabFlag = moveTabBefore;
    }
    else if ( _ev->button() == RightButton )
    {
        openPopupMenu( _ev->globalPos() );
    }
}


void KivioTabBar::mouseReleaseEvent( QMouseEvent* _ev )
{
    if ( !m_pView->koDocument()->isReadWrite() )
        return;

    if ( _ev->button() == LeftButton && m_moveTab != 0 )
    {
        if ( m_autoScroll != 0 )
        {
            m_pAutoScrollTimer->stop();
            m_autoScroll = 0;
        }
        m_pView->doc()->map()->movePage( (*tabsList.at( activeTab - 1 )),
                                          (*tabsList.at( m_moveTab - 1 )),
                                          m_moveTabFlag == moveTabBefore );
        moveTab( activeTab - 1, m_moveTab - 1, m_moveTabFlag == moveTabBefore );

        m_moveTabFlag = moveTabNo;
        if ( activeTab < m_moveTab && m_moveTabFlag == moveTabBefore )
            m_moveTab--;
        activeTab = m_moveTab;

        m_moveTab = 0;
        repaint( false );
    }
}

void KivioTabBar::slotAutoScroll( )
{
    if ( m_autoScroll == autoScrollLeft && leftTab > 1 )
    {
        m_moveTab = leftTab - 1;
        scrollLeft();
    }
    else if ( m_autoScroll == autoScrollRight )
    {
        scrollRight();
    }
    if ( leftTab <= 1 )
    {
        m_pAutoScrollTimer->stop();
        m_autoScroll = 0;
    }
}


void KivioTabBar::mouseMoveEvent( QMouseEvent* _ev )
{
    if ( !m_pView->koDocument()->isReadWrite() )
         return;
    if ( m_moveTabFlag == 0)
        return;

    QPainter painter;
    painter.begin( this );

    if ( _ev->pos().x() < 0 && leftTab > 1 && m_autoScroll == 0 )
    {
        m_autoScroll = autoScrollLeft;
        m_moveTab = leftTab - 1;
        scrollLeft();
        m_pAutoScrollTimer->start( 400 );
    }
    else if ( _ev->pos().x() > size().width() )
    {
        int i = tabsList.count();
        if ( activeTab != i && m_moveTab != i && activeTab != i - 1 )
        {
            m_moveTabFlag = moveTabAfter;
            m_moveTab = tabsList.count();
            repaint( false );
        }
        if ( m_rightTab != (int)tabsList.count() && m_autoScroll == 0 )
        {
            m_autoScroll = autoScrollRight;
            m_moveTab = leftTab;
            scrollRight();
            m_pAutoScrollTimer->start( 400 );
        }
    }
    else // ftf
    {
        int i = 1;
            int x = 0;

        QStringList::Iterator it;
            for ( it = tabsList.begin(); it != tabsList.end(); ++it )
        {
            QFontMetrics fm = painter.fontMetrics();
            int text_width = fm.width( *it );

            if ( i >= leftTab )
            {
                if ( x <= _ev->pos().x() && _ev->pos().x() <= x + 20 + text_width )
                {
                    if ( m_autoScroll != 0 )
                    {
                        m_pAutoScrollTimer->stop();
                        m_autoScroll = 0;
                    }

                    if ( ( activeTab != i && activeTab != i - 1 && m_moveTab != i ) || m_moveTabFlag == moveTabAfter )
                    {
                        m_moveTabFlag = moveTabBefore;
                        m_moveTab = i;
                        repaint( false );
                    }
                    else if ( (m_moveTab != i && m_moveTab != 0) || (activeTab == i - 1 && m_moveTab != 0) )
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

void KivioTabBar::mouseDoubleClickEvent( QMouseEvent*  )
{
    if ( !m_pView->koDocument()->isReadWrite() )
        return;
    slotRename();
}

void KivioTabBar::hidePage()
{
    if ( tabsList.count() ==  1)
    {
        KMessageBox::error( this, i18n("You cannot hide the last visible page.") );
        return;
    }
    else
    {
        KivioPage* page = m_pView->activePage();
        m_pView->activePage()->setHidden(true);
        QString activeName = page->pageName();
        removeTab( activeName );
        pagehide.append( activeName );
        // m_pView->setActivePage( m_pView->doc()->map()->findPage( tabsList.first()) );

        emit tabChanged( tabsList.first() );
    }
}

void KivioTabBar::showPage(const QString& text)
{
    pagehide.remove( text );
    addTab( text );

    m_pView->activePage()->setHidden( false );
}

void KivioTabBar::addHiddenTab(const QString & text)
{
    pagehide.append( text );
}
#include "kivio_tabbar.moc"
