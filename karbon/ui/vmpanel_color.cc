/* This file is part of the KDE project
   Made by Tomislav Lukman (tomislav.lukman@ck.tel.hr)
   Copyright (C) 2002, The Karbon Developers
*/

#include <qtabwidget.h>
#include <qlayout.h>
#include <qgroupbox.h>
#include <qspinbox.h>
#include <qlabel.h>
#include <kcolordialog.h>
#include <kbuttonbox.h>
#include <klocale.h>
#include <koMainWindow.h>
#include <koView.h>

#include "vcolorslider.h"
#include "vmpanel_color.h"

VColorPanel::VColorPanel( KoView* parent, const char* /*name*/ )
	: QDockWindow( QDockWindow::OutsideDock, parent->shell() )
{
	setCaption(i18n( "Uniform Color" ));

	mTabWidget = new QTabWidget( this );
	mRGBWidget = new QWidget(mTabWidget);
	QGridLayout *mainLayout = new QGridLayout(mRGBWidget, 4, 1);

	//Reference
	QGroupBox* groupbox = new QGroupBox(2, Horizontal, i18n("Reference"), mRGBWidget);
	QLabel *mNewText = new QLabel(i18n("Color:"), groupbox);
	mColorPreview = new KColorPatch(groupbox);
	QColor color( "black" );
	mColorPreview->setColor( color );
	mainLayout->addWidget( groupbox, 0, 0);
	
	//RGB
	QGroupBox* cgroupbox = new QGroupBox(1, Horizontal, i18n("Components"), mRGBWidget);
	mRedSlider = new VColorSlider( i18n("R:"), QColor( "red" ), QColor( "black" ), 0, 255, 0, cgroupbox );
	mGreenSlider = new VColorSlider( i18n("G:"), QColor( "green" ), QColor( "black" ), 0, 255, 0, cgroupbox );
	mBlueSlider = new VColorSlider( i18n("B:"), QColor( "blue" ), QColor( "black" ), 0, 255, 0, cgroupbox );
	mainLayout->addWidget( cgroupbox, 1, 0);


	//Buttons
	mButtonBox = new KButtonBox ( mRGBWidget );
	mButtonBox->addStretch(1);
	mRGBOutlineButton = mButtonBox->addButton( i18n( "Outline" ) );
	mButtonBox->addStretch(1);
	mRGBFillButton = mButtonBox->addButton( i18n( "Fill" ) );
	mButtonBox->addStretch(1);
	mButtonBox->layout();
	mainLayout->addWidget( mButtonBox, 2, 0);
	mainLayout->activate();
	mTabWidget->addTab(mRGBWidget, i18n("RGB"));
	setWidget( mTabWidget );
}

#include "vmpanel_color.moc"
