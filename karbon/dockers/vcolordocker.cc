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
#include <qwidget.h>

#include <kcolordialog.h>
#include <klocale.h>
#include <koMainWindow.h>

#include "karbon_part.h"
#include "karbon_view.h"
#include "vcolor.h"
#include "vcolorslider.h"
#include "vfillcmd.h"
#include "vselection.h"
#include "vstrokecmd.h"

#include "vcolordocker.h"

VColorDocker::VColorDocker( KarbonPart* part, KarbonView* parent, const char* /*name*/ )
	: VDocker( parent->shell() ), m_part ( part ), m_view( parent )
{
	setCaption( i18n( "Color" ) );

	mainWidget = new QWidget( this );
	mTabWidget = new QTabWidget( mainWidget );
	
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

	//Connections for Sliders
	connect( mRedSlider, SIGNAL( valueChanged ( int ) ), this, SLOT( updateRGB() ) );
	connect( mGreenSlider, SIGNAL( valueChanged ( int ) ), this, SLOT( updateRGB() ) );
	connect( mBlueSlider, SIGNAL( valueChanged ( int ) ), this, SLOT( updateRGB() ) );                                                                                              
	
	mainLayout->activate();
	mTabWidget->addTab( mRGBWidget, i18n( "RGB" ) );
	
	/* ##### CMYK WIDGET ##### */
	mCMYKWidget = new QWidget( mTabWidget );
	QGridLayout *mainCMYKLayout = new QGridLayout( mCMYKWidget, 4, 1);
	
	//Reference
	QGroupBox* crgroupbox = new QGroupBox( 2, Horizontal, i18n( "Reference" ), mCMYKWidget );
	new QLabel(i18n("Color:"), crgroupbox );
	mCMYKColorPreview = new KColorPatch( crgroupbox );
	mCMYKColorPreview->setColor( QColor( "white" ) );
	mainCMYKLayout->addWidget( crgroupbox, 0, 0 );
	
	//Sliders
	QGroupBox* csgroupbox = new QGroupBox( 1, Horizontal, i18n( "Components" ), mCMYKWidget );
	mCyanSlider = new VColorSlider( i18n( "C:" ), QColor( "cyan" ), QColor( "white" ), 0, 100, 0, csgroupbox );
	mMagentaSlider = new VColorSlider( i18n( "M:" ), QColor( "magenta" ), QColor( "white" ), 0, 100, 0, csgroupbox );
	mYellowSlider = new VColorSlider( i18n( "Y:" ), QColor( "yellow" ), QColor( "white" ), 0, 100, 0, csgroupbox );
	mBlackSlider = new VColorSlider( i18n( "K:" ), QColor( "black" ), QColor( "white" ), 0, 100, 0, csgroupbox );
	mainCMYKLayout->addWidget( csgroupbox, 1, 0 );
	
	//Connections for Sliders
	connect( mCyanSlider, SIGNAL( valueChanged ( int ) ), this, SLOT( updateCMYK() ) );
	connect( mMagentaSlider, SIGNAL( valueChanged ( int ) ), this, SLOT( updateCMYK() ) );
	connect( mYellowSlider, SIGNAL( valueChanged ( int ) ), this, SLOT( updateCMYK() ) );
	connect( mBlackSlider, SIGNAL( valueChanged ( int ) ), this, SLOT( updateCMYK() ) );
	
	mainCMYKLayout->activate();
	mTabWidget->addTab( mCMYKWidget, i18n("CMYK") );
	
	//Buttons
	mButtonGroup = new QHButtonGroup( mainWidget );
	QPushButton *button = new QPushButton( i18n( "Stroke" ), mButtonGroup );
	mButtonGroup->insert( button, Outline);
	button = new QPushButton( i18n( "Fill" ), mButtonGroup );
	mButtonGroup->insert( button, Fill );
	mainLayout->addWidget( mButtonGroup, 3, 0 );
	connect( mButtonGroup, SIGNAL( clicked ( int ) ), this, SLOT( buttonClicked ( int ) ) );
	
	//Opacity
	mOpacity = new VColorSlider( i18n( "Opacity:" ), QColor( "black" ), QColor( "white" ), 0, 100, 100, mainWidget );
	//TODO: Make "white" a transparent color
	connect( mOpacity, SIGNAL( valueChanged ( int ) ), this, SLOT( updateOpacity() ) );
	
	QVBoxLayout *mainWidgetLayout = new QVBoxLayout( mainWidget, 2 );
	mainWidgetLayout->addWidget( mTabWidget );
	mainWidgetLayout->addWidget( mOpacity );
	mainWidgetLayout->addWidget( mButtonGroup );
	mainWidgetLayout->activate();
	mainWidget->setMinimumWidth( 194 );
	setWidget( mainWidget );
	
	m_Color = new VColor();
}

void VColorDocker::buttonClicked( int button_ID )
{
	switch( button_ID ) {
	case Fill:
		if( m_part )
			m_part->addCommand( new VFillCmd( &m_part->document(), VFill( *m_Color ) ), true );
		m_view->selectionChanged();
		break;
	case Outline:
		if( m_part )
			m_part->addCommand( new VStrokeColorCmd( &m_part->document(), m_Color ), true );
		m_view->selectionChanged();
		break;
	}
}

void VColorDocker::updateRGB()
{
	float r = mRedSlider->value() / 255.0, g = mGreenSlider->value() / 255.0, b = mBlueSlider->value() / 255.0;
	m_Color->setColorSpace( VColor::rgb );
	m_Color->setValues( &r, &g, &b, 0L );
	updateColorPreviews();
}

void VColorDocker::updateCMYK()
{
	float c = mCyanSlider->value() / 100.0, m = mMagentaSlider->value() / 100.0, y = mYellowSlider->value() / 100.0;
	float k = mBlackSlider->value() / 100.0;
	m_Color->setColorSpace( VColor::cmyk );
	m_Color->setValues( &c, &m, &y, &k );
	updateColorPreviews();
}

void VColorDocker::updateOpacity()
{
	float op = mOpacity->value() / 100.0;
	m_Color->setOpacity( op );
}

void VColorDocker::updateColorPreviews()
{
	mRGBColorPreview->setColor( m_Color->toQColor() );
	mCMYKColorPreview->setColor( m_Color->toQColor() );
}
#include "vcolordocker.moc"

