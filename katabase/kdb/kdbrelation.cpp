/***************************************************************************
                          kdbrelation.cpp  -  description                              
                             -------------------                                         
    begin                : Sun Jun 27 1999                                           
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

#include <qnamespace.h>
#include <kdb.h>

#include <kdbdatabase.h>
#include <kdbdataset.h>
#include <kdbfieldset.h>
#include <kdbcriteria.h>
#include "kdbrelation.h"
#include "kdbrelation.moc"

kdbRelation::kdbRelation(kdbDataSet *set,const QString& p_field,const QString& p_refTab,const QString& p_refFld)
 : QObject(&set->field(p_field),p_field)
{
	kdbDataField *fld;
	
	_field     = p_field;
	_kind      = Kdb::Greater;
	_value     = "";
	_refTable  = p_refTab;
	_refField  = p_refFld;
	_listField = p_refFld;
	_listBox   = new QListBox( 0,0,Qt::WType_Popup );
	_set       = 0;
	connect( _listBox,SIGNAL(selected(int)),SLOT(selected(int)) );
	if ( (fld = &set->field(_field)) )
		connect(fld,SIGNAL(validateText(bool&,QString&)),this,SLOT(verifyText(bool&,QString&)));
}

kdbRelation::~kdbRelation()
{
	if (_listBox)
		delete _listBox;
}

void
kdbRelation::runQuery()
{
	QString str;
	uint i, fno;

	if ( !Kdb::isOpen() || _refTable.isEmpty() || _refField.isEmpty() )
		return;
	if ( !_set ) {
		_set = new kdbDataSet( 0,_refTable );
		_set->fieldSet()->addField( _listField );
		if ( _listField != _refField )
			_set->fieldSet()->addField( _refField );
		kdbCriteria crit( _set,_refField );
		crit =  _value;
		crit = _kind;
		Kdb::dataBase()->query( _set );
	}
	_listBox->clear();
	fno = _set->fieldNo( _listField );
	for( i = 0;i<_set->records();i++ )
		_listBox->insertItem( (*_set)[i][fno].text() );
	emit postLoad();
}

QListBox *
kdbRelation::listBox()
{
	return _listBox;
}

void
kdbRelation::verifyText(bool& p_ok, QString& p_txt)
{
	kdbDataField *r_Fld, *l_Fld;	

	if ( _set ) {
		r_Fld = &_set->field( _refField );
		l_Fld = &_set->field( _listField );
		for( uint i=0;i<_set->records();i++ ) {
			(void)(*_set)[i];
			if ( p_txt == l_Fld->text().left(p_txt.length()) )
				p_txt = r_Fld->text();
			if ( p_txt == r_Fld->text() )
				return;
		}
		p_ok = false;
	}		
}

void
kdbRelation::selected(int p_index)
{
	QObject *po = parent();
	QObject *ds;
		
	if ( !po->isA("kdbDataField") )
		return;
	ds = po->parent();
	((kdbDataField *)po)->set( (*_set)[p_index][_refField].text() );
	if ( ds->isA("kdbDataSet") )
		((kdbDataSet *)ds)->update();
}

int
kdbRelation::index(const QString& p_str)
{
	QString str = p_str.stripWhiteSpace();
	uint i;
	
	for( i=0;i<_set->records();i++ ) {
		if ( (*_set)[i][_refField].text().stripWhiteSpace() == str )
			return i;
	}
	return -1;
}






























