/* This file is part of the KDE project
   Copyright (C) 2001, The Karbon Developers
   Copyright (C) 2002, The Karbon Developers

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

#define HELPWIDTH 150
#define HELPHEIGHT 100

#include <qlabel.h>
#include <qlayout.h>
#include <qpainter.h>

#include <koMainWindow.h>

#include "vcontexthelpdocker.h"
#include "karbon_view.h"
#include "vtool.h"

static unsigned char upbits[]   = { 0xc, 0x1e, 0x3f };
static unsigned char downbits[] = { 0x3f, 0x1e, 0xc };

VHelpButton::VHelpButton( unsigned char* bits, QWidget* parent )
		: QWidget( parent )
{
	m_pressed = false;
	m_bitmap = QBitmap( 6, 3, bits, true );
	m_bitmap.setMask( m_bitmap );
	setFixedSize( 8, 5 );
	setBackgroundMode( PaletteLight );
} // VHelpButton::VHelpButton

void VHelpButton::paintEvent( QPaintEvent* )
{
	QPainter p( this );
	if ( isEnabled() )
	{
		if ( m_pressed )
			p.setPen( Qt::black );
		else
			p.setPen( colorGroup().button() );
		p.drawPixmap( 1, 1, m_bitmap );
	}
} // VHelpButton::paintEvent

void VHelpButton::enterEvent( QEvent* )
{
	if ( isEnabled() )
		emit pressed();
	m_pressed = true;
	update();
} // VHelpButton::enterEvent

void VHelpButton::leaveEvent( QEvent* )
{
	if ( isEnabled() )
		emit released();
	m_pressed = false;
	update();
} // VHelpButton::leaveEvent

VHelpWidget::VHelpWidget( QString help, QWidget* parent )
		: QFrame( parent )
{
	QGridLayout* layout = new QGridLayout( this, 3, 3 );
	layout->setMargin( 2 );
	layout->addWidget( m_upButton = new VHelpButton( upbits, this ), 0, 1, AlignHCenter );
	layout->addWidget( m_helpViewport = new QWidget( this ), 1, 1, AlignVCenter | AlignHCenter );
	layout->addWidget( m_downButton = new VHelpButton( downbits, this ), 2, 1, AlignHCenter );
	layout->addColSpacing( 0, 5 );
	layout->addColSpacing( 2, 5 );
	layout->setColStretch( 1, 1 );
	
	m_helpLabel = new QLabel( m_helpViewport );
	m_helpLabel->setBackgroundMode( PaletteLight );
	m_helpLabel->setFixedWidth( HELPWIDTH );
	m_helpViewport->setFixedSize( HELPWIDTH, HELPHEIGHT );
	m_helpViewport->setBackgroundMode( PaletteLight );
	setText( help );
	updateButtons();
	
	setFrameStyle( Sunken | Box );
	setBackgroundMode( PaletteLight );
	
	connect( m_upButton, SIGNAL( pressed() ), this, SLOT( scrollUp() ) );
	connect( m_downButton, SIGNAL( pressed() ), this, SLOT( scrollDown() ) );
	connect( m_upButton, SIGNAL( released() ), this, SLOT( stopScroll() ) );
	connect( m_downButton, SIGNAL( released() ), this, SLOT( stopScroll() ) );
} // VHelpWidget::VHelpWidget

void VHelpWidget::updateButtons()
{
	m_upButton->setEnabled( ypos < 0 );
	m_downButton->setEnabled( HELPHEIGHT - m_helpLabel->height() - ypos < 0 );
} // VHelpWidget::updateButtons

void VHelpWidget::setText( QString text )
{
	m_helpLabel->setText( text );
	m_helpLabel->setFixedHeight( m_helpLabel->heightForWidth( HELPWIDTH ) );
	m_helpLabel->move( 0, 0 );
	ypos = 0;
	updateButtons();
} // VHelpWidget::setText

void VHelpWidget::scrollUp()
{
	m_scrollDown = false;
	startTimer( 100 );
} // VHelpWidget::scrollUp

void VHelpWidget::scrollDown()
{
	m_scrollDown = true;
	startTimer( 100 );
} // VHelpWidget::scrollDown

void VHelpWidget::timerEvent( QTimerEvent* )
{
	ypos += ( m_scrollDown ? -2 : 2 );
	m_helpViewport->scroll( 0, ( m_scrollDown ? -2 : 2 ) );
	m_helpViewport->update();
	updateButtons();
	if ( ( m_scrollDown && HELPHEIGHT - m_helpLabel->height() - ypos > 0 ) ||
	   ( !m_scrollDown && ypos > 0 ) )
		stopScroll();
} // VHelpWidget::timerEvent

void VHelpWidget::stopScroll()
{
	killTimers();
} // VHelpWidget::stopScroll

VContextHelpDocker::VContextHelpDocker( KarbonView* view )
		: VDocker( view->shell() )
{
	m_helpWidget = new VHelpWidget( i18n( "Context help will appear here." ), this );
	
	setCaption( "Context Help" );
	setWidget( m_helpWidget );
} // VContextHelpDocker::VContextHelpDocker

VContextHelpDocker::~VContextHelpDocker()
{
} // VContextHelpDocker::~VContextHelpDocker

void VContextHelpDocker::manageTool( VTool* tool )
{
	m_helpWidget->setText( tool->contextHelp() );
} // VContextHelpDocker::manageTool

#include "vcontexthelpdocker.moc"
