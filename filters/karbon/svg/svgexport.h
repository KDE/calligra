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

#ifndef __SVGEXPORT_H__
#define __SVGEXPORT_H__

#include <koFilter.h>

#include "vvisitor.h"
#include "vgradient.h"


class QTextStream;
class VColor;
class VDocument;
class VFill;
class VGroup;
class VLayer;
class VPath;
class VSegmentList;
class VStroke;
class VText;


class SvgExport : public KoFilter, private VVisitor
{
	Q_OBJECT

public:
	SvgExport( KoFilter* parent, const char* name, const QStringList& );
	virtual ~SvgExport() {}

	virtual KoFilter::ConversionStatus convert( const QCString& from, const QCString& to );

private:
	virtual void visitVDocument( VDocument& document );
	virtual void visitVPath( VPath& path );
	virtual void visitVSegmentList( VSegmentList& segmentList );
	virtual void visitVText( VText& text );

	void getStroke( const VStroke& stroke );
	void getColorStops( const QValueList<VGradient::VColorStop> &colorStops );
	void getFill( const VFill& fill  );
	void getGradient( const VGradient& grad );
	void getHexColor( QTextStream *, const VColor& color  );

	QTextStream* m_stream;
	QTextStream* m_defs;
	QTextStream* m_body;
};

#endif

