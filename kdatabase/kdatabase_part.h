/* This file is part of the KDE project
   Copyright (C) 2002 Chris Machemer <machey@ceinetworks.com>

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License as published by the Free Software Foundation; either
   version 2 of the License.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this library; see the file COPYING.LIB.  If not, write to
   the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
   Boston, MA 02111-1307, USA.
*/

#ifndef KDATABASE_PART_H
#define KDATABASE_PART_H

#include <koDocument.h>
#include <klocale.h>

#include "maindlg.h"

class KDatabasePart;
class KDBStruct;
class KDBTable;
class KDBView;
class KDBForm;

class KDatabasePart : public KoDocument
{
    Q_OBJECT
public:
    KDatabasePart( QWidget *parentWidget = 0, const char *widgetName = 0, QObject* parent = 0, const char* name = 0, bool singleViewMode = false );

    virtual void paintContent( QPainter& painter, const QRect& rect, bool transparent = FALSE, double zoomX = 1.0, double zoomY = 1.0 );

    virtual bool initDoc();

    virtual bool loadXML( QIODevice *, const QDomDocument & );
    virtual QDomDocument saveXML();

    QDomDocument *getKDBFile();
private:
    QDomDocument KDBFile;

protected:
    virtual KoView* createViewInstance( QWidget* parent, const char* name );

};

class KDBTable {
//This class is meant for viewing and modifying table structure.
//KDBTableData is for accessing and modifying data.
public:
      QString getColumns(QString *tableName);
      bool insertColumn(QString *tableName, QString *columnInfo);
      bool modifyColumn(QString *tableName, QString *columnInfo);
      bool deleteColumn(QString *tableName, QString *columnInfo);

      KDBTable(void);

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

    QString* executeSQL(QString *sSQL, bool returnXML);

};
#endif
