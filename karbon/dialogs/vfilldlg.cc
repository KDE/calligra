/* This file is part of the KDE project
   Copyright (C) 2001, The Karbon Developers
   Copyright (C) 2002, The Karbon Developers
*/

#include <qgroupbox.h>
#include <qlabel.h>
#include <qlayout.h>
#include <qtabwidget.h>

#include <kcolordialog.h>
#include <klocale.h>
#include <knuminput.h>
#include <kselect.h>

#include "karbon_part.h"
#include "vcolor.h"
#include "vfillcmd.h"
#include "vfilldlg.h"
#include "vselection.h"

#include <kdebug.h>


VFillDlg::VFillDlg( KarbonPart* part, QWidget* parent, const char* name )
	: KDialogBase ( parent, name, true, i18n( "Uniform Color" ),
		KDialogBase::Ok | KDialogBase::Cancel ), m_part( part )
{
	QGridLayout *mainLayout;

	QTabWidget *mTabWidget = new QTabWidget( this );
	mRGBWidget = new QWidget( mTabWidget );
	mainLayout = new QGridLayout(mRGBWidget, 3, 3);
	mColorSelector = new KHSSelector( mRGBWidget );
	mColorSelector->setMinimumHeight(165);
	mColorSelector->setMinimumWidth(165);
	connect( mColorSelector, SIGNAL( valueChanged( int, int ) ), this, SLOT( slotHSChanged( int, int ) ) );
	mainLayout->addMultiCellWidget (mColorSelector, 0, 2, 0, 0);

	//Selector
	mSelector = new KGradientSelector( KSelector::Vertical, mRGBWidget );
	mSelector->setColors( QColor( "white" ), QColor( "black" ) );
	mSelector->setMinimumWidth(12);
	//TODO: Make it autochange color if the solid-filled object is selected (also for QSpinBoxes)
	connect( mSelector, SIGNAL( valueChanged( int ) ), this, SLOT( slotVChanged( int ) ) );
	mainLayout->addMultiCellWidget (mSelector, 0, 2, 1, 1);

	//Reference
	QGroupBox* groupbox = new QGroupBox(2, Vertical, i18n("Reference"), mRGBWidget);
	QLabel *mOldText = new QLabel(i18n("Old:"), groupbox);
	QLabel *mNewText = new QLabel(i18n("New:"), groupbox);
	mOldColor = new KColorPatch(groupbox);
	mColorPreview = new KColorPatch(groupbox);
	QColor color( "black" );
	if( part->document().selection()->objects().count() > 0 ) // there is a selection, so take the color of first selected object
		color = part->document().selection()->objects().getFirst()->fill()->color().toQColor();

	mOldColor->setColor( color );
	mColorPreview->setColor( color );
	connect (mColorPreview, SIGNAL( colorChanged( QColor* )), this, SLOT (slotUpdate( QColor* )));
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
	mainLayout->addWidget( cgroupbox, 1, 2);

	//--->Opacity
	QGroupBox* ogroupBox = new QGroupBox(1, Vertical, i18n("Opacity"), mRGBWidget);
	mOpacity = new KIntNumInput(100, ogroupBox);
	mOpacity->setRange(0, 100, 1, true);
	if( part->document().selection()->objects().count() > 0 ) // there is a selection, so take the opacity of first selected object
		mOpacity->setValue( static_cast<int>(part->document().selection()->objects().getFirst()->fill()->color().opacity() * 100.0) );
	mainLayout->addWidget( ogroupBox, 2, 2);

	mainLayout->setSpacing(2);
	mainLayout->setMargin(5);

	connect( this, SIGNAL( okClicked() ), this, SLOT( slotApplyButtonPressed() ) );

	mainLayout->activate();

	mTabWidget->addTab(mRGBWidget, i18n("RGB"));
	setMainWidget( mTabWidget );
	setFixedSize(baseSize());

	mRed->setValue( color.red() );
	mGreen->setValue( color.green() );
	mBlue->setValue( color.blue() );
}

void VFillDlg::slotUpdateFromRGBSpinBoxes()
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
	mSelector->setValue( static_cast<int>(( float( mValue->value() ) / 255.0 ) * 99.0) );
	mSelector->blockSignals( false );

	mHue->blockSignals( false );
	mSaturation->blockSignals( false );
	mValue->blockSignals( false );
}

void VFillDlg::slotUpdateFromHSVSpinBoxes()
{
    QColor color( mHue->value(), mSaturation->value(), mValue->value(), QColor::Hsv );
    mColorPreview->setColor( color );
	mColorPreview->update();

	// update gradient selector
	mSelector->blockSignals( true );
	mSelector->setValue( static_cast<int>(( float( mValue->value() ) / 255.0 ) * 99.0) );
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

void VFillDlg::slotApplyButtonPressed()
{
	VColor color;
	float r = mRed->value() / 255.0, g = mGreen->value() / 255.0, b = mBlue->value() / 255.0;
	float op = mOpacity->value() / 100.0;
	color.setValues( &r, &g, &b, 0L );
	color.setOpacity( op );

	if( m_part )
		m_part->addCommand( new VFillCmd( &m_part->document(), VFill( color ) ), true );
}

void VFillDlg::slotUpdate(QColor *color)
{
	/*mRed->setValue( color->red() );
	mGreen->setValue( color->green() );
	mBlue->setValue( color->blue() );*/
}

void VFillDlg::slotHSChanged( int h, int s )
{
	//QColor color( mHue->value(), mSaturation->value(), newVal, QColor::Hsv );
	mHue->setValue( h );
	mSaturation->setValue( s );
	QColor color1( h, s, 255, QColor::Hsv );
	QColor color2( h, s, 0, QColor::Hsv );
	mSelector->setColors( color1, color2 );
}

void VFillDlg::slotVChanged( int newVal )
{
	//QColor color( mHue->value(), mSaturation->value(), newVal, QColor::Hsv );
	mValue->setValue( static_cast<int>(float( newVal ) / 99.0 * 255.0) );
}

#include "vfilldlg.moc"

