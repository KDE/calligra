/* This file is part of the KDE project
   Copyright (C) 2002, The Karbon Developers
*/

#ifndef __VBOOLEAN_H__
#define __VBOOLEAN_H__

#include <qvaluelist.h>

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

	// declare but don't define, since a boolean operation with only one object
	// doesn't make much sense:
	virtual void visit( VObject& object );
	void visit( VObject& object1, VObject& object2 );

	virtual void visitVPath(
		VPath& path, QPtrList<VSegmentList>& lists );

private:
	void doIt();
	void recursiveSubdivision();

	// intersection parameters (t):
	QValueList<double> m_params;

	VBooleanType m_type;
	QPtrList<VSegmentList>* m_lists1;
	QPtrList<VSegmentList>* m_lists2;
};

#endif
