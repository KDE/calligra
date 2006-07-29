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

#include "kchartDataConfigPage.h"

#include "kchartDataConfigPage.moc"

#include <kapplication.h>
#include <klocale.h>
#include <kcolorbutton.h>
#include <kdebug.h>

#include <qhbox.h>
#include <qlayout.h>
#include <qlabel.h>
#include <qlineedit.h>
#include <qlistbox.h>
#include <qbuttongroup.h>
#include <qvbuttongroup.h>
#include <qpushbutton.h>
#include <qradiobutton.h>
#include <qcheckbox.h>
#include <qpainter.h>
#include <qwhatsthis.h>

#include <kfontdialog.h>

// For IRIX
namespace std {}

using namespace std;

#include "kchart_params.h"
#include "kchart_part.h"

namespace KChart
{

KChartDataConfigPage::KChartDataConfigPage( KChartParams* params,
					    QWidget* parent, 
					    KDChartTableData *dat) 
    : QWidget( parent ), m_params( params ), data(dat)
{
    QGridLayout *grid1 = new QGridLayout(this, 4, 1, KDialog::marginHint(),
					 KDialog::spacingHint());

    // The Data Area
    QButtonGroup *gb1 = new QVButtonGroup( i18n( "Data Area" ), this );

    // ================================================================
    // This code is copied from kchartWizardSelectDataFormatPage.cc
    QHBox   *hbox = new QHBox( gb1 );
    (void) new QLabel( i18n("Area: "), hbox);
    m_dataArea = new QLineEdit( hbox );
    //grid1->addWidget(gb1, 0, 0);

    // The row/column as label checkboxes. 
    m_firstRowAsLabel = new QCheckBox( i18n( "First row as label" ), gb1);
    m_firstColAsLabel = new QCheckBox( i18n( "First column as label" ), gb1);

    grid1->addWidget(gb1, 0, 0);

    // The Data Format button group
    QButtonGroup *gb = new QVButtonGroup( i18n( "Data Format" ), this );

    m_rowMajor = new QRadioButton( i18n( "Data in rows" ), gb );
    m_rowMajor->resize( m_rowMajor->sizeHint() );

    m_colMajor = new QRadioButton( i18n( "Data in columns" ), gb );
    m_colMajor->resize( m_colMajor->sizeHint() );

    grid1->addWidget(gb, 2, 0);

    QWhatsThis::add(this, i18n("This configuration page can be used to swap the interpretation of rows and columns."));
    QWhatsThis::add(m_rowMajor, i18n("By default one row is considered to be a data set and each column holds the individual values of the data series. This sets the data in rows on your chart."));

    QWhatsThis::add(m_colMajor, i18n("Here you can choose to have each column hold one data set. Note that the values are not really swapped but only their interpretation."));
    m_colMajor->resize( m_colMajor->sizeHint() );
    grid1->addWidget(gb, 1, 0);
    grid1->setColStretch(3, 0);

    grid1->activate();
}


void KChartDataConfigPage::init()
{
    if (m_params->dataDirection() == KChartParams::DataRows)
	m_rowMajor->setChecked(true);
    else
	m_colMajor->setChecked(true);

    m_firstRowAsLabel->setChecked( m_params->firstRowAsLabel() );
    m_firstColAsLabel->setChecked( m_params->firstColAsLabel() );
}


void KChartDataConfigPage::defaults()
{
    m_colMajor->setChecked( true );
    m_firstRowAsLabel->setChecked( false );
    m_firstColAsLabel->setChecked( false );
}


void KChartDataConfigPage::apply()
{
    if (m_rowMajor->isChecked())
	m_params->setDataDirection( KChartParams::DataRows );
    else
	m_params->setDataDirection( KChartParams::DataColumns );

    m_params->setFirstRowAsLabel( m_firstRowAsLabel->isChecked() );
    m_params->setFirstColAsLabel( m_firstColAsLabel->isChecked() );
}


}  //KChart namespace
