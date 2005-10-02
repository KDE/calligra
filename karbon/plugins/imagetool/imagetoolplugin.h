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

#ifndef __VIMAGETOOL_H__
#define __VIMAGETOOL_H__

#include <qstring.h>

#include "vtool.h"
#include <commands/vcommand.h>
#include <core/vkarbonplugin.h>

class KarbonViewBase;
class VImage;

class VImageTool : public VTool, public VKarbonPlugin
{
public:
	VImageTool( KarbonViewBase *view, const char *, const QStringList & );
	~VImageTool(); 

	virtual void activate();
	virtual void deactivate();

	virtual QString name() { return i18n( "Image Tool" ); }
	virtual QString contextHelp();
	virtual QString icon() { return "14_image"; }
	virtual QString statusText();

protected:
	class VInsertImageCmd : public VCommand
	{
	public:
		VInsertImageCmd( VDocument* doc, const QString& name, VImage *image, KoPoint pos );
		virtual ~VInsertImageCmd() {}

		virtual void execute();
		virtual void unexecute();

	protected:
		VImage	*m_image;
		KoPoint	m_pos;
	};

	virtual void mouseButtonRelease();
};

#endif

