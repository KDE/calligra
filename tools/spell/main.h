#ifndef __main_h__
#define __main_h__

#include <komApplication.h>
#include <komComponent.h>

#include "datatools.h"

#include <kspell.h>
#include <qobject.h>

class SpellChecker : public QObject
{
  Q_OBJECT
public:
  SpellChecker( const char *_buffer, KOM::Base_ptr _sender, const CORBA::Any& _id );

public slots:
  void slotReady();
  void slotDone( char* );
  
protected:
  QString m_strBuffer;
  KSpell* m_pSpell;
  KOM::Base_var m_vSender;
  CORBA::Any m_id;
};

class MyTool : virtual public DataTools::Tool_skel
{
public:
  MyTool( const CORBA::ORB::ObjectTag &_tag );
  MyTool( CORBA::Object_ptr _obj );
  
  virtual void run( const char* _command, KOM::Base_ptr _sender, const CORBA::Any& _value, const CORBA::Any& _id );
  virtual void cancel( CORBA::Long _id );
};

class MyApplication : public KOMApplication
{
public:
  MyApplication( int &argc, char **argv );
  
  void start();
};

#endif
