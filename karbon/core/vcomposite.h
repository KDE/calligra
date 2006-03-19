/* This file is part of the KDE project
   Copyright (C) 2001, 2002, 2003 The Karbon Developers

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

#ifndef __VCOMPOSITE_H__
#define __VCOMPOSITE_H__


#include <qptrlist.h>

#include <KoPoint.h>

#include "vobject.h"
#include "svgpathparser.h"
#include "vfillrule.h"
#include <koffice_export.h>

class QDomElement;
class VPainter;
class VSegment;
class VVisitor;
class VSubpath;

typedef QPtrList<VSubpath> VSubpathList;
typedef QPtrListIterator<VSubpath> VSubpathListIterator;


/**
 * A composite path consists of one or many subpaths.
 */

class KARBONBASE_EXPORT VPath : public VObject, SVGPathParser
{
public:
	VPath( VObject* parent, VState state = normal );
	VPath( const VPath& path );
	virtual ~VPath();

	virtual DCOPObject* dcopObject();

	/**
	 * Returns the knot of the last segment of the last subpath.
	 */
	const KoPoint& currentPoint() const;


	bool moveTo( const KoPoint& p );
	bool lineTo( const KoPoint& p );

	/*
	curveTo():

	   p1          p2
	    O   ____   O
	    : _/    \_ :
	    :/        \:
	    x          x
	currP          p3
	*/

	bool curveTo(
		const KoPoint& p1, const KoPoint& p2, const KoPoint& p3 );

	/*
	curve1To():

	               p2
	         ____  O
	      __/    \ :
	     /        \:
	    x          x
	currP          p3
	*/

	bool curve1To( const KoPoint& p2, const KoPoint& p3 );

	/*
	curve2To():

	   p1
	    O  ____
	    : /    \__
	    :/        \
	    x          x
	currP          p3
	*/

	bool curve2To( const KoPoint& p1, const KoPoint& p3 );

	/**
	 * A convenience function to aproximate a circular arc with a
	 * bezier curve. Input: 2 tangent vectors and a radius (same as in PostScript).
	 */

	/*
	arcTo():
	
	   p1 x....__--x....x p2
	      :  _/
	      : /
	      :/
	      |
	      x
	      |
	      |
	      x currP
	 */
	bool arcTo( const KoPoint& p1, const KoPoint& p2, double r );

	/**
	 * Closes the current subpath.
	 */
	void close();

	bool isClosed() const;
	/**
	 * Combines two composite paths. For example, the letter "O" is a combination
	 * of a larger and a smaller ellipitical path.
	 */
	void combine( const VPath& path );

	/**
	 * Adds a path to the composite path.
	 */
	void combinePath( const VSubpath& path );


	/**
	 * Returns true if point p is located inside the composite.
	 */
	bool pointIsInside( const KoPoint& p ) const;


	/**
	 * Returns true if the segment intersects this composite.
	 */
	bool intersects( const VSegment& segment ) const;


	const VSubpathList& paths() const
	{
		return m_paths;
	}

	virtual const KoRect& boundingBox() const;


	VFillRule fillMode() const;

	// TODO remove these functions.
	VFillRule fillRule() const
	{
		return m_fillRule;
	}

	void setFillRule( VFillRule fillRule )
	{
		m_fillRule = fillRule;
	}


	virtual void draw( VPainter *painter, const KoRect* rect = 0L ) const;

	bool drawCenterNode() const
	{
		return m_drawCenterNode;
	}

	void setDrawCenterNode( bool drawCenterNode = true )
	{
		m_drawCenterNode = drawCenterNode;
	}


	virtual void save( QDomElement& element ) const;
	virtual void saveOasis( KoStore *store, KoXmlWriter *docWriter, KoGenStyles &mainStyles, int &index ) const;
	virtual void load( const QDomElement& element );
	virtual bool loadOasis( const QDomElement &element, KoOasisLoadingContext &context );

	virtual VPath* clone() const;

	virtual void accept( VVisitor& visitor );

	void transform( const QString &transform );
	void transformOasis( const QString &transform );

	static QWMatrix parseTransform( const QString &transform );

	void transform( const QWMatrix &mat )
	{
		m_matrix *= mat;
	}


	void loadSvgPath( const QString & );
	void saveSvgPath( QString & ) const;

protected:
	QString buildSvgTransform() const;
	QString buildSvgTransform( const QWMatrix &mat ) const;
	QString buildOasisTransform() const;
	QString buildOasisTransform( const QWMatrix &mat ) const;

	void transformByViewbox( const QDomElement &element, QString viewbox );

	/// For svg path data parsing.
	virtual void svgMoveTo( double x1, double y1, bool abs = true );
	virtual void svgLineTo( double x1, double y1, bool abs = true );
	virtual void svgCurveToCubic( double x1, double y1, double x2, double y2, double x, double y, bool abs = true );
	virtual void svgClosePath();

	virtual void saveOasisFill( KoGenStyles &mainStyles, KoGenStyle &stylesojectauto ) const;
	QWMatrix parseOasisTransform( const QString &transform );

protected:
	QWMatrix m_matrix;

private:
	/**
	 * List of subpaths.
	 */
	VSubpathList m_paths;

	/// Should a center node be drawn?
	bool		m_drawCenterNode;
	VFillRule	m_fillRule	: 1;
};

#endif
