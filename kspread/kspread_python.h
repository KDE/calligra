/* This file is part of the KDE project
   Copyright (C) 1998, 1999 Torben Weis <weis@kde.org>
 
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

#ifndef __kspread_python_h__
#define __kspread_python_h__

#include <Python.h>
#include <graminit.h>

#include <qstring.h>

class KSpreadTable;
class KSpreadDoc;

// void pythonInit( int argc, char** argv );

// bool pythonEval( const char* _cmd, QString & _result );

class KPythonModule
{
public:
    enum StringModes { PY_EXPRESSION, PY_STATEMENT };
    
    KPythonModule( const char *_name );
    ~KPythonModule() { }
    
    int runCodeStr( StringModes mode, char *code, char *resfmt = NULL, void *cresult = NULL );
    int convertResult( PyObject *presult, char *resFormat, void *resTarget );
    int runFile( const char *_filename );
    void registerMethods( struct PyMethodDef* _methods );

    PyObject *pyModule() { return m_pModule; }
    PyObject *pyDict() { return m_pDict; }
    
protected:
    PyObject *m_pModule;
    PyObject *m_pDict;
    QString m_strName;
};

class KSpreadPythonModule : public KPythonModule
{
public:
  KSpreadPythonModule( const char *_name, int _doc_id );

  // Has to die ....
  bool setContext( int _map_id, int _table_id );
  PyObject* eval( const char* _cmd );

  bool setContext( KSpreadTable* _table );
  bool setContext( KSpreadDoc* _doc );
};

#endif
