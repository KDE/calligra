#ifndef __kspread_python_h__
#define __kspread_python_h__

#include <Python.h>
#include <graminit.h>

#include <qstring.h>

class KSpreadDoc;

void pythonInit( int argc, char** argv );

bool pythonEval( const char* _cmd, QString & _result );

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

    bool setContext( int _map_id, int _table_id );
    PyObject* eval( const char* _cmd );
};

#endif
