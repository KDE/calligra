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

#ifndef _SERIALLETTER_QtSql_BASE_H_
#define _SERIALLETTER_QtSql_BASE_H_

#include <qdom.h>
#include <kdialogbase.h>
#include <qsqldatabase.h>
#include <QPointer>

#include "KWMailMergeDataSource.h"
#include "qtsqlopenwidget.h"



/******************************************************************
 *
 * Class: KWQtSqlSerialDataSourceBase
 *
 ******************************************************************/
class KWQtSqlSerialDataSourceBase: public KWMailMergeDataSource
{
    Q_OBJECT
    K_DCOP
    public:
    KWQtSqlSerialDataSourceBase(KInstance *inst,QObject *parent);
    ~KWQtSqlSerialDataSourceBase();

   virtual bool showConfigDialog(QWidget *par,int action);

    protected:
	friend class KWQtSqlMailMergeOpen;
	QString hostname;
	QString username;
	QString driver;
	QString port;
	QString databasename;
	QPointer<QSqlDatabase> database;
	QString DataBaseConnection;
        static int connectionId;
   k_dcop:
     bool openDatabase();

};

#endif
