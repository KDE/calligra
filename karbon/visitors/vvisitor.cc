/* This file is part of the KDE project
   Copyright (C) 2002, The Karbon Developers
*/

#include "vobject.h"
#include "vvisitor.h"

void
VVisitor::visit( VObject& object )
{
	object.accept( *this );
}

