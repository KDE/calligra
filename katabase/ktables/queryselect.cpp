/***************************************************************************
                          queryselect.cpp  -  description                              
                             -------------------                                         
    begin                : Sun Jul 11 1999                                           
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

#include <qpopupmenu.h>
#include <qstringlist.h>

#include <klocale.h>

#include <kdb.h>
#include <kdbdataset.h>
#include <kdbfieldset.h>
#include <kdbdatafield.h>

#include "queryselect.h"
#include "propertyeditor.h"
#include "propertyitem.h"

#include "queryselect.moc"

#define ADD_TABLE            1010
#define ADD_FIELD            1020
#define ADD_CRITERIA         1030
#define ADD_JOIN             1040
#define REMOVE_ITEM          2010
#define PROPERTY_EDIT        3010

QuerySelect::QuerySelect(QWidget *p_par, const char *p_nam)
 : QListView(p_par,p_nam)
{
	resize(220,350);
	setRootIsDecorated( true );
	connect( this,SIGNAL(rightButtonClicked(QListViewItem*,const QPoint&,int)),SLOT(popupMenu(QListViewItem *,const QPoint&,int)));
	
	addColumn( i18n("Name") );
	addColumn( i18n("Type") );
}

QuerySelect::~QuerySelect()
{
}

static QListViewItem *
searchName(QListViewItem *p_item, const QString& p_name)
{
	QListViewItem *child;
	
	if ( p_item->text(1) != i18n("table") )
		return 0;
	if ( p_item->text(0) == p_name )
		return p_item;
	for( child=p_item->firstChild();child;child=child->nextSibling() )
		if ( p_item = searchName(child,p_name) )
			return p_item;
	return 0;
}

void
QuerySelect::addTable(const QString& p_table)
{
	kdbDataSet *set;
	QListViewItem *itm;
	uint i;
	
	set = Kdb::dataSet( p_table );
	if ( !set )
		return;
	for( itm=firstChild();itm;itm=itm->nextSibling() )
		if ( searchName(itm,p_table) )
			return;
	itm = new PropertyItem( this,p_table,i18n("table") );
	for( i=0;i<set->fieldSet()->count();i++ )
		(void)new PropertyItem( (PropertyItem *)itm,set->field(i).name(),i18n("field") );
}

void
QuerySelect::removeTable(const QString& p_table)
{
	QListViewItem *itm,*child = 0;
	
	for( itm=firstChild();itm;itm=itm->nextSibling() )
		if ( child = searchName(itm,p_table) )
			break;
	if ( child ) {
		delete child;
		repaint();
	}
}

void
QuerySelect::popupMenu(QListViewItem *p_itm, const QPoint& p_pos, int p_col)
{
	QPopupMenu *menu;
	QString name,type;
	
	type = QString::null;
	menu = new QPopupMenu( this,"items" );
	
	if ( !p_itm ) {
		menu->insertItem( i18n("Add table"),ADD_TABLE );
	} else if ( p_itm->text(1) == i18n("table") ) {
		menu->insertItem( i18n("Add field"),ADD_FIELD );
		menu->insertItem( i18n("Join table"),ADD_JOIN );
	} else if ( p_itm->text(1) == i18n("field") ) {
		menu->insertItem( i18n("Add criteria"),ADD_CRITERIA );
	} else if ( p_itm->text(1) == i18n("criteria") ) {
	}
	
	if ( p_itm ) {
		menu->insertItem( i18n("Remove item"),REMOVE_ITEM );
		menu->insertSeparator();
		menu->insertItem( i18n("Properties..."),PROPERTY_EDIT );
	}
	switch( menu->exec(p_pos) ) {
		case ADD_TABLE:
		case ADD_JOIN:
			name = i18n("<unknown>");
			type = i18n("table");
			break;
		case ADD_FIELD:
			name = i18n("<unknown>");
			type = i18n("field");
			break;
		case ADD_CRITERIA:
			name = i18n("<blank>");
			type = i18n("criteria");
			break;
		case REMOVE_ITEM:
			delete p_itm;
			repaint();
			break;
		case PROPERTY_EDIT:
			if ( p_itm->text(-1) == "PropertyItem" )
				((PropertyItem *)p_itm)->edit();
		default:
			break;
	}
	if ( type != QString::null ) {
		if ( p_itm )
			(void)new PropertyItem( (PropertyItem *)p_itm,name,type );
		else
			(void)new PropertyItem( this,name,type );
	}
	delete menu;
}

