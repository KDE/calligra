#ifndef __VAFFINEMAP_H__
#define __VAFFINEMAP_H__

// This class implements a 2d affine map (A+v) for karbon.
//
// Affine maps are used for basic vector-operations like translating, scaling,
// shearing, rotating.

class VAffineMap {
public:
	VAffineMap();

	double a11() const { return m_a11; }
	double a12() const { return m_a12; }
	double a21() const { return m_a21; }
	double a22() const { return m_a22; }
	double dx()  const { return m_v1; }
	double dy()  const { return m_v2; }

private:
	// the elements of a 3x3 matrix:
	double m_a11;
	double m_a12;
	double m_a21;
	double m_a22;

	// the elements of a translation-vector:
	double m_v1;
	double m_v2;
};

#endif
