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
#include <qdom.h>


KDBStruct::KDBStruct(QDomDocument* KDBFile){

    myKDBFile = KDBFile;
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


QString* KDBStruct::getTables(){

    return(NULL);
}

QString* KDBStruct::getViews(){

    return(NULL);
}

QString* KDBStruct::getForms(){

    return(NULL);
}


QString KDBTable::getColumns(QString *tableName, QString *returnMessage){

return(NULL);
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
