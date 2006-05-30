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

#include <QGroupBox>
#include <QLabel>
#include <QLayout>
#include <QGridLayout>

#include <kcolordialog.h>
#include <klocale.h>
#include <knuminput.h>

#include "vcolor.h"
#include "vfillcmd.h"
#include "vcolortab.h"
#include "vselection.h"

#include <kdebug.h>


VColorTab::VColorTab( const VColor &c, QWidget* parent, const char* name )
	: QTabWidget( parent )
{
	setObjectName(name);

	QGridLayout *mainLayout;

	mRGBWidget = new QWidget( this );
	mainLayout = new QGridLayout;
	mColorSelector = new KHSSelector( mRGBWidget );
	mColorSelector->setMinimumHeight( 165 );
	mColorSelector->setMinimumWidth( 165 );
	connect( mColorSelector, SIGNAL( valueChanged( int, int ) ), this, SLOT( slotHSChanged( int, int ) ) );
	mainLayout->addWidget(mColorSelector, 0, 2, 0, 0 );

	//Selector
	mSelector = new KGradientSelector( Qt::Vertical, mRGBWidget );
	mSelector->setColors( QColor( "white" ), QColor( "black" ) );
	mSelector->setMinimumWidth( 20 );
	mSelector->setMinimumHeight( 2 );
	//TODO: Make it autochange color if the solid-filled object is selected (also for QSpinBoxes)
	connect( mSelector, SIGNAL( valueChanged( int ) ), this, SLOT( slotVChanged( int ) ) );
	mainLayout->addWidget( mSelector, 0, 2, 1, 1 );

	//Reference
	QGroupBox* groupbox = new QGroupBox( i18n( "Reference" ), mRGBWidget );

	QGridLayout* layout = new QGridLayout;

	mOldColor = new KColorPatch(groupbox);
	mColorPreview = new KColorPatch(groupbox);

	layout->addWidget(new QLabel(i18n( "Old:" )), 0, 0);
	layout->addWidget(mOldColor, 0, 1);
	layout->addWidget(new QLabel(i18n( "New:" )), 1, 0);
	layout->addWidget(mColorPreview, 1, 1);

	groupbox->setLayout(layout);

	QColor color( c );
	mOldColor->setColor( color );
	mColorPreview->setColor( color );
	mainLayout->addWidget( groupbox, 0, 2 );

	//Components
	QGroupBox* cgroupbox = new QGroupBox( i18n( "Components" ), mRGBWidget );

	QGridLayout* clayout = new QGridLayout;

	//--->RGB
	clayout->addWidget(new QLabel(i18n( "R:" )), 0, 0);
	clayout->addWidget(new QLabel(i18n( "G:" )), 1, 0);
	clayout->addWidget(new QLabel(i18n( "B:" )), 2, 0);
	mRed = new KIntSpinBox( 0, 255, 1, 0, cgroupbox );
	mGreen = new KIntSpinBox( 0, 255, 1, 0, cgroupbox );
	mBlue = new KIntSpinBox( 0, 255, 1, 0, cgroupbox );
	clayout->addWidget(mRed, 0, 1);
	clayout->addWidget(mGreen, 1, 1);
	clayout->addWidget(mBlue, 2, 1);
	connect( mRed, SIGNAL( valueChanged(int) ), this, SLOT( slotUpdateFromRGBSpinBoxes() ) );
	connect( mGreen, SIGNAL( valueChanged(int) ), this, SLOT( slotUpdateFromRGBSpinBoxes() ) );
	connect( mBlue, SIGNAL( valueChanged(int) ), this, SLOT( slotUpdateFromRGBSpinBoxes() ) );

	//--->HSV
	clayout->addWidget(new QLabel(i18nc( "Hue:", "H:" )), 0, 2);
	clayout->addWidget(new QLabel(i18nc( "Saturation:", "S:" )), 1, 2);
	clayout->addWidget(new QLabel(i18nc( "Value:", "V:" )), 2, 2);

	mHue = new KIntSpinBox( 0, 359, 1, 0, cgroupbox );
	mSaturation = new KIntSpinBox( 0, 255, 1, 0, cgroupbox );
	mValue = new KIntSpinBox( 0, 255, 1, 0, cgroupbox );
	clayout->addWidget(mHue, 0, 3);
	clayout->addWidget(mSaturation, 1, 3);
	clayout->addWidget(mValue, 2, 3);
	connect( mHue, SIGNAL( valueChanged(int) ), this, SLOT( slotUpdateFromHSVSpinBoxes() ) );
	connect( mSaturation, SIGNAL( valueChanged(int) ), this, SLOT( slotUpdateFromHSVSpinBoxes() ) );
	connect( mValue, SIGNAL( valueChanged(int) ), this, SLOT( slotUpdateFromHSVSpinBoxes() ) );

	cgroupbox->setLayout(clayout);
	mainLayout->addWidget( cgroupbox, 1, 2 );

	//--->Opacity
	QGroupBox* ogroupBox = new QGroupBox( i18n( "Opacity" ), mRGBWidget );
	QGridLayout* olayout = new QGridLayout;
	mOpacity = new KIntNumInput( 100, ogroupBox );
	mOpacity->setRange( 0, 100, 1, true );
	mOpacity->setValue( int( c.opacity() * 100.0 ) );
	olayout->addWidget(mOpacity,0,0);
	ogroupBox->setLayout(olayout);
	mainLayout->addWidget( ogroupBox, 2, 2 );

	mainLayout->setSpacing( 2 );
	mainLayout->setMargin( 5 );

	mainLayout->activate();

	addTab( mRGBWidget, i18n( "RGB" ) );

	mRed->setValue( color.red() );
	mGreen->setValue( color.green() );
	mBlue->setValue( color.blue() );

	mRGBWidget->setLayout(mainLayout);
}

void VColorTab::slotUpdateFromRGBSpinBoxes()
{
	QColor color;
	color.setRgb( mRed->value(), mGreen->value(), mBlue->value() );
	mColorPreview->setColor( color );
	mColorPreview->update();

	// set HSV
	mHue->blockSignals( true );
	mSaturation->blockSignals( true );
	mValue->blockSignals( true );

	int h = color.hue();
	int s = color.saturation();
	int v = color.value();
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
	QColor color;
	color.setHsv( mHue->value(), mSaturation->value(), mValue->value() );
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
	QColor color1;
	color1.setHsv( h, s, 255 );
	QColor color2;
	color2.setHsv( h, s, 0 );
	mSelector->setColors( color1, color2 );
}

void VColorTab::slotVChanged( int newVal )
{
	//QColor color( mHue->value(), mSaturation->value(), newVal, QColor::Hsv );
	mValue->setValue( static_cast<int>( float( newVal ) / 99.0 * 255.0 ) );
}

#include "vcolortab.moc"

