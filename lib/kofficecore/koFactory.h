#ifndef __koFactory_h__
#define __koFactory_h__

#include <klibloader.h>

class KInstance;

class KoFactory : public KLibFactory
{
  Q_OBJECT
public:
  KoFactory( QObject *parent = 0, const char *name = 0 );
  virtual ~KoFactory();

  static KInstance *koInstance();

private:
  class KoFactoryPrivate;
  KoFactoryPrivate *d;
  static KInstance *s_instance;
  static unsigned long s_instanceRefCnt;
};

#endif
