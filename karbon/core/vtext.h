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

#ifndef __VTEXT_H__
#define __VTEXT_H__

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <qptrlist.h>
#include <qstring.h>
#include <qfont.h>
#include <koffice_export.h>

#include "vpath.h"
#include "vcomposite.h"

class VGroup;

typedef QPtrList<VPath> VPathList;
typedef QPtrListIterator<VPath> VPathListIterator;

#ifdef Above
#undef Above
#endif

class KARBONBASE_EXPORT VText : public VObject
{
public:
	enum Position {
		Above,
		On,
		Under
	};

	enum Alignment {
		Left,
		Center,
		Right
	};

	VText( VObject* parent, VState state = normal );
	VText( const QFont &font, const VSubpath& basePath, Position position, Alignment alignment, const QString& text );
	VText( const VText& text );
	virtual ~VText();
	virtual DCOPObject* dcopObject();

	virtual void setText( const QString& text );
	virtual const QString& text() { return m_text; }
	virtual void setFont( const QFont& font ) { m_font = font; }
	virtual const QFont& font() { return m_font; }
	virtual void setBasePath( const VSubpath& path ) { m_basePath = path; }
	virtual VSubpath& basePath() { return m_basePath; }
	virtual void setPosition( Position position ) { m_position = position; }
	virtual Position position() { return m_position; }
	virtual void setAlignment( Alignment alignment ) { m_alignment = alignment; }
	virtual Alignment alignment() { return m_alignment; }
	virtual void setUseShadow( bool state ) { m_shadow = state; }
	virtual bool useShadow() { return m_shadow; }
	virtual void setShadow( int angle, int distance, bool translucent ) 
		{ m_translucentShadow = translucent; m_shadowAngle = angle; m_shadowDistance = distance; }
	virtual bool translucentShadow() { return m_translucentShadow; }
	virtual int shadowAngle() { return m_shadowAngle; }
	virtual int shadowDistance() { return m_shadowDistance; }
	virtual void setOffset( double offset );
	virtual double offset() { return m_offset; }

	/**
	 * Provides read only access to the glyphs.
	 */
	const VPathList& glyphs() const
	{
		return m_glyphs;
	}

	virtual void draw( VPainter *painter, const KoRect* rect = 0L ) const;

	virtual const KoRect& boundingBox() const;

	virtual void save( QDomElement& element ) const;
	virtual void load( const QDomElement& element );

	virtual VText* clone() const;
	virtual VGroup* toVGroup() const;

	virtual void setState( const VState state );

	virtual void accept( VVisitor& visitor );

#ifdef HAVE_KARBONTEXT
	void traceText();

protected:
	QString buildRequest( QString family, int weight, int slant, double size, int &id );
#endif // HAVE_KARBONTEXT

private:
		// The font to use to draw the text.
	QFont       m_font;
		// The base path. Doesn't belong to the document.
	VSubpath       m_basePath;
		// The text position 
	Position    m_position;
		// The text alignment
	Alignment   m_alignment;
		// The text to draw
	QString     m_text;
		// Shadow parameters
	bool        m_shadow;
	bool        m_translucentShadow;
	int         m_shadowDistance;
	int         m_shadowAngle;
		// The glyphs (allow to keep a font even if not present on the computer. works as long as you don't edit the text.)
	VPathList   m_glyphs;
		// the position offset
	double      m_offset;
};

#endif
