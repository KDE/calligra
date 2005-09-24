/* This file is part of the KDE project
   Copyright (C) 2001, 2002, 2003 The Karbon Developers

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

#ifndef __VGRADIENTTOOL_H__
#define __VGRADIENTTOOL_H__

#include <kdialogbase.h>

#include "vtool.h"
#include "vgradient.h"

class VGradientTabWidget;

class VGradientTool : public VTool
{
public:
	VGradientTool( KarbonPart *part );
	virtual ~VGradientTool();

	virtual void activate();

	virtual QString name() { return i18n( "Gradient Tool" ); }
	virtual QString icon() { return "14_gradient"; }
	virtual QString contextHelp();
	virtual bool showDialog() const;

protected:
	virtual void draw();

	virtual void mouseButtonRelease();
	virtual void mouseButtonPress();
	virtual void mouseDragRelease();
	virtual void mouseDrag();
	void cancel();

private:
	class VGradientOptionsWidget : public KDialogBase
	{
	public:
		VGradientOptionsWidget( VGradient *gradient );
		VGradientTabWidget *gradientWidget() { return m_gradientWidget; }
	private:
		VGradientTabWidget *m_gradientWidget;
	};
	VGradient            m_gradient;
	KoPoint              m_current;
	VGradientOptionsWidget*  m_optionsWidget;
};

#endif

