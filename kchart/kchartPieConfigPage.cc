/* This file is part of the KDE project
   Copyright (C) 2000,2001,2002,2003,2004 Laurent Montel <montel@kde.org>

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

#include "kchartPieConfigPage.h"
#include "kchartPieConfigPage.moc"

#include <kapplication.h>
#include <klocale.h>
#include <qlayout.h>
#include <qlabel.h>
//Added by qt3to4:
#include <Q3GridLayout>
#include <kfontdialog.h>

#include <qcheckbox.h>
#include <qlineedit.h>
#include <qradiobutton.h>
#include <qpushbutton.h>
#include <qfont.h>
#include <qspinbox.h>

#include "kchart_params.h"


namespace KChart
{

KChartPieConfigPage::KChartPieConfigPage( KChartParams* params,
                                          QWidget* parent,
                                          KDChartTableData* data) :
    QWidget( parent ),_params( params )
{
    //    col=_params->colPie;
    pos=-1;
    Q3GridLayout *grid = new Q3GridLayout(this,10,4,KDialog::marginHint(), 
					KDialog::spacingHint());
    // The listview
    list = new Q3ListView( this );
    list->resize( list->sizeHint() );
    grid->addMultiCellWidget(list,0,9,0,0);
    list->addColumn( i18n("Hide Piece") );
    list->setRootIsDecorated( TRUE );

    QLabel* label = new QLabel( i18n( "Column active:" ), this );
    label->resize( label->sizeHint() );
    grid->addWidget( label,0,1);

    column = new QSpinBox(1, data->cols(), 1, this);
    column->resize(100, column->sizeHint().height() );
    grid->addWidget( column,1,1);

    column->setValue(col+1);

    label = new QLabel( i18n( "Move piece to:" ), this );
    label->resize( label->sizeHint() );
    grid->addWidget( label,2,1);

    dist = new QSpinBox(0, 400, 1, this);
    dist->resize(100, dist->sizeHint().height() );
    grid->addWidget( dist,3,1);

    label = new QLabel( i18n( "Explode factor (%):" ), this );
    label->resize( label->sizeHint() );
    grid->addWidget( label,4,1);

    explose = new QSpinBox(0, 100, 1, this);
    explose->resize(100, explose->sizeHint().height() );
    grid->addWidget( explose,5,1);

    grid->addItem( new QSpacerItem(list->width(), 0 ), 0, 0 );
    grid->addItem( new QSpacerItem(list->width(), 0 ), 0, 2 );
    grid->addItem( new QSpacerItem(list->width(), 0 ), 0, 3 );


    initList();
    dist->setEnabled(false);

    connect(column,SIGNAL(valueChanged(int)),this,SLOT(changeValue(int)));

    connect( list, SIGNAL( selectionChanged(Q3ListViewItem *) ), this, SLOT( slotselected(Q3ListViewItem *) ) );
}


void KChartPieConfigPage::initList()
{
    //int index;
    // PENDING(kalle) Put back in
    //     for( QStringList::Iterator it = _params->legend.begin(); it != _params->legend.end(); ++it ) {
//         (void)new QCheckListItem( list, (*it),QCheckListItem::CheckBox ) ;

//     }
    Q3ListViewItemIterator it( list );
    //Select or not CheckBox
    // PENDING(kalle) Put back in
    //     for( ; it.current(); ++it )	{
//         index = _params->legend.findIndex(((QCheckListItem*)it.current())->text());
//         ((QCheckListItem*)it.current())->setOn(_params->missing[_params->legend.count()*col+index]) ;
//     }
//     value.duplicate(_params->explode);
}


void KChartPieConfigPage::slotselected(Q3ListViewItem */*it*/)
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
    Q3ListViewItemIterator it( list );
    //Select or not CheckBox
    ;
    //int index=0;
    for( ; it.current(); ++it )	{
        // PENDING(kalle) Put back in
        //         index = _params->legend.findIndex(((QCheckListItem*)it.current())->text());
//         ((QCheckListItem*)it.current())->setOn(_params->missing[_params->legend.count()*col+index]) ;
    }

    if(pos!=-1) {
        value[pos]=dist->value();
        // PENDING(kalle) Put back in
        //         pos=_params->legend.count()*col+index;
        dist->setValue(value[pos]);
    }
}


void KChartPieConfigPage::init()
{
    Q3ListViewItemIterator it( list );
    //Select or not CheckBox

    //int index = 0;
    for( ; it.current(); ++it )	{
        // PENDING(kalle) Put back in
        //        index = _params->legend.findIndex(((QCheckListItem*)it.current())->text());
        //         ((QCheckListItem*)it.current())->setOn(_params->missing[_params->legend.count()*col+index]) ;
    }

    // PENDING(kalle) Put back in
//     value.duplicate(_params->explode);
//     if(pos!=-1) {
//             pos=_params->legend.count()*col;
//             dist->setValue(	value[pos]);
// 	}
    explose->setValue((int)(_params->explodeFactor()*100));
}


void KChartPieConfigPage::apply()
{
    Q3ListViewItemIterator it( list );
    //Select or not CheckBox
    //int index = 0;
    for( ; it.current(); ++it )	{
        // PENDING(kalle) Put back in
        //         index = _params->legend.findIndex(((QCheckListItem*)it.current())->text());
//         _params->missing[_params->legend.count()*col+index]=((QCheckListItem*)it.current())->isOn() ;
    }
    //     _params->colPie=col;

    // PENDING(kalle) Put back in
    //     value[pos]=dist->value();
//     _params->explode.duplicate(value);
    _params->setExplodeFactor(((double)(explose->value()))/100);
}

}  //KChart namespace
