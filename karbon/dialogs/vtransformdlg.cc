/* This file is part of the KDE project
   Made by Tomislav Lukman (tomislav.lukman@ck.tel.hr)
   Copyright (C) 2002, The Karbon Developers
*/

#include <qcheckbox.h>
#include <qgroupbox.h>
#include <qlayout.h>
#include <qpushbutton.h>
#include <qtabwidget.h>
#include <qvbuttongroup.h>

#include <klocale.h>
#include <koMainWindow.h>
#include <koView.h>

#include <tkfloatspinbox.h>

#include "karbon_part.h"
#include "vreference.h"

#include "vtransformdlg.h"

// Translate widget for this dialog...
VTranslateWidget::VTranslateWidget( QWidget* parent ) : QWidget ( parent )
{
	QVBoxLayout* mainLayout = new QVBoxLayout( this );
	
	QGroupBox* groupBox = new QGroupBox( 2, Vertical, i18n( "Position:" ), this );
	new QLabel( i18n( "H:" ), groupBox );
	new QLabel( i18n( "V:" ), groupBox );
	mHSpinBox = new TKUFloatSpinBox( groupBox );
	mHSpinBox->setDecimals(2);
	mHSpinBox->setMinValue(0.00);
	mHSpinBox->setLineStep(1.00);
	mVSpinBox = new TKUFloatSpinBox( groupBox );
	mVSpinBox->setDecimals(2);
	mVSpinBox->setMinValue(0.00);
	mVSpinBox->setLineStep(1.00);
	mainLayout->addWidget( groupBox );
	
	mRelative = new QCheckBox( i18n( "Relative Position" ), this );
	mRelative->setEnabled ( false ); //TODO: Make this useful and enable it
	mainLayout->addWidget( mRelative );
	
	mReference = new VReference( this );
	mainLayout->addWidget( mReference );
	
	mButtonGroup = new QVButtonGroup( this );
	QPushButton* button = new QPushButton( i18n( "Apply to Duplicate" ), mButtonGroup );
	mButtonGroup->insert( button, tr_Duplicate );
	button = new QPushButton( i18n( "Apply" ), mButtonGroup ); 
	mButtonGroup->insert( button, tr_Apply );
	mainLayout->addWidget( mButtonGroup );
	
	mainLayout->activate();
}

// Rotation widget for this dialog...
VRotateWidget::VRotateWidget( QWidget* parent ) : QWidget ( parent )
{
	QVBoxLayout* mainLayout = new QVBoxLayout( this );
	
	QGroupBox* groupBox = new QGroupBox( 3, Vertical, i18n( "Rotation:" ), this );
	new QLabel( i18n( "Angle:" ), groupBox );
	new QLabel( i18n( "H:" ), groupBox );
	new QLabel( i18n( "V:" ), groupBox );
	mAngle = new TKUFloatSpinBox( groupBox );
	mAngle->setDecimals(2);
	mAngle->setMinValue( -359.99 );
	mAngle->setMaxValue( 359.99 );
	mAngle->setLineStep(1.00);
	mAngle->setSuffix( i18n( "deg" ) );
	mHSpinBox = new TKUFloatSpinBox( groupBox );
	mHSpinBox->setDecimals(2);
	mHSpinBox->setMinValue(0.00);
	mHSpinBox->setLineStep(1.00);
	mVSpinBox = new TKUFloatSpinBox( groupBox );
	mVSpinBox->setDecimals(2);
	mVSpinBox->setMinValue(0.00);
	mVSpinBox->setLineStep(1.00);
	mainLayout->addWidget( groupBox );
	
	mRelative = new QCheckBox( i18n( "Relative Center" ), this );
	mRelative->setEnabled ( false ); //TODO: Make this useful and enable it
	mainLayout->addWidget( mRelative );
	
	mReference = new VReference( this );
	mainLayout->addWidget( mReference );
	
	mButtonGroup = new QVButtonGroup( this );
	QPushButton* button = new QPushButton( i18n( "Apply to Duplicate" ), mButtonGroup );
	mButtonGroup->insert( button, tr_Duplicate );
	button = new QPushButton( i18n( "Apply" ), mButtonGroup ); 
	mButtonGroup->insert( button, tr_Apply );
	mainLayout->addWidget( mButtonGroup );
	
	mainLayout->activate();
}

// Shear widget for this dialog...
VShearWidget::VShearWidget( QWidget* parent ) : QWidget ( parent )
{
	QVBoxLayout* mainLayout = new QVBoxLayout( this );
	
	QGroupBox* groupBox = new QGroupBox( 2, Vertical, i18n( "Shear:" ), this );
	new QLabel( i18n( "H:" ), groupBox );
	new QLabel( i18n( "V:" ), groupBox );
	mHSpinBox = new TKUFloatSpinBox( groupBox );
	mHSpinBox->setDecimals(2);
	mHSpinBox->setMinValue(0.00);
	mHSpinBox->setLineStep(1.00);
	mVSpinBox = new TKUFloatSpinBox( groupBox );
	mVSpinBox->setDecimals(2);
	mVSpinBox->setMinValue(0.00);
	mVSpinBox->setLineStep(1.00);
	mainLayout->addWidget( groupBox );
	
	mReference = new VReference( this );
	mainLayout->addWidget( mReference );
	
	mButtonGroup = new QVButtonGroup( this );
	QPushButton* button = new QPushButton( i18n( "Apply to Duplicate" ), mButtonGroup );
	mButtonGroup->insert( button, tr_Duplicate );
	button = new QPushButton( i18n( "Apply" ), mButtonGroup ); 
	mButtonGroup->insert( button, tr_Apply );
	mainLayout->addWidget( mButtonGroup );
	
	mainLayout->activate();
}

// Scale widget for this dialog...
VScaleWidget::VScaleWidget( QWidget* parent ) : QWidget ( parent )
{
	QVBoxLayout* mainLayout = new QVBoxLayout( this );

	QGroupBox* groupBox = new QGroupBox( 2, Vertical, i18n( "Size:" ), this );
	new QLabel( i18n( "H:" ), groupBox );
	new QLabel( i18n( "V:" ), groupBox );
	mHSpinBox = new TKUFloatSpinBox( groupBox );
	mHSpinBox->setDecimals(2);
	mHSpinBox->setMinValue(0.00);
	mHSpinBox->setLineStep(1.00);
	mVSpinBox = new TKUFloatSpinBox( groupBox );
	mVSpinBox->setDecimals(2);
	mVSpinBox->setMinValue(0.00);
	mVSpinBox->setLineStep(1.00);
	mainLayout->addWidget( groupBox );
	
	mRelative = new QCheckBox( i18n( "Non-proportional" ), this );
	mRelative->setEnabled ( false ); //TODO: Make this useful and enable it
	mainLayout->addWidget( mRelative );
	
	mReference = new VReference( this );
	mainLayout->addWidget( mReference );
	
	mButtonGroup = new QVButtonGroup( this );
	QPushButton* button = new QPushButton( i18n( "Apply to Duplicate" ), mButtonGroup );
	mButtonGroup->insert( button, tr_Duplicate );
	button = new QPushButton( i18n( "Apply" ), mButtonGroup ); 
	mButtonGroup->insert( button, tr_Apply );
	mainLayout->addWidget( mButtonGroup );
	
	mainLayout->activate();
}

// The Dialog...
VTransformDlg::VTransformDlg( KarbonPart* part, KoView* parent, const char* /*name*/ )
	: QDockWindow( QDockWindow::OutsideDock, parent->shell() ), m_part ( part )
{
	setCaption( i18n( "Transform" ) );
	setCloseMode( QDockWindow::Always );

	mTabWidget = new QTabWidget( this );
	
	mTranslateWidget = new VTranslateWidget( mTabWidget );
	mTabWidget->addTab( mTranslateWidget, i18n( "TR" ) ); //TODO: Put the icon instead of text
	
	mRotateWidget = new VRotateWidget( mTabWidget );
	mTabWidget->addTab( mRotateWidget, i18n( "RO" ) ); //TODO: Put the icon instead of text
	
	mShearWidget = new VShearWidget( mTabWidget );
	mTabWidget->addTab( mShearWidget, i18n( "SH" ) ); //TODO: Put the icon instead of text
	
	mScaleWidget = new VScaleWidget( mTabWidget );
	mTabWidget->addTab( mScaleWidget, i18n( "SC" ) ); //TODO: Put the icon instead of text
	
	setWidget( mTabWidget );
}

void VTransformDlg::setTab( TabChoice m_tabChoice )
{
	switch( m_tabChoice ) {
		case Translate:
			mTabWidget->showPage( mTranslateWidget );
		case Rotate:
			mTabWidget->showPage( mRotateWidget );
		case Shear:
			mTabWidget->showPage( mShearWidget );
		case Scale:
			mTabWidget->showPage( mScaleWidget );
	}
}


#include "vtransformdlg.moc"

