/* This file is part of the KDE project
   Copyright (C) 2002, The Karbon Developers
*/

#ifndef __VCLEANUP_H__
#define __VCLEANUP_H__


#include "vvisitor.h"


class VCleanUp : public VVisitor
{
public:
	virtual void visitVLayer( VLayer& layer );
};

#endif

