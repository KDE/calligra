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
//    kdDebug() << "myKDBNode.toString=" << myKDBNode->toString() << endl;
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
        kdDebug() << "In while loop...last table was " << *tableName << endl;
        myTableWalker = myTableWalker.nextSibling();
        }

    kdDebug() << "tableList contains " << tableList->count() << " items." << endl;
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

QPtrList<TableStructureRow> KDBTable::getColumns(QString *tableName, QString *returnMessage)
{

    QPtrList<TableStructureRow> columnList;
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
        kdDebug() << "getColumns::In while loop...checking table name :" << *tableName << endl;
        if (QString::localeAwareCompare(tableName->latin1(),myTableName.latin1())==0) {
          kdDebug() << "getColumns::In while loop...found table " << *tableName << endl;
          tableFound=TRUE; }
        else {
          kdDebug() << "getColumns::In while loop...not yet found table " << *tableName << endl;
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

//     TableStructureRow* myRow1 = (TableStructureRow*)malloc(sizeof(TableStructureRow));
     TableStructureRow myRow1 = {myKey, myName, myType, mySize, myDefault, myAllowNull};

//     myRow1->primary_key =  myKey;
//     myRow1->name =  myName;
//     myRow1->type =  myType;
//     myRow1->size =  mySize;
//     myRow1->Default =  myDefault;
//     myRow1->allow_null =  myAllowNull;
//     const TableStructureRow* myRow = myRow1;
     const TableStructureRow *myRow = &myRow1;
     columnList.append(myRow);
     myTableWalker.nextSibling();

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
     TableStructureRow myRow1 = { myKey, myName, myType, mySize, myDefault, myAllowNull };
     const TableStructureRow *myRow = &myRow1;
     columnList.append(myRow);

      myTableWalker = myTableWalker.nextSibling();
       }

    kdDebug() << "columnList contains " << columnList.count() << " items." << endl;

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
