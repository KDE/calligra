/***************************************************************************
 * pythonkexidbconnection.h
 * copyright (C)2003 by Sebastian Sauer (mail@dipe.org)
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 ***************************************************************************/

#ifndef KROSS_PYTHONKEXIDBCONNECTION_H
#define KROSS_PYTHONKEXIDBCONNECTION_H

#include <Python.h>

#include <iostream>

#include "../CXX/Objects.hxx"
#include "../CXX/Extensions.hxx"

#include <kexidb/drivermanager.h>
#include <kexidb/cursor.h>

namespace Kross
{

    class PythonKexiDBDriver;
    class PythonKexiDBConnectionData;
    class PythonKexiDBConnectionPrivate;

    /**
     * The PythonKexiDBConnection class is a from Py::Object inherited
     * object to represent the KexiDB::Connection class in python.
     */
    class PythonKexiDBConnection : public Py::PythonExtension<PythonKexiDBConnection>
    {
        public:
            PythonKexiDBConnection(PythonKexiDBDriver*, PythonKexiDBConnectionData*, KexiDB::Connection*);
            virtual ~PythonKexiDBConnection();

            virtual bool accepts(PyObject* pyobj) const;
            static void init_type(void);

        private:
            PythonKexiDBConnectionPrivate* d;

            Py::Object data(const Py::Tuple&);
            Py::Object driver(const Py::Tuple&);

            Py::Object connect(const Py::Tuple&);
            Py::Object isConnected(const Py::Tuple&);
            Py::Object disconnect(const Py::Tuple&);

            Py::Object isDatabase(const Py::Tuple&);
            Py::Object currentDatabase(const Py::Tuple&);
            Py::Object databaseNames(const Py::Tuple&);
            Py::Object isDatabaseUsed(const Py::Tuple&);
            Py::Object useDatabase(const Py::Tuple&);
            Py::Object closeDatabase(const Py::Tuple&);

            //bool insertRecord(TableSchema &tableSchema, QValueList<QVariant>& values);
            //bool insertRecord(FieldList& fields, QValueList<QVariant>& values);
            Py::Object insertRecord(const Py::Tuple&);

            //bool createDatabase( const QString &dbName );
            //bool dropDatabase( const QString &dbName = QString::null );
            //bool createTable( TableSchema* tableSchema, bool replaceExisting = false );
            //tristate dropTable( TableSchema* tableSchema );
            //tristate dropTable( const QString& table );
            //tristate alterTable( TableSchema& tableSchema, TableSchema& newTableSchema);
            //bool alterTableName(TableSchema& tableSchema, const QString& newName, bool replace = false);
            //bool executeSQL( const QString& statement );
            //QString selectStatement( QuerySchema& querySchema, int idEscaping = Driver::EscapeDriver|Driver::EscapeAsNecessary ) const;

            Py::Object tableNames(const Py::Tuple&);

            Py::Object executeQuery(const Py::Tuple&);
    };

}

#endif

