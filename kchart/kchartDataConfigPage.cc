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

#include <qlayout.h>
#include <qlabel.h>
#include <qlineedit.h>
#include <qlistbox.h>
#include <qbuttongroup.h>
#include <qvbuttongroup.h>
#include <qpushbutton.h>
#include <qradiobutton.h>
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
					    KoChart::Data *dat,
					    KChartAuxiliary *aux) :
    QWidget( parent ), m_params( params ), data(dat), m_aux(aux)
{
    QGridLayout *grid1 = new QGridLayout(this, 2, 1, KDialog::marginHint(),
					 KDialog::spacingHint());

    QButtonGroup* gb = new QVButtonGroup( i18n( "Data Format" ), this );
    QGridLayout *grid2 = new QGridLayout(gb, 2, 1, KDialog::marginHint(),
					 KDialog::spacingHint());
    QWhatsThis::add(gb, i18n("This configuration page can be used to swap the interpretation of rows and columns."));
    m_rowMajor = new QRadioButton( i18n( "Data in rows" ), gb );
    QWhatsThis::add(m_rowMajor, i18n("By default one row is considered to be a data set and each column holds the individual values of the data series."));
    m_rowMajor->resize( m_rowMajor->sizeHint() );
    grid2->addWidget( m_rowMajor, 0, 0);

    m_colMajor = new QRadioButton( i18n( "Data in columns" ), gb );
    QWhatsThis::add(m_colMajor, i18n("Here you can choose to have a each column hold one data set. Note that the values are not really swapped but only their interpretation."));
    m_colMajor->resize( m_colMajor->sizeHint() );
    grid2->addWidget( m_colMajor, 1, 0);
    grid2->setColStretch(1, 0);

    grid2->activate();
    grid1->addWidget(gb, 0, 0);
    grid1->setColStretch(1, 0);

    grid1->activate();

#if 0
    list = new QListBox(this);
    list->resize( list->sizeHint() );
    grid->addMultiCellWidget(list,0,4,0,0);
    fontButton = new QPushButton( this);
    fontButton->setText(i18n("Font..."));

    fontButton->resize( fontButton->sizeHint() );
    grid->addWidget( fontButton,2,1);

    connect( fontButton, SIGNAL(clicked()), this, SLOT(changeLabelFont()));
    connect( list, SIGNAL(doubleClicked ( QListBoxItem * )), this, SLOT(changeLabelFont()));

    initList();
#endif
}


void KChartDataConfigPage::init()
{
    if (m_aux->m_dataDirection == KChartAuxiliary::DataRows)
	m_rowMajor->setChecked(true);
    else
	m_colMajor->setChecked(true);
}


void KChartDataConfigPage::defaults()
{
    m_rowMajor->setChecked(true);
}


void KChartDataConfigPage::apply()
{
    if (m_rowMajor->isChecked())
	m_aux->m_dataDirection = KChartAuxiliary::DataRows;
    else
	m_aux->m_dataDirection = KChartAuxiliary::DataColumns;
}


}  //KChart namespace
