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
#include <qgrid.h>

#include <klocale.h>
#include <knuminput.h>
#include <koMainWindow.h>
#include <koView.h>

#include "karbon_part.h"

#include "vobjectdlg.h"

VObjectDlg::VObjectDlg( KarbonPart* part, KoView* parent, const char* /*name*/ )
	: QDockWindow( QDockWindow::OutsideDock, parent->shell() ), m_part ( part )
{
	setCaption( i18n( "Selection Properties" ) );
	setCloseMode( QDockWindow::Always );
	
	//Widgets layout:
	QGrid* mainLayout = new QGrid( 2, Vertical, this );
	mainLayout->setSpacing( 5 );
	new QLabel( i18n( "X:" ), mainLayout );
	new QLabel( i18n( "Y:" ), mainLayout );
	m_X = new KDoubleNumInput( 0.00, mainLayout );
	m_Y = new KDoubleNumInput( 0.00, mainLayout );
	new QLabel( i18n( "Width:" ), mainLayout );
	new QLabel( i18n( "Height:" ), mainLayout );
	m_Width = new KDoubleNumInput( 0.00, mainLayout );
	m_Height = new KDoubleNumInput( 0.00, mainLayout );
	new QLabel( i18n( "Rotation:" ), mainLayout );
	m_Rotation = new KDoubleNumInput( 0, mainLayout );
	
	setWidget( mainLayout );
}

VObjectDlg::~VObjectDlg()
{

}

#include "vobjectdlg.moc"

