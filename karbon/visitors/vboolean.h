/* This file is part of the KDE project
   Copyright (C) 2002, The Karbon Developers
*/

#ifndef __VBOOLEAN_H__
#define __VBOOLEAN_H__


#include <qvaluelist.h>

#include "vvisitor.h"


class VSegment;


enum VBooleanType
{
	bool_intersect,
	bool_union,
	bool_xor,
	bool_subtract
};

class VBoolean : public VVisitor
{
public:
	VBoolean( VBooleanType type = bool_intersect ) { m_type = type; }
	virtual ~VBoolean() {}

	VBooleanType type() const { return m_type; }
	void setType( VBooleanType type ) { m_type = type; }

	// we can only visit object pairs:
	virtual void visit( VObject& /*object*/ ) {}
	void visit( VObject& object1, VObject& object2 );

	virtual void visitVSegmentList( VSegmentList& segmentList );

private:
	void doIt();

	typedef QValueList<double> VParamList;
	void recursiveSubdivision(
		const VSegment& segment1, double t0_1, double t1_1,
		const VSegment& segment2, double t0_2, double t1_2,
		VParamList& params1, VParamList& params2 );

	VBooleanType m_type;
	VSegmentList* m_list1;
	VSegmentList* m_list2;
};

#endif

