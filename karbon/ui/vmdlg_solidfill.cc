/* This file is part of the KDE project
   Copyright (C) 2001, The Karbon Developers
   Copyright (C) 2002, The Karbon Developers
*/

#include <klocale.h>

#include <qlayout.h>
#include <kcolordlg.h>
#include <qlabel.h>
#include <qspinbox.h>
#include <qgroupbox.h>
#include <kselect.h>

#include "vmdlg_solidfill.h"

VMDlgSolidFill::VMDlgSolidFill() : QTabDialog ( 0L, 0, true )
{
	setCaption(i18n( "Solid Fill" ));
	setCancelButton();
	QGridLayout *mainLayout;

	mRGBWidget = new QWidget( this );
	mainLayout = new QGridLayout(mRGBWidget, 3, 2);
	mColorSelector = new KHSSelector( mRGBWidget );
	mColorSelector->setMinimumHeight(165);
	mColorSelector->setMinimumWidth(165);
	mainLayout->addMultiCellWidget (mColorSelector, 0, 1, 0, 0);

	//Selector
	mSelector = new KGradientSelector( KSelector::Vertical, mRGBWidget );
	mSelector->setColors( QColor( "white" ), QColor( "black" ) );
	mSelector->setMinimumWidth(12);
	//TODO: Make it autochange color if the solid-filled object is selected (also for QSpinBoxes)
	mainLayout->addMultiCellWidget (mSelector, 0, 1, 1, 1);

	//Reference
	QGroupBox* groupbox = new QGroupBox(2, Vertical, i18n("Reference"), mRGBWidget);
	QLabel *mOldText = new QLabel(i18n("Old:"), groupbox);
	QLabel *mNewText = new QLabel(i18n("New:"), groupbox);
	mColorPreview = new KColorPatch(groupbox);
	mColorPreview->setColor(QColor( "black" ) );
	mOldColor = new KColorPatch(groupbox);
	mOldColor->setColor(QColor( "black" ) );
	mainLayout->addWidget( groupbox, 0, 2);

	//Components
	QGroupBox* cgroupbox = new QGroupBox(3, Vertical, i18n("Components"), mRGBWidget);

	//--->RGB
	QLabel *mRedText = new QLabel(i18n("R:"), cgroupbox);
	QLabel *mGreenText = new QLabel(i18n("G:"), cgroupbox);
	QLabel *mBlueText = new QLabel(i18n("B:"), cgroupbox);
	mRed = new QSpinBox( 0, 255, 1, cgroupbox);
	mGreen = new QSpinBox( 0, 255, 1, cgroupbox);
	mBlue = new QSpinBox( 0, 255, 1, cgroupbox);

	//--->HSV
	QLabel *mHueText = new QLabel(i18n("H:"), cgroupbox);
	QLabel *mSatText = new QLabel(i18n("S:"), cgroupbox);
	QLabel *mBrText = new QLabel(i18n("V:"), cgroupbox);
	mHue = new QSpinBox( 0, 359, 1, cgroupbox);
	mSaturation = new QSpinBox( 0, 255, 1, cgroupbox);
	mValue = new QSpinBox( 0, 255, 1, cgroupbox);
	
	mainLayout->addWidget( cgroupbox, 1, 2);

	mainLayout->setSpacing(2);
	mainLayout->setMargin(5);
	mainLayout->activate();

	addTab(mRGBWidget, i18n("RGB"));

	setFixedSize(baseSize());
}

#include "vmdlg_solidfill.moc"
