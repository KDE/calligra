/***************************************************************************
                          kdatabase_struct.h  -  classes to access the DB structure (tables, views, forms)
                             -------------------
    begin                : Mon Apr 22 2002
    copyright            : (C) 2002 by Chris Machemer
    email                : machey@ceinetworks.com
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; version 2 of the License.           *
 *                                                                         *
 ***************************************************************************/
#ifndef KDATABASE_STRUCT_H
#define KDATABASE_STRUCT_H

#include <qdom.h>
#include <qptrlist.h>

class KDBStruct;
class KDBTable;
class KDBView;
class KDBForm;

class KDBTable {
//This class is meant for viewing and modifying table structure.
//KDBTableData is for accessing and modifying data.
public:
      QPtrList<QString> getColumns(QString *tableName, QString *returnMessage);
      bool insertColumn(QString *tableName, QString *columnInfo, QString *returnMessage);
      bool modifyColumn(QString *tableName, QString *columnInfo, QString *returnMessage);
      bool deleteColumn(QString *tableName, QString *columnInfo, QString *returnMessage);

      KDBTable(void);
      ~KDBTable(void);

};

class KDBView {
//This class is meant for viewing and modifying views.
public:
     bool createView(QString *viewName, QString *sSQL);
     bool modifyView(QString *viewName, QString *newSQL);
     bool deleteView(QString *viewName);

     KDBView(void);
};

class KDBForm {
//This class is meant for viewing and modifying forms. Its just a placeholder for now.
public:
    KDBForm(void);

};


class KDBStruct {

public:
    KDBStruct(QDomDocument* KDBFile);

    bool createTable(QString* tableName, QString fieldInfo);
    bool createView(QString* viewName, QString viewSQL);
    bool createReport(QString* reportName, QString reportSQL);
    bool deleteTable(QString* tableName);
    bool deleteView(QString* viewName);
    bool deleteReport(QString* reportName);
    bool renameTable(QString* oldTableName, QString newTableName);
    bool renameView(QString* oldViewName, QString newViewName);
    bool renameForm(QString* oldFormName, QString newFormName);

    QPtrList<QString> getTables();
    QPtrList<QString> getViews();
    QPtrList<QString> getForms();

    QString* executeSQL(QString *sSQL, bool returnXML);

private:
    QDomNode* myKDBNode;
};

#endif
