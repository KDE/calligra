#include <factory.h>

#include <plugin.h>

KInstance* KWordAddressesFactory::s_instance = 0L; 

KWordAddressesFactory::KWordAddressesFactory( QObject* parent, const char* name )
  : KLibFactory( parent, name )
{
  s_instance = new KInstance( "KWordAddressesFactory" );
}

KWordAddressesFactory::~KWordAddressesFactory()
{
  delete s_instance;
}

QObject* KWordAddressesFactory::create( QObject* parent, const char* name, const char*, const QStringList & )
{
  QObject* obj = new PluginKWordAddresses( parent, name );
  emit objectCreated( obj ); 
  return obj;
}

extern "C"
{
  void* init_libkword_adresses()
  {
    return new KWordAddressesFactory;
  }
}
