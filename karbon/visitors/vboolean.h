/* This file is part of the KDE project
   Copyright (C) 2002, The Karbon Developers
*/

#ifndef __VBOOLEAN_H__
#define __VBOOLEAN_H__

#include "vpath.h"
#include "vvisitor.h"


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

	virtual void visit( VShape& /*object*/ ) {}
	void visit( VShape& object1, VShape& object2 );

	virtual void visitVPath(
		VPath& path, QPtrList<VSegmentList>& lists );

private:
	void doIt();

	typedef QValueList<double> VParamList;
	void recursiveSubdivision(
		const VSegment& segment1, double t0_1, double t1_1,
		const VSegment& segment2, double t0_2, double t1_2,
		VParamList& params1, VParamList& params2 );

	VBooleanType m_type;
	QPtrList<VSegmentList>* m_lists1;
	QPtrList<VSegmentList>* m_lists2;
};

#endif

