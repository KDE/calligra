#include "main.h"

#include <iostream>
#include <stdio.h>
#include <assert.h>

KPythonModule* m_pModule = 0L;

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


#include <komAutoLoader.h>
#include <kom.h>

#include <qmsgbox.h>

typedef KOMAutoLoader<Factory> MyAutoLoader;

MyApplication::MyApplication( int &argc, char **argv ) : KoApplication( argc, argv, "kspread_filter")
{
}

void MyApplication::start()
{
}

Factory::Factory( const CORBA::ORB::ObjectTag &_tag ) : KOffice::FilterFactory_skel( _tag )
{
}

Factory::Factory( CORBA::Object_ptr _obj ) : KOffice::FilterFactory_skel( _obj )
{
}

KOffice::Filter_ptr Factory::create( const KOM::Component_ptr _core )
{
  return KOffice::Filter::_duplicate( new Filter );
}



Filter::Filter() : KOMComponent(), KOffice::Filter_skel()
{
}

void Filter::filter( KOffice::Filter::Data& data, const char *_from, const char *_to )
{
  if ( strcmp( _to, "application/x-kspread" ) != 0L )
  {
    KOffice::Filter::UnsupportedFormat exc;
    exc.format = CORBA::string_dup( _to );
    mico_throw( exc );
    return;
  }

  if ( strcmp( _from, "text/plain" ) != 0L )
  {
    KOffice::Filter::UnsupportedFormat exc;
    exc.format = CORBA::string_dup( _from );
    mico_throw( exc );
    return;
  }
  
  CORBA::ULong len = data.length();
  if ( len == 0 )
    return;
  
  char *buffer = new char[ len + 1 ];
  for( CORBA::ULong i = 0; i < len; ++i )
    buffer[i] = (char)data[i];
  buffer[len] = 0;
  
  cerr << "INPUT" << endl << buffer << "-----------------------------" << endl;
  
  PyObject* args = Py_BuildValue( "(s)", buffer );
  assert( args != 0L );
  PyObject* func = PyObject_GetAttrString( m_pModule->pyModule(), "filter" );
  assert( func != 0L );
  PyObject* ret = PyEval_CallObject( func, args );

  if ( ret == 0L )
  {
    KOffice::Filter::FormatError exc;
    mico_throw( exc );
  }
  
  char *str;
  PyArg_Parse( ret, "s", &str );
  
  cerr << "OUTPUT" << endl << str << "----------------------------" << endl;
  
  len = strlen( str );
  
  data.length( len );
  for( CORBA::ULong i = 0; i < len; ++i )
    data[i] = str[i];
  
  Py_DECREF( args );
  Py_DECREF( func );
  Py_DECREF( ret );

  delete buffer;
}


int main( int argc, char **argv )
{
  MyApplication app( argc, argv );
  
  MyAutoLoader loader( "IDL:KOffice/FilterFactory:1.0", "KSpreadFilter" );

  m_pModule = new KPythonModule( "Filter" );
  
  QString code;
  
  QString path = kapp->kde_datadir().copy();
  path += "/koffice/filter/komma.py";
  cerr << "Reading sources from" << path << endl;
  
  assert( m_pModule->runFile( path ) != -1 );

  app.exec();
  
  /*
  const char *infile = argv[ 1 ];
  cerr << "Reading " << infile << endl;
  
  FILE *f = fopen( infile, "r" );
  if ( f == 0L )
    assert( 0 );
  
  QString input;

  char buffer[ 4097 ];
  int n;
  while( ( n = fread( buffer, 1, 4096, f ) ) > 0 )
  {
    buffer[n] = 0;
    input += buffer;
  }
 
  fclose( f );

  KOffice::FilterFactory_var factory = new Factory( CORBA::BOA::ReferenceData() );
  KOffice::Filter_var filter = factory->create( 0L );

  CORBA::ULong len = input.length();
  
  KOffice::Filter::Data data;
  
  data.length( len );
  for( CORBA::ULong i = 0; i < len; ++i )
    data[i] = input[i];
  
    filter->filter( data ); */
}

#include "main.moc"
