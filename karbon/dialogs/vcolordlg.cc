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
#include <qgroupbox.h>
#include <qhbuttongroup.h>
#include <qpushbutton.h>
#include <qtabwidget.h>

#include <kcolordialog.h>
#include <klocale.h>
#include <knuminput.h>
#include <koMainWindow.h>
#include <koView.h>

#include "karbon_part.h"
#include "vcolor.h"
#include "vcolorslider.h"
#include "vfillcmd.h"
#include "vselection.h"
#include "vstrokecmd.h"

#include "vcolordlg.h"

VColorDlg::VColorDlg( KarbonPart* part, KoView* parent, const char* /*name*/ )
	: QDockWindow( QDockWindow::OutsideDock, parent->shell() ), m_part ( part )
{
	setCaption( i18n( "Color Manager" ) );

	setCloseMode( QDockWindow::Always );

	mTabWidget = new QTabWidget( this );
	
	/* ##### RGB WIDGET ##### */
	mRGBWidget = new QWidget( mTabWidget );
	QGridLayout *mainLayout = new QGridLayout( mRGBWidget, 4, 1 );

	//Reference
	QGroupBox* groupbox = new QGroupBox( 2, Horizontal, i18n( "Reference" ), mRGBWidget );
	new QLabel( i18n( "Color:" ), groupbox );
	mRGBColorPreview = new KColorPatch( groupbox );
	mRGBColorPreview->setColor( QColor( "black" ) );
	mainLayout->addWidget( groupbox, 0, 0);
	
	//RGB
	QGroupBox* cgroupbox = new QGroupBox( 1, Horizontal, i18n( "Components" ), mRGBWidget );
	mRedSlider = new VColorSlider( i18n( "R:" ), QColor( "red" ), QColor( "black" ), 0, 255, 0, cgroupbox );
	mGreenSlider = new VColorSlider( i18n( "G:" ), QColor( "green" ), QColor( "black" ), 0, 255, 0, cgroupbox );
	mBlueSlider = new VColorSlider( i18n( "B:" ), QColor( "blue" ), QColor( "black" ), 0, 255, 0, cgroupbox );
	mainLayout->addWidget( cgroupbox, 1, 0 );

	//Opacity
	QGroupBox* ogroupbox = new QGroupBox( 1, Horizontal, i18n( "Opacity" ), mRGBWidget );
	mRGBOpacity = new KIntNumInput( 100, ogroupbox );
	mRGBOpacity->setRange( 0, 100, 1, true );
	mainLayout->addWidget( ogroupbox, 2, 0 );
	
	//Connections for Sliders
	connect( mRedSlider, SIGNAL( valueChanged ( int ) ), this, SLOT( updateRGBColorPreview() ) );
	connect( mGreenSlider, SIGNAL( valueChanged ( int ) ), this, SLOT( updateRGBColorPreview() ) );
	connect( mBlueSlider, SIGNAL( valueChanged ( int ) ), this, SLOT( updateRGBColorPreview() ) );
	
	//Buttons
	QPushButton *button;
	mRGBButtonGroup = new QHButtonGroup( mRGBWidget );
	button = new QPushButton( i18n( "Stroke" ), mRGBButtonGroup );
	mRGBButtonGroup->insert( button, Outline);
	button = new QPushButton( i18n( "Fill" ), mRGBButtonGroup );
	mRGBButtonGroup->insert( button, Fill );
	mainLayout->addWidget( mRGBButtonGroup, 3, 0 );
	mainLayout->activate();
	mTabWidget->addTab( mRGBWidget, i18n("RGB") );
	
	//Button Connections
	connect( mRGBButtonGroup, SIGNAL( clicked ( int ) ), this, SLOT( buttonRGBClicked ( int ) ) );
	
	/* ##### CMYK WIDGET ##### */
	mCMYKWidget = new QWidget( mTabWidget );
	QGridLayout *mainCMYKLayout = new QGridLayout( mCMYKWidget, 4, 1);
	
	//Reference
	QGroupBox* crgroupbox = new QGroupBox( 2, Horizontal, i18n( "Reference" ), mCMYKWidget );
	new QLabel(i18n("Color:"), crgroupbox );
	mCMYKColorPreview = new KColorPatch( crgroupbox );
	mCMYKColorPreview->setColor( QColor( "black" ) );
	mainCMYKLayout->addWidget( crgroupbox, 0, 0 );
	
	//Sliders
	QGroupBox* csgroupbox = new QGroupBox( 1, Horizontal, i18n( "Components" ), mCMYKWidget );
	mCyanSlider = new VColorSlider( i18n( "C:" ), QColor( "cyan" ), QColor( "white" ), 0, 100, 0, csgroupbox );
	mMagentaSlider = new VColorSlider( i18n( "M:" ), QColor( "magenta" ), QColor( "white" ), 0, 100, 0, csgroupbox );
	mYellowSlider = new VColorSlider( i18n( "Y:" ), QColor( "yellow" ), QColor( "white" ), 0, 100, 0, csgroupbox );
	mBlackSlider = new VColorSlider( i18n( "K:" ), QColor( "black" ), QColor( "white" ), 0, 100, 0, csgroupbox );
	mainCMYKLayout->addWidget( csgroupbox, 1, 0 );
	
	//Connections for Sliders
	connect( mCyanSlider, SIGNAL( valueChanged ( int ) ), this, SLOT( updateCMYKColorPreview() ) );
	connect( mMagentaSlider, SIGNAL( valueChanged ( int ) ), this, SLOT( updateCMYKColorPreview() ) );
	connect( mYellowSlider, SIGNAL( valueChanged ( int ) ), this, SLOT( updateCMYKColorPreview() ) );
	connect( mBlackSlider, SIGNAL( valueChanged ( int ) ), this, SLOT( updateCMYKColorPreview() ) );
	
	//Opacity
	QGroupBox* cogroupbox = new QGroupBox( 1, Horizontal, i18n( "Opacity" ), mCMYKWidget );
	mCMYKOpacity = new KIntNumInput( 100, cogroupbox );
	mCMYKOpacity->setRange( 0, 100, 1, true );
	mainCMYKLayout->addWidget( cogroupbox, 2, 0 );
	
	//Buttons
	mCMYKButtonGroup = new QHButtonGroup( mCMYKWidget );
	button = new QPushButton( i18n( "Stroke" ), mCMYKButtonGroup );
	mCMYKButtonGroup->insert( button, Outline);
	button = new QPushButton( i18n( "Fill" ), mCMYKButtonGroup );
	mCMYKButtonGroup->insert( button, Fill );
	mainCMYKLayout->addWidget( mCMYKButtonGroup, 3, 0 );
	
	//Button Connections
	connect( mCMYKButtonGroup, SIGNAL( clicked ( int ) ), this, SLOT( buttonCMYKClicked ( int ) ) );
	
	mainCMYKLayout->activate();
	mTabWidget->addTab( mCMYKWidget, i18n("CMYK") );
	
	setWidget( mTabWidget );
}

void VColorDlg::updateRGBColorPreview()
{
	QColor color( mRedSlider->value(), mGreenSlider->value(), mBlueSlider->value() );
	mRGBColorPreview->setColor( color );
}

void VColorDlg::updateCMYKColorPreview()
{
	/*QColor color( mRedSlider->value(), mGreenSlider->value(), mBlueSlider->value() );
	mRGBColorPreview->setColor( color );*/
}

void VColorDlg::buttonRGBClicked( int button_ID )
{
	VColor color;
	
	float r = mRedSlider->value() / 255.0, g = mGreenSlider->value() / 255.0, b = mBlueSlider->value() / 255.0;
	float op = mRGBOpacity->value() / 100.0;
	color.setValues( &r, &g, &b, 0L );
	color.setOpacity( op );
	switch( button_ID ) {
	case Fill:
		if( m_part )
		m_part->addCommand( new VFillCmd( &m_part->document(), VFill( color ) ), true );
		break;
	case Outline:
		if( m_part )
		m_part->addCommand( new VStrokeColorCmd( &m_part->document(), &color ), true );
		break;
	}

}

void VColorDlg::buttonCMYKClicked( int button_ID )
{
	/*VColor color;
	
	float r = mRedSlider->value() / 255.0, g = mGreenSlider->value() / 255.0, b = mBlueSlider->value() / 255.0;
	float op = mRGBOpacity->value() / 100.0;
	color.setValues( &r, &g, &b, 0L );
	color.setOpacity( op );
	switch( button_ID ) {
	case Fill:
		if( m_part )
		m_part->addCommand( new VFillCmd( &m_part->document(), VFill( color ) ), true );
		break;
	case Outline:
		if( m_part )
		m_part->addCommand( new VStrokeColorCmd( &m_part->document(), &color ), true );
		break;
	}*/
}

#include "vcolordlg.moc"

