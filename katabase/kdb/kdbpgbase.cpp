/***************************************************************************
                          kdbPgBase.cpp  -  description
                             -------------------                                         
    begin                : Sat Jun 5 1999                                           
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
#include <libpq++.h>

#include <qobject.h>
#include <qobjectlist.h>

#include <kdb.h>
#include <kdbdataset.h>
#include <kdbfieldset.h>
#include <kdbdatafield.h>
#include "kdbpgbase.h"

/** The postgres environment, needed by the posgres library
	* to make a successful connection.  This is made static, as
	* this can be shared by any number of database connections
	* within a program.
*/
static PgEnv      *_env;
/** Each program can have several databases open.  Each
	* entity must therefore have it's own database connection
	* open to the postgres backend.
*/
static PgDatabase *_dbase;

kdbPgBase::kdbPgBase(const QString& p_base, const QString& p_host, const QString& p_port)
 : kdbDataBase(p_base)
{
	QString dateStyle = getenv("PGDATESTYLE");
	QChar sep = '-';
		
	setenv( "PGDATESTYLE","US",1 );
	if ( dateStyle.contains("iso",false) ) {
		Kdb::setDateFormat( "%Y-%m-%d" );
		dateStyle = "ISO";
	} else if ( dateStyle.contains("german",false) ) {
		Kdb::setDateFormat( "%d.%m.%Y" );
		dateStyle = "German";
	} else {
		if ( dateStyle.contains("sql",false) )
			sep = '/';
		if ( dateStyle.contains("euro",false) ) {
			Kdb::setDateFormat( QString("%d%1%m%2%Y").arg(sep).arg(sep) );
			setenv( "PGDATESTYLE","European",1 );
		} else
			Kdb::setDateFormat( QString("%m%1%d%2%Y").arg(sep).arg(sep) );
		if ( sep == '/' )
			dateStyle = "SQL";
		else
			dateStyle = "Postgres";
  }
  _env   = new PgEnv("ident",p_host.data(),p_port.data(),"","");
	_dbase = new PgDatabase(*_env, p_base);
	if (_dbase->ConnectionBad())
		throw Kdb::NoHost;
	_dbase->Exec( QString("SET DATESTYLE TO '%1'").arg(dateStyle) );
}

kdbPgBase::~kdbPgBase()
{
	delete _env;
	delete _dbase;
}

QString
kdbPgBase::fieldList(kdbFieldSet *p_set, bool p_types)
{
	QString fl;
	int i;
	kdbDataField *field;

	if (p_set == 0)
		return "";
	for(i=p_set->count()-1;i>=0;i--) {
		field = &p_set->field(i);
		fl += field->name();
		if (p_types)
			fl += QString(" %1(%2)").arg(field->type()).arg(field->length());
		if (i > 0)
			fl += ",";
	}
	return fl;
}

bool
kdbPgBase::create(const QString& p_table, kdbFieldSet *p_set)
{
	QString command;

	command.sprintf("CREATE TABLE %s(", p_table.data());
	command += fieldList(p_set, true);
	command += ")";
  return _dbase->Exec(command) == PGRES_COMMAND_OK;
}

bool
kdbPgBase::insert(const QString& p_table, kdbFieldSet *p_set)
{
	QString command, fl, dl;

	fl = p_set->fieldList(',');
	dl = p_set->valueList(',','\'');
	command = QString("INSERT INTO %1(%2) VALUES(%3)").arg(p_table).arg(fl).arg(dl);
	return _dbase->ExecCommandOk(command);
}

bool
kdbPgBase::insert(kdbDataSet *p_set)
{
	uint i;
	uint recpos;
		
	if ( !p_set )
		return false;
	recpos = p_set->record();
	_dbase->Exec("BEGIN");
	for( i=0;i<p_set->records();i++ ) {
		p_set->gotoRec( i );
		if ( p_set->recid() == -1 )
			if ( insert(p_set->name(), p_set->fieldSet()) ) {
				p_set->setRecId( QString(_dbase->OidStatus()).toInt() );
				p_set->update();
			}
	}
	_dbase->Exec("COMMIT");
	p_set->gotoRec( recpos );
	return true;
}

bool
kdbPgBase::update(kdbDataSet *p_set)
{
	QString command, setting;
	kdbFieldSet *fset;
	unsigned int i,f;

	_dbase->Exec("BEGIN");
	command = QString("UPDATE %1 SET ").arg(p_set->name());	
	for(i=0;i<p_set->records();i++) {
		(*p_set)[i];
		setting = "";
		if (p_set->recid() < -1) {
		  fset = p_set->fieldSet();
		  for(f=0;f<fset->count();f++) {
		  	if (f > 0)
		  		setting += ',';
		  	setting += QString("%1 = '%2'").arg(fset->field(f).name()).arg(fset->field(f).text());
		  }
			setting += QString(" WHERE oid = '%1'").arg(-p_set->recid());
			if ((f = _dbase->Exec(command+setting)) != PGRES_COMMAND_OK) {
				p_set->setRecId(-1);
				p_set->update();
			} else
				p_set->setRecId( -p_set->recid() );
		}
	}
	_dbase->Exec("COMMIT");
	p_set->gotoRec();
	return true;
}

bool
kdbPgBase::remove(kdbDataSet *p_set)
{
	QString command;
	unsigned int i=0;
	
	if ( !p_set )
		return false;
	_dbase->Exec("BEGIN");
	command = QString("DELETE FROM %1 WHERE oid = ").arg(p_set->name());
	while(i<p_set->records()) {
		(*p_set)[i];
		if(p_set->record() >= 0)
			if (_dbase->Exec(command+QString("'%1'").arg(p_set->recid())) == PGRES_COMMAND_OK)
				p_set->remove();
			else
				i++;
	}
	_dbase->Exec("COMMIT");
	return true;
}

bool
kdbPgBase::remove(const QString& p_table)
{
	QString command;

	command = QString("DROP TABLE %1").arg(p_table);
	return _dbase->Exec(command) == PGRES_COMMAND_OK;
}

bool
kdbPgBase::query(kdbDataSet *p_set)
{
	QString command, wclause="";
	QString fl, tl;
	QString wval="";
	kdbFieldSet *fset;
	kdbCriteria *crit;
	int i;

	if ( !p_set )
		return false;
	emit preLoad( p_set );
	tl  += p_set->name();
	fset = p_set->fieldSet();
	if ( fset->count() == 0 )
		getFields( p_set );
	fl = fieldList( fset,false );
	command = QString("SELECT oid,") + fl + QString(" FROM ") + tl;
	QObjectList *list = p_set->queryList( "kdbCriteria",0 );
	QObjectListIt it( *list );
	crit = (kdbCriteria *)it.current();
	kdbDataSet *join = p_set->join();
	if ( join ) {
		if ( join->fieldSet()->count() == 0 )
			getFields( join );
		join->gotoRec();
		if ( p_set->joinField() != "" && join->fieldSet()->has(p_set->joinField()) )
			wval = (*join)[p_set->joinField()].text();
		else if ( crit && join->fieldSet()->has(crit->field()) )
			wval = (*join)[crit->field()].text();
	} else if ( crit )
		wval = crit->value();
	if (wval != "") {
		wclause = QString(" WHERE ");
		while ( crit ) {
			switch((Kdb::RelationType)*crit) {
				case Kdb::Greater:
					wclause += QString("(%1 > '%2')").arg(crit->field()).arg(wval);
					break;
				case Kdb::Equal:
					wclause += QString("(%1 = '%2')").arg(crit->field()).arg(wval);
					break;
				case Kdb::Less:
					wclause += QString("(%1 < '%2')").arg(crit->field()).arg(wval);
					break;
				case Kdb::Like:
					wclause += QString("(%1 LIKE '%2')").arg(crit->field()).arg(wval);
					break;
			}
			++it;
			crit = (kdbCriteria *)it.current();
			if ( crit ) {
				wval = crit->value();
				switch((Kdb::Operator)*crit) {
					case Kdb::And:
						wclause += " AND ";
						break;
					case Kdb::Or:
						wclause += " OR ";
						break;
					case Kdb::NAnd:
						wclause += " AND NOT ";
						break;
					case Kdb::NOr:
						wclause += " OR NOT ";
						break;
				}
			}
		}
	}
	if ( !_dbase->ExecCommandOk("BEGIN") )
		return false;
	if ( _dbase->ExecCommandOk(QString("DECLARE query CURSOR FOR ")+command+wclause) )
	{
		p_set->clear();
		if ( _dbase->ExecTuplesOk(QString("SELECT Count(*) FROM %1").arg(tl)+wclause) && _dbase->Tuples() )
			p_set->progressControl( QString(_dbase->GetValue(0,0)).toInt() );
		while( _dbase->ExecTuplesOk("FETCH NEXT IN query") && _dbase->Tuples() ) {
			for( i=1;i<_dbase->Fields();i++ )
				fset->field( _dbase->FieldName(i)).set(_dbase->GetValue(0,i) );
			p_set->insert( QString(_dbase->GetValue(0,0)).toInt() );
		}
		_dbase->Exec( "CLOSE query" );
	}
	_dbase->Exec( "COMMIT" );
	emit postLoad( p_set );
	return true;
}

bool
kdbPgBase::getFields(kdbDataSet *set)
{
	kdbFieldSet *fset;
	kdbDataField *field;
	QString command;
	unsigned int i;

	if ( !set )
		return false;
	fset = set->fieldSet();
	command = QString("SELECT * FROM %1 WHERE false").arg(set->name());
	if ( !_dbase->ExecTuplesOk(command) )
		return false;
	fset->clear();
	for(i=0;i<(unsigned)_dbase->Fields();i++) {
		fset->addField(_dbase->FieldName(i));
		field = &fset->field(i);
		field->setLength(_dbase->FieldSize(i));
		if (_dbase->FieldSize(i) == -1 && _dbase->Tuples())
			field->setLength(strlen(_dbase->GetValue(0,i)));
		field->setType(QString("").sprintf("%d", _dbase->FieldType(i)));
	}
	for(i=0;i<fset->count();i++) {
		field = &fset->field(i);
		command.sprintf("SELECT typname FROM pg_type WHERE oid = '%s'",field->type().data());
		if ( _dbase->ExecTuplesOk(command) && _dbase->Tuples())
			field->setType(_dbase->GetValue(0,"typname"));
	}
	return true;
}

























































