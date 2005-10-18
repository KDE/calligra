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

#ifndef _SERIALLETTER_QTSQL_POWER_PLUGIN_H_
#define _SERIALLETTER_QTSQL_POWER_PLUGIN_H_

#include <qdom.h>
#include <klocale.h>
#include <kiconloader.h>
#include <kdialogbase.h>
#include <qsqldatabase.h>
/* FOR THE DIRTY HACK */
#include <qsqlcursor.h>
#include <qsqldriver.h>
/* END FOR THE DIRTY HACK */
#include <qguardedptr.h>

#include "defs.h"
#include "KWMailMergeDataSource.h"
#include "serialletter_qtsql_base.h"
#include "kwqtsqlpower.h"
#include "serialletter_sqlcursor.h"

/******************************************************************
 *
 * Class: KWQTSQLSerialDataSource
 *
 ******************************************************************/
class KWQTSQLPowerSerialDataSource: public KWQTSQLSerialDataSourceBase
{
    Q_OBJECT
    public:
    KWQTSQLPowerSerialDataSource(KInstance *inst,QObject *parent);
    ~KWQTSQLPowerSerialDataSource();

    virtual void save( QDomDocument &doc,QDomElement&);
    virtual void load( QDomElement& elem );
    virtual class QString getValue( const class QString &name, int record = -1 ) const;
    virtual int getNumRecords() const {
        return (myquery?((myquery->size()<0)?0:myquery->size()):0);
    }
    virtual  bool showConfigDialog(QWidget *,int);
    virtual void refresh(bool force);

    protected:
	friend class KWQTSQLPowerMailMergeEditor;
	QString query;
	QMySqlCursor *myquery;

    void clearSampleRecord();
    void addSampleRecordEntry(QString name);

};

/******************************************************************
 *
 * Class: KWQTSQLPowerMailMergeEditor
 *
 ******************************************************************/

class KWQTSQLPowerMailMergeEditor : public KDialogBase
{
    Q_OBJECT

public:
    KWQTSQLPowerMailMergeEditor( QWidget *parent, KWQTSQLPowerSerialDataSource *db_ );
    ~KWQTSQLPowerMailMergeEditor();
private:
 KWQTSQLPowerSerialDataSource *db;
 KWQTSQLPowerWidget *widget;
private slots:
 void openSetup();
 void updateDBViews();
 void slotTableChanged ( QListBoxItem * item );
 void slotExecute();
 void slotSetQuery();
};


#endif
