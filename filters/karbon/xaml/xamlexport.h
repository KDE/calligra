/* This file is part of the KDE project
   Copyright (C) 2002, 2003 The Karbon Developers

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

#ifndef __XAMLEXPORT_H__
#define __XAMLEXPORT_H__

#include <KoFilter.h>

#include "vvisitor.h"
#include "vgradient.h"

#include "xamlgraphiccontext.h"

#include <qptrstack.h>

class QTextStream;
class VColor;
class VPath;
class VDocument;
class VFill;
class VGroup;
class VLayer;
class VSubpath;
class VStroke;
class VText;


class XAMLExport : public KoFilter, private VVisitor
{
	Q_OBJECT

public:
	XAMLExport( KoFilter* parent, const char* name, const QStringList& );
	virtual ~XAMLExport() {}

	virtual KoFilter::ConversionStatus convert( const QCString& from, const QCString& to );

private:
	virtual void visitVPath( VPath& composite );
	virtual void visitVDocument( VDocument& document );
	virtual void visitVGroup( VGroup& group );
	virtual void visitVSubpath( VSubpath& path );
	//virtual void visitVText( VText& text );

	void getStroke( const VStroke& stroke );
	void getColorStops( const QPtrVector<VColorStop> &colorStops );
	void getFill( const VFill& fill  );
	void getGradient( const VGradient& grad );
	void getHexColor( QTextStream *, const VColor& color  );
	QString getID( VObject *obj );

	QTextStream* m_stream;
	QTextStream* m_defs;
	QTextStream* m_body;

	QPtrStack<XAMLGraphicsContext>	m_gc;
};

#endif

