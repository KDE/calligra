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
   KDBTable* myTable = new KDBTable(myKDBNode);
   return myTable;
}

bool KDBStruct::createView(QString* viewName, QString viewSQL){

   return(false);
}

bool KDBStruct::createReport(QString* reportName, QString reportSQL){

    return(false);
}


QPtrList<QString> KDBStruct::getTables(){

    QPtrList<QString> tableList;
    kdDebug() << "myKDBNode.toString=" << myKDBNode->toString() << endl;
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
        tableList.append(tableName);
        tableList.append(tableDescript);
        kdDebug() << "In while loop...last table was " << &tableName << endl;
        myTableWalker = myTableWalker.nextSibling();
        }

    kdDebug() << "tableList contains " << tableList.count() << " items." << endl;
    return(tableList);
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
    TableStructureRow myRow;

    QDomNode myTableNode = myKDBNode->documentElement().namedItem("STRUCTURE");

    myTableNode=myTableNode.namedItem("TABLES");

    QDomNode myTableWalker = myTableNode.firstChild();
    QDomElement myElement;
    bool tableFound = FALSE;
    //find the table in question
    while(!tableFound) {
        myElement = myTableWalker.toElement();
        const QString* myTableName = new QString(myElement.attribute("name"));
        if (QString::localeAwareCompare(tableName->latin1(),myTableName->latin1())==0) { tableFound=TRUE; }
        kdDebug() << "In while loop...last table was " << &tableName << endl;
        myTableWalker = myTableWalker.nextSibling();
        }

     //now that the table is found, build the list of its fields
     myTableWalker = myTableWalker.namedItem("FIELD");
     myElement = myTableWalker.toElement();
     const QString* columnIsInKey = new QString(myElement.attribute("isPKey"));
     const QString* columnName = new QString(myElement.attribute("name"));
     const QString* columnType = new QString(myElement.attribute("datatype"));
     const QString* columnSize = new QString(myElement.attribute("defaultSize"));
     const QString* columnDefault = new QString(myElement.attribute("defaultvalue"));
     const QString* columnAllowsNull = new QString(myElement.attribute("allownull"));

     myRow.primary_key = columnIsInKey;
     myRow.name = columnName;
     myRow.type = columnType;
     myRow.size = columnSize;
     myRow.Default = columnDefault;
     myRow.allow_null = columnAllowsNull;
     columnList.append(myRow);

    while(!myTableWalker.isNull()) {
      myElement = myTableWalker.toElement();
      columnIsInKey = new QString(myElement.attribute("isPKey"));
      columnName = new QString(myElement.attribute("name"));
      columnType = new QString(myElement.attribute("datatype"));
      columnSize = new QString(myElement.attribute("defaultSize"));
      columnDefault = new QString(myElement.attribute("defaultvalue"));
      columnAllowsNull = new QString(myElement.attribute("allownull"));

      columnList.append(columnIsInKey);
      columnList.append(columnName);
      columnList.append(columnType);
      columnList.append(columnSize);
      columnList.append(columnDefault);
      columnList.append(columnAllowsNull);

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
