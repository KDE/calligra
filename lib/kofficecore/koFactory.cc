
#include "koFactory.h"
#include "koFilterManager.h"

#include <kinstance.h>

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
  : KParts::Factory( parent, name )
{
  //d = new KoFactoryPrivate;
  KoFilterManager::incRef();
}

KoFactory::~KoFactory()
{
  //delete d;
  KoFilterManager::decRef();
}

