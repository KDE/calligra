
#include "koFactory.h"
#include "koFilterManager.h"

#include <kinstance.h>

KInstance *KoFactory::s_instance = 0;
unsigned long KoFactory::s_instanceRefCnt = 0;

class KoFactory::KoFactoryPrivate
{
public:
  KoFactoryPrivate()
  {
  }
  ~KoFactoryPrivate()
  {
  }
};

KoFactory::KoFactory( QObject *parent, const char *name )
  : KLibFactory( parent, name )
{
  d = new KoFactoryPrivate;
  s_instanceRefCnt++;
  KoFilterManager::incRef();
}

KoFactory::~KoFactory()
{
  delete d;

  s_instanceRefCnt--;
  if ( s_instanceRefCnt == 0 && s_instance )
  {
    delete s_instance;
    s_instance = 0;
  }

  KoFilterManager::decRef();
}

KInstance *KoFactory::koInstance()
{
  if ( !s_instance )
  {
    if ( s_instanceRefCnt == 0 ) // aha, someone not using KoFactory? tststs ;-)
      s_instanceRefCnt++;
  
    s_instance = new KInstance( "koffice" );
  }

  return s_instance;
}

