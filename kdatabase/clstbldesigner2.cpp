/***************************************************************************
                          clstbldesigner2.cpp  -  description
                             -------------------
    begin                : Mon Apr 29 2002
    copyright            : (C) 2002 by Chris Machemer
    email                : machey@ceinetworks.com
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License.         *
 *                                                                         *
 ***************************************************************************/

#include "clstbldesigner2.h"
#include <qheader.h>
#include <qtable.h>
#include <qlayout.h>
#include <kdebug.h>


clsTblDesigner2::clsTblDesigner2(){
	QGridLayout *g = new QGridLayout(this);

	this->resize(575,420);
	
	m_table = new QTable(1,6, this, "myQTable");
	m_table->resize(575,420);

	QHeader *myHeader = m_table->horizontalHeader();

	myHeader->setLabel(0,"Primary Key");
	myHeader->setLabel(1,"Name");
	myHeader->setLabel(2,"Data Type");
	myHeader->setLabel(3,"Size");
	myHeader->setLabel(4,"Default");
	myHeader->setLabel(5,"Allow Null");
  
//	m_table->setItem( 0, 0, new QCheckTableItem( m_table, "" ) );
//	m_table->setItem( 0, 5, new QCheckTableItem( m_table, "" ) );

//	QStringList m_comboEntries;
	m_comboEntries << "int" << "char" << "varchar" << "float" << "boolean";

//	m_table->setItem( 0, 2, new QComboTableItem( m_table, comboEntries, FALSE ));

	g->addWidget(m_table,	0,	0);

	m_rows = 0;

	
	this->addRow(true, "id", t_int, 12, "");
	this->addRow(false, "name", t_char, 10, "nobody");
}

clsTblDesigner2::~clsTblDesigner2(){
}


bool clsTblDesigner2::populateTblDesigner(QString tblName){

    //QString myTblName = &tblName + " - Table Designer";
    tblName.append(" - Table Designer");
    this->setCaption(tblName);
    return(true);
}

void clsTblDesigner2::addRow(bool primary_key, QString name, DataType type, int size, QString default_v, bool allow_null)
{
	kdDebug() << "clsTblDesigner2::addRow: adding row " << m_rows << endl;
	m_table->setNumRows(m_rows + 1);
	m_table->setItem(m_rows, 0, new QCheckTableItem(m_table, ""));
	m_table->setItem(m_rows, 0, new QCheckTableItem(m_table, ""));
	QComboTableItem *dataTypeView = new QComboTableItem(m_table, m_comboEntries, false);
	m_table->setItem(m_rows, 2, dataTypeView);
	
	switch(type)
	{
	
		case t_int:
			kdDebug() << "clsTblDesigner2::addRow: " << m_rows << " is int" << endl;
			dataTypeView->setCurrentItem(0);
			break;
	
		case t_char:
			kdDebug() << "clsTblDesigner2::addRow: " << m_rows << " is char" << endl;
			dataTypeView->setCurrentItem(1);
			break;
			
		case t_vchar:
			dataTypeView->setCurrentItem(2);
			break;
		
		case t_float:
			dataTypeView->setCurrentItem(3);
			break;
			
		case t_boolen:
			dataTypeView->setCurrentItem(4);
			break;
		
/*		default:
			kdDebug() << "clsTblDesigner2::addRow: " << m_rows << " isn't char or int" << endl;
			break;
*/
	
	}
	m_table->setText(m_rows, 1, name);
	//m_table->setText(m_rows, 3, (const QString)size);
	m_table->setText(m_rows, 4, default_v);
	m_rows++;
}

#include "clstbldesigner2.moc"
