/*
 * $Id$
 *
 * Copyright 2000 by Laurent Montel, released under Artistic License.
 */

#include "kchartPieConfigPage.h"
#include "kchartPieConfigPage.moc"

#include <kapp.h>
#include <klocale.h>
#include <qlayout.h>
#include <qlabel.h>
#include <kfontdialog.h>

#include <qcheckbox.h>
#include <qlineedit.h>
#include <qradiobutton.h>
#include <qpushbutton.h>
#include <qfont.h>
#include <qspinbox.h>

#include "kdchart/KDChartParams.h"

KChartPieConfigPage::KChartPieConfigPage( KDChartParams* params,
                                          QWidget* parent ) :
    QWidget( parent ),_params( params )
{
    //    col=_params->colPie;
    pos=-1;
    QGridLayout *grid = new QGridLayout(this,8,4,15,7);
    list = new QListView( this );
    list->resize( list->sizeHint() );
    grid->addMultiCellWidget(list,0,7,0,0);
    list->addColumn( i18n("Hide piece") );
    list->setRootIsDecorated( TRUE );

    QLabel* label = new QLabel( i18n( "Column active" ), this );
    label->resize( label->sizeHint() );
    label->setAlignment(Qt::AlignCenter);
    grid->addWidget( label,0,1);

    // PENDING(kalle) Put back in
    column = new QSpinBox(1,/*params->xlbl.count()*/0, 1, this);
    column->resize(100, column->sizeHint().height() );
    grid->addWidget( column,1,1);

    column->setValue(col+1);

    label = new QLabel( i18n( "Move piece to" ), this );
    label->resize( label->sizeHint() );
    label->setAlignment(Qt::AlignCenter);
    grid->addWidget( label,2,1);

    dist = new QSpinBox(0, 400, 1, this);
    dist->resize(100, dist->sizeHint().height() );
    grid->addWidget( dist,3,1);

    label = new QLabel( i18n( "Start" ), this );
    label->resize( label->sizeHint() );
    label->setAlignment(Qt::AlignCenter);
    grid->addWidget( label,4,1);

    angle = new QSpinBox(0, 90, 1, this);
    angle->resize(100, angle->sizeHint().height() );
    grid->addWidget( angle,5,1);

    label = new QLabel( i18n( "3D-Depth" ), this );
    label->resize( label->sizeHint() );
    label->setAlignment(Qt::AlignCenter);
    grid->addWidget( label,6,1);

    depth = new QSpinBox(0, 10, 1, this);
    depth->resize(100, depth->sizeHint().height() );
    grid->addWidget( depth,7,1);


    grid->addColSpacing(0,list->width());
    grid->addColSpacing(2,list->width());
    grid->addColSpacing(3,list->width());

    if(!_params->threeDPies()) {
        depth->setEnabled(false);
    }

    initList();
    dist->setEnabled(false);

    connect(column,SIGNAL(valueChanged(int)),this,SLOT(changeValue(int)));

    connect( list, SIGNAL( selectionChanged(QListViewItem *) ), this, SLOT( slotselected(QListViewItem *) ) );
}


void KChartPieConfigPage::initList()
{
    int index;
    // PENDING(kalle) Put back in
    //     for( QStringList::Iterator it = _params->legend.begin(); it != _params->legend.end(); ++it ) {
//         (void)new QCheckListItem( list, (*it),QCheckListItem::CheckBox ) ;

//     }
    QListViewItemIterator it( list );
    //Select or not CheckBox
    // PENDING(kalle) Put back in
    //     for( ; it.current(); ++it )	{
//         index = _params->legend.findIndex(((QCheckListItem*)it.current())->text());
//         ((QCheckListItem*)it.current())->setOn(_params->missing[_params->legend.count()*col+indice]) ;
//     }
//     value.duplicate(_params->explode);
}


void KChartPieConfigPage::slotselected(QListViewItem *it)
{
    //column : 0
    //cout <<"Select :"<<(it)->text(0).ascii()<<endl;
    // PENDING(kalle) Put back in
    //     int index = _params->legend.findIndex((it)->text(0));
    if(pos==-1)
	dist->setEnabled(true);
    else
	value[pos]=dist->value();

    // PENDING(kalle) Put back in
    //     pos=_params->legend.count()*col+index;
    dist->setValue(value[pos]);
}


void KChartPieConfigPage::changeValue(int val)
{
    col=val-1;
    QListViewItemIterator it( list );
    //Select or not CheckBox
    ;
    int index=0;
    for( ; it.current(); ++it )	{
        // PENDING(kalle) Put back in
        //         index = _params->legend.findIndex(((QCheckListItem*)it.current())->text());
//         ((QCheckListItem*)it.current())->setOn(_params->missing[_params->legend.count()*col+indice]) ;
    }

    if(pos!=-1) {
        value[pos]=dist->value();
        // PENDING(kalle) Put back in
        //         pos=_params->legend.count()*col+indice;
        dist->setValue(value[pos]);
    }
}


void KChartPieConfigPage::init()
{
    QListViewItemIterator it( list );
    //Select or not CheckBox

    int index = 0;
    for( ; it.current(); ++it )	{
        // PENDING(kalle) Put back in
        //        index = _params->legend.findIndex(((QCheckListItem*)it.current())->text());
        //         ((QCheckListItem*)it.current())->setOn(_params->missing[_params->legend.count()*col+indice]) ;
    }

    if( _params->threeDPies() )	{
        depth->setValue( _params->threeDPieHeight() );
    }

    angle->setValue( _params->pieStart() );

    // PENDING(kalle) Put back in
//     value.duplicate(_params->explode);
//     if(pos!=-1) {
//             pos=_params->legend.count()*col;
//             dist->setValue(	value[pos]);
// 	}
}


void KChartPieConfigPage::apply()
{
    QListViewItemIterator it( list );
    //Select or not CheckBox
    int index = 0;
    for( ; it.current(); ++it )	{
        // PENDING(kalle) Put back in
        //         index = _params->legend.findIndex(((QCheckListItem*)it.current())->text());
//         _params->missing[_params->legend.count()*col+indice]=((QCheckListItem*)it.current())->isOn() ;
    }
    //     _params->colPie=col;

    if( _params->threeDPies() )	{
        _params->setThreeDPieHeight( depth->value() );
    }
    
    _params->setPieStart( angle->value() );

    // PENDING(kalle) Put back in
    //     value[pos]=dist->value();
//     _params->explode.duplicate(value);
}
