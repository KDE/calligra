/* This file is part of the KDE project
   Copyright (C) 2001, The Karbon Developers
   Copyright (C) 2002, The Karbon Developers
*/

#include <qcombobox.h>
#include <qevent.h>
#include <qgroupbox.h>
#include <qlabel.h>
#include <qlayout.h>
#include <qlineedit.h>
#include <qpainter.h>
#include <qpushbutton.h>
#include <qspinbox.h>
#include <qstring.h>
#include <qwidget.h>

#include <klocale.h>
#include <kcolorbutton.h>

#include "vgradientdlg.h"

VGradientDlg::VGradientDlg( QWidget* parent, const char* name )
	: KDialog( parent, name, true, Qt::WStyle_Customize |
		WType_Dialog | Qt::WStyle_NormalBorder | Qt::WStyle_Title )
{
	setCaption( i18n( "Insert Gradient" ) );

	QBoxLayout* outerbox = new QHBoxLayout( this );

	// add input fields on the left:
	QGroupBox* group = new QGroupBox( 2, Qt::Horizontal, i18n( "Properties" ), this );
	outerbox->addWidget( group );

	new QLabel( i18n( "Start color:" ), group );
	m_startColor = new KColorButton( group );
	new QLabel( i18n( "End color:" ), group );
	m_endColor = new KColorButton( group );
	new QLabel( i18n( "Gradient target:" ), group );
	m_gradientFill = new QComboBox( false, group );
	m_gradientFill->insertItem( i18n( "Stroke" ), 0 );
	m_gradientFill->insertItem( i18n( "Fill" ), 1 );
	new QLabel( i18n( "Gradient repeat:" ), group );
	m_gradientRepeat = new QComboBox( false, group );
	m_gradientRepeat->insertItem( i18n( "None" ), 0 );
	m_gradientRepeat->insertItem( i18n( "Reflect" ), 1 );
	m_gradientRepeat->insertItem( i18n( "Repeat" ), 2 );
	new QLabel( i18n( "Gradient type:" ), group );
	m_gradientType = new QComboBox( false, group );
	m_gradientType->insertItem( i18n( "Linear" ), 0 );
	m_gradientType->insertItem( i18n( "Radial" ), 1 );
	m_gradientType->insertItem( i18n( "Conical" ), 2 );

	outerbox->addSpacing( 2 );

	// add buttons on the right side:
	QBoxLayout* innerbox = new QVBoxLayout( outerbox );

	innerbox->addStretch();

	QPushButton* okbutton = new QPushButton( i18n( "&Ok" ), this );
	QPushButton* cancelbutton = new QPushButton( i18n( "&Cancel" ), this );

	okbutton->setMaximumSize( okbutton->sizeHint() );
	cancelbutton->setMaximumSize( cancelbutton->sizeHint() );

	okbutton->setFocus();

	innerbox->addWidget( okbutton );
	innerbox->addSpacing( 2 );
	innerbox->addWidget( cancelbutton );

	// signals and slots:
	connect( okbutton, SIGNAL( clicked() ), this, SLOT( accept() ) );
	connect( cancelbutton, SIGNAL( clicked() ), this, SLOT( reject() ) );
}

QColor
VGradientDlg::startColor() const
{
	return m_startColor->color();
}

QColor
VGradientDlg::endColor() const
{
	return m_endColor->color();
}

int
VGradientDlg::gradientRepeat() const
{
	return m_gradientRepeat->currentItem();
}

int
VGradientDlg::gradientType() const
{
	return m_gradientType->currentItem();
}

bool
VGradientDlg::gradientFill() const
{
	return m_gradientFill->currentItem() == 1l;
}

void
VGradientDlg::setGradientRepeat( int type )
{
	m_gradientRepeat->setCurrentItem( type );
}

void
VGradientDlg::setGradientType( int type )
{
	m_gradientType->setCurrentItem( type );
}

void
VGradientDlg::setGradientFill( bool b )
{
	m_gradientFill->setCurrentItem( b );
}

void
VGradientDlg::setStartColor( const QColor &c )
{
	m_startColor->setColor( c );
}

void
VGradientDlg::setEndColor( const QColor &c )
{
	m_endColor->setColor( c );
}

#include "vgradientdlg.moc"

