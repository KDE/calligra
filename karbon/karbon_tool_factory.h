/*
   Copyright (c) 2004 Boudewijn Rempt <boud@valdyas.org>
   Copyright (c) 2005 Sven Langkamp <longamp@reallygood.de>

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
   the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
   Boston, MA 02111-1307, USA.
*/

#ifndef KARBON_TOOL_FACTORY_H_
#define KARBON_TOOL_FACTORY_H_

#include "kactioncollection.h"
#include "karbon_part.h"
#include <koffice_export.h>
class VTool;

class KARBONBASE_EXPORT KarbonAbstractToolFactory
{
public:
	KarbonAbstractToolFactory();
	virtual ~KarbonAbstractToolFactory();

	virtual VTool * createTool(KActionCollection * ac, KarbonPart* part) = 0;
};

template<class T> class KarbonToolFactory : public KarbonAbstractToolFactory
{
public:
	KarbonToolFactory() {}
	virtual ~KarbonToolFactory() {}

	virtual VTool * createTool(KActionCollection * ac, KarbonPart* part)
	{
		VTool * t = new T( part); 
		Q_CHECK_PTR(t);
		//t -> setup(ac); 
		return t; 
	}
};

#endif // KARBON_TOOL_FACTORY_H_

