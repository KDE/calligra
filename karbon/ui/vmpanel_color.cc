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

#include "vmpanel_color.h"

VColorPanel::VColorPanel( KoView* parent, const char* /*name*/ )
	: QDockWindow( QDockWindow::OutsideDock, parent->shell() )
{
	setCaption(i18n( "Uniform Color" ));

	mTabWidget = new QTabWidget( this );
	mRGBWidget = new QWidget(mTabWidget);
	QGridLayout *mainLayout = new QGridLayout(mRGBWidget, 2, 1);

	//Reference
	QGroupBox* groupbox = new QGroupBox(2, Horizontal, i18n("Reference"), mRGBWidget);
	QLabel *mNewText = new QLabel(i18n("Color:"), groupbox);
	mColorPreview = new KColorPatch(groupbox);
	QColor color( "black" );
	mColorPreview->setColor( color );
	mainLayout->addWidget( groupbox, 0, 0);
	
	//RGB
	QGroupBox* cgroupbox = new QGroupBox(3, Horizontal, i18n("Components"), mRGBWidget);
	QLabel *mRedText = new QLabel(i18n("R:"), cgroupbox);
	QLabel *mGreenText = new QLabel(i18n("G:"), cgroupbox);
	QLabel *mBlueText = new QLabel(i18n("B:"), cgroupbox);
	mRed = new QSpinBox( 0, 255, 1, cgroupbox);
	mRed->setValue( color.red() );
	mGreen = new QSpinBox( 0, 255, 1, cgroupbox);
	mGreen->setValue( color.green() );
	mBlue = new QSpinBox( 0, 255, 1, cgroupbox);
	mBlue->setValue( color.blue() );
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
