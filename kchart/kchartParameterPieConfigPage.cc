/*
 * $Id$
 *
 * Copyright 2000 by Laurent Montel, released under Artistic License.
 */

#include "kchartParameterPieConfigPage.h"

#include "kchartParameterPieConfigPage.moc"

#include <kapp.h>
#include <klocale.h>
#include <kdebug.h>

#include <qlayout.h>
#include <qlabel.h>
#include <qbuttongroup.h>
#include <qcheckbox.h>
#include <qlineedit.h>
#include <qradiobutton.h>
#include <qspinbox.h>

#include "kdchart/KDChartParams.h"

KChartParameterPieConfigPage::KChartParameterPieConfigPage( KDChartParams* params,
                                                            QWidget* parent ) :
    QWidget( parent ),_params( params )
{
    qDebug( "Sorry, not implemented: KChartParameterPieConfigPage::KChartParameterPieConfigPage()" );
#warning Put back in
#ifdef K
    QGridLayout *grid1 = new QGridLayout(this,2,2,15,7);

    QButtonGroup* gb = new QButtonGroup( i18n( "Label's position" ), this );

    QGridLayout *grid2 = new QGridLayout(gb,6,1,15,7);

    _typeNone = new QRadioButton( i18n( "None" ), gb );
    _typeNone->resize( _typeNone->sizeHint() );
    grid2->addWidget( _typeNone,1,0);

    _typeAbove = new QRadioButton( i18n( "Above" ), gb );
    _typeAbove->resize( _typeAbove->sizeHint() );
    grid2->addWidget( _typeAbove,2,0 );

     _typeBelow = new QRadioButton( i18n( "Below" ), gb );
     _typeBelow->resize(_typeBelow->sizeHint() );
    grid2->addWidget( _typeBelow,3,0 );

     _typeRight = new QRadioButton( i18n( "Right" ), gb );
     _typeRight->resize(_typeRight->sizeHint() );
    grid2->addWidget( _typeRight,4,0 );

     _typeLeft = new QRadioButton( i18n( "Left" ), gb );
     _typeLeft->resize(_typeLeft->sizeHint() );
    grid2->addWidget( _typeLeft,5,0 );

    grid2->addRowSpacing(0,7);
    grid2->addRowSpacing(1,_typeNone->height());
    grid2->addRowSpacing(2,_typeAbove->height());
    grid2->addRowSpacing(3,_typeBelow->height());
    grid2->addRowSpacing(4,_typeRight->height());
    grid2->addRowSpacing(5,_typeLeft->height());


    grid2->setRowStretch(0,0);
    grid2->setRowStretch(1,0);
    grid2->setRowStretch(2,0);
    grid2->setRowStretch(3,0);
    grid2->setRowStretch(4,1);
    grid2->setRowStretch(5,1);
    grid2->addColSpacing(0,_typeNone->width());
    grid2->addColSpacing(0,_typeAbove->width());
    grid2->addColSpacing(0,_typeBelow->width());
    grid2->addColSpacing(0,_typeRight->width());
    grid2->addColSpacing(0,_typeLeft->width());


    grid2->setColStretch(0,1);

    grid2->activate();
    grid1->addWidget(gb,0,0);


    grid1->addRowSpacing(0,gb->height());

    grid1->setRowStretch(0,0);
    grid1->setRowStretch(1,1);

    grid1->addColSpacing(0,gb->width());
    grid1->setColStretch(0,1);
    grid1->setColStretch(1,1);


    QGroupBox* gb2 = new QGroupBox( i18n( "Title" ), this );
    QGridLayout *grid3 = new QGridLayout(gb2,4,2,15,7);

    lineLabel=new QCheckBox(i18n("Line label"),gb2);
    lineLabel->resize( lineLabel->sizeHint() );
    grid3->addWidget( lineLabel,1,0 );

    QLabel* label2 = new QLabel( i18n( "Length of line" ), gb2 );
    label2->resize( label2->sizeHint() );
    label2->setAlignment(Qt::AlignCenter);
    grid3->addWidget( label2,2,0 );


    dist = new QSpinBox(1, 400, 1, gb2);
    dist->resize(100, dist->sizeHint().height() );
    grid3->addWidget( dist,3,0 );
    label2->setBuddy( dist );


    QLabel* label = new QLabel( i18n( "Title" ), gb2 );
    label->resize( label->sizeHint() );
    label->setAlignment(Qt::AlignCenter);
    grid3->addWidget( label,4,0 );

    title = new QLineEdit( gb2 );
    title->resize(100, title->sizeHint().height() );
    grid3->addWidget( title,5,0 );
    label->setBuddy( title );

    grid3->addRowSpacing(0,7);
    grid3->addRowSpacing(1,lineLabel->height());
    grid3->addRowSpacing(2,label2->height());
    grid3->addRowSpacing(3,dist->height());
    grid3->addRowSpacing(4,label->height());
    grid3->addRowSpacing(5,title->height());
    grid3->setRowStretch(0,0);
    grid3->setRowStretch(1,0);
    grid3->setRowStretch(2,1);

    grid3->addColSpacing(0,lineLabel->width());
    grid3->addColSpacing(1,label2->width());
    //grid3->addColSpacing(2,dist->width());
    //grid3->addColSpacing(3,label->width());
    //grid3->addColSpacing(4,title->width());
    grid3->setColStretch(0,0);
    grid3->setColStretch(1,1);
    grid3->setColStretch(2,1);
    grid3->activate();
    grid1->addWidget(gb2,0,1);


    grid1->addRowSpacing(0,gb2->height());
    grid1->setRowStretch(0,0);
    grid1->setRowStretch(1,1);
    grid1->addColSpacing(1,gb2->width());
    grid1->activate();

    if(_params->label_line)
    	dist->setEnabled(true);
    else
    	dist->setEnabled(false);
    connect( lineLabel, SIGNAL( toggled( bool ) ),
  		   this, SLOT( changeState( bool ) ) );
#endif
}

void KChartParameterPieConfigPage::changeState(bool state)
{
if(state)
   dist->setEnabled(true);
else
   dist->setEnabled(false);

}


void KChartParameterPieConfigPage::init()
{
    qDebug( "Sorry, not implemented: KChartParameterPieConfigPage::init()" );
#warning Put back in
#ifdef K
    title->setText(_params->title);
    lineLabel->setChecked(_params->label_line);
    dist->setValue(_params->label_dist);
    switch(_params->percent_labels)
    	{
    	case KCHARTPCTTYPE_ABOVE:
    		_typeAbove->setChecked(true);
    		break;
	case KCHARTPCTTYPE_NONE:
    		_typeNone->setChecked(true);
    		break;
	case KCHARTPCTTYPE_RIGHT:
    		_typeRight->setChecked(true);
    		break;
	case KCHARTPCTTYPE_LEFT:
    		_typeLeft->setChecked(true);
    		break;
	case KCHARTPCTTYPE_BELOW:
    		_typeBelow->setChecked(true);
    		break;
    	default:
                                   kdDebug(35001)<<" Error in _params->percent_labels\n";
    		break;
    	}
#endif
}


void KChartParameterPieConfigPage::apply()
{
    qDebug( "Sorry, not implemented: KChartParameterPieConfigPage::apply()" );
#warning Put back in
#ifdef K
    _params->title=title->text();
    _params->label_line=lineLabel->isChecked();
    if(lineLabel->isChecked())
    	_params->label_dist=dist->value();
    if(_typeAbove->isChecked())
    	_params->percent_labels=KCHARTPCTTYPE_ABOVE;
    else if(_typeNone->isChecked())
    	_params->percent_labels=KCHARTPCTTYPE_NONE;
    else if(_typeRight->isChecked())
    	_params->percent_labels=KCHARTPCTTYPE_RIGHT;
    else if(_typeLeft->isChecked())
    	_params->percent_labels=KCHARTPCTTYPE_LEFT;
    else if(_typeBelow->isChecked())
    	_params->percent_labels=KCHARTPCTTYPE_BELOW;
    else
    	kdDebug(35001)<<"Error in QRadioButton\n";
#endif
}
