/* This file is part of the KDE project
   Copyright (C) 2001, The Karbon Developers
   Copyright (C) 2002, The Karbon Developers
*/
#include "karbon_part.h"
#include "vmcmd_fill.h"
#include "vcolor.h"

#include <klocale.h>

#include <qlayout.h>
#include <kcolordialog.h>
#include <qlabel.h>
#include <qspinbox.h>
#include <qgroupbox.h>
#include <kselect.h>

#include "vmdlg_solidfill.h"

#include <kdebug.h>

VMDlgSolidFill::VMDlgSolidFill( KarbonPart *part ) : QTabDialog ( 0L, 0, true ), m_part( part )
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
	mOldColor = new KColorPatch(groupbox);
	mOldColor->setColor(QColor( "black" ) );
	mColorPreview = new KColorPatch(groupbox);
	mColorPreview->setColor(QColor( "black" ) );
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
	connect( mRed, SIGNAL( valueChanged(int) ), this, SLOT( slotUpdateFromRGBSpinBoxes() ) );
	connect( mGreen, SIGNAL( valueChanged(int) ), this, SLOT( slotUpdateFromRGBSpinBoxes() ) );
	connect( mBlue, SIGNAL( valueChanged(int) ), this, SLOT( slotUpdateFromRGBSpinBoxes() ) );

	//--->HSV
	QLabel *mHueText = new QLabel(i18n("H:"), cgroupbox);
	QLabel *mSatText = new QLabel(i18n("S:"), cgroupbox);
	QLabel *mBrText = new QLabel(i18n("V:"), cgroupbox);
	mHue = new QSpinBox( 0, 359, 1, cgroupbox);
	mSaturation = new QSpinBox( 0, 255, 1, cgroupbox);
	mValue = new QSpinBox( 0, 255, 1, cgroupbox);
	connect( mHue, SIGNAL( valueChanged(int) ), this, SLOT( slotUpdateFromHSVSpinBoxes() ) );
	connect( mSaturation, SIGNAL( valueChanged(int) ), this, SLOT( slotUpdateFromHSVSpinBoxes() ) );
	connect( mValue, SIGNAL( valueChanged(int) ), this, SLOT( slotUpdateFromHSVSpinBoxes() ) );

	connect( this, SIGNAL( applyButtonPressed() ), this, SLOT( slotApplyButtonPressed() ) );

	mainLayout->addWidget( cgroupbox, 1, 2);
	mainLayout->setSpacing(2);
	mainLayout->setMargin(5);
	mainLayout->activate();
	addTab(mRGBWidget, i18n("RGB"));
	setFixedSize(baseSize());
}

void VMDlgSolidFill::slotUpdateFromRGBSpinBoxes()
{
	mColorPreview->setColor( QColor( mRed->value(), mGreen->value(), mBlue->value(), QColor::Rgb ) );
	mColorPreview->update();
}

void VMDlgSolidFill::slotUpdateFromHSVSpinBoxes()
{
	mColorPreview->setColor( QColor( mHue->value(), mSaturation->value(), mValue->value(), QColor::Hsv ) );
	mColorPreview->update();
}

void VMDlgSolidFill::slotApplyButtonPressed()
{
	VColor color;
	float r = mRed->value() / 255.0, g = mGreen->value() / 255.0, b = mBlue->value() / 255.0;
	color.setValues( &r, &g, &b, 0L );

	if( m_part )
		m_part->addCommand( new VMCmdFill( m_part, color ), true );
}

#include "vmdlg_solidfill.moc"
