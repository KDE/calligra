/*
* Kexi Report Plugin
* Copyright (C) 2007-2009 by Adam Pigg (adam@piggz.co.uk)
*
* This library is free software; you can redistribute it and/or
* modify it under the terms of the GNU Lesser General Public
* License as published by the Free Software Foundation; either
* version 2.1 of the License, or (at your option) any later version.
*
* This library is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
* Lesser General Public License for more details.
*
* You should have received a copy of the GNU Lesser General Public
* License along with this library; if not, write to the Free Software
* Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
*/

#ifndef __KEXIDBREPORTDATA_H__
#define __KEXIDBREPORTDATA_H__

#include <QString>
#include <QStringList>

#include <kexidb/cursor.h>
#include <kexidb/utils.h>

#include "koreportdata.h"

/**

*/
class KexiDBReportData : public KoReportData {
    private:
        QString         m_qstrQuery;

        KexiDB::Cursor   *m_cursor;

        KexiDB::Connection   *m_connection;
        KexiDB::TableOrQuerySchema *m_schema;

        bool executeInternal();

        bool m_valid;

    public:
        KexiDBReportData ( const QString &, KexiDB::Connection *conn );
        virtual ~KexiDBReportData();

        virtual void* connection() const;
        virtual void* schema() const;

        virtual QStringList fieldNames();
        
        virtual QString source() const;
        virtual unsigned int fieldNumber ( const QString &field );
        virtual QVariant value ( unsigned int );
        virtual QVariant value ( const QString &field );

        virtual bool open();
        virtual bool close();
        virtual bool moveNext();
        virtual bool movePrevious();
        virtual bool moveFirst();
        virtual bool moveLast();

        virtual long at() const;
        virtual long recordCount() const;
};

#endif

