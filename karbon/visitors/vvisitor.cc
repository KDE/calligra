/* This file is part of the KDE project
   Copyright (C) 2002, The Karbon Developers
*/

#include "vshape.h"
#include "vvisitor.h"

void
VVisitor::visit( VShape& object )
{
	object.accept( *this );
}

