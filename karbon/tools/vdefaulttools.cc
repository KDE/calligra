/* This file is part of the KDE project
   Copyright (C) 2002, The Karbon Developers

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
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.

*/

#include <kgenericfactory.h>

#include "karbon_factory.h"
#include "karbon_tool_factory.h"
#include "karbon_tool_registry.h"

#include "vdefaulttools.h"

#include "vellipsetool.h"
#include "vgradienttool.h"
#include "vpatterntool.h"
#include "vpenciltool.h"
#include "vpolygontool.h"
#include "vpolylinetool.h"
#include "vrectangletool.h"
#include "vrotatetool.h"
#include "vroundrecttool.h"
#include "vselectnodestool.h"
#include "vselecttool.h"
#include "vshapetool.h"
#include "vsheartool.h"
#include "vsinustool.h"
#include "vspiraltool.h"
#include "vstartool.h"
#include "vtexttool.h"

typedef KGenericFactory<VDefaultTools> VDefaultToolsFactory;
K_EXPORT_COMPONENT_FACTORY( karbon_defaulttools, VDefaultToolsFactory( "karbon_defaulttools" ) )

VDefaultTools::VDefaultTools(QObject *parent, const QStringList &)
	: KParts::Plugin(parent/*, name*/)
{
	setInstance(VDefaultToolsFactory::instance());

	kDebug() << "VDefaultTools. Class: "
		<< metaObject()->className()
		<< ", Parent: "
		<< parent->metaObject()->className()
		<< "\n";

	if ( parent->inherits("KarbonFactory") )
	{
		KarbonToolRegistry* r = KarbonToolRegistry::instance();
		r->add(new KarbonToolFactory<VSelectTool>());
		r->add(new KarbonToolFactory<VSelectNodesTool>());
		r->add(new KarbonToolFactory<VRotateTool>());
		r->add(new KarbonToolFactory<VShearTool>());
		r->add(new KarbonToolFactory<VEllipseTool>());
		r->add(new KarbonToolFactory<VGradientTool>());
		r->add(new KarbonToolFactory<VPatternTool>());
		r->add(new KarbonToolFactory<VPencilTool>());
		r->add(new KarbonToolFactory<VPolygonTool>());
		r->add(new KarbonToolFactory<VPolylineTool>());
		r->add(new KarbonToolFactory<VRectangleTool>());
		r->add(new KarbonToolFactory<VRoundRectTool>());
		r->add(new KarbonToolFactory<VSinusTool>());
		r->add(new KarbonToolFactory<VSpiralTool>());
		r->add(new KarbonToolFactory<VStarTool>());
		r->add(new KarbonToolFactory<VTextTool>());
	}
}

VDefaultTools::~VDefaultTools()
{
}

#include "vdefaulttools.moc"
