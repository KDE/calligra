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
   the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
   Boston, MA 02111-1307, USA.
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

#include <koUtils.h>
#include "defs.h"
#include "serialletter_interface.h"
#include "qtsqlopenwidget.h"
#include "kwqtsqlpower.h"

/******************************************************************
 *
 * Class: KWQTSQLSerialDataSource
 *
 ******************************************************************/
typedef QValueList< DbRecord > Db;

class KWQTSQLPowerSerialDataSource: public KWSerialLetterDataSource
{
    public:
    KWQTSQLPowerSerialDataSource(KInstance *inst);
    ~KWQTSQLPowerSerialDataSource();

    bool openDatabase();

    virtual void save( QDomDocument &doc,QDomElement&);
    virtual void load( QDomElement& elem );
    virtual class QString getValue( const class QString &name, int record = -1 ) const;
    virtual int getNumRecords() const {
        return /*(int)db.count()*/ 0;
    }
    virtual  bool showConfigDialog(QWidget *,int);

    protected:
	friend class KWQTSQLPowerSerialLetterEditor;
	friend class KWQTSQLPowerSerialLetterOpen;
	QString hostname;
	QString username;
	QString driver;
	QString port;
	QString databasename;
	QString query;
	QGuardedPtr<QSqlDatabase> database;

};

/******************************************************************
 *
 * Class: KWQTSQLSerialLetterEditor
 *
 ******************************************************************/

class KWQTSQLPowerSerialLetterEditor : public KDialogBase
{
    Q_OBJECT

public:
    KWQTSQLPowerSerialLetterEditor( QWidget *parent, KWQTSQLPowerSerialDataSource *db_ );
    ~KWQTSQLPowerSerialLetterEditor();
private:
 KWQTSQLPowerSerialDataSource *db;
 KWQTSQLPowerWidget *widget;
private slots:
 void openSetup();
 void updateDBViews();
 void slotTableChanged ( QListBoxItem * item );
 void slotExecute();
};

/******************************************************************
 *
 * Class: KWQTSQPowerLSerialLetterOpen
 *
 ******************************************************************/
class KWQTSQLPowerSerialLetterOpen : public KDialogBase
{
    Q_OBJECT

public:
    KWQTSQLPowerSerialLetterOpen( QWidget *parent, KWQTSQLPowerSerialDataSource *db_ );
    ~KWQTSQLPowerSerialLetterOpen();
private:
 KWQTSQLPowerSerialDataSource *db;
 KWQTSQLOpenWidget *widget;
private slots:
void handleOk();
};


/******************************************************************
 *
 * DIRTY HACK FOR SOME INFLEXIBILITY IN QT3's SQL stuff
 *
 * This class is rom some Trolltech guy on QT-interest
 ******************************************************************/



class QMySqlCursor: public QSqlCursor
{
public:
    QMySqlCursor( const QString & query = QString::null, bool autopopulate = 
TRUE, QSqlDatabase* db = 0 ): QSqlCursor( QString::null, autopopulate, db )
    {
        exec( query );
        if ( autopopulate )
            *(QSqlRecord*)this = ((QSqlQuery*)this)->driver()->record( 
*(QSqlQuery*)this );
        setMode( QSqlCursor::ReadOnly );
    }
    QMySqlCursor( const QMySqlCursor & other ): QSqlCursor( other ) {}
    QMySqlCursor( const QSqlQuery & query, bool autopopulate = TRUE ): 
QSqlCursor( QString::null, autopopulate )
    {
        *(QSqlQuery*)this = query;
        if ( autopopulate )
            *(QSqlRecord*)this = query.driver()->record( query );
        setMode( QSqlCursor::ReadOnly );
    }
    bool select( const QString & /*filter*/, const QSqlIndex & /*sort*/ = 
QSqlIndex() ) { return exec( lastQuery() ); }
    QSqlIndex primaryIndex( bool /*prime*/ = TRUE ) const { return 
QSqlIndex(); }
    int insert( bool /*invalidate*/ = TRUE ) { return FALSE; }
    int update( bool /*invalidate*/ = TRUE ) { return FALSE; }
    int del( bool /*invalidate*/ = TRUE ) { return FALSE; }
    void setName( const QString& /*name*/, bool /*autopopulate*/ = TRUE ) {}
};


#endif
