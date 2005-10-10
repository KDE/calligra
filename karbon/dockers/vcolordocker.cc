/* This file is part of the KDE project
   Made by Tomislav Lukman (tomislav.lukman@ck.tel.hr)
   Copyright (C) 2002 - 2005, The Karbon Developers

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

#include <qlabel.h>
#include <qlayout.h>
#include <qtabwidget.h>
#include <qwidget.h>

#include <klocale.h>
#include <koMainWindow.h>

#include "karbon_part.h"
#include "karbon_view.h"
#include "karbon_factory.h"
#include "karbon_resourceserver.h"
#include "vcolor.h"
#include "vcolorslider.h"
#include "vselection.h"
#include "vfillcmd.h"
#include "vstrokecmd.h"

#include "vcolordocker.h"

VColorDocker::VColorDocker( KarbonPart* part, KarbonView* parent, const char* /*name*/ )
	: QWidget(), m_part ( part ), m_view( parent )
{
	m_isStrokeDocker = false;
	setCaption( i18n( "Fill Color" ) );

	mTabWidget = new QTabWidget( this );
	
	/* ##### RGB WIDGET ##### */
	mRGBWidget = new QWidget( mTabWidget );
	QGridLayout *mainLayout = new QGridLayout( mRGBWidget, 4, 1 );
	
	//RGB
	mRedSlider = new VColorSlider( i18n( "R:" ), QColor( "red" ), QColor( "black" ), 0, 255, 0, mRGBWidget );
	mGreenSlider = new VColorSlider( i18n( "G:" ), QColor( "green" ), QColor( "black" ), 0, 255, 0, mRGBWidget );
	mBlueSlider = new VColorSlider( i18n( "B:" ), QColor( "blue" ), QColor( "black" ), 0, 255, 0, mRGBWidget );
	mainLayout->addWidget( mRedSlider, 1, 0 );
	mainLayout->addWidget( mGreenSlider, 2, 0 );
	mainLayout->addWidget( mBlueSlider, 3, 0 );

	//Connections for Sliders
	connect( mRedSlider, SIGNAL( valueChanged ( int ) ), this, SLOT( updateRGB() ) );
	connect( mGreenSlider, SIGNAL( valueChanged ( int ) ), this, SLOT( updateRGB() ) );
	connect( mBlueSlider, SIGNAL( valueChanged ( int ) ), this, SLOT( updateRGB() ) ); 

	mainLayout->activate();
	mTabWidget->addTab( mRGBWidget, i18n( "RGB" ) );
	
	/* ##### CMYK WIDGET ##### */
	mCMYKWidget = new QWidget( mTabWidget );
	QGridLayout *mainCMYKLayout = new QGridLayout( mCMYKWidget, 4, 1);

	//Sliders
	mCyanSlider = new VColorSlider( i18n( "C:" ), QColor( "cyan" ), QColor( "white" ), 0, 100, 0, mCMYKWidget );
	mMagentaSlider = new VColorSlider( i18n( "M:" ), QColor( "magenta" ), QColor( "white" ), 0, 100, 0, mCMYKWidget );
	mYellowSlider = new VColorSlider( i18n( "Y:" ), QColor( "yellow" ), QColor( "white" ), 0, 100, 0, mCMYKWidget );
	mBlackSlider = new VColorSlider( i18n( "K:" ), QColor( "black" ), QColor( "white" ), 0, 100, 0, mCMYKWidget );
	mainCMYKLayout->addWidget( mCyanSlider, 1, 0 );
	mainCMYKLayout->addWidget( mMagentaSlider, 2, 0 );
	mainCMYKLayout->addWidget( mYellowSlider, 3, 0 );
	mainCMYKLayout->addWidget( mBlackSlider, 4, 0 );
	
	//Connections for Sliders
	connect( mCyanSlider, SIGNAL( valueChanged ( int ) ), this, SLOT( updateCMYK() ) );
	connect( mMagentaSlider, SIGNAL( valueChanged ( int ) ), this, SLOT( updateCMYK() ) );
	connect( mYellowSlider, SIGNAL( valueChanged ( int ) ), this, SLOT( updateCMYK() ) );
	connect( mBlackSlider, SIGNAL( valueChanged ( int ) ), this, SLOT( updateCMYK() ) );
	
	mainCMYKLayout->activate();
	mTabWidget->addTab( mCMYKWidget, i18n( "CMYK" ) );

	//Opacity
	mOpacity = new VColorSlider( i18n( "Opacity:" ), QColor( "black" ), QColor( "white" ), 0, 100, 100, this );
	//TODO: Make "white" a transparent color
	connect( mOpacity, SIGNAL( valueChanged ( int ) ), this, SLOT( updateOpacity() ) );

	QVBoxLayout *mainWidgetLayout = new QVBoxLayout( this, 3 );
	mainWidgetLayout->addWidget( mTabWidget );
	mainWidgetLayout->addWidget( mOpacity );
	mainWidgetLayout->activate();
	setMaximumHeight( 174 );
	setMinimumWidth( 194 );
}

VColorDocker::~VColorDocker()
{
}

void VColorDocker::updateRGB()
{
	float r = mRedSlider->value() / 255.0;
	float g = mGreenSlider->value() / 255.0;
	float b = mBlueSlider->value() / 255.0;

	m_oldColor = m_color;

	m_color.setColorSpace( VColor::rgb, false );
	m_color.set( r, g, b );

	changeColor();
}

void VColorDocker::updateCMYK()
{
	float c = mCyanSlider->value() / 100.0;
	float m = mMagentaSlider->value() / 100.0;
	float y = mYellowSlider->value() / 100.0;
	float k = mBlackSlider->value() / 100.0;

	m_oldColor = m_color;

	m_color.setColorSpace( VColor::cmyk, false );
	m_color.set( c, m, y, k );

	changeColor();
}

void VColorDocker::updateOpacity()
{
	float op = mOpacity->value() / 100.0;

	m_oldColor = m_color;

	m_color.setOpacity( op );

	changeColor();
}

void
VColorDocker::mouseReleaseEvent( QMouseEvent * )
{
	changeColor();
}

void
VColorDocker::changeColor()
{
	// check if objects are selected
	int objCnt = m_part->document().selection()->objects().count();
	if( ! objCnt ) 
		return;

	// do not change the color when still dragging one of the sliders or spinning a spinbox
	if( mRedSlider->isDragging() || mGreenSlider->isDragging() || mBlueSlider->isDragging() 
	|| mCyanSlider->isDragging() || mMagentaSlider->isDragging() || mYellowSlider->isDragging() 
	|| mBlackSlider->isDragging() || mOpacity->isDragging() ) 
		return;

	bool colorChanged = false;

	// check if the color really changed
	colorChanged |= (m_oldColor.opacity() != m_color.opacity());
	colorChanged |= (m_oldColor.colorSpace() != m_color.colorSpace());
	colorChanged |= (m_oldColor[0] != m_color[0]);
	colorChanged |= (m_oldColor[1] != m_color[1]);
	colorChanged |= (m_oldColor[2] != m_color[2]);
	colorChanged |= (m_oldColor[3] != m_color[3]);

	if( ! colorChanged )
		return;

	if ( isStrokeDocker() )
		m_part->addCommand( new VStrokeCmd( &m_part->document(), m_color ), true );
	else
		m_part->addCommand( new VFillCmd( &m_part->document(), VFill( m_color ) ), true );

	m_oldColor = m_color;
}

void VColorDocker::setFillDocker()
{
	m_isStrokeDocker = false;
	setCaption( i18n( "Fill Color" ) );
}

void VColorDocker::setStrokeDocker()
{
	m_isStrokeDocker = true;
	setCaption( i18n( "Stroke Color" ) );
}

void VColorDocker::update()
{
	//Disconnect sliders to avoid canvas updating
	disconnect( mRedSlider, SIGNAL( valueChanged ( int ) ), this, SLOT( updateRGB() ) );
	disconnect( mGreenSlider, SIGNAL( valueChanged ( int ) ), this, SLOT( updateRGB() ) );
	disconnect( mBlueSlider, SIGNAL( valueChanged ( int ) ), this, SLOT( updateRGB() ) );   
	disconnect( mCyanSlider, SIGNAL( valueChanged ( int ) ), this, SLOT( updateCMYK() ) );
	disconnect( mMagentaSlider, SIGNAL( valueChanged ( int ) ), this, SLOT( updateCMYK() ) );
	disconnect( mYellowSlider, SIGNAL( valueChanged ( int ) ), this, SLOT( updateCMYK() ) );
	disconnect( mBlackSlider, SIGNAL( valueChanged ( int ) ), this, SLOT( updateCMYK() ) );
	disconnect( mOpacity, SIGNAL( valueChanged ( int ) ), this, SLOT( updateOpacity() ) );
	
	int objCnt = m_part->document().selection()->objects().count();
	
	if( objCnt > 0 )
	{
		VObject *obj = m_part->document().selection()->objects().getFirst();

		m_color = m_oldColor = m_isStrokeDocker ? obj->stroke()->color() : obj->fill()->color();

		//Update sliders
		switch( m_color.colorSpace() )
		{
		case VColor::rgb:
			mRedSlider->setValue( int ( m_color.operator[](0) * 255 ) );
			mGreenSlider->setValue( int ( m_color.operator[](1) * 255 ) );
			mBlueSlider->setValue( int ( m_color.operator[](2) * 255 ) );
			mOpacity->setValue( int ( m_color.opacity() * 100 ) );
			mTabWidget->showPage( mRGBWidget );
		break;
		case VColor::cmyk:
			mCyanSlider->setValue( int ( m_color.operator[](0) * 100 ) );
			mMagentaSlider->setValue( int ( m_color.operator[](1) * 100 ) );
			mYellowSlider->setValue( int ( m_color.operator[](2) * 100 ) );
			mBlackSlider->setValue( int ( m_color.operator[](3) * 100 ) );
			mOpacity->setValue( int ( m_color.opacity() * 100 ) );
			mTabWidget->showPage( mCMYKWidget );
		break;
		default: break;
		}
	}
	
	//Reconnect sliders again
	connect( mRedSlider, SIGNAL( valueChanged ( int ) ), this, SLOT( updateRGB() ) );
	connect( mGreenSlider, SIGNAL( valueChanged ( int ) ), this, SLOT( updateRGB() ) );
	connect( mBlueSlider, SIGNAL( valueChanged ( int ) ), this, SLOT( updateRGB() ) );   
	connect( mCyanSlider, SIGNAL( valueChanged ( int ) ), this, SLOT( updateCMYK() ) );
	connect( mMagentaSlider, SIGNAL( valueChanged ( int ) ), this, SLOT( updateCMYK() ) );
	connect( mYellowSlider, SIGNAL( valueChanged ( int ) ), this, SLOT( updateCMYK() ) );
	connect( mBlackSlider, SIGNAL( valueChanged ( int ) ), this, SLOT( updateCMYK() ) );
	connect( mOpacity, SIGNAL( valueChanged ( int ) ), this, SLOT( updateOpacity() ) );
}

#include "vcolordocker.moc"

