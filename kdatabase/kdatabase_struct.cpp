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


QPtrList<QString> KDBTable::getColumns(QString *tableName, QString *returnMessage){

    QPtrList<QString> columnList;

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

KDBTable::KDBTable(void){

}

KDBTable::~KDBTable(void){

}
