/***************************************************************************
                          kdbDataSet.cpp  -  description
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

#include <qapplication.h>
#include <qobjectlist.h>

#include <kdb.h>
#include <kdbcriteria.h>
#include <kdbdatafield.h>
#include <kdbfieldset.h>
#include <kdbdatabase.h>
#include <kdbrelation.h>

#include "kdbdataset.h"
#include "kdbdataset.moc"

/**
 * Assignment of a new dataset, with a copy
 * of another.
 */
kdbDataSet::kdbDataSet(kdbDataSet& p_set)
 : QObject(p_set.dataBase(),p_set.name())
{
	_dataBase     = p_set.dataBase();
	_fields       = p_set.fieldSet();
	_table        = p_set.name();
	_record       = p_set.record();
	_join	        = p_set.join();
	_joinField    = p_set.joinField();
	_recid        = -1;
	_showProgress = _dataBase != 0;
	_data.setAutoDelete(true);
	if ( _showProgress )
		connect(_dataBase,SIGNAL(postLoad(kdbDataSet*)),SLOT(postLoad(kdbDataSet*)));
}

/**
 * Create a new dataset, and assign it to a specific
 * table.
 */
kdbDataSet::kdbDataSet(kdbDataBase *p_par,const QString& p_table)
 : QObject(p_par,p_table)
{
	_dataBase     = p_par;
	_fields       = new kdbFieldSet( this );
	_table        = p_table;
	_join         = 0;
	_record       = 0;
	_joinField    = "";
	_recid        = -1;
	_showProgress = _dataBase != 0;
	_data.setAutoDelete(true);
	if ( _showProgress )
		connect(_dataBase,SIGNAL(postLoad(kdbDataSet*)),SLOT(postLoad(kdbDataSet*)));
}

kdbDataSet::~kdbDataSet()
{
	delete _fields;
}

kdbFieldSet*
kdbDataSet::fieldSet()
{
	return _fields;
}

uint
kdbDataSet::fieldNo(const QString& p_f)
{
	return _fields->fieldNo(p_f);
}

kdbDataField&
kdbDataSet::field(const QString& p_f)
{
	return _fields->field(p_f);
}

kdbDataField&
kdbDataSet::field(uint p_ix)
{
	return _fields->field(p_ix);
}

void
kdbDataSet::clear()
{
	_record = 0;
	emit clearData();
	_data.clear();
}

void
kdbDataSet::progressControl(uint size)
{
	_progress = 0;
	if ( _showProgress ) {
		_progress  = new QProgressBar( size );
		QSize sz   = QApplication::desktop()->size();
		QSize psz  = _progress->sizeHint();
		QPoint pos = QPoint((sz.width()-(psz.width()*3)/2)/2,(sz.height()-psz.height())/2);
		_progress->move(pos);
		_progress->show();
	}
}

void
kdbDataSet::runQuery(bool p_trav)
{
	QObjectList *list;
	QObject     *obj;
	uint i;
	
	if ( !_dataBase )
		return;
	if ( p_trav && _join )
		_join->runQuery( true );
	list = queryList( "kdbRelation",0 );
	QObjectListIt it( *list );
	for( ;(obj = it.current());++it )
		((kdbRelation *)obj)->runQuery();
	for( i=0;i<_fields->count();i++ )
		_fields->field( i ).blockSignals( true );
	blockSignals( true );
	_dataBase->query( this );
	blockSignals( false );
	for( i=0;i<_fields->count();i++ )
		_fields->field( i ).blockSignals( false );
	emit initData();
}

void
kdbDataSet::postLoad(kdbDataSet *p_set)
{
	if ( p_set == this )
		if ( _showProgress && _progress ) {
			delete _progress;
			_progress = 0;
		}
}

void
kdbDataSet::setRecId(int p_rec)
{
	if (locate(p_rec))
		return;
	_recid = p_rec;
	update();
}

/**
 * Join this dataset, with another dataset.  The join is
 * over a specific field, which is the joinfield for the
 * joined set.  And the criteria field, for this set, when
 * query is run over the dataset.
 */
void
kdbDataSet::setJoin(class kdbDataSet *p_set, const QString& p_field)
{
	if (p_set == this || p_field == "")
		return;
	if (p_set->fieldSet()->has(p_field)) {
		_join      = p_set;
		_joinField = p_field;
	}
}

/**
 * A private function, to check if a record with a specific
 * record id exists in the dataset.
 */
bool
kdbDataSet::locate(int p_recid)
{
	QString *str;
	int ix;
	
	if (p_recid == -1)
		return false;
	for(unsigned int i=0;i<_data.count();i++) {
		str = _data.at(i);
		ix = str->find(':');
		if (ix > 0 && str->left(ix).toInt() == p_recid)
			return true;
	}
	return false;
}

/**
 * A private function, used to disassemble a string
 * into field data.
 */
void
kdbDataSet::retrieve()
{
	QString *str;
	int p=0,e,i,len=_fields->count();

	if ((str = _data.current()) == 0)
		return;
	p = str->find(':',0)+1;
	_recid = str->left(p-1).toInt();
	for(i=0;i<len;i++,p+=e+1) {
		e = str->find(char(1),p);
		if (e == -1)
			e = str->length();
		e -= p;
		_fields->field(i).set(str->mid(p,e).data());
	}
}

/**
 * Move the data pointer, to a specific record in the
 * dataset.
 *@return success if the record exists.
 */
bool
kdbDataSet::gotoRec(unsigned int p_ix)
{
	if (p_ix >= _data.count() || _data.at(p_ix) == 0)
		return false;
	_record = p_ix;
	retrieve();
	return true;
}

bool
kdbDataSet::gotoRec()
{
	return gotoRec(_record);
}

/**
 * Move the data pointer, to the next record in the
 * dataset.
 *@return success if there is a next record.
 */
bool
kdbDataSet::nextRec()
{
	if (_data.next() == 0)
		return false;
	_record = _data.at();
	retrieve();
	return true;
}

/**
 * Move the pointer, and disassemble the last record in
 * the dataset, if there is one.
 *@return success if there is a record before this one.
 */
bool
kdbDataSet::prevRec()
{
	if (_data.prev() == 0)
		return false;
	_record = _data.at();
	retrieve();
	return true;
}

/**
 * Insert a new record into the dataset.  The record will
 * have the given record id, which must be aunique number
 * or (-1) to indicate a new record.
 *@return success if the record was inserted, false.otherwise.
 */
bool
kdbDataSet::insert(int p_recid)
{
	bool cont = true;
	QString *str = new QString;
	int i,len=_fields->count();

	emit preInsert( cont );
	if ( !cont || (p_recid != -1 && locate(p_recid)) )
		return false;
	_recid = p_recid;
	str->sprintf("%d:",p_recid);
	for(i=0;i<len;i++) {
		*str += _fields->field(i).text();
		if (i < len)
			*str += char(1);
	}
	_data.append(str);
	_record = _data.at();
	if ( _progress )
		_progress->setProgress( _data.count() );
	emit postInsert();
	return true;
}

/**
 * Update the current record in the dataset, to hold
 * the information currently held in their respective
 * fields.
 */
bool
kdbDataSet::update()
{
	QString *str;
	bool cont = true;
	int i,len=_fields->count();

	emit preUpdate( cont );
	if ( !cont || (str = _data.current()) == 0 )
		return false;
	str->sprintf("%d:",(_recid<0?_recid:-_recid));
	for(i=0;i<len;i++) {
		(*str) += _fields->field(i).text();
		if (i < len)
			(*str) += char(1);
	}
	emit postUpdate();
	return true;
}

/**
 * Remove the current record from the dataset.  Since this
 * is only done in memory and not in the physical table, the
 * dataset needs to be synced with the table, for these
 * changes to be active.  This also gives the possibility of
 * undeleting records.
 * @return True if the removal successfull, false otherwise.
 */
bool
kdbDataSet::remove()
{
	bool cont = true;
	
	emit preDelete( cont );
	if ( !cont || _data.remove() == false)
		return false;
	_record = _data.at();
	if (_data.at() == -1)
		_record = 0;
	retrieve();
	emit postDelete();
	return true;
}

/**
 * Locates the indexed record in the dataset, and disassembles
 * it into the fields.
 *@return the fields with the disassembled text.
 */
kdbFieldSet&
kdbDataSet::operator[] (unsigned int p_ix)
{
	QString *str;
	kdbDataField *fld;
	int p=0,e,i,len=_fields->count();
  bool state;

	if ( !(str = _data.at(p_ix)) )
		throw Kdb::NoRecord;
	p = str->find(':',0)+1;
	_recid = str->left(p-1).toInt();
	for(i=0;i<len;i++,p+=e+1) {
		e = str->find(char(1),p);
		if (e == -1)
			e = str->length();
		e -= p;
		fld = &_fields->field( i );
		state = fld->signalsBlocked();
		fld->blockSignals( true );
		fld->set( str->mid(p,e) );
		fld->blockSignals( state );
	}
	return *_fields;
}































































