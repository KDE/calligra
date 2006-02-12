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

#ifndef __XCFEXPORT_H__
#define __XCFEXPORT_H__


#include <KoFilter.h>

#include "vvisitor.h"


class QDataStream;
class VDocument;
class VLayer;


class XcfExport : public KoFilter, private VVisitor
{
	Q_OBJECT

public:
	XcfExport( KoFilter* parent, const char* name, const QStringList& );
	virtual ~XcfExport() {}

	virtual KoFilter::ConversionStatus convert( const QCString& from, const QCString& to );

	virtual void visitVDocument( VDocument& document );
	virtual void visitVLayer( VLayer& layer );

private:
	/**
	 * Writes a hierarchy.
	 */
	 void writeHierarchy();

	/**
	 * Writes a level.
	 */
	 void writeLevel();

	/**
	 * Calculates levels from layer and tile size.
	 */
	static int levels( int layerSize, int tileSize );


	/**
	 * Tile size constants.
	 */
	static const unsigned m_tileWidth;
	static const unsigned m_tileHeight;

	/**
	 * Output stream.
	 */
	QDataStream* m_stream;

	/**
	 * Image width.
	 */
	unsigned m_width;

	/**
	 * Image height.
	 */
	unsigned m_height;

	/**
	 * X-zoom factor.
	 */
	 double m_zoomX;

	/**
	 * Y-zoom factor.
	 */
	 double m_zoomY;
};

#endif

