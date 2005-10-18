/* This file is part of the KDE project
   Copyright (C) 2001 Joseph Wenninger <jowenn@kde.org>

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License as published by the Free Software Foundation; either
   version 2 of the License, or (at your option) any later version.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this library; see the file COPYING.LIB.  If not, write to
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
*/

#ifndef _SERIALLETTER_QTSQL_PLUGIN_H_
#define _SERIALLETTER_QTSQL_PLUGIN_H_

#include <qdom.h>
#include <qsqlcursor.h>
#include "KWMailMergeDataSource.h"
#include "serialletter_qtsql_base.h"
#include "qtsqldatasourceeditor.h"


/******************************************************************
 *
 * Class: KWQTSQLSerialDataSource
 *
 ******************************************************************/

class KWQTSQLSerialDataSource: public KWQTSQLSerialDataSourceBase
{
    public:
    KWQTSQLSerialDataSource(KInstance *inst,QObject *parent);
    ~KWQTSQLSerialDataSource();

    virtual void save( QDomDocument &doc,QDomElement&);
    virtual void load( QDomElement& elem );
    virtual class QString getValue( const class QString &name, int record = -1 ) const;
    virtual int getNumRecords() const {
        return (myquery?((myquery->size()<0)?0:myquery->size()):0);
    }
    virtual void refresh(bool);
    virtual  bool showConfigDialog(QWidget *,int);

    protected:
	friend class KWQTSQLDataSourceEditor;
	QString tableName;
	QString filter;
	QSqlCursor *myquery;
};

/******************************************************************
 *
 * Class: KWQTSQLDataSourceEditor
 *
 ******************************************************************/

class KWQTSQLDataSourceEditor : public KDialogBase
{
    Q_OBJECT

public:
    KWQTSQLDataSourceEditor( QWidget *parent, KWQTSQLSerialDataSource *db_ );
    ~KWQTSQLDataSourceEditor(){;}
private:
  KWQTSQLSerialDataSource *db;
  QTSQLDataSourceEditor *widget;
  void updateTableCombo();
  QString filter;
  QString tableName;

private slots:
  void tableChanged(int);
  void slotSetQuery();
  void editFilter();
};


#endif
