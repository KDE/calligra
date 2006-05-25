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
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
*/

#include <q3groupbox.h>
#include <QLabel>
#include <QLayout>
//Added by qt3to4:
#include <Q3GridLayout>

#include <kcolordialog.h>
#include <klocale.h>
#include <knuminput.h>

#include "vcolor.h"
#include "vfillcmd.h"
#include "vcolortab.h"
#include "vselection.h"

#include <kdebug.h>


VColorTab::VColorTab( const VColor &c, QWidget* parent, const char* name )
	: QTabWidget( parent, name )
{
	Q3GridLayout *mainLayout;

	mRGBWidget = new QWidget( this );
	mainLayout = new Q3GridLayout( mRGBWidget, 3, 3 );
	mColorSelector = new KHSSelector( mRGBWidget );
	mColorSelector->setMinimumHeight( 165 );
	mColorSelector->setMinimumWidth( 165 );
	connect( mColorSelector, SIGNAL( valueChanged( int, int ) ), this, SLOT( slotHSChanged( int, int ) ) );
	mainLayout->addMultiCellWidget(mColorSelector, 0, 2, 0, 0 );

	//Selector
	mSelector = new KGradientSelector( Qt::Vertical, mRGBWidget );
	mSelector->setColors( QColor( "white" ), QColor( "black" ) );
	mSelector->setMinimumWidth( 20 );
	//TODO: Make it autochange color if the solid-filled object is selected (also for QSpinBoxes)
	connect( mSelector, SIGNAL( valueChanged( int ) ), this, SLOT( slotVChanged( int ) ) );
	mainLayout->addMultiCellWidget( mSelector, 0, 2, 1, 1 );

	//Reference
	Q3GroupBox* groupbox = new Q3GroupBox( 2, Qt::Vertical, i18n( "Reference" ), mRGBWidget );
	new QLabel( i18n( "Old:" ), groupbox );
	new QLabel( i18n( "New:" ), groupbox );
	mOldColor = new KColorPatch( groupbox );
	mColorPreview = new KColorPatch( groupbox );

	QColor color( c );
	mOldColor->setColor( color );
	mColorPreview->setColor( color );
	mainLayout->addWidget( groupbox, 0, 2 );

	//Components
	Q3GroupBox* cgroupbox = new Q3GroupBox( 3, Qt::Vertical, i18n( "Components" ), mRGBWidget );

	//--->RGB
	new QLabel( i18n( "R:" ), cgroupbox );
	new QLabel( i18n( "G:" ), cgroupbox );
	new QLabel( i18n( "B:" ), cgroupbox );
	mRed = new KIntSpinBox( 0, 255, 1, 0, cgroupbox );
	mGreen = new KIntSpinBox( 0, 255, 1, 0, cgroupbox );
	mBlue = new KIntSpinBox( 0, 255, 1, 0, cgroupbox );
	connect( mRed, SIGNAL( valueChanged(int) ), this, SLOT( slotUpdateFromRGBSpinBoxes() ) );
	connect( mGreen, SIGNAL( valueChanged(int) ), this, SLOT( slotUpdateFromRGBSpinBoxes() ) );
	connect( mBlue, SIGNAL( valueChanged(int) ), this, SLOT( slotUpdateFromRGBSpinBoxes() ) );

	//--->HSV
/* TODO: i18n needs porting:
	new QLabel( i18n( "Hue:", "H:" ), cgroupbox );
	new QLabel( i18n( "Saturation:", "S:" ), cgroupbox );
	new QLabel( i18n( "Value:", "V:" ), cgroupbox );
*/
	mHue = new KIntSpinBox( 0, 359, 1, 0, cgroupbox );
	mSaturation = new KIntSpinBox( 0, 255, 1, 0, cgroupbox );
	mValue = new KIntSpinBox( 0, 255, 1, 0, cgroupbox );
	connect( mHue, SIGNAL( valueChanged(int) ), this, SLOT( slotUpdateFromHSVSpinBoxes() ) );
	connect( mSaturation, SIGNAL( valueChanged(int) ), this, SLOT( slotUpdateFromHSVSpinBoxes() ) );
	connect( mValue, SIGNAL( valueChanged(int) ), this, SLOT( slotUpdateFromHSVSpinBoxes() ) );
	mainLayout->addWidget( cgroupbox, 1, 2 );

	//--->Opacity
	Q3GroupBox* ogroupBox = new Q3GroupBox( 1, Qt::Vertical, i18n( "Opacity" ), mRGBWidget );
	mOpacity = new KIntNumInput( 100, ogroupBox );
	mOpacity->setRange( 0, 100, 1, true );
	mOpacity->setValue( int( c.opacity() * 100.0 ) );
	mainLayout->addWidget( ogroupBox, 2, 2 );

	mainLayout->setSpacing( 2 );
	mainLayout->setMargin( 5 );

	mainLayout->activate();

	addTab( mRGBWidget, i18n( "RGB" ) );

	mRed->setValue( color.red() );
	mGreen->setValue( color.green() );
	mBlue->setValue( color.blue() );
}

void VColorTab::slotUpdateFromRGBSpinBoxes()
{
	QColor color( mRed->value(), mGreen->value(), mBlue->value(), QColor::Rgb );
	mColorPreview->setColor( color );
	mColorPreview->update();

	// set HSV
	mHue->blockSignals( true );
	mSaturation->blockSignals( true );
	mValue->blockSignals( true );

	int h, s, v;
	color.hsv( &h, &s, &v );
	mHue->setValue( h );
	mSaturation->setValue( s );
	mValue->setValue( v );

	// update gradient selector
	mSelector->blockSignals( true );
	mColorSelector->setValues( h, s );
	slotHSChanged( h, s );
	mSelector->setValue( static_cast<int>( ( float( mValue->value() ) / 255.0 ) * 99.0 ) );
	mSelector->blockSignals( false );

	mHue->blockSignals( false );
	mSaturation->blockSignals( false );
	mValue->blockSignals( false );
}

void VColorTab::slotUpdateFromHSVSpinBoxes()
{
    QColor color( mHue->value(), mSaturation->value(), mValue->value(), QColor::Hsv );
    mColorPreview->setColor( color );
	mColorPreview->update();

	// update gradient selector
	mSelector->blockSignals( true );
	mSelector->setValue( static_cast<int>( ( float( mValue->value() ) / 255.0 ) * 99.0 ) );
	mSelector->blockSignals( false );

	// set RGB
	mRed->blockSignals( true );
	mGreen->blockSignals( true );
	mBlue->blockSignals( true );

	mRed->setValue( color.red() );
	mGreen->setValue( color.green() );
	mBlue->setValue( color.blue() );

	mRed->blockSignals( false );
	mGreen->blockSignals( false );
	mBlue->blockSignals( false );
}

VColor VColorTab::Color()
{
	kDebug() << "VColorTab::slotApplyButtonPressed" << endl;
	float r = mRed->value() / 255.0, g = mGreen->value() / 255.0, b = mBlue->value() / 255.0;
	float op = mOpacity->value() / 100.0;

	VColor c;
	c.set( r, g, b );
	c.setOpacity( op );

	return c;
}

void VColorTab::slotHSChanged( int h, int s )
{
	//QColor color( mHue->value(), mSaturation->value(), newVal, QColor::Hsv );
	mHue->setValue( h );
	mSaturation->setValue( s );
	QColor color1( h, s, 255, QColor::Hsv );
	QColor color2( h, s, 0, QColor::Hsv );
	mSelector->setColors( color1, color2 );
}

void VColorTab::slotVChanged( int newVal )
{
	//QColor color( mHue->value(), mSaturation->value(), newVal, QColor::Hsv );
	mValue->setValue( static_cast<int>( float( newVal ) / 99.0 * 255.0 ) );
}

#include "vcolortab.moc"

