/* This file is part of the KDE project
   Copyright (C) 2002, The Karbon Developers
*/

#include "vstrokefillpreview.h"
#include "vkopainter.h"
#include "koPoint.h"
#include "kdebug.h"
#include <qcolor.h>
#include "vstroke.h"
#include "vfill.h"
#include "vfilldlg.h"
#include "vstrokedlg.h"
#include "vselection.h"
#include "karbon_part.h"
#include <kdebug.h>

#define FILL_TOPX		15.0
#define FILL_TOPY		20.0
#define FILL_BOTTOMX	45.0
#define FILL_BOTTOMY	50.0

#define STROKE_TOPX		5.0
#define STROKE_TOPY		10.0
#define STROKE_BOTTOMX	35.0
#define STROKE_BOTTOMY	40.0

#define STROKE_TOPX_INNER		STROKE_TOPX + 4
#define STROKE_TOPY_INNER		STROKE_TOPY + 4
#define STROKE_BOTTOMX_INNER	STROKE_BOTTOMX - 4
#define STROKE_BOTTOMY_INNER	STROKE_BOTTOMY - 4

VStrokeFillPreview::VStrokeFillPreview( KarbonPart *part, QWidget* parent = 0L, const char* name = 0L )
	: QFrame( parent, name ), m_part( part )
{
	setFocusPolicy( QWidget::NoFocus );
	setFrameStyle( QFrame::StyledPanel | QFrame::Sunken );
	installEventFilter( this );

	m_pixmap.resize( 50, 50 );
	m_painter = new VKoPainter( &m_pixmap, 50, 50 );
}

VStrokeFillPreview::~VStrokeFillPreview()
{
	delete( m_painter );
}

void
VStrokeFillPreview::paintEvent( QPaintEvent* event )
{
	bitBlt( this, 0, 0, &m_pixmap, 0, 0, 50, 50 );
}

bool
VStrokeFillPreview::eventFilter( QObject *, QEvent *event )
{
	if( event && event->type() == QEvent::MouseButtonPress )
	{
		QMouseEvent *e = static_cast<QMouseEvent *>( event );
		if( e->x() >= 15 && e->x() <= 45 && e->y() >= 20 && e->y() <= 50 )
		{
			VFillDlg* dialog = new VFillDlg( m_part );
			connect( dialog, SIGNAL( fillChanged( const VFill & ) ), this, SIGNAL( fillChanged( const VFill & ) ) );
			dialog->exec();
			delete dialog;
			disconnect( dialog, SIGNAL( fillChanged( const VFill & ) ), this, SIGNAL( fillChanged( const VFill & ) ) );
		}
		else if( e->x() >= 5 && e->x() <= 35 && e->y() >= 10 && e->y() <= 40 )
		{
			VStrokeDlg* dialog = new VStrokeDlg( m_part );
			connect( dialog, SIGNAL( strokeChanged( const VStroke & ) ), this, SIGNAL( strokeChanged( const VStroke & ) ) );
			dialog->exec();
			delete dialog;
			disconnect( dialog, SIGNAL( strokeChanged( const VStroke & ) ), this, SIGNAL( strokeChanged( const VStroke & ) ) );
		}
	}
	return false;
}


void
VStrokeFillPreview::update( const VStroke &s, const VFill &f )
{
	m_painter->begin();
	m_painter->clear( paletteBackgroundColor().rgb() );

	VStroke stroke;
	stroke.setLineWidth( 2.0 );

	m_painter->setPen( Qt::NoPen );
	if( s.type() != stroke_none )
	{
		if( s.type() == stroke_gradient )
		{
			VFill fill;
			fill.gradient() = s.gradient();
			if( s.gradient().type() == gradient_linear )
			{
				fill.gradient().setOrigin( KoPoint( 20, 10 ) );
				fill.gradient().setVector( KoPoint( 20, 40 ) );
			}
			else if( s.gradient().type() == gradient_radial )
			{
				fill.gradient().setOrigin( KoPoint( 20, 25 ) );
				fill.gradient().setVector( KoPoint( 20, 40 ) );
			}
			fill.setType( fill_gradient );
			m_painter->setBrush( fill );
		}
		else
			m_painter->setBrush( s.color() );

		m_painter->newPath();
		m_painter->moveTo( KoPoint( STROKE_TOPX, STROKE_TOPY ) );
		m_painter->lineTo( KoPoint( STROKE_BOTTOMX, STROKE_TOPY ) );
		m_painter->lineTo( KoPoint( STROKE_BOTTOMX, STROKE_BOTTOMY ) );
		m_painter->lineTo( KoPoint( STROKE_TOPX, STROKE_BOTTOMY ) );
		m_painter->lineTo( KoPoint( STROKE_TOPX, STROKE_TOPY ) );

		m_painter->moveTo( KoPoint( STROKE_TOPX_INNER, STROKE_TOPY_INNER ) );
		m_painter->lineTo( KoPoint( STROKE_BOTTOMX_INNER, STROKE_TOPY_INNER ) );
		m_painter->lineTo( KoPoint( STROKE_BOTTOMX_INNER, STROKE_BOTTOMY_INNER ) );
		m_painter->lineTo( KoPoint( STROKE_TOPX_INNER, STROKE_BOTTOMY_INNER ) );
		m_painter->lineTo( KoPoint( STROKE_TOPX_INNER, STROKE_TOPY_INNER ) );
		m_painter->fillPath();
	}

	// show 3D outline of stroke part
	stroke.setColor( Qt::white.rgb() );
	m_painter->setPen( stroke );
	m_painter->newPath();
	m_painter->moveTo( KoPoint( STROKE_BOTTOMX + 1, STROKE_TOPY - 1 ) );
	m_painter->lineTo( KoPoint( STROKE_TOPX - 1, STROKE_TOPY - 1 ) );
	m_painter->lineTo( KoPoint( STROKE_TOPX - 1, STROKE_BOTTOMY + 1 ) );
	m_painter->strokePath();

	stroke.setColor( Qt::black.rgb() );
	m_painter->setPen( stroke );
	m_painter->newPath();
	m_painter->moveTo( KoPoint( STROKE_BOTTOMX + 1, STROKE_TOPY - 1 ) );
	m_painter->lineTo( KoPoint( STROKE_BOTTOMX + 1, STROKE_BOTTOMY + 1 ) );
	m_painter->lineTo( KoPoint( STROKE_TOPX - 1, STROKE_BOTTOMY + 1 ) );
	m_painter->strokePath();

	stroke.setColor( Qt::black.rgb() );
	m_painter->setPen( stroke );
	m_painter->newPath();
	m_painter->moveTo( KoPoint( STROKE_BOTTOMX_INNER - 1, STROKE_TOPY_INNER + 1 ) );
	m_painter->lineTo( KoPoint( STROKE_TOPX_INNER + 1, STROKE_TOPY_INNER + 1 ) );
	m_painter->lineTo( KoPoint( STROKE_TOPX_INNER + 1, STROKE_BOTTOMY_INNER - 1 ) );
	m_painter->strokePath();

	stroke.setColor( Qt::white.rgb() );
	m_painter->setPen( stroke );
	m_painter->newPath();
	m_painter->moveTo( KoPoint( STROKE_BOTTOMX_INNER - 1, STROKE_TOPY_INNER + 1 ) );
	m_painter->lineTo( KoPoint( STROKE_BOTTOMX_INNER - 1, STROKE_BOTTOMY_INNER - 1 ) );
	m_painter->lineTo( KoPoint( STROKE_TOPX_INNER + 1, STROKE_BOTTOMY_INNER - 1 ) );
	m_painter->strokePath();

	if( f.type() != fill_none )
	{
		if( f.type() == fill_gradient )
		{
			VFill fill;
			fill = f;
			if( f.gradient().type() == gradient_linear )
			{
				fill.gradient().setOrigin( KoPoint( 30, 20 ) );
				fill.gradient().setVector( KoPoint( 30, 50 ) );
			}
			else if( f.gradient().type() == gradient_radial )
			{
				fill.gradient().setOrigin( KoPoint( 30, 35 ) );
				fill.gradient().setVector( KoPoint( 30, 50 ) );
			}
			m_painter->setBrush( fill );
		}
		else
			m_painter->setBrush( f.color() );

		m_painter->newPath();
		m_painter->moveTo( KoPoint( FILL_TOPX, FILL_TOPY ) );
		m_painter->lineTo( KoPoint( FILL_BOTTOMX, FILL_TOPY ) );
		m_painter->lineTo( KoPoint( FILL_BOTTOMX, FILL_BOTTOMY ) );
		m_painter->lineTo( KoPoint( FILL_TOPX, FILL_BOTTOMY ) );
		m_painter->lineTo( KoPoint( FILL_TOPX, FILL_TOPY) );
		m_painter->fillPath();
	}
	// show 3D outline of fill part
	m_painter->setBrush( Qt::NoBrush );
	stroke.setColor( Qt::white.rgb() );
	m_painter->setPen( stroke );
	m_painter->newPath();
	m_painter->moveTo( KoPoint( FILL_BOTTOMX, FILL_TOPY ) );
	m_painter->lineTo( KoPoint( FILL_TOPX, FILL_TOPY ) );
	m_painter->lineTo( KoPoint( FILL_TOPX, FILL_BOTTOMY ) );
	m_painter->strokePath();

	stroke.setColor( Qt::black.rgb() );
	m_painter->setPen( stroke );
	m_painter->newPath();
	m_painter->moveTo( KoPoint( FILL_BOTTOMX, FILL_TOPY ) );
	m_painter->lineTo( KoPoint( FILL_BOTTOMX, FILL_BOTTOMY ) );
	m_painter->lineTo( KoPoint( FILL_TOPX, FILL_BOTTOMY ) );
	m_painter->strokePath();

	m_painter->end();

	repaint();
}

#include "vstrokefillpreview.moc"

