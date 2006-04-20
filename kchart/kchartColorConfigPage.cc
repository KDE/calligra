/* This file is part of the KDE project
   Copyright (C) 1999 Matthias Kalle Dalheimer <kalle@kde.org>

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

#include "kchartColorConfigPage.h"
#include "kchartColorConfigPage.moc"

#include <kapplication.h>
#include <kdialog.h>
#include <klocale.h>
#include <kcolorbutton.h>
#include <qlayout.h>
#include <qlabel.h>
#include <q3buttongroup.h>


//Added by qt3to4:
#include <Q3VBoxLayout>
#include <Q3GridLayout>
#include <klistbox.h>
#include "kchart_params.h"
#include <kdebug.h>
#include <kvbox.h>

namespace KChart
{

KChartColorConfigPage::KChartColorConfigPage( KChartParams* params,
                                              QWidget* parent, 
					      KDChartTableData *dat ) :
    QWidget( parent ),
    m_params( params ),
    m_data( dat ),
    index( 0 )
{
    this->setWhatsThis( i18n( "This page lets you configure the colors "
                                 "in which your chart is displayed. Each "
                                 "part of the chart can be assigned a "
                                 "different color." ) );

    Q3VBoxLayout* toplevel = new Q3VBoxLayout( this, 10 );
    Q3ButtonGroup* gb = new Q3ButtonGroup( 0, Qt::Vertical, i18n("Colors"), this );
    gb->layout()->setSpacing(KDialog::spacingHint());
    gb->layout()->setMargin(KDialog::marginHint());
    toplevel->addWidget( gb);
    QString wtstr;
    Q3GridLayout* grid = new Q3GridLayout( gb->layout(), 8, 3 );

    QLabel* lineLA = new QLabel( i18n( "&Line color:" ), gb );
    lineLA->setAlignment( Qt::AlignRight | Qt::AlignVCenter );
    grid->addWidget( lineLA, 0, 0 );
    _lineCB = new KColorButton( gb );
    lineLA->setBuddy( _lineCB );
    grid->addWidget( _lineCB, 0, 1 );
    wtstr = i18n( "This is the color that is used for drawing lines like axes." );
    lineLA->setWhatsThis( wtstr );
    _lineCB->setWhatsThis( wtstr );

    QLabel* gridLA = new QLabel( i18n( "&Grid color:" ), gb );
    gridLA->setAlignment( Qt::AlignRight | Qt::AlignVCenter );
    grid->addWidget( gridLA, 1, 0 );
    _gridCB = new KColorButton( gb );
    gridLA->setBuddy( _gridCB );
    grid->addWidget( _gridCB, 1, 1 );
    wtstr = i18n( "Here you can configure the color that is used for the "
                  "chart grid. Of course, this setting will only "
                  "take effect if grid drawing is turned on." );
    gridLA->setWhatsThis( wtstr );
    _gridCB->setWhatsThis( wtstr );

    QLabel* xtitleLA = new QLabel( i18n("&X-title color:" ), gb );
    xtitleLA->setAlignment( Qt::AlignRight | Qt::AlignVCenter );
    grid->addWidget( xtitleLA, 2, 0 );
    _xtitleCB = new KColorButton( gb );
    xtitleLA->setBuddy( _xtitleCB );
    grid->addWidget( _xtitleCB, 2, 1 );
    wtstr = i18n( "This color is used for displaying titles for the "
                  "X (horizontal) axis. This setting overrides the setting "
                  "<i>Title Color</i>." );
    xtitleLA->setWhatsThis( wtstr );
    _xtitleCB->setWhatsThis( wtstr );

    QLabel* ytitleLA = new QLabel( i18n("&Y-title color:" ), gb );
    ytitleLA->setAlignment( Qt::AlignRight | Qt::AlignVCenter );
    grid->addWidget( ytitleLA, 3, 0 );
    _ytitleCB = new KColorButton( gb );
    ytitleLA->setBuddy( _ytitleCB );
    grid->addWidget( _ytitleCB, 3, 1 );
    wtstr = i18n( "This color is used for displaying titles for the "
                  "Y (vertical) axis. This setting overrides the setting "
                  "<i>Title Color</i>." );
    ytitleLA->setWhatsThis( wtstr );
    _ytitleCB->setWhatsThis( wtstr );

#if 0
    QLabel* ytitle2LA = new QLabel( i18n( "Y-title color (2nd axis):" ), gb );
    ytitle2LA->setAlignment( Qt::AlignRight | Qt::AlignVCenter );
    grid->addWidget( ytitle2LA, 4, 0 );
    _ytitle2CB = new KColorButton( gb );
    ytitle2LA->setBuddy( _ytitle2CB );
    grid->addWidget( _ytitle2CB, 4, 1 );
    wtstr = i18n( "This color is used for displaying titles for the "
                  "second Y (vertical) axis. It only takes effect if the "
                  "chart is configured to have a second Y axis. This setting "
                  "overrides the setting <i>Title Color</i>." );
    ytitle2LA->setWhatsThis( wtstr );
    _ytitle2CB->setWhatsThis( wtstr );
#endif

    QLabel* xlabelLA = new QLabel( i18n( "X-label color:" ), gb );
    xlabelLA->setAlignment( Qt::AlignRight | Qt::AlignVCenter );
    grid->addWidget( xlabelLA, 4, 0 );
    _xlabelCB = new KColorButton( gb );
    xlabelLA->setBuddy( _xlabelCB );
    grid->addWidget( _xlabelCB, 4, 1 );
    wtstr = i18n( "Here you can configure the color that is used for "
                  "labeling the X (horizontal) axis" );
    xlabelLA->setWhatsThis( wtstr );
    _xlabelCB->setWhatsThis( wtstr );

    QLabel* ylabelLA = new QLabel( i18n( "Y-label color:" ), gb );
    ylabelLA->setAlignment( Qt::AlignRight | Qt::AlignVCenter );
    grid->addWidget( ylabelLA, 5, 0 );
    _ylabelCB = new KColorButton( gb );
    ylabelLA->setBuddy( _ylabelCB );
    grid->addWidget( _ylabelCB, 5, 1 );
    wtstr = i18n( "Here you can configure the color that is used for "
                  "labeling the Y (vertical) axis" );
    ylabelLA->setWhatsThis( wtstr );
    _ylabelCB->setWhatsThis( wtstr );

#if 0
    QLabel* ylabel2LA = new QLabel( i18n( "Y-label color (2nd axis):" ), gb );
    ylabel2LA->setAlignment( Qt::AlignRight | Qt::AlignVCenter );
    grid->addWidget( ylabel2LA, 7, 0 );
    _ylabel2CB = new KColorButton( gb );
    ylabel2LA->setBuddy( _ylabel2CB );
    grid->addWidget( _ylabel2CB, 7, 1 );
    wtstr = i18n( "Here you can configure the color that is used for "
                  "labeling the second Y (vertical) axis. Of course, "
                  "this setting only takes effect if the chart is "
                  "configured to have two vertical axes." );
    ylabel2LA->setWhatsThis( wtstr );
    _ylabel2CB->setWhatsThis( wtstr );
#endif

    KHBox* dataColorHB = new KHBox( gb );
    grid->addMultiCellWidget( dataColorHB,  0, 7, 2, 2 );
    _dataColorLB = new KListBox(dataColorHB);
    _dataColorCB = new KColorButton( dataColorHB);
    wtstr = i18n( "Choose a row/column in the list on the left and change its color using this button.");
    _dataColorCB->setWhatsThis( wtstr );
    initDataColorList();
    connect( _dataColorLB, SIGNAL(highlighted(int )), this, SLOT(changeIndex(int)));
    connect( _dataColorLB, SIGNAL(doubleClicked ( Q3ListBoxItem * )), this, SLOT(activeColorButton()));



    /*QLabel* edgeLA = new QLabel( i18n( "Edge color (pies only)" ), this );
      edgeLA->setAlignment( AlignRight | AlignVCenter );
      grid->addWidget( edgeLA, 6, 0 );
      _edgeCB = new KColorButton( this );
      grid->addWidget( _edgeCB, 6, 1 );*/

//     for( int i = 0; i < NUMDATACOLORS; i++ ) {
// 	QString labeltext;
// 	labeltext.sprintf( i18n( "Data color #%d:"), i );
// 	QLabel* dataLA = new QLabel( labeltext, this );
// 	dataLA->setAlignment(AlignRight | AlignVCenter);
// 	dataLA->resize( dataLA->sizeHint() );
// 	grid->addWidget( dataLA, i, 2 );
// 	_dataCB[i] = new KColorButton( this );
// 	_dataCB[i]->resize( _dataCB[i]->sizeHint() );
// 	grid->addWidget( _dataCB[i], i, 3 );
// 	grid->addItem( new QSpacerItem( 0,_textCB->height()), i, 0 );
// 	grid->setRowStretch(i,0);
// 	grid->addItem( new QSpacerItem(dataLA->width() + 20, 0 ), 0, 2 );
//     }
}


void KChartColorConfigPage::changeIndex(int newindex)
{
    if(index > m_params->maxDataColor())
        _dataColorLB->setEnabled(false);
    else
    {
        if(!_dataColorCB->isEnabled())
            _dataColorCB->setEnabled(true);
        extColor[index] = _dataColorCB->color();
        _dataColorCB->setColor(extColor[newindex]);
        index=newindex;
    }
}


void KChartColorConfigPage::activeColorButton()
{
    _dataColorCB->animateClick();
}


void KChartColorConfigPage::initDataColorList()
{
    QStringList lst;
    for(uint i = 0; i < m_data->rows(); i++)
    {
        extColor.resize( m_params->maxDataColor() );
        if(i<m_params->maxDataColor())
        {
            _dataColorLB->insertItem(m_params->legendText( i ).isEmpty() ? i18n("Series %1").arg(i+1) :m_params->legendText( i ) );
            extColor[i] =m_params->dataColor(i);
        }
    }
    _dataColorLB->setCurrentItem(0);

	 //Fix crash when we didn't create data
	if ( m_data->rows() == 0 )
		_dataColorCB->setEnabled(false);
	else
    	_dataColorCB->setColor( extColor[index]);
}


void KChartColorConfigPage::apply()
{
	//Nothing to save
	if ( m_data->rows() == 0 )
		return;

    extColor[index] = _dataColorCB->color();
    for(uint i =0;i<m_data->rows();i++)
        if(i<m_params->maxDataColor())
            m_params->setDataColor(i,extColor[i]);
}

}  //KChart namespace
