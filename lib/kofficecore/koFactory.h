#ifndef __koFactory_h__
#define __koFactory_h__

#include <kparts/factory.h>

class KInstance;

class KoFactory : public KParts::Factory
{
  Q_OBJECT
public:
  KoFactory( QObject *parent = 0, const char *name = 0 );
  virtual ~KoFactory();

private:
  class KoFactoryPrivate;
  KoFactoryPrivate *d;
};

#endif
