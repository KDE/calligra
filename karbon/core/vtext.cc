/* This file is part of the KDE project
   Copyright (C) 2002, The Karbon Developers
*/

#include <qdom.h>

#include "karbon_view.h"
#include "vpath.h"
#include "vtext.h"
#include "vvisitor.h"

#include <kdebug.h>

#ifdef HAVE_FREETYPE

#include <X11/Xlib.h>
#include <X11/Xft/XftFreetype.h>

#include FT_FREETYPE_H
#include FT_OUTLINE_H
#include FT_GLYPH_H

#define FT_TOFLOAT(x) ((x) * (1.0 / 64.0))



// Trace routines for ttf / ps font -> VPath

int traceMoveto( FT_Vector *to, VPath *path )
{
	double tox = (to->x / 64);
	double toy = (-to->y / 64);

    QString add = "M" + QString::number(tox) + "," + QString::number(toy) + " ";
    kdDebug() << add.latin1() << endl;
	path->moveTo( KoPoint( tox, toy ) );
}

int traceLineto( FT_Vector *to, VPath *path )
{
	double tox = (to->x / 64);
	double toy = (-to->y / 64);

    QString add = "L" + QString::number(tox) + "," + QString::number(toy) + " ";
    kdDebug() << add.latin1() << endl;

	path->lineTo( KoPoint( tox, toy ) );
};

int traceQuadraticBezier( FT_Vector *control, FT_Vector *to, VPath *path )
{
	double x1 = (control->x / 64);
	double y1 = (-control->y / 64);
	double x2 = (to->x / 64);
	double y2 = (-to->y / 64);

    QString add = "Q" + QString::number(x1) + "," + QString::number(y1) + "," + QString::number(x2) + "," + QString::number(y2) + " ";
    kdDebug() << add.latin1() << endl;
	//path->curveTo( x1, y1, x1, y1, x2, y2 );
	path->curve2To( KoPoint( x1, y1 ), KoPoint( x2, y2 ) );
};

int traceCubicBezier( FT_Vector *p, FT_Vector *q, FT_Vector *to, VPath *path )
{
	double x1 = (p->x / 64);
	double y1 = (-p->y / 64);
	double x2 = (q->x / 64);
	double y2 = (-q->y / 64);
	double x3 = (to->x / 64);
	double y3 = (-to->y / 64);

	QString add = "C" + QString::number(x1) + "," + QString::number(y1) + "," + QString::number(x2) + "," + QString::number(y2) + "," + QString::number(x3) + "," + QString::number(y3);
	kdDebug() << add.latin1() << endl;

	path->curveTo( KoPoint( x1, y1 ), KoPoint( x2, y2 ), KoPoint( x3, y3 ) );
};

FT_Outline_Funcs OutlineMethods =
{
	(FT_Outline_MoveTo_Func) traceMoveto,
	(FT_Outline_LineTo_Func) traceLineto,
	(FT_Outline_ConicTo_Func) traceQuadraticBezier,
	(FT_Outline_CubicTo_Func) traceCubicBezier,
	0,
	0
};

#endif // HAVE_FREETYPE

VText::VText( VObject* parent, VState state )
	: VObject( parent, state )
{
}


VText::VText( KarbonView *view, const QFont &font )
	: VObject(), m_view( view ), m_font( font )
{
	m_glyphs.setAutoDelete( true );
}

VText::VText( KarbonView *view, const QFont &font, const QString &text )
	: VObject(), m_text( text ), m_view( view ), m_font( font )
{
}

VText::VText( const VText& text )
	: VObject( text ), m_text( text.m_text )
{
	// copy glyphs
	VObjectListIterator itr = text.m_glyphs;
	for ( ; itr.current() ; ++itr )
		m_glyphs.append( itr.current()->clone() );
}

VText::~VText()
{
}

void
VText::setState( const VState state )
{
	VObjectListIterator itr = m_glyphs;
	for ( ; itr.current() ; ++itr )
		itr.current()->setState( state );
}

void
VText::draw( VPainter* painter, const KoRect& rect ) const
{
	if( state() == state_deleted )
		return;

	//if( !rect.intersects( boundingBox( zoomFactor ) ) )
	//	return;

#ifdef HAVE_FREETYPE

	// setup glyphs
	if( m_glyphs.count() == 0 )
	{
		traceText( m_text );
		setState( state() );
	}

#endif // HAVE_FREETYPE

	// draw glyphs
	VObjectListIterator itr = m_glyphs;
	for ( ; itr.current() ; ++itr )
	{
		//kdDebug() << "draw!!!!" << itr.current() << endl;
		//itr.current()->setState( state() );
		itr.current()->setFill( m_fill );
		itr.current()->setStroke( m_stroke );
		itr.current()->draw( painter, rect );
	}
}

void
VText::transform( const QWMatrix& m )
{
	VObjectListIterator itr = m_glyphs;
	for ( ; itr.current() ; ++itr )
		itr.current()->transform( m );
}

bool
VText::isInside( const KoRect& rect ) const
{
	VObjectListIterator itr = m_glyphs;
	for ( ; itr.current() ; ++itr )
		if( itr.current()->isInside( rect ) )
			return true;

	return false;
}

VText*
VText::clone() const
{
	return new VText( *this );
}

void
VText::save( QDomElement& element ) const
{
	if( state() != state_deleted )
	{
		QDomElement me = element.ownerDocument().createElement( "TEXT" );

		VObject::save( me );

		// save font properties
		me.setAttribute( "text", m_text );
		me.setAttribute( "family", m_font.family() );
		me.setAttribute( "size", m_font.pointSize() );
		me.setAttribute( "italic", m_font.italic() );
		me.setAttribute( "bold", m_font.bold() );

		element.appendChild( me );

		// save all glyphs / paths
		VObjectListIterator itr = m_glyphs;
		for ( ; itr.current() ; ++itr )
			itr.current()->save( me );
	}
}

void
VText::load( const QDomElement& element )
{
	m_glyphs.clear();

	m_font.setFamily( element.attribute( "family", "Times" ) );
	m_font.setPointSize( element.attribute( "size", "12" ).toInt() );
	m_font.setItalic( element.attribute( "italic" ) == 0 ? false : true );
	m_font.setWeight( QFont::Normal );
	m_font.setBold( element.attribute( "bold" ) == 0 ? false : true );

	m_text = element.attribute( "text", "" );

    // load text gluphs:
	QDomNodeList list = element.childNodes();
	for( uint i = 0; i < list.count(); ++i )
	{
		if( list.item( i ).isElement() )
		{
			QDomElement e = list.item( i ).toElement();
			if( e.tagName() == "PATH" )
			{
				VPath *path = new VPath();
				path->load( e );
				m_glyphs.append( path );
			}
		}
	}
}

void
VText::accept( VVisitor& visitor )
{
	visitor.visitVText( *this );
}

#ifdef HAVE_FREETYPE

void VText::traceText( const QString &text )
{
	// load the font with all the properties set in m_font
	XftFont *font = 0L;
	kdDebug() << "size : " << m_font.pointSize() << endl;
	kdDebug() << " for requested font \"" << m_font.family().latin1() << "\"" << endl;
	// TODO : set more options
	int slant = XFT_SLANT_ROMAN;
	if( m_font.italic() )
		slant = XFT_SLANT_ITALIC;

	int weight = 0;
	if( m_font.bold() )
		weight = XFT_WEIGHT_BOLD;
	font = XftFontOpen( m_view->x11Display(), m_view->x11Screen(), XFT_FAMILY, XftTypeString, m_font.family().latin1(),
						XFT_SIZE, XftTypeDouble, double( m_font.pointSize() ), XFT_WEIGHT, XftTypeInteger, weight,
						XFT_SLANT, XftTypeInteger, slant, 0 );

	char *filename;
	if( font )
	{
		XftPatternGetString( font->pattern, XFT_FILE, 0, &filename );
		kdDebug() << "loading " << filename << " for requested font \"" << m_font.family().latin1() << "\"" << endl;

		FT_UInt glyphIndex;
		FT_Face fontFace = font->u.ft.font->face;

		// find the unicode charmap
		/* FT_CharMap found = 0;
		FT_CharMap charmap;
		for ( int n = 0; n < fontFace->num_charmaps; n++ )
	    {
		      charmap = fontFace->charmaps[ n ];
			  if( charmap->encoding_id == (FT_UShort)1 && charmap->platform_id == (FT_UShort)3 )
				found = charmap;
			  kdDebug() << "charmap.platform_id : " << charmap->platform_id << endl;
			  kdDebug() << "charmap.encoding_id : " << charmap->encoding_id << endl;
		}
		kdDebug() << "Found : " << found << endl;
		FT_Set_Charmap( fontFace, found ); */
		float unitsPerEM = float( fontFace->units_per_EM );
		float x = 100;
		float y = 200;
		for( int i = 0; i < m_text.length(); i++ )
		{
			// get the glyph index for the current character
			kdDebug() << m_text.at( i ).unicode() << endl;
			QChar character = m_text.at( i );
			glyphIndex = FT_Get_Char_Index( fontFace, character.unicode() );
			kdDebug() << "glyphindex : " << glyphIndex << endl;
			bool error = FT_Load_Glyph( fontFace, glyphIndex, FT_LOAD_NO_HINTING | FT_LOAD_NO_BITMAP );
			if( error )
			{
				kdDebug() << "Houston, we have a problem" << endl;
				return;
			}

			// decompose to vpath
			FT_OutlineGlyph g;
			FT_Get_Glyph( fontFace->glyph, reinterpret_cast<FT_Glyph *>( &g ) );
			VPath *path = new VPath();
			FT_Outline_Decompose(&g->outline, &OutlineMethods, path );
			path->close();

			// add x and y advance values
			// TODO : find out how the freetype coords work, kerning
			QWMatrix mat;
			mat.translate( x, y );
			/*kdDebug() << "Adding : " << ( float( fontFace->glyph->advance.x ) / float( fontFace->units_per_EM ) ) * 40.0 << endl;
			kdDebug() << "Font size : " << m_font.pixelSize() << endl;
			kdDebug() << "Per EM : " << unitsPerEM << endl;
			kdDebug() << "advance X : " << float( fontFace->glyph->advance.x ) << endl;*/

			x += FT_TOFLOAT( fontFace->glyph->advance.x );
			y += FT_TOFLOAT( fontFace->glyph->advance.y );
			//kdDebug() << "X : " << x << endl;
			//kdDebug() << "Y : " << y << endl;
			//mat.scale( 2, 2 );
			path->transform( mat );

			// append to the glyph collection
			m_glyphs.append( path );
		}
		XftFontClose( m_view->x11Display(), font );
	}
}

#endif // HAVE_FREETYPE
