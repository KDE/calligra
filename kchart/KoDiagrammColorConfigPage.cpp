/*
 * $Id$
 *
 * Copyright 1999 by Kalle Dalheimer, released under Artistic License.
 */

#include "KoDiagrammColorConfigPage.h"

#include "KoDiagrammColorConfigPage.moc"

#include <kapp.h>
#include <klocale.h>
#include <kcolorbtn.h>

#include <qlayout.h>
#include <qlabel.h>

KoDiagrammColorConfigPage::KoDiagrammColorConfigPage( QWidget* parent ) :
	QWidget( parent )
{
	QGridLayout* grid = new QGridLayout( this, 7, 4, 15,7);

	QLabel* accentLA = new QLabel( i18n( "Accentuation color" ), this );
	accentLA->resize( accentLA->sizeHint() );
	accentLA->setAlignment(AlignRight | AlignVCenter);
	grid->addWidget( accentLA, 0, 0 );
	grid->addColSpacing(0,accentLA->width());

	_accentCB = new KColorButton( this );
	_accentCB->resize( _accentCB->sizeHint() );
	grid->addWidget( _accentCB, 0, 1 );

	
	QLabel* axislabelLA = new QLabel( i18n( "Axis label color" ), this );
	axislabelLA->resize( axislabelLA->sizeHint() );
	axislabelLA->setAlignment(AlignRight | AlignVCenter);
	grid->addWidget( axislabelLA, 1, 0 );
	grid->addColSpacing(0,axislabelLA->width());

	_axislabelCB = new KColorButton( this );
	_axislabelCB->resize( _axislabelCB->sizeHint() );
	grid->addWidget( _axislabelCB, 1, 1 );


	QLabel* backgroundLA = new QLabel( i18n( "Background color" ), this );
	backgroundLA->resize( backgroundLA->sizeHint() );
	backgroundLA->setAlignment(AlignRight | AlignVCenter);
	grid->addWidget( backgroundLA, 2, 0 );
	grid->addColSpacing(0,backgroundLA->width());

	_backgroundCB = new KColorButton( this );
	_backgroundCB->resize( _backgroundCB->sizeHint() );
	grid->addWidget( _backgroundCB, 2, 1 );


	QLabel* foregroundLA = new QLabel( i18n( "Foreground color" ), this );
	foregroundLA->resize( foregroundLA->sizeHint() );
	foregroundLA->setAlignment(AlignRight | AlignVCenter);
	grid->addWidget( foregroundLA, 3, 0 );
	grid->addColSpacing(0,foregroundLA->width());
	
	_foregroundCB = new KColorButton( this );
	_foregroundCB->resize( _foregroundCB->sizeHint() );
	grid->addWidget( _foregroundCB, 3, 1 );
	

	QLabel* labelLA = new QLabel( i18n( "Label color" ), this );
	labelLA->resize( labelLA->sizeHint() );
	labelLA->setAlignment(AlignRight | AlignVCenter);
	grid->addWidget( labelLA, 4, 0 );
	grid->addColSpacing(0,labelLA->width());
	
	_labelCB = new KColorButton( this );
	_labelCB->resize( _labelCB->sizeHint() );
	grid->addWidget( _labelCB, 4, 1 );
	

	QLabel* textLA = new QLabel( i18n( "Text color" ), this );
	textLA->resize( textLA->sizeHint() );
	textLA->setAlignment(AlignRight | AlignVCenter);
	grid->addWidget( textLA, 5, 0 );
	grid->addColSpacing(0,textLA->width());
	
	_textCB = new KColorButton( this );
	_textCB->resize( _textCB->sizeHint() );
	grid->addWidget( _textCB, 5, 1 );

	for( int i = 0; i < NUMDATACOLORS; i++ ) {
		QString labeltext;
		labeltext.sprintf( i18n( "Data color #%d"), i );
		QLabel* dataLA = new QLabel( labeltext, this );
		dataLA->setAlignment(AlignRight | AlignVCenter);
		dataLA->resize( dataLA->sizeHint() );
		grid->addWidget( dataLA, i, 2 );
		_dataCB[i] = new KColorButton( this );
		_dataCB[i]->resize( _dataCB[i]->sizeHint() );
		grid->addWidget( _dataCB[i], i, 3 );
		grid->addRowSpacing(i,_textCB->height());
		grid->setRowStretch(i,0);
		grid->addColSpacing(2,dataLA->width() + 20);
	}
		
	grid->setRowStretch(6,1);

	grid->setColStretch(0,0);
	grid->setColStretch(1,1);
	grid->setColStretch(0,0);
	grid->setColStretch(3,1);

	grid->activate();
}
