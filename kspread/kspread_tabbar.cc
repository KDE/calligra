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

#include <qpntarry.h>
#include <stdio.h>
#include <stdlib.h>

#include "kspread_view.h"
#include "kspread_table.h"
#include "kspread_dlg_tabname.h"
#include "kspread_tabbar.h"

KSpreadTabBar::KSpreadTabBar( KSpreadView *_parent ) : QWidget( (QWidget *)_parent )
{
    m_pView = _parent;

    leftTab = 1;
    activeTab = 0;
}

void KSpreadTabBar::addTab( const QString& _text )
{
    tabsList.append( _text );
    
    repaint();
}

void KSpreadTabBar::removeTab( const QString& _text )
{
    int i = tabsList.findIndex( _text );
    if ( i == -1 )
    {
	printf("ERROR: KSpreadTable '%s' not found\n", _text.ascii() );
	return;
    }
    
    if ( activeTab == i + 1 )
	activeTab = i;

    if ( activeTab == 0 )
	leftTab = 1;
    else if ( leftTab > activeTab )
	leftTab = activeTab;
    
    tabsList.remove( _text );

    repaint();
}

void KSpreadTabBar::removeAllTabs()
{
    tabsList.clear();
    activeTab = 0;
    leftTab = 1;
    
    repaint();
}

void KSpreadTabBar::scrollLeft()
{
    if ( tabsList.count() == 0 )
	return;
    
    if ( leftTab == 1 )
	return;
    
    leftTab--;
    repaint();
}

void KSpreadTabBar::scrollRight()
{
    if ( tabsList.count() == 0 )
	return;
    
    if ( (unsigned int )leftTab == tabsList.count() )
	return;
    
    leftTab++;
    repaint();
}

void KSpreadTabBar::scrollFirst()
{
    if ( tabsList.count() == 0 )
	return;
    
    if ( leftTab == 1 )
	return;
    
    leftTab = 1;
    repaint();
}

void KSpreadTabBar::scrollLast()
{
    if ( tabsList.count() == 0 )
	return;
    
    if ( (unsigned int )leftTab == tabsList.count() )
	return;
    
    leftTab = tabsList.count();
    repaint();
}

void KSpreadTabBar::setActiveTab( const QString& _text )
{
    int i = tabsList.findIndex( _text );
    if ( i == -1 )
	return;
    
    activeTab = i + 1;
    repaint();
}

void KSpreadTabBar::paintEvent( QPaintEvent* )
{
    if ( tabsList.count() == 0 )
    {
	erase();
	return;
    }
    
    QPainter painter;
    painter.begin( this );

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
	    paintTab( painter, x, text, text_width, text_y, FALSE );
	    x += 10 + text_width;
	}
	
	i++;
    }

//    if ( active_text != 0L )
	paintTab( painter, active_x, active_text, active_width, active_y, TRUE );
	    
    painter.end();
}

void KSpreadTabBar::paintTab( QPainter & painter, int x, const QString& text, int text_width, int text_y, bool isactive )
{
    QPointArray parr;
    parr.setPoints( 4, x,0, x+10,height()-1, x+10+text_width,height()-1, x+20+text_width,0 );
    QRegion reg( parr );
    painter.setClipping( TRUE );
    painter.setClipRegion( reg );
    if ( isactive )
	painter.setBackgroundColor( white );
    else
	painter.setBackgroundColor( lightGray );
    painter.eraseRect( x, 0, text_width + 20, height() );
    painter.setClipping( FALSE );
    
    painter.drawLine( x, 0, x + 10, height() - 1 );
    painter.drawLine( x + 10, height() - 1, x + text_width + 10, height() - 1 );
    painter.drawLine( x + 10 + text_width, height() - 1, x + 20 + text_width, 0 );
    if ( !isactive )
	painter.drawLine( x, 0, x + 20 + text_width, 0 );
    
    painter.drawText( x + 10, text_y , text );
}

void KSpreadTabBar::mousePressEvent( QMouseEvent* _ev )
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
		active_text = text;
	    }
	    
	    x += 10 + text_width;
	}
	i++;
    }
	    
    painter.end();

    if ( activeTab != old_active )
    {
	repaint();
	emit tabChanged( active_text );
    }
}

void KSpreadTabBar::mouseDoubleClickEvent( QMouseEvent* _ev )
{
    QString activeName;
    QString newName;

    KSpreadTable* table = m_pView->activeTable();
    activeName = table->name();

    KSpreadTableName tndlg( (KSpreadView *)this->parentWidget(), "TableName" , activeName );
    if ( tndlg.exec() )
    {
        if ( ( newName = tndlg.tableName() ) != activeName )
        {
            table->setName( newName );
            QStringList::Iterator it = tabsList.find( activeName );
            (*it) = newName;
	    repaint();
        }	
    }
}

#include "kspread_tabbar.moc"
