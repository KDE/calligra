#ifndef __main_h__
#define __main_h__

#include <kom.h>
#include <komComponent.h>
#include <koffice.h>
#include <koApplication.h>

#include <Python.h>
#include <graminit.h>

#include <qstring.h>

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

class MyApplication : public KoApplication
{
  Q_OBJECT
public:
  MyApplication( int &argc, char **argv );
  
  void start();
};

class Factory : virtual public KOffice::FilterFactory_skel
{
public:
  Factory( const CORBA::ORB::ObjectTag &_tag );
  Factory( CORBA::Object_ptr _obj );

  KOffice::Filter_ptr create( const KOM::Component_ptr _comp );
};

class Filter : virtual public KOMComponent,
	       virtual public KOffice::Filter_skel
{
public:
  Filter();
  
  void filter( KOffice::Filter::Data& data, const char *_from, const char *_to );
};

#endif
