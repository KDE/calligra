/***************************************************************************
                          kdbrecordview.cpp  -  description                              
                             -------------------                                         
    begin                : Mon Jun 28 1999                                           
    copyright            : (C) 1999 by Ørn E. Hansen                         
    email                : hanseno@mail.bip.net                                     
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   * 
 *                                                                         *
 ***************************************************************************/

#include <iostream.h>

#include <qpainter.h>
#include <qheader.h>
#include <qapplication.h>

#include <kdbdatabase.h>
#include <kdbfieldset.h>
#include <kdbdataset.h>

#include "kdbrecordview.h"
#include "kdbrecordview.moc"

kdbRecordView::kdbRecordView(kdbDataSet *p_set, QWidget *p_par, const char *p_nam)
 : QListView(p_par, p_nam)
{
	if( p_set ) {
	}
	_columnPos = 0;
	_edit  = new QLineEdit( viewport(),"editor_0" );
	_edit->setFrame( false );
	_edit->hide();
	_dataSet = p_set;
	connect( _edit,SIGNAL(textChanged(const QString&)),SLOT(textChanged(const QString&)));
	connect( this,SIGNAL(selectionChanged(QListViewItem *)),SLOT(recordPicked(QListViewItem*)));
	connect( this,SIGNAL(rightButtonClicked(QListViewItem*,const QPoint&,int)),this,SLOT(buttonMenu(QListViewItem*,const QPoint&,int)) );
	_menu = new QPopupMenu( 0 );
	_menu->insertItem( "&Filter" );
	_menu->insertSeparator();
	_menu->insertItem( "&Remove item from list" );
	_menu->insertItem( "&Insert an empty item" );
	_menu->insertSeparator();
	_menu->insertItem( "&Properties" );
	QListView::setAllColumnsShowFocus( true );

	connect( header(),SIGNAL(sizeChange(int,int,int)),SLOT(sizeChanged(int,int,int)) );	
}

kdbRecordView::~kdbRecordView()
{
}

void
kdbRecordView::setTable(kdbDataSet *p_set)
{
	if ( !p_set )
		return;
	if ( _dataSet )
		disconnect( _dataSet );
	connect( p_set,SIGNAL(initData()),this,SLOT(init()) );
	if ( header()->count() == 0 )
		for ( uint ix=0;ix<p_set->fieldSet()->count();ix++ )
			addColumn( p_set->field(ix).name() );
	_dataSet = p_set;
}

void
kdbRecordView::removeData(bool& p_ok)
{
	QListViewItem *itm;
	
	if ( !p_ok )
		return;
	for( itm=firstChild();itm;itm=itm->nextSibling() )
		if ( ((kdbRecordViewItem *)itm)->record() == _dataSet->record() )
			break;
	if ( itm )
		removeItem( itm );
}

void
kdbRecordView::insertData()
{
	if ( _dataSet ) {
		kdbRecordViewItem *itm = new kdbRecordViewItem( _dataSet->record(),_dataSet,this );
		for( uint i=0;i<header()->count();i++ )
			if ( _dataSet->fieldSet()->has(columnText(i)) )
				itm->setText( i,_dataSet->field(columnText(i)).text() );
		setSelected( itm,true );
	}
}

void
kdbRecordView::insertItem(QListViewItem *p_item)
{
	if ( _dustBin.containsRef(p_item) )
		_dustBin.removeRef( p_item );
	QListView::insertItem( p_item );
}

void
kdbRecordView::takeItem(QListViewItem *p_item)
{
	_dustBin.append( p_item );
	QListView::takeItem( p_item );
}

void
kdbRecordView::removeItem(QListViewItem *p_item)
{
	_dustBin.append( p_item );
	QListView::removeItem( p_item );
}

void
kdbRecordView::clear()
{
	_dustBin.clear();
	QListView::clear();
}

void
kdbRecordView::commitDeletes()
{
}

void
kdbRecordView::commitEdits()
{
	if ( _dataSet->dataBase() ) {
		_dataSet->dataBase()->insert( _dataSet );
		_dataSet->dataBase()->update( _dataSet );
	}
}

void
kdbRecordView::buttonMenu(QListViewItem *p_lvi, const QPoint& p_gco, int p_col)
{
	_menu->popup( p_gco );
}

void
kdbRecordView::init()
{
	const QObject *obj = QObject::sender();
	
	if ( obj && _dataSet == (kdbDataSet *)obj ) {
		clear();
		for( uint rec=0;rec<_dataSet->records();rec++ )
			(void)new kdbRecordViewItem( rec,_dataSet,this );
		connect( _dataSet,SIGNAL(preDelete(bool&)),this,SLOT(removeData(bool&)) );
		connect( _dataSet,SIGNAL(postInsert()),this,SLOT(insertData()) );
	}
}

void
kdbRecordView::setEditItem(QListViewItem *p_item)
{
	int i,pos_x = 0;
	int marg = itemMargin();
	
	if ( !p_item )
		return;
	ensureItemVisible( p_item );
	for( i=0;i<_columnPos;i++ )
		pos_x += columnWidth( i );
	QRect r = itemRect( p_item );
	_edit->move( header()->cellPos(_columnPos),r.y() + marg );
	_edit->resize( header()->cellSize(_columnPos),r.height()-2*marg );
	_edit->setText( p_item->text(_columnPos) );
	_edit->show();
}

void
kdbRecordView::recordPicked(QListViewItem *p_item)
{
	kdbRecordViewItem *ip;
	
	if ( !_dataSet || !p_item )
		return;
	ip = (kdbRecordViewItem *)p_item;
	_dataSet->gotoRec( ip->record() );
	setEditItem( p_item );
}

void
kdbRecordView::focusInEvent(QFocusEvent *p_fev)
{
	QListView::focusInEvent( p_fev );
	setEditItem( currentItem() );	
}

void
kdbRecordView::focusOutEvent(QFocusEvent *p_fev)
{
	QListView::focusOutEvent( p_fev );
}

void
kdbRecordView::contentsMousePressEvent(QMouseEvent *p_me)
{
	uint i;
	int pos=p_me->x();
	
	for( i=0;i<header()->count();i++ ) {
		pos -= columnWidth(i);
		if ( pos < 0 )
			break;
	}
	if ( i<header()->count() ) {
		_columnPos = i;
		if ( itemRect(currentItem()).contains(p_me->pos()) ) {
			repaintItem( currentItem() );
			setEditItem( currentItem() );
		}
	}
	QListView::contentsMousePressEvent( p_me );
}

void
kdbRecordView::sizeChanged(int p_col, int p_oldSize, int p_newSize)
{
	if ( p_col < _columnPos )
		_edit->move( header()->cellPos( _columnPos ),_edit->y() );
	else if ( p_col == _columnPos )
		_edit->resize( p_newSize,_edit->height() );
}

void
kdbRecordView::keyPressEvent(QKeyEvent *p_kev)
{
	switch( p_kev->key() ) {
		case Key_Tab:
			if ( _columnPos < header()->count() )
				_columnPos++;
			setEditItem( currentItem() );
			break;
		case Key_Backtab:
			if ( _columnPos > 0 )
				_columnPos--;
			setEditItem( currentItem() );
			break;
		case Key_Left:
			if ( _columnPos > 0 )
				_columnPos--;
			setEditItem( currentItem() );
			break;
		case Key_Right:
			if ( _columnPos < header()->count() )
				_columnPos++;
			setEditItem( currentItem() );
			break;
		default:
			QListView::keyPressEvent( p_kev );
	}	
}

void
kdbRecordView::textChanged(const QString& p_str)
{
	QListViewItem *item = currentItem();
	
	if ( !item )
		return;
	item->setText( columnPos(),p_str );
}











































