#include <qprinter.h>
#include "kspread_table.h"
#include "kspread_doc.h"
#include "kspread_map.h"
#include "kspread_python.h"

#include <assert.h>
#include <iostream>

#include <qstring.h>
#include <kapp.h>

KPythonModule::KPythonModule( const char *_name )
{
  m_strName = _name;
    
  Py_Initialize();
  m_pModule = PyImport_AddModule( (char*)_name );
  assert( m_pModule );
  
  m_pDict = PyModule_GetDict( m_pModule );
  assert( m_pDict );
  
  PyDict_SetItemString( m_pDict, "__dummy__", Py_None );
  PyDict_SetItemString( m_pDict, "__builtins__", PyEval_GetBuiltins() );

  QString tmp = "import sys\n";
  runCodeStr( PY_STATEMENT, (char*)tmp.data() );
  tmp += "sys.path = [ \"";
  tmp += kapp->kde_bindir().data();
  tmp += "/../include/python\", \"";
  tmp += kapp->kde_bindir().data();
  tmp += "/../lib\", ] + sys.path\n";
  runCodeStr( PY_STATEMENT, (char*)tmp.data() );
  tmp = "from KSpread import *\n";
  runCodeStr( PY_STATEMENT, (char*)tmp.data() );
}

int KPythonModule::runCodeStr( StringModes mode, char *code, char *resfmt, void *cresult )
{
    PyObject *presult = PyRun_String( code, ( mode == PY_EXPRESSION ? eval_input : file_input ),
				      m_pDict, m_pDict );
    if ( presult == 0L )
	  PyErr_Print();
   
    if ( mode == PY_STATEMENT )
    {
	int result = ( presult == NULL ? -1 : 0 );
	Py_XDECREF( presult );
	return result;
    }
    return convertResult( presult, resfmt, cresult );
}

int KPythonModule::convertResult( PyObject *presult, char *resFormat, void *resTarget )
{
    if ( presult == NULL )
	return -1;
    if ( resTarget == NULL )
    {
	Py_DECREF( presult );
	return 0;
    }
    if ( !PyArg_Parse( presult, resFormat, resTarget ) )
    {
	Py_DECREF( presult );
	return -1;
    }
    if ( strcmp( resFormat, "0" ) != 0 )
	Py_DECREF( presult );
    return 0;
}

int KPythonModule::runFile( const char *_filename )
{
    FILE *f = fopen( _filename, "r" );
    if ( !f )
	return -1;

    QString script;
    char buffer[ 4096 ];
    while ( !feof( f ) )
    {
	int n = fread( buffer, 1, 4095, f );
	if ( n > 0 )
	{
	    buffer[n] = 0;
	    script += buffer;
	}
    }
    fclose( f );

    return runCodeStr( PY_STATEMENT, script.data() );
}

void KPythonModule::registerMethods( struct PyMethodDef* _methods )
{
    Py_InitModule( (char*)(m_strName.data()), _methods );
}

KSpreadPythonModule::KSpreadPythonModule( const char *_name, int _doc_id ) : KPythonModule( _name )
{
  PyObject* o = Py_BuildValue( "i", _doc_id );
  PyObject_SetAttrString( m_pModule, "_document_id", o );
  Py_DECREF( o );
}

bool KSpreadPythonModule::setContext( KSpreadDoc* _doc )
{
  PyObject* obj = Py_BuildValue( "s", "" );
  PyObject_SetAttrString( m_pModule, "tableIOR", obj );
  Py_DECREF( obj );

  obj = Py_BuildValue( "s", "" );
  PyObject_SetAttrString( m_pModule, "bookIOR", obj );
  Py_DECREF( obj );

  CORBA::String_var str = opapp_orb->object_to_string( _doc );
  obj = Py_BuildValue( "s", str.in() );
  PyObject_SetAttrString( m_pModule, "docIOR", obj );
  Py_DECREF( obj );

  return true;
}

bool KSpreadPythonModule::setContext( KSpreadTable* _table )
{
  CORBA::String_var str = opapp_orb->object_to_string( _table );
  PyObject* obj = Py_BuildValue( "s", str.in() );
  PyObject_SetAttrString( m_pModule, "tableIOR", obj );
  Py_DECREF( obj );

  str = opapp_orb->object_to_string( _table->map() );
  obj = Py_BuildValue( "s", str.in() );
  PyObject_SetAttrString( m_pModule, "bookIOR", obj );
  Py_DECREF( obj );

  str = opapp_orb->object_to_string( _table->doc() );
  obj = Py_BuildValue( "s", str.in() );
  PyObject_SetAttrString( m_pModule, "docIOR", obj );
  Py_DECREF( obj );

  return true;
}

bool KSpreadPythonModule::setContext( int _map_id, int _table_id )
{
  QString buffer;
  buffer.sprintf( "Workbooks(%i).Worksheets", _map_id );  
  PyObject *obj;
  obj = eval( buffer );
  if ( !obj )
  {
    cerr << "ERROR in python stuff 1" << endl;
    return false;
  }
  PyObject_SetAttrString( m_pModule, "Worksheets", obj );
  Py_DECREF( obj );

  buffer.sprintf( "Worksheets(%i).Range", _table_id );  
  obj = eval( buffer );
  if ( !obj )
  {
    cerr << "ERROR in python stuff 2" << endl;
    return false;
  }
  PyObject_SetAttrString( m_pModule, "Range", obj );
  Py_DECREF( obj );

  return true;
}

PyObject* KSpreadPythonModule::eval( const char* _cmd )
{
    printf("Running '%s'\n",_cmd);
    
    PyObject *v = PyRun_String( (char*)_cmd, eval_input, m_pDict, m_pDict );
    if ( v == 0L )
    {
	printf("ERROR: Python: Could not exec\n");

	PyObject *e = PyErr_Occurred();
	if ( e == NULL )
	    return 0L;

	PyObject *e1, *e2, *e3;
	PyErr_Fetch( &e1, &e2, &e3 );
	
	char *str1 = "";
	if ( e1 != 0L )
	    PyArg_Parse( e1, "s", &str1 );
	
	char *str2 = "";
	if ( e2 != 0L )
	    PyArg_Parse( e2, "s", &str2 );

	char *str3 = "";
	if ( e2 != 0L )
	    PyArg_Parse( e3, "s", &str3 );

	printf("Traceback:\n%s\n%s\n%s\n",str1,str2,str3);

	PyErr_Print();
	PyErr_Clear();
	
	return 0L;
    }
    
    return v;
}



