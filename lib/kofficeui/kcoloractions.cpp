/* This file is part of the KDE libraries
    Copyright (C) 2000 Reginald Stadlbauer <reggie@kde.org>

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Library General Public
    License version 2 as published by the Free Software Foundation.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Library General Public License for more details.

    You should have received a copy of the GNU Library General Public License
    along with this library; see the file COPYING.LIB.  If not, write to
    the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
    Boston, MA 02111-1307, USA.
*/

#include <kcoloractions.h>
#include <kcolordialog.h>
#include <klocale.h>
#include <kpopupmenu.h>
#include <ktoolbar.h>
#include <qdrawutil.h>
#include <qpainter.h>

KColorAction::KColorAction( const QString& text, int accel,
			    QObject* parent, const char* name )
    : KAction( text, accel, parent, name )
{
    typ = TextColor;
    init();
}

KColorAction::KColorAction( const QString& text, int accel,
			    QObject* receiver, const char* slot, QObject* parent,
			    const char* name )
    : KAction( text, accel, receiver, slot, parent, name )
{
    typ = TextColor;
    init();
}

KColorAction::KColorAction( const QString& text, Type type, int accel,
			    QObject* parent, const char* name )
    : KAction( text, accel, parent, name )
{
    typ = type;
    init();
}

KColorAction::KColorAction( const QString& text, Type type, int accel,
			    QObject* receiver, const char* slot, QObject* parent,
			    const char* name )
    : KAction( text, accel, receiver, slot, parent, name )
{
    typ = type;
    init();
}

KColorAction::KColorAction( QObject* parent, const char* name )
    : KAction( parent, name )
{
    typ = TextColor;
    init();
}

void KColorAction::setColor( const QColor &c )
{
    if ( c == col )
	return;

    col = c;
    createPixmap();
}

QColor KColorAction::color() const
{
    return col;
}

void KColorAction::setType( Type t )
{
    if ( t == typ )
	return;

    typ = t;
    createPixmap();
}

KColorAction::Type KColorAction::type() const
{
    return typ;
}

void KColorAction::init()
{
    col = Qt::black;
    createPixmap();
}

void KColorAction::createPixmap()
{
    int r, g, b;
    QCString pix;
    QCString line;

    col.rgb( &r, &g, &b );

    pix = "/* XPM */\n";

    pix += "static char * text_xpm[] = {\n";

    switch ( typ ) {
      case TextColor: {
        pix += "\"20 20 11 1\",\n";
        pix += "\"h c #c0c000\",\n";
        pix += "\"g c #808000\",\n";
        pix += "\"f c #c0c0ff\",\n";
        pix += "\"a c #000000\",\n";
        pix += "\"d c #ff8000\",\n";
        pix += "\". c none\",\n";
        pix += "\"e c #0000c0\",\n";
        pix += "\"i c #ffff00\",\n";
        line.sprintf( "\"# c #%02X%02X%02X \",\n", r, g, b );
        pix += line.copy();
        pix += "\"b c #c00000\",\n";
        pix += "\"c c #ff0000\",\n";
        pix += "\"....................\",\n";
        pix += "\"....................\",\n";
        pix += "\"....................\",\n";
        pix += "\"........#...........\",\n";
        pix += "\"........#a..........\",\n";
        pix += "\".......###..........\",\n";
        pix += "\".......###a.........\",\n";
        pix += "\"......##aa#.........\",\n";
        pix += "\"......##a.#a........\",\n";
        pix += "\".....##a...#........\",\n";
        pix += "\".....#######a.......\",\n";
        pix += "\"....##aaaaaa#.......\",\n";
        pix += "\"....##a.....aaaaaaaa\",\n";
        pix += "\"...####....#abbccdda\",\n";
        pix += "\"....aaaa....abbccdda\",\n";
        pix += "\"............aee##ffa\",\n";
        pix += "\"............aee##ffa\",\n";
        pix += "\"............agghhiia\",\n";
        pix += "\"............agghhiia\",\n";
        pix += "\"............aaaaaaaa\"};\n";
      } break;
      case FrameColor: {
	pix += "\" 20 20 3 1 \",\n";

	pix += "\"  c none \",\n";
	pix += "\"+ c white \",\n";
	line.sprintf( "\". c #%02X%02X%02X \",\n", r, g, b );
	pix += line.copy();

	pix += "\"                     \",\n";
	pix += "\"                     \",\n";
	pix += "\"  ................  \",\n";
	pix += "\"  ................  \",\n";
	pix += "\"  ................  \",\n";
	pix += "\"  ...++++++++++...  \",\n";
	pix += "\"  ...++++++++++...  \",\n";
	pix += "\"  ...++++++++++...  \",\n";
	pix += "\"  ...++++++++++...  \",\n";
	pix += "\"  ...++++++++++...  \",\n";
	pix += "\"  ...++++++++++...  \",\n";
	pix += "\"  ...++++++++++...  \",\n";
	pix += "\"  ...++++++++++...  \",\n";
	pix += "\"  ...++++++++++...  \",\n";
	pix += "\"  ...++++++++++...  \",\n";
	pix += "\"  ................  \",\n";
	pix += "\"  ................  \",\n";
	pix += "\"  ................  \",\n";
	pix += "\"                     \",\n";
	pix += "\"                     \";\n";
      } break;
      case BackgroundColor: {
        pix += "\" 20 20 3 1 \",\n";

        pix += "\"  c none \",\n";
        pix += "\". c red \",\n";
        line.sprintf( "\"+ c #%02X%02X%02X \",\n", r, g, b );
        pix += line.copy();

        pix += "\"                     \",\n";
        pix += "\"                     \",\n";
        pix += "\"  ................  \",\n";
        pix += "\"  ................  \",\n";
        pix += "\"  ..++++++++++++..  \",\n";
        pix += "\"  ..++++++++++++..  \",\n";
        pix += "\"  ..++++++++++++..  \",\n";
        pix += "\"  ..++++++++++++..  \",\n";
        pix += "\"  ..++++++++++++..  \",\n";
        pix += "\"  ..++++++++++++..  \",\n";
        pix += "\"  ..++++++++++++..  \",\n";
        pix += "\"  ..++++++++++++..  \",\n";
        pix += "\"  ..++++++++++++..  \",\n";
        pix += "\"  ..++++++++++++..  \",\n";
        pix += "\"  ..++++++++++++..  \",\n";
        pix += "\"  ..++++++++++++..  \",\n";
        pix += "\"  ................  \",\n";
        pix += "\"  ................  \",\n";
        pix += "\"                     \",\n";
        pix += "\"                     \";\n";
      } break;
    }

    QPixmap pixmap( pix );
    setIconSet( QIconSet( pixmap ) );
}

/////////////////

// A KColorAction with a popupmenu for changing the color
KSelectColorAction::KSelectColorAction( const QString& text, Type type,
                                          int accel, QObject* parent, const char* name )
  : KColorAction(text,type,accel,parent,name)
{
  initPopup();
}

KSelectColorAction::KSelectColorAction( const QString& text, Type type, int accel,
                    QObject* receiver, const char* slot, QObject* parent, const char* name )
  : KColorAction(text,type,accel,receiver,slot,parent,name)
{
  initPopup();
}

KSelectColorAction::~KSelectColorAction()
{
  delete m_popup;
}

void KSelectColorAction::initPopup()
{
  m_popup = new KPopupMenu();
  // TODO: insert rectangular widgets of various colors in the popup
  m_popup->insertItem(i18n("Other..."),this, SLOT( changeColor() ) );
}

void KSelectColorAction::changeColor()
{
  QColor col(color());
  if ( KColorDialog::getColor( col ) )
  {
    setColor( col );
    emit activated();
  }
}

int KSelectColorAction::plug(QWidget* widget, int index)
{
  if ( widget->inherits( "KToolBar" ) )
  {
    KToolBar *bar = (KToolBar *)widget;

    int id_ = KAction::getToolButtonID();
    bar->insertButton( iconSet(KIcon::Small).pixmap(), id_, SIGNAL( clicked() ), this,
                       SLOT( slotActivated() ), isEnabled(), plainText(),
                       index );

    addContainer( bar, id_ );

    connect( bar, SIGNAL( destroyed() ), this, SLOT( slotDestroyed() ) );

    bar->setDelayedPopup( id_, popupMenu(), true );

    return containerCount() - 1;
  }
  return KColorAction::plug( widget, index );
}

/////////////////

KColorBar::KColorBar( const QValueList<QColor> &cols,
		      QWidget *parent, const char *name )
    : QWidget( parent, name ), colors( cols ), orient( Vertical )
{
    setMinimumSize( 20, colors.count() * 16 + 10 );
    resize( minimumSize() );
    show();
}

void KColorBar::mousePressEvent( QMouseEvent *e )
{
    if ( orientation() == Vertical ) {
	int index = -1;
	int x = ( width() - 12 ) / 2;
	int y = 5;
	QValueList<QColor>::Iterator it = colors.begin();
	for ( int i = 0; it != colors.end(); ++it, ++i ) {
	    if ( QRect( x, y, 12, 12 ).contains( e->pos() ) )
		index = i;
	    y += 16;
	}

	if ( index != -1 && index < (int)colors.count() ) {
	    if ( e->button() == LeftButton )
		emit leftClicked( colors[ index ] );
	    else if ( e->button() == RightButton )
		emit rightClicked( colors[ index ] );
	}
    } else {
	int index = -1;
	int y = ( height() - 12 ) / 2;
	int x = 5;
	QValueList<QColor>::Iterator it = colors.begin();
	for ( int i = 0; it != colors.end(); ++it, ++i ) {
	    if ( QRect( x, y, 12, 12 ).contains( e->pos() ) )
		index = i;
	    x += 16;
	}

	if ( index != -1 && index < (int)colors.count() ) {
	    if ( e->button() == LeftButton )
		emit leftClicked( colors[ index ] );
	    else if ( e->button() == RightButton )
		emit rightClicked( colors[ index ] );
	}
    }
}

void KColorBar::orientationChanged( Orientation o )
{
    orient = o;
    if ( orientation() == Vertical ) {
	setMinimumSize( 20, colors.count() * 16 + 10 );
    } else {
	setMinimumSize( colors.count() * 16 + 10, 20 );
    }
    updateGeometry();
    repaint( FALSE );
}

void KColorBar::paintEvent( QPaintEvent * )
{
    QPainter p;
    p.begin( this );
    if ( orientation() == Vertical ) {
	int x = ( width() - 12 ) / 2;
	int y = 5;
	QValueList<QColor>::Iterator it = colors.begin();
	for ( ; it != colors.end(); ++it ) {
	    qDrawShadePanel( &p, x, y, 12, 12, colorGroup(), true );
	    p.fillRect( x + 1, y + 1, 10, 10, *it );
	    y += 16;
	}
    } else {
	int y = ( height() - 12 ) / 2;
	int x = 5;
	QValueList<QColor>::Iterator it = colors.begin();
	for ( ; it != colors.end(); ++it ) {
	    qDrawShadePanel( &p, x, y, 12, 12, colorGroup(), true );
	    p.fillRect( x + 1, y + 1, 10, 10, *it );
	    x += 16;
	}
    }
    p.end();
}

////////////

KColorBarAction::KColorBarAction( const QString &text, int accel,
				  QObject *r, const char *leftClickSlot_, const char *rightClickSlot_,
				  const QValueList<QColor> &cols, QObject *parent, const char *name )
  : KAction( text, accel, parent, name ), colors( cols ),
    receiver( r ),
    leftClickSlot ( leftClickSlot_ ),
    rightClickSlot ( rightClickSlot_ )
{
}

int KColorBarAction::plug( QWidget *widget, int index )
{
    if ( widget && widget->inherits( "QToolBar" ) ) {
	QToolBar* bar = (QToolBar*)widget;
	KColorBar *b;
	b = new KColorBar( colors, widget, "" );
	b->resize( 100, 25 );
	b->show();
	connect( b, SIGNAL( leftClicked( const QColor & ) ),
		 this, SIGNAL( leftClicked( const QColor & ) ) );
	connect( b, SIGNAL( rightClicked( const QColor & ) ),
		 this, SIGNAL( rightClicked( const QColor & ) ) );
	connect( b, SIGNAL( leftClicked( const QColor & ) ),
		 receiver, leftClickSlot );
	connect( b, SIGNAL( rightClicked( const QColor & ) ),
		 receiver, rightClickSlot );
	connect( bar, SIGNAL( orientationChanged( Orientation ) ),
		 b, SLOT( orientationChanged( Orientation ) ) );
	addContainer( bar, b );
	connect( bar, SIGNAL( destroyed() ), this, SLOT( slotDestroyed() ) );
	b->orientationChanged( bar->orientation() );
	return containerCount() - 1;
    } else if ( widget && widget->inherits( "KToolBar" ) ) {
	KToolBar* bar = (KToolBar*)widget;
	KColorBar *b;
	b = new KColorBar( colors, widget, "" );
	bar->insertWidget( -1, b->width(), b, index );
	b->resize( 100, 25 );
	b->show();
	connect( b, SIGNAL( leftClicked( const QColor & ) ),
		 this, SIGNAL( leftClicked( const QColor & ) ) );
	connect( b, SIGNAL( rightClicked( const QColor & ) ),
		 this, SIGNAL( rightClicked( const QColor & ) ) );
	connect( b, SIGNAL( leftClicked( const QColor & ) ),
		 receiver, leftClickSlot );
	connect( b, SIGNAL( rightClicked( const QColor & ) ),
		 receiver, rightClickSlot );
	connect( (QToolBar*)bar, SIGNAL( orientationChanged( Orientation ) ),
		 b, SLOT( orientationChanged( Orientation ) ) );

	addContainer( bar, b );
	connect( bar, SIGNAL( destroyed() ), this, SLOT( slotDestroyed() ) );

	b->orientationChanged( bar->orientation() );
	return containerCount() - 1;
    }

    return -1;
}

#include <kcoloractions.moc>
