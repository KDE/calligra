/* This file is part of the KDE project
   Made by Tomislav Lukman (tomislav.lukman@ck.tel.hr)
   Copyright (C) 2002, The Karbon Developers
*/

#include <qtabwidget.h>
#include <qlayout.h>
#include <qgroupbox.h>
#include <qhbuttongroup.h>
#include <qpushbutton.h>
#include <qlabel.h>
#include <kcolordialog.h>
#include <knuminput.h>
#include <klocale.h>
#include <koMainWindow.h>
#include <koView.h>

#include "karbon_part.h"
#include "vfillcmd.h"
#include "vstrokecmd.h"
#include "vcolorslider.h"
#include "vcolor.h"

#include "vcolordlg.h"

VColorDlg::VColorDlg( KarbonPart* part, KoView* parent, const char* /*name*/ )
	: QDockWindow( QDockWindow::OutsideDock, parent->shell() ), m_part ( part )
{
	setCaption( i18n( "Color Manager" ) );

	setCloseMode( QDockWindow::Always );

	mTabWidget = new QTabWidget( this );
	mRGBWidget = new QWidget(mTabWidget);

	QGridLayout *mainLayout = new QGridLayout(mRGBWidget, 4, 1);

	//Reference
	QGroupBox* groupbox = new QGroupBox(2, Horizontal, i18n("Reference"), mRGBWidget);
	QLabel *mNewText = new QLabel(i18n("Color:"), groupbox);
	mColorPreview = new KColorPatch(groupbox);
	mColorPreview->setColor( QColor( "black" ) );
	mainLayout->addWidget( groupbox, 0, 0);
	
	//RGB
	QGroupBox* cgroupbox = new QGroupBox(1, Horizontal, i18n("Components"), mRGBWidget);
	mRedSlider = new VColorSlider( i18n("R:"), QColor( "red" ), QColor( "black" ), 0, 255, 0, cgroupbox );
	mGreenSlider = new VColorSlider( i18n("G:"), QColor( "green" ), QColor( "black" ), 0, 255, 0, cgroupbox );
	mBlueSlider = new VColorSlider( i18n("B:"), QColor( "blue" ), QColor( "black" ), 0, 255, 0, cgroupbox );
	mainLayout->addWidget( cgroupbox, 1, 0);

	//Opacity
	QGroupBox* ogroupbox = new QGroupBox(1, Horizontal, i18n("Opacity"), mRGBWidget);
	mOpacity = new KIntNumInput(100, ogroupbox);
	mOpacity->setRange(0, 100, 1, true);
	mainLayout->addWidget( ogroupbox, 2, 0);
	
	//Connections for Sliders
	connect(
		mRedSlider, SIGNAL( valueChanged ( int ) ),
		this, SLOT( updateRGBColorPreview() ) );
	connect(
		mGreenSlider, SIGNAL( valueChanged ( int ) ),
		this, SLOT( updateRGBColorPreview() ) );
	connect(
		mBlueSlider, SIGNAL( valueChanged ( int ) ),
		this, SLOT( updateRGBColorPreview() ) );
	
	//Buttons
	QPushButton *button;
	mButtonGroup = new QHButtonGroup( mRGBWidget );
	button = new QPushButton( i18n( "Outline" ), mButtonGroup );
	mButtonGroup->insert( button, Outline);
	button = new QPushButton( i18n( "Fill" ), mButtonGroup );
	mButtonGroup->insert( button, Fill);
	mainLayout->addWidget( mButtonGroup, 3, 0);
	mainLayout->activate();
	mTabWidget->addTab(mRGBWidget, i18n("RGB"));
	setWidget( mTabWidget );
	
	//Button Connections
	connect(
		mButtonGroup, SIGNAL( clicked ( int ) ),
		this, SLOT( buttonClicked ( int ) ) );
}

void VColorDlg::updateRGBColorPreview()
{
	QColor color( mRedSlider->value(), mGreenSlider->value(), mBlueSlider->value() );
	mColorPreview->setColor( color );
}

void VColorDlg::buttonClicked( int button_ID )
{
	VColor color;
	float r = mRedSlider->value() / 255.0, g = mGreenSlider->value() / 255.0, b = mBlueSlider->value() / 255.0;
	float op = mOpacity->value() / 100.0;
	color.setValues( &r, &g, &b, 0L );
	color.setOpacity( op );
	switch( button_ID ) {
	case Fill:
		if( m_part )
		m_part->addCommand( new VFillCmd( &m_part->document(), color ), true );
		break;
	case Outline:
		if( m_part )
		m_part->addCommand( new VStrokeCmd( &m_part->document(), color ), true );
		break;
	}

}

#include "vcolordlg.moc"

