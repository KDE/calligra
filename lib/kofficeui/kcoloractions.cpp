#include "kcoloractions.h"
#include <qcstring.h>
#include "kcoloractions.moc"
#include <qpopupmenu.h>
#include <qtoolbar.h>
#include <qtoolbutton.h>
#include <qdrawutil.h>
#include <ktoolbar.h>

KColorAction::KColorAction( const QString& text, int accel,
			    QObject* parent, const char* name )
    : KAction( text, accel, parent, name )
{
    init();
}

KColorAction::KColorAction( const QString& text, int accel,
			    QObject* receiver, const char* slot, QObject* parent,
			    const char* name )
    : KAction( text, accel, receiver, slot, parent, name )
{
    init();
}

KColorAction::KColorAction( const QString& text, Type type, int accel,
			    QObject* parent, const char* name )
    : KAction( text, accel, parent, name )
{
    init();
    setType( type );
}

KColorAction::KColorAction( const QString& text, Type type, int accel,
			    QObject* receiver, const char* slot, QObject* parent,
			    const char* name )
    : KAction( text, accel, receiver, slot, parent, name )
{
    init();
    setType( type );
}

KColorAction::KColorAction( QObject* parent, const char* name )
    : KAction( parent, name )
{
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
    typ = TextColor;
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

KColorBar::KColorBar( const QValueList<QColor> &cols,
		      QWidget *parent, const char *name )
    : QWidget( parent, name ), colors( cols )
{
    setMinimumSize( 20, colors.count() * 16 + 10 );
    setMaximumWidth( 20 );
    resize( minimumSize() );
    show();
}

void KColorBar::mousePressEvent( QMouseEvent *e )
{
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
}

void KColorBar::paintEvent( QPaintEvent * )
{
    QPainter p;
    p.begin( this );
    int x = ( width() - 12 ) / 2;
    int y = 5;
    QValueList<QColor>::Iterator it = colors.begin();
    for ( ; it != colors.end(); ++it ) {
	qDrawShadePanel( &p, x, y, 12, 12, colorGroup(), TRUE );
	p.fillRect( x + 1, y + 1, 10, 10, *it );
	y += 16;
    }
    p.end();
}

KColorBarAction::KColorBarAction( const QString &text, int accel,
				  QObject *r, const char *leftClickSlot_, const char *rightClickSlot_,
				  const QValueList<QColor> &cols, QObject *parent, const char *name )
    : KAction( text, accel, parent, name ), colors( cols )
{
    receiver = r;
    leftClickSlot = qstrdup( leftClickSlot_ );
    rightClickSlot = qstrdup( rightClickSlot_ );
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

	addContainer( bar, b );
	connect( bar, SIGNAL( destroyed() ), this, SLOT( slotDestroyed() ) );
	
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

	addContainer( bar, b );
	connect( bar, SIGNAL( destroyed() ), this, SLOT( slotDestroyed() ) );
	
	return containerCount() - 1;
    }

    return -1;
}
