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

KChartColorConfigPage::KChartColorConfigPage( QWidget* parent ) :
    QWidget( parent )
{
    QVBoxLayout* toplevel = new QVBoxLayout( this, 10 );
    QButtonGroup* gb = new QButtonGroup( i18n("Colors"), this );
    toplevel->addWidget( gb);

    QGridLayout* grid = new QGridLayout( gb,7, 4, 15,7);

    QLabel* backgroundLA = new QLabel( i18n( "Background color" ), gb );
    backgroundLA->setAlignment( AlignRight | AlignVCenter );
    grid->addWidget( backgroundLA, 0, 0 );
    _backgroundCB = new KColorButton( gb );
    grid->addWidget( _backgroundCB, 0, 1 );

    QLabel* gridLA = new QLabel( i18n( "Grid color" ), gb );
    gridLA->setAlignment( AlignRight | AlignVCenter );
    grid->addWidget( gridLA, 0, 2 );
    _gridCB = new KColorButton( gb );
    grid->addWidget( _gridCB, 0, 3 );

    QLabel* lineLA = new QLabel( i18n( "Line color" ), gb );
    lineLA->setAlignment( AlignRight | AlignVCenter );
    grid->addWidget( lineLA, 1, 0 );
    _lineCB = new KColorButton( gb );
    grid->addWidget( _lineCB, 1, 1 );

    QLabel* plotLA = new QLabel( i18n( "Plot color" ), gb );
    plotLA->setAlignment( AlignRight | AlignVCenter );
    grid->addWidget( plotLA, 1, 2 );
    _plotCB = new KColorButton( gb );
    grid->addWidget( _plotCB, 1, 3 );

    QLabel* volLA = new QLabel( i18n( "Volume color" ), gb );
    volLA->setAlignment( AlignRight | AlignVCenter );
    grid->addWidget( volLA, 2, 0 );
    _volCB = new KColorButton( gb );
    grid->addWidget( _volCB, 2, 1 );

    QLabel* titleLA = new QLabel( i18n( "Title color" ), gb );
    titleLA->setAlignment( AlignRight | AlignVCenter );
    grid->addWidget( titleLA, 2, 2 );
    _titleCB = new KColorButton( gb );
    grid->addWidget( _titleCB, 2, 3 );

    QLabel* xtitleLA = new QLabel( i18n("X title color" ), gb );
    xtitleLA->setAlignment( AlignRight | AlignVCenter );
    grid->addWidget( xtitleLA, 3, 0 );
    _xtitleCB = new KColorButton( gb );
    grid->addWidget( _xtitleCB, 3, 1 );

    QLabel* ytitleLA = new QLabel( i18n("Y title color" ), gb );
    ytitleLA->setAlignment( AlignRight | AlignVCenter );
    grid->addWidget( ytitleLA, 3, 2 );
    _ytitleCB = new KColorButton( gb );
    grid->addWidget( _ytitleCB, 3, 3 );

    QLabel* ytitle2LA = new QLabel( i18n( "Y title color (2nd axis)" ), gb );
    ytitle2LA->setAlignment( AlignRight | AlignVCenter );
    grid->addWidget( ytitle2LA, 4, 0 );
    _ytitle2CB = new KColorButton( gb );
    grid->addWidget( _ytitle2CB, 4, 1 );

    QLabel* xlabelLA = new QLabel( i18n( "X label color" ), gb );
    xlabelLA->setAlignment( AlignRight | AlignVCenter );
    grid->addWidget( xlabelLA, 4, 2 );
    _xlabelCB = new KColorButton( gb );
    grid->addWidget( _xlabelCB, 4, 3 );

    QLabel* ylabelLA = new QLabel( i18n( "Y label color" ), gb );
    ylabelLA->setAlignment( AlignRight | AlignVCenter );
    grid->addWidget( ylabelLA, 5, 0 );
    _ylabelCB = new KColorButton( gb );
    grid->addWidget( _ylabelCB, 5, 1 );

    QLabel* ylabel2LA = new QLabel( i18n( "Y label color (2nd axis)" ), gb );
    ylabel2LA->setAlignment( AlignRight | AlignVCenter );
    grid->addWidget( ylabel2LA, 5, 2 );
    _ylabel2CB = new KColorButton( gb );
    grid->addWidget( _ylabel2CB, 5, 3 );

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
