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
#include <klocale.h>
#include <koMainWindow.h>
#include <koView.h>

#include "vcolorslider.h"
#include "vmpanel_color.h"

VColorPanel::VColorPanel( KoView* parent, const char* /*name*/ )
	: QDockWindow( QDockWindow::OutsideDock, parent->shell() )
{
	setCaption(i18n( "Color Manager" ));
	setCloseMode( QDockWindow::Always );
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
	QPushButton *button;
	mButtonGroup = new QHButtonGroup( mRGBWidget );
	button = new QPushButton( i18n( "Outline" ), mButtonGroup );
	mButtonGroup->insert( button, Outline);
	button = new QPushButton( i18n( "Fill" ), mButtonGroup );
	mButtonGroup->insert( button, Fill);
	mainLayout->addWidget( mButtonGroup, 2, 0);
	mainLayout->activate();
	mTabWidget->addTab(mRGBWidget, i18n("RGB"));
	setWidget( mTabWidget );
}

#include "vmpanel_color.moc"
