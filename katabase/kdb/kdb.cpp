/***************************************************************************
                          kdb.cpp  -  description                              
                             -------------------                                         
    begin                : Fri Jun 25 1999                                           
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

#include <qobjectlist.h>

#include <klocale.h>

#include <kdbpgbase.h>
#include <kdbdataset.h>
#include <kdbrelation.h>
#include <kdbdatafield.h>
#include <kdbfieldset.h>

#include "kdb.h"

QString      Kdb::_dateFormat     = "%m/%d/%/";
Kdb::Bases   Kdb::_connectionType = None;
kdbDataBase *Kdb::_dataBase       = 0;

Kdb::Kdb()
{
	_dataBase       = 0;
	_connectionType = None;
	_dateFormat     = "%m/%d/%Y";
}

Kdb::~Kdb()
{
}

QString
Kdb::exceptionMsg(ExceptionTypes t)
{
	switch( t ) {
		case NoHost:
			return i18n("No connection to host");
		case NoBase:
			return i18n("No database open");
		case NoField:
			return i18n("Field index out of bounds");
		case UnknownField:
			return i18n("Unknown field name");
		case NoRecord:
			return i18n("Record index out of bounds");
		case NotImplemented:
			return i18n("The feature isn't implemented yet");
		default:
			return i18n("unknwon exception type");
	}
}

bool
Kdb::isOpen()
{
	return ( _connectionType != None);
}

void
Kdb::Open(Bases p_baseType, const QString& p_base, const QString& p_host)
{
	kdbDataSet *dset;
	kdbCriteria *crit;
	
	switch( p_baseType ) {
		case Postgres:
			_dataBase       = new kdbPgBase( p_base,p_host );
			_connectionType = p_baseType;
			dset = new kdbDataSet( _dataBase,"pg_class" );
			dset->fieldSet()->addField( "relname" );
			crit = new kdbCriteria( dset,"relkind" );
			(*crit) = "r";
			(*crit) = Equal;
			crit = new kdbCriteria( dset,"relname" );
			(*crit) = Like;
			(*crit) = NAnd;
			(*crit) = "pg_%";
			dset->setVisualProgress( false );
			dset->runQuery( false );
			dset->setName( "$tables" );
			dset->field( "relname" ).setName( "$name" );
			break;
		default:
			throw NotImplemented;
	}
}

void
Kdb::Close()
{
	if ( _dataBase )
		delete _dataBase;
	_dataBase       = 0;
	_connectionType = None;
}

kdbDataBase *
Kdb::dataBase()
{
	if ( _connectionType == None )
		throw NoBase;
	return _dataBase;
}

bool
Kdb::hasDataSet(const QString& p_set)
{
	QObjectList *list;
	QObject     *obj;
		
	if ( _connectionType == None )
		throw NoBase;
	list  = _dataBase->queryList( "kdbDataSet",p_set,true,false );
	QObjectListIt it( *list );
	obj = it.current();
	delete list;
	return (obj != 0);
}

kdbDataSet *
Kdb::dataSet(const QString& p_set)
{
	QObjectList *list;
	QObject     *obj;
	
	if ( _connectionType == None )
		throw NoBase;
	list = _dataBase->queryList( "kdbDataSet",p_set,true,false );
	QObjectListIt it( *list );
	obj = it.current();
	delete list;
	if ( !obj ) {
		obj = new kdbDataSet( _dataBase,p_set );
		_dataBase->getFields( (kdbDataSet *)obj );
	}
	return (kdbDataSet *)obj;
}

kdbRelation *
Kdb::relation(kdbDataSet *p_set, const QString& p_rel)
{
	QObjectList *list;
	QObject     *obj;

	if ( p_set == 0 || p_rel == QString::null )
		return 0;
	obj = p_set->child( p_rel,"kdbRelation" );
	list = p_set->queryList( "kdbRelation",p_rel );
	QObjectListIt it( *list );
	for( ;(obj = it.current());++it )
		break;
	delete list;
	return (kdbRelation *)obj;
}

Kdb::Operator
Kdb::str2operator(const QString& p_str)
{
	if ( p_str.find("AND",0,false) != -1 )
		return And;
	else if ( p_str.find("OR",0,false) != -1 )
		return Or;
	else if ( p_str.find("NAND",0,false) != -1 )
		return NAnd;
	else
		return NOr;
}

Kdb::RelationType
Kdb::str2condition(const QString& p_str)
{
	if ( p_str.find("Equal",0,false) != -1 )
		return Equal;
	else if ( p_str.find("Less",0,false) != -1 )
		return Less;
	else if ( p_str.find("Greater",0,false) != -1 )
		return Greater;
	else
		return Like;
}





































