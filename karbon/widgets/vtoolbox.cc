/* This file is part of the KDE project
   Made by Tomislav Lukman (tomislav.lukman@ck.tel.hr)
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

#include "vtoolbox.h"
#include "vtool.h"

#include <kdebug.h>

VToolBox::VToolBox( KMainWindow *mainWin, const char* name, KInstance* instance ) : KoToolBox( mainWin, name, instance, 5 )
{
}

void
VToolBox::registerTool( VTool *tool )
{
	kDebug(38000) << "VToolBox::registerTool : " << tool->name() << endl;
	KoToolBox::registerTool( tool->action(), tool->toolType(), tool->priority() );
}

void
VToolBox::setupTools()
{
	kDebug(38000) << k_funcinfo << endl;
	KoToolBox::setupTools();
}

#include "vtoolbox.moc"
