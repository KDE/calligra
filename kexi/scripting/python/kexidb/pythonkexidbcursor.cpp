/***************************************************************************
 * pythonkexidbcursor.cpp
 * copyright (C)2003 by Sebastian Sauer (mail@dipe.org)
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 ***************************************************************************/

#include "pythonkexidbcursor.h"
//#include "pythonkexidb.h"
#include "../pythonutils.h"
#include "pythonkexidbconnection.h"

using namespace Kross;

namespace Kross
{
    class PythonKexiDBCursorPrivate
    {
        public:
            PythonKexiDBConnection* connection;
            KexiDB::Cursor* cursor;
    };
}

PythonKexiDBCursor::PythonKexiDBCursor(PythonKexiDBConnection* connection, KexiDB::Cursor* cursor)
{
    d = new PythonKexiDBCursorPrivate();
    d->connection = connection;
    d->cursor = cursor;
}

PythonKexiDBCursor::~PythonKexiDBCursor()
{
    /*FIXME why the hell deleteCursor() leads to crashes???
    KexiDB::Connection* connection = d->cursor->connection();
    if(connection) connection->deleteCursor(d->cursor);

    BT;
    #3  0x418fef00 in KexiDB::SQLiteCursor::drv_appendCurrentRecordToBuffer ()
        from /usr/lib/kde3/kexidb_sqlite3driver.so
    #4  0x40b7b2b8 in KexiDB::Cursor::getNextRecord () from /usr/lib/libkexidb.so.0
    #5  0x40b7a83b in KexiDB::Cursor::open () from /usr/lib/libkexidb.so.0
    #6  0x40b4de9e in KexiDB::Connection::executeQuery ()
        from /usr/lib/libkexidb.so.0
    #7  0x0807a0d9 in Kross::PythonKexiDBConnection::executeQuery (this=0x816c2c0,
        args=@0xbffff390) at kexidb/pythonkexidbconnection.cpp:217
    #8  0x0807a947 in Py::PythonExtension<Kross::PythonKexiDBConnection>::method_varargs_call_handler (_self_and_name_tuple=0x4176f32c, _args=0x4175bc0c)
        at Extensions.hxx:678
    */

    delete d;
}

bool PythonKexiDBCursor::accepts(PyObject* pyobj) const
{
    return pyobj && Py::PythonExtension<PythonKexiDBCursor>::check(pyobj);
}

void PythonKexiDBCursor::init_type(void)
{
    behaviors().name("KexiDBCursor");
    behaviors().doc(
        "The PythonKexiDBCursor object provides access to the "
        "KexiDB::Cursor class.\n"
    );

    add_varargs_method("moveFirst", &PythonKexiDBCursor::moveFirst,
        "boolean moveFirst()\n"
    );
    add_varargs_method("moveLast", &PythonKexiDBCursor::moveLast,
        "boolean moveLast()\n"
    );
    add_varargs_method("moveNext", &PythonKexiDBCursor::moveNext,
        "boolean moveNext()\n"
    );
    add_varargs_method("movePrev", &PythonKexiDBCursor::movePrev,
        "boolean movePrev()\n"
    );
    add_varargs_method("eof", &PythonKexiDBCursor::eof,
        "boolean eof()\n"
    );
    add_varargs_method("bof", &PythonKexiDBCursor::bof,
        "boolean bof()\n"
    );

    add_varargs_method("value", &PythonKexiDBCursor::value,
        "variant value()\n"
    );
    add_varargs_method("at", &PythonKexiDBCursor::at,
        "long at()\n"
    );
    add_varargs_method("fieldCount", &PythonKexiDBCursor::fieldCount,
        "int fieldCount()\n"
    );
}

Py::Object PythonKexiDBCursor::moveFirst(const Py::Tuple& args)
{
    PythonUtils::checkArgs(args, 0, 0);
    return Py::Int( d->cursor->moveFirst() );
}

Py::Object PythonKexiDBCursor::moveLast(const Py::Tuple& args)
{
    PythonUtils::checkArgs(args, 0, 0);
    return Py::Int( d->cursor->moveLast() );
}

Py::Object PythonKexiDBCursor::moveNext(const Py::Tuple& args)
{
    PythonUtils::checkArgs(args, 0, 0);
    return Py::Int( d->cursor->moveNext() );
}

Py::Object PythonKexiDBCursor::movePrev(const Py::Tuple& args)
{
    PythonUtils::checkArgs(args, 0, 0);
    return Py::Int( d->cursor->movePrev() );
}

Py::Object PythonKexiDBCursor::eof(const Py::Tuple& args)
{
    PythonUtils::checkArgs(args, 0, 0);
    return Py::Int( d->cursor->eof() );
}

Py::Object PythonKexiDBCursor::bof(const Py::Tuple& args)
{
    PythonUtils::checkArgs(args, 0, 0);
    return Py::Int( d->cursor->bof() );
}

Py::Object PythonKexiDBCursor::value(const Py::Tuple& args)
{
    PythonUtils::checkArgs(args, 1, 1);
    if(! args[0].isNumeric()) //FIXME: check needed?
        throw Py::TypeError("PythonKexiDBCursor.value(columnnumber) expects a numeric value as single parameter.");
    Py::Int col = args[0];
    return PythonUtils::toPyObject( d->cursor->value(col) );
}

Py::Object PythonKexiDBCursor::at(const Py::Tuple&)
{
    return Py::Long( (long)d->cursor->at() );
}

Py::Object PythonKexiDBCursor::fieldCount(const Py::Tuple&)
{
    return Py::Long( (unsigned long)d->cursor->fieldCount() );
}

