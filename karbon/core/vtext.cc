/* This file is part of the KDE project
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
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
*/

#include <qdom.h>
#include <QFile>
//Added by qt3to4:
#include <Q3ValueList>

#include <kdebug.h>
#include <KoPoint.h>
#include <KoRect.h>

#include "vpath.h"
#include "vtext.h"
#include "vtext_iface.h"
#include "vstroke.h"
#include "vfill.h"
#include "vvisitor.h"
#include "vsegment.h"
#include "vgroup.h"
#include "vpainter.h"
#include "commands/vtransformcmd.h"

#ifdef HAVE_KARBONTEXT

#include <ft2build.h>
#include <fontconfig/fontconfig.h>


#include FT_FREETYPE_H
#include FT_OUTLINE_H
#include FT_GLYPH_H

#define FT_TOFLOAT(x) ((x) * (1.0 / 64.0))
#define FT_FROMFLOAT(x) ((int) floor ((x) * 64.0 + 0.5))


// Trace routines for ttf / ps font -> VSubpath

int traceMoveto( FT_Vector *to, VPath *composite )
{
	double tox = ( to->x / 64.0 );
	double toy = ( -to->y / 64.0 );

	//QString add = "M" + QString::number(tox) + "," + QString::number(toy) + " ";
	//kDebug(38000) << add.latin1() << endl;
	composite->moveTo( KoPoint( tox, toy ) );

	return 0;
}

int traceLineto( FT_Vector *to, VPath *composite )
{
	double tox = ( to->x / 64.0 );
	double toy = ( -to->y / 64.0 );

	//QString add = "L" + QString::number(tox) + "," + QString::number(toy) + " ";
	//kDebug(38000) << add.latin1() << endl;

	composite->lineTo( KoPoint( tox, toy ) );

	return 0;
}

int traceQuadraticBezier( FT_Vector *control, FT_Vector *to, VPath *composite )
{
	double x1 = ( control->x / 64.0 );
	double y1 = ( -control->y / 64.0 );
	double x2 = ( to->x / 64.0 );
	double y2 = ( -to->y / 64.0 );

	//QString add = "Q" + QString::number(x1) + "," + QString::number(y1) + "," + QString::number(x2) + "," + QString::number(y2) + " ";
	//kDebug(38000) << add.latin1() << endl;
	composite->curveTo( KoPoint( x1, y1 ), KoPoint( x2, y2 ), KoPoint( x2, y2 ) );
	//composite->curve2To( KoPoint( x1, y1 ), KoPoint( x2, y2 ) );

	return 0;
}

int traceCubicBezier( FT_Vector *p, FT_Vector *q, FT_Vector *to, VPath *composite )
{
	double x1 = ( p->x / 64.0 );
	double y1 = ( -p->y / 64.0 );
	double x2 = ( q->x / 64.0 );
	double y2 = ( -q->y / 64.0 );
	double x3 = ( to->x / 64.0 );
	double y3 = ( -to->y / 64.0 );

	//QString add = "C" + QString::number(x1) + "," + QString::number(y1) + "," + QString::number(x2) + "," + QString::number(y2) + "," + QString::number(x3) + "," + QString::number(y3);
	//kDebug(38000) << add.latin1() << endl;

	composite->curveTo( KoPoint( x1, y1 ), KoPoint( x2, y2 ), KoPoint( x3, y3 ) );

	return 0;
}

FT_Outline_Funcs OutlineMethods =
{
	(FT_Outline_MoveTo_Func) traceMoveto,
	(FT_Outline_LineTo_Func) traceLineto,
	(FT_Outline_ConicTo_Func) traceQuadraticBezier,
	(FT_Outline_CubicTo_Func) traceCubicBezier,
	0,
	0
};

#endif // HAVE_KARBONTEXT

VText::VText( VObject* parent, VState state )
	: VObject( parent, state ), m_basePath( 0L )
{
	m_glyphs.setAutoDelete( true );
	m_boundingBoxIsInvalid = true;
	m_stroke = new VStroke( this );
	m_fill = new VFill();
	m_position			= (VText::Position)0;
	m_alignment			= (VText::Alignment)0;
	m_shadow			= false;
	m_translucentShadow	= false;
	m_shadowAngle		= 0;
	m_shadowDistance	= 0;
	m_offset			= 0.0;
}


VText::VText( const QFont &font, const VSubpath& basePath, Position position, Alignment alignment, const QString& text )
	: VObject( 0L ), m_font( font ), m_basePath( basePath ), m_position( position ), m_alignment( alignment ), m_text( text )
{
	m_glyphs.setAutoDelete( true );
	m_boundingBoxIsInvalid = true;
	m_stroke = new VStroke( this );
	m_fill = new VFill();
	m_offset = 0.0;
}

VText::VText( const VText& text )
	: VObject( text ), m_font( text.m_font ), m_basePath( text.m_basePath ), m_position( text.m_position ), m_alignment( text.m_alignment ), m_text( text.m_text ), m_shadow( text.m_shadow ), m_translucentShadow( text.m_translucentShadow ), m_shadowDistance( text.m_shadowDistance ), m_shadowAngle( text.m_shadowAngle ), m_offset( text.m_offset )
{
	m_stroke = new VStroke( *text.m_stroke );
	m_stroke->setParent( this );
	m_fill = new VFill( *text.m_fill );

	// copy glyphs
	VPathListIterator itr( text.m_glyphs );
	for( ; itr.current() ; ++itr )
	{
		VPath* c = itr.current()->clone();
		c->setParent( this );
		m_glyphs.append( c );
	}

	m_boundingBoxIsInvalid = true;
}

VText::~VText()
{
}

DCOPObject* VText::dcopObject()
{
	if( !m_dcop )
		m_dcop = new VTextIface( this );

	return m_dcop;
}


void
VText::draw( VPainter* painter, const KoRect* /*rect*/ ) const
{
	if(
		state() == deleted ||
		state() == hidden ||
		state() == hidden_locked )
	{
		return;
	}

	painter->save();

	VPathListIterator itr( m_glyphs );

	if( state() != edit )
	{
		// paint fill:
		painter->newPath();

		if ( m_shadow )
		{
			VColor color;
			if ( m_translucentShadow )
			{
				color.set( 0., 0., 0. );
				color.setOpacity( .3 );
			}
			else
			{
				color.set( .3, .3, .3 );
				color.setOpacity( 1. );
			}
			int shadowDx = int( m_shadowDistance * cos( m_shadowAngle / 360. * 6.2832 ) );
			int shadowDy = int( m_shadowDistance * sin( m_shadowAngle / 360. * 6.2832 ) );

			VTransformCmd trafo( 0L, QMatrix() );
			for( itr.toFirst(); itr.current(); ++itr )
			{
				trafo.setMatrix( QMatrix( 1, 0, 0, 1, shadowDx, shadowDy ) );
				trafo.visit( *( itr.current() ) );
				itr.current()->setFill( VFill( color ) );
				itr.current()->setStroke( VStroke( color ) );
				itr.current()->draw( painter );
				trafo.setMatrix( QMatrix( 1, 0, 0, 1, -shadowDx, -shadowDy ) );
				trafo.visit( *( itr.current() ) );
			}
		}

		for( itr.toFirst(); itr.current(); ++itr )
		{
			itr.current()->setFill( *m_fill );
			itr.current()->setStroke( *m_stroke );
			itr.current()->draw( painter );
		}
	}

	// draw simplistic contour:
	if( state() == edit )//|| state() == selected )
	{
		painter->newPath();
		painter->setRasterOp( Qt::XorROP );
		painter->setPen( Qt::yellow );
		painter->setBrush( Qt::NoBrush );

		for( itr.toFirst(); itr.current(); ++itr )
			itr.current()->draw( painter );

		painter->strokePath();
	}

	painter->restore();
}

const KoRect&
VText::boundingBox() const
{
	if( m_boundingBoxIsInvalid )
	{
		VPathListIterator itr( m_glyphs );
		itr.toFirst();
		// clear:
		m_boundingBox = itr.current() ? itr.current()->boundingBox() : KoRect();
		for( ++itr; itr.current(); ++itr )
			if( !itr.current()->boundingBox().isEmpty() )
				m_boundingBox |= itr.current()->boundingBox();

		// take line width into account:
		m_boundingBox.setCoords(
			m_boundingBox.left()   - 0.5 * stroke()->lineWidth(),
			m_boundingBox.top()    - 0.5 * stroke()->lineWidth(),
			m_boundingBox.right()  + 0.5 * stroke()->lineWidth(),
			m_boundingBox.bottom() + 0.5 * stroke()->lineWidth() );

		m_boundingBoxIsInvalid = false;
	}

	return m_boundingBox;
}

VText*
VText::clone() const
{
	return new VText( *this );
}

VGroup* VText::toVGroup() const
{
	VGroup* group = new VGroup( parent() );

	VPathListIterator itr( m_glyphs );
	for( itr.toFirst(); itr.current(); ++itr )
	{
		VPath* c = itr.current()->clone();
		c->setParent( group );
		group->append( c );
	}

	group->setFill( *m_fill );
	group->setStroke( *m_stroke );

	return group;
} // VText::toVGroup

void
VText::save( QDomElement& element ) const
{
	if( state() != deleted )
	{
		QDomElement me = element.ownerDocument().createElement( "TEXT" );

		VPath path( 0L );
		path.combinePath( m_basePath );
		path.save( me );

		VObject::save( me );

		// save font properties
		me.setAttribute( "text",				m_text );
		me.setAttribute( "family",				m_font.family() );
		me.setAttribute( "size",				m_font.pointSize() );
		me.setAttribute( "italic",				m_font.italic() );
		me.setAttribute( "bold",				m_font.bold() );
		me.setAttribute( "position",			m_position );
		me.setAttribute( "alignment",			m_alignment );
		me.setAttribute( "shadow",				m_shadow );
		me.setAttribute( "translucentshadow",	m_translucentShadow );
		me.setAttribute( "shadowangle",			m_shadowAngle );
		me.setAttribute( "shadowdist",			m_shadowDistance );
		me.setAttribute( "offset",				m_offset );
		element.appendChild( me );

		// save all glyphs / paths
		VPathListIterator itr = m_glyphs;
		for( itr.toFirst(); itr.current(); ++itr )
			itr.current()->save( me );
	}
}

void
VText::load( const QDomElement& element )
{
	m_glyphs.clear();

	m_font.setFamily( element.attribute( "family", "Times" ) );
	m_font.setPointSize( element.attribute( "size", "12" ).toInt() );
	m_font.setItalic( element.attribute( "italic" ).toInt() == 1 );
	m_font.setWeight( QFont::Normal );
	m_font.setBold( element.attribute( "bold" ).toInt() == 1 );
	m_position			= (Position)element.attribute( "position", "0" ).toInt();
	m_alignment			= (Alignment)element.attribute( "alignment", "0" ).toInt();
	m_shadow			= ( element.attribute( "shadow" ).toInt() == 1 );
	m_translucentShadow	= ( element.attribute( "translucentshadow" ).toInt() == 1 );
	m_shadowAngle		= element.attribute( "shadowangle" ).toInt();
	m_shadowDistance	= element.attribute( "shadowdist" ).toInt();
	m_offset			= element.attribute( "offset" ).toDouble();
	m_text = element.attribute( "text", "" );

	VObject::load( element );

	QDomNodeList list = element.childNodes();
	QDomElement e = list.item( 0 ).toElement();
	
	// element to start with reading glyph paths and stroke, fill, etc.
	uint startElement = 0;

	if( e.tagName() == "PATH" )
	{
		VPath path( 0L );
		path.load( e );
		m_basePath = *path.paths().getFirst();
		startElement++;
	}

	// load text glyphs:
	for( uint i = startElement; i < list.count(); ++i )
	{
		if( list.item( i ).isElement() )
		{
			e = list.item( i ).toElement();
			if( e.tagName() == "PATH" )
			{
				VPath *composite = new VPath( this );
				composite->load( e );
				m_glyphs.append( composite );
			}
			if( e.tagName() == "STROKE" )
				m_stroke->load( e );
			if( e.tagName() == "FILL" )
				m_fill->load( e );
		}
	}
	// if no glyphs yet, trace them
#ifdef HAVE_KARBONTEXT
	if( m_glyphs.count() == 0 )
		traceText();
#endif
	m_boundingBoxIsInvalid = true;
	//m_fill->setFillRule( VFill::evenOdd );
}

void
VText::setText( const QString& text )
{
	if( m_text != text )
	{
		m_text = text;
		m_glyphs.clear();
#ifdef HAVE_KARBONTEXT
		traceText();
#endif
	}
}

void
VText::setState( const VState state )
{
	VObject::setState( state );

	VPathListIterator itr( m_glyphs );
	for( itr.toFirst(); itr.current(); ++itr )
	{
		itr.current()->setState( state );
	}
}

void
VText::accept( VVisitor& visitor )
{
	visitor.visitVText( *this );
}

#ifdef HAVE_KARBONTEXT

void
VText::traceText()
{
	if( m_basePath.count() == 0 )
	{
		kDebug(38000) << "Can't draw a text without base path (was: " << m_text << ")." << endl;
		return;
	}

	// TODO : set more options
	int slant = FC_SLANT_ROMAN;
	if( m_font.italic() )
		slant = FC_SLANT_ITALIC;

	int weight = 0;
	if( m_font.bold() )
		weight = FC_WEIGHT_BOLD;

	// Build FontConfig request pattern
	int id = -1;
	QString filename = buildRequest( m_font.family(), weight, slant, m_font.pointSize(), id );
	m_glyphs.clear();

	kDebug(38000) << "Loading " << filename.latin1() << " for requested font \"" << m_font.family().latin1() << "\", " << m_font.pointSize() << " pt." << endl;

	FT_UInt glyphIndex;
	FT_Face fontFace;
	// TODO : this lib should probably be a singleton (Rob)
	FT_Library library;
	FT_Init_FreeType( &library );
	FT_Error error = FT_New_Face( library, QFile::encodeName(filename), id, &fontFace );

	if( error )
	{
		kDebug(38000) << "traceText(), could not load font. Aborting!" << endl;
		return;
	}

	bool foundCharmap = false;

	// Try to choose unicode charmap
	for( int charmap = 0; charmap < fontFace->num_charmaps; charmap++ )
	{
		if( fontFace->charmaps[charmap]->encoding == ft_encoding_unicode )
		{
			FT_Error error = FT_Set_Charmap( fontFace, fontFace->charmaps[charmap] );
			if( error )
			{
				kDebug(38000) << "traceText(), unable to select unicode charmap." << endl;
				continue;
			}
			foundCharmap = true;
		}
	}

	// Choose first charmap if no unicode charmap was found
	if( ! foundCharmap )
	{
		error = FT_Set_Charmap( fontFace, fontFace->charmaps[0] );
		if( error )
		{
			kDebug(38000) << "traceText(), unable to select charmap. Aborting!" << endl;
			FT_Done_Face( fontFace );
			FT_Done_FreeType( library );
			return;
		}
	}

	error = FT_Set_Char_Size( fontFace, FT_FROMFLOAT( m_font.pointSize() ), FT_FROMFLOAT( m_font.pointSize() ), 0, 0 );
	if( error )
	{
		kDebug(38000) << "traceText(), unable to set font size. Aborting!" << endl;
		FT_Done_Face( fontFace );
		FT_Done_FreeType( library );
		return;
	}

		// storing glyphs.
	float l = 0;
	Q3ValueList<float> glyphXAdvance;
	Q3ValueList<float> glyphYAdvance;
	for( unsigned int i = 0; i < m_text.length(); i++ )
	{
		// get the glyph index for the current character
		QChar character = m_text.at( i );
		glyphIndex = FT_Get_Char_Index( fontFace, character.unicode() );
		if( ! glyphIndex ) 
		{
			kDebug(38000) << "traceText(), unable get index of char : " << character << endl;
			continue;
		}
		//kDebug(38000) << "glyphIndex : " << glyphIndex << endl;
		FT_Error error = FT_Load_Glyph( fontFace, glyphIndex, FT_LOAD_NO_HINTING | FT_LOAD_NO_BITMAP );
		if( error )
		{
			kDebug(38000) << "traceText(), unable to load glyph : " << error << endl;
			continue;
		}

		// decompose to vpaths
		FT_OutlineGlyph g;
		error = FT_Get_Glyph( fontFace->glyph, reinterpret_cast<FT_Glyph *>( &g ) );
		if( error )
		{
			kDebug(38000) << "traceText(), unable to get glyph: " << error << endl;
			continue;
		}

		VPath *composite = new VPath( this );
		error = FT_Outline_Check( &g->outline );
		if( error ) 
		{
			kDebug(38000) << "traceText(), outline is broken : " << error << endl;
			continue;
		}

		error = FT_Outline_Decompose(&g->outline, &OutlineMethods, composite );
		if( error )
		{
			kDebug(38000) << "traceText(), unable to decompose outline : " << error << endl;
			continue;
		}

		m_glyphs.append( composite );
		glyphXAdvance.append( FT_TOFLOAT( fontFace->glyph->advance.x ) );
		glyphYAdvance.append( FT_TOFLOAT( fontFace->glyph->advance.y ) );
		l += FT_TOFLOAT( fontFace->glyph->advance.x );
		FT_Done_Glyph( reinterpret_cast<FT_Glyph>( g ) );
	}

	 // Placing the stored glyphs.
	float pathLength = 0;
	VSubpathIterator pIt( m_basePath );
	
	VSegment* seg;
	for( ; pIt.current(); ++pIt )
		if( (seg = pIt.current() ) ) 
			pathLength += seg->length();

	kDebug(38000) << "traceText(), using offset : " << m_offset << endl;
	float x = m_offset * pathLength;
	
	switch( m_alignment )
	{
		case Qt::DockLeft: x += 0; break;
		case Center: x -=  0.5 * l; break;
		case Qt::DockRight: x -= l; break;
	}
	float y = 0;
	float dx = 0;
	float sp = 0;
	KoPoint point;
	KoPoint normal;
	KoPoint tangent;
	VSubpathIterator pathIt( m_basePath );
	VSegment* oldSeg = pathIt.current();
	seg = ++pathIt;
	KoPoint extPoint;
	bool ext = false;
	float fsx = 0;
	float yoffset = ( m_position == Above ? 0 : ( m_position == On ? m_font.pointSize() / 3 : m_font.pointSize() / 1.5 ) );
	kDebug(38000) << "Position: " << m_position << " -> " << yoffset << endl;
	for( unsigned int i = 0; i < m_text.length(); i++ )
	{
		VPath* composite = m_glyphs.at( i );
		if( ! composite ) 
			continue;
		// Step 1: place (0, 0) to the rotation center of the glyph.
		dx = *glyphXAdvance.at( i ) / 2;
		x += dx;
		VTransformCmd trafo( 0L, QMatrix( 1, 0, 0, 1, -dx, y + yoffset ) );
		trafo.visit( *composite );

		// Step 2: find the position where to draw.
		//   3 possibilities: before, on, and after the basePath...
		if ( x < 0 )
		{
			if( !ext )
				seg->pointTangentNormalAt( 0, &extPoint, &tangent, &normal );
			point = extPoint + x * tangent;
			ext = true;
		}
		else
		{
			while ( seg && x > fsx + seg->length() )
			{
				fsx += seg->length();
				oldSeg = seg;
				seg = ++pathIt;
			}
			if( seg )
			{
				ext = false;
				sp = ( x - fsx ) / seg->length();
				seg->pointTangentNormalAt( sp, &point, &tangent, &normal );
			}
			else
			{
				if( !ext )
					oldSeg->pointTangentNormalAt( 1, &extPoint, &tangent, &normal );
				point = extPoint + ( x - fsx ) * tangent;
				ext = true;
			}
		}

		// Step 3: transform glyph and append it. That's it, we've got
		// text following a path. Really easy, isn't it ;) ?
		trafo.setMatrix( QMatrix( tangent.x(), tangent.y(), tangent.y(), -tangent.x(), point.x(), point.y() ) );
		trafo.visit( *composite );
		composite->setState( state() );

		//kDebug(38000) << "Glyph: " << (QString)character << " [String pos: " << x << ", " << y << " / Canvas pos: " << point.x() << ", " << point.y() << "]" << endl;

		x += dx;
		y += *glyphYAdvance.at( i );
	}
	FT_Done_Face( fontFace );
	FT_Done_FreeType( library );
	m_boundingBoxIsInvalid = true;
}

// This routine is copied from KSVGFont (Rob)
QString
VText::buildRequest( QString family, int weight, int slant, double size, int &id )
{
	// Strip those stupid [Xft or whatever]...
	int pos;
	if( ( pos = family.find( '[' ) ) )
		family = family.left( pos );

	// Use FontConfig to locate & select fonts and use  FreeType2 to open them
	FcPattern *pattern;
	QString fileName;

	pattern = FcPatternBuild( 0, FC_WEIGHT, FcTypeInteger, weight,
							  FC_SLANT, FcTypeInteger, slant,
							  FC_SIZE, FcTypeDouble, size, NULL );

	// Add font name
	FcPatternAddString( pattern, FC_FAMILY, reinterpret_cast<const FcChar8 *>( family.latin1() ) );

	// Disable hinting
	FcPatternAddBool( pattern, FC_HINTING, FcFalse );
	// Enforce scalability
	FcPatternAddBool( pattern, FC_SCALABLE, FcTrue );

	// Perform the default font pattern modification operations.
	FcDefaultSubstitute( pattern );
	FcConfigSubstitute( FcConfigGetCurrent(), pattern, FcMatchPattern );

	FcResult result;

	// we dont want to use bitmap fonts, so get a list of fonts sorted by closeness to pattern
	// and use the best matching scalable font
	FcFontSet *fset = FcFontSort( 0, pattern, FcFalse, 0L, &result );

	// Destroy pattern
	FcPatternDestroy( pattern );

	if( fset )
	{
		FcBool scalable;
		FcChar8 *temp;
	
		// iterate over font list and take best scaleable font
		for( int i = 0; i < fset->nfont; ++i )
		{
			pattern = fset->fonts[i];
			if( FcResultMatch != FcPatternGetBool( pattern, FC_SCALABLE, 0, &scalable ) )
				continue;
			if( scalable == FcTrue )
			{
				// Get index & filename
				if(	FcPatternGetString(pattern, FC_FILE, 0, &temp) != FcResultMatch ||
					FcPatternGetInteger(pattern, FC_INDEX, 0, &id) != FcResultMatch )
				{
					kDebug(38000) << "VText::buildRequest(), could not load font file for requested font \"" << family.latin1() << "\"" << endl;
					return QString::null;
				}
		
				fileName = QFile::decodeName(reinterpret_cast<const char *>( temp ));
				
				// get family name of matched font
				QString newFamily;

				if( FcResultMatch == FcPatternGetString( pattern, FC_FAMILY, 0, &temp ) )
					m_font.setFamily( reinterpret_cast<const char *>( temp ) );

				break;
			}
		}
		
		FcFontSetDestroy( fset );
	}

	
	return fileName;
}

void VText::setOffset( double offset )
{
	if( offset < 0.0 )
		m_offset = 0.0;
	else if( offset > 1.0 )
		m_offset = 1.0;
	else
		m_offset = offset;
}

#endif // HAVE_KARBONTEXT
