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

#ifndef __VTOOLBOX_H__
#define __VTOOLBOX_H__
#include <KoToolBox.h>
class VTool;

/// This class...

class VToolBox : public KoToolBox
{
	Q_OBJECT

public:
	VToolBox( KMainWindow *mainWin, const char* name, KInstance* instance );
	virtual ~VToolBox() {}

	void registerTool( VTool * );
	void setupTools();

signals:
	void activeToolChanged( VTool * );
};

#endif

