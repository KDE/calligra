/***************************************************************************
                          tableselect.cpp  -  description                              
                             -------------------                                         
    begin                : Thu Jul 8 1999                                           
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

#include <qlistbox.h>

#include <klocale.h>

#include <kdb.h>
#include <kdbdataset.h>
#include <kdbdatabase.h>
#include <kdbcriteria.h>
#include <kdbfieldset.h>

#include "tableselect.h"

TableSelect::TableSelect(QWidget *parent, const char *name)
 : QDialog(parent,name,true)
{
	initDialog();
}

void
TableSelect::init()
{
	kdbDataSet  *set;
	kdbCriteria *crit;

	if ( !Kdb::isOpen() )
		return; // Hey! you're not supposed to be here.
		
	set = new kdbDataSet(Kdb::dataBase(), "pg_class");
	set->setVisualProgress( false );
	set->fieldSet()->addField("relname");
  crit = new kdbCriteria( set,"relkind" );
  (*crit) = Kdb::Equal;
  (*crit) = "r";
  crit = new kdbCriteria( set,"relname" );
  (*crit) = Kdb::Like;
  (*crit) = Kdb::NAnd;
  (*crit) = "pg_%";
  Kdb::dataBase()->query( set );
  for( uint i=0;i<set->records();i++ )
  	_available->insertItem( (*set)[i]["relname"].text() );
  delete set;
  signalMsg(i18n("Ready."));
}

TableSelect::~TableSelect()
{
}

int
TableSelect::tables() const
{
	return _selected->count();
}

QString
TableSelect::tableName(int p_idx)
{
	if ( p_idx >= _selected->count() )
		return "";
	return _selected->text( p_idx );
}

void
TableSelect::accept()
{
	if ( _selected->count() > 0 )
		emit tableSelectionOk();
	hide();
}

void
TableSelect::reject()
{
	while ( _selected->count() > 0 )
		removeItem( 0 );
	hide();
}

void
TableSelect::acceptItem()
{
	acceptItem( _available->currentItem() );
}

void
TableSelect::acceptItem(int p_idx)
{
	QString str;
	
	if ( p_idx >= _available->count() )
		return;
	str = _available->text( p_idx );
	_available->removeItem( p_idx );
	_selected->insertItem( str );
}

void
TableSelect::removeItem()
{
	removeItem( _selected->currentItem() );
}

void
TableSelect::removeItem(int p_idx)
{
	QString str;
	
	if ( p_idx >= _selected->count() )
		return;
	str = _selected->text( p_idx );
	_selected->removeItem( p_idx );
	_available->insertItem( str );
}

#include "tableselect.moc"


