#include "kspread_python.h"

#include <assert.h>

PyObject *pyDict;

KPythonModule::KPythonModule( const char *_name )
{
  m_strName = _name;
    
  Py_Initialize();
  m_pModule = PyImport_AddModule( (char*)_name );
  assert( m_pModule );
  
  m_pDict = PyModule_GetDict( m_pModule );
  PyDict_SetItemString( m_pDict, "__dummy__", Py_None );
  PyDict_SetItemString( m_pDict, "__builtins__", PyEval_GetBuiltins() );
}

int KPythonModule::runCodeStr( StringModes mode, char *code, char *resfmt, void *cresult )
{
    PyObject *presult = PyRun_String( code, ( mode == PY_EXPRESSION ? eval_input : file_input ),
				      m_pDict, m_pDict );
    
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

KSpreadPythonModule::KSpreadPythonModule( const char *_name ) : KPythonModule( _name )
{
}

bool KSpreadPythonModule::eval( const char* _cmd, QString & _result )
{
    printf("Running '%s'\n",_cmd);
    
    PyObject *v = PyRun_String( (char*)_cmd, eval_input, m_pDict, m_pDict );
    if ( v == 0L )
    {
	printf("ERROR: Python: Could not exec\n");

	PyObject *e = PyErr_Occurred();
	if ( e == NULL )
	    return FALSE;

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
	
	PyErr_Clear();
	
	return FALSE;
    }

    double res;
    if ( !PyArg_Parse( v, "d", &res ) )
    {
	char *str;
	if ( !PyArg_Parse( v, "s", &str ) )
	{
	    printf(" Could not parse\n");
	    return FALSE;
	}
	_result = str;
	_result.detach();
    }
    else
    {
	char buffer[ 1024 ];
	sprintf( buffer, "%f", res );
	_result = buffer;
	_result.detach();
    }
    
    Py_DECREF( v );

    printf("RESULT is '%s'\n",_result.data());
    
    return TRUE;
}

void pythonInit( int argc, char** argv )
{
    /* Initialize the Python interpreter.  Required. */
    Py_Initialize();

    /* Define sys.argv.  It is up to the application if you
       want this; you can also let it undefined (since the Python 
       code is generally not a main program it has no business
       touching sys.argv...) */
    PySys_SetArgv(argc, argv);

    PyObject *m = PyImport_AddModule("__main__");
    if ( m == NULL )
    {
	printf("ERROR: Python: Could not import __main__\n");
	exit(1);
    }
    pyDict = PyModule_GetDict( m );

    /* Execute some Python statements (in module __main__) */
    PyRun_SimpleString("import sys\n");
    PyRun_SimpleString("from xcllib import *\n");
    PyRun_SimpleString("import xcl\n");
    PyRun_SimpleString("print sys.builtin_module_names\n");
    // PyRun_SimpleString("print sys.argv\n");

    // PyObject *m2 = Py_InitModule("xcl", xcl_methods);

    /* PyObject *m2 = PyImport_AddModule("xcl");
    if ( m2 == NULL )
    {
	printf("ERROR: Python: Could not import xcl\n");
	exit(1);
    } 

    pyKSpreadDict = PyModule_GetDict( m2 ); */

    /* if ( PyRun_SimpleFile( 0L, "xcl.py" ) != 0 )
    {
	printf("Could not exec xcl.py\n");
	exit(1);
    } */

    /* QString erg;
    if ( !pythonEval( "import xcl", erg ) )
    {
	printf("Could not exec xcl.py\n");
	exit(1);
    } */
}

void initqt()
{
}

bool pythonEval( const char* _cmd, QString & _result )
{
    PyObject *v = PyRun_String( (char*)_cmd, eval_input ,pyDict, pyDict );
    if ( v == 0L )
    {
	printf("ERROR: Python: Could not exec\n");

	PyObject *e = PyErr_Occurred();
	if ( e == NULL )
	    return FALSE;

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
	
	PyErr_Clear();
	
	return FALSE;
    }

    /*    s = PyObject_Str( v );
    if ( s == NULL )
    {
	printf("Could not convert to string\n");
	exit(1);
    } */
    
    /* char *str;
    if ( !PyArg_Parse( v, "s", &str ) )
    {
	printf(" Could not parse\n");
	return FALSE;
    }
    printf("=%s\n",str); */

    /* int res;
    if ( !PyArg_Parse( v, "i", &res ) )
    {
	printf(" Could not parse int\n");
	return 1;
    }
    printf("=%i\n",res); */

    double res;
    if ( !PyArg_Parse( v, "d", &res ) )
    {
	char *str;
	if ( !PyArg_Parse( v, "s", &str ) )
	{
	    printf(" Could not parse\n");
	    return FALSE;
	}
	_result = str;
	_result.detach();
    }
    else
    {
	char buffer[ 1024 ];
	sprintf( buffer, "%f", res );
	_result = buffer;
	_result.detach();
    }
    
    Py_DECREF( v );
    
    return TRUE;
}
