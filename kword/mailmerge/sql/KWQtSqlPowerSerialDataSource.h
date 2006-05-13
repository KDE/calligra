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

#ifndef _SERIALLETTER_QtSql_POWER_PLUGIN_H_
#define _SERIALLETTER_QtSql_POWER_PLUGIN_H_

#include <qdom.h>
#include <klocale.h>
#include <kiconloader.h>
#include <kdialogbase.h>
#include <qsqldatabase.h>
/* FOR THE DIRTY HACK */
#include <q3sqlcursor.h>
#include <qsqldriver.h>
/* END FOR THE DIRTY HACK */
#include <QPointer>

#include "defs.h"
#include "KWMailMergeDataSource.h"
#include "KWQtSqlSerialDataSourceBase.h"
#include "kwqtsqlpower.h"
#include "KWMySqlCursor.h"

/******************************************************************
 *
 * Class: KWQtSqlSerialDataSource
 *
 ******************************************************************/
class KWQtSqlPowerSerialDataSource: public KWQtSqlSerialDataSourceBase
{
    Q_OBJECT
    public:
    KWQtSqlPowerSerialDataSource(KInstance *inst,QObject *parent);
    ~KWQtSqlPowerSerialDataSource();

    virtual void save( QDomDocument &doc,QDomElement&);
    virtual void load( QDomElement& elem );
    virtual class QString getValue( const class QString &name, int record = -1 ) const;
    virtual int getNumRecords() const {
        return (myquery?((myquery->size()<0)?0:myquery->size()):0);
    }
    virtual  bool showConfigDialog(QWidget *,int);
    virtual void refresh(bool force);

    protected:
	friend class KWQtSqlPowerMailMergeEditor;
	QString query;
	KWMySqlCursor *myquery;

    void clearSampleRecord();
    void addSampleRecordEntry(QString name);

};

/******************************************************************
 *
 * Class: KWQtSqlPowerMailMergeEditor
 *
 ******************************************************************/

class KWQtSqlPowerMailMergeEditor : public KDialogBase
{
    Q_OBJECT

public:
    KWQtSqlPowerMailMergeEditor( QWidget *parent, KWQtSqlPowerSerialDataSource *db_ );
    ~KWQtSqlPowerMailMergeEditor();
private:
 KWQtSqlPowerSerialDataSource *db;
 KWQtSqlPowerWidget *widget;
private slots:
 void openSetup();
 void updateDBViews();
 void slotTableChanged ( Q3ListBoxItem * item );
 void slotExecute();
 void slotSetQuery();
};


#endif
