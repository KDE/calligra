/* This file is part of the KDE project
   Copyright (C) 2001, The Karbon Developers
*/

#include <qevent.h>
#include <qgroupbox.h>
#include <qlabel.h>
#include <qlayout.h>
#include <qlineedit.h>
#include <qpainter.h>
#include <qpushbutton.h>
#include <qstring.h>
#include <qwidget.h>

#include <klocale.h>

#include "vcdlg_ellipse.h"

VCDlgEllipse::VCDlgEllipse()
	: KDialog( 0L, i18n( "Ellipse" ), true, Qt::WStyle_Customize |
	  Qt::WStyle_Dialog | Qt::WStyle_NormalBorder | Qt::WStyle_Title )
{
	setCaption( i18n( "Ellipse" ) );

	QBoxLayout* outerbox = new QHBoxLayout( this );

	// add input fields on the left:
	QGroupBox* group = new QGroupBox( 2, Qt::Horizontal, i18n( "Values" ), this );
 	outerbox->addWidget( group );

	// add width/height-input:
	new QLabel( i18n( "Width:" ), group );
	m_width = new QLineEdit( 0, group );
	new QLabel( i18n( "Height:" ), group );
	m_height = new QLineEdit( 0, group );

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

double
VCDlgEllipse::valueWidth() const
{
	return m_width->text().toDouble();
}

double
VCDlgEllipse::valueHeight() const
{
	return m_height->text().toDouble();
}

void
VCDlgEllipse::setValueWidth( const double value )
{
	QString s;
	s.setNum( value, 'f', 3 );
	m_width->setText( s );
}

void
VCDlgEllipse::setValueHeight( const double value )
{
	QString s;
	s.setNum( value, 'f', 3 );
	m_height->setText( s );
}

#include "vcdlg_ellipse.moc"
