/* This file is part of the KDE project
   Copyright (C) 2002, Benoit Vautrin <benoit.vautrin@free.fr>

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

#include "kocontexthelp.h"

#include <qpainter.h>
#include <qregion.h>
#include <qfont.h>
#include <qlabel.h>
#include <qlayout.h>

#include <kpixmap.h>
#include <klocale.h>
#include <kdebug.h>
#include <kiconloader.h>

#define HELPWIDTH 140 
#define HELPHEIGHT 160

KoVerticalLabel::KoVerticalLabel( QWidget* parent, const char* name )
		: QWidget( parent, name, Qt::WRepaintNoErase )
{
	QFont font( font() );
	font.setPointSize( font.pointSize() + 2 );
	font.setBold( true );
	setFont( font );
	setBackgroundMode( PaletteLight );
} // KoVerticalLabel::KoVerticalLabel

KoVerticalLabel::~KoVerticalLabel()
{
} // KoVerticalLabel::~KoVerticalLabel

void KoVerticalLabel::setText( const QString& text )
{
	m_text = text;
	QFontMetrics fm( font() );
	setMinimumSize( fm.height() + 2, fm.width( m_text ) + 4 );
	update();
} // KoVerticalLabel::setText

void KoVerticalLabel::paintEvent( QPaintEvent* )
{
	KPixmap pm;
	pm.resize( height(), width() );
	QPainter p( &pm );
	p.fillRect( 0, 0, height(), width(), colorGroup().background() );
	p.setFont( font() );
	p.drawText( 0, 0, height(), width(), AlignCenter, m_text );
	p.end();
	QPainter ap( this );
	ap.rotate( 270. );
	ap.translate( -height(), 0 );
	ap.drawPixmap( 0, 0, pm );
} // KoVerticalLabel::paintEvent

static unsigned char upbits[]   = { 0xc, 0x1e, 0x3f };
static unsigned char downbits[] = { 0x3f, 0x1e, 0xc };

KoHelpNavButton::KoHelpNavButton( NavDirection d, QWidget* parent )
		: QWidget( parent )
{
	m_pressed = false;
	m_bitmap = QBitmap( 6, 3, ( d == Up ? upbits : downbits ), true );
	m_bitmap.setMask( m_bitmap );
	setFixedSize( 8, 5 );
	setBackgroundMode( PaletteLight );
} // KoHelpNavButton::KoHelpNavButton

void KoHelpNavButton::paintEvent( QPaintEvent* )
{
	QPainter p( this );
	if ( isEnabled() )
	{
		if ( m_pressed )
			p.setPen( Qt::black );
		else
			p.setPen( colorGroup().highlight() );
		p.drawPixmap( 1, 1, m_bitmap );
	}
} // KoHelpNavButton::paintEvent

void KoHelpNavButton::enterEvent( QEvent* )
{
	if ( isEnabled() )
		emit pressed();
	m_pressed = true;
	update();
} // KoHelpNavButton::enterEvent

void KoHelpNavButton::leaveEvent( QEvent* )
{
	if ( isEnabled() )
		emit released();
	m_pressed = false;
	update();
} // KoHelpNavButton::leaveEvent

static unsigned char notstickybits[] = { 0x8, 0x1e, 0xc, 0xa, 0x1 };
static unsigned char stickybits[]    = { 0xe, 0x11, 0x15, 0x11, 0xe };
static unsigned char closebits[]     = { 0x11, 0xa, 0x4, 0xa, 0x11 };

KoTinyButton::KoTinyButton( Action a, QWidget* parent )
		: QWidget( parent ), m_action( a )
{
	m_pressed = false;
	m_toggled = false;
	switch ( a )
	{
		case Sticky:
				m_bitmap = QBitmap( 5, 5, notstickybits, true );
			break;
			
		default:
			m_bitmap = QBitmap( 5, 5, closebits, true );
	}
	m_bitmap.setMask( m_bitmap );
	setMinimumSize( 7, 7 );
	setBackgroundMode( PaletteBackground );
}; // KoTinyButton::KoTinyButton

void KoTinyButton::paintEvent( QPaintEvent* )
{
	QPainter p( this );
	if ( isEnabled() )
	{
		if ( m_pressed )
			p.setPen( Qt::black );
		else
			p.setPen( colorGroup().highlight() );
		p.drawPixmap( width() / 2 - 2, 1, m_bitmap );
	}
}; // KoTinyButton::paintEvent

void KoTinyButton::mousePressEvent( QMouseEvent* )
{
	if ( isEnabled() )
	{
		m_pressed = true;
		update();
	}
}; // KoTinyButton::mousePressEvent

void KoTinyButton::mouseReleaseEvent( QMouseEvent* )
{
	if ( isEnabled() && m_pressed )
	{
		m_pressed = false;
		emit( clicked() );
		if ( ( m_action == Sticky ) )
		{
			m_toggled = !m_toggled;
			emit( toggled( m_toggled ) );
			//switch ( m_action )
			//{
			//	case Sticky:
						m_bitmap = QBitmap( 5, 5, ( m_toggled ? stickybits : notstickybits ), true );
			//}
			m_bitmap.setMask( m_bitmap );
		}
		update();
	}
}; // KoTinyButton::mouseReleaseEvent

KoHelpWidget::KoHelpWidget( QString help, QWidget* parent )
		: QWidget( parent )
{
	QGridLayout* layout = new QGridLayout( this, 3, 3 );
	layout->setMargin( 2 );
	layout->addWidget( m_upButton = new KoHelpNavButton( KoHelpNavButton::Up, this ), 0, 1, AlignHCenter );
	layout->addWidget( m_helpViewport = new QWidget( this ), 1, 1, AlignVCenter | AlignHCenter );
	layout->addWidget( m_downButton = new KoHelpNavButton( KoHelpNavButton::Down, this ), 2, 1, AlignHCenter );
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
	
	setBackgroundMode( PaletteLight );
	
	connect( m_upButton, SIGNAL( pressed() ), this, SLOT( startScrollingUp() ) );
	connect( m_downButton, SIGNAL( pressed() ), this, SLOT( startScrollingDown() ) );
	connect( m_upButton, SIGNAL( released() ), this, SLOT( stopScrolling() ) );
	connect( m_downButton, SIGNAL( released() ), this, SLOT( stopScrolling() ) );
} // KoHelpWidget::KoHelpWidget

void KoHelpWidget::updateButtons()
{
	m_upButton->setEnabled( m_ypos < 0 );
	m_downButton->setEnabled( HELPHEIGHT - m_helpLabel->height() - m_ypos < 0 );
} // KoHelpWidget::updateButtons

void KoHelpWidget::setText( QString text )
{
	m_helpLabel->setText( text );
	m_helpLabel->setFixedHeight( m_helpLabel->heightForWidth( HELPWIDTH ) );
	m_helpLabel->move( 0, 0 );
	m_ypos = 0;
	updateButtons();
} // KoHelpWidget::setText

void KoHelpWidget::startScrollingUp()
{
	if ( !m_upButton->isEnabled() )
		return;
	m_scrollDown = false;
	startTimer( 80 );
} // KoHelpWidget::startScrollingUp

void KoHelpWidget::startScrollingDown()
{
	if ( !m_downButton->isEnabled() )
		return;
	m_scrollDown = true;
	startTimer( 80 );
} // KoHelpWidget::startScrollingDown

void KoHelpWidget::scrollUp()
{
	m_ypos += 2;
	m_helpViewport->scroll( 0, 2 );
	m_helpViewport->update();
	updateButtons();
	if ( m_ypos > 0 ) 
		stopScrolling();
} // KoHelpWidget::scrollUp()

void KoHelpWidget::scrollDown()
{
	m_ypos -= 2;
	m_helpViewport->scroll( 0, -2 );
	m_helpViewport->update();
	updateButtons();
	if ( HELPHEIGHT - m_helpLabel->height() - m_ypos > 0 )
		stopScrolling();
} // KoHelpWidget::scrollUp()

void KoHelpWidget::timerEvent( QTimerEvent* )
{
	if ( m_scrollDown )
		scrollDown();
	else
		scrollUp();
} // KoHelpWidget::timerEvent

void KoHelpWidget::stopScrolling()
{
	killTimers();
} // KoHelpWidget::stopScrolling

KoContextHelpPopup::KoContextHelpPopup( QWidget* parent )
		: QWidget( parent, "", WType_Dialog | WStyle_Customize | WStyle_NoBorder )
{
	QGridLayout* layout = new QGridLayout( this );
	QHBoxLayout* buttonLayout;
	layout->addWidget( m_helpIcon = new QLabel( this ), 0, 0 );
	layout->addWidget( m_helpTitle = new KoVerticalLabel( this ), 1, 0 );
	layout->addLayout( buttonLayout = new QHBoxLayout( layout ), 2, 0 );
	layout->addMultiCellWidget( m_helpViewer = new KoHelpWidget( "", this ), 0, 2, 1, 1 );
	buttonLayout->add( m_close = new KoTinyButton( KoTinyButton::Close, this ) ); 
	buttonLayout->add( m_sticky = new KoTinyButton( KoTinyButton::Sticky, this ) );
	layout->setMargin( 3 );
	layout->setSpacing( 1 );
	layout->setRowStretch( 1, 1 );
	buttonLayout->setSpacing( 1 );
	
	m_isSticky = false;
	setFocusPolicy( StrongFocus );
	
	connect( m_close, SIGNAL( clicked() ), this, SIGNAL( wantsToBeClosed() ) );
	connect( m_sticky, SIGNAL( toggled( bool ) ), this, SLOT( setSticky( bool ) ) );
} // KoContextHelpPopup::KoContextHelpPopup

KoContextHelpPopup::~KoContextHelpPopup()
{
} // KoContextHelpPopup::~KoContextHelpPopup

void KoContextHelpPopup::setContextHelp( const QString& title, const QString& text, const QPixmap* icon )
{
	m_helpIcon->setPixmap( icon ? *icon : BarIcon( "help" ) );
	m_helpTitle->setText( title );
	m_helpViewer->setText( text );
} // KoContextHelpPopup::updateHelp

void KoContextHelpPopup::mousePressEvent( QMouseEvent* e )
{
	m_mousePos = e->globalPos() - pos();
} // KoContextHelpPopup::mousePressEvent

void KoContextHelpPopup::mouseMoveEvent( QMouseEvent* e )
{
	move( e->globalPos() - m_mousePos );
} // KoContextHelpPopup::mouseMoveEvent

void KoContextHelpPopup::resizeEvent( QResizeEvent* )
{
	QRegion mask(0, 0, width(), height());
	mask -= QRegion( 0, 0, 2, 2 ) + QRegion( 0, 2, 1, 2 ) + QRegion( 2, 0, 2, 1 );
	mask -= QRegion( width() - 2, 0, 2, 2 ) + QRegion( width() - 1, 2, 1, 2 ) + QRegion( width() - 4, 0, 2, 1 );
	mask -= QRegion( 0, height() - 2, 2, 2 ) + QRegion( 0, height() - 4, 1, 2 ) + QRegion( 2, height() - 1, 2, 1 );
	mask -= QRegion( width() - 2, height() - 2, 2, 2 ) + QRegion( width() - 1, height() - 4, 1, 2 ) + QRegion( width() - 4, height() - 1, 2, 1 );
	setMask( mask );
} // KoContextHelpPopup::resizeEvent

void KoContextHelpPopup::paintEvent( QPaintEvent* )
{
	QPainter p( this );
	p.fillRect( 0, 0, width(), height(), colorGroup().light() );
	p.setPen( black );
	p.drawRect( 0, 0, width(), height() );
	p.drawLine( 1, 2, 1, 3 );
	p.drawLine( 2, 1, 3, 1 );
	p.drawLine( width() - 2, 2, width() - 2, 3 );
	p.drawLine( width() - 3, 1, width() - 4, 1 );
	p.drawLine( 1, height() - 3, 1, height() - 4 );
	p.drawLine( 2, height() - 2, 3, height() - 2 );
	p.drawLine( width() - 2, height() - 3, width() - 2, height() - 4 );
	p.drawLine( width() - 3, height() - 2, width() - 4, height() - 2 );
} // KoContextHelpPopup::paintEvent

void KoContextHelpPopup::windowActivationChange( bool )
{
	if ( !isActiveWindow() && !m_isSticky )
		emit wantsToBeClosed();
} // KoContestHelpPopup::windowActivationChange

void KoContextHelpPopup::keyPressEvent( QKeyEvent* e )
{
	switch ( e->key() )
	{
		case Key_Up:
				m_helpViewer->startScrollingUp();
			break;
		
		case Key_Down:
				m_helpViewer->startScrollingDown();
			break;
	}
} // KoContextHelpPopup::keyPressEvent

void KoContextHelpPopup::keyReleaseEvent( QKeyEvent* e )
{
	switch ( e->key() )
	{
		case Key_Up:
		case Key_Down:
				m_helpViewer->stopScrolling();
			break;
			
		case Key_Escape:
				emit wantsToBeClosed();
			break;
	}
} // KoContextHelpPopup::keyPressEvent

KoContextHelpAction::KoContextHelpAction( KActionCollection* parent, QWidget* popupParent )
		: KToggleAction( i18n( "Context help" ), BarIcon( "help" ), KShortcut( "CTRL+SHIFT+F1" ), 0, 0, parent, "help_context" )
{
	m_popup = new KoContextHelpPopup( popupParent );
	connect( m_popup, SIGNAL( wantsToBeClosed() ), this, SLOT( closePopup() ) );
	connect( this, SIGNAL( toggled( bool ) ), m_popup, SLOT( setShown( bool ) ) );
} // KoContextHelpAction::KoContextHelpAction

KoContextHelpAction::~KoContextHelpAction()
{
	delete m_popup;
} // KoContextHelpAction::~KoContextHelpAction

void KoContextHelpAction::updateHelp( const QString& title, const QString& text, const QPixmap* icon )
{
	m_popup->setContextHelp( title, text, icon );
} // KoContextHelpAction::updateHelp

void KoContextHelpAction::closePopup()
{
	activate();
	setChecked( false ); // For a unknown reason, this is needed...
} // KoContextHelpAction::closePopup

KoContextHelpDocker::KoContextHelpDocker( QWidget* parent, const char* name )
		: QDockWindow( parent, name )
{
	QGridLayout* layout = new QGridLayout( this );
	layout->addWidget( m_helpIcon = new QLabel( this ), 0, 0 );
	layout->addWidget( m_helpTitle = new KoVerticalLabel( this ), 1, 0 );
	layout->addMultiCellWidget( m_helpViewer = new KoHelpWidget( "", this ), 0, 1, 1, 1 );
	layout->setMargin( 2 );
	layout->setSpacing( 1 );
	layout->setRowStretch( 1, 1 );
} // KoContextHelpDocker::KoContextHelpDocker

KoContextHelpDocker::~KoContextHelpDocker()
{
} // KoContextHelpDocker::~KoContextHelpDocker

void KoContextHelpDocker::setContextHelp( const QString& title, const QString& text, const QPixmap* icon )
{
	m_helpIcon->setPixmap( icon ? *icon : BarIcon( "help" ) );
	m_helpTitle->setText( title );
	m_helpViewer->setText( text );
} // KoContextHelpDocker::updateHelp

#include "kocontexthelp.moc"
