/* This file is part of the KDE project
   Copyright (C) 2001, The Karbon Developers
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
#include <knuminput.h>
#include "vstardlg.h"
#include "karbon_part.h"

VStarDlg::VStarDlg( KarbonPart*part, QWidget* parent, const char* name )
	: KDialog( parent, name, true, Qt::WStyle_Customize |
                   WType_Dialog | Qt::WStyle_NormalBorder | Qt::WStyle_Title ),
          m_part(part)
{
	setCaption( i18n( "Insert Star" ) );

	QBoxLayout* outerbox = new QHBoxLayout( this );

	// add input fields on the left:
	QGroupBox* group = new QGroupBox( 2, Qt::Horizontal, i18n( "Properties" ), this );
 	outerbox->addWidget( group );

	// add width/height-input:

	m_outerRLabel=new QLabel( i18n( "Outer Radius(%1):" ).arg(m_part->getUnitName()), group );
	m_outerR = new KDoubleNumInput( 0, group );
	m_innerRLabel=new QLabel( i18n( "Inner Radius(%1):" ).arg(m_part->getUnitName()), group );
	m_innerR = new KDoubleNumInput( 0, group );
	new QLabel( i18n( "Edges:" ), group );
	m_edges = new QSpinBox( group );
	m_edges->setMinValue( 3 );

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
        resize( 300, 80);
}

double
VStarDlg::innerR() const
{
    	return KoUnit::ptFromUnit(m_innerR->value(),m_part->getUnit()) ;

}

double
VStarDlg::outerR() const
{
    	return KoUnit::ptFromUnit(m_outerR->value(),m_part->getUnit()) ;
}

uint
VStarDlg::edges() const
{
	return m_edges->value();
}

void
VStarDlg::setInnerR( double value )
{
    m_innerR->setValue(KoUnit::ptToUnit( value, m_part->getUnit() ));

}

void
VStarDlg::setOuterR( double value )
{
    m_outerR->setValue(KoUnit::ptToUnit( value, m_part->getUnit() ));
}

void
VStarDlg::setEdges( uint value )
{
	m_edges->setValue( value );
}

void VStarDlg::refreshUnit ()
{
	m_outerRLabel->setText( i18n( "Outer Radius(%1):" ).arg(m_part->getUnitName()));
	m_innerRLabel->setText( i18n( "Inner Radius(%1):" ).arg(m_part->getUnitName()));

}


#include "vstardlg.moc"

