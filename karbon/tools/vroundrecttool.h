/* This file is part of the KDE project
   Copyright (C) 2001, The Karbon Developers
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
   the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
   Boston, MA 02111-1307, USA.
*/

#ifndef __VROUNDRECTTOOL_H__
#define __VROUNDRECTTOOL_H__

#include <qgroupbox.h>

#include <klocale.h>

#include "vshapetool.h"

class KDoubleNumInput;
class KarbonPart;
class QLabel;

class VRoundRectTool : public VShapeTool
{
public:
	VRoundRectTool( KarbonView* view );
	virtual ~VRoundRectTool();

	virtual QWidget *optionsWidget() { return m_optionsWidget; }
	virtual QString name() { return i18n( "Round rectangle tool" ); }

	virtual VComposite* shape( bool interactive = false ) const;

    void refreshUnit();

private:
	class VRoundRectOptionsWidget : public QGroupBox
	{
	public:
		VRoundRectOptionsWidget( KarbonPart *part, QWidget *parent = 0L, const char *name = 0L );

		double width() const;
		double height() const;
		double round() const;
		void setWidth( double value );
		void setHeight( double value );
		void setRound( double value );
		void refreshUnit ();

	private:
		KDoubleNumInput		*m_width;
		KDoubleNumInput		*m_height;
		KDoubleNumInput		*m_round;
		KarbonPart			*m_part;
		QLabel				*m_heightLabel;
		QLabel				*m_widthLabel;
	};

	VRoundRectOptionsWidget* m_optionsWidget;
};

#endif

