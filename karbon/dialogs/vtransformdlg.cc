/* This file is part of the KDE project
   Made by Tomislav Lukman (tomislav.lukman@ck.tel.hr)
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
   the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
   Boston, MA 02111-1307, USA.
*/

#include <qcheckbox.h>
#include <qgroupbox.h>
#include <qlayout.h>
#include <qpushbutton.h>
#include <qtabwidget.h>
#include <qvbuttongroup.h>
#include <qlabel.h>

#include <klocale.h>
#include <koMainWindow.h>
#include <koView.h>

#include "koUnitWidgets.h"

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
	mHSpinBox = new KoUnitDoubleSpinBox( groupBox, 0.0, 1000.0, 1.0 );
	mVSpinBox = new KoUnitDoubleSpinBox( groupBox, 0.0, 1000.0, 1.0 );
	mainLayout->addWidget( groupBox );
	
	mRelative = new QCheckBox( i18n( "Relative position" ), this );
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
	/*mAngle = new KoUnitDoubleSpinBox( groupBox, -359.99, 359.99, 1.0 );
	mAngle->setDecimals(2);
	mAngle->setMinValue( -359.99 );
	mAngle->setMaxValue( 359.99 );
	mAngle->setLineStep(1.00);
	mAngle->setSuffix( i18n( "deg" ) );*/
	mHSpinBox = new KoUnitDoubleSpinBox( groupBox, 0.0, 1000.0, 1.0 );
	mVSpinBox = new KoUnitDoubleSpinBox( groupBox, 0.0, 1000.0, 1.0 );
	mainLayout->addWidget( groupBox );
	
	mRelative = new QCheckBox( i18n( "Relative center" ), this );
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
	mHSpinBox = new KoUnitDoubleSpinBox( groupBox, 0.0, 1000.0, 1.0 );
	mVSpinBox = new KoUnitDoubleSpinBox( groupBox, 0.0, 1000.0, 1.0 );
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
	mHSpinBox = new KoUnitDoubleSpinBox( groupBox, 0.0, 1000.0, 1.0 );
	mVSpinBox = new KoUnitDoubleSpinBox( groupBox, 0.0, 1000.0, 1.0 );
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

