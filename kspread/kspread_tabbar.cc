#include <qpntarry.h>
#include <stdio.h>
#include <stdlib.h>

#include "kspread_tabbar.h"

KSpreadTabBar::KSpreadTabBar( QWidget *_parent ) : QWidget( _parent )
{
    leftTab = 1;
    activeTab = 0;
}

void KSpreadTabBar::addTab( const char *_text )
{
    tabsList.append( _text );
    
    repaint();
}

void KSpreadTabBar::removeTab( const char *_text )
{
    int i = tabsList.find( _text );
    if ( i == -1 )
    {
	printf("ERROR: KSpreadTable '%s' not found\n", _text );
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

void KSpreadTabBar::setActiveTab( const char *_text )
{
    int i = tabsList.find( _text );
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
	paintTab( painter, -10, "", 0, 0, FALSE );
    
    int i = 1;
    int x = 0;
    char *text;
    char *active_text = 0L;
    int active_x = -1;
    int active_width;
    int active_y;
    
    for ( text = tabsList.first(); text != 0L; text = tabsList.next() )
    {
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

    if ( active_text != 0L )
	paintTab( painter, active_x, active_text, active_width, active_y, TRUE );
	    
    painter.end();
}

void KSpreadTabBar::paintTab( QPainter & painter, int x, char *text, int text_width, int text_y, bool isactive )
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
    char *text;
    const char *active_text = 0L;
    
    for ( text = tabsList.first(); text != 0L; text = tabsList.next() )
    {
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

#include "kspread_tabbar.moc"
