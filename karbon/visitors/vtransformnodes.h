/* This file is part of the KDE project
   Copyright (C) 2002, The Karbon Developers
*/

#ifndef __VTRANSFORMNODES_H__
#define __VTRANSFORMNODES_H__

#include <qwmatrix.h>

#include "vvisitor.h"


class VSegment;


class VTransformNodes : public VVisitor
{
public:
	VTransformNodes( const QWMatrix& m );

	virtual void visitVSegmentList( VSegmentList& segmentList );

private:
	QWMatrix m_matrix;
};

#endif

