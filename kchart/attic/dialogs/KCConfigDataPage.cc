/* This file is part of the KDE project
   Copyright (C) 2001,2002,2003,2004 Laurent Montel <montel@kde.org>

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
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
*/

#include "KCConfigDataPage.h"


#include <klocale.h>
#include <kcolorbutton.h>
#include <kdebug.h>


#include <QLayout>
#include <QGroupBox>
#include <QButtonGroup>
#include <QLabel>
#include <QLineEdit>
#include <q3listbox.h>
//#include <q3buttongroup.h>
#include <QPushButton>
#include <qradiobutton.h>
#include <QCheckBox>
#include <qpainter.h>

#include <kfontdialog.h>
#include <kvbox.h>

// For IRIX
namespace std {}

using namespace std;

#include "kchart_params.h"
#include "kchart_part.h"

namespace KChart
{

KCConfigDataPage::KCConfigDataPage( KChartParams* params,
				    QWidget* parent, 
				    KDChartTableData *dat) 
    : QWidget( parent ), m_params( params ), data(dat)
{
    QVBoxLayout  * layout2;

    QVBoxLayout  *layout = new QVBoxLayout( );
    layout->setMargin( KDialog::marginHint() );
    layout->setSpacing( KDialog::spacingHint() );
    this->setLayout( layout );

    // The Data Area
    QGroupBox *gb1 = new QGroupBox( i18n( "Data Area" ) );
    layout->addWidget( gb1 );

    layout2 = new QVBoxLayout( );
    layout2->setMargin( KDialog::marginHint() );
    layout2->setSpacing( KDialog::spacingHint() );
    gb1->setLayout( layout2 );

    KHBox   *hbox = new KHBox( gb1 );
    (void) new QLabel( i18n("Area: "), hbox);
    m_dataArea = new QLineEdit( hbox );
    layout2->addWidget( hbox );

    // The row/column as label checkboxes. 
    m_firstRowAsLabel = new QCheckBox( i18n( "First row as label" ) );
    layout2->addWidget( m_firstRowAsLabel );
    m_firstColAsLabel = new QCheckBox( i18n( "First column as label" ) );
    layout2->addWidget( m_firstColAsLabel );

    layout2->addStretch( 1 );

    // The Data Format groupbox
    QGroupBox *gb2 = new QGroupBox( i18n( "Data Format" ) );
    layout->addWidget( gb2 );

    layout2 = new QVBoxLayout( );
    layout2->setMargin( KDialog::marginHint() );
    layout2->setSpacing( KDialog::spacingHint() );
    gb2->setLayout( layout2 );

    QButtonGroup *bg = new QButtonGroup( );
    bg->setExclusive( true );

    m_rowMajor = new QRadioButton( i18n( "Data in rows" ), gb2 );
    layout2->addWidget( m_rowMajor );
    bg->addButton( m_rowMajor );

    m_colMajor = new QRadioButton( i18n( "Data in columns" ), gb2 );
    m_colMajor->resize( m_colMajor->sizeHint() );
    layout2->addWidget( m_colMajor );
    bg->addButton( m_colMajor );

    layout2->addStretch( 1 );

    this->setWhatsThis( i18n("This configuration page can be used to swap the interpretation of rows and columns."));
    m_rowMajor->setWhatsThis( i18n("By default one row is considered to be a data set and each column holds the individual values of the data series. This sets the data in rows on your chart."));

    m_colMajor->setWhatsThis( i18n("Here you can choose to have each column hold one data set. Note that the values are not really swapped but only their interpretation."));
    m_colMajor->resize( m_colMajor->sizeHint() );
}


void KCConfigDataPage::init()
{
    if (m_params->dataDirection() == KChartParams::DataRows)
	m_rowMajor->setChecked(true);
    else
	m_colMajor->setChecked(true);

    m_firstRowAsLabel->setChecked( m_params->firstRowAsLabel() );
    m_firstColAsLabel->setChecked( m_params->firstColAsLabel() );
}


void KCConfigDataPage::defaults()
{
    m_colMajor->setChecked( true );
    m_firstRowAsLabel->setChecked( false );
    m_firstColAsLabel->setChecked( false );
}


void KCConfigDataPage::apply()
{
    if (m_rowMajor->isChecked())
	m_params->setDataDirection( KChartParams::DataRows );
    else
	m_params->setDataDirection( KChartParams::DataColumns );

    m_params->setFirstRowAsLabel( m_firstRowAsLabel->isChecked() );
    m_params->setFirstColAsLabel( m_firstColAsLabel->isChecked() );
}


}  //KChart namespace

#include "KCConfigDataPage.moc"
