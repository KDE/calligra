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

clsTblDesigner2::clsTblDesigner2(){
   this->resize(575,420);
   myQTable = new QTable(1,6, this, "myQTable");
   myQTable->resize(575,420);

   QHeader *myHeader=myQTable->horizontalHeader();

   myHeader->setLabel(0,"Primary Key");
   myHeader->setLabel(1,"Name");
   myHeader->setLabel(2,"Data Type");
   myHeader->setLabel(3,"Size");
   myHeader->setLabel(4,"Default");
   myHeader->setLabel(5,"Allow Null");

   //QCheckTableItem* myCheckTableItem1= new QCheckTableItem(myQTable, "");
	myQTable->setItem( 0, 0, new QCheckTableItem( myQTable, "" ) );
	myQTable->setItem( 0, 5, new QCheckTableItem( myQTable, "" ) );

   QStringList comboEntries;
   comboEntries << "int" << "char" << "varchar" << "float" << "boolean";

	myQTable->setItem( 0, 2, new QComboTableItem( myQTable, comboEntries, FALSE ));

}

clsTblDesigner2::~clsTblDesigner2(){
}


bool clsTblDesigner2::populateTblDesigner(QString tblName){

    //QString myTblName = &tblName + " - Table Designer";
    tblName.append(" - Table Designer");
    this->setCaption(tblName);
    return(true);
}

//#include "clstbldesigner2.moc"
