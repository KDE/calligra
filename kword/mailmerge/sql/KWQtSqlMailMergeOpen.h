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

#ifndef _SERIALLETTER_QtSql_OPEN_EDITOR_H_
#define _SERIALLETTER_QtSql_OPEN_EDITOR_H_

#include <qdom.h>
#include <kdialogbase.h>
#include <qwidget.h>

#include "KWMailMergeDataSource.h"
#include "KWQtSqlSerialDataSourceBase.h"
#include "qtsqlopenwidget.h"

/******************************************************************
 *
 * Class: KWQtSqlMailMergeOpen
 *
 ******************************************************************/
class KWQtSqlMailMergeOpen : public KDialogBase
{
    Q_OBJECT

public:
    KWQtSqlMailMergeOpen( QWidget *parent, KWQtSqlSerialDataSourceBase *db_ );
    ~KWQtSqlMailMergeOpen();
private:
 KWQtSqlSerialDataSourceBase *db;
 KWQtSqlOpenWidget *widget;

 void fillSavedProperties();

private slots:
void handleOk();
public slots:
void savedPropertiesChanged(const QString&);
void slotSave();
};

#endif

