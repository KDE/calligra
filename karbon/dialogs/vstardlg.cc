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

#include <qgroupbox.h>
#include <qlabel.h>

#include <klocale.h>
#include <knuminput.h>

#include "karbon_part.h"
#include "vstardlg.h"

VStarDlg::VStarDlg( KarbonPart*part, QWidget* parent, const char* name )
	: KDialogBase( parent, name, true, i18n( "Insert Star" ), Ok | Cancel ), m_part(part)
{
	// add input fields:
	QGroupBox* group = new QGroupBox( 2, Qt::Horizontal, i18n( "Properties" ), this );

	// add width/height-input:
	m_outerRLabel=new QLabel( i18n( "Outer Radius(%1):" ).arg(m_part->getUnitName()), group );
	m_outerR = new KDoubleNumInput( 0, group );
	m_innerRLabel=new QLabel( i18n( "Inner Radius(%1):" ).arg(m_part->getUnitName()), group );
	m_innerR = new KDoubleNumInput( 0, group );
	new QLabel( i18n( "Edges:" ), group );
	m_edges = new KIntSpinBox( group );
	m_edges->setMinValue( 3 );
	group->setMinimumWidth( 300 );

	// signals and slots:
	connect( this, SIGNAL( okClicked() ), this, SLOT( accept() ) );
	connect( this, SIGNAL( cancelClicked() ), this, SLOT( reject() ) );
	
	setMainWidget( group );
	setFixedSize( baseSize() );
}

double
VStarDlg::innerR() const
{
	return KoUnit::ptFromUnit( m_innerR->value(),m_part->getUnit() ) ;

}

double
VStarDlg::outerR() const
{
	return KoUnit::ptFromUnit( m_outerR->value(),m_part->getUnit() ) ;
}

uint
VStarDlg::edges() const
{
	return m_edges->value();
}

void
VStarDlg::setInnerR( double value )
{
	m_innerR->setValue( KoUnit::ptToUnit( value, m_part->getUnit() ) );
}

void
VStarDlg::setOuterR( double value )
{
	m_outerR->setValue( KoUnit::ptToUnit( value, m_part->getUnit() ) );
}

void
VStarDlg::setEdges( uint value )
{
	m_edges->setValue( value );
}

void VStarDlg::refreshUnit ()
{
	m_outerRLabel->setText( i18n( "Outer Radius(%1):" ).arg(m_part->getUnitName() ) );
	m_innerRLabel->setText( i18n( "Inner Radius(%1):" ).arg(m_part->getUnitName() ) );
}


#include "vstardlg.moc"

