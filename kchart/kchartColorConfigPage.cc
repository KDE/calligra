/*
 * $Id$
 *
 * Copyright 1999 by Kalle Dalheimer, released under Artistic License.
 */

#include "kchartColorConfigPage.h"
#include "kchartColorConfigPage.moc"

#include <kapp.h>
#include <klocale.h>
#include <kcolorbtn.h>
#include <qlayout.h>
#include <qlabel.h>
#include <qbuttongroup.h>
#include <qwhatsthis.h>

KChartColorConfigPage::KChartColorConfigPage( QWidget* parent ) :
    QWidget( parent )
{
  QWhatsThis::add( this, i18n( "This page lets you configure the colors "
							   "in which your chart is displayed. Each "
							   "part of the chart can be assigned a "
							   "different color." ) );
  
    QVBoxLayout* toplevel = new QVBoxLayout( this, 10 );
    QButtonGroup* gb = new QButtonGroup( i18n("Colors"), this );
    toplevel->addWidget( gb);

    QGridLayout* grid = new QGridLayout( gb,7, 4, 15,7);

    QLabel* backgroundLA = new QLabel( i18n( "&Background color" ), gb );
    backgroundLA->setAlignment( AlignRight | AlignVCenter );
    grid->addWidget( backgroundLA, 0, 0 );
    _backgroundCB = new KColorButton( gb );
	backgroundLA->setBuddy( _backgroundCB );
    grid->addWidget( _backgroundCB, 0, 1 );
	QString wtstr = i18n( "Here you set the color in which the background "
						  "of the chart is painted." );
	QWhatsThis::add( backgroundLA, wtstr );
	QWhatsThis::add( _backgroundCB, wtstr );

    QLabel* gridLA = new QLabel( i18n( "&Grid color" ), gb );
    gridLA->setAlignment( AlignRight | AlignVCenter );
    grid->addWidget( gridLA, 0, 2 );
    _gridCB = new KColorButton( gb );
	gridLA->setBuddy( _gridCB );
    grid->addWidget( _gridCB, 0, 3 );
	wtstr = i18n( "Here you can configure the color that is used for the "
				  "chart grid. Of course, this setting will only "
				  "take effect if grid drawing is turned on." );
	QWhatsThis::add( gridLA, wtstr );
	QWhatsThis::add( _gridCB, wtstr );

    QLabel* lineLA = new QLabel( i18n( "&Line color" ), gb );
    lineLA->setAlignment( AlignRight | AlignVCenter );
    grid->addWidget( lineLA, 1, 0 );
    _lineCB = new KColorButton( gb );
	lineLA->setBuddy( _lineCB );
    grid->addWidget( _lineCB, 1, 1 );
	wtstr = i18n( "This is the color that is used for drawing lines like axes." );
	QWhatsThis::add( lineLA, wtstr );
	QWhatsThis::add( _lineCB, wtstr );

    QLabel* plotLA = new QLabel( i18n( "&Plot color" ), gb );
    plotLA->setAlignment( AlignRight | AlignVCenter );
    grid->addWidget( plotLA, 1, 2 );
    _plotCB = new KColorButton( gb );
	plotLA->setBuddy( _plotCB );
    grid->addWidget( _plotCB, 1, 3 );
	wtstr = i18n( "This color is used for plotting some chart aspects, "
				  "most notably in pies." );
	QWhatsThis::add( plotLA, wtstr );
	QWhatsThis::add( _plotCB, wtstr );

    QLabel* volLA = new QLabel( i18n( "&Volume color" ), gb );
    volLA->setAlignment( AlignRight | AlignVCenter );
    grid->addWidget( volLA, 2, 0 );
    _volCB = new KColorButton( gb );
	volLA->setBuddy( _volCB );
    grid->addWidget( _volCB, 2, 1 );
	wtstr= i18n( "This color is used for displaying volumes." );
	QWhatsThis::add( volLA, wtstr );
	QWhatsThis::add( _volCB, wtstr );

    QLabel* titleLA = new QLabel( i18n( "&Title color" ), gb );
    titleLA->setAlignment( AlignRight | AlignVCenter );
    grid->addWidget( titleLA, 2, 2 );
    _titleCB = new KColorButton( gb );
	titleLA->setBuddy( _titleCB );
    grid->addWidget( _titleCB, 2, 3 );
	wtstr = i18n( "This color is used for displaying titles for the axes. "
				  "This setting can be overridden by the settings for the "
				  "individual axes." );
	QWhatsThis::add( titleLA, wtstr );
	QWhatsThis::add( _titleCB, wtstr );

    QLabel* xtitleLA = new QLabel( i18n("&X title color" ), gb );
    xtitleLA->setAlignment( AlignRight | AlignVCenter );
    grid->addWidget( xtitleLA, 3, 0 );
    _xtitleCB = new KColorButton( gb );
	xtitleLA->setBuddy( _xtitleCB );
    grid->addWidget( _xtitleCB, 3, 1 );
	wtstr = i18n( "This color is used for displaying titles for the "
				  "X (horizontal) axis. This setting overrides the setting "
				  "<i>Title Color</i>" );
	QWhatsThis::add( xtitleLA, wtstr );
	QWhatsThis::add( _xtitleCB, wtstr );

    QLabel* ytitleLA = new QLabel( i18n("&Y title color" ), gb );
    ytitleLA->setAlignment( AlignRight | AlignVCenter );
    grid->addWidget( ytitleLA, 3, 2 );
    _ytitleCB = new KColorButton( gb );
	ytitleLA->setBuddy( _ytitleCB );
    grid->addWidget( _ytitleCB, 3, 3 );
	wtstr = i18n( "This color is used for displaying titles for the "
				  "Y (vertical) axis. This setting overrides the setting "
				  "<i>Title Color</i>" );
	QWhatsThis::add( ytitleLA, wtstr );
	QWhatsThis::add( _ytitleCB, wtstr );

    QLabel* ytitle2LA = new QLabel( i18n( "Y title color (2nd axis)" ), gb );
    ytitle2LA->setAlignment( AlignRight | AlignVCenter );
    grid->addWidget( ytitle2LA, 4, 0 );
    _ytitle2CB = new KColorButton( gb );
	ytitle2LA->setBuddy( _ytitle2CB );
    grid->addWidget( _ytitle2CB, 4, 1 );
	wtstr = i18n( "This color is used for displaying titles for the "
				  "second Y (vertical) axis. It only takes effect if the "
				  "chart is configured to have a second Y axis. This setting "
				  "overrides the setting <i>Title Color</i>" );
	QWhatsThis::add( ytitle2LA, wtstr );
	QWhatsThis::add( _ytitle2CB, wtstr );

    QLabel* xlabelLA = new QLabel( i18n( "X label color" ), gb );
    xlabelLA->setAlignment( AlignRight | AlignVCenter );
    grid->addWidget( xlabelLA, 4, 2 );
    _xlabelCB = new KColorButton( gb );
	xlabelLA->setBuddy( _xlabelCB );
    grid->addWidget( _xlabelCB, 4, 3 );
	wtstr = i18n( "Here you can configure the color that is used for "
				  "labelling the X (horizontal) axis" );
	QWhatsThis::add( xlabelLA, wtstr );
	QWhatsThis::add( _xlabelCB, wtstr );

    QLabel* ylabelLA = new QLabel( i18n( "Y label color" ), gb );
    ylabelLA->setAlignment( AlignRight | AlignVCenter );
    grid->addWidget( ylabelLA, 5, 0 );
    _ylabelCB = new KColorButton( gb );
	ylabelLA->setBuddy( _ylabelCB );
    grid->addWidget( _ylabelCB, 5, 1 );
	wtstr = i18n( "Here you can configure the color that is used for "
				  "labelling the Y (vertical) axis" );
	QWhatsThis::add( ylabelLA, wtstr );
	QWhatsThis::add( _ylabelCB, wtstr );

    QLabel* ylabel2LA = new QLabel( i18n( "Y label color (2nd axis)" ), gb );
    ylabel2LA->setAlignment( AlignRight | AlignVCenter );
    grid->addWidget( ylabel2LA, 5, 2 );
    _ylabel2CB = new KColorButton( gb );
	ylabel2LA->setBuddy( _ylabel2CB );
    grid->addWidget( _ylabel2CB, 5, 3 );
	wtstr = i18n( "Here you can configure the color that is used for "
				  "labelling the second Y (vertical) axis. Of course, "
				  "this setting only takes effect if the chart is "
				  "configured to have two vertical axes" );
	QWhatsThis::add( ylabel2LA, wtstr );
	QWhatsThis::add( _ylabel2CB, wtstr );

    /*QLabel* edgeLA = new QLabel( i18n( "Edge color (pies only)" ), this );
    edgeLA->setAlignment( AlignRight | AlignVCenter );
    grid->addWidget( edgeLA, 6, 0 );
    _edgeCB = new KColorButton( this );
    grid->addWidget( _edgeCB, 6, 1 );*/

//     for( int i = 0; i < NUMDATACOLORS; i++ ) {
// 	QString labeltext;
// 	labeltext.sprintf( i18n( "Data color #%d"), i );
// 	QLabel* dataLA = new QLabel( labeltext, this );
// 	dataLA->setAlignment(AlignRight | AlignVCenter);
// 	dataLA->resize( dataLA->sizeHint() );
// 	grid->addWidget( dataLA, i, 2 );
// 	_dataCB[i] = new KColorButton( this );
// 	_dataCB[i]->resize( _dataCB[i]->sizeHint() );
// 	grid->addWidget( _dataCB[i], i, 3 );
// 	grid->addRowSpacing(i,_textCB->height());
// 	grid->setRowStretch(i,0);
// 	grid->addColSpacing(2,dataLA->width() + 20);
//     }
}
