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
   the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
   Boston, MA 02111-1307, USA.
*/

#ifndef __VTEXT_H__
#define __VTEXT_H__

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <qptrlist.h>
#include <qstring.h>
#include <qfont.h>

#include "vpath.h"
#include "vcomposite.h"
#include "vpainter.h"

typedef QPtrList<VComposite> VCompositeList;
typedef QPtrListIterator<VComposite> VCompositeListIterator;

class VText : public VObject
{
public:
	enum Position {
		Above,
		On,
		Under
	};

	VText( VObject* parent, VState state = normal );
	VText( const QFont &font, const VPath& basePath, Position position, const QString& text );
	VText( const VText& text );
	virtual ~VText();

	virtual void setText( const QString& text ) { m_text = text; }
	virtual const QString& text() { return m_text; }
	virtual void setFont( const QFont& font ) { m_font = font; }
	virtual const QFont& font() { return m_font; }
	virtual void setBasePath( const VPath& path ) { m_basePath = path; }
	virtual VPath& basePath() { return m_basePath; }
	virtual void setPosition( Position position ) { m_position = position; }
	virtual Position position() { return m_position; }
	
	virtual void draw( VPainter* painter, const KoRect* rect = 0L ) const;

	virtual void transform( const QWMatrix& m );

	virtual const KoRect& boundingBox() const;

	virtual void save( QDomElement& element ) const;
	virtual void load( const QDomElement& element );

	virtual VText* clone() const;

	virtual void setState( const VState state );
	
	virtual void accept( VVisitor& visitor );
	
#ifdef HAVE_FREETYPE
	void traceText();
#endif // HAVE_FREETYPE

protected:
	QString buildRequest( QString family, int weight, int slant, double size, int &id );

private:
		// The font to use to draw the text.
	QFont       m_font;
		// The base path. Doesn't belong to the document.
	VPath       m_basePath;
		// The text position 
	Position    m_position;
		// The text to draw
	QString     m_text;
		// The glyphs (allow to keep a font even if not present on the computer. works as long as you don't edit the text.)
	VCompositeList   m_glyphs;
};

#endif
