/***************************************************************************
                          querydialog.cpp  -  description                              
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

#include <qpushbutton.h>
#include <qlayout.h>

#include <klocale.h>

#include <kdb.h>
#include <kdbdataset.h>
#include <kdbfieldset.h>
#include <kdbdatafield.h>
#include <kdbcriteria.h>

#include "propertyitem.h"

#include "querydialog.h"
#include "queryselect.h"
#include "querydialog.moc"

QueryDialog::QueryDialog(QWidget *p_par, const char *p_nam)
 : QDialog(p_par,p_nam)
{
	QVBoxLayout *vbox;
	QHBoxLayout *hbox;
	
	vbox = new QVBoxLayout( this );
	vbox->setMargin( 15 );
	
	_tree = new QuerySelect( this,"query_tree" );
	vbox->addWidget( _tree );
	vbox->addSpacing( 10 );
	
	hbox = new QHBoxLayout( this );
	hbox->setMargin( 15 );
	
	vbox->addLayout( hbox );
	
	QPushButton *but = new QPushButton( this,"ok_button" );
	but->setText( i18n("OK") );
	connect( but,SIGNAL(clicked()),SLOT(ok_pushed()) );
	hbox->addWidget( but );
	hbox->addSpacing( 10 );
	
	but = new QPushButton( this,"cancel_button" );
	but->setText( i18n("Cancel") );
	connect( but,SIGNAL(clicked()),SLOT(cancel_pushed()) );
	hbox->addWidget( but );
}

QueryDialog::~QueryDialog()
{
}

static void
treatCriteria(kdbDataSet *p_set, const QString& p_name, PropertyItem *p_item)
{
	kdbCriteria *crit;
	
	if ( !p_item )
		return;
	crit = new kdbCriteria( p_set,p_name );
	(*crit) = Kdb::str2operator( p_item->getProperty(i18n("Operator")) );
	(*crit) = Kdb::str2condition( p_item->getProperty(i18n("Condition")) );
	(*crit) = p_item->getProperty( i18n("Value") );
}

static void
treatDataSet(PropertyItem *p_item)
{
	kdbDataSet *dset;
	QStringList field_list;
	PropertyItem *ch_item;
	
	if ( p_item->text(0) == i18n("<unknown>") )
		return;
	dset = Kdb::dataSet( p_item->text(0) );
	for ( ch_item=(PropertyItem*)p_item->firstChild();ch_item;ch_item=(PropertyItem*)ch_item->nextSibling() )
		if ( ch_item->text(1) == i18n("field") ) {
			field_list += ch_item->text(0);
			treatCriteria( dset,ch_item->text(0),(PropertyItem*)ch_item->firstChild() );
		}
	for ( int i=0;i<dset->fieldSet()->count();i++ ) {
		if ( field_list.find(dset->field(i).name()) == field_list.end() ) {
			dset->fieldSet()->removeField( dset->field(i).name() );
			i -= 1;
		}
	}
}

void
QueryDialog::buildQuery()
{
	QListViewItem *item;
	
	for ( item=_tree->firstChild();item;item=item->nextSibling() )
		treatDataSet( (PropertyItem*)item );
}

void
QueryDialog::ok_pushed()
{
	hide();
}

void
QueryDialog::cancel_pushed()
{
	hide();
}









