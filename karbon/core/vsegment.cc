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

#include <math.h>

#include <qdom.h>
#include <QSizeF>
#include <Q3ValueList>

#include "vpainter.h"
#include "vpath.h"
#include "vsegment.h"
#include "vglobal.h"

#include <kdebug.h>

VSegment::VSegment( unsigned short deg )
{
	m_degree = deg;

	m_nodes = new VNodeData[ degree() ];

	for( unsigned short i = 0; i < degree(); ++i )
		selectPoint( i );

	m_state = normal;

	m_prev = 0L;
	m_next = 0L;
}

VSegment::VSegment( const VSegment& segment )
{
	m_degree = segment.degree();

	m_nodes = new VNodeData[ degree() ];

	m_state = segment.m_state;

	// Copying the pointers m_prev/m_next has some advantages (see VSegment::length()).
	// Inserting a segment into a path overwrites these anyway.
	m_prev = segment.m_prev;
	m_next = segment.m_next;

	// Copy points.
	for( unsigned short i = 0; i < degree(); i++ )
	{
		setPoint( i, segment.point( i ) );
		selectPoint( i, segment.pointIsSelected( i ) );
	}
}

VSegment::~VSegment()
{
	delete[]( m_nodes );
}

void
VSegment::setDegree( unsigned short deg )
{
	// Do nothing if old and new degrees are identical.
	if( degree() == deg )
		return;

	// TODO : this code is fishy, please make it sane

	// Remember old nodes.
	VNodeData* oldNodes = m_nodes;
	QPointF oldKnot = knot();

	// Allocate new node data.
	m_nodes = new VNodeData[ deg ];

	if( deg == 1 )
		m_nodes[ 0 ].m_vector = oldKnot;
	else
	{
		// Copy old node data (from the knot "backwards".
		unsigned short offset = qMax( 0, deg - m_degree );

		for( unsigned short i = offset; i < deg; ++i )
		{
			m_nodes[ i ].m_vector = oldNodes[ i - offset ].m_vector;
		}

		// Fill with "zeros" if necessary.
		for( unsigned short i = 0; i < offset; ++i )
		{
			m_nodes[ i ].m_vector = QPointF( 0.0, 0.0 );
		}
	}

	// Set new degree.
	m_degree = deg;

	// Delete old nodes.
	delete[]( oldNodes );
}

void
VSegment::draw( VPainter* painter ) const
{
	// Don't draw a deleted segment.
	if( state() == deleted )
		return;


	if( prev() )
	{
		if( degree() == 3 )
		{
			painter->curveTo( point( 0 ), point( 1 ), point( 2 ) );
		}
		else
		{
			painter->lineTo( knot() );
		}
	}
	else
	{
		painter->moveTo( knot() );
	}
}

bool
VSegment::isFlat( double flatness ) const
{
	// Lines and "begin" segments are flat.
	if(
		!prev() ||
		degree() == 1 )
	{
		return true;
	}


	// Iterate over control points.
	for( unsigned short i = 0; i < degree() - 1; ++i )
	{
		if(
			height( prev()->knot(), point( i ), knot() ) / chordLength()
			>= flatness )
		{
			return false;
		}
	}

	return true;
}

QPointF
VSegment::pointAt( double t ) const
{
	QPointF p;

	pointDerivativesAt( t, &p );

	return p;
}

void
VSegment::pointDerivativesAt( double t, QPointF* p,
							  QPointF* d1, QPointF* d2 ) const
{
	if( !prev() )
		return;


	// Optimise the line case.
	if( degree() == 1 )
	{
		const QPointF diff = knot() - prev()->knot();

		if( p )
			*p = prev()->knot() + diff * t;

		if( d1 )
			*d1 = diff;

		if( d2 )
			*d2 = QPointF( 0.0, 0.0 );

		return;
	}


	// Beziers.

	// Copy points.
	QPointF* q = new QPointF[ degree() + 1 ];

	q[ 0 ] = prev()->knot();

	for( unsigned short i = 0; i < degree(); ++i )
	{
		q[ i + 1 ] = point( i );
	}


	// The De Casteljau algorithm.
	for( unsigned short j = 1; j <= degree(); j++ )
	{
		for( unsigned short i = 0; i <= degree() - j; i++ )
		{
			q[ i ] = ( 1.0 - t ) * q[ i ] + t * q[ i + 1 ];
		}

		// Save second derivative now that we have it.
		if( j == degree() - 2 )
		{
			if( d2 )
				*d2 = degree() * ( degree() - 1 )
					  * ( q[ 2 ] - 2 * q[ 1 ] + q[ 0 ] );
		}

		// Save first derivative now that we have it.
		else if( j == degree() - 1 )
		{
			if( d1 )
				*d1 = degree() * ( q[ 1 ] - q[ 0 ] );
		}
	}

	// Save point.
	if( p )
		*p = q[ 0 ];

	delete[]( q );


	return;
}

QPointF
VSegment::tangentAt( double t ) const
{
	QPointF tangent;

	pointTangentNormalAt( t, 0L, &tangent );

	return tangent;
}

void
VSegment::pointTangentNormalAt( double t, QPointF* p,
								QPointF* tn, QPointF* n ) const
{
	// Calculate derivative if necessary.
	QPointF d;

	pointDerivativesAt( t, p, tn || n ? &d : 0L );


	// Normalize derivative.
	if( tn || n )
	{
		const double norm =
			sqrt( d.x() * d.x() + d.y() * d.y() );

		d = norm ? d * ( 1.0 / norm ) : QPointF( 0.0, 0.0 );
	}

	// Assign tangent vector.
	if( tn )
		*tn = d;

	// Calculate normal vector.
	if( n )
	{
		// Calculate vector product of "binormal" x tangent
		// (0,0,1) x (dx,dy,0), which is simply (dy,-dx,0).
		n->setX( d.y() );
		n->setY( -d.x() );
	}
}

double
VSegment::length( double t ) const
{
	if( !prev() || t == 0.0 )
	{
		return 0.0;
	}


	// Optimise the line case.
	if( degree() == 1 )
	{
		return
			t * chordLength();
	}


	/* This algortihm is by Jens Gravesen <gravesen AT mat DOT dth DOT dk>.
	 * We calculate the chord length "chord"=|P0P3| and the length of the control point
	 * polygon "poly"=|P0P1|+|P1P2|+|P2P3|. The approximation for the bezier length is
	 * 0.5 * poly + 0.5 * chord. "poly - chord" is a measure for the error.
	 * We subdivide each segment until the error is smaller than a given tolerance
	 * and add up the subresults.
	 */

	// "Copy segment" splitted at t into a path.
	VSubpath path( 0L );
	path.moveTo( prev()->knot() );

	// Optimize a bit: most of the time we'll need the
	// length of the whole segment.
	if( t == 1.0 )
		path.append( this->clone() );
	else
	{
		VSegment* copy = this->clone();
		path.append( copy->splitAt( t ) );
		delete copy;
	}


	double chord;
	double poly;

	double length = 0.0;

	while( path.current() )
	{
		chord = path.current()->chordLength();
		poly = path.current()->polyLength();

		if(
			poly &&
			( poly - chord ) / poly > VGlobal::lengthTolerance )
		{
			// Split at midpoint.
			path.insert(
				path.current()->splitAt( 0.5 ) );
		}
		else
		{
			length += 0.5 * poly + 0.5 * chord;
			path.next();
		}
	}


	return length;
}

double
VSegment::chordLength() const
{
	if( !prev() )
		return 0.0;


	QPointF d = knot() - prev()->knot();

	return sqrt( VGlobal::multiplyPoints(d,d) );
}

double
VSegment::polyLength() const
{
	if( !prev() )
		return 0.0;


	// Start with distance |first point - previous knot|.
	QPointF d = point( 0 ) - prev()->knot();

	double length = sqrt( VGlobal::multiplyPoints(d,d) );

	// Iterate over remaining points.
	for( unsigned short i = 1; i < degree(); ++i )
	{
		d = point( i ) - point( i - 1 );
		length += sqrt( VGlobal::multiplyPoints(d,d) );
	}


	return length;
}

double
VSegment::lengthParam( double len ) const
{
	if(
		!prev() ||
		len == 0.0 )		// We divide by len below.
	{
		return 0.0;
	}


	// Optimise the line case.
	if( degree() == 1 )
	{
		return
			len / chordLength();
	}


	// Perform a successive interval bisection.
	double param1 = 0.0;
	double paramMid = 0.5;
	double param2 = 1.0;

	double lengthMid = length( paramMid );

	while( QABS( lengthMid - len ) / len > VGlobal::paramLengthTolerance )
	{
		if( lengthMid < len )
			param1 = paramMid;
		else
			param2 = paramMid;

		paramMid = 0.5 * ( param2 + param1 );

		lengthMid = length( paramMid );
	}

	return paramMid;
}

double
VSegment::nearestPointParam( const QPointF& p ) const
{
	if( !prev() )
	{
		return 1.0;
	}


	/* This function solves the "nearest point on curve" problem. That means, it
	 * calculates the point q (to be precise: it's parameter t) on this segment, which
	 * is located nearest to the input point P.
	 * The basic idea is best described (because it is freely available) in "Phoenix:
	 * An Interactive Curve Design System Based on the Automatic Fitting of
	 * Hand-Sketched Curves", Philip J. Schneider (Master thesis, University of
	 * Washington).
	 *
	 * For the nearest point q = C(t) on this segment, the first derivative is
	 * orthogonal to the distance vector "C(t) - P". In other words we are looking for
	 * solutions of f(t) = ( C(t) - P ) * C'(t) = 0.
	 * ( C(t) - P ) is a nth degree curve, C'(t) a n-1th degree curve => f(t) is a
	 * (2n - 1)th degree curve and thus has up to 2n - 1 distinct solutions.
	 * We solve the problem f(t) = 0 by using something called "Approximate Inversion Method".
	 * Let's write f(t) explicitly (with c_i = p_i - P and d_j = p_{j+1} - p_j):
	 *
	 *         n                     n-1
	 * f(t) = SUM c_i * B^n_i(t)  *  SUM d_j * B^{n-1}_j(t)
	 *        i=0                    j=0
	 *
	 *         n  n-1
	 *      = SUM SUM w_{ij} * B^{2n-1}_{i+j}(t)
	 *        i=0 j=0
	 *
	 * with w_{ij} = c_i * d_j * z_{ij} and
	 *
	 *          BinomialCoeff( n, i ) * BinomialCoeff( n - i ,j )
	 * z_{ij} = -----------------------------------------------
	 *                   BinomialCoeff( 2n - 1, i + j )
	 *
	 * This Bernstein-Bezier polynom representation can now be solved for it's roots.
	 */


	// Calculate the c_i = point( i ) - P.
	QPointF* c = new QPointF[ degree() + 1 ];

	c[ 0 ] = prev()->knot() - p;

	for( unsigned short i = 1; i <= degree(); ++i )
	{
		c[ i ] = point( i - 1 ) - p;
	}


	// Calculate the d_j = point( j + 1 ) - point( j ).
	QPointF* d = new QPointF[ degree() ];

	d[ 0 ] = point( 0 ) - prev()->knot();

	for( unsigned short j = 1; j <= degree() - 1; ++j )
	{
		d[ j ] = 3.0 * ( point( j ) - point( j - 1 ) );
	}


	// Calculate the z_{ij}.
	double* z = new double[ degree() * ( degree() + 1 ) ];

	for( unsigned short j = 0; j <= degree() - 1; ++j )
	{
		for( unsigned short i = 0; i <= degree(); ++i )
		{
			z[ j * ( degree() + 1 ) + i ] =
				static_cast<double>(
					VGlobal::binomialCoeff( degree(), i ) *
					VGlobal::binomialCoeff( degree() - i, j ) )
				/
				static_cast<double>(
					VGlobal::binomialCoeff( 2 * degree() - 1, i + j ) );
		}
	}


	// Calculate the dot products of c_i and d_i.
	double* products = new double[ degree() * ( degree() + 1 ) ];

	for( unsigned short j = 0; j <= degree() - 1; ++j )
	{
		for( unsigned short i = 0; i <= degree(); ++i )
		{
			products[ j * ( degree() + 1 ) + i ] =
				VGlobal::multiplyPoints(d[ j ],c[ i ]);
		}
	}

	// We don't need the c_i and d_i anymore.
	delete[]( d );
	delete[]( c );


	// Calculate the control points of the new 2n-1th degree curve.
	VSubpath newCurve( 0L );
	newCurve.append( new VSegment( 2 * degree() - 1 ) );

	// Set up control points in the ( u, f(u) )-plane.
	for( unsigned short u = 0; u <= 2 * degree() - 1; ++u )
	{
		newCurve.current()->setP(
			u,
			QPointF(
				static_cast<double>( u ) / static_cast<double>( 2 * degree() - 1 ),
				0.0 ) );
	}


	// Set f(u)-values.
	for( unsigned short k = 0; k <= 2 * degree() - 1; ++k )
	{
		unsigned short min = qMin( k, degree() );

		for(
			unsigned short i = qMax( 0, k - ( degree() - 1 ) );
			i <= min;
			++i )
		{
			unsigned short j = k - i;

			// p_k += products[j][i] * z[j][i].
			newCurve.getLast()->setP(
				k,
				QPointF(
					newCurve.getLast()->p( k ).x(),
					newCurve.getLast()->p( k ).y() +
						products[ j * ( degree() + 1 ) + i ] *
							z[ j * ( degree() + 1 ) + i ] ) );
		}
	}

	// We don't need the c_i/d_i dot products and the z_{ij} anymore.
	delete[]( products );
	delete[]( z );

kDebug(38000) << "results" << endl;
for( int i = 0; i <= 2 * degree() - 1; ++i )
{
kDebug(38000) << newCurve.getLast()->p( i ).x() << " "
<< newCurve.getLast()->p( i ).y() << endl;
}
kDebug(38000) << endl;

	// Find roots.
	Q3ValueList<double> params;

	newCurve.getLast()->rootParams( params );


	// Now compare the distances of the candidate points.
	double resultParam;
	double distanceSquared;
	double oldDistanceSquared;
	QPointF dist;

	// First candidate is the previous knot.
	dist = prev()->knot() - p;
	distanceSquared = VGlobal::multiplyPoints(dist,dist);
	resultParam = 0.0;

	// Iterate over the found candidate params.
	for( Q3ValueListConstIterator<double> itr = params.begin(); itr != params.end(); ++itr )
	{
		pointDerivativesAt( *itr, &dist );
		dist -= p;
		oldDistanceSquared = distanceSquared;
		distanceSquared = VGlobal::multiplyPoints(dist,dist);

		if( distanceSquared < oldDistanceSquared )
			resultParam = *itr;
	}

	// Last candidate is the knot.
	dist = knot() - p;
	oldDistanceSquared = distanceSquared;
	distanceSquared = VGlobal::multiplyPoints(dist,dist);

	if( distanceSquared < oldDistanceSquared )
		resultParam = 1.0;


	return resultParam;
}

void
VSegment::rootParams( Q3ValueList<double>& params ) const
{
	if( !prev() )
	{
		return;
	}


	// Calculate how often the control polygon crosses the x-axis
	// This is the upper limit for the number of roots.
	switch( controlPolygonZeros() )
	{
		// No solutions.
		case 0:
			return;
		// Exactly one solution.
		case 1:
			if( isFlat( VGlobal::flatnessTolerance / chordLength() ) )
			{
				// Calculate intersection of chord with x-axis.
				QPointF chord = knot() - prev()->knot();

kDebug(38000) << prev()->knot().x()  << " " << prev()->knot().y()
<< knot().x() << " " << knot().y() << " ---> "
<< ( chord.x() * prev()->knot().y() - chord.y() * prev()->knot().x() ) / - chord.y() << endl;
				params.append(
					( chord.x() * prev()->knot().y() - chord.y() * prev()->knot().x() )
					/ - chord.y() );

				return;
			}
			break;
	}

	// Many solutions. Do recursive midpoint subdivision.
	VSubpath path( *this );
	path.insert( path.current()->splitAt( 0.5 ) );

	path.current()->rootParams( params );
	path.next()->rootParams( params );
}

int
VSegment::controlPolygonZeros() const
{
	if( !prev() )
	{
		return 0;
	}


	int signChanges = 0;

	int sign = VGlobal::sign( prev()->knot().y() );
	int oldSign;

	for( unsigned short i = 0; i < degree(); ++i )
	{
		oldSign = sign;
		sign = VGlobal::sign( point( i ).y() );

		if( sign != oldSign )
		{
			++signChanges;
		}
	}


	return signChanges;
}

bool
VSegment::isSmooth( const VSegment& next ) const
{
	// Return false if this segment is a "begin".
	if( !prev() )
		return false;


	// Calculate tangents.
	QPointF t1;
	QPointF t2;

	pointTangentNormalAt( 1.0, 0L, &t1 );

	next.pointTangentNormalAt( 0.0, 0L, &t2 );


	// Dot product.
	if( VGlobal::multiplyPoints(t1,t2) >= VGlobal::parallelTolerance )
		return true;

	return false;
}

QRectF
VSegment::boundingBox() const
{
	// Initialize with knot.
	QRectF rect( knot(), QSizeF(knot().x(), knot().y()) );

	// Add p0, if it exists.
	if( prev() )
	{
		if( prev()->knot().x() < rect.left() )
			rect.setLeft( prev()->knot().x() );

		if( prev()->knot().x() > rect.right() )
			rect.setRight( prev()->knot().x() );

		if( prev()->knot().y() < rect.top() )
			rect.setTop( prev()->knot().y() );

		if( prev()->knot().y() > rect.bottom() )
			rect.setBottom( prev()->knot().y() );
	}

	if( degree() == 3 )
	{
		/* 
		The basic idea for calculating the axis aligned bounding box (AABB) for bezier segments
		was found in comp.graphics.algorithms:
		
		Both the x coordinate and the y coordinate are polynomial. Newton told 
 		us that at a maximum or minimum the derivative will be zero. Take all 
 		those points, and take the ends; their AABB will be that of the curve. 
		
		We have a helpful trick for the derivatives: use the curve defined by 
 		differences of successive control points. 
		This is a quadratic Bezier curve:
				
				2
		r(t) = Sum Bi,2(t) *Pi = B0,2(t) * P0 + B1,2(t) * P1 + B2,2(t) * P2
			   i=0

		r(t) = (1-t)^2 * P0 + 2t(1-t) * P1 + t^2 * P2

		r(t) = (P2 - 2*P1 + P0) * t^2 + (2*P1 - 2*P0) * t + P0

		Setting r(t) to zero and using the x and y coordinates of differences of
		successive control points lets us find the paramters t, where the original 
		bezier curve has a minimum or a maximum.
		*/
		double t[4];
	
		// calcualting the differnces between successive control points
		QPointF x0 = p(1)-p(0);
		QPointF x1 = p(2)-p(1);
		QPointF x2 = p(3)-p(2);

		// calculating the coefficents
		QPointF a = x2 - 2.0*x1 + x0;
		QPointF b = 2.0*x1 - 2.0*x0;
		QPointF c = x0;

		// calculating parameter t at minimum/maximum in x-direction
		if( a.x() == 0.0 )
		{
			t[0] = - c.x() / b.x();
			t[1] = -1.0;
		}
		else
		{
			double rx = b.x()*b.x() - 4.0*a.x()*c.x();
			if( rx < 0.0 )
				rx = 0.0;
			t[0] = ( -b.x() + sqrt( rx ) ) / (2.0*a.x());
			t[1] = ( -b.x() - sqrt( rx ) ) / (2.0*a.x());
		}

		// calculating parameter t at minimum/maximum in y-direction
		if( a.y() == 0.0 )
		{
			t[2] = - c.y() / b.y();
			t[3] = -1.0;
		}
		else
		{
			double ry = b.y()*b.y() - 4.0*a.y()*c.y();
			if( ry < 0.0 )
				ry = 0.0;
			t[2] = ( -b.y() + sqrt( ry ) ) / (2.0*a.y());
			t[3] = ( -b.y() - sqrt( ry ) ) / (2.0*a.y());
		}

		// calculate points at found minimum/maximum and update bounding box
		for( int i = 0; i < 4; ++i ) 
		{
			if( t[i] >= 0.0 && t[i] <= 1.0 )
			{
				QPointF p = pointAt( t[i] );
	
				if( p.x() < rect.left() )
					rect.setLeft( p.x() );
		
				if( p.x() > rect.right() )
					rect.setRight( p.x() );

				if( p.y() < rect.top() )
					rect.setTop( p.y() );
		
				if( p.y() > rect.bottom() )
					rect.setBottom( p.y() );
			}
		}
	
		return rect;
	}

	for( unsigned short i = 0; i < degree() - 1; ++i )
	{
		if( point( i ).x() < rect.left() )
			rect.setLeft( point( i ).x() );

		if( point( i ).x() > rect.right() )
			rect.setRight( point( i ).x() );

		if( point( i ).y() < rect.top() )
			rect.setTop( point( i ).y() );

		if( point( i ).y() > rect.bottom() )
			rect.setBottom( point( i ).y() );
	}


	return rect;
}

VSegment*
VSegment::splitAt( double t )
{
	if( !prev() )
	{
		return 0L;
	}


	// Create new segment.
	VSegment* segment = new VSegment( degree() );

	// Set segment state.
	segment->m_state = m_state;


	// Lines are easy: no need to modify the current segment.
	if( degree() == 1 )
	{
		segment->setKnot(
			prev()->knot() +
			( knot() - prev()->knot() ) * t );

		return segment;
	}


	// Beziers.

	// Copy points.
	QPointF* q = new QPointF[ degree() + 1 ];

	q[ 0 ] = prev()->knot();

	for( unsigned short i = 0; i < degree(); ++i )
	{
		q[ i + 1 ] = point( i );
	}


	// The De Casteljau algorithm.
	for( unsigned short j = 1; j <= degree(); ++j )
	{
		for( unsigned short i = 0; i <= degree() - j; ++i )
		{
			q[ i ] = ( 1.0 - t ) * q[ i ] + t * q[ i + 1 ];
		}

		// Modify the new segment.
		segment->setPoint( j - 1, q[ 0 ] );
	}

	// Modify the current segment (no need to modify the knot though).
	for( unsigned short i = 1; i < degree(); ++i )
	{
		setPoint( i - 1, q[ i ] );
	}


	delete[]( q );


	return segment;
}

double
VSegment::height(
	const QPointF& a,
	const QPointF& p,
	const QPointF& b )
{
	// Calculate determinant of AP and AB to obtain projection of vector AP to
	// the orthogonal vector of AB.
	const double det =
		p.x() * a.y() + b.x() * p.y() - p.x() * b.y() -
		a.x() * p.y() + a.x() * b.y() - b.x() * a.y();

	// Calculate norm = length(AB).
	const QPointF ab = b - a;
	const double norm = sqrt( VGlobal::multiplyPoints(ab, ab) );

	// If norm is very small, simply use distance AP.
	if( norm < VGlobal::verySmallNumber )
		return
			sqrt(
				( p.x() - a.x() ) * ( p.x() - a.x() ) +
				( p.y() - a.y() ) * ( p.y() - a.y() ) );

	// Normalize.
	return QABS( det ) / norm;
}

bool
VSegment::linesIntersect(
	const QPointF& a0,
	const QPointF& a1,
	const QPointF& b0,
	const QPointF& b1 )
{
	const QPointF delta_a = a1 - a0;
	const double det_a = a1.x() * a0.y() - a1.y() * a0.x();

	const double r_b0 = delta_a.y() * b0.x() - delta_a.x() * b0.y() + det_a;
	const double r_b1 = delta_a.y() * b1.x() - delta_a.x() * b1.y() + det_a;

	if( r_b0 != 0.0 && r_b1 != 0.0 && r_b0 * r_b1 > 0.0 )
		return false;

	const QPointF delta_b = b1 - b0;

	const double det_b = b1.x() * b0.y() - b1.y() * b0.x();

	const double r_a0 = delta_b.y() * a0.x() - delta_b.x() * a0.y() + det_b;
	const double r_a1 = delta_b.y() * a1.x() - delta_b.x() * a1.y() + det_b;

	if( r_a0 != 0.0 && r_a1 != 0.0 && r_a0 * r_a1 > 0.0 )
		return false;

	return true;
}

bool
VSegment::intersects( const VSegment& segment ) const
{
	if(
		!prev() ||
		!segment.prev() )
	{
		return false;
	}


	//TODO: this just dumbs down beziers to lines!
	return linesIntersect( segment.prev()->knot(), segment.knot(), prev()->knot(), knot() );
}

// TODO: Move this function into "userland"
uint
VSegment::nodeNear( const QPointF& p, double isNearRange ) const
{
	int index = 0;

	for( unsigned short i = 0; i < degree(); ++i )
	{
		if( VGlobal::pointsAreNear(point( 0 ), p, isNearRange) )
		{
			index = i + 1;
			break;
		}
	}

	return index;
}

VSegment*
VSegment::revert() const
{
	if( !prev() )
		return 0L;

	// Create new segment.
	VSegment* segment = new VSegment( degree() );

	segment->m_state = m_state;


	// Swap points.
	for( unsigned short i = 0; i < degree() - 1; ++i )
	{
		segment->setPoint( i, point( degree() - 2 - i ) );
	}

	segment->setKnot( prev()->knot() );


	// TODO swap node attributes (selected)

	return segment;
}

VSegment*
VSegment::prev() const
{
	VSegment* segment = m_prev;

	while( segment && segment->state() == deleted )
	{
		segment = segment->m_prev;
	}

	return segment;
}

VSegment*
VSegment::next() const
{
	VSegment* segment = m_next;

	while( segment && segment->state() == deleted )
	{
		segment = segment->m_next;
	}

	return segment;
}

// TODO: remove this backward compatibility function after koffice 1.3.x
void
VSegment::load( const QDomElement& element )
{
	if( element.tagName() == "CURVE" )
	{
		setDegree( 3 );

		setPoint(
			0,
			QPointF(
				element.attribute( "x1" ).toDouble(),
				element.attribute( "y1" ).toDouble() ) );

		setPoint(
			1,
			QPointF(
				element.attribute( "x2" ).toDouble(),
				element.attribute( "y2" ).toDouble() ) );

		setKnot(
			QPointF(
				element.attribute( "x3" ).toDouble(),
				element.attribute( "y3" ).toDouble() ) );
	}
	else if( element.tagName() == "LINE" )
	{
		setDegree( 1 );

		setKnot(
			QPointF(
				element.attribute( "x" ).toDouble(),
				element.attribute( "y" ).toDouble() ) );
	}
	else if( element.tagName() == "MOVE" )
	{
		setDegree( 1 );

		setKnot(
			QPointF(
				element.attribute( "x" ).toDouble(),
				element.attribute( "y" ).toDouble() ) );
	}
}

VSegment*
VSegment::clone() const
{
	return new VSegment( *this );
}

