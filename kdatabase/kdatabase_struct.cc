/***************************************************************************
                          kdatabase_struct.cpp  -  description
                             -------------------
    begin                : Mon Apr 22 2002
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
#include "kdatabase_struct.h"
#include <stdlib.h>
#include <kdebug.h>
#include <qdom.h>
#include <qptrlist.h>

KDBStruct::KDBStruct(QDomDocument* KDBFile){

    myKDBNode = KDBFile;
}

bool KDBStruct::createTable(QString* tableName, QString fieldInfo){

   return(false);
}


KDBTable* KDBStruct::getTable(QString tblName) {
   KDBTable *myTable = new KDBTable(myKDBNode);
   return myTable;
}

bool KDBStruct::createView(QString* viewName, QString viewSQL){

   return(false);
}

bool KDBStruct::createReport(QString* reportName, QString reportSQL){

    return(false);
}


QPtrList<QString> KDBStruct::getTables(){

    QPtrList<QString> *tableList = new QPtrList <QString>;
    if(myKDBNode->isNull()) {
       kdDebug() << "KDBNode is already null" << endl;
      }
    QDomNode myTablesNode = myKDBNode->documentElement().namedItem("STRUCTURE");
    if(myTablesNode.isNull()) {
       kdDebug() << "myTablesNode is already null" << endl;
      }

    myTablesNode=myTablesNode.namedItem("TABLES");
    QDomNode myTableWalker = myTablesNode.firstChild();
    QDomElement myElement;
    while(!myTableWalker.isNull()) {
        myElement = myTableWalker.toElement();
        const QString* tableName = new QString(myElement.attribute("name"));
        const QString* tableDescript = new QString(myElement.attribute("description"));
        tableList->append(tableName);
        tableList->append(tableDescript);
        myTableWalker = myTableWalker.nextSibling();
        }

    return(*tableList);
}

QPtrList<QString> KDBStruct::getViews(){
    QPtrList<QString> viewList;

    return(viewList);
}

QPtrList<QString> KDBStruct::getForms(){
    QPtrList<QString> formList;

    return(formList);
}

QPtrList<TableStructureRow>* KDBTable::getColumns(QString *tableName, QString *returnMessage)
{

    QPtrList<TableStructureRow> *columnList = new QPtrList<TableStructureRow>;
    //const TableStructureRow* myRow;
    int mySize;
    bool myKey;
    bool myAllowNull;
    DataType myType;
    QString myName;
    QString myDefault;

    //columnList = new QPtrList<TableStructureRow>;
    QDomNode myTableNode = myKDBNode->documentElement().namedItem("STRUCTURE");

    myTableNode=myTableNode.namedItem("TABLES");

    QDomNode myTableWalker = myTableNode.firstChild();
    QDomElement myElement;
    bool tableFound = false;
    //find the table in question
    while(!tableFound) {
        myElement = myTableWalker.toElement();
        QString myTableName = myElement.attribute("name");
        if (QString::localeAwareCompare(tableName->latin1(),myTableName.latin1())==0) {
          tableFound=TRUE; }
        else {
          myTableWalker = myTableWalker.nextSibling(); }
        }

     //now that the table is found, build the list of its fields
     myTableWalker = myTableWalker.namedItem("FIELD");
     myElement = myTableWalker.toElement();
     QString columnIsInKey = myElement.attribute("isPKey");
     QString columnName = myElement.attribute("name");
     QString columnType = myElement.attribute("datatype");
     QString columnSize = myElement.attribute("defaultsize");
     QString columnDefault = myElement.attribute("defaultvalue");
     QString columnAllowsNull = myElement.attribute("allownull");

     if (QString::compare(columnIsInKey.latin1(), "false")==0) {
        myKey = false;
        }
     else {
        myKey = true;
        }
     myName = columnName.latin1();
     if(QString::compare(columnType.latin1(), "int") == 0) {
		myType = t_int;
       }
     else if(QString::compare(columnType.latin1(), "char") ==0) {
		myType = t_char;
       }
     else if(QString::compare(columnType.latin1(), "varchar")==0) {
		myType = t_vchar;
       }
     else if(QString::compare(columnType.latin1(), "float")==0) {
		myType = t_float;
       }
     else {
		myType = t_boolen;
       }
     mySize = columnSize.toInt();
     myDefault = columnDefault.latin1();
     if (QString::compare(columnAllowsNull.latin1(), "false")==0) {
        myAllowNull = false;
        }
     else {
        myAllowNull = true;
        }

     const TableStructureRow* myRow1 = new TableStructureRow(myKey, myName, myType, mySize, myDefault, myAllowNull);
     columnList->append(myRow1);
     myTableWalker = myTableWalker.nextSibling();

    while(!myTableWalker.isNull()) {
      myElement = myTableWalker.toElement();
      columnIsInKey = myElement.attribute("isPKey");
      columnName = myElement.attribute("name");
      columnType = myElement.attribute("datatype");
      columnSize = myElement.attribute("defaultsize");
      columnDefault = myElement.attribute("defaultvalue");
      columnAllowsNull = myElement.attribute("allownull");

     if (QString::compare(columnIsInKey.latin1(), "false")==0) {
        myKey = false;
        }
     else {
        myKey = true;
        }
     myName = columnName.latin1();
     if(columnType.latin1() == "int") {
		myType = t_int;
       }
     else if(columnType.latin1() == "char") {
		myType = t_char;
       }
     else if(columnType.latin1() == "varchar") {
		myType = t_vchar;
       }
     else if(columnType.latin1() == "float") {
		myType = t_float;
       }
     else {
		myType = t_boolen;
       }
     mySize = columnSize.toInt();
     myDefault = columnDefault.latin1();
     if (QString::compare(columnAllowsNull.latin1(), "false")==0) {
        myAllowNull = false;
        }
     else {
        myAllowNull = true;
        }
     const TableStructureRow* myRow1 = new TableStructureRow(myKey, myName, myType, mySize, myDefault, myAllowNull);
     columnList->append(myRow1);

      myTableWalker = myTableWalker.nextSibling();
       }

    kdDebug() << "columnList contains " << columnList->count() << " items." << endl;

    return(columnList);
}

bool KDBTable::insertColumn(QString *tableName, QString *columnInfo, QString *returnMessage){

return(true);
}

bool KDBTable::modifyColumn(QString *tableName, QString *columnInfo, QString *returnMessage){

return(true);
}

bool KDBTable::deleteColumn(QString *tableName, QString *columnInfo, QString *returnMessage){

return(true);
}

KDBTable::KDBTable(QDomDocument* KDBFile){
    myKDBNode = KDBFile;
}

KDBTable::~KDBTable(void){

}

TableStructureRow::TableStructureRow(bool pri_key, QString myname, DataType mytype, int mySize, QString myDefault,bool allowNull) {
	 primary_key = pri_key;
	 name = myname;
	 type = mytype;
	 size=mySize;
 	 Default = myDefault;
	 allow_null = allowNull;
}

TableStructureRow::TableStructureRow( ) {
	 primary_key = false;
	 name = "";
	 type = t_int;
	 size=0;
 	 Default = "";
	 allow_null = false;
}

TableStructureRow::~TableStructureRow( ) {

}
