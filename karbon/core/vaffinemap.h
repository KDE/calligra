/* This file is part of the KDE project
   Copyright (C) 2001, The Karbon Developers
*/

#ifndef __VAFFINEMAP_H__
#define __VAFFINEMAP_H__

// This class implements a 2d affine map (A+v) for karbon.
//
// Affine maps are used for basic vector-operations like translating, scaling,
// shearing, rotating.

class VPoint;

class VAffineMap
{
public:
	// default = the identity:
	VAffineMap(
		const double a11 = 1.0, const double a12 = 0.0,
		const double a21 = 0.0, const double a22 = 1.0,
		const double v1 = 0.0,  const double v2 = 0.0 );

	const double a11() const { return m_a11; }
	const double a12() const { return m_a12; }
	const double a21() const { return m_a21; }
	const double a22() const { return m_a22; }
	const double v1()  const { return m_v1; }
	const double v2()  const { return m_v2; }

	void seta11( const double& a11 ) { m_a11 = a11; }
	void seta12( const double& a12 ) { m_a12 = a12; }
	void seta21( const double& a21 ) { m_a21 = a21; }
	void seta22( const double& a22 ) { m_a22 = a22; }
	void setv1( const double& v1 )   { m_v1  = v1; }
	void setv2( const double& v2 )   { m_v2  = v2; }

	void reset();

	void translate( const double& dx, const double& dy );
	void rotate( const double& ang );
	void mirror( const bool horiz = false, const bool verti = false );
	void scale( const double& sx, const double& sy );
	void shear( const double& sh, const double& sv );
	void skew( const double& ang );

	void mul( const VAffineMap& affMap );

	VPoint map( const VPoint& point ) const;

private:
	// the elements of a 2x2 transformation-matrix:
	double m_a11;
	double m_a12;
	double m_a21;
	double m_a22;

	// the elements of a translation-vector:
	double m_v1;
	double m_v2;
};

#endif
