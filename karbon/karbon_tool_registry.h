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

#ifndef KARBON_TOOL_REGISTRY_H_
#define KARBON_TOOL_REGISTRY_H_

#include <qvaluevector.h>

class KActionCollection;
class VTool;
class KarbonPart;
class KarbonAbstractToolFactory;

/**
 * A registry where new tool plugins can register themselves.
 */
class KarbonToolRegistry : private QValueVector<KarbonAbstractToolFactory*>
{
public:
	virtual ~KarbonToolRegistry();

	static KarbonToolRegistry* instance();

	QValueVector<VTool*> createTools(KActionCollection * ac, KarbonPart* part);
	void add(KarbonAbstractToolFactory* factory);

private:
	KarbonToolRegistry();
	KarbonToolRegistry(const KarbonToolRegistry&);
	KarbonToolRegistry operator=(const KarbonToolRegistry&);

	static KarbonToolRegistry *m_singleton;
};

#endif // KARBON_TOOL_REGISTRY_H_

