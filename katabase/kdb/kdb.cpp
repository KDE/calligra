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

#include <kdbpgbase.h>
#include <kdbdataset.h>
#include <kdbrelation.h>

#include "kdb.h"

QString      Kdb::_dateFormat;
Kdb::Bases   Kdb::_connectionType;
kdbDataBase *Kdb::_dataBase;

Kdb::Kdb()
{
	_dataBase       = 0;
	_connectionType = None;
	_dateFormat     = "%m/%d/%Y";
}

Kdb::~Kdb()
{
}

bool
Kdb::isOpen()
{
	return ( _connectionType != None);
}

void
Kdb::Open(Bases p_baseType, const QString& p_base, const QString& p_host)
{
	switch( p_baseType ) {
		case Postgres:
			_dataBase = new kdbPgBase( p_base,p_host );
			_connectionType = p_baseType;
			break;
		default:
			throw "Not implemented yet";
	}
}

void
Kdb::Close()
{
	if ( _dataBase )
		delete _dataBase;
	_dataBase = 0;
	_connectionType = None;
}

kdbDataBase *
Kdb::dataBase()
{
	if ( _connectionType == None )
		throw "No such database open";
	return _dataBase;
}

bool
Kdb::hasDataSet(const QString& p_set)
{
	QObjectList *list;
	QObject     *obj;
		
	if ( !_dataBase )
		throw "Open database first";
	list  = _dataBase->queryList( "kdbDataSet",p_set,true,false );
	QObjectListIt it( *list );
	for( ;(obj = it.current());++it )
		break;
	delete list;
	return (obj != 0);
}

kdbDataSet *
Kdb::dataSet(const QString& p_set)
{
	QObjectList *list;
	QObject     *obj;
	
	if ( !_dataBase )
		throw "Open database first";
	list = _dataBase->queryList( "kdbDataSet",p_set,true,false );
	QObjectListIt it( *list );
	for( ;(obj = it.current());++it )
		break;
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































