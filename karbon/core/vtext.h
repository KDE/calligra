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

#include "vobjectlist.h"

class KarbonView;

class VText : public VObject
{
public:
	VText( VObject* parent, VState state = state_normal );
	VText( KarbonView *view, const QFont &font );
	VText( KarbonView *view, const QFont &font, const QString& text );
	VText( const VText& text );
	virtual ~VText();

	virtual void draw( VPainter* painter, const KoRect* rect = 0L ) const;

	virtual void transform( const QWMatrix& m, bool selectedSubObjects = false );

	virtual const KoRect& boundingBox() const
		{ return m_glyphs.boundingBox(); }

	virtual bool isInside( const KoRect& rect ) const;

	virtual void save( QDomElement& element ) const;
	virtual void load( const QDomElement& element );

	virtual VText* clone() const;

	virtual void accept( VVisitor& visitor );
	
	void setState( const VState state );

private:
#ifdef HAVE_FREETYPE
	void traceText( const QString &text );
#endif // HAVE_FREETYPE

	QString m_text;
	VObjectList m_glyphs;
	const KarbonView *m_view;
	QFont m_font;
};

#endif
