/* This file is part of the KDE project
   Copyright (C) 2001, The Karbon Developers
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

#ifndef __VCOMPOSITE_H__
#define __VCOMPOSITE_H__


#include <qptrlist.h>

#include <koPoint.h>

#include "vobject.h"
#include "vpath.h"
#include "svgpathparser.h"


class QDomElement;
class QWMatrix;
class VPainter;
class VSegment;
class VVisitor;


typedef QPtrList<VPath> VPathList;
typedef QPtrListIterator<VPath> VPathListIterator;


/**
 * A composite path consists of one or many subpaths.
 */

class VComposite : public VObject, SVGPathParser
{
public:
	VComposite( VObject* parent, VState state = normal );
	VComposite( const VComposite& path );
	virtual ~VComposite();

	const KoPoint& currentPoint() const;

	// postscript-like commands:
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
	bool arcTo(
		const KoPoint& p1, const KoPoint& p2, double r );

	/**
	 * Closes the current subpath.
	 */
	void close();

	/**
	 * Combines two composite paths. For example, the letter "O" is a combination
	 * of a larger and a smaller ellipitical path.
	 */
	void combine( const VComposite& path );

	/**
	 * Adds a path to the composite path.
	 */
	void combinePath( const VPath& path );

	virtual void draw( VPainter *painter, const KoRect* rect = 0L ) const;

	const VSegment* lastSegment() const
		{ return m_paths.getLast()->getLast(); }

	const VPathList& paths() const
		{ return m_paths; }

	virtual const KoRect& boundingBox() const;

	virtual void save( QDomElement& element ) const;
	virtual void load( const QDomElement& element );

	virtual VComposite* clone() const;

	virtual void accept( VVisitor& visitor );

	bool drawCenterNode() const { return m_drawCenterNode; }
	void setDrawCenterNode( bool drawCenterNode = true )
		{ m_drawCenterNode = drawCenterNode; }

	/// For svg path data parsing
	virtual void svgMoveTo( double x1, double y1 );
	virtual void svgLineTo( double x1, double y1 );
	virtual void svgCurveTo( double x1, double y1, double x2, double y2, double x3, double y3 );
	virtual void svgClosePath();

	void loadSvgPath( const QString & );
	void saveSvgPath( QString & ) const;

private:
	VPathList m_paths;		// list of paths

	/// Should a center node be drawn?
	bool m_drawCenterNode;
};

#endif
