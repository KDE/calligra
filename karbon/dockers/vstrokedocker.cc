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
   the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
   Boston, MA 02111-1307, USA.
*/

#include <qlabel.h>
#include <qlayout.h>
#include <qwidget.h>

#include <klocale.h>
#include <koMainWindow.h>

#include <tkfloatspinbox.h>

#include "karbon_part.h"
#include "karbon_view.h"
#include "vstroke.h"
#include "vcolorslider.h"
#include "vstrokecmd.h"

#include "vstrokedocker.h"

VStrokeDocker::VStrokeDocker( KarbonPart* part, KarbonView* parent, const char* /*name*/ )
	: VDocker( parent->shell() ), m_part ( part ), m_view( parent )
{
	setCaption( i18n( "Stroke Properties" ) );

	mainWidget = new QWidget( this );
	QGridLayout *mainLayout = new QGridLayout( mainWidget, 4, 2 );
	
	QLabel* widthLabel = new QLabel( i18n ( "Width:" ), mainWidget );
	mainLayout->addWidget( widthLabel, 0, 0 );
	m_setLineWidth = new TKUFloatSpinBox( mainWidget );
	m_setLineWidth->setDecimals(1);
	m_setLineWidth->setMinValue(0.0);
	m_setLineWidth->setLineStep(0.5);
	mainLayout->addWidget ( m_setLineWidth, 0, 1 );
	
	mainLayout->activate();
	setWidget( mainWidget );
	
	m_stroke = new VStroke();
}

#include "vstrokedocker.moc"

